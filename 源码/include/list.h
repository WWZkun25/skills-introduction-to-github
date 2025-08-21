#ifndef __lsit_H__
#define __list_H__

#include <dirent.h>
#include <fcntl.h>
#include <linux/input.h>
#include <pthread.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

typedef struct image
{
    char image_name[20];
    struct image *prev;
    struct image *next;
} SIM;

// 图片链表相关函数
// 初始化头结点
extern SIM *head_SIM();
// 创建图片链表新节点
extern SIM *node_SIM(char *name);
// 万能插
extern void insert(SIM *prev_node, SIM *node, SIM *next_node);
// 头插节点
extern void insert_head(SIM *head, SIM *node);
// 尾插节点
extern void insert_tail(SIM *head, SIM *node);
// 释放图片信息链表
extern void release_SIM(SIM *head);
// 打开/images文件夹并将图片插入到链表中
extern void open_dir(char *path, SIM *head);


#endif