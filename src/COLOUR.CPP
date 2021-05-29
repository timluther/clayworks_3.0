//WATCOM C VGA colour library
#include <stdlib.h>
#include <math.h>
#include "ttypes.hpp"
#include "gbasics.hpp"

UINT16 maxcolours,coloursavailable;

trgba *colourmap;
UINT16 *colourusage;

char COLinit(UINT16 _maxcolours)
{
    maxcolours=coloursavailable=_maxcolours;    
    if (!(colourmap=new trgba[maxcolours])) return 0;
    if (!(colourusage=new UINT16[maxcolours])) return 0;
    //clear colourusage    
    return 1;
}

void COLdone()
{
    if (colourmap)
      delete colourmap;
}

extern void _setrgb(UINT8 colno,UINT8 r,UINT8 g,UINT8 b);
#pragma aux _setrgb = \
  "mov dx,3c8h"\
  "out dx,al"\
  "inc dx"\
  "mov al,ah"\
  "out dx,al"\
  "mov al,bl"\
  "out dx,al"\
  "mov al,bh"\
  "out dx,al"\
  parm[al][ ah] [bl][ bh]\
  modify[dx];



void COLsetrgb(UINT8 colno,UINT8 red,UINT8 green,UINT8 blue)
{
  colourmap[colno].r=red;
  colourmap[colno].g=green;
  colourmap[colno].b=blue;
  _setrgb(colno,red,green,blue);  
}


INT8 COLallocatergb(UINT8 rc,UINT8 gc,UINT8 bc)
{
  UINT16 currcol;
  INT8 matchindex;
  INT8 colfound;
  UINT16 diff,match;

  currcol=0;
  match=32000;
  colfound=FALSE;

  while ((!colfound)&&(currcol<=maxcolours))
  { 
    if ((colourmap[currcol].r==rc)&&(colourmap[currcol].g==gc)&&(colourmap[currcol].b==bc))
      colfound=TRUE;
    if ((colfound!=TRUE)&&(coloursavailable<=0))
    {
      diff=abs(gc-colourmap[currcol].g)*gwt;
      if (diff<match)
      {
        diff+=(abs(rc-colourmap[currcol].r)*rwt+abs(bc-colourmap[currcol].b)*bwt);
        if (diff<match){        
          match=diff;
          matchindex=currcol;
        }
      }
    }
    currcol++;
  }
  currcol--;
  
  if ((colfound)&&(colourusage[currcol]>0)){
    colourusage[currcol]++;
    return currcol;
  }else if (coloursavailable>0){
    currcol=0;
    while ((colourusage[currcol]!=0)&&(currcol<maxcolours))
      currcol++;
    colourusage[currcol]=1;
    COLsetrgb(currcol,rc,gc,bc);
    coloursavailable--;
    return currcol;
  }else
  {
    colourusage[matchindex]++;
    return matchindex;
  }
}

void COLsetpal(trgba *pal,UINT8 start,UINT8 len)
{
  UINT8 lop;
  trgba *currcol;
  currcol=pal;
  for (lop=start;lop<start+(len-1);lop++){
    colourmap[lop].r=currcol->r;
    colourmap[lop].g=currcol->g;
    colourmap[lop].b=currcol->b;
    currcol++;
  } 
  pal+=start;
  //_setpal(pal,len,sizeof(trgba));
}

void COLgetpal(trgba *pal)
{
  INT8 lop;
  trgba *currcol;
  currcol=pal;
  for (lop=0;lop<=maxcolours;lop++){
    currcol->r=colourmap[lop].r;
    currcol->g=colourmap[lop].g;
    currcol->b=colourmap[lop].b;
    currcol++;
  }
}

void COLgethardwarepal(trgba *pal)
{
_asm
{
  mov esi,pal
  mov cl,256
  mov dx,0x3C7
  mov al,0
  out dx,al
  mov dx,0x3C9
  inc dx 
  _colloop:
      in al,dx
      shl al,2
      mov [esi],al
      in al,dx
      shl al,2 
      mov [esi+1],al
      in al,dx
      shl al,2 
      mov [esi+2],al
      add edi,4
      dec cx
  jnz _colloop
}    
}


void COLsethardwarepal(trgba *pal,UINT16 start, UINT16 len)
{
  //pal+=start;
  _asm
  {
  mov esi,pal
  mov bx,start
  mov cx,len
  inc cx
  sub cx,bx
  mov dx,0x3C6
  mov al,255
  out dx,al
  inc dx
  
  _colloop:
      mov dx,0x3C8
      mov al,bl
      out dx,al
      inc dx
      mov al,[esi]
      shr al,2
      out dx,al 
      mov al,[esi+1]
      shr al,2
      out dx,al 
      mov al,[esi+2]
      shr al,2
      out dx,al 
      add esi,4
      inc bx
      dec cx
   jnz _colloop
  }
}   

void COLgetrgb(UINT8 colno,UINT8 *red,UINT8 *green,UINT8 *blue)
{
  *red=colourmap[colno].r;
  *green=colourmap[colno].g;
  *blue=colourmap[colno].b;
}  

void COLsethsv(trgba *col,float h,float s,float v)
{
  float r,g,b,f,q,p,t;  int i;
  h*=6;
  s/=255;
  v/=255;
  if (s==0){
    r=v;g=v;b=v;
  }else{
    if (h>=1530) 
      h=0;
    h/=255;
    i=(INT16)h;
    f=h-i;
    p=v*(1-s);
    q=v*(1-(s*f));
    t=v*(1-(s*(1-f)));
    //with 1/6 is Hue in?}
    switch (i){
      case 0:r=v;g=t;b=p;break;
      case 1:r=q;g=v;b=p;break;
      case 2:r=p;g=v;b=t;break;
      case 3:r=p;g=q;b=v;break;
      case 4:r=t;g=p;b=v;break;
      case 5:r=v;g=p;b=q;break;
    }  
   }
   *col=trgba((INT16)(r*255),(INT16)(g*255),(INT16)(b*255),0);
}


void COLgethsv(trgba *col,UINT8 &hi,UINT8 &si,UINT8 &vi)
{
  float max,min,diff,r_dist,g_dist,b_dist,r,g,b,h,s,v;;
  UINT8 ri,gi,bi;

  ri=col->r;
  gi=col->g;
  gi=col->b;
  
  r=ri/255; g=gi / 255; b=bi / 255;
  if (r>g) max=r; else max=g;
  if (b>max) max=b;
  if (r<g) min=r; else min=g;
  if (b<min) min=b;
  diff=max-min;
  v=max;
  if (max!=0) s=diff/max; else s=0;
  if (s=0)
    h=0;
  else {
    r_dist=(max - r)/diff;
    g_dist=(max - g)/diff;
    b_dist=(max - b)/diff;
    if (r==max) 
      h=b_dist-g_dist;
    else if (g==max)
      h=2+r_dist-b_dist;
    else if (b==max) 
      h=4+g_dist-r_dist;
    h=h * 255;
    if (h<0) h+=1530;
  }
  hi=(INT16)(h / 255);
  si=(INT16)(s * 255);
  vi=(INT16)(v * 255);
}
/*
void gethsv(trgba col,UINT8 *hi,UINT8 *si,UINT8 *vi)
{
  float max,min,diff,r_dist,g_dist,b_dist,r,g,b,h,s,v;;
  UINT8 ri,gi,bi;

  ri=col.r;
  gi=col.g;
  gi=col.b;
  
  r=*ri/255; g=*gi / 255; b=*bi / 255;
  if (r>g) max=r; else max=g;
  if (b>max) max=b;
  if (r<g) min=r; else min=g;
  if (b<min) min=b;
  diff=max-min;
  v=max;
  if (max!=0) s=diff/max; else s=0;
  if (s=0)
    h=0;
  else {
    r_dist=(max - r)/diff;
    g_dist=(max - g)/diff;
    b_dist=(max - b)/diff;
    if (r==max) 
      h=b_dist-g_dist;
    else if (g==max)
      h=2+r_dist-b_dist;
    else if (b==max) 
      h=4+g_dist-r_dist;
    h=h * 255;
    if (h<0) h+=384;
  }
  *hi=(INT16)(h / 255);
  *si=(INT16)(s * 255);
  *vi=(INT16)(v * 255);
}
*/

void COLswaprgb(trgba *col1,trgba *col2)
{
  trgba tempcol;
  tempcol=*col1;
  *col1=*col2;
  *col2=tempcol;
}  
  
void COLswapcol(UINT8 col1,UINT8 col2)
{
  COLswaprgb((trgba*)&colourmap[col1],(trgba*)&colourmap[col2]);
  //no get hardware in sync
  _setrgb(col1,colourmap[col1].r,colourmap[col1].g,colourmap[col1].b);
  _setrgb(col2,colourmap[col2].r,colourmap[col2].g,colourmap[col2].b);

}


void COLcopycol(UINT8 col1,UINT8 col2)
{
    COLsetrgb(col2,colourmap[col1].r,colourmap[col1].g,colourmap[col1].b);
}


void COLgrad(trgba *pal,UINT16 col1,UINT16 col2)
{
  #define colshift 8
  INT16 rplus,bplus,gplus,aplus,rp,gp,bp,ap,step;  
  if (col2<col1) swap(col1,col2);
  step=1+(col2-col1);
  
  rplus=((pal[col2].r-pal[col1].r)<< colshift)/ step;
  gplus=((pal[col2].g-pal[col1].g)<< colshift)/ step;
  bplus=((pal[col2].b-pal[col1].b)<< colshift)/ step;
  aplus=((pal[col2].a-pal[col1].a)<< colshift)/ step;
    
  rp=(pal[col1].r<<colshift);
  gp=(pal[col1].g<<colshift);
  bp=(pal[col1].b<<colshift);
  ap=(pal[col1].a<<colshift);  
      for (;col1<col2;col1++)
      {
          pal[col1].r=rp>>colshift;
          pal[col1].g=gp>>colshift;
          pal[col1].b=bp>>colshift;
          pal[col1].a=ap>>colshift;                        
          rp+=rplus; gp+=gplus; bp+=bplus;ap+=aplus;
      }
  
}
void COLgrad(tcolour *pal,UINT16 col1,UINT16 col2,trgba tcol1,trgba tcol2)
{
  #define colshift 8
  INT16 rplus,bplus,gplus,rp,gp,bp,step;
  //,aplus,ap
  
  if (col2<col1) swap(col1,col2);
  step=1+(col2-col1);
  
  rplus=((tcol2.r-tcol1.r)<< colshift)/ step;
  gplus=((tcol2.g-tcol1.g)<< colshift)/ step;
  bplus=((tcol2.b-tcol1.b)<< colshift)/ step;
  //aplus=((tcol2.a-tcol1.a)<< colshift)/ step;
    
  rp=(tcol1.r<<colshift);
  gp=(tcol1.g<<colshift);
  bp=(tcol1.b<<colshift);
  //ap=(tcol1.a<<colshift);  
      for (;col1<col2;col1++)
      {
          pal[col1]=getcolour(rp>>colshift,gp>>colshift,bp>>colshift);
          //pal[col1].a=ap>>colshift;                        
          rp+=rplus; gp+=gplus; bp+=bplus;
          //ap+=aplus;
      }
  
}

