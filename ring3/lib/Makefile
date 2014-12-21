# 编译器设定和编译选项
CC = gcc
CFLAGS = -m32 -march=i386 -static -MD -std=gnu89 -ggdb \
		 -fno-builtin -fno-stack-protector -fno-omit-frame-pointer \
		 -Wall -Werror -O2 -I../include
ASFLAGS = -ggdb -m32 -MD -I../include

AR = ar
ARFLAGS = rv

# 编译目标：目录下的所有.c和.S文件
CFILES = $(shell find . -name "*.c")
SFILES = $(shell find . -name "*.S")
OBJS = $(CFILES:.c=.o) $(SFILES:.S=.o)

-include $(patsubst %.o, %.d, $(OBJS))

.PHONY:all clean

all:nanos

nanos:$(OBJS)
	$(AR) $(ARFLAGS) libnanos.a $(OBJS)

clean:
	rm -f libnanos.a $(OBJS) $(OBJS:.o=.d)
