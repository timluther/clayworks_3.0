//graphics primatives header file
//these are device independent functions that use the device dependent primatives
//defined in ggraph.hpp
//They are not classed by type, rather they are loosley connected useful graphics functions

#ifndef GPRIM_H
#define GPRIM_H

#define nodesize 2
#define nodesizedec1 nodesize-1
#define border 4
#define gadgetsize 6
#define halfgadget gadgetsize/2

extern tcolour highlight; //for icons e.t.c.//
extern tcolour textcol;   //text colour//
extern tcolour titletextcol; //for window titles//
extern tcolour scrollcol; //scroller background//
extern tcolour lightcol;  //highlight colour//
extern tcolour midcol;    //middle colour//
extern tcolour mid_darkcol;   //shadow colour//
extern tcolour mid_darkcol2;   //shadow colour//
extern tcolour darkcol;   //shadow colour//
extern tcolour backcol;   //background colour//
extern tcolour gridcol;    //grid colour//
extern tcolour selectcol;  //selection colour//
extern tcolour titlecol;   //titlebar colour//
extern tcolour disabledcol;
extern tcolour menuselcolback; 
extern tcolour menuselcolfore;
extern tcolour scrollhighlight;
extern tcolour textboxcol;

extern UINT8 circlepic[8];
extern UINT8 tickpic[8];
extern UINT8 closegadget[8];
extern UINT8 uparrow[8];
extern UINT8 downarrow[8];
extern UINT8 resizegadget[8];
extern UINT8 leftarrow[8];
extern UINT8 rightarrow[8];
extern UINT8 updownarrow[8];
extern trgbbase greypal[16];


void drawarrow(flt x1,flt y1,flt x2,flt y2,flt head,flt tail,flt width);
void captilebitmapy(INT16 x,INT16 y,INT16 y2,tbitmap **bm,UINT8 useattrib=0);
void captilebitmapx(INT16 x,INT16 y,INT16 x2,tbitmap **bm,UINT8 useattrib=0);
void captilebitmapy(INT16 x,INT16 y,INT16 y2,tbitmap *bm1,tbitmap *bm2,tbitmap *bm3);
void captilebitmapx(INT16 x,INT16 y,INT16 x2,tbitmap *bm1,tbitmap *bm2,tbitmap *bm3);
void tilebitmapx(INT16 x,INT16 y,INT16 x2,tbitmap *bm,UINT8 useattrib=0);
void tilebitmapy(INT16 x,INT16 y,INT16 y2,tbitmap *bm,UINT8 useattrib=0);
void outtextxy3d(INT16 x,INT16 y,char *txt);
void outtextxyu(INT16 x,INT16 y,char *txt,char underchar);
void outtextxyu3d(INT16 x,INT16 y,char *txt,char underchar);
void drawx(INT16 x,INT16 y);
void drawcursor(INT16 x,INT16 y);
void grid(INT16 x,INT16 y,INT16 gxsize,INT16 gysize,INT16 stpx,INT16 stpy);
void dotgrid(INT16 x,INT16 y,INT16 gxsize,INT16 gysize,INT16 stpx,INT16 stpy);
void boxgrid(INT16 x,INT16 y,INT16 gxsize,INT16 gysize,INT16 stpx,INT16 stpy);
void colourgrid2(INT16 x,INT16 y,INT16 gxsize,INT16 gysize,INT16 stpx,INT16 stpy);
void colourgrid(INT16 x,INT16 y,INT16 gxsize,INT16 gysize,INT16 stpx,INT16 stpy);
void outbox(INT16 x1,INT16 y1,INT16 x2,INT16 y2);
void outboxcol(INT16 x1,INT16 y1,INT16 x2,INT16 y2);
void inbox(INT16 x1,INT16 y1,INT16 x2,INT16 y2);
void inboxcol(INT16 x1,INT16 y1,INT16 x2,INT16 y2);
void inboxcolb(INT16 x1,INT16 y1,INT16 x2,INT16 y2);
void insetborder(INT16 x1,INT16 y1,INT16 x2,INT16 y2,char *txt);
void textcurs(INT16 x,INT16 y,INT16 y2);
void textsel(INT16 x,INT16 y,INT16 x2);
void textbox(INT16 x1,INT16 y1,INT16 x2,INT16 y2);
void textboxfilled(INT16 x1,INT16 y1,INT16 x2,INT16 y2);
void drawnode(INT16 x,INT16 y);
void hline3d(INT16 x1,INT16 x2,INT16 y1);
void draw3dborder(INT16 x1,INT16 y1,INT16 x2,INT16 y2,char inv);
void rectangle_rotated(INT16 x1,INT16 y1,INT16 x2,INT16 y2,INT16 cx,INT16 cy,INT16 angle); 
void thickrectangle(INT16 x1,INT16 y1,INT16 x2,INT16 y2,INT16 thickness);
void rectangle(INT16 x1,INT16 y1,INT16 x2,INT16 y2);
void rectangle(trect b);
void drawbound(INT16 x1,INT16 y1,INT16 x2,INT16 y2);
void drawarrow(INT16 x1,INT16 y1,INT16 x2,INT16 y2);
INT16 lineangle(INT16 x1,INT16 y1,INT16 x2,INT16 y2);
void drawborder(INT16 x1,INT16 y1,INT16 x2,INT16 y2,UINT8 borderwidth);
void label2(INT16 x,INT16 y,char *txt);
void label(INT16 x,INT16 y,char *txt);

inline void linef(flt x1,flt y1,flt x2,flt y2)
{
  line(float2int(x1),float2int(y1),float2int(x2),float2int(y2));  
}

inline void rectanglef(flt x1,flt y1,flt x2,flt y2)
{  
   rectangle(float2int(x1),float2int(y1),float2int(x2),float2int(y2));  
}



#endif
