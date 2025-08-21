#变量
TAR = bin/main
SRC = $(wildcard src/*.c) #搜索src目录下面的所有.c文件
MAIN = main/1.c
INC = -I ./include/
#CC = gcc
CC = arm-linux-gcc

#静态库
LIBA = -L ./lib/liba -lfont
#动态库
LIBSO = -L ./lib/libso -ljpeg

#伪指令
.PHONY:$(TAR) run clean


#目标:依赖
$(TAR): $(MAIN) $(SRC)
	$(CC)  $^ -o $@ $(INC) $(LIBSO) $(LIBA) -lpthread 

#执行
run:
	./$(TAR)

#在clean前面加上.PHONY使之变成伪指令
#.PHONY:clean
#清除
clean:
	$(RM) $(TAR)


#make all的时候一次性执行三条指令
all:clean $(TAR) run
