#include "stdafx.h"
#include <string.h>
#include <stdio.h>
#include <conio.h>
#include "gbasics.hpp"
#include "svga64k.hpp"
#include "vga256.hpp"
//#include "svga16.hpp"
//#include "svga32k.hpp"
#include "vesainfo.hpp"
#include "strtable.hpp"

char GFX_INITIALIZED=0;

void setup_displayroutines(UINT32 driver)
{
    switch (driver & BMFformat)
    {
        case STsvga_flat64k:
        case STsvga_flat32k:
            setmode=svga64k_setmode;
            closemode=vesa_close;
        break;
        case STsvga_flat:
            setmode=vga256_setmode;
            closemode=vga256_close;
        break;
        case STsvga_plainiar:
        break;
    }
}    

void setup_inputroutines(UINT32 driver)
{
    switch (driver & BMFformat)
    {
        case STsvga_flat64k:
        case STsvga_flat32k:
        getbitmap=svga64k_getbitmap;
        getpixel=svga64k_getpixel;
        getrgbpixel=svga64k_getrgbpixel;
        break;
        case STsvga_flat:
        getpixel=vga256_getpixel;
        break;
        case STsvga_plainiar:
        break;
    }                    
}

void setup_outputroutines(UINT32 driver)
{
    switch (driver & BMFformat)
    {
        case STsvga_flat64k:
        case STsvga_flat32k:
        getcolour=svga64k_getcolour;        
        putpixel=svga64k_putpixel;
        hline=svga64k_hline;
        vline=svga64k_vline;
        line=svga64k_line;
        bar=svga64k_bar;        
        tri=svga64k_tri;        
        putbitmap_base=svga64k_putbitmap;        
        circle=svga64k_circle;
        outtextxy_base=svga64k_outtextxy; 
        drawbytes=svga64k_drawbytes;
        cleardevice=svga64k_cleardevice;                        
        break;        
        case STsvga_flat:
        //getcolour=vga256_getcolour;
        putpixel=vga256_putpixel;
        hline=vga256_hline;
        vline=vga256_vline;
        line=vga256_line;
        bar=vga256_bar;        
        //tri=svga64k_tri;
        putbitmap_base=vga256_putbitmap;
        circle=vga256_circle;
        outtextxy_base=vga256_outtextxy; 
        drawbytes=vga256_drawbytes;
        cleardevice=vga256_cleardevice;
        getcolour=vga256_getcolour;           
        break;
        case STsvga_plainiar:

        break;
    }
}
    
void setinput(screenDataType *sc)
{
    IC=sc?SC=*sc:SC_BASE;
    setup_inputroutines(IC.driver);
}    

void setoutput(screenDataType *sc)
{
    SC=sc?SC=*sc:SC_BASE;
    setup_outputroutines(SC.driver);
}    

  
char initgraph(screenIDType &sID)
{
  
    INT16 i;
    UINT32 coffset;
    UINT32 *coffsettableptr;
    ERR_LEVEL=ERR_NOERROR;
    
    
    SC.driver=sID.driver;
    //SC.colourdepth=sID.colourdepth;
    SC.size=sID.size;

    SC.viewport.rassign(0,0,SC.size.x-1,SC.size.y-1);
    

    setup_displayroutines(SC.driver);
    setup_inputroutines(SC.driver);
    setup_outputroutines(SC.driver);
    

    COLinit(SC.colourdepth);
    
            
    if (!setmode(sID))
    {
        printf("Error encountered initializing graphics driver:\n%s\n",ERR_STRINGS[ERR_LEVEL]);
        ERR_LEVEL=0;
        return 0;
    }

    SC.pagesize=SC.offsd*SC.size.y;
    if (!(SC.startoffsets=new UINT32[SC.size.y]) )return 0;
    coffset=(UINT32)SC.scrptr;
    coffsettableptr=SC.startoffsets;
    for(i=0;i<SC.size.y;i++)
    {
        *coffsettableptr=coffset;
        coffset+=SC.offsd;
        coffsettableptr++;
    }
    t_col=getcolour(255,255,255);
    t_fillcol=getcolour(0,0,0);
    t_chroma=t_fillcol;
    //t_writemode=
    t_linepattern=0xff;
    t_palette=NULL;
    t_nativepalette=NULL;    
    
    SC_BASE=SC;
    IC=SC;
    GFX_INITIALIZED=1;    
    return 1;

}
//For use with that well written obj file I found.....
/*
  VBEINFO *vbeInfo;
  if( (vbeInfo = vbeDetect()) == NULL )
  {
    printf("Can't find VESA bios extentions\n");
    exit(0);
  }



  if( vbeInfo->VbeVersion < 0x200 )
  {
    printf("VESA bios extention 2.0 or better needed\n");
    exit(0);
  }

  myScreen = vbeOpen(640,480,8);
  SC.scrptr=(char *)myScreen->lfb;
    VBESURFACE * myScreen;    
*/


extern _setmode(UINT16 mode);
#pragma aux _setmode=\
"int 0x10"\
parm [ax]

void closegraph()
{
    delete SC.startoffsets;
    closemode();
    COLdone();
    GFX_INITIALIZED=0;    
    _setmode(0x3);
}

extern void _waitvbl();
#pragma aux _waitvbl=\
  "mov dx,0x3da"\
  "runloop:"\
     "in al,dx"\
    "test al,08h"\      
  "jz runloop"

void waitvbl()
{
    _waitvbl();
}


/*
procedure setlinepattern(pattern:byte;background:boolean);assembler;
asm
  mov al,pattern
  mov ah,al
  mov maskword,ax
  mov al,background
  mov maskbackground,al
end;

procedure setfillpattern(const pattern:fillpatterntype;background:boolean);
var i:byte;
begin
  fillpattern:=pattern;
  nullpattern:=true;
  i:=0;
  while (nullpattern=true)and(i<8) do begin
    nullpattern:=solidfill[i]=pattern[i];
    inc(i);
  end;
  maskbackground:=background;
end;

procedure setsolidfill;
begin
  maskbackground:=false;
  nullpattern:=true;
  maskword:=$ffff;
end;


Procedure pageswap;begin
  visual:=visual xor 1;active:=active xor 1;
  setactivepage(active);setvisualpage(visual);
end;

Procedure setoffset(offst:byte);assembler;
asm
  mov dx,CRTC_ADDR         {load dx with CRT controller address}
  mov al,$1                {function $1}
  mov ah,offst             {screen offset}
  out dx,ax                {write to port}

  mov al,$13               {funtion $13}
  out dx,ax                {write to port}

  mov al,0                 {function 0}
  shl ah,1                 {half offset}
  out dx,ax                {write to port}

  mov byte ptr SC.offsd,ah              {set my offset value}
end;

procedure mapmask(mask:byte);assembler;
asm
  mov dx,SEQU_ADDR;             {load Sequencer}
  mov al,2; mov ah,mask         {function 2, mask}
  out dx,ax                     {write to port}
end;

procedure readmask(plane:byte);assembler;
asm
  mov dx,GRPH_ADDR              {Load graphics address}
  mov al,4                      {function 4-set readmask}
  mov ah,plane                  {Planes to mask}
  out dx,ax                     {write to port}
end;

Procedure Setvisualpage(pageno:byte);   {set page that is displayed}
begin
  setbase(pageno*SC.pagesize);
end;

Procedure Setactivepage(pageno:byte);
begin
  SC.scrptr:=ptr($a000,pageno*SC.pagesize);
end;

Procedure memorymode(mask:byte);assembler;
asm
  mov dx,SEQU_ADDR               {write to sequencer}
  mov al,4                       {function 4-memory modes}
  mov ah,mask                    {set up mask}
  out dx,ax                      {write to port}
end;

{address mode control the way in which data is written to the screen.
3=No bitmask needed.
1=use bitmask.
0=32 bit screen to screen transfer.
}
Procedure adrsmode(mask:byte);assembler;
asm
  mov dx,GRPH_ADDR               {Graphics address}
  mov al,5;                      {function 5, address mode}
  mov ah,mask;                   {load mode}
  out dx,ax                      {write to port}
end;

Procedure Setbase(scrnbase:word);assembler;
asm
  mov dx,CRTC_ADDR               {CRT controller}
  mov bx,scrnbase                {base of screen}
  mov al,$C ;mov ah,bh           {load up low bits}
  out dx,ax                      {write to port}
  inc al    ;mov ah,bl           {load up high bits}
  out dx,ax                      {write to port}
end;


{wait for vertiacal blank}
Procedure waitvbl;assembler;
Asm
  mov dx,STATUS_ADDR   {load status address}
  @runloop:
    in al,dx           {get status}
    test al,08h        {is it equal to al?}
  jz @runloop          {if not then loop}
end;

procedure setcurs(x,y:byte);assembler;
  asm
    mov ah,02h
    mov bh,0
    mov dh,x
    mov dl,y
    int 10h
  end;

procedure getcurs(var x,y:byte);assembler;
asm
  mov ah,03h
  mov bh,0
  int 10h
  les di,x
  mov es:[di],dh
  les di,y
  mov es:[di],dl
end;
*/

   //fillchar(tcharset,255*fontheight,0);

    /*
    {get address of 8 pixel high font}
    mov ah,$35
    mov al,fontintr
    int $21
    {correct vector, subtract a k}
    sub bx,1024

    {get tcharset pointer}
    lea si,tcharset

    {inititiate a loop}
    mov cx,256
    @copyloop:
      push cx
      mov cx,8
      @byteloop:
        mov al,es:[bx]
        mov ds:[si+(fontheight shr 1)-4],al
        inc bx
        inc si
      loop @byteloop
      add si,fontheight shr 1
      pop cx
      loop @copyloop*/




