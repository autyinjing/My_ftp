
# ------------------------------------------
#
# 	Makefile of my_ftp
#
#-------------------------------------------

C_SOURCES = $(shell find . -name "*.c")
C_OBJECTS = $(patsubst %.c, %.o, $(C_SOURCES))

#print:
#	echo $(C_SOURCES)
#	echo $(C_OBJECTS)

C_FLAGS = -c -g -rdynamic -Wall
C_LINKFLAGS = -lpthread

TARGET = my_ftp

CC = gcc

$(TARGET): $(C_OBJECTS)
	@echo 链接 ...
	$(CC) -o $(TARGET) $(C_OBJECTS) $(C_LINKFLAGS)

$(C_OBJECTS): %.o: %.c
	@echo 编译 ...
	$(CC) $(C_FLAGS) $< -o $@

.PHONY: clean

clean:
	@echo 删除中间文件
	rm -f $(C_OBJECTS)

.PHONY: install
# 需要root权限
install:
	@echo 安装服务器程序
	chmod a+x my_ftp
	cp my_ftp /usr/bin/

#end of Makefile
