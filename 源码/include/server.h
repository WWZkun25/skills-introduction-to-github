#ifndef SERVER_H
#define SERVER_H

// 服务器配置结构
typedef struct {
    const char *ip;
    int port;
    const char *file_dir;
    int backlog;
} ServerConfig;

// 启动文件服务器
int start_file_server(const ServerConfig *config);
//extern void* start_file_server_wrapper(void* arg);

#endif