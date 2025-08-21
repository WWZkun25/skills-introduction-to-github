#include "../include/bmp.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <linux/input.h> //触摸屏的头文件

int *plcd=NULL;
int fd_lcd = 0;

//屏幕初始化
void lcd_init()
{
	//打开屏幕
	fd_lcd = open("/dev/fb0", O_RDWR);
    if (fd_lcd == -1)
    {
        printf("open /dev/fd0 error\n");
        exit(1);
    }

	//映射
	plcd = (int *)mmap(NULL,800*480*4,PROT_EXEC | PROT_READ | PROT_WRITE,MAP_SHARED ,
						  fd_lcd, 0);
	if(plcd == MAP_FAILED)
	{
		printf("mmap lcd error\n");
		exit(1);
	}
}
			
			
//解除屏幕初始化			
void lcd_uninit()
{
    munmap(plcd, 800*480*4);
    close(fd_lcd);
}
			
/*
	给显示屏上的点显示一个颜色
	@x,y:点的坐标
	@color:需要画的颜色
*/
void lcd_point(int x,int y,int color)
{
	if(x>=0 && x<800 && y>=0 && y<480)
	{
		*(plcd+800*y+x)=color;
	}

}

/**
 * @brief 画矩形
 * 
 * @param w 矩形的宽
 * @param h 矩形的高
 * @param x 矩形最左上角的横坐标
 * @param y 矩形最左上角的纵坐标
 * @param color 矩形的颜色
 */
void lcd_draw_rect(int w, int h, int x, int y, int color)
{
    int i, j;
    for (i = y; i < y+h; i++)
    {
        for (j = x; j < x+w; j++)
        {
            lcd_point(j, i, color);
        }
    }
}

/**
 * @brief 画圆
 * 
 * @param x 圆心的横坐标
 * @param y 圆心的纵坐标
 * @param r 圆的半径
 * @param color 圆的颜色
 */
void lcd_draw_circle(int x, int y, int r, int color)
{
    int i, j;
    for (j = 0; j < 480; j++)
    {
        for (i = 0; i < 800; i++)
        {
            if ((i-x)*(i-x) + (j-y)*(j-y) <= r*r)
            {
                lcd_point(i, j, color);
            }
        }
    }
}

/**
 * @brief 获取图片信息，开辟存储空间保存像素数组
 * 
 * @param bmpname 图片名称
 * @return BMP 返回图片所需信息结构体
 */
BMP get_bmp_info(char* bmpname)
{
	//练习，打开bmp文件读取文件大小，宽，高，色深，开辟一个空间保存像素数组
	//可以下载其他格式的图片，在电脑自带的图片编辑中，将图片的像素进行调整，另存为bmp(24位)图片即可
	//size --> 0x02 - 4字节   宽 --> 0x12 -- 4字节  高 ->> 0x16 -- 4字节      色深 ->> 0x1c -- 2字节
	int fd_bmp = open(bmpname, O_RDONLY);
	if (fd_bmp == -1)
	{
		printf("open bmp error\n");
		exit(1);
	}

	char bm[2] = {0};
	read(fd_bmp, bm, 2);
	if (!(bm[0] == 'B' && bm[1] == 'M'))
	{
		// printf("bm[0] = %c, bm[1] = %c\n", bm[0], bm[1]);
		printf("%s is not a bmp file\n", bmpname);
		exit(1);
	}

	BMP bmp;
	bmp.bmpname = bmpname;

	//大小
	lseek(fd_bmp, 0x02, SEEK_SET);
	read(fd_bmp, &bmp.size, 4);

	//宽
	lseek(fd_bmp, 0x12, SEEK_SET);
	read(fd_bmp, &bmp.width, 4);
	//高
	read(fd_bmp, &bmp.height, 4);
	//色深
	lseek(fd_bmp, 0x1c, SEEK_SET);
	read(fd_bmp, &bmp.bpp, 2);

	//开辟一块空间用来保存像素数组
	bmp.data = (char*)malloc(bmp.size - 54);
	lseek(fd_bmp, 0x36, SEEK_SET);
	read(fd_bmp, bmp.data, (bmp.size-54));

	close(fd_bmp);
	return bmp;
}


/**
 * @brief 在开发板上显示一bmp张图片
 * 
 * @param bmpname 图片的名称
 * @param x 图片最左上角的横坐标
 * @param y 图片做左上角的纵坐标
 */
void lcd_draw_bmp(char* bmpname, int x, int y)
{
	BMP bmp = get_bmp_info(bmpname);
	//打印图片的基本信息
	printf("bmpname = %s, size = %d, width = %d, height = %d, bpp = %hd\n", 
			bmp.bmpname, bmp.size, bmp.width, bmp.height, bmp.bpp);

	//一行的有效字节数
	int line_types = abs(bmp.width) * bmp.bpp / 8;

	//一行的填充字节数
	int pad_types = (line_types % 4 == 0) ? 0 : (4 - (line_types % 4));

	int color;
	char* p = bmp.data; //遍历指针,遍历整个像素数组

	//解析像素数组中的数据并画到开发板上
	for (int h = 0; h < abs(bmp.height); h++) //表示行
	{
		for (int w = 0; w < abs(bmp.width); w++) //表示列
		{
			char b = *(p++);
			char g = *(p++);
			char r = *(p++);
			char a = (bmp.bpp == 24) ? 0 : *(p++);

			color = a << 24 | r << 16 | g << 8 | b;

			//练习： 确定m和n的值, 推荐用问号表达式
			// width > 0 : 从左到右保存每一个像素点
            // width < 0 : 从右到左保存每一个像素点
			// height > 0: 从下到上保存每一个像素点
            // height < 0: 从上到下保存每一个像素点

			int m =x+(bmp.width>0?w:(bmp.width-1-w));
			int n =y+(bmp.height<0?h:(bmp.height-1-h));
		
			lcd_point(m, n, color);
		}
		p += pad_types;//跳过填充字节
	}

	
}

int show_bmp_size(int *addr,char *bmp_path,int offset_x,int offset_y )
{
   //打开bmp图(任意大小:长宽必须是4的倍数)
   int fd_bmp = open(bmp_path,O_RDWR);
   if(fd_bmp < 0)
   {
       printf("open bmp fail\n");
       return -1;
   }

   //计算bmp图片的高度和高度
   int height = 0; //高度
   int width = 0; //宽度
   lseek(fd_bmp,18,SEEK_SET);
   read(fd_bmp,&width,4); //读取bmp宽度
   read(fd_bmp,&height,4);//读取bmp高度
   printf("bmp width:%d height:%d\n",width,height);

   //读取bmp图
   //去掉bmp图片的头54字节
   lseek(fd_bmp,54,SEEK_SET);
   char buf[width*height*3]; //变长数组不能初始化
   read(fd_bmp,buf,width*height*3);

   //读写lcd文件
   int x = 0; //横轴
   int y = 0; //纵轴
   for(y=0;y<height;y++)
   {
       for(x=0;x<width;x++)
       {
           //bmp照片最后一行的起始点的坐标是(offset_x,offset_y+height-1)
           *(addr+((offset_y+height-1)-y)*800 + offset_x+x) = 
                                   buf[(y*width+x)*3 +0] |  
                                   buf[(y*width+x)*3 +1]<<8 |
                                   buf[(y*width+x)*3 +2]<<16;
       }
   }

   //关闭bmp
   close(fd_bmp);
}