#include <string.h>
#include <string.h>
#include <stdio.h>
#include "imgloadr.hpp"
#include "gbasics.hpp"
#include "ttypes.hpp"

#define headerchar 10
#define no_bytes_back 769
#define check256val 12
#define chkbits 0xC0
#define maskbits 0x3f


typedef struct pcx_header
{
    UINT8 header;
    UINT8 version;
    UINT8 encoding;
    UINT8 bpp;
    trect16 window;
    tpoint16 resolution;
    trgb cmap[16];
    UINT8 reserved;
    UINT8 numplanes;
    UINT16 offset;
    UINT16 PaletteInfo;
    tpoint16 screensize;
    UINT8 filler[54];        
}pcx_header;

pcx_header header;


void printpcxheader(INT16 x,INT16 y,pcx_header &h)
{
    char buf[100];
    sprintf(buf,"Headerbyte: %i",h.header);
    outtextxy(x,y+=18,buf);
    sprintf(buf,"Version: %i",h.version);
    outtextxy(x,y+=18,buf);
    sprintf(buf,"Encoding: %i",h.encoding);
    outtextxy(x,y+=18,buf);
    sprintf(buf,"bpp: %i",h.bpp);
    outtextxy(x,y+=18,buf);    
    sprintf(buf,"window: width:%i height:%i",h.window.width(),h.window.height());
    outtextxy(x,y+=18,buf);    
    sprintf(buf,"resolution: x:%i y:%i",h.resolution.x,h.resolution.y);
    outtextxy(x,y+=18,buf);
    sprintf(buf,"planes: %i",h.numplanes);
    outtextxy(x,y+=18,buf);
    sprintf(buf,"offset: %i",h.offset);
    outtextxy(x,y+=18,buf);
    sprintf(buf,"Paletteinfo: %i",h.PaletteInfo);
    outtextxy(x,y+=18,buf);
    sprintf(buf,"screensize: x: %i y: %i",h.screensize.x,h.screensize.y);            
    outtextxy(x,y+=18,buf);
    int i;
    for (i=0;i<16;i++)
    {
        sprintf(buf,"Pal %1: r:%i g:%i b:%i",i,header.cmap[i].r,header.cmap[i].g,header.cmap[i].b);
        outtextxy(x,y+=18,buf);
    }                    
}



UINT8 *loadscanline(UINT8 *scanptr,UINT8 val,UINT8 bitplane,UINT32 count)
{

    bitplane=1 << bitplane;
    
    while (count>0)
    {        
        if (val & 0x80)
            *scanptr|=bitplane;
        scanptr++;
        if (val & 0x40)
            *scanptr|=bitplane;
        scanptr++;            
        if (val & 0x20)
            *scanptr|=bitplane;
        scanptr++;            
        if (val & 0x10)
            *scanptr|=bitplane;
        scanptr++;            
        if (val & 0x8)
            *scanptr|=bitplane;
        scanptr++;            
        if (val & 0x4)
            *scanptr|=bitplane;
        scanptr++;            
        if (val & 0x2)
            *scanptr|=bitplane;
        scanptr++;            
        if (val & 0x1)
            *scanptr|=bitplane;
        scanptr++;            
        count--;
    }
    return scanptr;
    
}

tpoint startpos;

extern char maskit(UINT8);
#pragma aux maskit=\
"and al,0x0C"\
parm [al]    

//SEEK_SET, SEEK_END, SEEK_CUR
  /*if (pos.y==0)
                    {
                        t_col=lcmap[255];
                        sprintf(buf,"%i",chk);
                        
                        outtextxy(fontpos,300,buf);
                        fontpos+=strlen(buf)<<3;
                    }*/

BOOL getpcxmetrics(char *filename,UINT16 &w,UINT16 &h,UINT8 &bpp)
{
    FILE *f;        
    if (!(f=fopen(filename,"rb"))) return FALSE;    
    fread(&header,128,1,f);
    w=header.window.width()+1;
    h=header.window.height()+1;
    bpp=header.bpp;
    fclose(f);
    return TRUE;
}

//loads PCX file into current device context
BOOL loadpcxdata(char *filename,char dopalette)
{
    UINT16 i;
    UINT8 *tempscan,*cbyte,planei;    
    UINT8 runlength;
    UINT8 chk;
    INT16 palsize=0;
    UINT32 scanwidth;
    UINT16 subtotal;
    tpoint wsize,pos;
    trgb temprgb;
    FILE *f;    
    
    if (!(f=fopen(filename,"rb"))) return NULL;
    fseek(f,sizeof(header),SEEK_CUR);    
    scanwidth=header.offset*header.numplanes;
    wsize.x=header.window.width()+1;
    wsize.y=header.window.height()+1;
          
    //load up palette
        
        if (header.bpp<=4)
        {
            palsize=16;
            //lcmap=(tcolour*)malloc(palsize*sizeof(tcolour));
            for (i=0;i<16;i++)
                lcmaprgb[i]=header.cmap[i];                
        }else
        {
            fseek(f,-no_bytes_back,SEEK_END);
            chk=fgetc(f);
            if (chk==check256val)
            {       
                palsize=256;          
                for (i=0;i<256;i++)
                {
                    fread(&lcmaprgb[i],sizeof(trgb),1,f);
                    lcmaprgb[i].a=0;
                }
            }
            fseek(f,128,SEEK_SET);        
        }
    
    
    for (i=0;i<256;i++)
    {
        if (dopalette)
          lcmap[i]=getcolour(lcmaprgb[i].r,lcmaprgb[i].g,lcmaprgb[i].b);
        else
          lcmap[i].c8=i;
    }
             
    pos=startpos;

   
        
    tempscan=new UINT8[scanwidth<<1];
    runlength=0;
        
    for (pos.y=0;pos.y<wsize.y;pos.y++)
    {
        memset(tempscan,0,scanwidth<<1);
        subtotal=0;
        cbyte=tempscan;                            
        
        while (subtotal<scanwidth) 
        {                    
            if ((chk=fgetc(f))==EOF)return NULL;                    
            if(chkbits==(chk & chkbits))
            {                        
                runlength=chk&maskbits;                        
                if ((chk=fgetc(f))==EOF)return NULL;                                                
            }else                                                                  
                runlength=1;
            //if ((subtotal+runlength)>scanwidth)  return NULL; //error in file, scanlength too long
                                        
            memset(cbyte,chk,runlength);
            cbyte+=runlength;
            subtotal+=runlength;                    
        }                                                                               
        
        UINT32 o=header.offset;
        UINT8 bppi;
        UINT8 colindex;
        UINT32 scanpos;
        //decode scanline from nbpp to 8bpp
            
        for (scanpos=0;scanpos<header.offset;scanpos++)
        {
            if (header.bpp==8)
            {
                if (pos.x<wsize.x)  //clip padding
                    if (header.numplanes==3)                    
                        t_col=getcolour(
                        tempscan[scanpos],
                        tempscan[scanpos+header.offset],
                        tempscan[scanpos+header.offset+header.offset]);
                        
                    else
                    {
                        
                        t_col=lcmap[tempscan[scanpos]];                        
                    }
                    
                    putpixel(pos.x,pos.y);
                pos.x++;
            }else
            if (header.bpp==1)
            {                    
                UINT8 cmask=128;
                for (bppi=0;bppi<8;bppi++)
                {
                    if (pos.x<wsize.x)  //clip padding
                    {
                        cmask=128 >> bppi;
                        colindex=0;
                        for (planei=0;planei<header.numplanes;planei++)                        
                            if (tempscan[scanpos+(header.offset*planei)]&cmask)
                               colindex|=1 << planei;
                        t_col=lcmap[colindex];
                        
                        putpixel(pos.x,pos.y);                        
                    }
                    pos.x++;                            
                }                    
            }
            
        }
        pos.x=0;                                 
    }
    delete tempscan;
    fclose(f);
    return TRUE;

}

tbitmap* readpcx(char *filename,UINT8 type,UINT8 drawmode,UINT8 attrib)
{
    screenDataType bmsc,oldsc;    
    tsize wsize;
    tbitmap *bm;
    UINT8 bpp;
    getpcxmetrics(filename,wsize.x,wsize.y,bpp);    
    bm=new tbitmap(wsize.x,wsize.y,type,drawmode,attrib);
    oldsc=SC;    
    createcontext(&bmsc,bm);    
    setoutput(&bmsc);    
    if (!loadpcxdata(filename,(attrib & BMFdontloadpalette)==0)) return NULL;
    destroycontext(&bmsc);    
    setoutput(&oldsc);
    
    return bm;
}    
    


