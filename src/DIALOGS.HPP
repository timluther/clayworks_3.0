#ifndef dialogs_h
#define dialogs_h

#include "widgets.hpp"

#define cmRED 1
#define cmGREEN 2
#define cmBLUE 4
#define cmHUE 8
#define cmSATURATION 16
#define cmVALUE 32

#define cmHSVMASK 56
#define cmRGBMASK 7


#define seBOLD 1
#define seEMBOSSED 1


//file listing change flags
#define FLdrivechange 0x1
#define FLpathchange 0x2
#define FLnamechange 0x4
#define FLextchange 0x8

//file dialog types
#define FDTsave 0x1
#define FDTopen 0x2
#define FDTappend 0x3
#define FDTmultiopen 0x4 
#define FDTmultiappend 0x5

//dialog drawing modes
#define DTdrawall 1
#define DTdrawtext 2
#define DTdrawtext1 3


#define CMDcolourschanged 10000
#define CMDcolourschangedrelease 10001

class filtername
{
    char filter[10];
    char description[100];
};

class backview:public tview
{    
    public:
    tcolour bgcol;
    backview(INT16 x,INT16 y,INT16 x2,INT16 y2,tcolour bgcol_):tview(x,y,x2,y2){bgcol=bgcol_;growmode=TGgrowall;}
    
    virtual void draw(){t_fillcol=bgcol;bar(a.x,a.y,b.x,b.y);}    
};

class stringelem:public telem
{
    public:
    char *label;
    UINT8 attribute;
    tpoint pos;
    tcolour colour;   
};

class backviewwithtext:public tview
{    
    public:
    tcolour bgcol;
    tlinkedlist strings;
    backviewwithtext(INT16 x,INT16 y,INT16 x2,INT16 y2,tcolour bgcol_);
    ~backviewwithtext();
    void addstring(char *label,tpoint pos,UINT8 attribute,tcolour colour);            
    virtual void draw();
};

class fileviewstruct
{
    public:
    
    char fullpath[128];
    UINT32 okcommand,cancelcommand;    
    UINT32 filterindex;
    UINT32 fileindex;
    UINT32 filtercount;
    UINT8 type;
    listelem *presetfilters;            
};    

class fileview:public tgroup
{
    public:
    UINT8 drawmode;
    listbox *directories;
    listbox *files;
    edittextbox *pathname;
    tbutton *ok,*cancel;
    dropbox *filters;
    fileviewstruct *datasave;
    UINT32 dirdepth;//current depth of directory
    char drive[4];
    char path[128];
    char name[9];
    char ext[5];
    char searchfilter[12];                   
    fileview(UINT16 x,UINT16 y,fileviewstruct *dirinfo);
    virtual void handleevent(tevent *event);
    void draw();
    private:
    void exitifok();
    void getfilelisting();
    void getdirectorylisting();
    bool isfilter();
    UINT32 getcomponents(char *pathname);    
};

class colourselector;

class colourview:public tview
{
    public:    
    tbitmap *colourdisplay;
    screenDataType scrdc;    
    colourselector *cs;
    INT16 xp,yp;
             
    colourview(INT16 x,INT16 y,colourselector *cs);
    ~colourview();
    void rendercolours();
    
    virtual void draw();
    virtual void handleevent(tevent *event);    
};

class verticalcolourview:public tview
{
    public:    
    colourselector *cs;
    verticalcolourview(INT16 x,INT16 y,colourselector *cs);
    INT16 yp;
        
    virtual void draw();
    virtual void handleevent(tevent *event);            
};

class colourselector:public tgroup
{
    public:    
    trgba col;
    UINT8 h,s,v;
    colourview *cview;
    verticalcolourview *cviewv;
    UINT8 colourmode;
    edittextbox *htmlcode;
    numbox *nb_r,*nb_g,*nb_b,*nb_h,*nb_s,*nb_v;
    tbutton *ok,*cancel,*tg_r,*tg_g,*tg_b,*tg_h,*tg_s,*tg_v;
    colourselector(INT16 x,INT16 y,trgba *col);
    virtual void handleevent(tevent *event);
    void redrawviews(UINT8 what);
    virtual void draw();
    void setcolour(UINT8 r,UINT8 g,UINT8 b);
};

#endif
