#ifndef __X86_MEMORY_H__
#define __X86_MEMORY_H__

#define DPL_KERNEL              0
#define DPL_USER                3

#define NR_SEGMENTS             3
#define SEG_KERNEL_CODE         1 
#define SEG_KERNEL_DATA         2

struct GateDescriptor {
	uint32_t offset_15_0      : 16; // offset的低16位
	uint32_t segment          : 16;
	uint32_t pad0             : 8;  // 0
	uint32_t type             : 4;  // Task gate, interrupt gate or trap gate
	uint32_t system           : 1;  // = 0 for interrupt gates.
	uint32_t privilege_level  : 2;  // 调用需要的最低特权等级，防止特权指令被用户空间调用
	uint32_t present          : 1;  // can be set to 0 for unused interrupts or for Paging.
	uint32_t offset_31_16     : 16; // offset的高16位
};

struct TrapFrame {
	uint32_t edi, esi, ebp, xxx, ebx, edx, ecx, eax;
	int32_t irq;
};

#endif
