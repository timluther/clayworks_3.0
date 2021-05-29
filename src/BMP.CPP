#include <string.h>
#include <stdio.h>
#include <conio.h>
#include "imgloadr.hpp"
#include "gbasics.hpp"
#include "ttypes.hpp"


/* constants for the biCompression field */
#define BI_RGB        0L
#define BI_RLE8       1L
#define BI_RLE4       2L
#define BI_BITFIELDS  3L

typedef struct tagRGBQUAD
{     /* rgbq */
    UINT8    r;
    UINT8    g;
    UINT8    b;
    UINT8    res;
} RGBQUAD;

typedef struct tagBITMAPFILEHEADER
{    /* bmfh */
    UINT16    bfType;  //Specifies the type of file. This member must be BM. 
    UINT32    bfSize;  //Specifies the size of the file, in bytes.
    UINT16    bfReserved1;
    UINT16    bfReserved2;
    UINT32    bfOffBits; //Specifies the byte offset from the BITMAPFILEHEADER structure
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER
{    /* bmih */
    UINT32  biSize;  //number of bytes in this structure
    INT32   biWidth; //Specifies the width of the bitmap, in pixels. 
    INT32   biHeight;
    UINT16  biPlanes;// number of planes, set to 1
    UINT16  biBitCount;//Specifies the number of bits per pixel. This value must be 1, 4, 8, or 24.
    UINT32   biCompression; //BI_RGB BI_RLE8 BI_RLE4
    UINT32   biSizeImage;  //Specifies the size, in bytes, of the image. It is valid to set this member to zero if the bitmap is in the BI_RGB format.
    INT32    biXPelsPerMeter;
    INT32    biYPelsPerMeter;
    UINT32   biClrUsed;   //Specifies the number of color indexes in the color table
    UINT32   biClrImportant;
} BITMAPINFOHEADER;

typedef struct tagBITMAPINFO
{  /* bmi */    
    BITMAPINFOHEADER    bmiHeader;
    RGBQUAD             bmiColors[1];
} BITMAPINFO;


BITMAPFILEHEADER fileheader;
BITMAPINFOHEADER bh;

RGBQUAD *palette;


void printbmpheader(INT16 x,INT16 y,BITMAPFILEHEADER &fh,BITMAPINFOHEADER &h)
{
    char buf[100];

    sprintf(buf,"Type: %i",fh.bfType);
    outtextxy(x,y+=18,buf);
    sprintf(buf,"File Size: %i",fh.bfSize);
    outtextxy(x,y+=18,buf);
    sprintf(buf,"Reserved bytes: %i %i",fh.bfReserved1,fh.bfReserved2);
    outtextxy(x,y+=18,buf);
    sprintf(buf,"Offset to whatever: %i",fh.bfOffBits);
    outtextxy(x,y+=18,buf);                    
    
    sprintf(buf,"Structuresize: %i",h.biSize);
    outtextxy(x,y+=18,buf);
    sprintf(buf,"Width: %i",h.biWidth);
    outtextxy(x,y+=18,buf);
    sprintf(buf,"Height: %i",h.biHeight);
    outtextxy(x,y+=18,buf);
    sprintf(buf,"Number of planes %i",h.biPlanes);
    outtextxy(x,y+=18,buf);
    sprintf(buf,"Bit count %i",h.biBitCount);
    outtextxy(x,y+=18,buf);    
    sprintf(buf,"Compression: %i",h.biCompression);
    outtextxy(x,y+=18,buf);
    sprintf(buf,"Image size: %i",h.biSizeImage);
    outtextxy(x,y+=18,buf);
    sprintf(buf,"Pels x: %i",h.biXPelsPerMeter);
    outtextxy(x,y+=18,buf);
    sprintf(buf,"Pels y: %i",h.biYPelsPerMeter);
    outtextxy(x,y+=18,buf);
    sprintf(buf,"colours used %i",h.biClrUsed);
    outtextxy(x,y+=18,buf);
    sprintf(buf,"Important (MS?!?!) colours used %i",h.biClrImportant);                
    outtextxy(x,y+=18,buf);                     
}

BOOL getbmpmetrics(char *filename,UINT16 &w,UINT16 &h,UINT8 &bpp)
{
    FILE *f;
    f=fopen(filename,"rb");
    if (!f) return FALSE;    
    fread(&fileheader,14,1,f);   
    fread(&bh,sizeof(BITMAPINFOHEADER),1,f);
    w=bh.biWidth;
    h=bh.biHeight;
    bpp=bh.biBitCount;
    fclose(f);
    return TRUE;
}



BOOL loadbmpdata(char *filename)
{    
    UINT8 datain[4];
    UINT8 dummy;
    trgb temprgb;

    INT32 i,xi,padding,yi,xcount;
    FILE *f;    
    
    f=fopen(filename,"rb");
    if (!f) return FALSE;    
    fread(&fileheader,14,1,f);   
    fread(&bh,sizeof(BITMAPINFOHEADER),1,f);
    padding=sizeof(BITMAPINFOHEADER)-bh.biSize;

    if (padding>0)
    for (i=0;i<padding;i++)
       fread(&dummy,1,1,f);
          
    if (bh.biClrUsed>0)
    {
       //palette=new RGBQUAD[bh.biClrUsed];
       fread(lcmaprgb,sizeof(RGBQUAD)*bh.biClrUsed,1,f);
       for (i=0;i<bh.biClrUsed;i++)
           lcmap[i]=getcolour(lcmaprgb[i].b,lcmaprgb[i].g,lcmaprgb[i].r);
       delete palette;
    }       

    padding=bh.biWidth-((bh.biWidth >> 2)<<2);    
    switch (bh.biBitCount)
    {
        case 1:break;
        case 4:
        for (yi=0;yi<bh.biHeight;yi++)
        {
            xcount=0;
            for (xi=0;xi<bh.biWidth >> 1;xi++)
            {
                fread(datain,1,1,f);
                t_col=lcmap[datain[0] & 15];
                putpixel(xcount++,yi);
                t_col=lcmap[(datain[0]>>4) & 15];
                putpixel(xcount++,bh.biHeight-yi-1);                                    
            }
            if (padding)
               fread(datain,4-padding,1,f);
        }
        break;
        case 8:
        for (yi=0;yi<bh.biHeight;yi++)
        {
            for (xi=0;xi<bh.biWidth;xi++)
            {
                fread(datain,1,1,f);
                t_col=lcmap[datain[0]];
                putpixel(xi,bh.biHeight-yi-1);                                    
            }
            if (padding)
               fread(datain,4-padding,1,f);
               //padding
        }
        break;
        case 24:
        for (yi=0;yi<bh.biHeight;yi++)
        {
            for (xi=0;xi<bh.biWidth;xi++)
            {
                fread(datain,3,1,f);
                t_col=getcolour(datain[2],datain[1],datain[0]);
                putpixel(xi,bh.biHeight-yi-1);                                    
            }
            if (padding)
               fread(datain,padding,1,f);
        }
        break;        
    }    
    fclose(f);  
    return TRUE;
}

tbitmap* readbmp(char *filename,UINT8 type,UINT8 drawmode,UINT8 attrib)
{
    tsize size;
    UINT8 bpp;
    tbitmap *bm;

    screenDataType bmsc,oldsc;
    oldsc=SC;

    if (!getbmpmetrics(filename,size.x,size.y,bpp))return NULL;
    bm=new tbitmap(size.x,size.y,type,drawmode,attrib);
    createcontext(&bmsc,bm);
    setoutput(&bmsc);   

    loadbmpdata(filename);
    destroycontext(&bmsc);
    setoutput(&oldsc);
  
    return bm;        
}    
