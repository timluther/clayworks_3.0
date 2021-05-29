#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vga256.hpp"
#include "ttypes.hpp"
#include "vesainfo.hpp"
#include "lowlev.hpp"

trgba hwpal[256];

char vga256_setmode(screenIDType &sid)
{
    if ((sid.size.x==320)&&(sid.size.y==200))
    {
    _asm
    {
        mov ax,0x13;
        int 0x10;      
    }
    SC.offsd=SC.size.x;
    SC.scrptr=(char*)0xA0000;
    }
    else
       vesa_setmode(sid);
    SC.rgbpalette=hwpal;
    t_maxcolours=256;
    COLgethardwarepal(hwpal);                    
    return 1;
}

void vga256_close()
{
}


tcolour vga256_getcolour(UINT8 r,UINT8 g,UINT8 b)
{
  INT32 currcol;  
  INT32 diff,match;
  tcolour retcol;
  retcol.c8=0;
  currcol=0;
  match=32000;
  
  if (!SC.rgbpalette) return retcol;  
  while (currcol<t_maxcolours)
  { 
      if ((SC.rgbpalette[currcol].r==r)&&(SC.rgbpalette[currcol].g==g)&&(SC.rgbpalette[currcol].b==b))
      {
          retcol.c8=currcol;
          return retcol;
      }
      diff=abs(g-SC.rgbpalette[currcol].g)*gwt;
      if (diff<match)
      {
         diff+=(abs(r-SC.rgbpalette[currcol].r)*rwt+abs(b-SC.rgbpalette[currcol].b)*bwt);
         if (diff<match)
         {        
             match=diff;
             retcol.c8=currcol;
         }
      }    
      currcol++;
  }
  return retcol;  
}


void vga256_putpixel(PIXINT x,PIXINT y)
{
   /* _asm
   {
   cmp ax,word ptr SC+OFFbx
   jg fin
   cmp ax,word ptr SC+OFFax
   jl fin
   cmp dx,word ptr SC+OFFby
   jg fin
   cmp dx,word ptr SC+OFFay
   jl fin
               
   mov edi,DWORD PTR SC+OFFstartoffsets //load up offsets pointer
   mov edi,[edi+edx*4]                  //get yoffset
   mov dl,byte ptr t_col                //loadup colour
   mov [edi+eax],dl
   fin:
   }*/
   if (!SC.viewport.rcontains(x,y)) return;
   *(UINT8*)(SC.startoffsets[y]+x)=t_col.c8;
}

tcolour vga256_getpixel(PIXINT x,PIXINT y)
{        
   return *(tcolour*)(IC.startoffsets[y]+x);  
}

void extern _hline(UINT8 *adrs,UINT32 cx,UINT8 col);
#pragma aux _hline=\
"mov bh,bl"\
"mov ax,bx"\
"shl eax,16"\
"mov ax,bx"\
"mov bl,cl"\
"and bl,3"\
"shr ecx,2"\
"rep stosd"\
"mov cl,bl"\
"rep stosb"\
parm[edi][ecx][bl]

void extern _hlinexor(UINT8 *adrs,UINT32 cx,UINT8 col);
#pragma aux _hlinexor=\
"mov bh,bl"\
"mov ax,bx"\
"shl eax,16"\
"mov ax,bx"\
"mov bl,cl"\
"and bl,3"\
"shr cx,2"\
"drawloop:"\
"  xor [edi],eax"\
"  add edi,4"\
"  dec cx"\
"jnz drawloop"\
"mov cl,bl"\
"jz end"\
"xor [edi],al"\
"dec cl"\
"jz end"\
"xor [edi],al"\
"dec cl"\
"jz end"\
"xor [edi],al"\
"end:"\
parm[edi][ecx][bl]



void vga256_hline(PIXINT x1,PIXINT x2,PIXINT y)
{
    PIXINT xlen;
    if (x1>x2) swap(x2,x1);
    if ((y<SC.viewport.a.y)||(y>SC.viewport.b.y)) return;
    if (x1<SC.viewport.a.x) x1=SC.viewport.a.x;
    if (x2>SC.viewport.b.x) x2=SC.viewport.b.x;
    if (x2<=x1) return;
    
    UINT8 *adrs=(UINT8*)(SC.startoffsets[y]+x1);
    xlen=1+x2-x1;
    _hline(adrs,xlen,t_col.c8);
    //memset(adrs,t_col,xlen);
    /*for (xlen=x1;xlen<x2;xlen++)
    {
        *adrs=t_col;
        adrs++;
    } */   
      
    //memfill(adrs,x2-x1,t_col);
}

void extern _vline(UINT8 *adrs,UINT32 offs,UINT32 cy);
#pragma aux _vline=\
"mov al,byte ptr t_col"\
"_yloop:"\
"  mov [edi],al"\
"  add edi,edx"\
"  dec cx"\
"jnz _yloop"\
parm [edi ][edx][ecx]\
modify [al]

void extern _vline_xor(UINT8 *adrs,UINT32 offs,UINT32 cy);
#pragma aux _vlinexor=\
"mov al,byte ptr t_col"\
"_yloop:"\
"  xor [edi],al"\
"  add edi,edx"\
"  dec cx"\
"jnz _yloop"\
parm [edi ][edx][ cx]\
modify [al]


void extern _vline_or(UINT8 *adrs,UINT32 offs,UINT32 cy);
#pragma aux _vline_or=\
"mov al,byte ptr t_col"\
"_yloop:"\
"  or [edi],al"\
"  add edi,edx"\
"  dec cx"\
"jnz _yloop"\
parm [edi ][edx][ cx][al]\
modify [al]


void vga256_vline(PIXINT x,PIXINT y1,PIXINT y2)
{
    PIXINT ylen;
    if (y1>y2) swap(y2,y1);
    if ((x<SC.viewport.a.x)||(x>SC.viewport.b.x)) return;
    if (y1<SC.viewport.a.y) y1=SC.viewport.a.y;
    if (y2>SC.viewport.b.y) y2=SC.viewport.b.y;
    if (y2<=y1) return;
    
    UINT8 *adrs=(UINT8*)(SC.startoffsets[y1]+x);
    ylen=(y2-y1)+1;
    _vline(adrs,SC.offsd,ylen);
    /*for (ylen=y1;ylen<=y2;ylen++)
    {
        *adrs=t_col;
        adrs+=SC.offsd;
    } */
}
    

    

extern void xmajorline();
#pragma aux xmajorline=\
   "mov edx,addy"\
   "mov ebx,edx"\
   "shr bx,1"\
   "mov al,byte ptr t_col"\
   "mov esi,linexdiff"\
   "mov edi,sadrs"\
   "stosb"\
   "xor ecx,ecx"\
   "@xloop:"\
     "add ebx,edx"\
     "jnc @nodraw"\
     "rep stosb"\
     "add edi,offs"\
     "@nodraw:"\
     "inc ecx"\
     "dec esi"\
   "jnz @xloop"\
   modify [edx ebx al esi edi]

extern void ymajorline1();
#pragma aux ymajorline1=\
  "mov edx,addy"\
  "mov ebx,edx"\
  "shr ebx,1"\
  "mov al,byte ptr t_col"\
  "mov esi,offs"\
  "mov edi,sadrs"\
  "mov ecx,lineydiff"\
  "@yloop:"\
    "mov [edi],al"\
    "add ebx,edx"\
    "sbb edi,esi"\
    "dec ecx"\
  "jnz @yloop"\
  modify [edx ebx al esi edi ecx]
  


extern void ymajorline2();
#pragma aux ymajorline2=\
  "mov edx,addy"\
  "mov ebx,edx"\
  "shr ebx,1"\
  "mov al,byte ptr t_col"\
  "mov esi,offs"\
  "mov edi,sadrs"\
  "mov ecx,lineydiff"\
  "@yloop:"\
    "mov [edi],al"\
    "add ebx,edx"\
    "adc edi,esi"\
    "dec ecx"\
  "jnz @yloop"\
  modify [edx ebx al esi edi]



extern void vert_or_diagline();
#pragma aux vert_or_diagline=\
  "mov al,byte ptr t_col"\
  "mov esi,offs"\
  "mov edi,sadrs"\
  "mov ecx,linexdiff"\
  "@yloop:"\
    "mov [edi],al"\
    "add edi,esi"\
    "dec ecx"\
  "jnz @yloop"\
  "stosb"\                
  modify [al esi edi cx]      



        
void vga256_line(PIXINT x1,PIXINT y1,PIXINT x2,PIXINT y2)
{    
    INT32 grad;
    
    if (!SC.viewport.clipline(x1,y1,x2,y2)) return;
    
    linexdiff=abs(x2-x1);
    lineydiff=abs(y2-y1);
    if (linexdiff>lineydiff)
    {
        if (x1>x2)
        {
            swap(x1,x2);
            swap(y1,y2);
        }    
        sadrs=(UINT8*)(SC.startoffsets[y1]+x1); 

        if (y2>y1)
            offs=SC.offsd;
        else
            offs=-SC.offsd;
        lineydiff++;
        linexdiff++;      
        grad=(lineydiff << 16)/ linexdiff;
        addy=grad;
        xmajorline();
              
    } else if (lineydiff>linexdiff)
    {
        if (y1>y2)
        {
            swap(x1,x2);
            swap(y1,y2);
        }
        sadrs=(UINT8*)(SC.startoffsets[y1]+x1); 
        lineydiff++;
        linexdiff++;
      
        grad=(linexdiff << 16)/ lineydiff;
        addy=grad;
     
        if (x1>x2)
        {
            offs=-SC.offsd;
            ymajorline1();
                

        } else
        {
            offs=SC.offsd;
            ymajorline2();


        }
    }
    else if (y1==y2)
    {
        _hline((UINT8*)SC.startoffsets[y1]+x1,1+x2-x1,t_col.c8);
        return;
    }
    
    else
    {
    //diagonals +vertical
        if (y1>y2)
        {
            swap(x1,x2);
            swap(y1,y2);
        }
        sadrs=(UINT8*)(SC.startoffsets[y1]+x1); 
      
        if (x1==x2)
            offs=SC.offsd;
        else if (x2>x1)
            offs=SC.offsd+1;
        else 
            offs=SC.offsd-1;
        vert_or_diagline();
        
        
    }
}
 
UINT32 temp32;
void extern _bar(UINT8 *adrs,UINT32 cx,UINT32 cy);
#pragma aux _bar=\
"mov al,byte ptr t_fillcol"\
"mov ah,al       "\
"mov bx,ax       "\
"shl eax,16      "\
"mov ax,bx       "\
"mov bl,dl       "\
"and bl,3        "\
"xor ecx,ecx     "\
"shr dx,2        "\
"yloop:          "\
"  mov cx,dx     "\
"  rep stosd     "\
"  mov cl,bl     "\
"  rep stosb     "\
"  add edi,temp32"\
"  dec esi        "\
"jnz yloop       "\
parm[edi][edx][esi]

void vga256_bar(PIXINT x1,PIXINT y1,PIXINT x2,PIXINT y2)
{


    PIXINT xlen;
    
    if (y1>y2) swap(y2,y1);
    if (x1>x2) swap(x2,x1);
    
    if ((x1>SC.viewport.b.x)||(x2<SC.viewport.a.x)||
       (y1>SC.viewport.b.y)||(y2<SC.viewport.a.y))return;
        
    if (x1<SC.viewport.a.x) x1=SC.viewport.a.x;
    if (x2>SC.viewport.b.x) x2=SC.viewport.b.x;
    if (y1<SC.viewport.a.y) y1=SC.viewport.a.y;
    if (y2>SC.viewport.b.y) y2=SC.viewport.b.y;
    
    UINT8 *adrs=(UINT8*)(SC.startoffsets[y1]+x1);
    xlen=1+x2-x1;
    y2-=(y1-1);
    temp32=SC.offsd-xlen;
    _bar(adrs,xlen,y2);
    /*for (i=y2;i>=0;i--)
    {
        _hline(adrs,xlen);
        //memset(adrs,t_fillcol,xlen);
        adrs+=SC.offsd;
    }    
      */  
        
    
    
}    


void vga256_putbitmap(PIXINT x,PIXINT y,tbitmap *pic,char useglobalattribs)
{
    PIXINT x2,y2,xlen,ylen,xdiff,ydiff;
    UINT32 mode;
            
    UINT32 picoffset=pic->offsd;
    x2=x+pic->size.x-1;
    y2=y+pic->size.y-1;
    
    if (x<SC.viewport.a.x)
    {
        if (x2<SC.viewport.a.x) return;
        xdiff=SC.viewport.a.x-x; 
        x=SC.viewport.a.x;
    }else
        xdiff=0;
    if (y<SC.viewport.a.y)
    {
        if (y2<SC.viewport.a.y)return;        
        ydiff=SC.viewport.a.y-y; 
        y=SC.viewport.a.y;
    }else
        ydiff=0;
    
    if (x2>SC.viewport.b.x)
    {
        if (x>SC.viewport.b.x) return;
        x2=SC.viewport.b.x;
    }
    if (y2>SC.viewport.b.y)
    {
        if (y>SC.viewport.b.y) return;        
        y2=SC.viewport.b.y;
    }

    xlen=x2-x+1;
    ylen=y2-y+1;         
    sadrs=(char*)(SC.startoffsets[y]+x+x);

    if (useglobalattribs==0)
        t_nativepalette=pic->palette;
    mode = pic->driver | (pic->drawmode << 8);
    padrs=((char*)pic->startoffsets[ydiff]) +xdiff;
    
    
    while (ylen>0)
    {
        memcpy(sadrs,padrs,xlen);
        padrs+=picoffset;
        sadrs+=SC.offsd;
        ylen--;
    }    
}
/*
"mov cl,xlen"\
"mov ch,ylen"\
"mov esi data"\
"mov edi adrs"\
"mov ah,mask"\*/


extern void drawbytes_1bit(char *adrs,char *data,UINT8 xlen,UINT8 ylen,UINT32 offs,UINT8 mask,UINT8 col);
#pragma aux drawbytes_1bit=\
"mov byte ptr temp32,cl"\
"_yloop:"\
"   mov cl,byte ptr temp32"\
"  _xloop:"\
"    mov al,[esi]"\
"    and al,ah"\
"    test al,128"\
"    jz _nodraw1"\
"      mov [edi],bl"\
"    _nodraw1:"\
"    test al,64"\
"    jz _nodraw2"\
"      mov [edi+1],bl"\
"    _nodraw2:"\
"    test al,32"\
"    jz _nodraw3"\
"      mov [edi+2],bl"\
"    _nodraw3:"\
"    test al,16"\
"    jz _nodraw4"\
"      mov [edi+3],bl"\
"    _nodraw4:"\
"    test al,8"\
"    jz _nodraw5"\
"      mov [edi+4],bl"\
"    _nodraw5:"\
"    test al,4"\
"    jz _nodraw6"\
"      mov [edi+5],bl"\
"    _nodraw6:"\
"    test al,2"\
"    jz _nodraw7"\
"      mov [edi+6],bl"\
"    _nodraw7:"\
"    test al,1"\
"    jz _nodraw8"\
"      mov [edi+7],bl"\
"    _nodraw8:"\    
"    inc esi"\
"    add edi,edx"\
"    dec cl"\
"  jnz _xloop"\
"  dec ch"\
"  jnz _yloop"\
   parm [edi][esi][cl][ch][edx][ah][bl]\
   modify [esi edi ax]




/*
inline void drawbytes_1bit(char *adrs,char *data,UINT8 xlen,UINT8 ylen,UINT8 mask)
{
    UINT8 cbyte;
    UINT32 poffs=SC.offsd-xlen;
    //while (ylen>=0)
    UINT8 yi,xi;
    for(yi=0;yi<ylen;yi++)
    {
        //while (xlen>=0)
        for(xi=0;xi<xlen;xi++)
        {
            cbyte=*data & mask;
            data++;
            if ((cbyte & 128)!=0) *adrs=t_col;   adrs++;
            if ((cbyte & 64)!=0)  *adrs=t_col;   adrs++;
            if ((cbyte & 32)!=0)  *adrs=t_col;   adrs++;
            if ((cbyte & 16)!=0)  *adrs=t_col;   adrs++;
            if ((cbyte & 8)!=0)   *adrs=t_col;   adrs++;
            if ((cbyte & 4)!=0)   *adrs=t_col;   adrs++;
            if ((cbyte & 2)!=0)   *adrs=t_col;   adrs++;
            if ((cbyte & 1)!=0)   *adrs=t_col;   adrs++;
            //xlen--;
        }
        adrs+=poffs;
        //ylen--;
   }                     
}*/

inline PIXINT min(PIXINT a,PIXINT b)
{
    return a<b?a:b;
}    

void vga256_circle(PIXINT cx,PIXINT cy,PIXINT radius)
{
}

void vga256_outtextxy(PIXINT cx,PIXINT cy,char *txt,PIXINT slen)
{
    //char *adrs;
    register tfontchar *ch;        
    UINT32 schar=0;

    if (slen<0)
       slen=strlen(txt);

    while (cx<SC.viewport.a.x)
    {
        
        cx+=FNTgetcharwidth(*txt);
        txt++;
        schar++;
        
    }
    if (schar!=0)
    {
        schar--;
        txt--;
        cx-=FNTgetcharwidth(*txt);
    }        

    while (cx<=SC.viewport.b.x)
    {
        if (schar>slen) return;
        ch=&t_charset.offsets[*txt];
        drawbytes(cx,cy,ch->bytewidth,ch->height,ch->data);
        cx+=FNTgetcharwidth(ch);
        txt++;
        schar++;
    }        
}

void vga256_drawbytes(PIXINT x,PIXINT y,UINT8 w,UINT8 h,char *bytes)
{
    PIXINT y2,x2;
    char *adrs;
    PIXINT y_clip=0;
    unsigned char mask;
    
    x2=x+(w<<3);
    y2=y+h;
    if ((y>SC.viewport.b.y)||(y2<=SC.viewport.a.y)||(x>SC.viewport.b.x)||(x2<=SC.viewport.a.x))
      return;

    if (y<SC.viewport.a.y)
    {
        y_clip=abs(SC.viewport.a.y-y);
        y=SC.viewport.a.y;
        bytes+=y_clip*t_charset.xbytesize;
        h-=y_clip;
    }

    if (y2>SC.viewport.b.y)
    {
        y_clip=(y2-SC.viewport.b.y)-1;        
        h-=y_clip;
    }    
    
    mask=0xff;
    if (x<SC.viewport.a.x)
        mask&=(0xff>>(SC.viewport.a.x-x));
    if (x2>SC.viewport.b.x)                                
        mask&=(0xff<<(x2-SC.viewport.b.x-1));

    adrs=((char*)SC.startoffsets[y]+x);
    drawbytes_1bit(adrs,bytes,w,h,SC.offsd,mask,t_col.c8);            
}


void vga256_clearDevice(void)
{
    memset(SC.scrptr,t_fillcol.c8,SC.pagesize);
    
}


