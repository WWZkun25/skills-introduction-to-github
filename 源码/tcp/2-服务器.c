// 服务器端代码 (4.c)
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>
#include <dirent.h>

#define SERVER_IP   "192.168.2.135"
#define SERVER_PORT 60000
#define FILE_DIR "/qaq/tcp/"
#define MAX_PATH_LENGTH 2048

// 定义操作类型
#define OP_SEND_FILE 0
#define OP_DELETE_FILE 1
#define OP_LIST_FILES 2

// 服务器配置结构
typedef struct {
    const char *ip;
    int port;
    const char *file_dir;
    int backlog;
} ServerConfig;

// 客户端信息结构
typedef struct {
    int socket_client;
    struct sockaddr_in client_addr;
} ClientInfo;

// 内部函数声明
static void create_directory(const char *path);
static int handle_file_receive(int socket_client);
static int handle_file_delete(int socket_client);
static int handle_file_list(int socket_client);
static void *handle_client(void *arg);
static int init_server(int *socket_fd, const ServerConfig *config); // 更新声明
static void cleanup_client(int socket_client, const char *message, int client_fd);

// 清理客户端资源
static void cleanup_client(int socket_client, const char *message, int client_fd) {
    if (message) {
        printf("%s\n", message);
    }
    if (client_fd >= 0) {
        close(client_fd);
    }
    if (socket_client >= 0) {
        close(socket_client);
    }
}

// 创建目录（如果不存在）
static void create_directory(const char *path) {
    struct stat st = {0};
    if (stat(path, &st) == -1) {
        if (mkdir(path, 0777) == -1) {
            perror("mkdir");
        } else {
            printf("目录创建成功: %s\n", path);
        }
    }
}

// 处理文件接收
static int handle_file_receive(int socket_client) {
    char file_name[1024] = {0};
    if (recv(socket_client, file_name, sizeof(file_name), 0) <= 0) {
        return -1;
    }
    printf("接收文件名: %s\n", file_name);

    char full_path[MAX_PATH_LENGTH];
    snprintf(full_path, sizeof(full_path), "%s/%s", FILE_DIR, file_name);

    int file_size;
    if (recv(socket_client, &file_size, sizeof(int), 0) <= 0) {
        return -1;
    }

    if (file_size == -1) {
        return -1;
    }

    int fd = open(full_path, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd < 0) {
        perror("创建文件失败");
        int error_code = -1;
        send(socket_client, &error_code, sizeof(int), 0);
        return -1;
    }

    char buf[1024];
    int total_bytes = 0;
    
    while (total_bytes < file_size) {
        int bytes_to_read = (file_size - total_bytes) < sizeof(buf) ? 
                           (file_size - total_bytes) : sizeof(buf);
        int bytes_received = recv(socket_client, buf, bytes_to_read, 0);
        if (bytes_received <= 0) {
            cleanup_client(socket_client, "接收文件内容失败", fd);
            return -1;
        }
            
        write(fd, buf, bytes_received);
        total_bytes += bytes_received;
    }

    close(fd);
    printf("文件接收完成: %s (%d 字节)\n", file_name, total_bytes);
    return 0;
}

// 处理文件删除
static int handle_file_delete(int socket_client) {
    char file_name[1024] = {0};
    if (recv(socket_client, file_name, sizeof(file_name), 0) <= 0) {
        return -1;
    }
    printf("请求删除文件: %s\n", file_name);

    char full_path[MAX_PATH_LENGTH];
    snprintf(full_path, sizeof(full_path), "%s/%s", FILE_DIR, file_name);

    int result = unlink(full_path);
    send(socket_client, &result, sizeof(int), 0);
    printf("%s: %s\n", result ? "文件删除失败" : "文件删除成功", file_name);
    return result;
}

// 处理文件列表请求
static int handle_file_list(int socket_client) {
    printf("客户端请求文件列表\n");
    
    DIR *dir = opendir(FILE_DIR);
    if (!dir) {
        perror("打开目录失败");
        const char *error_msg = "无法获取文件列表";
        send(socket_client, error_msg, strlen(error_msg), 0);
        return -1;
    }

    char list_buf[4096] = {0};
    int count = 0;
    
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG && entry->d_name[0] != '.') {
            char file_info[256];
            snprintf(file_info, sizeof(file_info), "%-30s\n", entry->d_name);
            if (strlen(list_buf) + strlen(file_info) >= sizeof(list_buf) - 1) {
                break;
            }
            strcat(list_buf, file_info);
            count++;
        }
    }
    
    closedir(dir);
    send(socket_client, list_buf, strlen(list_buf), 0);
    printf("已发送文件列表 (%d 个文件)\n", count);
    return 0;
}

// 客户端处理线程函数
static void *handle_client(void *arg) {
    ClientInfo *cli_info = (ClientInfo *)arg;
    int socket_client = cli_info->socket_client;
    char *ip = inet_ntoa(cli_info->client_addr.sin_addr);
    int port = ntohs(cli_info->client_addr.sin_port);
    printf("客户端 [%s:%d] 已连接\n", ip, port);

    create_directory(FILE_DIR);

    while (1) {
        int op;
        if (recv(socket_client, &op, sizeof(int), 0) <= 0) {
            break;
        }

        switch (op) {
            case OP_SEND_FILE:
                handle_file_receive(socket_client);
                break;
            case OP_DELETE_FILE:
                handle_file_delete(socket_client);
                break;
            case OP_LIST_FILES:
                handle_file_list(socket_client);
                break;
            default:
                printf("未知操作类型: %d\n", op);
                break;
        }
    }

    close(socket_client);
    free(cli_info);  // 释放动态分配的内存
    return NULL;
}

// 初始化服务器
static int init_server(int *socket_fd, const ServerConfig *config) {
    *socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (*socket_fd < 0) {
        perror("socket");
        return -1;
    }

    int optval = 1;
    if (setsockopt(*socket_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
        perror("setsockopt");
        close(*socket_fd);
        return -1;
    }

    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(config->port);
    server_addr.sin_addr.s_addr = inet_addr(config->ip);

    if (bind(*socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(*socket_fd);
        return -1;
    }

    if (listen(*socket_fd, config->backlog) < 0) {
        perror("listen");
        close(*socket_fd);
        return -1;
    }

    printf("服务器绑定成功 [%s:%d]\n", config->ip, config->port);
    return 0;
}

// 运行服务器主循环
static void run_server_loop(int socket_fd, const ServerConfig *config) {
    printf("服务器已启动，等待连接...\n");

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t len = sizeof(client_addr);
        int socket_client = accept(socket_fd, (struct sockaddr *)&client_addr, &len);
        
        if (socket_client < 0) {
            perror("accept");
            continue;
        }

        // 动态分配客户端信息结构
        ClientInfo *cli_info = malloc(sizeof(ClientInfo));
        if (!cli_info) {
            perror("malloc");
            close(socket_client);
            continue;
        }
        
        cli_info->socket_client = socket_client;
        cli_info->client_addr = client_addr;

        pthread_t tid;
        if (pthread_create(&tid, NULL, handle_client, cli_info) != 0) {
            perror("pthread_create");
            cleanup_client(socket_client, NULL, -1);
            free(cli_info);
        } else {
            pthread_detach(tid);  // 分离线程，自动回收资源
        }
    }
}

// 服务器主函数
static int start_file_server(const ServerConfig *config) {
    int socket_fd;
    
    if (init_server(&socket_fd, config) != 0) {
        return -1;
    }

    run_server_loop(socket_fd, config);
    
    // 不会执行到这里，添加关闭以保持代码完整性
    close(socket_fd);
    return 0;
}

int main() {
    const ServerConfig config = {
        .ip = SERVER_IP,
        .port = SERVER_PORT,
        .file_dir = FILE_DIR,
        .backlog = 20
    };
    
    return start_file_server(&config);
}