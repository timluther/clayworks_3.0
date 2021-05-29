
#ifndef views_h
#define views_h

//Windowing Basics header
/*Notes:
  a.) Events must NOT be called in creation of objects.
  b.) Make sure that the root object never tries to cal it's parent
      Non-group objects must allways be owned by a group | procedures
      at null pointers may be called resulting in a crash.
  c.) ALL drawing operations nust be called in a view's 'draw' function
      with possible case statments if only some things need to be drawn.
  d.) make sure all possible command events are handled in some way,
      avoiding misinterpertation(?!@#) of commands.
  e.) The global 'root' must always be set using the command 'inittwin'
  f.) The global 'global_select' is also important but it is handled solely
      within the code of this unit.
*/





#include "gbasics.hpp"
#include "ttypes.hpp"

#define TWINmodalstacklimit 20
//commands to container to sliders
#define CMDclose 1

//sent to a view when it's interted into another view
#define CMDinitialupdate 95


//window display attributes.. controls such things as the positioning of the title text on the window
#define TWtitletextafterindent 1
#define TWtitletextcentrex 2    //plonks the text in the middle.. needs a custom skin!
#define TWtitletextcentrey 4
#define TWblendedges 8          //future implementation.. edges of window bitmaps are blended together

#define TSvisible 0x1  //global visibility. If not set, nothing will be drawn!

extern UINT32 twSTATE;
extern UINT32 twDISPLAYATTRIB;
extern char RESOURCE_PATH[128];
extern trgbabase *windowselectedcolsrgb;
extern tcolour *windowselectedcols;
extern trgbabase *windowdarkcolsrgb;
extern tcolour *windowdarkcols;
extern tcolour windowcols[256];
extern trgbabase windowcolsrgb[256];

#define NULL 0
#define PIDundefined 65535 /*for views that don't want to be individuals*/

/*option flags*/
#define OFselectable     0x0001 
#define OFnooverlap      0x0002
#define OFaltkey         0x0004
#define OFxorient        0x0008
#define OFpopup          0x0010
#define OFtopselect      0x0020
#define OFautoraise      0x0040
#define OFsoliddrag      0x0080
#define OFkeygrab        0x0100
#define OFclicktopselect 0x0200
#define OFparentresize   0x0400
#define OFtabcycleselect 0x0800
#define OFautofocus      0x1000
#define OFclientedge     0x2000
#define OFnoresize       0x4000
#define OFdisturable     0x8000


/*status flags*/
#define SFvisible     0x0001 
#define SFselected    0x0002 
#define SFfocused     0x0004 
#define SFdrawnonce   0x0008 
#define SFdisabled    0x0010 
#define SFmodal       0x0020 
#define SFexposed     0x0040 
#define SFmute        0x0080
#define SFreserved    0x0100
#define SFdisturbed   0x0200
#define SFmouseover   0x0400
#define SFdeaf        0x0800
#define SFwaitingforrelease 0x1000
#define SFenabled 0x2000


/*Grow mode constants*/
#define TGgrowx1     0x01 
#define TGgrowy1     0x02 
#define TGgrowx2     0x04 
#define TGgrowy2     0x08 
#define TGdragonx1    0x10   /*Only  equiv of the changed side will move, not it's opposite*/
#define TGdragony1    0x20
#define TGdragonx2    0x40   /*Only  equiv of the changed side will move, not it's opposite*/
#define TGdragony2    0x80 


#define TGdragx1 TGgrowx1+TGdragonx1 
#define TGdragy1 TGgrowy1+TGdragony1 
#define TGdragx2 TGgrowx2+TGdragonx2 
#define TGdragy2 TGgrowy2+TGdragony2 
#define TGrelative   0x40

#define TGgrowallx   TGgrowx1+TGgrowx2 
#define TGgrowally   TGgrowy1+TGgrowy2 
#define TGgrowall    TGgrowallx+TGgrowally 

/*TGrowLoX   = right;
TGrowHiX   = left;
TGrowLoY   = bellow;
TGrowHiY   = above;
TGrowRel   = 0x10;*/

/*Event constants*/
#define EVmousedown   0x0001  /* Mouse button depressed */
#define EVmouseup     0x0002  /* Mouse button released  */
#define EVmousemove   0x0004  /* Mouse changed location */
#define EVmousedrag   0x0008  /* Periodic event while mouse button held down*/
#define EVmousepressed EVmousedown+EVmousedrag 
#define EVkeydown     0x0010  /* Key pressed    */
#define EVkeyup       0x0020  /* Key released */
#define EVcommand     0x0040  /* Command event  */


#define EVnothing    0x0000  /* Event already handled*/
#define EVmouse      0x000F  /* Mouse event   */
#define EVkeyboard   0x0030  /* Keyboard event*/
#define EVmessage    0xFF00  /* Message (command, broadcast, | user-defined) event*/
#define EVall        0xFFFF 

/*add_and_draw constants*/
#define VPcentre 1
#define VPbottomright 2 
#define VPtopright 3 
#define VPbottomleft 4 
#define VPtopleft 5 
#define VPindent 3  




class tview;
class tgroup;




struct tevent
{
  UINT8 what;
  UINT32 command;
  union
  {     
     tpointbase m;                  //mouse position                    
     char key;
     UINT16 scankey; 
     void *infoptr;
     tview *infotview;
     tgroup *infotgroup; 
     UINT8 infoUINT8;
     UINT16 infoUINT16;
     UINT32 infoUINT32;
     INT8 infoINT8;
     INT16 infoINT16;
     INT32 infoINT32;          
     char *infotxt;
  };
};

class windowgfx
{
    public:
    //also, for non client edge windows, a new set of "filler" gadgets is needed
    //topleft topright bottom left bottom right
    //corner gadgets, corner gadgets for buttons and interior corner gadgets (for client edges). All same size
    tbitmap *br,*tr,*bl,*tl,*rb,*mb,*lb,*br0,*br1,*itl,*ibl,*itr,*ibr;
    //connector gadgets, exterior and interior client edge. all horizontal, left and right rotated 90degrees
    //all same size, for now
    tbitmap *l,*r,*t,*b,*il,*ir,*it,*ib;
    //text area (spacer - text main -spacer) and filler gadgets.
    tbitmap *title[6];
    void freeall();
};

     



class tobject
{
  tobject();
  virtual ~tobject();
};

class tview:public trect
{   
  public:
  UINT16 pid;  
  UINT16 options,state;
  tview *next,*firstchild;
  tgroup *owner;
  tview *target;
  UINT8 growmode;

  tview(INT16 x1,INT16 y1,INT16 x2,INT16 y2);
  tview();
  virtual ~tview();
  /*this is the one you use if you don't want to redraw the view!*/
  virtual void changebounds(trect &bounds);
  virtual char setstate(UINT16 astate,char enable );
  virtual void draw()=0; 
  virtual void handleevent(tevent *event);
  virtual void initialupdate();

  BOOL isvisible(){return (state & SFvisible)!=0;}
  void makeinvisible(){state&=~SFvisible;}
  void makevisible(){state|=SFvisible;}
  void unmute(){state &=~SFmute;}
  void mute(){state |=SFmute;}
  void undeafen(){state &=~SFdeaf;}
  void deafen(){state |=SFdeaf;}

  void setbounds(UINT16 x1,UINT16 y1,UINT16 x2,UINT16 y2){rassign(x1,y1,x2,y2);};
  void setbounds(trect &bounds){rassign(bounds.a.x,bounds.a.y,bounds.b.x,bounds.b.y);};
  void getbounds(trect *bounds){bounds->a=a;bounds->b=b;};
  void tmoveto(INT16 x1,INT16 y1);
  void tmove(INT16 x1,INT16 y1);
  void tgrow(INT16 x1,INT16 y1);
  void calcbounds(trect &change);
  void limitmouse(trect &bounds);
  void reposview(trect &bounds);
  void select();
  //void dragview(char mode,trect limits,INT16 minx,INT16 miny,INT16 maxx,INT16 maxy);
  void drawview();
  void doneevent(tevent *event);
  void evcallback(UINT16 command);
  void sendcommand(UINT16 command,tview *destination,void *data);
  void whilemousein();
  void remove();
  tview *prev();
  void putinfrontof(tview *view,bool redraw);
  char exposed();
  char inview(INT16 x,INT16 y);
};
                   
                   
class tgroup:public tview
{    
  public:
  tview *lastchild;
  tgroup(INT16 x1,INT16 y1,INT16 x2,INT16 y2);
  tgroup();
  virtual ~tgroup();

  virtual void changebounds(trect &bounds);
  virtual void draw(); 
  virtual void handleevent(tevent *event);
  tview* insert(tview *view);
  tview* insertrel(tview *view);
  char add_and_draw(tview *view,char where,UINT32 pid);
  bool ischild(tview *view);
  
};


/*----****IMPORTANT GLOBALS****----*/
//standard icon, border and mouse pointer bitmaps
//these will be held in a the file clay.dat
extern tsize w_corner;
extern tsize w_btn;
/*extern UINT16 w_btnindentl;
extern UINT16 w_btnindentm;
extern UINT16 w_btnindentr;*/
extern UINT16 w_ch;  //conector height.. this will have a value for each connector in future. 
extern UINT16 w_cw;  //conector width 
extern tsize sb_btn;
extern UINT16 sb_basel,sb_basem,sb_baser;
extern UINT16 sb_l,sb_m,sb_r,sb_shad;
extern UINT16 tab_l; //tab left
extern UINT16 tab_m; //tab mid
extern UINT16 tab_r; //tab right
extern UINT16 tab_h; //tab height
extern UINT16 tab_fill;
extern UINT16 ns_btnh; //number selector button height
extern UINT16 ns_btnw; //number selector button width
extern UINT16 ns_spacerh; //number spacer/mouse scroller height
extern UINT16 drop_h; //drop bar height
extern UINT16 drop_btnw; //drop bar button wifth

extern tbitmap *m_pointer,*m_waiting,
                *m_pointup,*m_pointdown,*m_pointleft,*m_pointright,
                *m_rotate,*m_hand;
extern windowgfx w_s,w_u;  //see above for definition. Contains corner bitmaps and connector bars
extern tbitmap *tab_gfx[10];  //tab graphics, contains selected and deselected bitmaps
extern tbitmap *paper_gfx[8]; //gets dimensions from connector width, always square
extern tbitmap *ns_gfx[6];    //number selector graphics
extern tbitmap *drop_gfx[2];  //drop box graphics (just a button, uses paper gfx for main part)
extern tbitmap *sb_basex[3],*sbx[3]; //scrollbar X graphics. In file, only X are stored, the rest are rotatedf
extern tbitmap *sb_basey[3],*sby[3]; //scrollbar Y graphics
extern tbitmap *fileicons;
//extern tbitmap *b_connectors[4];     //outer 
extern tbitmap *b_close0,*b_close1,*b_close2;
extern tbitmap *b_min0,*b_min1,*b_min2;
extern tbitmap *b_max0,*b_max1,*b_max2;
extern tbitmap *b_up0,*b_up1,*b_up2;
extern tbitmap *b_down0,*b_down1,*b_down2;
extern tbitmap *b_right0,*b_right1,*b_right2;
extern tbitmap *b_left0,*b_left1,*b_left2;
extern tbitmap *b_toggle0,*b_toggle1;
extern tbitmap *TWINBM_shadow[7];

extern tview *modal_view;                
                
extern char charcode;
extern char scancode;
//extern UINT16 keycode;
extern theapmem disturbedlist;
extern UINT8 TWINdefaultgrowmode;
extern tview *global_selected;
extern tgroup *root;
//the modal stack is needed because views belonging to a modal dialog
//may be selected but on release, the modal behaviour of the dialog
//is still desired; therefore, this is nessiciary;
extern tview *TWINmodalstack[TWINmodalstacklimit];
extern INT32 TWINmodalstackhead;

BOOL inittwin(char *resourcedir);
void setroot(tgroup *root_);
void eventmpos();
void eventkey();
void closetwin();
bool ismodal(tview *view);

void setmodal(tview *view);
void endmodal();
void TWINendallmodals();
extern bool TWINtabkeybehaviour(tgroup *view,tevent *event);

#endif

