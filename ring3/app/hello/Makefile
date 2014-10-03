include ../path.make

# 编译器设定和编译选项
CC = gcc
CFLAGS = -m32 -march=i386 -static -MD -std=gnu89 -ggdb \
		 -fno-builtin -fno-stack-protector -fno-omit-frame-pointer \
		 -Wall -Werror -O2 -I$(INC_PATH)
ASFLAGS = -ggdb -m32 -MD -I$(INC_PATH)
LDFLAGS = -melf_i386

# 链接器设定
LD = ld
LIBS = -L$(LIB_PATH) -lnanos

# 编译目标
TARGET = $(notdir $(CURDIR)) 
CFILES = $(shell find . -name "*.c")
SFILES = $(shell find . -name "*.S")
OBJS = $(CFILES:.c=.o) $(SFILES:.S=.o)

-include $(patsubst %.o, %.d, $(OBJS))

app: $(OBJS)
	$(LD) $(LDFLAGS) -e main -o $(TARGET) $(OBJS) $(LIBS)

# 定义的一些伪目标
.PHONY: all clean

all: app

clean:
	rm -f $(TARGET) $(OBJS) $(OBJS:.o=.d)
