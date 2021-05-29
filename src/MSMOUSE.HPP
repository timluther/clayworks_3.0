//msmouse header file
#ifndef MSMOUSE_H
#define MSMOUSE_H

#include "gbasics.hpp"

#define clickwait 3
#define dclickwait 6
#define dclick_xthreshold 2
#define dclick_ythreshold 2
#define linger_for 400

#define MS_PRESSED 1
#define MS_RELEASED 2
#define MS_CLICKED 3
#define MS_DOUBLECLICKED 7
#define MS_MOVED 8
#define MS_LINGER 16
#define MS_VISIBLE 32
#define MS_WRAPMODE 64



//****Globals****
extern tcolour black;
extern signed int ms_visible;
extern char oldvis;
extern tpoint mp,moldp,mdiff;
extern UINT8 mb,moldb;

extern tsize msize;
extern char ms_pressed,ms_released,ms_clicked,ms_doubleclicked,ms_moved,ms_lingering;
extern UINT32 ms_attrib;



BOOL MOUSEchkouticon(INT16 x1,INT16 y1,INT16 x2,INT16 y2);
BOOL MOUSEchkicon(INT16 x1,INT16 y1,INT16 x2,INT16 y2);
BOOL MOUSEreset();
BOOL MOUSEinit(tbitmap* bm);
void MOUSEon();
void MOUSEoff();
void MOUSEstoprepeat();
void MOUSEstoprepeatwait(UINT16 wait);

void MOUSEshutdown();
void MOUSEpoll();
void MOUSEsetbounds(INT16 x,INT16 y,INT16 x2,INT16 y2);       
void MOUSEsetpos(INT16 x1, INT16 y1);
void MOUSEsetshape(INT16 x,INT16 y,UINT16 *mask);
void MOUSEsetmickeys(INT16 x,INT16 y);
void MOUSEsetsense(INT16 x,INT16 y,INT16 dspeed);
//var counter,Dcounter,Lcounter:word;

#endif 
