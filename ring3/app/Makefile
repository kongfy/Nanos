#编译所有子目录
SUBDIRS=`ls -d */`

define foreach_make
	@for subdir in $(SUBDIRS) ; do \
		(cd $$subdir && make $1) \
	done;
endef

.PHONY:all clean

all:
	$(call foreach_make, all)

clean:
	$(call foreach_make, clean)