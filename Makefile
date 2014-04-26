# GNU make手册：http://www.gnu.org/software/make/manual/make.html
# ************ 遇到不明白的地方请google以及阅读手册 *************

# 编译器设定和编译选项
CC = gcc
LD = ld
CFLAGS = -m32 -march=i386 -static -MD -std=gnu89 -ggdb \
		 -fno-builtin -fno-stack-protector -fno-omit-frame-pointer \
		 -Wall -Werror -O2 -I./include
ASFLAGS = -ggdb -m32 -MD
LDFLAGS = -melf_i386
QEMU = qemu-system-i386

# 编译目标：src目录下的所有.c和.S文件
CFILES = $(shell find src/ -name "*.c")
SFILES = $(shell find src/ -name "*.S")
OBJS = $(CFILES:.c=.o) $(SFILES:.S=.o)

kernel.img: kernel
	@cd boot; make
	cat boot/bootblock kernel > kernel.img

kernel: $(OBJS)
	$(LD) $(LDFLAGS) -e entry -Ttext 0x00100000 -o kernel $(OBJS)

-include $(patsubst %.o, %.d, $(OBJS))

# 定义的一些伪目标
.PHONY: play clean debug
play: kernel.img
	$(QEMU) -serial stdio kernel.img

debug: kernel.img
	$(QEMU) -serial stdio -s -S kernel.img

clean:
	@cd boot; make clean
	rm -f kernel kernel.img $(OBJS) $(OBJS:.o=.d)
