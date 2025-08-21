#ifndef __TOUCH_H__
#define __TOUCH_H__


#include "bmp.h"
#include <linux/input.h>

#define BIN_TOUCH 0x14a


//初始化触摸屏
void touch_init();
//解除触摸屏初始化
void touch_uninit();
//获取触摸点坐标 x表示触摸点的横坐标 y表示触摸点的纵坐标
void get_xy(int* x, int* y);
//获取手指离开开发板时的坐标
void get_xy1(int* x, int* y);


#endif 



