#项目名称
    基于ARM开发板的XXXXXX

#项目介绍
  本项目是基于ARM开发板完成的户外电子广告屏。
   能够实现显示图片广告和字体的基本功能：

#环境依赖
    ununtu18.04 、Linux环境、ARM6818开发板、交叉编译工具链

#编译的方法
编译
make 

清除+编译+执行
make all

功能描述
1. 主界面显示 ：展示主界面背景及多个应用入口图标，用户可通过触摸相应区域进入不同应用。
2. 图片浏览应用 ：支持显示指定路径下的 BMP 和 JPG 图片，可进行图片的切换、缩放操作。
3. 文字显示应用 ：在屏幕上显示文字信息，可设置文字的颜色、位置等属性。
4. 开机动画 ：系统启动时播放开机动画，提升用户体验。
5. 触摸屏交互 ：通过触摸屏进行应用切换、图片浏览等操作，支持点击、滑动等手势。


#目录结构
├── bin		 编译后的可执行文件目录
│   └── main
├── include	 头文件目录
│   ├── head.h	bmp和ts的头文件
│   ├── jconfig.h
│   ├── jerror.h
│   ├── jmorecfg.h
│   ├── jpeglib.h
│   ├── display.h
│   ├── touch.h
│   └── lcdjpg.h
├── lib		库文件目录
│   ├── liba	静态库
│   │   ├── libjpeg.a
│   │   └── libjpeg.la
│   └── libso	动态库
│       ├── libjpeg.so
│       ├── libjpeg.so.9
│       └── libjpeg.so.9.1.0
├── main		 主程序的目录
│   └── main.c
├── Makefile	 工程管理文件Makefile
├── README.txt	 工程说明文档Readme
└── src	源文件目录(除了main程序的其他所有.c程序)
    ├── bmp.c
    ├── lcdjpg.c
    ├── touch.c
    ├── display.c
    └── ts.c
    


7 directories, 18 files

#版本
V1.0.0版本

#作者     
bling  

#时间
2025.05.28

#联系方式
QQ：1678843172

#说明 tree指令的安装
gec@ubuntu:/mnt/hgfs/GZFX2308/5_阶段1项目/5_工程编译/project$
sudo apt-get install tree
[sudo] password for gec: 
