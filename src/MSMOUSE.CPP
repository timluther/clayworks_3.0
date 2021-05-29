//msmouse unit

//uses gbasics,tgraph;
#include "msmouse.hpp"
#include "ttypes.hpp"
#include "gbasics.hpp"
#include "dosstuff.hpp"

#ifndef MSMOUSE_C
#define MSMOUSE_C

#define COM1INTR 0x0C
#define COM1PORT 0x3F8




//*****Globals*****


trect oldvp;
tbitmap *mouseimage=NULL;
tbitmap *mouseunderarea=NULL;
tcolour black;
tpoint hotspot;
trect mrect,ms_bounds;
UINT32 ms_counter;
UINT32 ms_dcounter;
UINT32 ms_lcounter;

signed int ms_visible=0;
char oldvis=FALSE;
tpoint mp,moldp,mdiff,internalmdiff,internalmp(0,0);
UINT8 mb,moldb;

tsize msize;
//change to single attribute
char ms_pressed,ms_released,ms_clicked,ms_doubleclicked,ms_moved,ms_lingering;
UINT32 ms_attrib;


void MOUSEsetpos(INT16 x,INT16 y)
{
   _asm
   {
     mov ax,4
     mov cx,x
     mov dx,y
     
     int 0x33
     mov word ptr mp,cx
     mov word ptr mp+2,dx     
   }
   MOUSEpoll();
}
extern void MOUSEgetpos();
#pragma aux MOUSEgetpos= \
    "mov ax,3"\
    "int 0x33"\
    "mov word ptr internalmp,cx"\
    "mov word ptr internalmp+4,dx"\
    "mov mb,bl"\
    modify [cx dx bx ax esi]


extern char MOUSEresetasm();
#pragma aux MOUSEresetasm=\
"mov ax,0"\
"int 0x33"


BOOL MOUSEreset()
{  
  return MOUSEresetasm();
  
}


//extern void mouseon();



void MOUSEsetshape(INT16 x,INT16 y,tbitmap *bm)
{
   hotspot=tpoint(x,y);
   mouseimage=bm;
   if (msize!=bm->size)
   {
      msize=bm->size;
      delete mouseunderarea;         
      mouseunderarea=new tbitmap(msize.x,msize.y,SC.driver);
   }
}

extern MOUSEsetsenseasm(INT16 x,INT16 y,INT16 dspeed);
#pragma aux MOUSEsetsenseasm=\
"mov ax,0x1A"\
"int 0x33"\
parm [bx][cx][dx]

void MOUSEoff()
{
        if (ms_visible>=1)
        {
            oldvp=SC.viewport;
            SC.viewport.rassign(SC.size.x,SC.size.y);
            putbitmap(mp.x,mp.y,mouseunderarea);
            SC.viewport=oldvp; 
        }
        ms_visible--;    
    
}

void MOUSEon()
{
    oldvp=SC.viewport;
    SC.viewport.rassign(SC.size.x,SC.size.y);
    if (mouseunderarea)
            getbitmap(mp.x,mp.y,mouseunderarea);
    ms_visible++;
    if (ms_visible>1) ms_visible=1;
    
    if (ms_visible==1)
    {                          
        if (mouseimage)
            putbitmap(mp.x,mp.y,mouseimage);        
    }
    ms_attrib=MS_VISIBLE;
    SC.viewport=oldvp;
}

    
void MOUSEsetsense(INT16 x,INT16 y,INT16 dspeed)
{
    MOUSEsetsenseasm(x,y,dspeed);
}

void MOUSEsetbounds(INT16 x,INT16 y,INT16 x2,INT16 y2)
{
    ms_bounds.rassign(x,y,x2,y2);
/*    _asm
    {
        mov ax,7
        mov cx,-31000
        mov dx,31000
        int 33h

        mov ax,8
        mov cx,-31000
        mov dx,31000
        int 33h        
    }*/
    _asm
    {
        mov ax,7
        mov cx,x
        mov dx,x2
        int 33h

        mov ax,8
        mov cx,y
        mov dx,y2
        int 33h        
    }
}

void MOUSEhardwarepoll()
{
    static tpoint oldinternalmp;
    moldb=mb;
    oldinternalmp=internalmp;
    MOUSEgetpos();
    internalmdiff=internalmp-oldinternalmp;
}

/*extern UINT8 inportb(UINT16 port);
#pragma aux inportb="in al,dx" parm[dx]

extern UINT8 outportb(UINT16 port,UINT8 b);
#pragma aux outportb="out dx,al" parm[dx al]*/
/*

void MOUSEhardwarepoll2() 
{
  moldb=mb;
  char combytes[3];
  char inbyte;  
  //sync it
  while ((inbyte & 64) == 0) inbyte = inportb(COM1PORT);        
  combytes[0]=inbyte;
  combytes[1]=inportb(COM1PORT);
  combytes[2]=inportb(COM1PORT);
  
  internalmdiff.x= (combytes[0] & 3) << 6 + combytes[1];
  internalmdiff.y= (combytes[0] & 12) << 4 + combytes[2];
  if (internalmdiff.x>= 128) internalmdiff.x-=256;
  if (internalmdiff.y>= 128) internalmdiff.y-=256;    
  mb = (combytes[0] & 32) >> 5 + (combytes[0] & 16) >> 3;
  internalmp+=internalmdiff;  
  //Acknowledge the interrupt
  outportb(0x20) = 0x20;
}
*/
void MOUSEpoll()
{
  moldb=mb;  
  moldp=mp;  
  //MOUSEhardwarepoll();
  MOUSEgetpos();
  mp=internalmp;
  mdiff=mp-moldp;
  //mp+=internalmdiff;
  /*if ((ms_attrib & MS_WRAPMODE )!=0)
  {
      mdiff=mp-moldp;
      if (mp.x<ms_bounds.a.x)    
         mp.x=ms_bounds.b.x-(ms_bounds.a.x-mp.x);
      else if (mp.x>ms_bounds.b.x)
         mp.x=ms_bounds.a.x+(mp.x-ms_bounds.b.x);
      if (mp.y<ms_bounds.a.y)    
         mp.y=ms_bounds.b.y-(ms_bounds.a.y-mp.y);
      else if (mp.y>ms_bounds.b.y)
         mp.y=ms_bounds.a.y+(mp.y-ms_bounds.b.y);
  }else
  {
      if (mp.x<ms_bounds.a.x)    
         mp.x=ms_bounds.a.x;
      else if (mp.x>ms_bounds.b.x)
         mp.x=ms_bounds.b.x;
      if (mp.y<ms_bounds.a.y)    
         mp.y=ms_bounds.a.y;
      else if (mp.y>ms_bounds.b.y)
         mp.y=ms_bounds.b.y;
      mdiff=mp-moldp;
  }
  /*_asm
  {
     mov ax,4
     mov cx,word ptr mp
     mov dx,word ptr mp+2
     int 0x33
  } */      
  mrect.a=mp;
  mrect.b.x=mp.x+msize.x;
  mrect.b.y=mp.y+msize.y;
      
  ms_moved=moldp!=mp;
  ms_released=(moldb!=0)&&(mb==0);
  ms_pressed=(moldb!=mb)&&(mb!=0);

  if ((ms_moved)&&(ms_visible))
  {
      oldvp=SC.viewport;
      SC.viewport.rassign(SC.size.x,SC.size.y);
      if (mouseunderarea)
      {
         putbitmap(moldp.x,moldp.y,mouseunderarea);
         getbitmap(mp.x,mp.y,mouseunderarea);          
      }
      if (mouseimage)
          putbitmap(mp.x,mp.y,mouseimage);
      SC.viewport=oldvp;
  }

  if (ms_pressed) ms_counter=*timer+clickwait;  
  ms_clicked=((ms_released)&&(*timer<ms_counter));
  ms_doubleclicked=((*timer<ms_dcounter)); //&&(oldmouserect.rcontains(mp.x,mp.y))
  if (ms_clicked) ms_dcounter=*timer+dclickwait;    
  
  /*if ms_clicked {
    oldmouserect.rassign(mp.x-dclick_xthreshold,mp.y-dclick_ythreshold,mp.x+dclick_xthreshold,mp.y+dclick_ythreshold);
    if ((mp.x=moldp.x)&&(moldp.y=mp.y))
    {
      lcounter++;
      ms_lingering=lcounter>linger_for;
    }  
  } else {
    lcounter=0;
    ms_lingering=false;
  }*/
}

BOOL  MOUSEinit(tbitmap *mouseimage_)
{
    black.c32val=0;
    ms_counter=clickwait;
    ms_dcounter=dclickwait;
    ms_lcounter=linger_for;
  
    mouseimage=mouseimage_;
    mp=moldp=mdiff=0;
    mb=moldb=0;
    msize=mouseimage->size;
    mouseunderarea=new tbitmap(msize.x,msize.y,SC.driver);
    //set the mouse bounds to huge numbers; effectivley bypasses the system.
    
    BOOL reset=MOUSEreset();

    return reset;    
}

void MOUSEshutdown()
{
    if (mouseimage)
      delete mouseimage;
    if (mouseunderarea)
      delete mouseunderarea;
}

void MOUSEstoprepeat()
{
    while (mb!=0) MOUSEpoll();
}

BOOL MOUSEchkicon(INT16 x1,INT16 y1,INT16 x2,INT16 y2)
{
    if ((mp.x>=x1) && (mp.y>=y1) && (mp.x<=x1+x2) && (mp.y<=y1+y2))
       return mb;
    else
       return -1;
}

BOOL MOUSEchkouticon(INT16 x1,INT16 y1,INT16 x2,INT16 y2)
{
    if ((mp.x<x1) | (mp.y<y1) | (mp.x>x1+x2) | (mp.y>y1+y2))    
       return mb;
    else
       return -1;
}

void MOUSEstoprepeatwait(UINT16 wait)
{
    UINT16 start=*timer,count=0;
    while ((mb!=0) && (count<wait))
    {
        MOUSEpoll();
        count=*timer-start;
    }
}


#endif


/* com stuff, get back to this
#define COM1INTR 0x0C
#define COM1PORT 0x3F8
char combytes[3];
char inbyte;  
  //sync it
  while ((inbyte & 64) == 0) inbyte = inp(COM1PORT);        
  combytes[0]=inbyte;
  combytes[1]=inport(COM1PORT);
  combytes[2]=inport(COM1PORT);
  
  ms_xdiff := (combytes[0] & 3) << 6 + combytes[1];
  ms_ydiff := (combytes[0] & 12) << 4 + combytes[2];
  if (ms_xdiff >= 128) ms_xdiff-=256);
  if (ms_ydiff >= 128) ms_ydiff-=256);

  mp.x+=ms_xdiff;
  mp.y+=ms_ydiff;  
    
  mb = (combytes[0] & 32) >> 5 + (combytes[0] & 16) >> 3);
  
  //Acknowledge the interrupt
  outport(0x20) = 0x20;
*/

