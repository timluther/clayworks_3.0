#include "lowlev.hpp"

 thlines *gr_chline;    //current scanline
 PIXINT gr_ydiff,gr_y2;  //difference in y, lowest y point

 thline hlinelist[1024]; //line list; not needed if using triangles
 gpoint leftlist[256],rightlist[256]; //left scanlist, right scan list
    
 thline *lline;
 UINT32 temp32;  //generic temporary storage
 INT32 d_x,px;   //difference in X, current X
 INT32 pu,pv;    //current U & V
 INT32 pr,pb,pg,pa; //current R,G,B & A
 INT32 d_u,d_v;  //UV deltas
 UINT32 d_r,d_g,d_b,d_a; //R,G,B & A deltas
//used for offsets into transparency table
 UINT32 pa1,pa2;     //alphaindex1 and alphaindex2
 INT32 wcx; //width of scanline
 char *sadrs;  //screen address
 char *padrs;  //image address
 tcolour pix;   //texel
 tcolour pixin; //for screen read
 UINT32* startoffsets;
//these are used in the linedrawing routines
 PIXINT  addy;
 INT32 offs;
 PIXINT  linexdiff,lineydiff;

//could be replaced by a single var
BOOL clockwise=TRUE;
UINT32 leadingbyte=0;
UINT32 extrabyte=0;

//these scan and render the scanlines of polygons
void (*scanedgeptr)(gpoint *p1,gpoint *p2,char offset);
void (*drawhlinesptr)(PIXINT ytop,PIXINT ybottom);   

//these perform colour clipping and convertion from 24 to 16bit colour
rgb1024 bound0to256_16;
rgb512 conv24to16;
trgba conv16to24[65536];
UINT32 bound0to256[1024];
