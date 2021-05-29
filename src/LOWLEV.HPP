#ifndef lowlevelgraphicsdeclarations_h_
#define lowlevelgraphicsdeclarations_h_
#include "gbasics.hpp"

//The purpose of this file is to try and collect the messy, shared global variables of the 
//low level graphics drivers.
//Hopefully I can reduce this mostly to defines in the future.

typedef struct rgb512
{
    UINT32 r[512];
    UINT32 g[512];
    UINT32 b[512];    
}rgb512;

typedef struct rgb1024
{
    UINT32 r[1024];
    UINT32 g[1024];
    UINT32 b[1024];    
}rgb1024;

#define MISC_ADDR 0x3C2
#define MISC_READ_ADDR 0x3CC
#define ATTR_ADDR 0x3c0
#define ATTR_DATA_ADDR 0x3c1
#define SEQU_ADDR 0x3c4
#define SEQU_DATA_ADDR 0x3c5
#define CRTC_ADDR 0x3D4
#define CRTC_DATA_ADDR 0x3d5
#define GRPH_ADDR 0x3CE
#define GRPH_DATA_ADDR 0x3CF
#define STATUS_ADDR 0x3da
#define noclear 128
#define allplanes 3841

//Register masks
#define data_rotate 0x3
#define chainfour 0x8
#define oddeven 0x4
#define exetendedmem 0x2
#define alphaon 0x1

#define normalputsl8 0x00
#define xorputsl8 0x100
#define andputsl8 0x200
#define orputsl8 0x400
#define addputsl8 0x800
#define subputsl8 0x1000
#define blendputsl8 0x2000
#define chromaputsl8 0x4000




//these are global in here so they can be used in asm routines without hassle
#define COLHALFSHIFTBY 15
#define COLSHIFTBY 16
#define COLSHIFTBY5BIT 5
#define COLSHIFTBY6BIT 6
#define SHIFTBY 16
#define HALFSHIFTBY 15
#define MULBY 1<<SHIFTBY
#define HLINESIZE sizeof(thline)
#define THLINEOFFSET 1
//current maximum resolution, up it when better display h/w comes out :)

extern thlines *gr_chline;    //current scanline
extern PIXINT gr_ydiff,gr_y2;  //difference in y, lowest y point

extern thline hlinelist[1024];//768
extern gpoint leftlist[256],rightlist[256];
extern thline *lline;
extern UINT32 temp32;
extern INT32 d_x,px;
extern INT32 pu,pv;
extern INT32 pr,pb,pg,pa;
extern INT32 d_u,d_v;
extern UINT32 d_r,d_g,d_b,d_a;
//used for offsets into transparency table
extern UINT32 pa1,pa2;
extern INT32 wcx;      //width of polyscanline.. maybe <0 so HAS to be signed!
extern char *sadrs;
extern char *padrs;
extern tcolour pix;   //for texel
extern tcolour pixin; //for screen read

extern UINT32* startoffsets;

//these are used in the linedrawing routines
extern PIXINT addy;
extern PIXINT offs;
extern PIXINT linexdiff,lineydiff;

//could be replaced by a single var
extern BOOL clockwise;
extern UINT32 leadingbyte;
extern UINT32 extrabyte;

//these scan and render the scanlines of polygons
extern void (*scanedgeptr)(gpoint *p1,gpoint *p2,char offset);
extern void (*drawhlinesptr)(PIXINT ytop,PIXINT ybottom);   


//these perform colour clipping and convertion from 24 to 16bit colour
extern rgb1024 bound0to256_16;
extern rgb512 conv24to16;
extern trgba conv16to24[65536];
extern UINT32 bound0to256[1024];

//tri scanning structures
/*struct coordinterp_
{
      
      INT32 xl,dxl,xr,dxr,yred,dyred,ygreen,dygreen,yblue,dyblue,ya,dya,yu,dyu,yv,dyv,yz,dyz;      
}ic;*/

struct coordinterp_
{
      
      INT32 xl,dxl,xr,dxr,yred,dyred,ygreen,dygreen,yblue,dyblue,ya,dya,yu,dyu,yv,dyv,yz,dyz;      
}ic;

struct xdeltas_
{
      INT32 r,dr,g,dg,b,db,a,da,u,du,v,dv,z,dz;      
}xd;
//r=0
//dr=4
//g=8
//dg=12
//b=16
//db=20
//a=24
//da=28
//u=32
//du=36
//v=40
//dv=44
//z=48
//dz=52


#endif
