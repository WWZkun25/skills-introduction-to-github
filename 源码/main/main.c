#include "../include/main.h"

/**
 * @brief 主函数：电梯控制系统入口
 * @return 程序退出状态
 */
int main() {
	//初始化显示屏
	lcd_init();
	//初始化触摸屏
	touch_init();
	//启动进入界面
	showbmp();
	//创建线程
	creat_pthread();
    //电梯运行操作
	elevator_operation();
	//解除触摸屏初始化
	touch_uninit();
	//解除显示屏初始化
	lcd_uninit();
}