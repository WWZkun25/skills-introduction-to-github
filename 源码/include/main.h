#ifndef _MAIN_H_
#define _MAINL_H_

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "../include/list.h"
#include "../include/server.h"
#include "../include/bmp.h"
#include "../include/touch.h"

/* 常量定义 */
#define SERVER_IP   "192.168.2.135"
#define SERVER_PORT 60000
#define FILE_DIR "/qaq/tcp/"

/* 全局变量声明 */
extern int j[10];          // 用来标识楼层是否被按下
extern int nowf;           // 当前楼层
extern int tf;             // 目标楼层
extern int h_num;          // 当前楼层上面有几个楼层被按下
extern int l_num;          // 当前楼层下面有几个楼层被按下
extern int x7, y7;         // 计算后的楼层对应结果
extern int x, y;           // 触摸点坐标
extern int *addr;          // 内存映射地址

/* 图片路径声明 */
extern char *onbmp[10];    // 楼层按键亮起图片路径
extern char *keybmp[10];   // 楼层按键原始图片路径
extern char *numbmp[10];   // 数字显示图片路径

/* 函数声明 */
void analyz(int n);//楼层对应图片位置
void fenxi();//楼层按键情况
void showbmp();//显示图片
int creat_pthread();//创建线程
void elevator_operation();//电梯操作
void* ad(void* arg);//广告
void* run(void* arg);//电梯
void* server_create(void* arg);//服务器

#endif 