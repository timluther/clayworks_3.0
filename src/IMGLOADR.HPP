#ifndef IMAGELOADER_H_
#define IMAGELOADER_H_
#include "gbasics.hpp"

//image file loading routines
//future extentions could include threaded loading

extern trgba lcmaprgb[256];
extern tcolour lcmap[256];

extern tpoint startpos;

extern UINT32 arch_palbase;

#define CTnone 0
#define CTrunlength 1
#define CTlzw 2
#define CTjpg 3
#define CTmixed 4


typedef struct imageinfoheader
{
    UINT32 type; //type of file
    tsize size;
    trect cliprect;
    UINT8 colourmode;
    UINT16 colourdepth;   //The amount of palette colour available with this mode}            
    UINT8 rsize,gsize,bsize,rpos,gpos,bpos,maskr,maskg,maskb; //for truecolour modes
    char infostr[512];   //could contain possible author information
    char compression; //compression type
    UINT32 flags; //extra flags
}imageinfoheader;

//pcx specific
BOOL getpcxmetrics(char *filename,UINT16 &w,UINT16 &h,UINT8 &bpp);
tbitmap* readpcx(char *filename,UINT8 type,UINT8 drawmode=normalput,UINT8 attrib=BMFdoublewordalign);
void writepcx(char *filename,tbitmap *bm);
//void printpcxheader(INT16 x,INT16 y,pcx_header &h);
//bmp specific
BOOL getbmpmetrics(char *filename,UINT16 &w,UINT16 &h,UINT8 &bpp);
tbitmap* readbmp(char *filename,UINT8 type,UINT8 drawmode=normalput,UINT8 attrib=BMFdoublewordalign);
void writebmp(char *filename,tbitmap *bm);
//void printbmpheader(INT16 x,INT16 y,BITMAPFILEHEADER &fhs,BITMAPINFO &h);
//gif specific
BOOL getgifmetrics(char *filename,UINT16 &w,UINT16 &h,UINT8 &bpp);
tbitmap* readgif(char *filename,UINT8 type,UINT8 drawmode=normalput,UINT8 attrib=BMFdoublewordalign);
void writegif(char *filename,tbitmap *bm);
//void printgifheader(INT16 x,INT16 y,BITMAPFILEHEADER &fhs,BITMAPINFO &h);

BOOL getarchmetrics(char *filename,UINT16 &w,UINT16 &h,UINT8 &bpp);
tbitmap* readarch(char *filename,UINT8 type,UINT8 drawmode=normalput,UINT8 attrib=BMFdoublewordalign);
BOOL loadarchdata(char *filename,char dopalette);
//generic
//for extracting palette information; returns null if not applicable (i.e. truecolour images)
trgba *getimage_rgbpalette();
tcolour *getimage_nativepalette();
//generic image load; uses the extention as a hint but if the check on the header fails
//it should check the header against other known types before eventually failing
tbitmap* readimage(char *filename,UINT8 type,UINT8 drawmode=normalput,UINT8 attrib=BMFdoublewordalign);



#endif
