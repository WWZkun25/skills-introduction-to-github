#ifndef __BMP_H
#define __BMP_H

#include <stdio.h>
#include <stdlib.h>
//open
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
//mmap
#include <sys/mman.h>
//close,read,write
#include <unistd.h>

//定义一个bmp图片信息相关的结构体类型
typedef struct BMP
{
    char* bmpname;          //保存bmp图片的名称
    int size;               //文件的总大小
    int width;              //文件的宽
    int height;             //文件的高
    short bpp;              //色深
    char* data;             //像素数组的首地址
}BMP;

//屏幕初始化
void lcd_init();
//解除屏幕初始化
void lcd_uninit();
//画点
void lcd_point(int x,int y,int color);
//画矩形
void lcd_draw_rect(int w, int h, int x, int y, int color);
//画圆
void lcd_draw_circle(int x, int y, int r, int color);
//获取bmp图片信息
BMP get_bmp_info(char* bmpname);
//画一张bmp图片
void lcd_draw_bmp(char* bmpname, int x, int y);
extern int show_bmp_size(int *addr,char *bmp_path,int offset_x,int offset_y );


#endif
