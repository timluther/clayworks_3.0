#include <stdlib.h>
#include <i86.h>
#include <dos.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <stdarg.h>
#include "ttypes.hpp"
#include "dosstuff.hpp"
#include "gbasics.hpp"

char *scan=(char*)0x417;
UINT16 *timer=(UINT16*)0x46C;//=(0x40,0x6c);
UINT16 logypos=0;
union REGS regs;
struct SREGS sregs;

FILE *logfile;

void (*logmessage)(void* data,const char *format,...);

void tdelay(UINT16 count)
{
    UINT16 start=*timer+count;
    while (start>*timer);              
}


#define SPEAKER_PORT 0x61
#define PIT_CONTROL  0x43
#define PIT_CHANNEL_2 0x42
#define PIT_FREQ 0x1234DD


void tsoundoff()
{
  outportb(SPEAKER_PORT,inportb(SPEAKER_PORT)&0xFC);
}

void tsound(UINT16 frequency)
{
  UINT16 counter;

  //Program the PIT chip }
  if (frequency<10) return;
  counter = PIT_FREQ / frequency;
  outportb(PIT_CONTROL,0xB6);
  outportb(PIT_CHANNEL_2,counter & 255);
  outportb(PIT_CHANNEL_2,counter >> 8);

  //Connect the speaker to the PIT }
  
  outportb(SPEAKER_PORT,inportb(SPEAKER_PORT)|3);
}



char lowcase(char str)
{
    if ((str>='a')&&(str<='z'))
       return str-32;
    else
       return 0;
}

char upcase(char str)
{
    if ((str>='a')&&(str<='z'))
       return str-32;
    else
       return 0;
}


void upcase(char *str)
{
    while (*str!=0)
    {
        if ((*str>='a')&&(*str<='z'))
           *str-=32;
        str++;
    }
}

void lowcase(char *str)
{
    while (*str!=0)
    {
        if ((*str>='A')&&(*str<='Z'))
           *str+=32;
        str++;
    }
}


void initlog(void (*logmessagefunc)(void* data,const char *format,...))
{
    logmessage=logmessagefunc;    
}


/*

void outconsole(char *str,INT32 length)
{
    GFX_INITIALIZED=1;    
   static char buf[255];
   outtextxy(0,ypos+=15,str,length);
   
   if (length!=-1)
   {
       memcpy(buf,str,length);
       buf[length]='\n';
       //fprintf(logfile,buf);
   }else
   {
       sprintf(buf,"%s\n",str);
       //fprintf(logfile,buf);
   }               
   if (ypos>SC.size.y)
   {
      getch();
      cleardevice();
      ypos=0;
   }
}*/





inline void PrepareRegisters (void)
{
  memset(&sregs,0,sizeof(sregs));
  memset(&regs,0,sizeof(regs));
}

void *realtolinear(UINT32 realAddr)
{
    return (void *) (((realAddr & ~0xFFFFL) >> 12) +
                          (realAddr & 0xFFFF));
}

/* LinearToReal -- convert linear address to real mode */
/* (returned as ulong), returns 0 if not in first megabyte */


UINT32 lineartoreal(void *linear)
{
   if ((UINT32) linear > 0x100000)
       return 0;          /* not in first megabyte */
   return ((((UINT32) linear >> 4) & 0xFFFF) << 16) +
           ((UINT32) linear & 0xFL);
}

//real mode irq
void RMIRQ(char irq,rminfo *RMI)
{
  PrepareRegisters();
  regs.w.ax =  0x0300;               // Simulate Real-Mode interrupt
  regs.h.bl =  irq;
  sregs.es =   FP_SEG(RMI);
  regs.x.edi = FP_OFF(RMI);
  int386x(0x31, &regs, &regs, &sregs);
}

//allocate dos memory, loads segment and selector into 'p' and returns the linear address

char *DPMI_AllocDOSMem (short int paras,DPMI_PTR &p)
{
  /* DPMI call 100h allocates DOS memory */
  PrepareRegisters();
  regs.w.ax=0x0100;
  regs.w.bx=paras;
  int386x( 0x31, &regs, &regs, &sregs);
  //check carry flag for failure: not enough dos mem
  if (regs.x.cflag) return NULL;
  p.segment=regs.w.ax;
  p.selector=regs.w.dx;
  p.size=paras;
  //return protected mode pointer
  return (char *)((long)regs.w.ax << 4);
}

BOOL DPMI_FreeDOSMem (DPMI_PTR &p)
{
  /* DPMI call 101h free DOS memory */
  PrepareRegisters();
  regs.w.ax=0x0101;
  regs.w.dx=p.selector;
  int386x( 0x31, &regs, &regs, &sregs);
  return !regs.x.cflag;
}

//map an area of physical memory to a linear pointer

void * DPMI_MAP_PHYSICAL (void *p, UINT32 size)
{
  /* DPMI call 800h map physical memory*/
  PrepareRegisters();
  regs.w.ax=0x0800;
  regs.w.bx=(UINT16) (((UINT32)p)>>16);
  regs.w.cx=(UINT16) (((UINT32)p)&0xffff);
  regs.w.si=(UINT16) (((UINT32)size)>>16);
  regs.w.di=(UINT16) (((UINT32)size)&0xffff);
  int386x( 0x31, &regs, &regs, &sregs);
  return (void *) ((regs.w.bx << 16) + regs.w.cx);  
}

//unmap an area of physical memory to a linear pointer

void DPMI_UNMAP_PHYSICAL (void *p)
{
  /* DPMI call 800h map physical memory*/
  PrepareRegisters();
  regs.w.ax=0x0801;
  regs.w.bx=(UINT16) (((UINT32)p)>>16);
  regs.w.cx=(UINT16) (((UINT32)p)&0xffff);
  int386x( 0x31, &regs, &regs, &sregs);
}

//asm versions, use when you're sure they don't destablize
 extern char *_MapLinear(UINT32 ptr,UINT32 size);
#pragma aux _MapLinear = \
"mov     ecx, ebx"\
"shr     ebx, 16"\ 
"mov     esi, 0x3FFFFF"\
"mov     edi, esi"\
"shr     esi, 16"\
"mov     ax, 0800h"\
"int     031h"\
"shl     ebx, 16"\
"mov     bx, cx"\
"mov     eax, ebx"\
 parm [ebx] [esi]\
 modify [ecx eax edi]

extern void _UnMaplinear(UINT32 ptr);
#pragma aux _UnMapLinear = \
        "mov     ax, 0801h"\
        "mov     ecx, ebx"\
        "shr     ebx, 16"\
        "int     031h"\
        parm [ebx]\
        modify[ax ebx ecx]

    
char *makepath(char *outtext,char *intext)
{
    if (outtext==NULL)
      outtext=intext;
    if (outtext!=intext)
      strcpy(outtext,intext);
      

      
    char *cchar=outtext;
    char *lastslash=NULL;
    while (*cchar!=0)
    {
        if (*cchar=='\\')
            lastslash=cchar+1;
        cchar++;
    }
    if (lastslash!=NULL)
       *lastslash=0;
    return outtext;
}                

BOOL fexist(char *filename)
{

    FILE *f;
    if (!(f=fopen(filename,"r")))
    {
        fclose(f);
        return FALSE;
    }else
    {
        fclose(f);
        return TRUE;
    }     
}

int fgetsize(char *path)
{
   find_t findstruct;
   UINT32 doserror=_dos_findfirst(path,0xFF,&findstruct);
   return findstruct.size;    
}

    

