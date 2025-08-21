#include "../include/main.h"

/* 全局变量定义 */
int j[10] = {0};          // 用来标识楼层是否被按下
int nowf = 1;             // 当前楼层
int tf = 0;               // 目标楼层
int h_num = 0;            // 当前楼层上面有几个楼层被按下
int l_num = 0;            // 当前楼层下面有几个楼层被按下
int x7, y7;               // 计算后的楼层对应结果
int x, y;                 // 触摸点坐标
int *addr = NULL;         // 内存映射地址

/* 图片路径定义 */
char *onbmp[10] = {
    "/qaq/bmp/001.bmp", "/qaq/bmp/002.bmp", "/qaq/bmp/003.bmp",
    "/qaq/bmp/004.bmp", "/qaq/bmp/005.bmp", "/qaq/bmp/006.bmp",
    "/qaq/bmp/007.bmp", "/qaq/bmp/008.bmp", "/qaq/bmp/009.bmp",
    "/qaq/bmp/010.bmp"
};

char *keybmp[10] = {
    "/qaq/bmp/01.bmp", "/qaq/bmp/02.bmp", "/qaq/bmp/03.bmp",
    "/qaq/bmp/04.bmp", "/qaq/bmp/05.bmp", "/qaq/bmp/06.bmp",
    "/qaq/bmp/07.bmp", "/qaq/bmp/08.bmp", "/qaq/bmp/09.bmp",
    "/qaq/bmp/10.bmp"
};

char *numbmp[10] = {
    "/qaq/bmp/1.bmp", "/qaq/bmp/2.bmp", "/qaq/bmp/3.bmp",
    "/qaq/bmp/4.bmp", "/qaq/bmp/5.bmp", "/qaq/bmp/6.bmp",
    "/qaq/bmp/7.bmp", "/qaq/bmp/8.bmp", "/qaq/bmp/9.bmp",
    "/qaq/bmp/0010.bmp"
};

/**
 * @brief 分析触摸点对应的楼层
 * @param n 触摸计算后的楼层编号
 */
void analyz(int n) {
    int x3, y3;
    x3 = n % 2;                  // 计算楼层奇偶性，奇数x=600，偶数x=700
    y3 = ((n + 1) / 2 - 1) * 96; // 计算按钮对应的y坐标
    x7 = x3;                     // 赋值给全局变量
    y7 = y3;
}

/**
 * @brief 分析当前楼层上下还有多少楼层被按下
 */
void fenxi() {
    int nowftemp = nowf;
    int *p = j;
    int h_n = 0, l_n = 0;
    
    // 计算当前楼层上面被按下的楼层数
    for (int i = nowftemp; i < 11; i++) {
        if (p[i-1] == 1) {
            h_n++;
        }
    }
    
    // 计算当前楼层下面被按下的楼层数
    for (int i = nowftemp; i > 0; i--) {
        if (p[i-1] == 1) {
            l_n++;
        }
    }
    
    h_num = h_n;
    l_num = l_n;
    printf("分析：现在在：%d\n", nowf);
    printf("分析：上面有:%d 下面有%d\n", h_num, l_num);
}

/**
 * @brief 显示电梯界面和楼层按钮
 */
void showbmp() {
    // 显示主界面
    lcd_draw_bmp("/qaq/bmp/elevatorinterface.bmp", 0, 0);
    
    // 显示楼层按钮
    lcd_draw_bmp(keybmp[0], 600, 0);//1
    lcd_draw_bmp(keybmp[1], 700, 0);//2
    lcd_draw_bmp(keybmp[2], 600, 96);//3
    lcd_draw_bmp(keybmp[3], 700, 96);//4
    lcd_draw_bmp(keybmp[4], 600, 192);//5
    lcd_draw_bmp(keybmp[5], 700, 192);//6
    lcd_draw_bmp(keybmp[6], 600, 288);//7
    lcd_draw_bmp(keybmp[7], 700, 288);//8
    lcd_draw_bmp(keybmp[8], 600, 384);//9
    lcd_draw_bmp(keybmp[9], 700, 384);//10
    
    // 显示当前楼层
    lcd_draw_bmp(numbmp[nowf-1], 200, 120);
}

/**
 * @brief 创建系统运行所需的线程
 * @return 0表示成功，-1表示失败
 */
int creat_pthread() {
    pthread_t pid1, pid2, pid3;
    int ret;
    
    // 创建广告线程
    ret = pthread_create(&pid1, NULL, ad, NULL);
    if (ret == -1) {
        printf("pthread_create(ad) error\n");
        return -1;
    }
    pthread_detach(pid1);
    
    // 创建按键处理线程
    ret = pthread_create(&pid2, NULL, run, NULL);
    if (ret == -1) {
        printf("pthread_create(run) error\n");
        return -1;
    }
    pthread_detach(pid2);
    
    // 创建服务器线程
    ret = pthread_create(&pid3, NULL, server_create, NULL);
    if (ret == -1) {
        printf("pthread_create(server_create) error\n");
        return -1;
    }
    pthread_detach(pid3);
    
    return 0;
}

/**
 * @brief 广告显示线程函数
 * @param arg 线程参数（未使用）
 * @return 线程返回值
 */
void* ad(void* arg) {
    SIM *head = head_SIM();
    if (head == NULL) {
        perror("head_SIM failed");
        return NULL;
    }
    
    // 读取目录下的图片到链表
    open_dir(FILE_DIR, head);
   
    
    // 循环显示广告图片
    while (1) {
        SIM *p = head->next;
        while (p != head) {
            printf("显示广告图片: %s\n", p->image_name);
            lcd_draw_bmp(p->image_name, 0, 0);
            p = p->next;
            sleep(3);  // 每张图片显示3秒
        }
    }
    
    // 释放链表（实际不会执行到这里）
    // release_SIM(head);
    return NULL;
}

/**
 * @brief 按键处理线程函数
 * @param arg 线程参数（未使用）
 * @return 线程返回值
 */
void* run(void* arg) {
    int n = 0;
    printf("进入按键判断\n");
    
    while (1) {
        // 获取触摸坐标
        get_xy1(&x, &y);
        printf("获取坐标值了\n");
        
        // 坐标转换（适配屏幕分辨率）
        x = (x * 800) / 1024;
        y = (y * 480) / 600;
        printf("x=%d y=%d\n", x, y);
        
        // 判断触摸的楼层按钮
        if (x >= 600 && x < 700 && y >= 0 && y < 96)
            n = 1;
        else if (x >= 700 && x < 800 && y >= 0 && y < 96)
            n = 2;
        else if (x >= 600 && x < 700 && y >= 96 && y < 192)
            n = 3;
        else if (x >= 700 && x < 800 && y >= 96 && y < 192)
            n = 4;
        else if (x >= 600 && x < 700 && y >= 192 && y < 288)
            n = 5;
        else if (x >= 700 && x < 800 && y >= 192 && y < 288)
            n = 6;
        else if (x >= 600 && x < 700 && y >= 288 && y < 384)
            n = 7;
        else if (x >= 700 && x < 800 && y >= 288 && y < 384)
            n = 8;
        else if (x >= 600 && x < 700 && y >= 384 && y < 480)
            n = 9;
        else if (x >= 700 && x < 800 && y >= 384 && y < 480)
            n = 10;
        
        printf("当前按下的是%d\n", n);
        
        // 分析楼层位置
        analyz(n);
        
        // 处理楼层按键状态
        if (j[n-1] == 0) {
            if (nowf == n) {
                // 当前楼层按下，闪烁提示
                if (x7 == 1) {
                    lcd_draw_bmp(onbmp[n-1], 600, y7); // 楼层按钮亮起
                    sleep(1);
                    lcd_draw_bmp(keybmp[n-1], 600, y7); // 楼层按钮熄灭
                } else {
                    lcd_draw_bmp(onbmp[n-1], 700, y7); // 楼层按钮亮起
                    sleep(1);
                    lcd_draw_bmp(keybmp[n-1], 700, y7); // 楼层按钮熄灭
                }
            } else {
                // 非当前楼层，记录并点亮按钮
                j[n-1] = 1;
                fenxi();
                if (x7 == 1) {
                    lcd_draw_bmp(onbmp[n-1], 600, y7); // 楼层按钮亮起
                } else {
                    lcd_draw_bmp(onbmp[n-1], 700, y7); // 楼层按钮亮起
                }
            }
        } else {
            // 取消楼层选择，熄灭按钮
            j[n-1] = 0;
            if (x7 == 1) {
                lcd_draw_bmp(keybmp[n-1], 600, y7); // 楼层按钮熄灭
            } else {
                lcd_draw_bmp(keybmp[n-1], 700, y7); // 楼层按钮熄灭
            }
            fenxi();
        }
        
        n = 0;
    }
}

/**
 * @brief 服务器创建线程函数
 * @param arg 线程参数（未使用）
 * @return 线程返回值
 */
void* server_create(void* arg) {
    const ServerConfig config = {
        .ip = SERVER_IP,
        .port = SERVER_PORT,
        .file_dir = FILE_DIR,
        .backlog = 20
    };
    start_file_server(&config);
}

/**
 * @brief 电梯运行控制函数
 */
void elevator_operation() {
    int x1, y1; // 楼层按钮坐标
    
    while (1) {
        // 电梯向上运行
        for (; h_num != 0; nowf++) {
            printf("电梯运行中现在是在：%d层\n", nowf);
            printf("电梯运行中上面还有 %d层\n", h_num);
            
            // 显示向上箭头和当前楼层
            lcd_draw_bmp("/qaq/bmp/up.bmp", 200, 0);
            lcd_draw_bmp(numbmp[nowf - 1], 200, 120);
            
            if (j[nowf - 1] == 1) {
                // 到达目标楼层，开门处理
                sleep(1);                      // 模拟开门延时
                j[nowf - 1] = 0;               // 清除楼层标记
                analyz(nowf);                  // 分析楼层位置
                x1 = x7; y1 = y7;              // 获取按钮坐标
                printf("x1=%d  y1=%d\n", x1, y1);
                
                // 熄灭对应楼层按钮
                if (x1 == 1) {
                    lcd_draw_bmp(keybmp[nowf - 1], 600, y1);
                } else {
                    lcd_draw_bmp(keybmp[nowf - 1], 700, y1);
                }
                
                h_num--;                       // 减少上层计数
                fenxi();                       // 重新分析楼层状态
            }
            
            sleep(1);                          // 模拟运行延时
            
            if (h_num == 0) {
                // 上层无目标，清除箭头显示
                printf("现在在：%d\n", nowf);
                lcd_draw_bmp("/qaq/bmp/restore_pic1.bmp", 200, 0);
                lcd_draw_bmp(numbmp[nowf - 1], 200, 120);
                break;
            }
        }
        
        // 电梯向下运行
        for (; l_num != 0; nowf--) {
            // 显示向下箭头和当前楼层
            lcd_draw_bmp("/qaq/bmp/down.bmp", 200, 0);
            lcd_draw_bmp(numbmp[nowf - 1], 200, 120);
            
            if (j[nowf - 1] == 1) {
                // 到达目标楼层，开门处理
                sleep(1);                      // 模拟开门延时
                j[nowf - 1] = 0;               // 清除楼层标记
                analyz(nowf);                  // 分析楼层位置
                x1 = x7; y1 = y7;              // 获取按钮坐标
                printf("x1=%d  y1=%d\n", x1, y1);
                
                // 熄灭对应楼层按钮
                if (x1 == 1) {
                    lcd_draw_bmp(keybmp[nowf - 1], 600, y1);
                } else {
                    lcd_draw_bmp(keybmp[nowf - 1], 700, y1);
                }
                
                l_num--;                       // 减少下层计数
                fenxi();                       // 重新分析楼层状态
            }
            
            sleep(1);                          // 模拟运行延时
            
            if (l_num == 0) {
                // 下层无目标，清除箭头显示
                printf("现在在：%d\n", nowf);
                lcd_draw_bmp("/qaq/bmp/restore_pic1.bmp", 200, 0);
                lcd_draw_bmp(numbmp[nowf - 1], 200, 120);
                break;
            }
        }
    }
}