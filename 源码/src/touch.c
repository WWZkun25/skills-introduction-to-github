#include "../include/touch.h"

int fd_ev = -1;

//初始化触摸屏
void touch_init()
{
	fd_ev = open("/dev/input/event0", O_RDONLY);
	if (fd_ev == -1)
	{
		printf("open ev error\n");
		exit(1);
	}
}

//解除触摸屏初始化
void touch_uninit()
{
	close(fd_ev);
}

//获取触摸点坐标 x表示触摸点的横坐标 y表示触摸点的纵坐标
void get_xy(int* x, int* y)
{
	int flag_x = -1;
	int flag_y = -1; //两个标志位, 为-1表示还没获取到坐标

	//定义一个结构体
	struct input_event data;
	while (1)
	{
		printf("进入这个循环了\n");
		int ret = read(fd_ev, &data, sizeof(data));
		printf("读到数据了\n");
		if (ret == -1)
		{
			printf("read data error\n");
			exit(1);
		}

		if (data.type == EV_ABS && data.code == ABS_X) //值为触摸点的横坐标
		{
			*x = data.value;
			flag_x = 1; //改变标志位
			printf("x表示触摸点的横坐标:%d",*x);
		}

		if (data.type == EV_ABS && data.code == ABS_Y)//值为触摸点的纵坐标
		{
			*y = data.value;
			flag_y = 1; 
		}

		if (flag_x == 1  &&  flag_y == 1) //结束条件
		{
			return;
		}
	}

}

//获取手指离开开发板时的坐标
void get_xy1(int* x, int* y)
{
	//定义一个结构体
	struct input_event data;
	while (1)
	{
		//printf("进入get_xy1循环了\n");
		int ret = read(fd_ev, &data, sizeof(data));
		//printf("获取到屏幕信息了\n");
		if (ret == -1)
		{
			printf("read data error\n");
			exit(1);
		}

		if (data.type == EV_ABS && data.code == ABS_X) //值为触摸点的横坐标
		{
			*x = data.value;
		}

		if (data.type == EV_ABS && data.code == ABS_Y)//值为触摸点的纵坐标
		{
			*y = data.value; 
		}

		if (data.type == EV_KEY && data.code == BIN_TOUCH && data.value == 0) //结束条件,手指抬起来的时候
		{
			return;
		}
	}

}













