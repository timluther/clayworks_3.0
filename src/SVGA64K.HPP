#ifndef _svga64k_h_
#define _svga64k_h_


#include "gbasics.hpp"


//256 colour flat mode graphics library; No 64k pages.
void drawcolour();

void svga64k_close();
char svga64k_setmode(screenIDType &sid);
tcolour svga64k_getcolour(UINT8 r,UINT8 g,UINT8 b);
tcolour svga64k_getpixel(PIXINT x,PIXINT y);
trgba svga64k_getrgbpixel(PIXINT x,PIXINT y);
void svga64k_putpixel(PIXINT x,PIXINT y);
void svga64k_hline(PIXINT x1,PIXINT x2,PIXINT y);
void svga64k_vline(PIXINT x,PIXINT y1,PIXINT y2);
void svga64k_line(PIXINT x1,PIXINT y1,PIXINT x2,PIXINT y2);
void svga64k_bar(PIXINT x1,PIXINT y1,PIXINT x2,PIXINT y2);
void svga64k_scanmode(UINT8 mode);
void svga64k_tri(trivert *v0,trivert *v1,trivert *v2);
void svga64k_putbitmap(PIXINT x,PIXINT y,tbitmap *pic,char useglobalattrib);
void svga64k_getbitmap(PIXINT x,PIXINT y,tbitmap *pic);
void svga64k_circle(PIXINT cx,PIXINT cy,PIXINT radius);
void svga64k_outtextxy(PIXINT cx,PIXINT cy,char *txt,PIXINT slen);
void svga64k_drawbytes(PIXINT x,PIXINT y,UINT8 w,UINT8 h,char *bytes);
void svga64k_cleardevice(void);
extern trgba conv16to24[65536];
#endif
