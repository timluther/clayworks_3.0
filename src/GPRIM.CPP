//driver indep}ant drawing 'primatives'

#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "gbasics.hpp"
#include "gprim.hpp"
#include "ttypes.hpp"

UINT8 circlepic[8]={0,24,60,126,126,60,24,0};
UINT8 tickpic[8]={0,1,3,6,140,216,112,32};
UINT8 closegadget[8]={255,153,24,24,24,24,24,60};
UINT8 uparrow[8]={24,60,126,255,60,60,60,60};
UINT8 downarrow[8]={60,60,60,60,255,126,60,24};
UINT8 resizegadget[8]={253,253,253,253,253,253,1,255};
UINT8 leftarrow[8]={16,48,127,255,255,127,48,16};
UINT8 rightarrow[8]={8,12,254,255,255,254,12,8};
UINT8 updownarrow[8]={24,60,126,0,126,60,24,0};

trgbbase greypal[16]={{10,10,30},{27,30,45},{2,3,34},{12,9,16},
{25,25,29},{35,35,37},{45,45,49},{55,55,58},{63,20,15},{63,15,15},
{23,45,13},{45,55,62},{15,62,20},{32,35,50},{28,25,40},{62,62,62}};

tcolour scrollhighlight;
tcolour highlight; //for icons e.t.c.//
tcolour textcol;   //text colour//
tcolour titletextcol; //for window titles//
tcolour scrollcol; //scroller background//
tcolour lightcol;  //highlight colour//
tcolour midcol;    //middle colour//
tcolour mid_darkcol;   //shadow colour//
tcolour mid_darkcol2;   //shadow colour//
tcolour darkcol;   //shadow colour//
tcolour backcol;   //background colour//
tcolour gridcol;    //grid colour//
tcolour selectcol;  //selection colour//
tcolour titlecol;   //titlebar colour//
tcolour disabledcol;
tcolour menuselcolback; 
tcolour menuselcolfore;
tcolour textboxcol;


char nodebm[5]={124,68,68,68,124}; //pattern for nodes

void drawarrow(flt x1,flt y1,flt x2,flt y2,flt head,flt tail,flt width)
{
    flt len;
    tpointfloat tailp,headp,diff;
    line(float2int(x1),float2int(y1),float2int(x2),float2int(y2));
    
    //m=ydiff/xdiff;
    //im=diff.x/diff.y;
    diff.x=x2-x1;
    diff.y=y2-y1;
    
    len=(flt)sqrt(diff.x*diff.x + diff.y*diff.y);
    if (len<=1.0f) return;
    headp.x=x1+(diff.x * head);
    headp.y=y1+(diff.y * head);
    tailp.x=x1+(diff.x * tail);
    tailp.y=y1+(diff.y * tail);
     
    //drawbytes(float2int(headp.x)-3,float2int(headp.y)-3,1,5,nodebm);
    //drawbytes(float2int(tailp.x)-3,float2int(tailp.y)-3,1,5,nodebm);
    diff/=len;
    diff*=width;
    
    line(float2int(headp.x),float2int(headp.y),float2int(tailp.x-diff.y),float2int(tailp.y+diff.x));
    line(float2int(headp.x),float2int(headp.y),float2int(tailp.x+diff.y),float2int(tailp.y-diff.x));           
}

void captilebitmapy(INT16 x,INT16 y,INT16 y2,tbitmap **bm,UINT8 useattrib)
{
    putbitmap(x,y,*bm,useattrib);
    INT16 oldy1=SC.viewport.a.y;
    INT16 lasty;
    if (oldy1<y)
      SC.viewport.a.y=y;
    if (SC.viewport.b.y<SC.viewport.a.y) goto exit;
    lasty=y2-bm[2]->size.y;
    if (y<y2)
    {
        y+=(*bm)->size.y;        
        bm++;
        tilebitmapy(x,y,lasty-1,*bm,useattrib);
        bm++;
        putbitmap(x,lasty,*bm,useattrib);
    }
exit:
    SC.viewport.a.y=oldy1;   
}

void captilebitmapx(INT16 x,INT16 y,INT16 x2,tbitmap **bm,UINT8 useattrib)
{
    putbitmap(x,y,*bm,useattrib);
    INT16 oldx1=SC.viewport.a.x;
    INT16 lastx;
    if (oldx1<x)
      SC.viewport.a.x=x;
    if (SC.viewport.b.x<SC.viewport.a.x) goto exit;
    lastx=x2-bm[2]->size.x;
    if (x<x2)
    {
        x+=(*bm)->size.x;        
        bm++;
        tilebitmapx(x,y,lastx-1,*bm,useattrib);
        bm++;
        putbitmap(lastx,y,*bm,useattrib);
    }
exit:
    SC.viewport.a.x=oldx1;   
}

void captilebitmapx(INT16 x,INT16 y,INT16 x2,tbitmap *bm1,tbitmap *bm2,tbitmap *bm3)
{        
    /*putbitmap(x,y,bm1);
    x+=bm1->size.x;
    INT16 lastx=x2-bm2->size.x;
    tilebitmapx(x,y,lastx-1,bm2);
    putbitmap(lastx,y,bm3);*/    
}

void captilebitmapy(INT16 x,INT16 y,INT16 y2,tbitmap *bm1,tbitmap *bm2,tbitmap *bm3)
{
    /*putbitmap(x,y,bm1);
    y+=bm1->size.y;
    INT16 lasty=y2-bm2->size.y;
    tilebitmapy(x,y,lasty-1,bm2);
    putbitmap(x,lasty,bm3);          */
}




void tilebitmapx(INT16 x,INT16 y,INT16 x2,tbitmap *bm,UINT8 useattrib)
{
    INT16 oldx2=SC.viewport.b.x;
    if (SC.viewport.b.x>x2)
        SC.viewport.b.x=x2;
    else
        x2=SC.viewport.b.x;
    if (SC.viewport.b.x>SC.size.x-1) goto exit;
    while (x<=x2)
    {
      putbitmap(x,y,bm,useattrib);
      x+=bm->size.x;
    }
exit:
    SC.viewport.b.x=oldx2;
}

void tilebitmapy(INT16 x,INT16 y,INT16 y2,tbitmap *bm,UINT8 useattrib)
{
    INT16 oldy2=SC.viewport.b.y;
    if (SC.viewport.b.y>y2)
       SC.viewport.b.y=y2;
    else
       y2=SC.viewport.b.y;
    if (SC.viewport.b.y>SC.size.y-1) goto exit;
    while (y<=y2)
    {
      putbitmap(x,y,bm,useattrib);
      y+=bm->size.y;
    }
exit:
    SC.viewport.b.y=oldy2;
}


void outboxcol(INT16 x1,INT16 y1,INT16 x2,INT16 y2)
{
  t_fillcol=t_nativepalette[8];
  bar(x1+2,y1+2,x2-2,y2-2);  
  t_col=t_nativepalette[10];vline (x1,y1,y2);hline (x1,x2,y1);
  t_col=t_nativepalette[9];vline (x1+1,y1+1,y2-1);hline (x1+1,x2-1,y1+1);  
  t_col=t_nativepalette[13];putpixel(x1,y1);
  t_col=t_nativepalette[5]; vline(x2-1,y1+1,y2-2);hline(x1+1,x2-1,y2-1);
  t_col=t_nativepalette[1]; vline (x2,y1,y2);hline (x1,x2,y2);
}

void inboxcol(INT16 x1,INT16 y1,INT16 x2,INT16 y2)
{
  t_fillcol=t_nativepalette[8];
  bar(x1+2,y1+2,x2,y2);
  t_col=t_nativepalette[5];vline (x1+1,y1+1,y2);hline (x1+1,x2,y1+1);
  t_col=t_nativepalette[1];vline (x1,y1,y2);hline (x1,x2,y1);
};

void outbox(INT16 x1,INT16 y1,INT16 x2,INT16 y2)
{
  t_fillcol=midcol;bar(x1+1,y1+1,x2-2,y2-2);
  t_col=lightcol;vline(x1,y1,y2-1); hline(x1,x2-1,y1);
  t_col=highlight;putpixel(x1,y1);
  t_col=mid_darkcol;vline(x2-1,y1+1,y2-1);hline(x1+1,x2-1,y2-1);
  t_col=darkcol;vline(x2,y1,y2); hline(x1,x2,y2);
}

void drawborder(INT16 x1,INT16 y1,INT16 x2,INT16 y2,UINT8 borderwidth)
{
  t_col=t_nativepalette[10];vline (x1,y1,y2);hline (x1,x2,y1);
  t_col=t_nativepalette[9];vline (x1+1,y1+1,y2-1);hline (x1+1,x2-1,y1+1);  
  t_col=t_nativepalette[13];putpixel(x1,y1);
  t_col=t_nativepalette[5]; vline(x2-1,y1+1,y2-2);hline(x1+1,x2-1,y2-1);
  t_col=t_nativepalette[1]; vline (x2,y1,y2);hline (x1,x2,y2);
  t_col=t_nativepalette[8];
  thickrectangle(x1+2,y1+2,x2-2,y2-2,borderwidth-2);
}


void inbox(INT16 x1,INT16 y1,INT16 x2,INT16 y2)
{
  t_fillcol=midcol;bar(x1+2,y1+2,x2,y2);
  t_col=mid_darkcol;vline (x1+1,y1+1,y2);hline (x1+1,x2,y1+1);
  t_col=darkcol;vline (x1,y1,y2);hline (x1,x2,y1);
}

void insetborder(INT16 x1,INT16 y1,INT16 x2,INT16 y2,char *txt)
{
  t_col=darkcol;
  outtextxy(x1+8,y1-2,txt);
  vline(x1,y1+1,y2-1);vline(x2,y1+1,y2-1);
  t_col=highlight;
  vline(x1+1,y1+1,y2-1);vline(x2+1,y1+1,y2);
  hline3d(x1+1,x1+4,y1+1);hline3d(x1+10+strlen(txt)*8,x2-1,y1+1);
  hline3d(x1+1,x2+1,y2+1);
};

void hline3d(INT16 x1,INT16 x2,INT16 y1)
{
  hline(x1,x2,y1+1);
  t_col=darkcol;
  hline(x1,x2,y1);
}

void grid(INT16 x,INT16 y,INT16 gxsize,INT16 gysize,INT16 stpx,INT16 stpy)
{
  register INT16 lop,x2,y2;
  x2=gxsize*stpx;
  y2=gysize*stpy;
  for (lop=1;lop<=stpx;lop++)
    vline(x+(lop*gxsize),y-1,y2+y-1);
  for (lop=1;lop<=stpx;lop++)
    hline(x+1,x2+x+1,y+(lop*gysize));
}

void dotgrid(INT16 x,INT16 y,INT16 gxsize,INT16 gysize,INT16 stpx,INT16 stpy)
{
  register INT16 xlop,ylop,ycount;
  for (xlop=1;xlop<=stpx;xlop++){
    ycount=y;
    for (ylop=1;ylop<=stpy;ylop++){
      putpixel(x,ycount);
      ycount+=gysize;
    }
    x+=gxsize;
  }
}

void boxgrid(INT16 x,INT16 y,INT16 gxsize,INT16 gysize,INT16 stpx,INT16 stpy)
{
  register INT16 xlop,ylop,ycount;
  for (xlop=1;xlop<=stpx;xlop++){
    ycount=y;
    for (ylop=1;ylop<=stpy;ylop++){
      outbox(x,ycount,x+gxsize-1,y+gysize-1);
      ycount+=gysize;
    }
    x+=gxsize;
  }
}

void colourgrid(INT16 x,INT16 y,INT16 gxsize,INT16 gysize,INT16 stpx,INT16 stpy)
{
  register INT16 xlop,ylop,ycount;
  //t_fillcol=scol;
  for (xlop=1;xlop<=stpx;xlop++){
    ycount=y;
    for (ylop=1;ylop<=stpy;ylop++){
      bar(x,ycount,x+gxsize-1,y+gysize-1);
      ycount+=gysize;
      //t_fillcol++;
    }
    x+=gxsize;
  }
}

void colourgrid2(INT16 x,INT16 y,INT16 gxsize,INT16 gysize,INT16 stpx,INT16 stpy)
{
  register INT16 xlop,ylop,ycount;
  //t_fillcol=scol;
  for (xlop=1;xlop<=stpx;xlop++)
  {
    ycount=y;
    for (ylop=1;ylop<=stpy;ylop++)
    {
      
      bar(x,ycount,x+gxsize,ycount+gysize);
      swap(t_fillcol.c32val,t_col.c32val);
      ycount+=gysize+1;
      //t_fillcol++;
    }
    x+=gxsize+1;
  }
}

void textbox(INT16 x1,INT16 y1,INT16 x2,INT16 y2)
{
  t_col=t_nativepalette[7];hline(x1+1,x2-2,y1+1);
  vline(x1+1,y1+1,y2-2);
  t_col=t_nativepalette[13];hline(x1+1,x2-1,y2-1);
  vline(x2-1,y1+1,y2-1);
  t_col=t_nativepalette[3];rectangle(x1,y1,x2,y2);
};

void textboxfilled(INT16 x1,INT16 y1,INT16 x2,INT16 y2)
{
  t_col=t_nativepalette[7];
  hline(x1+1,x2-2,y1+1);
  vline(x1+1,y1+1,y2-2);
  t_col=t_nativepalette[13];
  hline(x1+1,x2-1,y2-1);
  vline(x2-1,y1+1,y2-1);
  t_col=t_nativepalette[3];
  rectangle(x1,y1,x2,y2);
  t_fillcol=t_nativepalette[10];
  bar(x1+2,y1+2,x2-2,y2-2);
};

/*void textsel(x,y,x2:INT16eger);
{
  if x>x2  asm mov ax,x;Xchg x2,ax;mov x,ax;};
  if x!=x2  { dec(x2);solidrect(x,y,x2,y+8);};
};*/

void  textcurs(INT16 x,INT16 y,INT16 y2)
{
  hline(x-2,x+2,y-1);
  hline(x-2,x+2,y+y2);
  vline(x,y,y+y2-1);
}

void  drawnode(INT16 x,INT16 y)
{
  hline(x-nodesize,x+nodesize,y-nodesize);
  hline(x-nodesize,x+nodesize,y+nodesize);
  vline(x-nodesize,y-nodesizedec1,y+nodesizedec1);
  vline(x+nodesize,y-nodesizedec1,y+nodesizedec1);
}

void drawcursor(INT16 x,INT16 y)
{
  hline(x-7,x+7,y);
  hline(x-7,x+7,y+1);
  vline(x+1,y-7,y+7);
  vline(x,y-7,y+7);
}

void drawx(INT16 x,INT16 y)
{
  line(x-5,y-5,x+5,y+5);
  line(x-5,y+5,x+5,y-5);
}


void thickrectangle(INT16 x1,INT16 y1,INT16 x2,INT16 y2,INT16 thickness)
{
  INT16 lop;
  for (lop=0;lop<thickness;lop++)
  {
    rectangle(x1,y1,x2,y2);
    x1++; y1++;
    x2--; y2--;
  }  
}  
/*
void rotate_point(INT16  *x,INT16 *y,INT16 cx,INT16 cy,INT16 angle)
{
  INT16 tmp;
  INT16 angle=angle & maxangle_mask;
  if (angle!=0){
    x-=cx;y-+cy;
    tmp=trunc((x*coz[angle]) - (y* si[angle]) );
    y=trunc( (x* si[angle]) + (y*coz[angle]) );
    x=tmp;
    x+=cx;y+=cy;
  }
} */

void draw3dborder(INT16 x1,INT16 y1,INT16 x2,INT16 y2,char inv)
{
  if (inv) t_col=darkcol; else t_col=lightcol;
  hline(x1,x2,y1);
  vline(x1,y1,y2-1);
  if (inv) t_col=lightcol; else t_col=darkcol;
  hline(x1,x2-1,y2);
  vline(x2,y1,y2);
  t_col=highlight;
  if (inv)
    putpixel(x2,y2);
  else
    putpixel(x1,y1);
}

void rectangle(INT16 x1,INT16 y1,INT16 x2,INT16 y2)
{

  hline(x1,x2,y1);
  hline(x1,x2,y2);
  if (y2>y1){
    vline(x1,y1+1,y2-1);
    vline(x2,y1+1,y2-1);}
  else {
    vline(x1,y2+1,y1-1);
    vline(x2,y2+1,y1-1);
  }  
}


void rectangle(trect b)
{

  hline(b.a.x,b.b.x,b.a.y);
  hline(b.a.x,b.b.x,b.b.y);
  if (b.b.y>b.a.y){
    vline(b.a.x,b.a.y+1,b.b.y-1);
    vline(b.b.x,b.a.y+1,b.b.y-1);}
  else {
    vline(b.a.x,b.b.y+1,b.a.y-1);
    vline(b.b.x,b.b.y+1,b.a.y-1);
  }  
}



void rectangle_rotated(INT16 x1,INT16 y1,INT16 x2,INT16 y2,INT16 cx,INT16 cy,INT16 angle)
{
  tpoint p[4];
  //char lop;
  
  x1-=border;y1-=border;
  x2+=border;y2+=border;
  p[0].x=x1;p[0].y=y1;
  p[1].x=x2;p[1].y=y1;
  p[2].x=x2;p[2].y=y2;
  p[3].x=x1;p[3].y=y2;
  /*if (angle!=0)
    for (lop=0;lop<4;lop++) 
      rotate_point(&p[lop].x,&p[lop].y,cx,cy,angle);*/
  
  line(p[0].x,p[0].y,p[1].x,p[1].y);
  line(p[1].x,p[1].y,p[2].x,p[2].y);
  line(p[2].x,p[2].y,p[3].x,p[3].y);
  line(p[3].x,p[3].y,p[0].x,p[0].y);
};

void  drawbound(INT16 x1,INT16 y1,INT16 x2,INT16 y2)
{
  #define s halfgadget
  INT16 halfx,halfy;
  x1-=border;y1-=border;x2+=border;y2+=border;
  rectangle(x1,y1,x2,y2);
  rectangle(x2-s,y2-s,x2+s,y2+s);
  rectangle(x1-s,y2-s,x1+s,y2+s);
  rectangle(x1-s,y1-s,x1+s,y1+s);
  rectangle(x2-s,y1-s,x2+s,y1+s);
  halfx=(x2-x1) >> 1;
  halfy=(y2-y1) >> 1;
  rectangle(halfx+x1-s,y2-s,halfx+x1+s,y2+s);
  rectangle(x2-s,halfy+y1-s,x2+s,halfy+y1+s);
  rectangle(halfx+x1-s,y1-s,halfx+x1+s,y1+s);
  rectangle(x1-s,halfy+y1-s,x1+s,halfy+y1+s);
};

INT16 lineangle(INT16 x1,INT16 y1,INT16 x2,INT16 y2)
{
  INT16 hyp,opp,adj;
  opp=abs(x2-x1);
  adj=abs(y2-y1);
  hyp=(INT16)sqrt((opp*opp)+(adj*adj));
  if (hyp==0)  
    hyp=1;
  return (INT16)(sin(sin(90)*opp)/hyp);
};

/*void drawArrow(INT16 x1,INT16 y1,INT16 x2,INT16 y2)
{
  line(x1,y1,x2,y2);
  drawbytes(x2-4,y2-4,(char far*)&circlepic,8);
  p2.x=x2+6;p2.y=x2+6;
  angle=lineangle(x1,y1,x2,y2);
  rotate_point(p1.x,p1.y,x2,y2,angle);
  rotate_point(p2.x,p2.y,x2,y2,angle);
  line(x2-1,y2-1,p1.x,p1.y);
  line(x2+1,y2+1,p2.x,p2.y);
};*/

void outtextxy3d(INT16 x,INT16 y,char *txt)
{
  outtextxy(x+1,y+1,txt);
  t_col=t_fillcol;
  outtextxy(x,y,txt);
};

void outtextxyu(INT16 x,INT16 y,char *txt,char underchar)
{
  char lop;
  char *data;
  INT16 xpos=0;
  outtextxy(x,y,txt);
  if (underchar>0)
  {
    
    lop=t_charset.ysize;
    UINT8 width=t_charset.offsets[underchar].width;
    UINT8 i=0;
    data=t_charset.offsets[txt[underchar]].data;
    underchar--;
    while ((lop>=0) && (data[lop]==0))
      lop--;
    while (i<underchar)                                
        xpos+=t_charset.offsets[i++].width;
    
    xpos=x+(FNTgetwidthlength(txt,underchar));
    hline(xpos,xpos+width-2,y+2+lop);
  }
}

void  outtextxyu3d(INT16 x,INT16 y,char *txt,char underchar)
{
    outtextxyu(x+1,y+1,txt,underchar);
    t_col=t_fillcol;    
    outtextxyu(x,y,txt,underchar);      
}

void label(INT16 x,INT16 y,char *txt)
{
    tsize b;
    b.assign(x+4+FNTgetwidth(txt),y+t_charset.ysize+4);
    bar(x+1,y+1,b.x-1,b.y-1);
    rectangle(x,y,b.x,b.y);
    
    outtextxy(x+2,y+2,txt);        
}

void label2(INT16 x,INT16 y,char *txt)
{
    tsize b;
    b.assign(x+FNTgetwidth(txt),y+t_charset.ysize);
    bar(x,y,b.x,b.y);   
    outtextxy(x,y,txt);        
}

