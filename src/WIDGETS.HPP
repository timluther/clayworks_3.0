#ifndef widgets_h
#define widgets_h

#include "views.hpp"
#include "gbasics.hpp"
//window creation flags

#define windowdrawborder 1
#define windowdrawall 2

#define tabdrawborder 1
#define tabdrawselected 2
#define tabdrawchange 3
#define tabdrawall 4

#define CFxbar 1
#define CFybar 2
#define CFstatusbar 4
#define CFclosebutton 8
#define CFminimizebutton 16
#define CFclientedge 32
#define CFnoresize 64
#define CFshadow 128
//dropbox drawing modes
#define DBtopbit 3
#define DBtext 1
#define DBbutton 2
#define DBlist 4
#define barminsize 16


//standard windows commands
#define CMDclosewindow 80
#define CMDmaximize 81
#define CMDminimizewindow 82
//commands from container classes
#define CMDxdeltachanged 90
#define CMDydeltachanged 91
#define CMDxdeltavalchanged 92
#define CMDydeltavalchanged 93
#define CMDxvalchanged 94
#define CMDyvalchanged 95
#define CMDvalchanged 96
#define CMDmakebarsinvisible 97
#define CMDmakebarsvisible 98
#define CMDchildunfocused 99
#define CMDdragselectstopped 100

//edit string commands
#define CMDeditstringaltered 131
#define CMDeditreturnpressed 132
//valuetype commands
#define CMDincval 110
#define CMDdecval 111
#define CMDselectedchanged   112
#define CMDitemclicked      113
#define CMDitemdclicked      114
#define CMDtoggledrop 115
#define CMDtabchanged 116
#define CMDcontainermodalended 120 //called when the modal mode of a container is ended by a click outside the view
#define CMDnumboxreleased 121
#define CMDdropboxclosed 122



#define LEselected 1
#define LEdisabled 1

#define listdrawall 1    //draws list, and sliderbars
#define listdrawchange 2  //draws the change in selection
#define listdrawselected 3 //just draws the selected item(s)
#define listdrawafter 4   //draw items after the 'drawafter' pointer
#define listdrawlist 5   //draws the whole list, but not the sliders or the border

#define menudrawall 1
#define menudrawchange 2



        
#define ETeditindent_y 3
#define ETeditindent_x 8
#define ETeditindex_x2 16

// Textbox attributes
#define TBnumeric 1  //is a number, no text shit allowed
#define TBfloat   3  //it's a float so one period allowed, and the exponential symbol


//button modes//
#define BMclickaction 0
#define BMreleaseaction 1
#define BMperiodaction 2
#define BMtoggleaction 3
#define BMglobaltoggleaction 4
#define BMbothtoggleaction 24


#define BMcheckboxcircle 5
#define BMcheckboxsquare 6
#define BMsquarebutton 7

//button attributes
#define BAdown 1  //don't change this whatever you do!
#define BAdrawonlyborder 2
#define BAstatechange 4
#define BAhighlightedborder 8
#define BAnobox 16
#define BAnooffset 32
#define BAxcentre 64
#define BAycentre 128
#define BAcentre 192

//Number sel attributes
#define NSdown 1  //don't change this whatever you do!
#define NSup 2
#define NSsliding 3

//slider attributes
#define SAxorient 1
#define SAmodalslide 2
#define SAdontdrawbuttons 4

//Toolbutton states
#define TBAdown 0x1       //button is down don't change this whatever you do!
#define TBAhighlighted 0x2    //button is highlighted, draw as appropriate
#define TBAseperator 0x4  //has no action, just a spacer
#define TBAdrawbox 0x8    //draw a box behind the image
#define TBAglobaltoggle 0x10    //if clicked, others with this flag will turn off
#define TBAtoggle 0x20    //local toggle button

//Menu states
#define MApopup 1
#define MAxorient 2
#define MAshadow 4
#define MAactive 8
//Menu item state
#define MIAchecked 1
#define MIA3dlook 2
#define MIAdisabled 4
#define MIAseperator 8


//Tree view states
#define squaresize 3
#define TEselected 1
#define TEdisabled 2
#define TEcollapsed 4



class tmenu;

class tmenuitem:public telem
{
    public:
    char text[20];
    tsize size;
    UINT8 shortkey;
    tbitmap *bitmap;
    UINT32 attrib;
    UINT32 command;
    tmenu *submenu;    
};

class tmenu:public tview
{
    public:
    tbitmap *sideimage;
    tmenu *parentmenu;
    tmenuitem *sel,*oldsel;
    tlinkedlist data;
    UINT32 mattrib;
    UINT8 drawmode;
    tmenu(INT16 x,INT16 y,UINT32 attrib_=0,tbitmap *sideimg=NULL);
    ~tmenu();
    tmenuitem *additem(char *text,UINT32 command,UINT32 attrib=0,tmenu *submenu=NULL,tbitmap *bm=NULL,UINT8 shortkey_=0);
    bool removeitem(tmenuitem *item);
    void drawitem(INT16 xpos,INT16 ypos,INT16 w,INT16 h,tmenuitem *item);
    void selectitem(tmenuitem *item);
    tmenuitem *getitematpos(tpoint p);
    bool setitem(tmenuitem *item,bool askey);
    bool opensubmenu(tmenuitem *item,bool askey);
    //closes any submenus in the menu then gets rid of itself
    tmenu *findopenleaf();
    tmenu *findroot();
    void closesubmenus();
    tmenuitem *getprevitem(tmenuitem *item);
    tmenuitem *getnextitem(tmenuitem *item);
    tpoint getitempos(tmenuitem *item);
    void colapse(UINT8 docommand,tmenu *submenu,tmenu *stopat);
    virtual void draw();
    virtual void handleevent(tevent *event);
    virtual char setstate(UINT16 astate, char enable);        
};


class ttoolbutton:public telem
{
    public:
    UINT32 command;
    UINT32 attribute;
    UINT32 image;
    
    ttoolbutton(UINT32 at,UINT32 img,UINT32 cmnd);
};

class ttoolbar:public tview
{
    public:
    tlinkedlist buttons;    
    ttoolbutton *lasttoggledown,*modalbutton;  //the last global toggling button that was pressed
    tbitmap *images;
    tsize bsize;
    UINT32 rows,cols; //calculated internally
    UINT32 numimages;
    ttoolbar(INT16 x,INT16 y,INT16 w,INT16 h,tbitmap *imgs,UINT8 numimgs,UINT16 btnx,UINT16 btny);
    addbutton(UINT32 attribute,UINT32 image,UINT32 command);
    bool switchbutton(ttoolbutton *button);
    bool switchbuttonbyref(UINT32 buttonref);
    ttoolbutton *getbuttonat(UINT16 x,UINT16 y); //in screen coordinates
    
    void calculatedimensions();//calculate rows and collums based on size of toolbar and the size of the buttons                     
    virtual void handleevent(tevent *event);
    virtual void draw();      
    virtual char setstate(UINT16 astate, char enable);        
};



class tbutton:public tview
{
  public:
  UINT8 borderwidth,action,dstate,style;
  tcolour *palette;
  UINT16 docommand;
  UINT8 hotkeycode;
  char *text;

  tbitmap *bm,*bms,*bmd;
    
  tbutton(INT16 x,INT16 y,INT16 w,INT16 h,
                 tcolour  *palette,UINT8 _borderwidth,
                 UINT8 _action,UINT16 _docommand,UINT8 hotkey=0,char dstate=0,
                 char *txt=NULL,tbitmap *bm_=NULL,tbitmap *bms=NULL,tbitmap *bmd_=NULL);
                 
  tbutton(INT16 x,INT16 y,
                 UINT8 _borderwidth,
                 UINT8 _action,UINT16 _docommand,
                 UINT8 hotkey,tbitmap *bm_,tbitmap *bms_=NULL,tbitmap *bmd_=NULL);
  
  
  virtual void handleevent(tevent *event);
  virtual void draw();      
  virtual char setstate(UINT16 astate, char enable);
  bool isdown(){return dstate & BAdown;}
  void setdown(){dstate|=BAdown;drawview();}
  void setup(){dstate&=~BAdown;drawview();}
  
};

class tcollection:public tgroup
{
    public:
    tbutton *lastselected;
    tcollection(INT16,INT16,INT16,INT16);
    ~tcollection();
    virtual void draw();
    virtual void handleevent(tevent *event);
};

class valuedata
{
    public:
    INT32 val,min,max;
};

class valuetype:public tgroup
{
    public:
    INT32 val,minval,maxval;    
   
    valuetype(INT32 minval_,INT32 maxval_,INT32 val_);
    bool setvalmute(INT32 val_);
    bool setval(INT32 val_);
    virtual void setrange(INT32 minval_,INT32 maxval_,INT32 val_);
};


class slidertype:public valuetype
{
    public:
    INT32 s1,s2,offset;
    INT16 buttonsize;    
    char attrib;
    UINT32 updatecommand;
    //slidertype(INT16 x,INT16 y,UINT16 l,INT32 minval_,INT32 maxval_,char attrib_,UINT32 updatecommand_=CMDvalchanged,INT32 val_=0);
    slidertype(INT16 x,INT16 y,INT16 l,INT32 minval_,INT32 maxval_,char attrib_,UINT32 updatecommand_=CMDvalchanged,INT32 val_=0);

    virtual void handleevent(tevent *event);
    virtual void draw();
    virtual void changebounds(trect &bounds);
    virtual void setrange(INT32 minval_,INT32 maxval_,INT32 val_);
    //private:
    void getsrect(trect &srect);
    void calcslidepos();
};


class edittextbox:public tview
{

    UINT8 tbattrib;
    UINT8 insertmode;
    INT32 curspos,curslen;
    //  start offset          string length
    INT32 off,xlen,slen,strmem,lastchar;
    INT16 cp1,cp2;     //cp1 & cp2 are the positions of the selection in pixels.
                                //lastchar is the index of the last character in the current visible field
    INT32 c1,c2; //cx are character index versions of cpx and lastchar is the index of the last visible character in the textbox
      
    public:
    char *txt;
    char *vistext;

    
    edittextbox(INT16 x,INT16 y,UINT8 attr,char *txt_,INT32 slen_,INT16 w);
    edittextbox(INT16 x,INT16 y,UINT8 attr,char *txt_,INT32 slen_,INT16 w,INT16 h);
    ~edittextbox();
    void calcinternalvars();
    //~edittextbox();
    virtual void draw();

    virtual void handleevent(tevent *event);
    virtual char setstate(UINT16 astate, char enable);
    char setcursorpos(INT32 pos,char followtail);
    void settext(char *string,bool selectall=false);
    //for numeric processing
    bool setval(UINT32 val);
    bool setvalflt(float val);
    float getvalfloat();
    UINT32 getvalint();        
    void setinsertmode(UINT8 mode){insertmode=mode;}
    void selectall();
    
    void inserttext(INT32 pos,char *txt_,INT32 length);
    void deletetext(INT32 pos,INT32 length);
    
};


class numbox:public valuetype
{
    public:
    edittextbox *edit;
    UINT32 updatecommand;
    UINT8 dmode;    
    numbox(INT16 x,INT16 y,INT16 w,UINT32 updatecommand_,INT32 minval_,INT32 maxval_,INT32 val_=NULL);
    ~numbox();
    virtual void setrange(INT32 minval_,INT32 maxval_,INT32 val_);
    virtual void handleevent(tevent *event);
    virtual void draw();
    
};

typedef struct listelembase_
{
    char txt[32];    
    UINT32 attrib;
    UINT32 contextattrib;
    INT16 offset;
    UINT8 image;
}listelembase;

class listelem:public listelembase
{
    public:
    set(char *txt_,INT16 offset_,UINT32 attrib_,UINT32 contextattrib_,UINT8 image_)
    {
        strcpy(txt,txt_);
        offset=offset_;
        contextattrib=contextattrib_;
        attrib=attrib_;
        image=image_;
    }
};

class scrollcontainer:public tgroup
{
    public:
    UINT32 attrib;
    tview *view;
    slidertype *xbar,*ybar;
    scrollcontainer(INT16 x1,INT16 y1,INT16 x2,INT16 y2,tview *view_,UINT32 attrib=3);
    virtual void draw();      
    //scrollcontainer(INT16 x1,INT16 y1,tview *view_,UINT32 attrib=3);
    virtual void handleevent(tevent *event);    
};


class listbox:public tview
{
  public:
  INT32 maxopts,maxylen,oldsel,sel,off;
  UINT8 drawmode;
  bool destroyold,changed;
  tbitmap *images;
  INT16 imgwidth;
  void (*rightclickfunc)(listbox *view,tevent *event);  
  listelem *info;

  listbox(INT16 x,INT16 y,INT16 w,INT16 h,listelem *info_,INT16 maxopts_,char destroyold_);
  ~listbox();
  listelem *getsel(){return &info[sel];}
  char *getseltext(){return info[sel].txt;}
  UINT32 getselimageindex(){return info[sel].image;}
  UINT32 getselattrib(){return info[sel].attrib;}
  UINT32 getselcontextattrib(){return info[sel].contextattrib;}
    
  void makelist(INT32 nopts,listelem *list,char drawit=TRUE);
  void setitem(INT32 opt,char *txt,INT16 offset,UINT32 attrib,UINT32 contextattrib,UINT8 img);
  void setlist(INT32 nopts,listelem *list,char drawit=TRUE);
  void kill_list(char drawit=TRUE);   
  void setval(INT32 val_);
  void drawitem(INT32 itemno);
  bool find(UINT32 from,char *txt);
  void sortlist(UINT32 from,UINT32 to);
  INT16 calcylen();
  void changebounds(trect &bounds);
  void calcinternalvars();
  virtual void handleevent(tevent *event);
  virtual void draw();      
  virtual char setstate(UINT16 astate, char enable);
};

//{*===================================*}
//{*        -*Tree box  type*-         *}
//{*===================================*}

typedef void (*gettreeitemdatafunc)(ttreenode *item,char **txt,tbitmap **pic,UINT32 **col);

class treebox:public tview
{
    public:
    ttree *tree;
    ttreenode *oldsel,*currsel,*ref;
    UINT8 drawmode;
    INT16 height;
    trect irect;
    INT32 off;
    gettreeitemdatafunc gettreeitemdata;
    void update();
    UINT32 *getattrib(ttreenode *node);
    treebox(INT16 x,INT16 y,INT16 w,INT16 h,ttree *tree,gettreeitemdatafunc gettreeitemdata_);
    ~treebox();
    INT32 calcylen();
    void killtree(ttreenode *node,char drawit=TRUE);    
    void setbarrange();
    ttreenode *getnextitem(ttreenode *celem);
    ttreenode *getprevitem(ttreenode *celem);
    void setsel(ttreenode *newelem);
    void drawitem(ttreenode *elem,INT32 ypos,bool justtext=FALSE);
    ttreenode *getitem(INT32 y);
    INT32 getypos(ttreenode *elem);    
    virtual void changebounds(trect &bounds);
    virtual void draw();
    virtual void handleevent(tevent *event);
    virtual char setstate(UINT16 astate, char enable);
};

class tabelement:public telem
{
    public:
    char name[32];
    tview *view;
    tbitmap *d,*u;
};

class tabbox: public tgroup
{
    public:
    UINT8 drawmode;
    tabelement *rootelem,*seltab;    
    tabbox(INT16 x1,INT16 y1,INT16 x2,INT16 y2);
    ~tabbox();
    tview *getseltabview(){return seltab->view;}
    UINT32 getseltabviewid(){return seltab->view->pid;}
    void drawtab(tabelement *ctab,INT16 x1,INT16 x2);
    virtual void draw();
    virtual void handleevent(tevent *event);
    //virtual void changebounds(trect &bounds);
    void addtabcontrol(tview *view,char *name);
    virtual char setstate(UINT16 astate, char enable);
};   
    


/*
bordertype=^bordertype_;
bordertype_=object(tview_)
  size:byte;
  Constructor create(xa,ya,xb,yb:integer;size_:byte);
  Procedure draw;virtual;
end;

*/






class dropbox:public tview
{
    public:
    UINT8 drawmode;
    //edittextbox text;
    tbutton *dropbutton;
    listbox *list;
    scrollcontainer *container;
    dropbox(INT16 x1,INT16 x2,INT16 y1,listelem *info_,INT16 maxopts_);
    ~dropbox();
    virtual void handleevent(tevent *event);
    virtual void draw();
    virtual void changebounds(trect &bounds);
    listelem *getsel(){return list->getsel();}
    char *getseltext(){return list->getseltext();}
    UINT32 getselimageindex(){return list->getselimageindex();}
    UINT32 getselattrib(){return list->getselattrib();}
    UINT32 getselcontextattrib(){return list->getselcontextattrib();}
    void toggledrop();
};

class window:public tgroup
{
    public:    
    UINT8 drawmode,resizemode;
    char *text;
    tbutton *close,*min,*max;
    slidertype *xbar,*ybar;
    tview *view;   //should know how to handle events from xbar and ybar
    window(INT16 x,INT16 y,INT16 w,INT16 h,char *txt,tview *cview,UINT16 createflags);
    //window(INT16 x,INT16 y,char *txt,tview *cview,UINT16 createflags);

    
    virtual void draw();
    virtual void handleevent(tevent *event);
    virtual char setstate(UINT16 astate, char enable);
};

class bordercontainer:public tgroup
{
    public:
    bordercontainer(INT16 x,INT16 y,INT16 w,INT16 h,tview *inner);
    bordercontainer(INT16 x,INT16 y,tview *inner);        
    virtual void draw();
    virtual void handleevent(tevent *event);
};


////////////////////////////////////////////
//external function declarations

extern void drawclientedge(INT16 x1,INT16 y1,INT16 x2,INT16 y2,windowgfx *wg);
extern void drawouteredge(INT16 x1,INT16 y1,INT16 x2,INT16 y2,windowgfx *wg);
extern UINT32 findlongesttext(listelembase *list,UINT32 nopts);
void textboxb(INT16 x1,INT16 y1,INT16 x2,INT16 y2);
void textboxfilledb(INT16 x1,INT16 y1,INT16 x2,INT16 y2);


#endif
