#include "kernel.h"
#include "string.h"

void copy_from_kernel(Thread* pcb, void* dest, void* src, int len){
    memcpy(dest, src, len);
}

void copy_to_kernel(Thread* pcb, void* dest, void* src, int len){
    memcpy(dest, src, len);
}

void strcpy_to_kernel(Thread* pcb, char* dest, char* src){
    strcpy(dest, src);
}

void strcpy_from_kernel(Thread* pcb, char* dest, char* src){
    strcpy(dest, src);
}
