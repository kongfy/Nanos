#include "x86.h"

#define PORT_PIC_MASTER 0x20
#define PORT_PIC_SLAVE  0xA0
#define PORT_PIC_MASTER_COMMAND PORT_PIC_MASTER
#define PORT_PIC_MASTER_DATA (PORT_PIC_MASTER + 1)
#define PORT_PIC_SLAVE_COMMAND PORT_PIC_SLAVE
#define PORT_PIC_SLAVE_DATA (PORT_PIC_SLAVE + 1)
#define IRQ_SLAVE       2

/* 初始化8259中断控制器：
 * 硬件中断IRQ从32号开始，自动发送EOI */
void
init_intr(void) {
    out_byte(PORT_PIC_MASTER_DATA, 0xFF);
    out_byte(PORT_PIC_SLAVE_DATA , 0xFF);

    // 0x11为PIC初始化请求，PIC接受请求后会等待三条data port上的"initialisation words"
    // 分别设置IRQ的偏移、PIC连接方式和附加信息
    // 初始化MASTER PIC
    out_byte(PORT_PIC_MASTER_COMMAND, 0x11);
    out_byte(PORT_PIC_MASTER_DATA, 32);           // Master PIC vector offset
    out_byte(PORT_PIC_MASTER_DATA, 1 << 2);       // tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
    out_byte(PORT_PIC_MASTER_DATA, 0x3);          // 0x01 | 0x02, 0x01:8086/88 (MCS-80/85) mode; 0x02:Auto (normal) EOI
    // 初始化SLAVE PIC
    out_byte(PORT_PIC_SLAVE_COMMAND, 0x11);
    out_byte(PORT_PIC_SLAVE_DATA, 32 + 8);        // Slave PIC vector offset
    out_byte(PORT_PIC_SLAVE_DATA, 2);             // tell Slave PIC its cascade identity (0000 0010)
    out_byte(PORT_PIC_SLAVE_DATA, 0x3);           // 0x01 | 0x02, 0x01:8086/88 (MCS-80/85) mode; 0x02:Auto (normal) EOI

    out_byte(PORT_PIC_MASTER_COMMAND, 0x68);
    out_byte(PORT_PIC_MASTER_COMMAND, 0x0A);
    out_byte(PORT_PIC_SLAVE_COMMAND, 0x68);
    out_byte(PORT_PIC_SLAVE_COMMAND, 0x0A);
}

// 详细参见 http://wiki.osdev.org/8259_PIC
