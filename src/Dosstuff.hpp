//dos functions library
//Handles abstracted file stuff, interrupts and DPMI functions for allocating low memory
#ifndef _dosstuff_h_
#define _dosstuff_h_
#include <stdio.h>
#include "ttypes.hpp"
extern FILE *logfile;
extern char *scan;
extern UINT16 *timer;

#define _Packed

typedef struct segptr
{
    UINT16 seg;
    UINT16 offset;
} segptr;    
    

typedef _Packed struct rminfo
{
    long EDI;
    long ESI;
    long EBP;
    long reserved_by_system;
    long EBX;
    long EDX;
    long ECX;
    long EAX;
    short flags;
    short ES,DS,FS,GS,IP,CS,SP,SS;
} rminfo;

extern rminfo RMI;




typedef _Packed struct DPMI_PTR
{
  UINT16 segment;
  UINT16 selector;
  UINT32 size; //size in paragraphs
} DPMI_PTR;


extern void lowcase(char *str);
extern void upcase(char *str);
extern char lowcase(char str);
extern char upcase(char str);
extern void tdelay(UINT16 count);

extern void initlog(void (*logmessagefunc)(void *data,const char *format,...));
extern void shutdownlog();
extern void logmessagexy(UINT16 x,UINT16 y,const char *format,...);
extern void *realtolinear(UINT32 realAddr);
extern UINT32 lineartoreal(void *linear);
extern void RMIRQ(char irq,rminfo *RMI);
extern char *DPMI_AllocDOSMem (short int paras,DPMI_PTR &p);
extern BOOL DPMI_FreeDOSMem (DPMI_PTR &p);
extern void * DPMI_MAP_PHYSICAL(void *p, UINT32 size);
extern void DPMI_UNMAP_PHYSICAL(void *p);
extern char *makepath(char *outtext,char *intxt);
extern void tsound(UINT16 frequency);
extern void tsoundoff();

extern BOOL fexist(char *filename);
extern int fgetsize(char *path);

extern void (*logmessage)(void* data,const char *format,...);

void extern outportb(UINT16 port,UINT8 data);
#pragma aux outportb=\
"out dx,al"\
parm[dx][al]

void extern outportw(UINT16 port,UINT16 data);
#pragma aux outportw=\
"out dx,ax"\
parm[dx][ax]

UINT8 extern inportb(UINT16 port);
#pragma aux inportb=\
"in al,dx"\
parm[dx]\

void extern inportw(UINT16 port);
#pragma aux inportw=\
"in ax,dx"\
parm[dx]


// internal pentium counter instructions
#define RDTSC "db 0x0f, 0x031"//read time stamp counter into EDX:EAX
#define RDMSR "db 0x0f, 0x032"//read MSR variable ECX into EDX:EAX
#define WRMSR "db 0x0f, 0x030"//write MSR variable ECX from EDX:EAX

void cleartextscreen (void);
// just clears the text-screen
#pragma aux cleartextscreen = "pushad            " \
                              "mov edi, 0b8000h  " \
                              "mov eax, 7200720h " \
                              "mov ecx, 1000     " \
                              "rep stosd         " \
                              "mov ah, 02h       " \
                              "xor ebx, ebx      " \
                              "xor edx, edx      " \
                              "int 10h           " \
                              "popad             " \
                              modify[]




#endif
