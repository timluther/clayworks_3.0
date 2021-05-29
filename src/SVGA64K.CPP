#include <string.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include "dosstuff.hpp"
#include "ttypes.hpp"
#include "svga64k.hpp"
#include "vesainfo.hpp"
#include "lowlev.hpp"
#include "tmaths.hpp"
#include "fixedp.hpp"

#define shiftcol 13
#define shiftred 19
#define shiftgreen 18
#define shiftblue 19

UINT8 extern _gettexpix8();
#pragma aux _gettexpix8=\
"mov edi,t_texturestartoffsets"\
"xor eax,eax"\
"mov ax,word ptr pv+2"\
"mov edi,[edi+eax*4]"\
"mov ax,word ptr pu+2"\
"mov bl,[edi+eax]"\
modify[ebx edi]
//needs a palette to lookup in

UINT32 extern _gettexpix16();
#pragma aux _gettexpix16=\
"mov edi,t_texturestartoffsets"\
"xor eax,eax"\
"mov eax,dword ptr xd+40"\
"shr eax,16"\
"mov edi,[edi+eax*4]"\
"mov eax,dword ptr xd+32"\
"shr eax,16"\
"mov ax,[edi+eax*2]"\
modify[eax edi]

extern tcolour _getcolour(UINT8 r,UINT8 g,UINT8 b);
#pragma aux _getcolour=\
"shr ax,3"\
"shr dx,2"\
"shl dx,5"\
"or ax,dx"\
"shr cx,3"\
"shl cx,11"\
"or ax,cx"\
"mov cx,ax"\
"ror eax,16"\
"mov ax,cx"\
parm [ecx][edx][eax]\
modify[edi ecx]


extern PIXINT _getcolourint16(UINT8 r,UINT8 g,UINT8 b);
#pragma aux _getcolourint16=\
"shr ax,3"\
"shr dx,2"\
"shl dx,5"\
"or ax,dx"\
"shr cx,3"\
"shl cx,11"\
"or ax,cx"\
"mov cx,ax"\
"ror eax,16"\
"mov ax,cx"\
parm [ecx][edx][eax]\
modify[edi ecx]



void gradit(UINT32 col1,UINT32 col2,UINT32 v1,UINT32 v2)
{
  #define colshift 8
  PIXINT plus,p,step;  
  if (col2<col1) swap(col1,col2);
  step=1+(col2-col1);
  
  plus=((v2-v1)<< colshift)/ step;
    
  p=(v1<<colshift);
  for (;col1<col2;col1++)
  {
        UINT32 rb=(p>>colshift)&255;
        bound0to256_16.r[col1]=conv24to16.r[rb];
        bound0to256_16.g[col1]=conv24to16.g[rb];
        bound0to256_16.b[col1]=conv24to16.b[rb];  
        p+=plus;
  }  
}



   
char svga64k_setmode(screenIDType &sid)
{
    
    if (!vesa_setmode(sid))return FALSE;

    INT32 i;
    svga64k_scanmode(0);
    UINT8 rsize=8-SC.rsize;
    UINT8 gsize=8-SC.gsize;
    UINT8 bsize=8-SC.bsize;
    UINT32 rmax=(255>>rsize)<<SC.rpos;
    UINT32 gmax=(255>>gsize)<<SC.gpos;
    UINT32 bmax=(255>>bsize)<<SC.bpos;
    

    //setup rgb lookup table for 16/15 bit modes
    
    //setup small rgb lookup table for 16/15 bit modes
    for (i=0;i<65536;i++)
    {
       conv16to24[i].r=i>>SC.rpos<<rsize;
       conv16to24[i].g=i>>SC.gpos<<gsize;
       conv16to24[i].b=i>>SC.bpos<<bsize;
       //conv16to24[i].a=0;//i>>SC.bpos<<bsize;              
    }
    //the alpha values are used for transparency
    for (i=0;i<256;i++)
    {
        for (UINT32 i2=0;i2<256;i2++)
        {
            conv16to24[(i<<8) + i2].a=(UINT8)((flt)i2*((flt)i/255.0f));
        }           
    }        
    
    for (i=0;i<256;i++)
    {
       bound0to256_16.r[i]=bound0to256_16.g[i]=bound0to256_16.b[i]=bound0to256[i]=0;       
       conv24to16.r[i]=(i>>rsize)<<SC.rpos;
       conv24to16.g[i]=(i>>gsize)<<SC.gpos;
       conv24to16.b[i]=(i>>bsize)<<SC.bpos;
    }
    int ib;
    for (i=256;i<512;i++)       
    { 
       ib=(i-256);       
       bound0to256_16.r[i]=conv24to16.r[ib];
       bound0to256_16.g[i]=conv24to16.g[ib];
       bound0to256_16.b[i]=conv24to16.b[ib];
       bound0to256[i]=ib;
       conv24to16.r[i]=rmax;
       conv24to16.g[i]=gmax;
       conv24to16.b[i]=bmax;
    }
    
    for (i=512;i<1024;i++)
    {
       bound0to256[i]=255;
       bound0to256_16.r[i]=rmax;
       bound0to256_16.g[i]=gmax;
       bound0to256_16.b[i]=bmax;
    }
    gradit(0,198,0,0);
    gradit(198,340,0,100);
    gradit(340,560,100,255);
    /*
    gradit(748,1023,255,255);*/
    
    
    return TRUE;   
}    



tcolour svga64k_getcolour(UINT8 r,UINT8 g,UINT8 b)
{    
    return _getcolour(r,g,b);
}       
   
tcolour svga64k_getpixel(PIXINT x,PIXINT y)
{
   static tcolour retval;
   if (!IC.viewport.rcontains(x,y)) return retval;
   return *(tcolour*)(IC.startoffsets[y]+(x+x));
}    

trgba svga64k_getrgbpixel(PIXINT x,PIXINT y)
{
   if (!IC.viewport.rcontains(x,y)) return conv16to24[0];
   UINT16 col=*(UINT16*)(IC.startoffsets[y]+(x+x));
   return conv16to24[col];
}        

    /*if (!SC.viewport.rcontains(x,y)) return;
    *((UINT32*)(SC.startoffsets[y]+(x+x)))=t_col.c16;*/

//ax,dx,bx,cx
//



void svga64k_putpixel(register PIXINT x,register PIXINT y)
{
   UINT32 *so=SC.startoffsets;
   if ((x>=SC.viewport.a.x) && (x<=SC.viewport.b.x) && (y>=SC.viewport.a.y) && (y<=SC.viewport.b.y))
      *(UINT16*)(SC.startoffsets[y]+x+x)=t_col.c16;
/*
   _asm
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
   mov dx,word ptr t_col                //loadup colour
   mov [edi+eax*2],dx
   fin:
   }*/
}               

//needs optimizing!
void extern _hlinexor(char *adrs,UINT32 cx,UINT32 col);
#pragma aux _hlinexor=\
"mov eax,ebx"\
"shl eax,16"\
"mov eax,ebx"\
"drawloop:"\
"  xor [edi],al"\
"  add edi,1"\
"  dec ecx"\
"jnz drawloop"\
parm[edi][ecx][ebx]


void extern _hline(char *adrs,UINT32 cx,UINT32 col);
#pragma aux _hline=\
"mov eax,ebx"\
"shl eax,16"\
"mov eax,ebx"\
"drawloop:"\
"  mov [edi],ax"\
"  add edi,2"\
"  dec ecx"\
"jnz drawloop"\
parm[edi][ecx][ebx]

/*
    INT32 xlen;    
    if (x1>x2) swap(x2,x1);
    if ((y<SC.viewport.a.y)||(y>SC.viewport.b.y)) return;
    if (x1<SC.viewport.a.x) x1=SC.viewport.a.x;
    if (x2>SC.viewport.b.x) x2=SC.viewport.b.x;
    x2++;
    xlen=x2-x1;
    if (xlen<=0) return;
    
    //char *adrs=((char*)SC.startoffsets[y]+(x1+x1));
    
    //_hline(adrs,xlen,t_col.c16);
    //void extern _hline(char *adrs,UINT32 cx,UINT32 col);
    */
    //is line at all visable?}
       /*cmp bx,word ptr SC+OFFbx
       jg fin
       cmp cx,word ptr SC+OFFax
       jl fin*/
       
    
void svga64k_hline(PIXINT x1,PIXINT x2,PIXINT y)
{
    INT32 xlen;    
    if (x1>x2) swap(x2,x1);
    if ((y<SC.viewport.a.y)||(y>SC.viewport.b.y)) return;
    if (x1<SC.viewport.a.x) x1=SC.viewport.a.x;
    if (x2>SC.viewport.b.x) x2=SC.viewport.b.x;
    x2++;
    xlen=x2-x1;
    if (xlen<=0) return;
    
    
    _hline(((char*)SC.startoffsets[y]+(x1+x1)),xlen,t_col.c16);
    
    /*_asm
    {
       movzx eax,x1
       movzx edx,x2
       //movzx ebx,y       
       //swap x1,x1 if x1>x2}
       cmp bx,word ptr SC+OFFby             //check the y axis
       jg fin
       cmp bx,word ptr SC+OFFay
       jl fin

       
       cmp ax,dx
       jle no_swap
       Xchg ax,dx
       no_swap:

       //clip x1 and x2
       mov cx,word ptr SC+OFFax
       cmp ax,cx
       JGE nochange_x1
          mov ax,cx
       nochange_x1:
       mov cx,word ptr SC+OFFbx
       cmp dx,cx
       JLE nochange_x2
          mov dx,cx
       nochange_x2:
       //calculate length
       inc dx
       sub dx,ax
       //if it's negative, the line ain't visible.
       cmp dx,0
       jle fin                     
              
       mov edi,DWORD PTR SC+OFFstartoffsets; //load up offsets pointer
       mov edi,[edi+ebx*4]                   //get yoffset
       lea edi,[edi+eax*2]                   //add xoffset
       
       mov eax,dword ptr t_col                 //loadup colour
       test edi,2              
       xor ecx,ecx
       setnz cl    
       rep stosw
       sub dx,cx
       mov cx,dx
       shr cx,1
       rep stosd
       adc cl,cl
       rep stosw
       fin:
    }*/
}

void extern _vline(char *adrs,UINT32 offs,UINT32 cy,UINT32 col);
#pragma aux _vline=\
"_yloop:"\
"  mov [edi],ax"\
"  add edi,edx"\
"  dec ecx"\
"jnz _yloop"\
parm [edi ][edx][ecx][eax]\
modify [edi]

void extern _vline_xor(char *adrs,UINT32 offs,UINT32 cy,UINT32 col);
#pragma aux _vlinexor=\
"_yloop:"\
"  xor [edi],ax"\
"  add edi,edx"\
"  dec ecx"\
"jnz _yloop"\
parm [edi ][edx][ecx][eax]\
modify [al]




void extern _vline_or(char *adrs,UINT32 offs,UINT32 cy,UINT32 col);
#pragma aux _vline_or=\
"_yloop:"\
"  or [edi],ax"\
"  add edi,edx"\
"  dec ecx"\
"jnz _yloop"\
parm [edi ][edx][ecx][eax]\
modify [edi]


void svga64k_vline(PIXINT x,PIXINT y1,PIXINT y2)
{
    PIXINT ylen;
    if (y1>y2) swap(y2,y1);
    if ((x<SC.viewport.a.x)||(x>SC.viewport.b.x)) return;
    if (y1<SC.viewport.a.y) y1=SC.viewport.a.y;
    if (y2>SC.viewport.b.y) y2=SC.viewport.b.y;
    y2++;
    ylen=(y2-y1);
    if (ylen<=0) return;
    
    char *adrs=((char*)SC.startoffsets[y1]+(x+x));
    
    _vline(adrs,SC.offsd,ylen,t_col.c16);
    /*for (ylen=y1;ylen<=y2;ylen++)
    {
        *adrs=t_col;
        adrs+=SC.offsd;
    } */
}
    

    

extern void xmajorline(UINT32 col);
#pragma aux xmajorline=\
   "mov edx,addy"\
   "mov ebx,edx"\
   "shr ebx,1"\
   "mov esi,linexdiff"\   
   "mov edi,sadrs"\
   "stosw"\
   "xor ecx,ecx"\
   "@xloop:"\
     "add bx,dx"\
     "jnc @nodraw"\
     "rep stosw"\
     "add edi,offs"\
     "@nodraw:"\
     "inc ecx"\
     "dec esi"\
   "jnz @xloop"\
   parm[eax]


extern void ymajorline1(UINT32 col);
#pragma aux ymajorline1=\
  "mov edx,addy"\
  "mov ebx,edx"\
  "shr ebx,1"\
  "mov esi,offs"\
  "mov edi,sadrs"\
  "mov ecx,lineydiff"\
  "@yloop:"\
    "mov [edi],ax"\
    "add bx,dx"\
    "jnc @nooverflow"\
      "sub edi,2"\
    "@nooverflow:"\    
    "sub edi,esi"\
    "dec ecx"\
  "jnz @yloop"\
  parm[eax]


extern void ymajorline2(UINT32 col);
#pragma aux ymajorline2=\
  "mov edx,addy"\
  "mov ebx,edx"\
  "shr ebx,1"\
  "mov esi,offs"\
  "mov edi,sadrs"\
  "mov ecx,lineydiff"\
  "@yloop:"\
    "mov [edi],ax"\
    "add bx,dx"\
    "jnc @nooverflow"\
      "add edi,2"\
    "@nooverflow:"\
    "add edi,esi"\
    "dec ecx"\
  "jnz @yloop"\
  parm[ax]



extern void vert_or_diagline(UINT32 col);
#pragma aux vert_or_diagline=\
  "mov esi,offs"\
  "mov edi,sadrs"\
  "mov ecx,linexdiff"\
  "@yloop:"\
    "mov [edi],ax"\
    "add edi,esi"\
    "dec ecx"\
  "jnz @yloop"\  
  parm [eax]\
  modify [esi edi ecx]      


        
void svga64k_line(PIXINT x1,PIXINT y1,PIXINT x2,PIXINT y2)
{
    
    if (y2==y1) {hline(x1,x2,y1);return;}
    if (!SC.viewport.clipline(x1,y1,x2,y2)) return;
    INT32 offsd=SC.offsd;
    linexdiff=abs(x2-x1);
    lineydiff=abs(y2-y1);
    lineydiff++;
    linexdiff++;      
    if (linexdiff>lineydiff)
    {        
        if (x1>x2)
        {        
            swap(x1,x2);
            swap(y1,y2);
        }    
        sadrs=((char*)SC.startoffsets[y1]+(x1+x1));
        
        if (y2>y1)
           offs=offsd;
        else
           offs=-offsd;
                           
        addy=(lineydiff << 16)/ linexdiff;
        xmajorline(t_col.c16);              
    } else if (lineydiff>linexdiff)
    {
        
                      
        if (y1>y2)
        {
            swap(x1,x2);
            swap(y1,y2);
        }
        sadrs=((char*)SC.startoffsets[y1]+(x1+x1));
        addy=(linexdiff << 16)/ lineydiff;
     
        if (x1>x2)
        {
            offs=-offsd;
            ymajorline1(t_col.c16);               
        } else
        {            
            offs=offsd;
            ymajorline2(t_col.c16);
        }
    }
    /*else if (y1==y2)
    {
        /*if (x1>x2)        
            swap(x1,x2);            
        adrs=((char*)SC.startoffsets[y1]+(x1+x1));
        
        offs=2;
        vert_or_diagline(t_col.c16);                
        hline(x1,x2,y1);
        return;
    }*/
    
    else
    {
    //diagonals +vertical
        if (y1>y2)
        {
            swap(x1,x2);
            swap(y1,y2);
        }
        sadrs=((char*)SC.startoffsets[y1]+(x1+x1));             
        if (x1==x2)
            offs=SC.offsd;
        else if (x2>x1)
            offs=SC.offsd+2;
        else 
            offs=SC.offsd-2;
        vert_or_diagline(t_col.c16);                
    }
}
 

//this routine draws a bar onto the screen. It uses floatword string writes and
//floatword aligns the data before writting. Thanks for that one Martin!
//maybe the inner loop could be done with the adc rep stosw trick
void extern _bar(char *adrs,UINT32 cx,UINT32 cy);
#pragma aux _bar=\
"mov eax,dword ptr t_fillcol"\
"xor ecx,ecx     "\
"mov ebx,edi     "\
"and bx,2        "\
"shr bl,1        "\
"sub dx,bx       "\
"push ebp        "\
"mov ebp,temp32  "\
"shr edx,1        "\
"setc bh         "\
"yloop:          "\
"  mov cl,bl     "\
"  rep stosw     "\
"  mov cx,dx     "\
"  rep stosd     "\
"  mov cl,bh     "\
"  rep stosw     "\
"  add edi,ebp   "\
"  dec esi        "\
"jnz yloop       "\
"pop ebp         "\
parm[edi][edx][esi]\
modify [eax]
/*"  shr cx,1      "
"  rep stosd     "
"  adc cl,cl     "*/


void svga64k_bar(PIXINT x1,PIXINT y1,PIXINT x2,PIXINT y2)
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
    
    char *adrs=((char*)SC.startoffsets[y1]+(x1+x1));
    xlen=1+x2-x1;
    y2-=(y1-1);
    temp32=SC.offsd-(xlen+xlen);
    _bar(adrs,xlen,y2);      
}


////////////////////////////////////////////////////////////////////////////////////////////
//smooth shaded.. lots going on here to optimize.
//Aligned on dword boundaries
//Optimized upper word calculations
//probably more cylces can be shaved by better pentium instruction pairing

UINT32 cnt;
void sl_g(char *adrs,UINT32 count)
{
    _asm
    {
        mov edi,adrs
        mov ecx,count
        mov cnt,0
        
        push ebp
        mov ebp,dword ptr xd   //red start        
        mov esi,dword ptr xd+8 //green start        
        mov edx,dword ptr xd+16//blue start
                
        cmp edi,1
        jz nopredraw
          mov eax,edx                //get blue 16:16 value          
          shr eax,19                 //shift blue value into range & place
          add edx,dword ptr xd+20    //add blue delta          
          mov bx,ax                  //put blue in
          mov eax,esi                //get green 16:16 value          
          shr eax,18                 //shift green into range
          add esi,dword ptr xd+12    //add green delta
          shl eax,5                  //shift green into place          
          or bx,ax                   //or green value into word          
          mov eax,ebp                //get red value
          shr eax,8                  //shift into range & place (upper byte)
          and ah,0xF8                //and out unwanted bits
          add ebp,dword ptr xd+4     //add red delta
          or bh,ah                   //or to final word
          mov [edi],bx               //write to screen
          add edi,2
          dec ecx          
        nopredraw:
        mov eax,ecx
        shr eax,1
        adc cnt,0
        shr ecx,1                
        jle nobody
        //these two words are in the wrong friggin order!
        xloop:          
          mov ebx,edx                //get blue 16:16 value
          mov eax,esi                //get green 16:16 value
          add edx,dword ptr xd+20    //add blue delta
          shr ebx,3
          add esi,dword ptr xd+12    //add green delta                    
          shl eax,3                  //shift green into range
          and eax,0x7E00000              
          or ebx,eax                   //or green value into word          
          mov  eax,ebp                //get red value
          shl eax,8
          and eax,0xF8000000           //and out unwanted bits
          add ebp,dword ptr xd+4     //add red delta
          or  ebx,eax                   //or to final word          
          
          mov eax,edx                //get blue 16:16 value          
          shr eax,19                 //shift blue value into range & place
          add edx,dword ptr xd+20    //add blue delta          
          mov bx,ax                  //put blue in
          mov eax,esi                //get green 16:16 value          
          shr eax,18                 //shift green into range
          add esi,dword ptr xd+12    //add green delta
          shl eax,5                  //shift green into place          
          or bx,ax                   //or green value into word          
          mov eax,ebp                //get red value
          shr eax,8                  //shift into range & place (upper byte)
          and ah,0xF8                //and out unwanted bits
          add ebp,dword ptr xd+4     //add red delta
          or bh,ah                   //or to final word
          
          mov [edi],ebx               //write to screen
          add edi,4          
          dec ecx
        jnz xloop
        nobody:
        cmp cnt,1
        jnz nopostdraw
          mov eax,edx                //get blue 16:16 value          
          shr eax,19                 //shift blue value into range & place          
          mov bx,ax                  //put blue in
          mov eax,esi                //get green 16:16 value          
          shr eax,18                 //shift green into range          
          shl eax,5                  //shift green into place          
          or bx,ax                   //or green value into word          
          mov eax,ebp                //get red value
          shr eax,8                  //shift into range & place (upper byte)
          and ah,0xF8                //and out unwanted bits          
          or bh,ah                   //or to final word
          mov [edi],bx               //write to screen
        nopostdraw:                  
        pop ebp                
    }    
}
/*
void sl_g(char *adrs,UINT32 count)
{
    _asm
    {
        mov edi,adrs
        mov ecx,count
        mov cnt,0
        
        push ebp
        mov ebp,dword ptr xd   //red start        
        mov esi,dword ptr xd+8 //green start        
        mov edx,dword ptr xd+16//blue start
        
                
        xloop:          
          
          mov eax,edx                //get blue 16:16 value          
          shr eax,19                 //shift blue value into range & place
          add edx,dword ptr xd+20    //add blue delta          
          mov bx,ax                  //put blue in
          mov eax,esi                //get green 16:16 value          
          shr eax,18                 //shift green into range
          add esi,dword ptr xd+12    //add green delta
          shl eax,5                  //shift green into place          
          or bx,ax                   //or green value into word          
          mov eax,ebp                //get red value
          shr eax,8                  //shift into range & place (upper byte)
          and ah,0xF8                //and out unwanted bits
          add ebp,dword ptr xd+4     //add red delta
          or bh,ah                   //or to final word
          
          mov [edi],bx               //write to screen
          add edi,2          
          dec ecx
        jnz xloop
        nobody:
        pop ebp                
    }    
}*/

void sl_ga(char *adrs,UINT32 count)
{
    cnt=count;    
    while (count>0)
    {
        pixin.c16=*(UINT16*)adrs;    
        pa1=(xd.a>>COLSHIFTBY);
        pa2=(255-pa1)<<8;
        pa1<<=8;                    
        *(UINT16*)adrs=
            conv24to16.r[conv16to24[conv16to24[pixin.c16].r+pa1].a+conv16to24[(xd.r>>COLSHIFTBY)+pa2].a]|
            conv24to16.g[conv16to24[conv16to24[pixin.c16].g+pa1].a+conv16to24[(xd.g>>COLSHIFTBY)+pa2].a]|
            conv24to16.b[conv16to24[conv16to24[pixin.c16].b+pa1].a+conv16to24[(xd.b>>COLSHIFTBY)+pa2].a];

        xd.r+=xd.dr;
        xd.g+=xd.dg;
        xd.b+=xd.db;
        xd.a+=xd.da;
        adrs+=2;
        count--;                         
    }
}



void sl_f(char *adrs,UINT32 count)
{
    _asm
    {
        mov edi,adrs
        mov ecx,count
        mov bx,word ptr t_col
        mov eax,ebx
        shl eax,16
        mov ax,bx
        test edi,1
        jz noshift           //align that data
          mov [edi],ax
          add edi,2
          dec ecx
        noshift:
        shr ecx,1
        rep stosd            //blast out some dwords
        adc ecx,ecx          //did we get a trailing word?        
        rep stosw            //if so,draw it.
    }    
}

void sl_fa(char *adrs,UINT32 count)
{
    cnt=count;
    static UINT8 cr,cg,cb;        
    pa1=(xd.a>>COLSHIFTBY);
    pa2=(255-pa1)<<8;

    cr=conv16to24[t_colrgba.r+pa2].a;
    cg=conv16to24[t_colrgba.g+pa2].a;
    cb=conv16to24[t_colrgba.b+pa2].a;
    
    pa1<<=8;                    
    while (count>0)    
    {
        pixin.c16=*(UINT16*)adrs;    
        *(UINT16*)adrs=
            conv24to16.r[conv16to24[conv16to24[pixin.c16].r+pa1].a+cr]|
            conv24to16.g[conv16to24[conv16to24[pixin.c16].g+pa1].a+cg]|
            conv24to16.b[conv16to24[conv16to24[pixin.c16].b+pa1].a+cb];
        adrs+=2;
        count--;                                                  
    }
}


void sl_t16(char *adrs,UINT32 count)
{
    _asm
    {
      mov edx,adrs
      mov ecx,count
      push ebp                  
      mov ebp,dword ptr xd+40 //get v
      mov eax,dword ptr xd+32 //get u
      mov esi,t_texturestartoffsets   //point to bitmap offsets                
      xloop:
          //get texel          
          mov ebx,ebp            //load up integer part of v                
          shr ebx,16
          add ebp,dword ptr xd+44 //add dv to v
          mov edi,[esi+ebx*4]    //load up y address          
          mov ebx,eax            //get integer part of u
          add eax,dword ptr xd+36  //get du
          shr ebx,16
          mov bx,[edi+ebx*2]     //get pixel address
          mov [edx],bx
          add edx,2
          dec ecx                    
      jnz xloop          
      pop ebp
    }
}

/*while (count>0)
    {
        *((UINT32*)adrs)=_gettexpix16();        
        xd.u+=xd.du;
        xd.v+=xd.dv;        
        adrs+=2;
        count--;
    }*/

//r=0
//dr=4
//g=8
//dg=12
//b=16
//db=20
//a=24
//da=28
//u=32
//du=36
//v=40
//dv=44

//this will use 16bit range numbers in the tables to reduce cache misses
//unscientific pentium optimizations :/

void sl_gt16(char *adrs,UINT32 count)
{
    //static UINT32 pix,r,g,b;    
    cnt=count;
    _asm
    {
      xor eax,eax
      xor ecx,ecx            //clear for later use
      xor ebx,ebx            //clear for later use
      xor edx,edx            //clear for later use
      mov edi,adrs
      push ebp
      
      mov ebp,dword ptr xd+40 //get v
      xloop:
          //get texel
          mov esi,t_texturestartoffsets   //point to bitmap offsets                
          mov ebx,ebp            //load up integer part of v                
          shr ebx,16                               
          mov esi,[esi+ebx*4]    //load up y address          
          mov ebx,dword ptr xd+32//get integer part of u                
          shr ebx,16
          mov bx,[esi+ebx*2]     //get pixel address          
          
          //mov ebx,dword ptr [conv16to24+eax*4]          //get rgb value into ebx
          //green                
          mov ecx,dword ptr xd+8    //load up integer part of pg
          mov dx,bx                 //g byte from ebx
          and dx,0x7E0              //mask out non green bits
          shr dx,5
          shr ecx,COLHALFSHIFTBY
          shl dx,2          
          lea esi,[edx+ecx]      //calculate green
          mov ax,word ptr [bound0to256_16+esi*4+4096]  //or in green bits to screen word*/
          xor dh,dh
          //red                
          mov ecx,dword ptr xd    //load up integer part of pr                
          mov dl,bh               //r byte from ebx          
          shr ecx,COLHALFSHIFTBY          
          and dx,0xF8             //all that's needed to convert, same as shift left 3 then right 3 :)
          lea esi,[edx+ecx]      //calculate red                
          or ax,word ptr [bound0to256_16+esi*4]      //start building 16bit screen word*/
          
          //blue
          mov ecx,dword ptr xd+16    //load up integer part of pb
          mov dl,bl               //b byte from ebx
          shr ecx,COLHALFSHIFTBY          
          shl dl,3
          lea esi,[edx+ecx]      //calculate blue                                                                
          or ax,word ptr [bound0to256_16+esi*4+8192]  //or in final blue bits to screen word*/
          //bingo!
          mov [edi],ax            //write screen word to screen
          add edi,2               //next screen word please!
          
          mov ebx,dword ptr xd+4  //add dr to r
          mov eax,dword ptr xd+12 //add dg to g
          mov ecx,dword ptr xd+20 //add db to b
          add dword ptr xd,ebx
          add dword ptr xd+8,eax
          add dword ptr xd+16,ecx
          add ebp,dword ptr xd+44 //add dv to v
          mov esi,dword ptr xd+36  //get du
          add dword ptr xd+32,esi      //add du to u
          dec cnt
          jnz xloop
          pop ebp
      }
    /*
    while (count>0)
    {
        pix=_gettexpix16();        
        b=(pix & 0x1F);
        g=(pix & 0x7E0)>>5; 
        r=(pix & 0xF800) >> 11;
        
        r=(r*(xd.r >> 19))>>5;
        g=(g*(xd.g >> 18))>>6;
        b=(b*(xd.b >> 19))>>5;        
        *((UINT32*)adrs)=r | (g << 5) | (b << 11);
        
        xd.u+=xd.du;
        xd.v+=xd.dv;
        xd.r+=xd.dr;
        xd.g+=xd.dg;
        xd.b+=xd.db;


        
        adrs+=2;
        count--;
    }*/
}

void sl_ft16(char *adrs,UINT32 count)
{
    static UINT32 pix;
    static UINT8 r,g,b;    
    cnt=count;
    UINT8 cr=t_colrgba.r;
    UINT8 cg=t_colrgba.g;
    UINT8 cb=t_colrgba.b;        
    
    while (count>0)
    {
        pix=_gettexpix16();        
        b=pix << 3;
        g=(pix>>5) << 2; 
        r=(pix >> 11) << 3;        
        *((UINT16*)adrs)=bound0to256_16.r[cr+r]|bound0to256_16.g[cg+g]|bound0to256_16.b[cb+b];
        
        xd.u+=xd.du;
        xd.v+=xd.dv;
        adrs+=2;
        count--;
    }
}

void sl_gt16a(char *adrs,UINT32 count)
{

}

void sl_ft16a(char *adrs,UINT32 count)
{

}

void sl_t16a(char *adrs,UINT32 count)
{

}



////////////////////////////////////////////////////////////////////////////////////////////
//Set the drawing mode for the polygons.. only implemented in this driver as of 12/1/99


void svga64k_scanmode(UINT8 mode)
{
    switch(mode)
    {
        case 0:drawscanline=sl_f;break;
        case 1:drawscanline=sl_g;break;
        case 2:drawscanline=sl_ft16;break;
        case 3:drawscanline=sl_gt16;break;
        case 4:drawscanline=sl_fa;break;
        case 5:drawscanline=sl_ga;break;
        case 6:drawscanline=sl_ft16a;break;
        case 7:drawscanline=sl_gt16a;break;        
    }
    
}

double flthalf=0.5;

inline float fceil(const float input)
{
   float retCode;
   _asm
   {   
       fld     input
       fadd    qword ptr flthalf
       frndint        
       fstp    retCode
   }
   return retCode;
}

inline float fgetscanlines(const float s1,const float s2)
{
   float retCode;
   _asm
   {   
       fld     s1
       fadd    qword ptr flthalf
       frndint
       fld     s2
       fadd    qword ptr flthalf
       frndint
       fsubp   st(1),st
       fstp    retCode
   }
   return retCode;
}


inline float SUB_PIX(float input)
{
    //return ceil(input)-input;//float2int(input)-input;
    float retCode;    
    _asm
    {   
        fld     input
        fadd    qword ptr flthalf
        frndint        
        fld     input        
        fsubp   st(1), st
        fstp    retCode
    }    
    return retCode;        
}


//draw a triangle. Note that the input variable in trivert are floats to preserve sub-pixel accuracy
//tried it with fixed points all the way through but it always overflows :(
//prehaps I was doing something stupid but floats are fast too these days and they are only used in setup
//I'm now using my own ceil functions and float2fixed conversions; visibly much faster; those watcom ceils
//were really causing problems.

char buf[100];

void svga64k_tri(trivert *v0,trivert *v1,trivert *v2)     
{       
   //long dxr,dxl,xl,xr;
   
   static UINT32 ti;
   static UINT32 scanlines;
   static float v0yceil;
   static float height,invheight;
   static float d,id;
   static int prestep;  
   static INT32 soffs,soffsend;
   static float diffy1_y2,diffy0_y2,diffr1_r2,diffr0_r2,diffg1_g2,diffg0_g2,diffb1_b2,diffb0_b2;
   static trivert *tempv;
   
   //sort
   if (v0->y>v2->y) {tempv=v0;v0=v2;v2=tempv;}           
   if (v0->y>v1->y) {tempv=v0;v0=v1;v1=tempv;}           
   if (v1->y>v2->y) {tempv=v1;v1=v2;v2=tempv;}
   
   //scanlines=v2->y-v0->y;

   diffy1_y2=v1->y-v2->y;
   diffy0_y2=v0->y-v2->y;

   d = (v0->x - v2->x) * diffy1_y2 - (v1->x - v2->x) * diffy0_y2;
   if (d==0.0)
      return;

   height=v2->y-v0->y;        //mov di,yc;sub di,ya
   
   diffr1_r2=v1->r-v2->r;
   diffr0_r2=v0->r-v2->r;

   diffg1_g2=v1->g-v2->g;
   diffg0_g2=v0->g-v2->g;

   diffb1_b2=v1->b-v2->b;
   diffb0_b2=v0->b-v2->b;
   
   id= 1.0f/d;
   v0yceil=fceil(v0->y);
                   
   UINT16 *sbase=(UINT16*)SC.scrptr+(int)(v0yceil*(SC.offsd>>1));
   //should do this in asm
   //prestep = float2fixed((ceil(v0->y)-v0->y));
   prestep = float2fixed(v0yceil-v0->y);
   xd.dr=float2int((( diffr0_r2*diffy1_y2)-(diffr1_r2*diffy0_y2))*id);
   xd.dg=float2int((( diffg0_g2*diffy1_y2)-(diffg1_g2*diffy0_y2))*id);
   xd.db=float2int((( diffb0_b2*diffy1_y2)-(diffb1_b2*diffy0_y2))*id);
   xd.da=float2int((( (v0->a-v2->a)*diffy1_y2)-((v1->a-v2->a)*diffy0_y2))*id);
   xd.du=float2int((( (v0->u-v2->u)*diffy1_y2)-((v1->u-v2->u)*diffy0_y2))*id);
   xd.dv=float2int((( (v0->v-v2->v)*diffy1_y2)-((v1->v-v2->v)*diffy0_y2))*id);
   //id=iceil(id);
   
   if (d>0)                
   {
      invheight = 1.0f/height;
      ic.dxl =    float2fixed(((v2->x-v0->x)*invheight));      
      ic.xl  =    float2fixed(v0->x)+ imul(prestep,ic.dxl);
      ic.yred   = float2int(v0->r)+ imul(prestep,(ic.dyred=   float2int(-diffr0_r2*invheight)));
      ic.ygreen = float2int(v0->g)+ imul(prestep,(ic.dygreen= float2int(-diffg0_g2*invheight)));
      ic.yblue=   float2int(v0->b)+ imul(prestep,(ic.dyblue=  float2int(-diffb0_b2*invheight)));
      ic.ya   =   float2int(v0->a)+ imul(prestep,(ic.dya =    float2int((v2->a-v0->a)*invheight)));
      ic.yu   =   float2int(v0->u)+ imul(prestep,(ic.dyu=     float2int((v2->u-v0->u)*invheight)));
      ic.yv   =   float2int(v0->v)+ imul(prestep,(ic.dyv=     float2int((v2->v-v0->v)*invheight)));

   }else           
   {
      ic.dxr = float2fixed(((v2->x-v0->x)/height));      
      ic.xr= float2fixed(v0->x)+ imul(prestep,ic.dxr);
   }    
                                       
   for (ti=0;ti<2;ti++)
   {       
       //calculate difference. changes at midpoint, hence the outer loop. much better than checking each scanline
       height=(v1->y-v0->y);
       scanlines=fgetscanlines(v1->y,v0->y);//ceil(v1->y)-ceil(v0->y);
       
              
       if (height>0)
       {
       //sprintf(buf,"Scanline count:%i",scanlines);
       //outtextxy(10,yc+=16,buf);
          prestep = float2fixed(SUB_PIX(v0->y));
          //prestep = float2fixed((ceil(v0->y)-v0->y));
          if (d>0)                
          {                                                                     
             ic.dxr = float2fixed(((v1->x-v0->x)/height));      
             ic.xr  = float2fixed(v0->x)+ imul(prestep,ic.dxr);      
          }else           
          {
             invheight = 1.0f/height;                                         
             ic.dxl    = float2fixed(((v1->x-v0->x)*invheight));      
             ic.xl     = float2fixed(v0->x)+ imul(prestep,ic.dxl);
     
             ic.yred   = float2int(v0->r)+ imul(prestep,(ic.dyred=   float2int(((v1->r-v0->r)*invheight))));
             ic.ygreen = float2int(v0->g)+ imul(prestep,(ic.dygreen= float2int(((v1->g-v0->g)*invheight))));
             ic.yblue  = float2int(v0->b)+ imul(prestep,(ic.dyblue=  float2int(((v1->b-v0->b)*invheight))));
             ic.ya   =   float2int(v0->a)+ imul(prestep,(ic.dya =    float2int(((v1->a-v0->a)*invheight))));
             ic.yu     = float2int(v0->u)+ imul(prestep,(ic.dyu=     float2int(((v1->u-v0->u)*invheight))));
             ic.yv     = float2int(v0->v)+ imul(prestep,(ic.dyv=     float2int(((v1->v-v0->v)*invheight))));
          }
          while (scanlines>0)
          {                                               
              soffs=iceil(ic.xl);
              soffsend=iceil(ic.xr)-soffs;
              ic.xl+=ic.dxl;
              ic.xr+=ic.dxr;
              //pixel plotting loop;
              xd.r=ic.yred;
              ic.yred+=ic.dyred;
              xd.g=ic.ygreen;
              ic.ygreen+=ic.dygreen;
              xd.b=ic.yblue;
              ic.yblue+=ic.dyblue;              
              xd.a=ic.ya;
              ic.ya+=ic.dya;
              xd.u=ic.yu;
              ic.yu+=ic.dyu;
              xd.v=ic.yv;
              ic.yv+=ic.dyv;              
              //drawscanline
              if (soffsend >0)
                 drawscanline((char*)sbase+(soffs << 1),soffsend);           
              
              sbase+=SC.offsd >> 1;
              scanlines--;
          }
     }                                
     //shift down to next set of vals
     v0=v1;
     v1=v2;
     }
}


void svga64k_convexpolygon(trivert *points,UINT32 count)
{
    /*if (count>3)
    {
        for (;id<(count >> 1)-1;id++)
        {                                                       
             svga64k_tri(pclipv[id],pclipv[id+1],pclipv[count-id-1],ct);    
             svga64k_tri(pclipv[id+1],pclipv[count-id-2],pclipv[count-id-1],ct);                                                                 
        }                                                               
    }                                                               
    if ((count & 1)==1) 
        addtemptriangle(points);    */
}







////////////////////////////////////////////////////////////////////////////////////////////
//Bitmap pasting routines

/*


extern putbmflat16(char *screen,char *bitmap,UINT32 xlen,UINT32 ylen,UINT32 offset,UINT32 bmoffset);
#pragma aux putbmflat16=\
"test edi,2"\
"setnz byte ptr leadingbyte"\
"sub ecx,leadingbyte"\
"mov temp32,ecx"\
"add ecx,leadingbyte"\
"add ecx,ecx"\
"sub ebx,ecx"\
"sub eax,ecx"\
"xor ecx,ecx"\
"yloop:"\
"  mov cl,byte ptr leadingbyte"\
"  rep movsw"\
"  mov ecx,temp32"\
"  shr ecx,1"\
"  rep movsd"\
"  adc cl,cl"\
"  rep movsw"\
"  add edi,ebx"\
"  add esi,eax"\
"dec dx"\
"jnz yloop"\
parm [edi][esi][ecx][edx][ebx][eax]
//"sub ebx,256"\

*/
extern putbmflat16(UINT32 xlen,UINT32 ylen,UINT32 offset,UINT32 bmoffset);
#pragma aux putbmflat16=\
"mov esi,padrs"\
"mov edi,sadrs"\
"test edi,2"\
"setnz byte ptr leadingbyte"\
"sub ecx,leadingbyte"\
"mov temp32,ecx"\
"add ecx,leadingbyte"\
"add ecx,ecx"\
"sub ebx,ecx"\
"sub eax,ecx"\
"xor ecx,ecx"\
"yloop:"\
"  mov cl,byte ptr leadingbyte"\
"  rep movsw"\
"  mov ecx,temp32"\
"  shr ecx,1"\
"  rep movsd"\
"  adc cl,cl"\
"  rep movsw"\
"  add edi,ebx"\
"  add esi,eax"\
"dec dx"\
"jnz yloop"\
parm [ecx][edx][ebx][eax]

/*void putbmflat16(UINT32 xlen,UINT32 ylen,UINT32 offset,UINT32 bmoffset)
{
  UINT32 xlop;
  bmoffset-=(xlen+xlen);
  offset-=(xlen+xlen);  

  while (ylen>0)
  {
      for (xlop=0;xlop<xlen;xlop++)
      {
          *(UINT32*)sadrs=*(UINT32*)padrs;
          padrs+=2;
          sadrs+=2;                               
      }
      padrs+=bmoffset;
      sadrs+=offset;
      ylen--;
  }
}
*/

/*void putbmflat8str(UINT32 xlen,UINT32 ylen,UINT32 tgtxlen,UINT32 tgtylen,UINT32 offset,UINT32 bmoffset)
{
  UINT32 xaddby,yaddby;
  UINT32 scladrs;

  xaddby=(xlen << 16) / tgtxlen;
  yaddby=(ylen << 16) / tgtylen;
  bmoffset<<=16;
  padrs<<=16;
  
  UINT32 xlop;
  bmoffset-=xlen;
  offset-=(xlen+xlen);  
  while (ylen>0)
  {                  
      for (xlop=0;xlop<xlen;xlop++)
      {   
          *(UINT32*)sadrs=t_nativebitmappalette[*(padrs >> 16)].c16;          
          padrs+=xaddby;
          sadrs+=2;                               
      }
      padrs+=bmoffset;
      sadrs+=offset;                                 
      ylen--;
  }
}*/

//a horizontal 8bit line aligned to float words on both sides in memory and on screen
extern bm8_hline_dalign(char *padrs,char *sadrs,UINT32 *pal,UINT32 count);
#pragma aux bm8_hline_dalign=\
"push ebp"\
"shr ecx,1"\
"mov ebp,ecx"\
"xor ecx,ecx"\      
"xloop:"\
"  mov eax,[edi]"\  
"  mov cl,ah"\      
"  mov dx,[ebx+ecx*4]"\ 
"  rol edx,16"\     
"  mov cl,al"\      
"  mov dx,[ebx+ecx*4]"\ 
"  mov [esi],edx"\  
"  add esi,4"\      
"  shr eax,16"\     
"  mov cl,ah"\      
"  mov dx,[ebx+ecx*4]"\
"  rol edx,16"\     
"  mov cl,al"\      
"  mov dx,[ebx+ecx*4]"\ 
"  mov [esi],edx"\  
"  add esi,4"\      
"  dec ebp"\
"jnz xloop"\
"pop ebp"\
parm [edi][esi][ebx][ecx]

//see bottom of file for annotated version of the above

//uses native palette
void putbmflat8(UINT32 xlen,UINT32 ylen,UINT32 offset,UINT32 bmoffset)
{
  UINT32 xlop;
  bmoffset-=xlen;
  offset-=(xlen+xlen);  
  while (ylen>0)
  {                  
      for (xlop=0;xlop<xlen;xlop++)
      {   
          *(UINT16*)sadrs=t_nativebitmappalette[*padrs].c16;          
          padrs++;
          sadrs+=2;                               
      }
      padrs+=bmoffset;
      sadrs+=offset;                                 
      ylen--;
  }
}


void putbmflat8ckey(UINT32 xlen,UINT32 ylen,UINT32 offset,UINT32 bmoffset)
{
  //outtextxy(0,0,"This is a fookking string");
  UINT32 xlop;
  bmoffset-=xlen;
  offset-=(xlen+xlen);
  UINT8 inchar;  
  while (ylen>0)
  {                  
      for (xlop=0;xlop<xlen;xlop++)
      {
          inchar=*padrs;
          if (inchar!=t_chroma.c8)
          {
              pixin=t_nativebitmappalette[inchar];              
              *(UINT16*)sadrs=pixin.c16;
          }                        
          sadrs+=2;
          padrs++;            
      }
      padrs+=bmoffset;
      sadrs+=offset;                                 
      ylen--;
  }
}

//uses 8bpc palette
void putbmflat8addalpha(UINT32 xlen,UINT32 ylen,UINT32 offset,UINT32 bmoffset)
{
   UINT32 xlop;
   trgba col;
   trgba scol;
   bmoffset-=(xlen);
   offset-=(xlen+xlen);
   while (ylen>0)
   {                            
        for (xlop=0;xlop<xlen;xlop++)
        {

            scol=conv16to24[*(UINT16*)sadrs];
            col=t_bitmappalette[*(UINT8*)padrs];
            
            *(UINT16*)sadrs=conv24to16.r[scol.r+col.r]|
               conv24to16.g[scol.g+col.g]|               
               conv24to16.b[scol.b+col.b];
            sadrs+=2;        
            padrs++;
                                   
        }
        padrs+=bmoffset;
        sadrs+=offset;                                 
        ylen--;
   }
}

void putbmflat8subalpha(UINT32 xlen,UINT32 ylen,UINT32 offset,UINT32 bmoffset)
{
   UINT32 xlop;
   trgba col;
   trgba scol;
   bmoffset-=(xlen);
   offset-=(xlen+xlen);
   while (ylen>0)
   {                            
        for (xlop=0;xlop<xlen;xlop++)
        {
            scol=conv16to24[*(UINT16*)sadrs];     
            col=t_bitmappalette[*(UINT8*)padrs];     
            *(UINT16*)sadrs=
               bound0to256_16.r[256+scol.r-col.r]|
               bound0to256_16.g[256+scol.g-col.g]|
               bound0to256_16.b[256+scol.b-col.b];
            padrs++;
            sadrs+=2;                               
        }
        padrs+=bmoffset;
        sadrs+=offset;                                 
        ylen--;
   }
}

void putbmflat16ckey(UINT32 xlen,UINT32 ylen,UINT32 offset,UINT32 bmoffset)
{
  UINT32 xlop;
  bmoffset-=(xlen+xlen);
  offset-=(xlen+xlen);  

  while (ylen>0)
  {
      for (xlop=0;xlop<xlen;xlop++)
      {
          pixin.c16=*(UINT16*)padrs;
          if (pixin.c16!=t_chroma.c16)
             *(UINT16*)sadrs=pixin.c16;
          padrs+=2;
          sadrs+=2;                               
      }
      padrs+=bmoffset;
      sadrs+=offset;
      ylen--;
  }
}

void putbmflat16addalpha(UINT32 xlen,UINT32 ylen,UINT32 offset,UINT32 bmoffset)
{ 
   UINT32 xlop;
   trgba col,scol;
   bmoffset-=(xlen+xlen);
   offset-=(xlen+xlen);
   while (ylen>0)
   {
       
        for (xlop=0;xlop<xlen;xlop++)
        {
        _asm
        {
            xor eax,eax         //clear eax
            xor ebx,ebx         //clear ebx
            xor ecx,ecx         //clear ebx
            xor edx,edx         //clear ebx
            lea esi,conv16to24  //24bit->16bit rgb lookup table            
            mov edi,padrs       //bitmap pixel address     
            mov bx,[edi]        //get pixel value
            mov ebx,[esi+ebx*4]   //get rgb value
            and ebx,0x00FFFFFF
            mov edi,sadrs       //screen pixel address                 
            mov ax,[edi]        //get pixel value
            mov eax,[esi+eax*4]   //get rgb value
            and eax,0x00FFFFFF
            lea esi,conv24to16  //16bit->24bit rgb lookup table            
            mov cl,al
            mov dl,bl
            add cx,dx
            mov di,[esi+ecx*4]
            xor ch,ch
            mov cl,ah
            mov dl,bh
            add cx,dx
            or  di,[esi+ecx*4+2048]
            shr eax,16
            shr ebx,16
            xor bh,bh
            add ax,bx                                                
            or  di,[esi+eax*4+4096]
            mov esi,sadrs       //screen pixel address     
            mov [esi],di                                
        }
        
         /* scol=conv16to24[*(UINT16*)sadrs];     
            col=conv16to24[*(UINT16*)padrs];     
            *(UINT16*)sadrs=
               conv24to16.r[scol.r+col.r]|
               conv24to16.g[scol.g+col.g]|
               conv24to16.b[scol.b+col.b];*/
              padrs+=2;
        sadrs+=2;                               
        
            
        }
        padrs+=bmoffset;
        sadrs+=offset;                                 
        ylen--;
   }
}

void putbmflat16subalpha(UINT32 xlen,UINT32 ylen,UINT32 offset,UINT32 bmoffset)
{ 
   UINT32 xlop;
   trgba col,scol;
   INT32 r,g,b;
   
   bmoffset-=(xlen+xlen);
   offset-=(xlen+xlen);
   while (ylen>0)
   {                            
        for (xlop=0;xlop<xlen;xlop++)
        {
            scol=conv16to24[*(UINT16*)sadrs];     
            col=conv16to24[*(UINT16*)padrs];
            r=scol.r;
            g=scol.g;
            b=scol.b;
            r-=col.r;
            g-=col.g;
            b-=col.b;
            if (r<0) r=0;
            if (g<0) g=0;
            if (b<0) b=0;                                    
            
            *(UINT16*)sadrs=
               conv24to16.r[r]|conv24to16.g[g]|conv24to16.b[b];
               /*bound0to256_16.r[256+(INT32)scol.r-col.r]|
               bound0to256_16.g[256+(INT32)scol.g-col.g]|
               bound0to256_16.b[256+(INT32)scol.b-col.b];*/
            padrs+=2;
            sadrs+=2;                               
        }
        padrs+=bmoffset;
        sadrs+=offset;                                 
        ylen--;
   }
}

void putbmflat8blendalpha(UINT32 xlen,UINT32 ylen,UINT32 offset,UINT32 bmoffset)
{ 
   UINT32 xlop;
   trgba col;
   trgba scol;
   bmoffset-=(xlen);
   offset-=(xlen+xlen);

   pa1=t_colrgba.a;
   pa2=(255-pa1)<<8;
            
   while (ylen>0)
   {                            
        for (xlop=0;xlop<xlen;xlop++)
        {
            pixin.c16=*(UINT16*)sadrs;
            scol=conv16to24[pixin.c16];                 
            col=t_bitmappalette[*(UINT8*)padrs];     
            *(UINT16*)sadrs=
               conv24to16.r[conv16to24[scol.r+pa1].a+conv16to24[col.r+pa2].a]|
               conv24to16.g[conv16to24[scol.g+pa1].a+conv16to24[col.g+pa2].a]|
               conv24to16.b[conv16to24[scol.b+pa1].a+conv16to24[col.b+pa2].a];               
            padrs++;
            sadrs+=2;                               
        }
        padrs+=bmoffset;
        sadrs+=offset;                                 
        ylen--;
   }
}

void putbmflat16blendalpha(UINT32 xlen,UINT32 ylen,UINT32 offset,UINT32 bmoffset)
{ 
   UINT32 xlop;
   trgba col,scol;
   bmoffset-=(xlen+xlen);
   offset-=(xlen+xlen);
   pa1=t_colrgba.a;//t_fillcol.c32.a;
   pa2=(255-pa1)<<8;
   pa1<<=8;    
   
   while (ylen>0)
   {                            
        for (xlop=0;xlop<xlen;xlop++)
        {
            scol=conv16to24[*(UINT16*)sadrs];
            col=conv16to24[*(UINT16*)padrs];            
            
            *(UINT16*)sadrs=
               conv24to16.r[conv16to24[scol.r+pa2].a+conv16to24[col.r+pa1].a]|
               conv24to16.g[conv16to24[scol.g+pa2].a+conv16to24[col.g+pa1].a]|
               conv24to16.b[conv16to24[scol.b+pa2].a+conv16to24[col.b+pa1].a];
            padrs+=2;
            sadrs+=2;                               
        }
        padrs+=bmoffset;
        sadrs+=offset;                                 
        ylen--;
   }
}


void svga64k_putbitmap(PIXINT x,PIXINT y,tbitmap *pic,char useglobalattribs)
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
    if (xlen<=0 || ylen <=0) return;
    sadrs=(char*)(SC.startoffsets[y]+x+x);

    if (useglobalattribs==0)
        t_nativebitmappalette=pic->palette;
    mode = pic->driver | (pic->drawmode << 8);

    switch (mode)
    {
    case BMflat8:
        padrs=((char*)pic->startoffsets[ydiff]) +xdiff;        
        putbmflat8(xlen,ylen,SC.offsd,picoffset);        
        break;
    
    case (BMflat8|subputsl8):
        padrs=((char*)pic->startoffsets[ydiff]) +xdiff;        
        putbmflat8subalpha(xlen,ylen,SC.offsd,picoffset);
        break;    
    case (BMflat8|blendputsl8):
        padrs=((char*)pic->startoffsets[ydiff]) +xdiff;        
        putbmflat8blendalpha(xlen,ylen,SC.offsd,picoffset);
        break;    
    case (BMflat8|chromaputsl8):
        padrs=((char*)pic->startoffsets[ydiff]) +xdiff;        
        putbmflat8ckey(xlen,ylen,SC.offsd,picoffset);
        break;
    case (BMflat8|addputsl8):
        padrs=((char*)pic->startoffsets[ydiff]) +xdiff;        
        putbmflat8addalpha(xlen,ylen,SC.offsd,picoffset);
        break;    
    case BMflat16:
        padrs=((char*)pic->startoffsets[ydiff])+xdiff+xdiff;        
        putbmflat16(xlen,ylen,SC.offsd,picoffset);        
        break;
    case (BMflat16|chromaputsl8):
        padrs=((char*)pic->startoffsets[ydiff])+xdiff+xdiff;        
        putbmflat16ckey(xlen,ylen,SC.offsd,picoffset);
        break;
    case (BMflat16|addputsl8):
        padrs=((char*)pic->startoffsets[ydiff]) +xdiff+xdiff;        
        putbmflat16addalpha(xlen,ylen,SC.offsd,picoffset);
        break;
    case (BMflat16|subputsl8):
        padrs=((char*)pic->startoffsets[ydiff]) +xdiff+xdiff;        
        putbmflat16subalpha(xlen,ylen,SC.offsd,picoffset);
        break;
    case (BMflat16|blendputsl8):
        padrs=((char*)pic->startoffsets[ydiff]) +xdiff+xdiff;        
        putbmflat16blendalpha(xlen,ylen,SC.offsd,picoffset);
        break;
    }
}

void svga64k_getbitmap(PIXINT x,PIXINT y,tbitmap *pic)
{
    PIXINT x2,y2,xlen,ylen,xdiff,ydiff;
        
    UINT32 picoffset=pic->offsd;
    x2=x+pic->size.x-1;
    y2=y+pic->size.y-1;

    if((x>IC.viewport.b.x)||(x2<IC.viewport.a.x)||
       (y>IC.viewport.b.y)||(y2<IC.viewport.a.y))return;
    
    if (x<IC.viewport.a.x)
    {
        xdiff=IC.viewport.a.x-x; 
        x=IC.viewport.a.x;
    }else
        xdiff=0;
    if (y<IC.viewport.a.y)
    {
        ydiff=IC.viewport.a.y-y; 
        y=IC.viewport.a.y;
    }else
        ydiff=0;
    
    if (x2>IC.viewport.b.x) x2=IC.viewport.b.x;
    if (y<IC.viewport.a.y) y=IC.viewport.a.y;
    if (y2>IC.viewport.b.y) y2=IC.viewport.b.y;

    xlen=x2-x+1;
    ylen=y2-y+1;     
    
    padrs=(char*)(IC.startoffsets[y]+x+x);
    sadrs=((char*)pic->startoffsets[ydiff])+xdiff+xdiff;        
    putbmflat16(xlen,ylen,picoffset,SC.offsd);
          
}


/*
tbitmap *svga64k_getbitmap(PIXINT x,UINT32 y,char tbitmap)
{
    switch (tbitmap)
    {
    }    
    while (ylen>=0)
    {
        memcpy(picadrs,screenadrs,picoffset);
        picadrs+=picoffset;
        screenadrs+=SC.offsd;
        ylen--;
    }   
}*/
/*
"mov cl,xlen"\
"mov ch,ylen"\
"mov esi data"\
"mov edi adrs"\
"mov ah,mask"\*/




/*"    test cl,0"\
"    jz nomask"\
"    nomask:"\*/

extern void drawbytes_1bit(char *adrs,char *data,UINT8 xlen,UINT8 ylen,UINT32 offs,UINT8 mask,UINT32 col);
#pragma aux drawbytes_1bit=\
"mov byte ptr temp32,cl"\
"_yloop:"\
"   mov cl,byte ptr temp32"\
"  _xloop:"\
"    mov al,[esi]"\
"    and al,ah"\
"    test al,128"\
"    jz _nodraw1"\
"      mov [edi],bx"\
"    _nodraw1:"\
"    test al,64"\
"    jz _nodraw2"\
"      mov [edi+2],bx"\
"    _nodraw2:"\
"    test al,32"\
"    jz _nodraw3"\
"      mov [edi+4],bx"\
"    _nodraw3:"\
"    test al,16"\
"    jz _nodraw4"\
"      mov [edi+6],bx"\
"    _nodraw4:"\
"    test al,8"\
"    jz _nodraw5"\
"      mov [edi+8],bx"\
"    _nodraw5:"\
"    test al,4"\
"    jz _nodraw6"\
"      mov [edi+10],bx"\
"    _nodraw6:"\
"    test al,2"\
"    jz _nodraw7"\
"      mov [edi+12],bx"\
"    _nodraw7:"\
"    test al,1"\
"    jz _nodraw8"\
"      mov [edi+14],bx"\
"    _nodraw8:"\    
"    inc esi"\
"    add edi,edx"\
"    dec cl"\
"  jnz _xloop"\
"  dec ch"\
"  jnz _yloop"\
   parm [edi][esi][cl][ch][edx][ah][bx]\
   modify [esi edi ax]


extern void drawbytes_1bitbg(char *adrs,char *data,UINT8 xlen,UINT8 ylen,UINT32 offs,UINT8 mask,UINT32 col);
#pragma aux drawbytes_1bitbg=\
"mov byte ptr temp32,cl"\
"_yloop:"\
"   mov cl,byte ptr temp32"\
"   xor edx,edx" \
"  _xloop:"\
"    mov al,[esi]"\
"    and al,ah"\
"    mov [edi],bx"\
"    mov [edi+2],bx"\
"    mov [edi+4],bx"\
"    mov [edi+6],bx"\
"    mov [edi+8],bx"\
"    mov [edi+10],bx"\
"    mov [edi+12],bx"\
"    mov [edi+14],bx"\
"    inc esi"\
"    add edi,edx"\
"    dec cl"\
"  jnz _xloop"\
"  dec ch"\
"  jnz _yloop"\
   parm [edi][esi][cl][ch][edx][ah][ebx]\
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


void svga64k_circle(INT32 cx,INT32 cy,INT32 radius)
{
     trect circrect=trect(cx-radius,cy-radius,cx+radius,cy+radius);
     if (!circrect.rinside(SC.viewport)) return;
     register UINT16 col=t_col.c16;
     UINT32 halfoffsd=SC.offsd >>1;
     INT32 x,y,d,deltaE,deltaSE;
     INT32 yoff;
     INT32 xoff;


     x=xoff=0;
     y=radius;
     yoff=(SC.startoffsets[y]-(UINT32)SC.scrptr)>>1;
     d=1-radius;
     deltaE=3;
     deltaSE=5-(radius<<1);
     
     UINT8 *base=(UINT8*)SC.startoffsets[cy]+(cx<<1);
     
     /**(UINT16*)(basey+cx+y)=t_col.c16;
     *(UINT16*)(basey+yoff+cx)=t_col.c16;
     *(UINT16*)(basey+yoff-cx)=t_col.c16;
     *(UINT16*)(basey+cx-y)=t_col.c16;
     *(UINT16*)(basey+cx-y)=t_col.c16;
     *(UINT16*)(basey-yoff+cx)=t_col.c16;
     *(UINT16*)(basey-yoff-cx)=t_col.c16;
     *(UINT16*)(basey-cx+y)=t_col.c16;*/
     
     *(UINT16*)( ((UINT16*)base)+y)=col;
     *(UINT16*)( ((UINT16*)base)-y)=col;
     *(UINT16*)( ((UINT16*)base)-y)=col;
     *(UINT16*)( ((UINT16*)base)+y)=col;     
     *(UINT16*)( ((UINT16*)base)+yoff)=col;     
     *(UINT16*)( ((UINT16*)base)-yoff)=col;     

     while(y>x)
     {
         if (d<0)
         {
              d+=deltaE;
              deltaE+=2;
              deltaSE+=2;
              x++;
              xoff+=halfoffsd;
         }
         else
         {
              d+=deltaSE;
              deltaE+=2;
              deltaSE+=4;
              x++;
              y--;
              xoff+=halfoffsd;
              yoff-=halfoffsd;
         }         
         *(UINT16*)( ((UINT16*)base)+xoff+y)=col;
         *(UINT16*)( ((UINT16*)base)+xoff-y)=col;
         *(UINT16*)( ((UINT16*)base)-xoff-y)=col;
         *(UINT16*)( ((UINT16*)base)-xoff+y)=col;
                  
         *(UINT16*)( ((UINT16*)base)+yoff+x)=col;
         *(UINT16*)( ((UINT16*)base)+yoff-x)=col;
         *(UINT16*)( ((UINT16*)base)-yoff-x)=col;
         *(UINT16*)( ((UINT16*)base)-yoff+x)=col;
         
        }
}



/*(UINT32*)( ((UINT32*)SC.startoffsets[cy+x])+cx+y)=t_col.c16;
         *(UINT32*)( ((UINT32*)SC.startoffsets[cy+x])+cx-y)=t_col.c16;
         *(UINT32*)( ((UINT32*)SC.startoffsets[cy-x])+cx-y)=t_col.c16;
         *(UINT32*)( ((UINT32*)SC.startoffsets[cy-x])+cx+y)=t_col.c16;
                  
         *(UINT32*)( ((UINT32*)SC.startoffsets[cy+y])+cx+x)=t_col.c16;
         *(UINT32*)( ((UINT32*)SC.startoffsets[cy+y])+cx-x)=t_col.c16;
         *(UINT32*)( ((UINT32*)SC.startoffsets[cy-y])+cx-x)=t_col.c16;
         *(UINT32*)( ((UINT32*)SC.startoffsets[cy-y])+cx+x)=t_col.c16;*/

void svga64k_clearouttextxy(PIXINT cx,PIXINT cy,char *txt,PIXINT slen)
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


void svga64k_outtextxy(PIXINT cx,PIXINT cy,char *txt,PIXINT slen)
{
    //char *adrs;
    register tfontchar *ch;        
    UINT32 schar=0;
    if (cx>SC.viewport.b.x) return;
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


// This is the only Watcom C/C++ specific part of the function. These

// instructions take a 26:6 bit fixed point number and converts it 

// to 32:32 bit. Then divides it with another 16:16 bit fixed point 

// number. The result is 16:16 bit. This must be done in asm where we

// can do 64/32 bit divides.





/*
x2:=x1+8;
  dec(nbytes);
  if (x2>=sc.viewport.x1)and(x1<=SC.viewport.x2)and(y1+nbytes>=SC.viewport.y1)and(y1<=SC.viewport.y2)then begin
  Adrs:=(y1*SC.offsw)+x1;

  {**XCliping**}
  mask:=$ff;
  if x1<SC.viewport.x1 then
    mask:=$ff shr (SC.viewport.x1-x1);
  if x2>SC.viewport.x2 then
    mask:=mask and ($ff shl (x2-SC.viewport.x2-1));

  {**YCliping**}
  If y1+nbytes>SC.viewport.y2 then lop:=(SC.viewport.y2-y1) else lop:=nbytes;
  if y1<SC.viewport.y1 then begin
     y1:=SC.viewport.y1-y1;inc(Adrs,y1*SC.offsw);
  end else y1:=0;*/


void svga64k_drawbytes(PIXINT x,PIXINT y,UINT8 w,UINT8 h,char *bytes)
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

    adrs=((char*)SC.startoffsets[y]+(x+x));
    drawbytes_1bit(adrs,bytes,w,h,SC.offsd,mask,t_col.c16);            
}

extern _cleardevice(char *screen,UINT32 pagesize,UINT32 col);
#pragma aux _cleardevice=\
"mov bx,ax"\
"shl eax,16"\
"mov ax,bx"\
"shr ecx,2"\
"rep stosd"\
parm [edi][ecx][ax]\
modify[bx eax]

    
    
void svga64k_clearDevice(void)
{
    _cleardevice(SC.scrptr,SC.pagesize,t_fillcol.c16);
}


/*
extern bm8_hline_dalign(char *padrs,char *sadrs,UINT32 *pal,UINT32 count);
#pragma aux bm8_hline_dalign=\
"push ebp"\
"shr ecx,1"\
"mov ebp,ecx"\
"xor ecx,ecx"\      //clear ecx
"xloop:"\
"  mov eax,[edi]"\  /*get 4 bytes from images
"  mov cl,ah"\      /*mov upper byte into dl
"  mov dx,[ebx+ecx*4]"\ /*get the actual screen pix
"  rol edx,16"\     /*rotate dx so that pix is in the right place
"  mov cl,al"\      /*mov lower byte into dl
"  mov dx,[ebx+ecx*4]"\ /*get the actual screen pix
"  mov [esi],edx"\  /*write first dword to screen
"  add esi,4"\      /*next dest pixel
"  shr eax,16"\     /*rotate eax, get the next 2 pixels into the low word
"  mov cl,ah"\      /*mov upper byte into dl
"  mov dx,[ebx+ecx*4]" /*get the actual screen pix
"  rol edx,16"\     /*rotate dx so that pix is in the right place
"  mov cl,al"\      /*mov lower byte into dl
"  mov dx,[ebx+ecx*4]"\ /*get the actual screen pix
"  mov [esi],edx"\  /*write first dword to screen
"  add esi,4"\      /*next dest pixel
"  dec ebp"\
"jnz xloop:"\
"pop ebp"\
parm [edi][esi][ebx][ecx]
*/
