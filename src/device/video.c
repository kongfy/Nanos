#include "common.h"
#include "device.h"

static uint8_t vbuf[SCR_SIZE];
static uint8_t vref[SCR_SIZE];
uint8_t *vmem = vbuf;

void
prepare_buffer(void) {
	memcpy(vref, vbuf, SCR_SIZE);
	vmem = vbuf;
	memset(vmem, 0, SCR_SIZE);
}

void
display_buffer(void) {
	int i;
	uint32_t *buf = (uint32_t*)vbuf;
	uint32_t *ref = (uint32_t*)vref;
	uint32_t *mem = (uint32_t*)VMEM_ADDR;
	vmem = VMEM_ADDR;
	for (i = 0; i < SCR_SIZE / 4; i ++) {
		if (buf[i] != ref[i]) { // 只更新修改过的像素，这对一些QEMU实现有巨大性能提升
			mem[i] = buf[i];
		}
	}
}

static inline void
draw_character(char ch, int x, int y, int color) {
	int i, j;
	assert((ch & 0x80) == 0);
	char *p = font8x8_basic[(int)ch];
	for (i = 0; i < 8; i ++) 
		for (j = 0; j < 8; j ++) 
			if ((p[i] >> j) & 1)
				draw_pixel(x + j, y + i, color);
}

void
draw_string(const char *str, int x, int y, int color) {
	while (*str) {
		draw_character(*str ++, x, y, color);
		if (x + 8 >= SCR_WIDTH) {
			y += 8; x = 0;
		} else {
			x += 8;
		}
	}
}

