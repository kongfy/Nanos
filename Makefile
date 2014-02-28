# GNU make手册：http://www.gnu.org/software/make/manual/make.html
# ************ 遇到不明白的地方请google以及阅读手册 *************

# 编译器设定和编译选项
CC = gcc
LD = ld
CFLAGS = -m32 -march=i386 -static -MD -std=gnu89 -ggdb \
		 -fno-builtin -fno-stack-protector -fno-omit-frame-pointer \
		 -Wall -Werror -O2 -I./include
ASFLAGS = -m32 -MD
LDFLAGS = -melf_i386
GITFLAGS = -q --author='tracer <tracer@njuoslab.org>' --no-verify --allow-empty
QEMU = qemu-system-i386

# 编译目标：src目录下的所有.c和.S文件
CFILES = $(shell find src/ -name "*.c")
SFILES = $(shell find src/ -name "*.S")
OBJS = $(CFILES:.c=.o) $(SFILES:.S=.o)

game.img: game
	@cd boot; make
	cat boot/bootblock game > game.img
	-@git add . --ignore-errors &> /dev/null # KEEP IT
	-@(echo "> compile" && uname -a && uptime && pstree -A) | git commit -F - $(GITFLAGS) # KEEP IT

game: $(OBJS)
	$(LD) $(LDFLAGS) -e game_init -Ttext 0x00100000 -o game $(OBJS)

-include $(patsubst %.o, %.d, $(OBJS))

# 定义的一些伪目标
.PHONY: play clean debug
play: game.img
	-@git commit -m "> play" $(GITFLAGS) # KEEP IT
	$(QEMU) -enable-kvm game.img

debug: game.img
	$(QEMU) -s -S game.img

clean:
	@cd boot; make clean
	rm -f game game.img $(OBJS) $(OBJS:.o=.d)
