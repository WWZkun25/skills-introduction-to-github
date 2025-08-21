#include "../include/list.h"

// 初始化图片链表头节点
SIM *head_SIM()
{
    SIM *head = malloc(sizeof(SIM));
    if (!head)
    {
        perror("分配头节点内存失败！");
        return NULL;
    }

    strcpy(head->image_name, "");
    head->prev = head;
    head->next = head;

    return head;
}

// 创建图片链表新节点
SIM *node_SIM(char *name)
{
    if (name == NULL)
    {
        printf("Error: NULL name in node_SIM()\n");
        return NULL;
    }

    SIM *node = malloc(sizeof(SIM));
    if (!node)
    {
        perror("分配节点内存失败！");
        return NULL;
    }

    strncpy(node->image_name, name, 19);
    node->prev = NULL;
    node->next = NULL;

    return node;
}

// 万能插
void insert(SIM *prev_node, SIM *node, SIM *next_node)
{
    node->prev = prev_node;
    node->next = next_node;
    next_node->prev = node;
    prev_node->next = node;
}

// 头插节点
void insert_head(SIM *head, SIM *node)
{
    if (head == NULL || node == NULL)
    {
        printf("Error: NULL pointer in insert_head()\n");
        return;
    }
    insert(head, node, head->next);
}

// 尾插节点
void insert_tail(SIM *head, SIM *node)
{
    if (head == NULL || node == NULL)
    {
        printf("Error: NULL pointer in insert_tail()\n");
        return;
    }
    insert(head->prev, node, head);
}

// 释放图片信息链表
void release_SIM(SIM *head)
{
    SIM *p = head->next;

    while (p != head)
    {
        SIM *tmp = p;
        p = p->next;
        free(tmp);
    }

    free(head);
    printf("链表释放成功！\n");
}


// 打开目录并查找照片
void open_dir(char *path, SIM *head)
{
    // 打开目录
    DIR *dp = opendir(path);
    if (dp == NULL)
    {
        printf("opendir %s fail\n", path);
        return;
    }

    struct dirent *ep = NULL;
    SIM *node = NULL;
    
    // 计算路径长度，用于构造完整路径
    size_t path_len = strlen(path);
    
    while (1)
    {
        // 读目录
        ep = readdir(dp);
        if (ep == NULL) // 目录读取结束
        {
            break;
        }

        if (ep->d_name[0] == '.')
            continue;

        // 构造完整的文件路径
        char full_path[1024]; // 假设最大路径长度为1024
        snprintf(full_path, sizeof(full_path), "%s/%s", path, ep->d_name);
        
        printf("完整路径: %s\n", full_path);
        
        // 创建节点并插入链表
        node = node_SIM(full_path);
        if (node) {
            insert_tail(head, node);
            printf("insert ok! 文件: %s\n", ep->d_name);
        } else {
            printf("创建节点失败: %s\n", full_path);
        }
    }
    
    // 关闭目录
    closedir(dp);
}

