//string table unit, to be included by every application
//all string constants go in here! 
//this file will be made available for translation

//Error codes
#ifndef _strtable_h_
#define _strtable_h_

#define ERR_NOERROR 0x0   //no vesa vbe driver found
#define ERR_NOVESA 0x1   //no vesa vbe driver found
#define ERR_NOLINEAR 0x2 //no linear buffer, segmented buffers suck
#define ERR_MODENOTAVAIL 0x3 //The requested video mode is not available
#define ERR_LOWDOSMEM 0x4 //The requested video mode is not available

extern char EXE_PATH[255];
extern unsigned short ERR_LEVEL;
extern char ERR_STRINGS[10][50];
extern char noyesstr[2][10];
extern char str_file[];
extern char str_directories[];
#endif
