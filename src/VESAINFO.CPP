#include <string.h>
#include <conio.h>
#include <stdio.h>
#include "vesainfo.hpp"
#include "strtable.hpp"
#include "dosstuff.hpp"

//#define FORCE_BANKED

rminfo RMI;

char vesasig[4]={'V','E','S','A'};
char VBE2SIGNATURE[4]={'V','B','E','2'};
char VBE3SIGNATURE[4]={'V','B','E','3'};

/*
 * globals -
 */


//the mode block is global so that further calls can be made to it
//log file
FILE *f;
void *winfuncptr;
DPMI_PTR modeblock;
infoblocktype *mblk=NULL;
DPMI_PTR headerblock;
headerblocktype *hblk=NULL;
BOOL lfb;

//headerblocktype vesaheader;
//infoblocktype mblk->;
 
char vesa_getinfo()
{                                
        if (hblk==NULL)return FALSE;
        memset (hblk, 0, sizeof (headerblocktype));
        memcpy (hblk, VBE2SIGNATURE, 4);
        memset(&RMI, 0, sizeof(RMI));        
        RMI.EAX=0x00004f00; 
        RMI.ES=headerblock.segment;     
        RMI.EDI=0;        
        RMIRQ(0x10,&RMI);
        if (memcmp(hblk,vesasig,4))
        {
            ERR_LEVEL=ERR_NOVESA;
            DPMI_FreeDOSMem(headerblock);
            return FALSE;
        }                           
        hblk->OEMStringPtr=(char*)realtolinear((UINT32)hblk->OEMStringPtr);
        hblk->VideoModePtr=(UINT16*)realtolinear((UINT32)hblk->VideoModePtr);
        hblk->OemVendorNamePtr=(char*)realtolinear((UINT32)hblk->OemVendorNamePtr);
        hblk->OemProductNamePtr=(char*)realtolinear((UINT32)hblk->OemProductNamePtr);
        hblk->OemProductRevPtr=(char*)realtolinear((UINT32)hblk->OemProductRevPtr);
        return TRUE;
}



/*
 *      vesa_getmodeinfo -
 *
 *      Gets the mode info block on a given mode.
 */

char vesa_getmodeinfo(UINT16 mode)
{
    if ((mblk==NULL)||(hblk==NULL))return FALSE;    
    memset(&RMI,0,sizeof(RMI)); 
    RMI.EAX=0x00004f01; 
    RMI.ECX=mode;
    RMI.ES=modeblock.segment;     
    RMI.EDI=0;         
    RMIRQ(0x10,&RMI);
    //block->WinFuncPtr=PtrToLinear((UINT32)block->WinFuncPtr);
    return 1;
}


/*
 *      vesa_setmode -
 *
 *      Set a SVGA/VGA video mode. If the most significant bit of
 *      'mode' is set, video memory is not cleared.
 */

extern UINT16 _setmode(UINT16 mode);
#pragma aux _setmode = \
"and ebx,0xffff"\
"mov ax,0x4f02"\
"int 0x10"\
parm [bx]


extern UINT16 _vgasetmode(UINT16 mode);
#pragma aux _vgasetmode = \
"int 0x10"\
parm [ax]

//RMI.EBX=(Mode & 0x01ff)|(1<<14)|(1<<15)


extern UINT16 getmode();
#pragma aux getmode = \
"mov ax,0x4f03"\
"int 0x10"\
"mov ax,bx"\
modify [ax bx]


/*
 *      vesa_setwindow -
 *
 *      CPU video memory window control.
 *      'window' = 0 means window A
 *             = 1 means window B
 *      'position' is in window granuality units
 */
/*
void vesa_setwindow( UINT16 position)
{
    RMI.EAX=0x00004f05; 
    RMI.EBX=0;
    RMI.EDX=position;     
    RMIRQ(0x10,&RMI);
} */    
extern void vesa_setwindow(UINT16 position);
#pragma aux vesa_setwindow=\
"xor ebx,ebx"\
"and edx,0xffff"\
"mov eax,0x4f05"\
"int 0x10"\
parm [dx]\
modify [ebx edx eax]

/*#pragma aux vesa_setwindow=\
"and ecx,0xff"\
"and edx,0xffff"\
"mov eax,0x4f05"\
"call winfuncptr"\
parm [cl][dx]\
modify [ecx edx eax]*/
        
        


/*
 *      vesa_setscanlinelength -
 *
 *      Sets logical scan line length. The function does not return
 *      anything but there are variables in RMI.
 *      RMI.EBX = bytes per scan line
 *      RMI.ECX = pixels per scan line
 *      RMI.EDX = max number of scan lines
 */


extern void vesa_setscanlinelength(UINT16 length);
#pragma aux vesa_setscanlinelength=\
"mov ax,0x4f06"\
"xor bx,bx"\
"int 0x10"\
parm [bx]



/*
 *      vesa_getscanlinelength -
 *
 *      Gets logical scan line length. 
 *      Returns pixels per scan line, other variables in RMI.
  *      RMI.EBX = bytes per scan line
 *      RMI.ECX = pixels per scan line
 *      RMI.EDX = max number of scan lines
 Timmy version:
 ax=pixels per scan line
 eax << 16 = max number of scan lines

 */
 

extern UINT32 vesa_getscanlinedimensions();
#pragma aux vesa_getscanlinedimensions=\
"mov ax,0x4f06"\
"mov bx,1"\
"int 0x10"\
"mov eax,edx"\
"shl eax,16"\
"mov ax,cx"\
parm [cx]



/*
 *      vesa_setscreenpos -
 *
 *      Sets display start.  
 *      In:     x - first displayed pixel in scan line
 *              y - first displayed scan line
 */
extern void vesa_setscreenpos(short x, short y);
#pragma aux vesa_setscreenpos=\
"mov ax,0x4f07"\
"xor bx,bx"\
"int 0x10"\
parm [cx][dx]

/*
 *      vesa_getscreenpos -
 *
 *      Gets display start.  
 *      Out:    x - first displayed pixel in scan line
 *              y - first displayed scan line
 */

extern void vesa_getscreenpos(short *x, short *y);
#pragma aux vesa_getscreenpos=\
"mov ax,0x4f07"\
"mov bx,1"\
"int 0x10"\
"mov [edi],cx"\
"mov [esi],dx"\
parm [edi][esi]






char *Types[8] = {"Text","CGA","Hercules","Planar","Packed Pixel","Unchained","Directcolor","YUV"};

void vesa_printavailmodes()
{
  int scroll;
  if ((hblk==NULL)||(mblk==NULL))return;
  //cleartextscreen();
  scroll=0;
  //printf ("Mode-Number  XRes   YRes  BPP  RGB   Type         Pages lfb BankSize\n");
  //printf ("------------------------------------------------------------------------------\n");
  fprintf (f,"Mode-Number  XRes   YRes  BPP  RGB   Type         Pages lfb BankSize\n");
  fprintf (f,"------------------------------------------------------------------------------\n");
  for (UINT16 mode=0; hblk->VideoModePtr[mode]!=0xffff; mode++)
  {
    vesa_getmodeinfo(hblk->VideoModePtr[mode]);
    printf ("0x%3x        %4i   %4i  %2i   %hi%hi%hi   %-12s %2hi   %-5s  %u\n",
       hblk->VideoModePtr[mode],
       mblk->XResolution,
       mblk->YResolution,
       mblk->BitsPerPixel,
       mblk->RedMaskSize,
       mblk->GreenMaskSize,
       mblk->BlueMaskSize,
       Types[mblk->MemoryModel],
       mblk->NumberOfImagePages,
       noyesstr[(mblk->ModeAttributes&VESAMODEATTRIB_LINEARBUFFER)?1:0],
       mblk->WinSize*1024);
    /*if (f!=NULL)
    fprintf (f,"0x%3x        %4i   %4i  %2i   %hi%hi%hi   %-12s %2hi   %-5s  %u\n",
       hblk->VideoModePtr[mode],
       mblk->XResolution,
       mblk->YResolution,
       mblk->BitsPerPixel,
       mblk->RedMaskSize,
       mblk->GreenMaskSize,
       mblk->BlueMaskSize,
       Types[mblk->MemoryModel],
       mblk->NumberOfImagePages,
       noyesstr[(mblk->ModeAttributes&VESAMODEATTRIB_LINEARBUFFER)?1:0],
       mblk->WinSize*1024);

    /*if (scroll++ == 20)
    {
      getch();
      scroll=0;
      cleartextscreen();
      printf ("Mode-Number  XRes   YRes  BPP  RGB   Type         Pages\n lfb\n");
      printf ("------------------------------------------------------------------------------\n");
    }*/
  }
}



UINT16 vesa_getmode(screenIDType &sid)
{
    if ((hblk==NULL)||(mblk==NULL))return NULL;
     /*fprintf(f,"OEM string: %s\nOEM vendor: %s\nOEM product: %s\nOEM product rev: %s\n",
      hblk->OEMStringPtr,
      hblk->OemVendorNamePtr,
      hblk->OemProductNamePtr,
      hblk->OemProductRevPtr);*/
    for (UINT16 mode=0; hblk->VideoModePtr[mode]!=0xffff; mode++)
    {
        mode&=0x1ff;
       vesa_getmodeinfo(hblk->VideoModePtr[mode]);
       // look for 16 bit modes which are only 15 bits wide and correct it
       if  ((mblk->BitsPerPixel == 16) &&
           ((mblk->RedMaskSize +
             mblk->GreenMaskSize +
             mblk->BlueMaskSize) != 16)) mblk->BitsPerPixel = 15;
                          
       if (
          (sid.size.x==mblk->XResolution)&&
          (sid.size.y==mblk->YResolution)&&
          (sid.getbpp()==mblk->BitsPerPixel)&&
          /*(sid.bitplanes==mblk->NumberOfPlanes)&&*/
          (
            (mblk->MemoryModel!=7)||
            (sid.rsize==mblk->RedMaskSize)&&
            (sid.gsize==mblk->GreenMaskSize)&&
            (sid.bsize==mblk->BlueMaskSize)
          )
          ) return hblk->VideoModePtr[mode];
    }                    
    return 0;                   
}    
#define MODE_LINEARBUFFER (1<<14)
#define MODE_NOCLEAR (1<<15)

char vesa_setmode(screenIDType &sid)
{
      hblk=(headerblocktype*)DPMI_AllocDOSMem(512/16, headerblock);
      mblk=(infoblocktype*)DPMI_AllocDOSMem (256/16, modeblock);

      if ((hblk==NULL)||(mblk==NULL))
      {
          ERR_LEVEL=ERR_LOWDOSMEM;
          return FALSE;
      }                
       
      if (!vesa_getinfo()) return 0;       

      /*printf("vesa sig %c%c%c%c\n",hblk[0],
                                     hblk[1],
                                     hblk[2],
                                     hblk[3]);
      printf("vesa version %i%i\n",hblk[4],
                                     hblk[5]);*/
      //char buf[255];
      //sprintf(buf,"%svesalog.txt",EXE_PATH);
      //f=fopen(buf,"w");                     
      //vesa_printavailmodes();
      //fclose(f);

      UINT16 mode=vesa_getmode(sid);

      if (mode==0)
      {
          ERR_LEVEL=ERR_MODENOTAVAIL;
          return FALSE;
      }          
      if (!vesa_getmodeinfo(mode))
      {
          ERR_LEVEL=ERR_MODENOTAVAIL;
          return FALSE;
      }
#ifdef FORCE_BANKED
      lfb=0;
#else
      lfb=((mblk->ModeAttributes&VESAMODEATTRIB_LINEARBUFFER)!=0);
#endif
      
      mode&=0x1ff;
      if (lfb)
        mode|=MODE_LINEARBUFFER;
      else
      {
        mode&=~MODE_LINEARBUFFER;
        return 0;        
      }
      mode|=MODE_NOCLEAR;  
      
      _setmode(mode);        
      if (lfb)
      {      
          SC.scrptr=(char*)DPMI_MAP_PHYSICAL((void*)mblk->PhysBasePtr,(long)(hblk->TotalMemory)*64*1024);
      }else
      {
          SC.scrptr=(char*)0xA0000;
      }
      winfuncptr=realtolinear(mblk->WinFuncPtr);      

      //setup rgblookup table                              
      SC.offsd=mblk->BytesPerScanLine;
      SC.rsize=mblk->RedMaskSize;
      SC.gsize=mblk->GreenMaskSize;
      SC.bsize=mblk->BlueMaskSize;
      SC.rpos=mblk->RedFieldPosition;
      SC.gpos=mblk->GreenFieldPosition;
      SC.bpos=mblk->BlueFieldPosition;
      SC.maskr=(0xff >> 8-SC.rsize)<<SC.rpos;
      SC.maskg=(0xff >> 8-SC.gsize)<<SC.gpos;
      SC.maskb=(0xff >> 8-SC.bsize)<<SC.bpos;                                                            
      return (1);
}


void vesa_copyscreen(char *dest,char *src,UINT32 size)
{
    UINT32 banksize=mblk->WinSize*1024;
    UINT16 windowcount=size/banksize;
    UINT16 count;
    
    for(count=0;count<=windowcount;count++)
    {
        vesa_setwindow(count);
        memcpy(dest,src,banksize);
        src+=banksize;                
    }
}    



void vesa_close()
{

     //unmap frame buffer
     DPMI_UNMAP_PHYSICAL((void*)SC.scrptr);
     //deallocate dos memory
     DPMI_FreeDOSMem(modeblock);
     DPMI_FreeDOSMem(headerblock);
     hblk=NULL;
     mblk=NULL;
}





