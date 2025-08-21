// 客户端代码 (1.c)
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>

#define SERVER_IP   "192.168.2.135"
#define SERVER_PORT 60000

// 定义操作类型
#define OP_SEND_FILE 0
#define OP_DELETE_FILE 1
#define OP_LIST_FILES 2

int main()
{
    int ret = 0;
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0)
    {
        perror("socket fail");
        return -1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    ret = connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in));
    if (ret < 0)
    {
        perror("connect fail");
        close(socket_fd);
        return -1;
    }
    printf("connect succcess [%s][%d]\n", SERVER_IP, SERVER_PORT);

    char file_name[1024];
    int op;
    while (1)
    {
        // 清空输入缓冲区
        while (getchar() != '\n');
        
        printf("\n请选择操作：\n");
        printf("0: 发送文件\n");
        printf("1: 删除文件\n");
        printf("2: 查看文件列表\n");
        printf("3: 退出\n");
        printf("请输入选项 (0-3): ");
        
        if (scanf("%d", &op) != 1) {
            printf("输入无效，请重新输入！\n");
            while (getchar() != '\n'); // 清除无效输入
            continue;
        }

        if (op == 3)
        {
            break;
        }

        if (op < 0 || op > 2) {
            printf("无效的选项，请重新输入！\n");
            continue;
        }

        // 发送操作类型
        ret = send(socket_fd, &op, sizeof(int), 0);
        if (ret < 0)
        {
            perror("发送操作类型失败");
            continue;
        }

        switch (op)
        {
            case OP_SEND_FILE:
                printf("请输入要发送的文件名: ");
                while (getchar() != '\n'); // 清除换行符
                if (fgets(file_name, sizeof(file_name), stdin) == NULL) {
                    continue;
                }
                file_name[strcspn(file_name, "\n")] = 0;

                // 发送文件名
                ret = send(socket_fd, file_name, strlen(file_name), 0);
                if (ret < 0)
                {
                    perror("发送文件名失败");
                    continue;
                }
                printf("发送文件名成功\n");

                // 检查文件是否存在
                int fd = open(file_name, O_RDONLY);
                if (fd < 0)
                {
                    perror("打开文件失败");
                    int file_size = -1;
                    send(socket_fd, &file_size, sizeof(int), 0); // 发送错误信号
                    continue;
                }

                // 发送文件大小
                int file_size = lseek(fd, 0, SEEK_END);
                lseek(fd, 0, SEEK_SET);
                ret = send(socket_fd, &file_size, sizeof(int), 0);
                if (ret < 0)
                {
                    perror("发送文件大小失败");
                    close(fd);
                    continue;
                }
                printf("发送文件大小成功: %d 字节\n", file_size);

                // 发送文件内容
                char buf[1024];
                int bytes_read;
                while ((bytes_read = read(fd, buf, sizeof(buf))) > 0)
                {
                    ret = send(socket_fd, buf, bytes_read, 0);
                    if (ret < 0)
                    {
                        perror("发送文件内容失败");
                        break;
                    }
                }
                close(fd);
                printf("文件发送完成\n");
                break;

            case OP_DELETE_FILE:
                printf("请输入要删除的文件名: ");
                while (getchar() != '\n'); // 清除换行符
                if (fgets(file_name, sizeof(file_name), stdin) == NULL) {
                    continue;
                }
                file_name[strcspn(file_name, "\n")] = 0;

                // 发送文件名
                ret = send(socket_fd, file_name, strlen(file_name), 0);
                if (ret < 0)
                {
                    perror("发送文件名失败");
                    continue;
                }

                // 接收删除结果
                int result;
                ret = recv(socket_fd, &result, sizeof(int), 0);
                if (ret < 0)
                {
                    perror("接收删除结果失败");
                    continue;
                }

                if (result == 0)
                    printf("文件删除成功\n");
                else
                    printf("文件删除失败 (可能不存在或权限不足)\n");
                break;

            case OP_LIST_FILES:
                printf("正在获取文件列表...\n");
                
                // 接收文件列表
                char list_buf[4096] = {0};
                ret = recv(socket_fd, list_buf, sizeof(list_buf), 0);
                if (ret < 0)
                {
                    perror("接收文件列表失败");
                    continue;
                }

                if (strlen(list_buf) == 0)
                    printf("服务器目录为空\n");
                else
                    printf("服务器文件列表:\n%s\n", list_buf);
                break;
        }
    }

    close(socket_fd);
    return 0;
}