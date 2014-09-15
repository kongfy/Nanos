#ifndef __ELF_H__
#define __ELF_H__

/* ELF32二进制文件头 */
struct ELFHeader {
    unsigned int   magic;
    unsigned char  elf[12];
    unsigned short type;
    unsigned short machine;
    unsigned int   version;
    unsigned int   entry;
    unsigned int   phoff;
    unsigned int   shoff;
    unsigned int   flags;
    unsigned short ehsize;
    unsigned short phentsize;
    unsigned short phnum;
    unsigned short shentsize;
    unsigned short shnum;
    unsigned short shstrndx;
};

/* ELF32 Program header */
struct ProgramHeader {
    unsigned int type;
    unsigned int off;
    unsigned int vaddr;
    unsigned int paddr;
    unsigned int filesz;
    unsigned int memsz;
    unsigned int flags;
    unsigned int align;
};

#endif /* __ELF_H__ */
