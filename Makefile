# GNU make手册：http://www.gnu.org/software/make/manual/make.html
# ************ 遇到不明白的地方请google以及阅读手册 *************

# 编译器设定和编译选项
CC = gcc
LD = ld
CFLAGS = -m32 -march=i386 -static -MD -std=gnu89 -ggdb \
		 -fno-builtin -fno-stack-protector -fno-omit-frame-pointer \
		 -Wall -Werror -O2 -I./include -I./include/lib
ASFLAGS = -ggdb -m32 -MD -I./include -I./include/lib
LDFLAGS = -melf_i386
QEMU = qemu-system-i386

# 编译目标：src目录下的所有.c和.S文件
CFILES = $(shell find src/ -name "*.c")
SFILES = $(shell find src/ -name "*.S")
OBJS = $(CFILES:.c=.o) $(SFILES:.S=.o)

nanos.img: kernel.img disk.img
	@./extend.pl kernel.img
	cat kernel.img disk.img > nanos.img

kernel.img: kernel
	@cd boot; make
	cat boot/bootblock kernel > kernel.img

kernel: $(OBJS)
	$(LD) $(LDFLAGS) -e entry -Ttext 0xC0100000 -o kernel $(OBJS)

disk.img: programs mkdisk
	./mkdisk/mkdisk -p disk

-include $(patsubst %.o, %.d, $(OBJS))

# 定义的一些伪目标
.PHONY: play clean debug programs mkdisk
programs:
	@cd programs; make all

mkdisk:
	@cd mkdisk; make

play: nanos.img
	$(QEMU) -no-reboot -no-shutdown -serial stdio nanos.img

debug: nanos.img
	$(QEMU) -no-reboot -no-shutdown -serial stdio -s -S nanos.img

clean:
	@cd boot; make clean
	@cd programs; make clean
	@cd mkdisk; make clean
	rm -f kernel kernel.img disk.img nanos.img $(OBJS) $(OBJS:.o=.d)
