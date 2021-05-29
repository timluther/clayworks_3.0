#ifndef PCX_H_
#define PCX_H_
#include "gbasics.hpp"


typedef struct pcx_header
{
    UINT8 header;
    UINT8 version;
    UINT8 encoding;
    UINT8 bpp;
    trect window;
    tpoint resolution;
    trgb cmap[16];
    UINT8 reserved;
    UINT8 numplanes;
    UINT16 offset;
    UINT16 PaletteInfo;
    tpoint screensize;
    UINT8 filler[54];        
}pcx_header;

extern tpoint startpos;
extern pcx_header header;

trgba *getpcxrgbpalette();
tcolour *getpcxnativepalette();

BOOL getpcxmetrics(char *filename,UINT16 &w,UINT16 &h,UINT8 &bpp);
tbitmap* readpcx(char *filename,UINT8 type,UINT8 drawmode=normalput,UINT8 attrib=BMFdoublewordalign);
void writepcx(char *filename,tbitmap *bm);
void printpcxheader(INT16 x,INT16 y,pcx_header &h);

#endif
