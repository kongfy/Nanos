# 定义的一些伪目标
.PHONY:all lib app clean
all: lib app

lib:
	@cd lib; make all

app:
	-@mkdir ../disk/bin
	@cd app; make all

clean:
	-@rm -rf ../disk/bin
	@cd lib; make clean
	@cd app; make clean
