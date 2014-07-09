# 编译器设定和编译选项
CC = gcc
CFLAGS = -m32 -march=i386 -static -MD -std=gnu89 -ggdb \
		 -fno-builtin -fno-stack-protector -fno-omit-frame-pointer \
		 -Wall -Werror -O2 -I../../include
ASFLAGS = -ggdb -m32 -MD -I../../include

# 链接器设定
LD = ld
LIBS = -L../../lib

# 编译目标
CFILES = $(shell find . -name "*.c")
SFILES = $(shell find . -name "*.S")
OBJS = $(CFILES:.c=.o) $(SFILES:.S=.o)

app: $(OBJS)
	$(LD) $(LDFLAGS) -e main -o app $(OBJS)

# 定义的一些伪目标
.PHONY: all clean

all: app

clean:
	rm -f app $(OBJS) $(OBJS:.o=.d)
