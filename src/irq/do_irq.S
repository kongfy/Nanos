# 中断和异常处理函数的入口
# 中断处理函数会在IDT中为相应的中断/异常设置处理程序
# 中断/异常的行为参见i386手册

#include "x86/memory.h"

#----|-----entry------|-errorcode-|-----id-----|---handler---|
.globl vec0;    vec0:   pushl $0;  pushl    $0; jmp asm_do_irq
.globl vec1;    vec1:   pushl $0;  pushl    $1; jmp asm_do_irq
.globl vec2;    vec2:   pushl $0;  pushl    $2; jmp asm_do_irq
.globl vec3;    vec3:   pushl $0;  pushl    $3; jmp asm_do_irq
.globl vec4;    vec4:   pushl $0;  pushl    $4; jmp asm_do_irq
.globl vec5;    vec5:   pushl $0;  pushl    $5; jmp asm_do_irq
.globl vec6;    vec6:   pushl $0;  pushl    $6; jmp asm_do_irq
.globl vec7;    vec7:   pushl $0;  pushl    $7; jmp asm_do_irq
.globl vec8;    vec8:              pushl    $8; jmp asm_do_irq
.globl vec9;    vec9:   pushl $0;  pushl    $9; jmp asm_do_irq
.globl vec10;   vec10:             pushl   $10; jmp asm_do_irq
.globl vec11;   vec11:             pushl   $11; jmp asm_do_irq
.globl vec12;   vec12:             pushl   $12; jmp asm_do_irq
.globl vec13;   vec13:             pushl   $13; jmp asm_do_irq
.globl vec14;   vec14:             pushl   $14; jmp asm_do_irq

.globl vecsys; vecsys:  pushl $0;  pushl $0x80; jmp asm_do_irq

.globl irq0;     irq0:  pushl $0;  pushl $1000; jmp asm_do_irq
.globl irq1;     irq1:  pushl $0;  pushl $1001; jmp asm_do_irq
.globl irq12;   irq12:  pushl $0;  pushl $1012; jmp asm_do_irq
.globl irq13;   irq13:  pushl $0;  pushl $1013; jmp asm_do_irq
.globl irq14;   irq14:  pushl $0;  pushl $1014; jmp asm_do_irq
.globl irq_empty; irq_empty:pushl $0;pushl $-1; jmp asm_do_irq

# ！调用C编写的中断处理程序(见irq/irq_handle.c)！
.globl asm_do_irq
.extern irq_handle
.extern current # 对应C代码中全局变量current
asm_do_irq:
    cli
    pushl %ds; pushl %es; pushl %fs; pushl %gs
    pushal # 依次把寄存器AX、CX、DX、BX、SP、BP、SI和DI压栈
    movw $SELECTOR_KERNEL(SEG_KERNEL_DATA), %ax
    movw %ax, %ds
    movw %ax, %es

    pushl %esp # 作为irq_handle函数的指针参数使用
    call irq_handle

    # 栈切换
    movl (current), %eax
    movl (%eax), %esp # 将esp置为current->tf

    # During popal intruction, The ESP value is discarded instead of loaded into ESP
    # http://www.fermimn.gov.it/linux/quarta/x86/popa.htm
    popal
    popl %gs; popl %fs; popl %es; popl %ds
    addl $8, %esp
    iret # ISR（Interrupt Service Routines）必须以iret返回

# 详细参见 http://wiki.osdev.org/Interrupt_Service_Routines
