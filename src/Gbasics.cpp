//Graphical basics unit -defines points, rectangles, etc.

#include "stdafx.h"
#include <stdlib.h>
#include <conio.h>
#include <string.h>

#include "gbasics.hpp"
#include "ttypes.hpp"

//stored bpps
UINT8 vidmodebytesizes[9]={0,1,1,1,1,2,2,3,4};
UINT8 vidmodebpp[9]={0,1,1,8,8,15,16,24,32};
UINT8 vidpalmode[9]={0,PMindexed,PMindexed,PMindexed,PMindexed,PMdirect,PMdirect,PMdirect,PMdirect};


//rectangle list stuff
theapmem rectbuffer;
UINT8 rectlist_status;
UINT32 rect_count;
trectlist *lastrect;

//Current Colours & writemode }
//there is a distinction between the remaped palette and the actual palette
//in all modes but 32 and 24 bit colour, the 8 channel rgb palettes are different
//and some functions need to know the rgb colours, rather than the 'native' mode
//colours.. a function will be provided to load up a palette from 8chan to native
UINT32 t_maxcolours;
tcolour t_fillcol,t_col,t_chroma,*t_nativepalette,*t_nativebitmappalette;
trgba t_colrgba;
trgba *t_palette,*t_bitmappalette;
UINT8 t_alpha;
tbitmap *t_texture;
//used in texture mapping
char *t_texturedataptr;
UINT32 *t_texturestartoffsets;
tsize  t_texturesize;
float t_texturewidthscale;
float t_textureheightscale;


UINT8 t_writemode;
UINT8 t_linepattern;
//should be pointer...
tfont t_charset;
//for fill patterns
UINT32 maskword;
//char maskbyte

fillpatterntype fillpattern;
char maskbackground,nullpattern;
screenIDType SVGA320x200x256={STsvga_flat,{320,200},0,0,0};
screenIDType SVGA640x400x256={STsvga_flat,{640,400},0,0,0};
screenIDType SVGA640x480x256={STsvga_flat,{640,480},0,0,0};
screenIDType SVGA800x600x256={STsvga_flat,{800,600},0,0,0};
screenIDType SVGA1024x768x256={STsvga_flat,{1024,768},0,0,0};
screenIDType SVGA1280x1024x256={STsvga_flat,{1280,1024},0,0,0};

screenIDType SVGA320x200x32k={STsvga_flat32k,{640,400},5,5,5};
screenIDType SVGA640x480x32k={STsvga_flat32k,{640,480},5,5,5};
screenIDType SVGA800x600x32k={STsvga_flat32k,{800,600},5,5,5};
screenIDType SVGA1024x768x32k={STsvga_flat32k,{1024,768},5,5,5};
screenIDType SVGA1280x1024x32k={STsvga_flat32k,{1280,1024},5,5,5};

screenIDType SVGA320x200x64k={STsvga_flat64k,{320,200},5,6,5};
screenIDType SVGA320x240x64k={STsvga_flat64k,{320,240},5,6,5};
screenIDType SVGA640x480x64k={STsvga_flat64k,{640,480},5,6,5};
screenIDType SVGA800x600x64k={STsvga_flat64k,{800,600},5,6,5};
screenIDType SVGA1024x768x64k={STsvga_flat64k,{1024,768},5,6,5};
screenIDType SVGA1280x1024x64k={STsvga_flat64k,{1280,1024},5,6,5};

screenIDType SVGA320x200x24b={STsvga_flat24b,{320,200},8,8,8};
screenIDType SVGA320x240x24b={STsvga_flat24b,{320,240},8,8,8};
screenIDType SVGA640x480x24b={STsvga_flat24b,{640,480},8,8,8};
screenIDType SVGA800x600x24b={STsvga_flat24b,{800,600},8,8,8};
screenIDType SVGA1024x768x24b={STsvga_flat24b,{1024,768},8,8,8};
screenIDType SVGA1280x1024x24b={STsvga_flat24b,{1280,1024},8,8,8};

screenIDType SVGA320x200x16m={STsvga_flat32b,{320,200},8,8,8};
screenIDType SVGA320x240x16m={STsvga_flat32b,{320,240},8,8,8};
screenIDType SVGA640x480x16m={STsvga_flat32b,{640,480},8,8,8};
screenIDType SVGA800x600x16m={STsvga_flat32b,{800,600},8,8,8};
screenIDType SVGA1024x768x16m={STsvga_flat32b,{1024,768},8,8,8};
screenIDType SVGA1280x1024x16m={STsvga_flat32b,{1280,1024},8,8,8};

screenIDType VGA320x200x16={STsvga_plainiar,{320,200},0,0,0}; //0xd
screenIDType VGA640x200x16={STsvga_plainiar,{320,200},0,0,0}; //0xe
screenIDType VGA640x350x16={STsvga_plainiar,{320,200},0,0,0}; //0x10
screenIDType VGA640x480x16={STsvga_plainiar,{320,200},0,0,0}; //0x12
screenIDType SVGA800x600x16={STsvga_plainiar,{320,200},0,0,0}; //102
screenIDType SVGA1024x768x16={STsvga_plainiar,{320,200},0,0,0}; //104
screenIDType SVGA1280x1024x16={STsvga_plainiar,{320,200},0,0,0}; //106

screenDataType SC,IC,SC_BASE;  //screen & bitmap context}

UINT8 singlepixelbitmasks[8]={128,64,32,16,8,4,2,1};
UINT8 left_pixelbitmasks[8]={255,127,63,31,15,7,3,1};
UINT8 right_pixelbitmasks[8]={0,128,192,224,240,248,252,254};

void (*closemode)(void);
char (*setmode)(screenIDType &sid);
void (*putpixel)(PIXINT x,PIXINT y);
void (*drawscanline)(char *adrs,UINT32 count);
tcolour (*getcolour)(UINT8 r,UINT8 g,UINT8 b);
tcolour (*getpixel)(PIXINT x,PIXINT y);
trgba (*getrgbpixel)(PIXINT x,PIXINT y);
void (*hline)(PIXINT x1,PIXINT x2,PIXINT y);
void (*vline)(PIXINT x,PIXINT y1,PIXINT y2);
void (*line)(PIXINT x1,PIXINT y1,PIXINT x2,PIXINT y2);
void (*bar)(PIXINT x1,PIXINT y1,PIXINT x2,PIXINT y2);
void (*tri)(trivert *v0,trivert *v1,trivert *v2);
void (*putbitmap_base)(PIXINT x,PIXINT y,tbitmap *pic,char useglobalattribs);
void (*getbitmap)(PIXINT x,PIXINT y,tbitmap *pic);
void (*circle)(PIXINT cx,PIXINT cy,PIXINT radius);
void (*outtextxy_base)(PIXINT cx,PIXINT cy,char *txt,PIXINT slen);
void (*drawbytes)(PIXINT x,PIXINT y,UINT8 w,UINT8 h,char *bytes);
void (*cleardevice)(void);

UINT8 screenIDType::getbpp(){return vidmodebpp[driver];}
UINT8 surfaceData::getpalmode(){return vidpalmode[driver];}
UINT8 surfaceData::getbpp(){return vidmodebpp[driver];}
UINT8 surfaceData::getbytesize(){return vidmodebytesizes[driver];}


tbitmap::tbitmap(tbitmap *bm)   //possibly add a filter in here
{
    *this=*bm;
    scrptr = new char[pagesize];
    attrib|=BMFowndata;
    UINT32 *offsets=startoffsets= new UINT32[size.y];     
    for (UINT32 i=0;i<size.y;i++)    
        *offsets++=(UINT32)scrptr+(i*offsd);        
    memcpy(scrptr,bm->scrptr,pagesize);
}



tbitmap::tbitmap(UINT32 x,UINT32 y,UINT32 xlen,UINT32 ylen,tbitmap *src,
            UINT8 drawmode_,UINT8 newdata)
{
    UINT8 *olddata,*currline;
    UINT32* offsets;
    UINT32 i;    
    
    palette=src->palette;
    drawmode=drawmode_;
    attrib=src->attrib;
    driver=src->driver;    
    size.x=xlen;
    size.y=ylen;
    scrptr=src->scrptr+(x*vidmodebytesizes[driver])+(y*src->offsd);
    if (newdata)
    {
        olddata=(UINT8*)scrptr;
        getoffs();
        pagesize=offsd*(size.y);
        currline=new UINT8[pagesize];                
                scrptr=(char*)currline;
        for (i=0;i<ylen;i++)
        {
            memcpy(currline,olddata,offsd);
            olddata+=src->offsd;
            currline+=offsd;            
        }
        attrib|=BMFowndata;
    }else
    {
        attrib&=~BMFowndata;
        offsd=src->offsd;
        pagesize=offsd*ylen;
    }
    
    offsets=startoffsets=new UINT32[ylen];
    for (i=0;i<ylen;i++)    
        *offsets++=(UINT32)scrptr+(i*offsd);    
}

tbitmap::tbitmap(UINT32 xlen,UINT32 ylen,UINT8 type_,UINT8 drawmode_,UINT8 attrib_,char *data,tcolour *palette_)
{
    UINT32* offsets;
    UINT32 i;
    size.x=xlen;
    size.y=ylen;
    drawmode=drawmode_;
    attrib=attrib_;
    driver=type_;
    
    getoffs();
    if ( ((attrib & BMFwordalign )!=0) &&((offsd & 3)!=0))
        offsd+=(4-(offsd & 3));  //8 byte align
    else if ( ((attrib & BMFdoublewordalign)!=0)&&((offsd & 7)!=0))        
        offsd+=(8-(offsd & 7));  //8 byte align
                                                
    pagesize=offsd*(ylen);
    palette=palette_;
    if (!data)
    {
       scrptr = new char[pagesize];
       attrib|=BMFowndata;
    }
    else
    {
       scrptr=data;
       attrib&=~BMFowndata;
    }     
    offsets=startoffsets= new UINT32[ylen];     
    for (i=0;i<ylen;i++)    
        *offsets++=(UINT32)scrptr+(i*offsd);                   
}

tbitmap::~tbitmap()
{
    if ((attrib & BMFowndata)!=0)
      delete scrptr;
}

void tbitmap::getoffs()
{
    offsd=size.x;
    switch (driver & BMFformat)
    {
        case BMplanes:offsd>>=3;break;
        case BMflat16:offsd+=offsd;break;
        case BMflat24:offsd+=size.x+size.x;break;
        case BMflat32:offsd<<= 2;break;
    }
}        

UINT32 tbitmap::getoffsetat(PIXINT x,PIXINT y)
{
    UINT32 off=x;
    switch (driver & BMFformat)
    {
        case BMplanes:x>>= 3;break;        
        case BMflat16:off+=x;break;
        case BMflat24:off+=x+x;break;               
        case BMflat32:off<<=2;break;
    }
    return off+(offsd*size.y);    
}

void tbitmap::resize(UINT32 xlen,UINT32 ylen)
{
    UINT32* offsets;
    UINT32 i;
    if ((size.x==xlen)&&(size.y==ylen))return;
    size.assign(xlen,ylen);
    getoffs();
        
    if ((attrib & BMFowndata)!=0)
    {
       if (scrptr!=NULL)       
           delete scrptr;
       scrptr = new char[pagesize];
    }
    if (startoffsets!=NULL)
       delete startoffsets;
    offsets=startoffsets=new UINT32[ylen];
    for (i=0;i<ylen;i++)    
        *offsets++=(UINT32)scrptr+(i*offsd);
    
    /*for (i=0;i<pagesize();i+=offsd)    
       *(offsets++)=(UINT32)scrptr+i;               */
}

void tbitmap::yflip()
{
}

//currently only works for 8bit size.x==offsd
void tbitmap::rotate90()
{
    UINT32 bpp=getbytesize();
    UINT32 addy=size.y*bpp;

    
    UINT8 *newdata=new UINT8[pagesize];
    UINT8 *src,*dst=newdata;
    
    for (UINT32 y=0;y<size.y;y++)
    {
      UINT8* olddst=dst;
      src=(UINT8*)startoffsets[y];
      for (UINT32 x=0;x<size.x;x++)
      {
          memmove(dst,src,bpp);
          //*dst=*src;
          dst+=addy;
          src+=bpp;          
      }
      dst=olddst;
      dst+=bpp;
    }
    swap(size.y,size.x);
    memcpy(scrptr,newdata,pagesize);
    getoffs();
    delete startoffsets;
    UINT32 *offsets=startoffsets=new UINT32[size.y];
    for (UINT32 i=0;i<size.y;i++)    
        *offsets++=(UINT32)scrptr+(i*offsd);    
}

template <class T> BOOL trecttemplate<T>::clipline(T &xa,T &ya,T &xb,T &yb)
{   
  unsigned char  code0,code1;
  T swapme;
  while (1)
  {
    code0=outcode(xb,yb);
    code1=outcode(xa,ya);            
    if ((code0 & code1) !=0)
      return(FALSE);//Trivial reject}
    else {
      if (!(code0 | code1)!=0)
         return(TRUE); //Trivial accept}
      else if (code0==0){
        swap(code1,code0);
        swapme=xb;xb=xa;xa=swapme;
        swapme=yb;yb=ya;ya=swapme;        
      }
      if ((code0 & bellow)!=0)
      {
        xb+=((xa-xb)*(b.y-yb)/(ya-yb));
        yb=b.y;
      }else if ((code0 & above)!=0)
      {
        xb+=((xa-xb)*(a.y-yb)/(ya-yb));
        yb=a.y;
      }else if ((code0 & right)!=0)
      {
        yb+=((ya-yb)*(b.x-xb)/(xa-xb));
        xb=b.x;
      }else if ((code0 & left)!=0)
      {
        yb+=((ya-yb)*(a.x-xb)/(xa-xb));
        xb=a.x;
      }
    }
  }
}

inline void swapb(UINT8 &a, UINT8 &b){UINT8 t;t=a;a=b;b=t;}

void tbitmap::xflip()
{
    //UINT8 data=new UINT8[offsd];
    UINT8 *scra,*scrb;
    UINT8 bytesize=1;
    for (UINT32 y=0;y<size.y;y++)
    {        
        scra=(UINT8*)startoffsets[y];
        scrb=scra+(size.x*bytesize);
        for (INT32 x=0;x<(size.x >> 1)-1;x++)
        {
            UINT8 t=*scra;*scra=*scrb;*scrb=t;            
            scra++;
            scrb--;                                           
        }        
    }    
}

//trectlist *cutfrom(trecttemplate &r);
template <class T> trectlist* trecttemplate<T>::cutfrom(trecttemplate r)
{
    trectlist *newlist;
        
    
    if (!(r.rintersect(*this)))
    {
        rect_count=1;
        lastrect=getrectmem();
        lastrect->rassign(a.x,a.y,b.x,b.y);
        rectlist_status=RSnooverlap;
        lastrect->next=NULL;
        return lastrect;
    }
    rect_count=0;
    lastrect=NULL;
    if (*this==r)
    {
        rectlist_status=RSobscured;
        return NULL;
    }    
    newlist=lastrect=(trectlist*)rectbuffer.curr;
    
    if (a.y<r.a.y)
    {
         lastrect=lastrect->next=getrectmem();
         rect_count++;
         lastrect->rassign(a.x,a.y,b.x,r.a.y-1);
    };
    if (a.x<r.a.x)
    {
         lastrect=lastrect->next=getrectmem();
         rect_count++;
         lastrect->rassign(a.x,r.a.y,r.a.x-1,r.b.y);
    }
        
    if (b.x>r.b.x)
    {
        lastrect=lastrect->next=getrectmem();
        rect_count++;
        lastrect->rassign(r.b.x+1,r.a.y,b.x,r.b.y);
    }
    if (b.y>r.b.y)
    {
        lastrect=lastrect->next=getrectmem();
        rect_count++;
        lastrect->rassign(a.x,r.b.y+1,b.x,b.y);
    }
    
    if (rect_count>0)
    {
        rectlist_status=RSlist;
        lastrect->next=NULL;
        return newlist;
    }else
    {
        rectlist_status=RSobscured;
        return NULL;        
    }        
}


void destroyplist(trectlist *p)
{
    trectlist *curr_p,*next_p;
    curr_p=p;
    while (curr_p!=NULL)
    {
        next_p=curr_p->next;
        delete curr_p;
        curr_p=next_p;
    }
}

trectlist* getrectmem()
{
    return (trectlist*)rectbuffer.grab(sizeof(trectlist));
}



trectlist* ungetrectmem()
{
    return (trectlist*)rectbuffer.ungrab(sizeof(trectlist));
}

screenDataType *createcontext(screenDataType *sc,tbitmap *bm)
{
    sc->driver=bm->driver;
    sc->size=bm->size;
    sc->colourdepth=1<<vidmodebpp[sc->driver];
    sc->scrptr=bm->scrptr;
    sc->palette=bm->palette;
    sc->offsd=bm->offsd;
    sc->memavail=bm->memavail;
    sc->pagesize=bm->pagesize;
    sc->startoffsets=bm->startoffsets;                       
    sc->viewport.rassign(sc->size.x,sc->size.y);
    return sc;
}    
//////////////////////////////////////////////////////////////////////////////
//trectfloat


tpointfloat *zrotatepoint(tpointfloat *in,tpointfloat *out,flt r)
{    
    flt cr=cos(r);
    flt sr=sin(r);
    out->x=(float)(in->x*cr + in->y*sr);
    out->y=(float)(in->x*-sr+ in->y*cr);
    return out;
}                

tpointfloat *zrotatepointaround(tpointfloat in,tpointfloat *out,flt r,tpointfloat *around)
{    
    flt cr=cos(r);
    flt sr=sin(r);
    in-=*around;
    out->x=(float)(in.x*cr + in.y*sr);
    out->y=(float)(in.x*-sr+ in.y*cr);
    *out+=*around;
    return out;
}                

void destroycontext(screenDataType *sc)
{
    sc->driver=0;    
}


void settexture(tbitmap *tex)
{
    t_texture=tex;
    t_texturedataptr=tex->scrptr;
    t_texturestartoffsets=tex->startoffsets;
    t_texturesize=tex->size;
    t_textureheightscale=(tex->size.y-1)*65536.0f;
    t_texturewidthscale=(tex->size.x-1)*65536.0f;
}

