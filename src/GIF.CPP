#include <string.h>
#include <stdio.h>
#include <conio.h>
#include "imgloadr.hpp"
#include "gbasics.hpp"
#include "ttypes.hpp"

//infomasks}


const char gifsig[]={'G','I','F','8','9','a'};


//local masks
#define LOCALMAP 128
#define INTERLACED 64 

//header masks
#define COLOURMAP 128 
#define BPC 112
#define BPC_SHIFT 4
#define BPP 7 

char truestr[]="TRUE";
char falsestr[]="FALSE";


typedef struct gifattrib
{
    char colourmap:1;
    char bpc:3;
    char reserved:1;
    char bpp:3;       
}gifattrib;


typedef struct gifiamgeattrib
{
    char colourmap:1;
    char interlaced:1;
    char reserved:3;
    char bpp:3;       
}gifimageattrib;    

typedef struct gifscreenheader
{
  tsize size;
  gifattrib infobyte;
  char background;
  char tmp;
}gifscreeheader;

typedef struct gifimageheader
{
  trect size;
  gifimageattrib infobyte;
}gifimageheader;

inline char *boolstr(char bval)
{
    return bval?truestr:falsestr;
}

void writegifheader(tpoint p,gifscreenheader &h)
{
    char buf[255];    
    sprintf(buf,"Size of attribute 'byte'%i ",sizeof(gifscreenheader));
    outtextxy(p.x,p.y+=16,buf);
    sprintf(buf,"Screen dimensions: X:%i Y:%i",h.size.x,h.size.y);    
    outtextxy(p.x,p.y+=16,buf);
    sprintf(buf,"Colourmap: %s",boolstr(h.infobyte.colourmap));
    outtextxy(p.x,p.y+=16,buf);
    sprintf(buf,"Bits per colour: %i",1+h.infobyte.bpc);
    outtextxy(p.x,p.y+=16,buf);
    sprintf(buf,"Bits per pixel: %i",1+h.infobyte.bpp);
    outtextxy(p.x,p.y+=16,buf);
    /*sprintf(buf,"Interlaced: %s",boolstr(h.infobyte & BPC));
    outtextxy(p.x,p.y+=16,buf);*/
}

void writegifimageheader(tpoint p,gifimageheader &h)
{
    char buf[255];    
    sprintf(buf,"image dimensions: X1:%i Y1:%i X2:%i Y2:%i",h.size.a.x,h.size.a.y,h.size.b.x,h.size.b.y);    
    outtextxy(p.x,p.y+=16,buf);
    sprintf(buf,"Colourmap: %s",boolstr(h.infobyte.colourmap));
    outtextxy(p.x,p.y+=16,buf);
    sprintf(buf,"Bits per colour: %i",1+h.infobyte.interlaced);
    outtextxy(p.x,p.y+=16,buf);
    sprintf(buf,"Bits per pixel: %i",1+h.infobyte.bpp);
    outtextxy(p.x,p.y+=16,buf);
    /*sprintf(buf,"Interlaced: %s",boolstr(h.infobyte & BPC));
    outtextxy(p.x,p.y+=16,buf);*/
}

/*Procedure writeimagehead(header:image_disc);
begin
  with header do begin
    writeln('Screen dimensions. X1:',size.x1,' Y1:',size.y1);
    writeln('                   X2:',size.x2,' Y2:',size.y2);
    writeln('Colour map there?',(infobyte and localmap)shr localmap_shift);
    writeln('Colour map there?',(infobyte and interlaced)shr interlaced_shift);
    writeln('Pixel resolution:',1+infobyte and BitsPerPixel);
  end;
  readkey;
end;*/
/*
Procedure swaphilo(var N:integer);assembler;
asm
  les di,n
  mov ax,es:[di]
  xchg al,ah
  mov es:[di],ax
end;*/

trgb pal[256];


tbitmap *readgif(char *filename,UINT8 pattrib)
{
    char clearcode;
    char bpi; //bits per item
    UINT16 i;
    char sepchar;
    char sig[6];
    tbitmap *bm;
    gifscreenheader h;
    gifimageheader ih;
    UINT16 maxcols;
    FILE *f;
    
    
    if (!(f=fopen(filename,"rb"))) return NULL;

    fread(&sig,sizeof (sig),1,f);
    if (memcmp(sig,gifsig,3))
    {
        outtextxy(0,0,sig,3);
        return NULL;
    }        
    fread(&h,7,1,f);
    writegifheader(tpoint(0,0),h);
  
   
    maxcols=1 <<  h.infobyte.bpp+1;

    if (h.infobyte.colourmap!=0)
    {
        fread(&pal,maxcols*sizeof(trgb),1,f);
        for (i=0;i<maxcols;i++)
        {
            t_col=getcolour(pal[i].r,pal[i].g,pal[i].b);
            hline(10,40,90+i);
        }
    }
    sepchar=getc(f);
    while (sepchar==',')
    {               
        fread(&ih,9,1,f);
        writegifimageheader(tpoint(0,100),ih);
        bpi=getc(f);
        clearcode=1<<bpi;
        
        
        sepchar=getc(f);
        
    }                
    

    fclose(f);       
    
    return bm;
    
}    
    
    
    
  


