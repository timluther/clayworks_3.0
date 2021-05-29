
#ifndef _gbasics_h_
#define _gbasics_h_

#include <math.h>
#include "ttypes.hpp"
#include "tmaths.hpp"

////Logical modes
#define normalput 0x00
#define xorput 0x01
#define andput 0x02
#define orput 0x04
#define addput 0x8
#define subput 0x10
#define blendput 0x20
#define chromaput 0x40

//bitmap creation flags
#define BMFwordalign 0x1
#define BMFdoublewordalign 0x2
#define BMFdontloadpalette 0x4
#define BMFowndata 0x8
#define BMFdontremap 0x20  //when loading a picture, the system will try and use 'getcolour' to form a 
                           //native colour palette which in indexed modes causes remaping to take place
                           //if you want to use the orginal palette and just index into it as the file does
                           //then use this flag; Note, it only applies to indexed palettes.
                           //it has no effect in direct colour models
//these are for the high order word in bitmap pasting

//colour weights
#define rwt 12 
#define gwt 26 
#define bwt 14

////graphic driver identifiers//////////////////////////////////////////////
#define STnull 0
#define STsvga_plainiar 1
#define STbitmap_plainiar 2
#define STsvga_flat 3
#define STxvga_plainiar 4
#define STsvga_flat32k 5
#define STsvga_flat64k 6
#define STsvga_flat24b 7
#define STsvga_flat32b 8

#define BMFbpp      31  //mask for bpp
#define BMFformat   63  //mask for the format bits

#define BMnull     0                   //Use this if you wish to create a device compatible bitmap
#define BMplanes   STbitmap_plainiar
#define BMflat8    STsvga_flat
#define BMflat16   STsvga_flat64k
#define BMflat24   STsvga_flat24b
#define BMflat32   STsvga_flat32b

// palette orgainisation types
#define PMindexed 1
#define PMdirect  2

#define right   0x01
#define bellow  0x02
#define left    0x04
#define above    0x08
#define above_right  above+right
#define bellow_right bellow+right
#define above_left   above+left
#define bellow_left  bellow+left
#define middle       above+bellow+right+left

//Rectangle list return codes
#define RSlist 0
#define RSobscured 1
#define RSnooverlap 2


//INT32 *ica=(INT32*)&ic;
class trivert
{
    public:
    float x,y,r,g,b,a,u,v,z;
    void assign(float x_,float y_){x=x_;y=y_;};
    void colour(float r_,float g_,float b_,float a_){r=r_*65536.0f;g=g_*65536.0f;b=b_;a=a_*65536.0f;};
    void uv(float u_,float v_){u=u_*65536.0f;v=v_*65536.0f;};
};


//POLYGON SCAN STRUCTURES////////////////////////////////////////////////////////

typedef struct thlines
{
    INT32 x,u,v;
    UINT16 r,g,b,a;
}thlines;    

typedef struct thline
{
    INT32 x1,u1,v1;UINT16 r1,g1,b1,a1;
    INT32 x2,u2,v2;UINT16 r2,g2,b2,a2;
}thline;    

////COLOUR STRUCTURES//////////////////////////////////////////////////////////
typedef struct trgbfloatbase
{
    float r,g,b;
}trgbfloatbase;


typedef struct trgbafloatbase
{
    float r,g,b,a;
}trgbafloatbase;

typedef struct trgbbase
{
    UINT8 r,g,b;
}trgbbase;

typedef struct trgbabase
{
    UINT8 r,g,b,a;
}trgbabase;


typedef struct trgbwordbase
{
    UINT16 r,g,b;
}trgbwordbase;


typedef struct trgbawordbase
{
    UINT16 r,g,b,a;
}trgbawordbase;

class trgb:public trgbbase
{
    public:
    trgb(){r=g=b=0;}
    trgb(UINT8 r_,UINT8 g_,UINT8 b_){r=r_;g=g_;b=b_;}
    bool operator ==(trgb &other){return (r==other.r && g==other.g && b==other.b);}
    bool operator !=(trgb &other){return (r!=other.r || g!=other.g || b!=other.b);}
    trgb &operator =(UINT8 val){r=b=g=val;return *this;}
    trgb &operator <<=(UINT8 val){r<<=val;g<<=val;b<<=val;return *this;}
    trgb &operator >>=(UINT8 val){r>>=val;g>>=val;b>>=val;return *this;}
    UINT32 getdword(){return r|g<<8|b<<16;}
    
    /*operator trgb <<(UINT8 val){trgb r;r.r=r<<val;r.g=g<<val;r.g=g<<val;return r;}    
    operator trgb >>(UINT8 val){trgb r;r.r=r>>val;r.g=g>>val;r.g=g>>val;return r;}*/
};


class trgbword:public trgbwordbase
{
    public:

    trgbword(){r=g=b=0;}
    trgbword(UINT8 r_,UINT8 g_,UINT8 b_){r=r_;g=g_;b=b_;}
    bool operator ==(trgbword &other){return (r==other.r && g==other.g && b==other.b);}
    bool operator !=(trgbword &other){return (r!=other.r || g!=other.g || b!=other.b);}
    bool operator ==(trgb &other){return (r==other.r && g==other.g && b==other.b);}
    bool operator !=(trgb &other){return (r!=other.r || g!=other.g || b!=other.b);}
    trgbword &operator =(UINT8 val){r=b=g=val;return *this;}
    trgbword &operator =(trgb &other){r=other.r;b=other.b;g=other.g;return *this;}
    trgbword &operator <<=(UINT8 val){r<<=val;g<<=val;b<<=val;return *this;}
    trgbword &operator >>=(UINT8 val){r>>=val;g>>=val;b>>=val;return *this;}

};

class trgba:public trgbabase
{
    public:
    trgba(){r=g=b=a=0;}
    trgba(UINT8 r_,UINT8 g_,UINT8 b_,UINT8 a_){r=r_;g=g_;b=b_;a=a_;}
    bool operator ==(trgba &other){return (r==other.r && g==other.g && b==other.b && a==other.a);}
    bool operator !=(trgba &other){return (r!=other.r || g!=other.g || b!=other.b || a!=other.a);}
    trgba &operator =(UINT8 val){r=b=g=a=val;return *this;}
    trgba &operator <<=(UINT8 val){r<<=val;g<<=val;b<<=val;a<<=val;return *this;}
    trgba &operator >>=(UINT8 val){r>>=val;g>>=val;b>>=val;a<<=val;return *this;}
    trgba &operator =(trgbbase &other){r=other.r;g=other.g;b=other.b;a=0;return *this;}
    UINT32 getdword(){return r|g<<8|b<<16|a<<24;}
    
    /*operator trgb <<(UINT8 val){trgb r;r.r=r<<val;r.g=g<<val;r.g=g<<val;return r;}    
    operator trgb >>(UINT8 val){trgb r;r.r=r>>val;r.g=g>>val;r.g=g>>val;return r;}*/
};      

class trgbaword:public trgbawordbase
{
    public:
    trgbaword(){r=g=b=a=0;}
    trgbaword(UINT16 r_,UINT16 g_,UINT16 b_,UINT16 a_){r=r_;g=g_;b=b_;a=a_;}
    bool operator ==(trgbaword &other){return (r==other.r && g==other.g && b==other.b && a==other.a);}
    bool operator !=(trgbaword &other){return (r!=other.r || g!=other.g || b!=other.b || a!=other.a);}
    bool operator ==(trgba &other){return (r==other.r && g==other.g && b==other.b && a==other.a);}
    bool operator !=(trgba &other){return (r!=other.r || g!=other.g || b!=other.b || a!=other.a);}
    trgbaword &operator =(UINT8 val){r=b=g=a=val;return *this;}
    trgbaword &operator <<=(UINT8 val){r<<=val;g<<=val;b<<=val;a<<=val;return *this;}
    trgbaword &operator >>=(UINT8 val){r>>=val;g>>=val;b>>=val;a<<=val;return *this;}
    /*operator trgb <<(UINT8 val){trgb r;r.r=r<<val;r.g=g<<val;r.g=g<<val;return r;}    
    operator trgb >>(UINT8 val){trgb r;r.r=r>>val;r.g=g>>val;r.g=g>>val;return r;}*/
};

class trgbfloat:public trgbfloatbase
{
    public:
    trgbfloat(){r=g=b=0.0f;}
    trgbfloat(float r_,float g_,float b_){r=r_;g=g_;b=b_;}
    trgbfloat &operator =(trgb &other){r=other.r /255.0f;g=other.g /255.0f;b=other.b/255.0f;return *this;}
    trgbfloat &operator =(trgba &other){r=other.r /255.0f;g=other.g /255.0f;b=other.b/255.0f;return *this;}
    bool operator ==(trgbfloat &other){return (r==other.r && g==other.g && b==other.b);}
    bool operator !=(trgbfloat &other){return (r!=other.r || g!=other.g || b!=other.b);}
    trgbfloat &operator =(float val){r=b=g=val;return *this;}
    trgbfloat &operator =(trgbbase &other){r=other.r/256.0f;g=other.g/256.0f;b=other.b/256.0f;return *this;}
};      



//This is the union which t_col and t_fillcol typed as.
//possibility of extra fields for alpha values.

typedef struct tcolour
{
    union
    {
        struct { UINT8 c8; };                                                      
        struct { UINT16 c16;};
        trgbabase c32;
        UINT32 c32val;
    };
}tcolour;

class trectlist; 

class tpointbase
{
    public:
    INT16 x,y;
};
     
template <class T> class tpointtemplate
{
    public:
    T x,y;
    tpointtemplate(){x=y=0;}
    tpointtemplate(T v){x=y=v;}
    tpointtemplate(T x_,T y_){x=x_;y=y_;}

    tpointtemplate &operator =(tpointtemplate &other){x=other.x;y=other.y;return *this;}    
    tpointtemplate &operator =(T val){x=y=val;return *this;}    
    void assign(T x1,T y1){x=x1;y=y1;};
    BOOL closeto(tpointtemplate *other,T t){return ((x>=other->x-t)&&(y>=other->y-t)&&(x<=other->x+t)&&(y<=other->y+t));}
    tpointtemplate &operator +(tpointtemplate &other){static tpointtemplate r;r.x=x+other.x;r.y=y+other.y;return r;}
    tpointtemplate &operator +(T v){static tpointtemplate r;r.x=x+v;r.y=y+v;return r;}
    tpointtemplate &operator -(tpointtemplate &other){static tpointtemplate r;r.x=x-other.x;r.y=y-other.y;return r;}
    tpointtemplate &operator -(T v){static tpointtemplate r;r.x=x-v;r.y=y-v;return r;}
    tpointtemplate &operator *(tpointtemplate &other){static tpointtemplate r;r.x=x*other.x;r.y=y*other.y;return r;}
    tpointtemplate &operator *(T v){static tpointtemplate r;r.x=x*v;r.y=y*v;return r;}
    tpointtemplate &operator /(tpointtemplate &other){static tpointtemplate r;r.x=x/other.x;r.y=y/other.y;return r;}
    tpointtemplate &operator /(T v){static tpointtemplate r;r.x=x/v;r.y=y/v;return r;}
    tpointtemplate &operator +=(tpointtemplate &other){x+=other.x;y+=other.y;return *this;}
    tpointtemplate &operator +=(T v){x+=v;y+=v;return *this;}
    tpointtemplate &operator -=(tpointtemplate &other){x-=other.x;y-=other.y;return *this;}
    tpointtemplate &operator -=(T v){x-=v;y-=v;return *this;}
    tpointtemplate &operator *=(tpointtemplate &other){x*=other.x;y*=other.y;return *this;}
    tpointtemplate &operator *=(T v){x*=v;y*=v;return *this;}
    tpointtemplate &operator /=(tpointtemplate &other){x/=other.x;y/=other.y;return *this;}
    tpointtemplate &operator /=(T v){x/=v;y/=v;return *this;}    
    
    BOOL operator==(tpointtemplate &other){return (x==other.x)&&(y==other.y);};
    BOOL operator==(T v){return (x==v)&&(y==v);};
    BOOL operator!=(tpointtemplate &other){return (x!=other.x)||(y!=other.y);};
    BOOL operator!=(T v){return (x!=v)||(y!=v);};
    BOOL aligned(tpointtemplate &other){return((x==other.x)||(y==other.y));};
    T length(){return (T)sqrt((double)(x*x) + (y*y));}
    //turn into unit length vector
    void normalize()
    {
        T uli=(T)length();
        if (uli!=0)
        {            
            x/=uli;
            y/=uli;
        }            
    }
};

template <class T> class trecttemplate
{
    public:
    tpointtemplate<T> a,b;
    trecttemplate(){a.x=b.y=0;}
    trecttemplate(tpointtemplate<T> a_,tpointtemplate<T> b_){a=a_;b=b_;}
    trecttemplate(T x,T y){a.x=a.y=0;b.x=x-1;b.y=y-1;}
    trecttemplate(T  x1,T y1,T x2,T y2){a.x=x1;a.y=y1;b.x=x2;b.y=y2;}    
    
    void rpos(T x1, T y1){T w=width();T h=height();a.x=x1;a.y=y1;b.x=x1+w;b.y=y1+h;};    
    void rmove(T x1, T y1){a.x += x1;a.y += y1;b.x += x1;b.y += y1;};
    void rgrow(T x1, T y1){a.x -= x1;a.y -= y1;b.x += x1;b.y += y1;}       
    BOOL rcontains(T x1, T y1){return (x1>=a.x) && (y1>=a.y) && (x1<=b.x) && (y1<=b.y);};
    BOOL rcontains(tpointtemplate<T>  &p){return (p.x>=a.x) && (p.y>=a.y) && (p.x<=b.x) && (p.y<=b.y);};
    BOOL rinside (trecttemplate &r){  return (a.x>=r.a.x) && (b.x<=r.b.x) && (a.y>=r.a.y) && (b.y<=r.b.y);};    
    BOOL roverlap(trecttemplate &r){return ( (r.b.x>=a.x) && (r.a.x<=b.x) && (r.b.y>=a.y) && (r.a.y<=b.y) );};
    void zero(){a.x=a.y=b.x=b.y=0;}
    UINT8 rempty(){return (a==b);}
    T height(){return b.y-a.y;}
    T width(){return b.x-a.x;}
    T halfx(){return width() / 2;}
    T halfy(){return height() /2;}
    
    trecttemplate  &operator =(trecttemplate &other){a.x=other.a.x;a.y=other.a.y;b.x=other.b.x;b.y=other.b.y;return *this;}    
    trecttemplate &operator +(tpointtemplate<T> &other){static trecttemplate r;r.a=a+other;r.b=b+other;return r;}
    trecttemplate &operator -(tpointtemplate<T> &other){static trecttemplate r;r.a=a-other;r.b=b-other;return r;}
    trecttemplate &operator +=(tpointtemplate<T>&other){a+=other;b+=other;return *this;}
    trecttemplate &operator -=(tpointtemplate<T>&other){a-=other;b-=other;return *this;}

    trecttemplate &operator =(tpointtemplate<T> &other){a=0;b.x=other.x-1;b.y=other.y-1;return *this;}
    BOOL operator==(trecttemplate &other){return (a==other.a) && (b==other.b);}
    BOOL operator!=(trecttemplate &other){return (a!=other.a) | (b!=other.b);}

    void rassign(T x1,T y1, T x2, T y2)
    {
        if (x2>=x1) {a.x=x1;b.x=x2;} else {a.x=x2;b.x=x1;}
        if (y2>=y1) {a.y=y1;b.y=y2;} else {a.y=y2;b.y=y1;}
    }

    void rassign(T x2, T y2)
    {
        a.x=a.y=0;
        b.x=x2-1;
        b.y=y2-1;
    }
    
    void sortelements()
    {
       T temp;
       if ((temp=a.x)>b.x) {a.x=b.x;b.x=temp;}
       if ((temp=a.y)>b.y) {a.y=b.y;b.y=temp;}
    }

    void runion(trecttemplate &r)
    {   
        if (r.b.x>b.x) b.x=r.b.x;
        if (r.b.y>b.y) b.y=r.b.y;
        if (r.a.x<a.x) a.x=r.a.x;
        if (r.a.y<a.y) a.y=r.a.y;
    }                      

    BOOL rintersect(trecttemplate &r)
    {
        if ((r.b.x>=a.x) && (r.a.x<=b.x) && (r.b.y>=a.y) && (r.a.y<=b.y)) 
        {
           if ((r.a.x>=a.x) && (r.a.x<=b.x)) a.x=r.a.x;
           if ((r.a.y>=a.y) && (r.a.y<=b.y)) a.y=r.a.y;
           if ((r.b.x>=a.x) && (r.b.x<=b.x)) b.x=r.b.x;
           if ((r.b.y>=a.y) && (r.b.y<=b.y)) b.y=r.b.y;
           return TRUE;
       }else
       {
          zero(); 
          return FALSE;
       }    
   }

   BOOL rintersect(T x1,T y1,T x2,T y2)
   {
      if ((x2>=a.x) && (x1<=b.x) && (y2>=a.y) && (y1<=b.y)) 
      {
          if ((x1>=a.x) && (x1<=b.x)) a.x=x1;
          if ((y1>=a.y) && (y1<=b.y)) a.y=y1;
          if ((x2>=a.x) && (x2<=b.x)) b.x=x2;
          if ((y2>=a.y) && (y2<=b.y)) b.y=y2;
          return TRUE;
      }else
      {
          zero(); 
          return FALSE;
      }
   }

   BOOL rintersect(T x,T y)
   {
      if ((x>a.x) && (0<=b.x) && (y>a.y) && (0<=b.y)) 
      {
          if ((0>=a.x) && (0<=b.x)) a.x=0;
          if ((0>=a.y) && (0<=b.y)) a.y=0;
          if ((a.x<x) && (x<b.x)) b.x=x-1;
          if ((a.y<y) && (y<b.y)) b.y=y-1;
          return TRUE;
      }else
      {
          zero(); 
          return FALSE;
      }
   }

   
   BOOL rconcatable(trecttemplate &r)
   {
       return (((r.b.x>=a.x-1) && (r.a.x<=b.x+1) && (r.a.y==a.y) && (r.b.y==b.y))||((r.a.x==a.x)&&(r.b.x==b.x)&&(r.b.y>=a.y-1)&&(r.a.y<=b.y+1)));
   }

   inline char outcode(T &x1,T &y1)
   {
      return  ((x1>b.x))+     //right  =$01}
               ((y1>b.y)<< 1)+ //bellow =$02}
               ((x1<a.x)<< 2)+ //left   =$04}
               ((y1<a.y)<< 3); //above  =$08}
   }

   BOOL clipline(T &xa,T &ya,T &xb,T &yb);
   trectlist *cutfrom(trecttemplate r);
};

typedef tpointtemplate<INT16> tpoint16;
typedef tpointtemplate<INT32> tpoint;
typedef tpointtemplate<INT32> tpoint32;
typedef tpointtemplate<float> tpointfloat;
typedef tpointtemplate<double> tpointdouble;
typedef tpointtemplate<float> tpointflt;
typedef trecttemplate<float> trectfloat;
typedef trecttemplate<double> trectdouble;
typedef trecttemplate<float> trectflt;
typedef trecttemplate<INT16> trect16;
typedef trecttemplate<INT32> trect;

class tsize
{      
    public:
    UINT16 x,y;
    //tsize(){x=y=0;}
    //tsize(UINT16 x_,UINT16 y_){x=x_;y=y_;}    
    void assign(UINT16 x1,UINT16 y1){x=x1;y=y1;}
    tsize &operator =(UINT16 val){x=y=val;return *this;}
    tsize &operator +(tsize &other){static tsize r;r.x=x+other.x;r.y=y+other.y;return r;}
    tsize &operator -(tsize &other){static tsize r;r.x=x-other.x;r.y=y-other.y;return r;}
    tsize &operator *(tsize &other){static tsize r;r.x=x*other.x;r.y=y*other.y;return r;}
    tsize &operator /(tsize &other){static tsize r;r.x=x/other.x;r.y=y/other.y;return r;}
    tsize &operator +=(tsize &other){x+=other.x;y+=other.y;return *this;}
    tsize &operator -=(tsize &other){x-=other.x;y-=other.y;return *this;}
    tsize &operator *=(tsize &other){x*=other.x;y*=other.y;return *this;}
    tsize &operator /=(tsize &other){x/=other.x;y/=other.y;return *this;}
    
    BOOL operator==(tsize &other){return (x==other.x)&&(y==other.y);}
    BOOL operator!=(tsize &other){return (x!=other.x)||(y!=other.y);}
    BOOL operator==(tpoint &other){return (x==other.x)&&(y==other.y);}
    BOOL operator!=(tpoint &other){return (x!=other.x)||(y!=other.y);}
    
};

class trectlist:public trect
{
  public:
  void operator=(trect &other){a=other.a;b=other.b;}
  trectlist *next;
};

typedef struct gpoint
{
    INT16 x,y;
    UINT8 r,g,b,a;
    UINT16 u,v;
    UINT32 z;
}gpoint;


#define CTproportional 1 //use this for monospaced fonts
#define CTreversed 2     //fontdata is in reversed format
#define CTpixelpacked 3   //data is in byte per pixel format

////FONT STRUCTURES//////////////////////////////////////////////////////////

typedef struct tfontchar
{
  UINT8 height,width,bytewidth;
  char data[32];
}charStruct;

typedef struct tfont
{
  //font data is structured like this:
  //char Height,char width,char bytewidth and then the bit data.
  //Use the macros to access individual bits
  tfontchar offsets[256];  //pointer offsets to chardata in 'fontdata'
  UINT8 xbytesize;
  UINT8 ysize;              //maximum y size
  UINT8 yextsize;           //extender y size
  
  UINT8 xsize;              
 
  UINT8 cattrib;       //attribute byte
  union {
    UINT8 numplanes;   //how many bitplanes for the data
    UINT8 numbits;     //or how many bits per pixel
  };
  

}tfont;

////SCREEN DISPLAY INFORMATION//////////////////////////////////////////////////////////
//Used to identify a screen mode, pass to initgraph to setup the screenDataType struct.

typedef struct screenIDType
{    
    UINT32 driver;   //screen mode describes the general graphics driver}    
    tsize size;      //resolution of the screen}    
    UINT8 rsize,gsize,bsize;  //size of r, g and b components if applicable
    UINT8 getbpp();    //get bits per pixel for this mode (1,2,4,8,15,16,24 or 32)
}screenIDType;




//size=44 bytes
class surfaceData
{
    public:    
    UINT32 driver;     //screen mode; from this you can determine many mode details
    UINT32 *startoffsets;    //offsets to begining of screendata. possibly slower than mul on pentiums
    tsize size;        //resolution of the screen
    char *scrptr;     //pointer to screendata
    UINT32 offsd;      //offset of screen in bytes 
    UINT32 pagesize;   //size of visible page
    UINT32 memavail;   //maximum size of surface memory available
    
    UINT32 colourdepth; //The amount of colours available in this mode/image
    tcolour *palette;   //copy of the hardware palette, not defined if the palettemode != PTindexed
    trgba *rgbpalette;  //8 8 8 8 format palette, not defined if palettemode !=PTindexed        
    
    UINT8 getpalmode();//returns either PTindexed or PTdirect
    UINT8 getbpp();    //get bits per pixel for this mode (1,2,4,8,15,16,24 or 32)
    UINT8 getbytesize();
};


//please note! this must be changed if you change the order of the fields bellow!!
#define OFFstartoffsets 4
#define OFFax 44
#define OFFbx 46
#define OFFay 48
#define OFFby 50

//drawable surface
//size=63
//:public surfaceData
class screenDataType:public surfaceData
{
    public:
    trect viewport;     //viewport,all clipping is performed through this          
    UINT8 rsize,gsize,bsize,rpos,gpos,bpos,maskr,maskg,maskb;   
};

//bitmap surface, similar to drawable surface. you need to create a drawable surface from this in order to draw to a bitmap
class tbitmap:public surfaceData
{
    public:       
           
    UINT32 drawmode;            //mode for drawing
    UINT32 attrib;              //bitmap attributes
    //t_colour chroma;
    tbitmap(tbitmap *bm);   //possibly add a filter in here
    tbitmap(UINT32 x,UINT32 y,
            UINT32 xlen,UINT32 ylen,
            tbitmap *src,
            UINT8 drawmode_=normalput,UINT8 newdata=0);
    tbitmap(UINT32 xlen,UINT32 ylen,
            UINT8 type_,
            UINT8 drawmode_=normalput,
            UINT8 attrib_=BMFdoublewordalign,            
            char *data=NULL,
            tcolour *palette_=NULL);
    ~tbitmap();
    //UINT32 pagesize(){return offsd*size.y;}    
    UINT32 getoffsetat(PIXINT x,PIXINT y);  //gives an offset into the image (in bytes) at position (x,y)
    void resize(UINT32 xlen,UINT32 ylen);
    void xflip();
    void yflip();
    void rotate90();
    
    private:
    void getoffs();  //calculates offsd according to bitmap type
};

//////////////////////////////////////////////////////////////////////////////////////////
//global variables
 
typedef UINT16 textline[80];
typedef char fillpatterntype[8];
//these are used in 16 colour or plainiar modes. They are included in here
//instead of inside those specific Libraries as the 16 colour bitmap routines
//are always included and will need these constants}

extern UINT8 singlepixelbitmasks[8];
extern UINT8 left_pixelbitmasks[8];
extern UINT8 right_pixelbitmasks[8];

extern char GFX_INITIALIZED;



//rectlist global data
extern theapmem rectbuffer;
extern UINT8 rectlist_status;
extern UINT32 rect_count;
extern trectlist *lastrect;

//screen disply types
extern screenIDType VGA320x200x16;
extern screenIDType VGA640x200x16;
extern screenIDType VGA640x350x16;
extern screenIDType VGA640x480x16;
extern screenIDType SVGA800x600x16;
extern screenIDType SVGA1024x768x16;
extern screenIDType SVGA1280x1024x16;
extern screenIDType SVGA320x200x256;////256 colour flat 64k
extern screenIDType SVGA640x400x256;
extern screenIDType SVGA640x480x256;
extern screenIDType SVGA800x600x256;
extern screenIDType SVGA1024x768x256;
extern screenIDType SVGA1280x1024x256;
extern screenIDType SVGA320x200x32k;
extern screenIDType SVGA320x240x32k;
extern screenIDType SVGA640x480x32k;
extern screenIDType SSVGA800x600x32k;
extern screenIDType SVGA1024x768x32k;
extern screenIDType SVGA1280x1024x32k;
extern screenIDType SVGA320x200x64k;
extern screenIDType SVGA320x240x64k;
extern screenIDType SVGA640x480x64k;
extern screenIDType SVGA800x600x64k;
extern screenIDType SVGA1024x768x64k;
extern screenIDType SVGA1280x1024x64k;
extern screenIDType SVGA320x200x16m;
extern screenIDType SVGA320x240x16m;
extern screenIDType SVGA640x480x16m;
extern screenIDType SSVGA800x600x16m;
extern screenIDType SVGA1024x768x16m;
extern screenIDType SVGA1280x1024x16m;

//The two main diplay contexts, output and input.
extern screenDataType SC,IC,SC_BASE;
//Current Colours & writemode 
extern UINT32 t_maxcolours;
extern tcolour t_fillcol,t_col,t_chroma,*t_nativepalette,*t_nativebitmappalette;
extern trgba t_colrgba;
extern trgba *t_palette,*t_bitmappalette;
extern UINT8 t_alpha;
extern tbitmap *t_texture;

extern char *t_texturedataptr;
extern UINT32 *t_texturestartoffsets;
extern tsize t_texturesize;
extern float t_texturewidthscale;
extern float t_textureheightscale;



extern UINT8 t_writemode;
extern UINT8 t_linepattern;
//for fill patterns
extern tfont t_charset;
//extern UINT16 maskword;
//char maskbyte
extern fillpatterntype fillpattern;
extern char maskbackground,nullpattern;

////////////////////////////////////////////////////////////////////
//Global functions
extern void (*drawscanline)(char *adrs,UINT32 count);
extern tcolour (*getcolour)(UINT8 r,UINT8 g,UINT8 b);
extern void (*closemode)(void);
extern char (*setmode)(screenIDType &sid);
extern void (*putpixel)(PIXINT x,PIXINT y);
extern tcolour (*getpixel)(PIXINT x,PIXINT y);
extern trgba (*getrgbpixel)(PIXINT x,PIXINT y);
extern void (*hline)(PIXINT x1,PIXINT x2,PIXINT y);
extern void (*vline)(PIXINT x,PIXINT y1,PIXINT y2);
extern void (*line)(PIXINT x1,PIXINT y1,PIXINT x2,PIXINT y2);
extern void (*bar)(PIXINT x1,PIXINT y1,PIXINT x2,PIXINT y2);
extern void (*tri)(trivert *v0,trivert *v1,trivert *v2);     
extern void (*putbitmap_base)(PIXINT x,PIXINT y,tbitmap *pic,char useglobalattribs);
extern void (*getbitmap)(PIXINT x,PIXINT y,tbitmap *pic);
extern void (*circle)(PIXINT cx,PIXINT cy,PIXINT radius);
extern void (*outtextxy_base)(PIXINT cx,PIXINT cy,char *txt,PIXINT length);
extern void (*drawbytes)(PIXINT x,PIXINT y,UINT8 w,UINT8 h,char *bytes);
extern void (*cleardevice)(void);
//the rectbuffer heap
void destroyplist(trectlist *p);       
trectlist* getrectmem();
trectlist* ungetrectmem();
void waitvbl();
void closegraph();
char initgraph(screenIDType &sID);
//input and output contexts. getbitmap and getpixel are input context related functions.
screenDataType *createcontext(screenDataType *sc,tbitmap *bm);
void destroycontext(screenDataType *sc);
void setinput(screenDataType *sc=NULL);
void setoutput(screenDataType *sc=NULL);
void setup_outputroutines(UINT32 driver);   
void setup_inputroutines(UINT32 driver);
void setup_displayroutines(UINT32 driver);

//these are global inline functions to give a polymorphic apperance to stub functions
inline void putbitmap(PIXINT x,PIXINT y,tbitmap *pic,char useglobalattribs)
{
    putbitmap_base(x,y,pic,useglobalattribs);
}

inline void putbitmap(PIXINT x,PIXINT y,tbitmap *pic)
{
    putbitmap_base(x,y,pic,0);
}

inline void outtextxy(PIXINT cx,PIXINT cy,char *txt)
{
    outtextxy_base(cx,cy,txt,-1);
}

inline void outtextxy(PIXINT cx,PIXINT cy,char *txt,INT16 len)
{
    outtextxy_base(cx,cy,txt,len);
}    

/*inline void setcolour(char col)
{
    t_col=col;
}

inline void setbkcolour(char col)
{
    t_fillcol=col;
} */       

inline void setwritemode(char mode)
{
    t_writemode=mode;
}

inline UINT8 GetRecommenedBM()
{
    return SC.driver;
}

void settexture(tbitmap *tex);

////FONT ROUTINE DEFINTIONS//////////////////////////////////////////////////////////

inline UINT8 FNTgetcharwidth(charStruct *ch)
{
    return (t_charset.cattrib & CTproportional!=0)?ch->width:t_charset.xsize;
}

inline UINT8 FNTgetcharwidth(char ch)
{
    return (t_charset.cattrib & CTproportional!=0)?t_charset.offsets[ch].width:t_charset.xsize;
}    

void FNTnotfont(tfontchar *ch);
void FNTrorfont(tfontchar *ch);
void FNTrodfont(tfontchar *ch);
void FNTrolfont(tfontchar *ch);
void FNTroufont(tfontchar *ch);
void FNTflipxfont(tfontchar *ch);
void FNTflipyfont(tfontchar *ch);

void FNTsavefontlist(char *filename,tfont* chlist);
tfont *FNTload(char *filename,tfont* chlist);
void FNTproportionalize(tfont* chlist);
UINT32 FNTgetwidth(char *txt);
UINT32 FNTgetwidthlength(char *txt,UINT32 pos);
UINT32 FNTgetheight(char *txt);
INT16 FNTgetindexfrompos(char *txt,int x);


//COLOUR & PALETTE HANDLING ROUTINES////////////////////////////////////////////////
//these need some work...

void COLdone();
char COLinit(UINT16 _maxcolours);
void COLsetrgb(UINT8 colno,UINT8 red,UINT8 green,UINT8 blue);
INT8 COLallocatergb(UINT8 rc,UINT8 gc,UINT8 bc);
void COLsetpal(trgba *pal,UINT8 start,UINT8 len);
void COLgethardwarepal(trgba *pal);
void COLsethardwarepal(trgba *pal,UINT16 start, UINT16 len);
void COLgetrgb(UINT8 colno,UINT8 *red,UINT8 *green,UINT8 *blue);
void COLsethsv(trgba *col,float h,float s,float v);
void COLgethsv(trgba *col,UINT8 &hi,UINT8 &si,UINT8 &vi);
void COLswaprgb(trgba *col1,trgba *col2);
void COLswapcol(UINT8 col1,UINT8 col2);
void COLcopycol(UINT8 col1,UINT8 col2);
void COLgrad(trgba *pal,UINT16 col1,UINT16 col2);
void COLgrad(tcolour *pal,UINT16 col1,UINT16 col2,trgba tcol1,trgba tcol2);
tpointfloat *zrotatepoint(tpointfloat *in,tpointfloat *out,flt r);
tpointfloat *zrotatepointaround(tpointfloat in,tpointfloat *out,flt r,tpointfloat *around);
extern trgba *colourmap;

inline void SET(trectfloat &a,trect &b)
{
    a.a.x=(float)b.a.x;
    a.a.y=(float)b.a.y;
    a.b.x=(float)b.b.x;
    a.b.y=(float)b.b.y;
}

/*inline void SET(trectflt &a,trect &b)
{
    a.a.x=(flt)b.a.x;
    a.a.y=(flt)b.a.y;
    a.b.x=(flt)b.b.x;
    a.b.y=(flt)b.b.y;
}*/

inline void SET(trectfloat &a,tsize &b)
{
    a.a.x=a.a.y=0.0f;
    a.b.x=(float)(b.x-1);
    a.b.y=(float)(b.y-1);
}




#endif

