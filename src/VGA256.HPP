#ifndef _vga256_h_
#define _vga256_h_


#include "gbasics.hpp"


//256 colour flat mode graphics library; No 64k pages.


void vga256_close();
char vga256_setmode(screenIDType &sid);
void vga256_putpixel(PIXINT x,PIXINT y);
tcolour vga256_getpixel(PIXINT x,PIXINT y);
void vga256_hline(PIXINT x1,PIXINT x2,PIXINT y);
void vga256_vline(PIXINT x,PIXINT y1,PIXINT y2);
void vga256_line(PIXINT x1,PIXINT y1,PIXINT x2,PIXINT y2);
void vga256_bar(PIXINT x1,PIXINT y1,PIXINT x2,PIXINT y2);
void vga256_convexpolygon(gpoint *points,UINT16 count);
void vga256_setpolymode(UINT8 mode);
void vga256_putbitmap(PIXINT x,PIXINT y,tbitmap *pic,char useglobalattribs);
void vga256_circle(PIXINT cx,PIXINT cy,PIXINT radius);
void vga256_outtextxy(PIXINT cx,PIXINT cy,char *txt,PIXINT slen);
void vga256_drawbytes(PIXINT x,PIXINT y,UINT8 w,UINT8 h,char *bytes);
void vga256_cleardevice(void);
tcolour vga256_getcolour(UINT8 r,UINT8 g,UINT8 b);

#endif
