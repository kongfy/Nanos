/* 参考：i386手册 */
#define SEG_KCODE 1
#define SEG_KDATA 2

#define KSEL(n)  \
	((n) << 3)
