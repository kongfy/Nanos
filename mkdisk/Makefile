# 编译器设定和编译选项
CC = gcc
CFLAGS = -m32 -march=i386 -static -MD -std=gnu89 -ggdb \
		 -Wall -Werror -O2 -I./include
LDFLAGS = -melf_i386

# 链接器设定
LD = gcc

CFILES = $(shell find src/ -name "*.c")
SFILES = $(shell find src/ -name "*.S")
OBJS = $(CFILES:.c=.o) $(SFILES:.S=.o)

mkdisk: $(OBJS)
	$(LD) $(LDFLAGS) -o mkdisk $(OBJS) $(LIBS)

-include $(patsubst %.o, %.d, $(OBJS))

# 定义的一些伪目标
.PHONY: all clean

all: mkdisk

debug: mkdisk
	gdb mkdisk

clean:
	rm -f mkdisk $(OBJS) $(OBJS:.o=.d)
