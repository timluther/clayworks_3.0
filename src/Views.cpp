//////windowing basics code 
#include "stdafx.h"
#include <conio.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "imgloadr.hpp"
#include "dosstuff.hpp"
#include "views.hpp"
#include "msmouse.hpp"
#include "gprim.hpp"


  
/*----****IMPORTANT GLOBALS****----*/
//all of these values control the size of graphics used in the widgets (most of this is actually used in widgets.h)
//the naming convention (roughly) is an abrieviated form of the widget in question (w for window, sb for scroll bar ect)
//and then an underbar folled by an abrieviated description
//w is used for width, h for height, l for left, m for middle, r for right, c for corner (normally a tsize varialbe containing X and Y size).
//btn means button
tsize w_corner;
tsize w_btn;
//UINT16 w_btnindentl;
//UINT16 w_btnindentm;
//UINT16 w_btnindentr;
//note that the left and right connectors are read in as horizontal bars and then rotated by 90 degrees
UINT16 w_ch;  //conector height.. this will have a value for each connector in future. 
UINT16 w_cw;  //connector width 
UINT16 w_titlebarl;
UINT16 w_titlebarm;
UINT16 w_titlebarr;
UINT16 w_titlefillerl;
UINT16 w_titlefillerm;
UINT16 w_titlefillerr;

tsize sb_btn;
UINT16 sb_basel,sb_basem,sb_baser;
UINT16 sb_l,sb_m,sb_r,sb_shad;
UINT16 tab_l;
UINT16 tab_m;
UINT16 tab_r;
UINT16 tab_h;
UINT16 tab_fill;

UINT16 ns_btnh; //number selector button height
UINT16 ns_btnw; //number selector button width
UINT16 ns_spacerh; //number spacer/mouse scroller height
UINT16 drop_h; //drop bar height
UINT16 drop_btnw; //drop bar button width


char RESOURCE_PATH[128];
char charcode;
char scancode;

tview *TWINmodalstack[TWINmodalstacklimit];
INT32 TWINmodalstackhead;

UINT32 TWINattributes;

trgbabase *windowselectedcolsrgb;
tcolour *windowselectedcols;
trgbabase *windowdarkcolsrgb;
tcolour *windowdarkcols;

tcolour windowcols[256];
trgbabase windowcolsrgb[256];
/*=
{{0,0,0,0},{27,27,27,0},{43,43,43,0},{63,63,63,0},{83,83,83,0},{103,103,103,0}
,{119,119,119,0},{139,139,139,0},{159,159,159,0},{179,179,179,0},{195,195,195,0},{215,215,215,0}
,{235,235,235,0},{255,255,255,0},{255,255,255,0},{100,100,160,0}
,{0,0,0,0},{39,39,103,0},{51,51,115,0},{73,73,137,0},{95,95,155,0},{111,111,167,0}
,{123,123,179,0},{139,139,187,0},{155,155,199,0},{175,175,221,0},{191,191,223,0},{221,221,231,0}
,{231,231,243,0},{255,255,255,0},{255,255,139,0},{255,111,0,0}
,{0,0,0,0},{17,17,17,0},{23,23,23,0},{43,43,43,0},{63,63,63,0},{83,83,83,0}
,{109,109,109,0},{119,119,119,0},{139,139,139,0},{159,159,159,0},{175,175,175,0},{195,195,195,0}};
*/

windowgfx w_s,w_u;

tcolour buttoncols[5];
/*tcolour scrollbackcols[5];
tcolour scrollbuttoncols[5];*/
UINT32 twSTATE;
UINT32 twDISPLAYATTRIB;
UINT8 TWINdefaultgrowmode=TGgrowall;


tview *terminal_mouserecipient=NULL;
tview *modal_view=NULL;
tview *global_selected=NULL;
tview *last_mouseover=NULL;
tgroup *root=NULL;
tbitmap *m_pointer,*m_waiting,
        *m_pointup,*m_pointdown,*m_pointleft,*m_pointright,
        *m_rotate,*m_hand;
tbitmap *fileicons;
tbitmap *sb_basex[3],*sbx[3];
tbitmap *sb_basey[3],*sby[3];
tbitmap *tab_gfx[10];
tbitmap *paper_gfx[8]; //gets dimensions from connector width, always square
tbitmap *ns_gfx[6];    //number selector graphics
tbitmap *drop_gfx[2];  //drop box graphics (just a button, uses paper gfx for main part)
tbitmap *b_toggle0,*b_toggle1;
tbitmap *b_close0,*b_close1,*b_close2;
tbitmap *b_min0,*b_min1,*b_min2;
tbitmap *b_max0,*b_max1,*b_max2;
tbitmap *b_up0,*b_up1,*b_up2;
tbitmap *b_down0,*b_down1,*b_down2;
tbitmap *b_right0,*b_right1,*b_right2;
tbitmap *b_left0,*b_left1,*b_left2;        
tbitmap *TWINBM_shadow[7];


/************************====--- TObject ---====************************/
tobject::tobject(){ }
tobject::~tobject(){ }

/************************====--- tview ---====************************/
/*make is for view whose dimensions are determined latter -they are minimal
 constructrs*/
tview::tview()
{
  growmode=TWINdefaultgrowmode;
  target=NULL;
  owner=NULL;
  next=firstchild=NULL;
  pid=PIDundefined;
  state=SFenabled;  
  options=OFselectable;
}

tview::tview(INT16 x1,INT16 y1,INT16 x2,INT16 y2)
{
  a.x=x1; 
  a.y=y1;
  b.x=x2; 
  b.y=y2;
  pid=PIDundefined;
  
  growmode=TWINdefaultgrowmode;
  target=NULL;
  owner=NULL;
  next=firstchild=NULL;
  state=SFvisible+SFenabled;
  options=OFselectable+OFsoliddrag;
}

tview::~tview()
{
    if (modal_view==this)
       endmodal();            
}

void tview::tmove(INT16 x1,INT16 y1)
{  
  trect tmp_loader;
  getbounds(&tmp_loader);
  tmp_loader.rmove(x1,y1);
  reposview(tmp_loader);
}

void tview::tmoveto(INT16 x1,INT16 y1)
{  
  trect tmp_loader;
  getbounds(&tmp_loader);
  tmp_loader.rpos(x1,y1);  
  reposview(tmp_loader);
}

/*        Procedure Tview_.calcbounds(change:trect);
var dragx,dragy:boolean;
    tmp_loader:trect;
Begin
  getbounds(tmp_loader);
  with tmp_loader do
  if change.empty then
    rmove(change.x1,change.y1)
  else begin
    dragX:=growmode and TGdragonX <> 0;
    dragY:=growmode and TGdragonY <> 0;
    if growmode and TGgrowx1<>0 then begin
      inc(x1,change.x1);if dragx then inc(x2,change.x1);
    end;
    if growmode and TGgrowy1<>0 then begin
      inc(y1,change.y1);if dragy then inc(y2,change.y1);
    end;
    if growmode and TGgrowx2<>0 then begin
      inc(x2,change.x2);if dragx then inc(x1,change.x2);
    end;
    if growmode and TGgrowy2<>0 then begin
      inc(y2,change.y2);if dragy then inc(y1,change.y2);
    end;
  end;
  changebounds(tmp_loader);
end;*/
        
void tview::calcbounds(trect &change)
{
  trect tmp_loader;
  getbounds(&tmp_loader);
  if (change.rempty())
    tmp_loader.rmove(change.a.x,change.a.y);
  else
  {
    if ((growmode & (TGgrowx1|TGdragonx1))!=0)
    {
      tmp_loader.a.x+=change.a.x;
      if ((growmode & TGdragonx1)!=0) tmp_loader.b.x+=change.a.x;
    }
    if ((growmode & (TGgrowx2|TGdragonx2))!=0)
    {
      tmp_loader.b.x+=change.b.x;
      if ((growmode & TGdragonx2)!=0) tmp_loader.a.x+=change.b.x;
    }    
    if ((growmode & (TGgrowy1|TGdragony1))!=0)
    {
      tmp_loader.a.y+=change.a.y;
      if ((growmode & TGdragony1)!=0) tmp_loader.b.y+=change.a.y;
    }    
    if ((growmode & (TGgrowy2|TGdragony2))!=0)
    {
      tmp_loader.b.y+=change.b.y;
      if ((growmode & TGdragony2)!=0) tmp_loader.a.y+=change.b.y;
    }
  }
  changebounds(tmp_loader);
}

/*void tview::dragview(char mode,trect limits,INT16 minx,INT16 miny,INT16 maxx,INT16 maxy)
{
  char movegrowmode;
  trect tmp_loader,oldbounds,offsets;

  movegrowmode=mode & (TDmove+TDgrow);
F  getbounds(&tmp_loader);
  offsets.rassign(a.x-mp.x,a.y-mp.y,b.x-mp.x,b.y-mp.y);
  if (((options & OFsoliddrag)==0)&&(owner!=NULL))
    SC.viewport.rintersect(*owner);

  MOUSEsetbounds(limits.a.x,limits.a.y,limits.b.x,limits.b.y);
  while (mb!=0){
    mpos();
    oldbounds=tmp_loader;
    if (movegrowmode == TDmove)
      rpos(mp.x+offsets.a.x,mp.y+offsets.a.y);
    else {
      tmp_loader.b.x=mp.x+offsets.b.x; 
      tmp_loader.b.y=mp.y+offsets.b.y;
    }
    if ((tmp_loader.b.x-tmp_loader.a.x)<minx)  tmp_loader.b.x=tmp_loader.a.x+minx;
    if ((tmp_loader.b.y-tmp_loader.a.y)<miny)  tmp_loader.b.y=tmp_loader.a.y+miny;
    if ((tmp_loader.b.x-tmp_loader.a.x)>maxx)  tmp_loader.b.x=tmp_loader.a.x+maxx;
    if ((tmp_loader.b.y-tmp_loader.a.y)>maxy)  tmp_loader.b.y=tmp_loader.a.y+maxy;

    if ((options & OFsoliddrag)!=0) {
      if ((oldbounds!=tmp_loader) && (ms_moved)) 
        reposview(tmp_loader);
    } else {
      setwritemode(xorput);
      t_col=selectcol;
      MOUSEoff();
      thickrectangle(tmp_loader.a.x,tmp_loader.a.y,tmp_loader.b.x,tmp_loader.b.y,3);MOUSEon();
      while ((! ms_moved)&&(mb!=0)) mpos();
      MOUSEoff();
      thickrectangle(tmp_loader.a.x,tmp_loader.a.y,tmp_loader.b.x,tmp_loader.b.y,3);MOUSEon();
      setwritemode(0);
    }
  }
  if ((options & OFsoliddrag)==0)
  {
    SC.viewport=SC.screenport;
    reposview(tmp_loader);    
  }
  MOUSEsetbounds(SC.screenport.a.x,SC.screenport.a.y,SC.screenport.b.x,SC.screenport.b.y);
}*/

void tview::reposview(trect &bounds)
{
  trect old,oldvp;
  tview *currview;
 
  getbounds(&old);
  if (old==bounds) return;
  changebounds(bounds);
  if ((state & SFvisible)!=0)
  {
  MOUSEoff();
  ms_visible=-128;

  drawview();
  if (owner!=NULL)
  {
    oldvp=SC.viewport;
    old.rintersect(SC.viewport);
    SC.viewport=old;
    currview=next;
    while (currview!=NULL)
    {
      currview->drawview();
      currview=currview->next;
    }
    SC.viewport=oldvp;
  }
  
  ms_visible=1;
  MOUSEon();
  }
}

void tview::changebounds(trect &bounds)
{
    rassign(bounds.a.x,bounds.a.y,bounds.b.x,bounds.b.y);
}

void tview::select()
{
  char sf;
  tview *selview;
  if ((global_selected==this)||(owner==NULL)) return;
  sf=SFselected+SFfocused;
  if ( ((state & SFselected)==0)&&((options & OFselectable)!=0)&&(owner!=NULL))
  {    
    selview=this;
    //traverse up tree, marking owners so that they don't get defocused
    while (selview!=NULL)
    {      
      selview->state|=SFreserved;
      selview=selview->owner;      
    }  
    //traverse tree, deselecting owners*/         
    while (global_selected!=NULL)
    {
       if ((global_selected->state & SFreserved)==0)
           global_selected->setstate(sf,FALSE);
       global_selected=global_selected->owner;
    }
    global_selected=this;
    selview=owner;
    /*traverse up tree, selecting owners and reseting reserved flag*/
    while (selview!=NULL)
    {
        selview->state&=~SFreserved;            
        selview->setstate(SFfocused,TRUE);        
        selview=selview->owner;      
    }
    state&=~SFreserved;    
    setstate(sf,TRUE);
    if ((options & OFtopselect)!=0) 
       putinfrontof(owner->firstchild,true);        
  }
}


char tview::setstate(UINT16 astate,char enable)
{    
    UINT16 oldstate=state & ~SFreserved;
    (enable==1)?state|=astate:state &= astate^0xFFFF;//));
    return ((state&~SFreserved)!=oldstate);
}


void tview::drawview()
{
    trectlist *p,*oldlop,*plop,*newp;//,*prevrect;
    trect oldvp;
    tgroup  *parent;
    tview *currview,*stopat;
    UINT32 rectidx;
    char *rectpos;
    if ((roverlap(SC.viewport))&&(state & SFvisible)&&(twSTATE & TSvisible)&&(b.x>a.x)&&(b.y>a.y))
    {
        //this part does the basic clipping and also ensures that the view is not
        //drawn if any of it's parents are invisible or if the view is
        //not part of the main windows heirarchy (i.e. if ultimate parent is not 'root')
        rectidx=rectbuffer.currindex;
        rectpos=rectbuffer.curr;
        oldvp=SC.viewport;
        SC.viewport.rintersect(*this);
                //stopat=root;
        if (this!=root)
        {
            parent=owner;        
            while (parent!=NULL)
            {
                if ((parent->state & SFvisible)==0) goto end;           
                SC.viewport.rintersect(*parent);
                stopat=(tview*)parent;
                parent=parent->owner;
            }        
            if (stopat!=(tview*)root)goto end;
        }
        p=getrectmem();

        *p=SC.viewport;
        //p->rassign(SC.viewport.a.x,SC.viewport.a.y,SC.viewport.b.x,SC.viewport.b.y);
        p->next=NULL;
        parent=owner;
        stopat=this;

        while (parent!=NULL)
        {
            currview=parent->firstchild;
            if ((parent->options & OFnooverlap) == 0)
            while ((currview!=stopat)&&(p!=NULL))
            {
                /*traverse current rectangle list*/
                plop=p;
                
                while (plop!=NULL)
                {
                                                          
                    newp=plop->cutfrom(*currview);                    
                    switch (rectlist_status)
                    {
                        /*replace current rectangle with the rectangles cut away from it*/
                        
                        case RSlist:
                            lastrect->next=plop->next;
                            plop->a=newp->a;
                            plop->b=newp->b;                                                        
                            plop->next=newp->next;                            
                            plop=lastrect->next;
                            /*should check for 'concatablity' by 'ere*/
                            break;                                           
                        case RSobscured:
                            newp=plop->next;
                            if (plop==p)
                                p=newp;
                            else  //get previous and setup it's 'next' ptr;
                            {
                                oldlop=p;
                                while ((oldlop!=NULL)&&(oldlop->next!=plop))
                                    oldlop=oldlop->next;
                                oldlop->next=newp;
                                
                            }
                            plop=newp;
                            break;
                        /* leave it alone if nothing touches it*/
                        default:                            
                            plop=plop->next;
                            break;
                    }
                }
                currview=currview->next;
            }
            stopat=parent;
            parent=parent->owner;
        }    
        if (p!=NULL)
        {
            /*draw loop*/
            plop=p;
            MOUSEoff();
            while (plop!=NULL)
            {
                SC.viewport=*plop;
                draw();
                
                state=state | SFdrawnonce;
                plop=plop->next;
            }

            MOUSEon();          
            state^=SFdrawnonce;           
        }
end:        
        SC.viewport=oldvp;          
        rectbuffer.currindex=rectidx;
        rectbuffer.curr=rectpos;
    }
}


/*the allways overridden standard handling procedure*/
void tview::handleevent(tevent *event)
{
  if ((event->what & EVmouse)!=0)
  {
      terminal_mouserecipient=this;
  }else
  if (event->what==EVcommand)
  {
      switch (event->command)
      {
      case CMDclose:remove();event->what=0;break;
      }
  }
}


//called when inserted
void tview::initialupdate()
{
}

void tview::doneevent(tevent *event)
{
  event->what=0;
}

void tview::evcallback(UINT16 command)
{  
    sendcommand(command,target,this);
}

void tview::sendcommand(UINT16 command,tview *destination,void *data)
{
  if ((destination==NULL)||(state & SFmute)) return;
  tevent sendevent;
  sendevent.what=EVcommand;
  sendevent.infoptr=data;
  sendevent.command=command;
  if ((state & SFmute)==0) 
      destination->handleevent(&sendevent);
}

void tview::limitmouse(trect &bounds)
{
  MOUSEsetbounds(bounds.a.x-(a.x-mp.x),bounds.a.y-(a.y-mp.y),bounds.b.x-(b.x-mp.x),bounds.b.y-(b.y-mp.y));
}

tview *tview::prev()
{
  tview *currview;
  if (owner==NULL) return NULL;
  currview=owner->firstchild;
  if (currview==this) return currview;
  while (currview->next!=this)
  {
    if (currview==NULL)return NULL;
    currview=currview->next;    
  }
  return currview;
}

void tview::putinfrontof(tview *view,bool redraw)
{
  if (owner==NULL) return;  
  if ((owner->lastchild==owner->firstchild)||(this==owner->firstchild)) return;
  prev()->next=next;  
  next=owner->firstchild;
  owner->firstchild=this;
  if ((redraw)&&((state & SFvisible)!=0))
  {
      MOUSEoff();
      ms_visible=-128;
      drawview();
      ms_visible=1;
      MOUSEon();
  }
  //next=owner->firstchild;
  //owner->firstchild=this;
  //draw();
  /*tview *currview;  
  trect oldvp;
  trectlist *plist,*currp;
  char nooverlaps;
    
  if (view!=this)
  {
    if ((owner!=NULL)&&(this!=owner->firstchild))
    {
      rectbuffer.reset();
      currview=owner->firstchild;
      plist=getrectmem();
      nooverlaps=TRUE;
      currp=plist;
      while (currview!=this)
      {
        if (currview->roverlap(*this))
        {
          nooverlaps=FALSE;
          currp->rassign(currview->a.x,currview->a.y,currview->b.x,currview->b.y);
          currp->rintersect(SC.screenport);
          if (currview->next!=this)
          {
            currp->next=getrectmem();            
            currp=currp->next;
          }
        }
        currview=currview->next;
      }
      currp->next=NULL;
    }

    //rearange the list
    if (view->owner->firstchild==view)
        view->owner->firstchild=this; 
    else if (view->owner->firstchild=this)
        view->owner->firstchild=next;
    prev()->next=next;    
    view->prev()->next=this;
    next=view;
    if ((state & SFvisible)!=0)
    {
        oldvp=SC.viewport;
        
        
        //draw 'self' in only the areas that have been revealed
        if (owner->firstchild==this) select();
        if (nooverlaps)
            currp=NULL;
        else
            currp=plist;
        while (currp!=NULL)
        {
            SC.viewport=*currp;
            drawview();
            currp=currp->next;
        }        
        
        //write on views, if any, that will obscure 'self'
        currview=owner->firstchild;
        SC.viewport=*this;
        while ((currview!=this)&& (currview!=NULL))
        {
            currview->drawview();
            currview=currview->next;
        }
        SC.viewport=oldvp;
    }
    rectbuffer.reset(); 
  }  */
}

char tview::inview(INT16 x,INT16 y)
{
  tgroup *parent=owner;
  tview *stopat=this,*currview;
  
  if (!rcontains(x,y)) return FALSE;
  
  while (parent!=NULL)
  {
      currview=parent->firstchild;
      if (((parent->options & OFnooverlap) == 0) )
      while (currview!=stopat)
      {
          if ((currview->rcontains(x,y))|(! parent->rcontains(x,y)))
              return (FALSE);
          currview=currview->next;
      }
      stopat=parent;
      parent=parent->owner;
  }
  return (TRUE);  
}

char tview::exposed()
{
    tgroup *parent=owner;
    tview *currview=this,*stopat;
    //stopat=root
    while (parent!=NULL)
    {
        currview=parent->firstchild;
        if ((parent->options & OFnooverlap) == 0 )
        while (currview!=stopat)
        {
            if (stopat->rinside(*currview))
                return (FALSE);
            currview=currview->next;
        }
        stopat=parent;
        parent=parent->owner;
    }
    return(TRUE);
}

void tview::remove()
{
    if (owner==NULL) return;
    trect oldvp;
    tview *currview;
    oldvp=SC.viewport;
    SC.viewport=*this;
    SC.viewport.rintersect(0,0,SC.size.x,SC.size.y);
    currview=prev();
    
    if (currview)
      currview->next=next;
    if (owner->firstchild==this)
        owner->firstchild=next;
    if (owner->lastchild==this)
        owner->lastchild=currview;
        
    root->select();        
    currview=next;
    while (currview!=NULL)
    {
         currview->drawview();
         currview=currview->next;
    }
    target=NULL;
    owner=NULL;
    state&=~SFvisible;
    //currview->drawview();
    SC.viewport=oldvp;    
} 

void tview::whilemousein(){
  while (inview(mp.x,mp.y) && (mb!=0));
    //mpos();
}

/************************====--- tgroup_ ---====************************/

tgroup::tgroup(INT16 x1,INT16 y1,INT16 x2,INT16 y2):tview(x1,y1,x2,y2)
{
  lastchild=NULL;  
}

tgroup::tgroup():tview()
{
  lastchild=NULL;  
}

void tgroup::handleevent(tevent *event)
{
      tview *currview;
      
      tview::handleevent(event);
      if ((event->what & EVmouse)!=0)
      {
          currview=firstchild;
          while ((event->what!=0)&&(currview!=NULL))
          {
              if ((currview->inview(mp.x,mp.y)))              
                  currview->handleevent(event);              
              currview=currview->next;
          }                      
      }
      else if ((event->what & EVkeyboard)!=0)
      {          
          currview=firstchild;
          while ((currview!=NULL)&&(event->what!=0))
          {
              if ((currview!=global_selected)&&(currview->options & OFkeygrab!=0))
                 currview->handleevent(event);                 
              currview=currview->next;
          }             
      }
}

tview* tgroup::insertrel(tview *view)
{
  //relative insert
  trect tmp=trect(view->a.x+a.x,view->a.y+a.y,view->b.x+a.x,view->b.y+a.y);
  view->changebounds(tmp);
  return insert(view);  
}

tview* tgroup::insert(tview *view)
{   
  if (lastchild==NULL)
  { 
      lastchild=view;
      firstchild=view;            
  } else
  {
      view->next=firstchild;
      firstchild=view;
  }
  lastchild->next=NULL;
  view->target=(tview*)this;
  view->owner=this;
  //sendcommand(CMDinitialupdate,view,NULL);
  view->state|=SFvisible;
  view->initialupdate();
  return view;  
  /*view->select();
 */
}

/*
procedure Tgroup_.insert(view:tview);
begin
  if last=nil then begin
    last:=view;
    first:=view;
  end else begin
    view^.next:=first;
    first:=view;
  end;
  last^.next:=nil;
  view^.owner:=@self;
  {state:=state and not sfvisible;
  view^.select;
  state:=state or sfvisible;}
end;
*/

char tgroup::add_and_draw(tview *view,char where,UINT32 pid)
{
  trect tmprect;
  tview *currview=firstchild;
  
  while (currview!=NULL)
  {
      if ((currview->pid!=PIDundefined)&&(currview->pid==pid))
      {
          currview->putinfrontof(firstchild,true);
          delete view;
          return false;
      }
      currview=currview->next;
  }
  insert(view);
  view->pid=pid;
  tmprect=*view;
  switch (where)
  {
      case VPcentre:
          tmprect.rpos(a.x+halfx()-view->halfx(), a.y+halfy()-view->halfy());
          break;
      case VPbottomright:
          tmprect.rpos(b.x-(view->b.x-view->a.x)-VPindent,b.y-(view->b.y-view->a.y)-VPindent);
          break;
      case VPtopright:
          tmprect.rpos(b.x-(view->b.x-view->a.x)-VPindent,a.y+VPindent);
          break;
  }
  if (where!=0) view->changebounds(tmprect);
    view->drawview();
  return(TRUE);
}

tgroup::~tgroup()
{
  tview *nextview;   
  char leave=0;
  if (firstchild!=NULL )
  while (1)
  {
      nextview=firstchild->next;
      delete firstchild;
      if(!firstchild) break;
      firstchild=nextview;
  }
  tview::~tview(); 
}

void tgroup::draw()
{
    if (firstchild!=NULL)
    {
        trect oldvp;
        tview *currview=firstchild;
        while (currview!=NULL)
        {
            
            if ((options & OFnooverlap) != 0)
            {
                if (currview->roverlap(SC.viewport))
                {
                    oldvp=SC.viewport;
                    SC.viewport.rintersect(*currview);
                    currview->draw();
                    SC.viewport=oldvp;                    
                }
            } else
                currview->drawview();
            currview=currview->next;
        }
    }
}

void tgroup::changebounds(trect &bounds)
{

  trect diff;
  tview *currview ;
  /*get the difference between the old rectangle && the new*/
  diff.a=bounds.a-a;
  diff.b=bounds.b-b;
  setbounds(bounds);

  currview=firstchild;
  /*options=options | ofparentresize; */
  while (currview!=NULL)
  {
      currview->calcbounds(diff);
      currview=currview->next;
  }
  /*options=options xor ofparentresize;*/
}

bool tgroup::ischild(tview *view)
{
    tview *v=firstchild;
    while (v!=NULL)
    {
       if (v==view)      
          return true;
       v=v->next;
    }
    return false;
}

//loose functions. Should really put a TWIN prefix or something on these things

bool TWINtabkeybehaviour(tgroup *view,tevent *event)
{
    if (event->what & EVkeydown)
    {
        if (scancode==tabkey)
        {
          if ((global_selected==NULL)||(global_selected->next==NULL)||(!view->ischild(global_selected)))
            view->firstchild->select();
          else          
            global_selected->next->select();
          return true;                  
        }        
    }
    return false;
}

/*Core key && mouse event handling*/
void eventmpos()
{
  terminal_mouserecipient=NULL;
  UINT32 what=0;
  tevent mevent;
  MOUSEpoll();
  if (ms_released) 
      what+=EVmouseup;
  else
  {
      if (mb!=0) 
          what+=EVmousedrag;
      if (ms_pressed)  
          what+=EVmousedown;
  }
  if (ms_moved)
      what+=EVmousemove;
            
  if (what!=0)
  {
      mevent.what=what;
      if (modal_view!=NULL)
         modal_view->handleevent(&mevent);
      else if (mevent.what!=0) 
         root->handleevent(&mevent);
         
      if (terminal_mouserecipient)
      {
          //if applicable, select terminal recipient and and focus it's ancestors
          if (((terminal_mouserecipient->options & OFautoraise)!=0)||(mb!=0))
             terminal_mouserecipient->select();
          //set mouseover flag for all ancestors of terminal recipient
          if ((terminal_mouserecipient->state & SFmouseover)==0)
          {
            tview *loopback=terminal_mouserecipient;
            //switch on mouseover flags and reseverd flags       
            while (loopback)
            {               
               loopback->setstate(SFmouseover+SFreserved,true);
               loopback=loopback->owner;
            }
            //switch off all flags that were not just set
            while (last_mouseover)
            { 
               if ((last_mouseover->state & SFreserved)==0)               
                   last_mouseover->setstate(SFmouseover,false);                 
               
               last_mouseover=last_mouseover->owner;
            }
            loopback=terminal_mouserecipient;
            //clear reserved flags
            while (loopback)
            {
               loopback->state&=~SFreserved;
               loopback=loopback->owner;
            }
            last_mouseover=terminal_mouserecipient;
          }
      }    
  }              
}

char buf[20];
void eventkey()
{
  tevent keyevent;
  tview *currview;
  if (kbhit())
  {
    charcode=getch();
    if (charcode==0) 
        scancode=getch();
    else
        scancode=0;
    itoa(scancode,buf,10);
    
    label(0,SC.size.y-20,buf);
    
    keyevent.what=EVkeydown;
    /*if (modal_view!=NULL)
        modal_view->handleevent(&keyevent);
    else */
    if (global_selected!=NULL)
    {
        currview=global_selected;
        while ((keyevent.what!=0)&&(currview!=NULL))
        {
            currview->handleevent(&keyevent);
            //if (keyevent.what==0)
            //  rectangle(currview->a.x,currview->a.y,currview->b.x,currview->b.y);
            currview=currview->owner;
        }
    }else
        root->handleevent(&keyevent);
  }
}


bool ismodal(tview *view)
{
    
    INT32 i=TWINmodalstackhead;
    while (i>0)    
        if (TWINmodalstack[i--]==view) return true;
    return false;        
}

void setmodal(tview *view)
{
    if (TWINmodalstackhead<TWINmodalstacklimit-1)
      TWINmodalstackhead++; 
    TWINmodalstack[TWINmodalstackhead]=view;        
    modal_view=view;
    view->state|=SFmodal;    
}

void endmodal()
{
    TWINmodalstackhead--;    
    if (TWINmodalstackhead<0)
      TWINmodalstackhead=0;
    modal_view->state &= (SFmodal ^ 0xff);
    modal_view=TWINmodalstack[TWINmodalstackhead];
}

void TWINendallmodals()
{
    while (TWINmodalstackhead>=0)
    {
        modal_view->state &= (SFmodal ^ 0xff);
        modal_view=TWINmodalstack[TWINmodalstackhead];
        TWINmodalstackhead--;    
    }
}
    
tbitmap *getresourceimage(char *imagename,UINT8 mode,UINT8 putmode=normalput,UINT32 flags=0)
{
    char dir[255];
    strcpy(dir,RESOURCE_PATH);
    strcat(dir,imagename);
    return readpcx(dir,mode,putmode,flags);    
}

UINT16 cx,cy;
tbitmap *base;
tbitmap *getimage(UINT16 w,UINT16 h,UINT8 drawmode)
{
    tbitmap *newbm=new tbitmap(cx,cy,w,h,base,drawmode);
    putbitmap(cx,cy,newbm);
    cx+=w;
    //getch();
    return newbm;
}

tbitmap *getnewimage(UINT16 w,UINT16 h,UINT8 drawmode)
{
    tbitmap *newbm=new tbitmap(cx,cy,w,h,base,drawmode,1);
    //putbitmap(cx,cy,newbm);
    cx+=w;
    //getch();
    return newbm;
}


void getwindowbitmaps(windowgfx *w)
{
    tbitmap **cbm=&w->br;
    UINT8 i;
    //gets corner graphics AND indent client edge graphics
    for (i=0;i<13;i++)
    {
        *cbm=getimage(w_corner.x,w_corner.y,chromaput);
        cbm++;
    }
    w->title[0]=getimage(w_titlebarl,w_corner.y,normalput);
    w->title[1]=getimage(w_titlebarm,w_corner.y,normalput);
    w->title[2]=getimage(w_titlebarr,w_corner.y,normalput);
    w->title[3]=getimage(w_titlefillerl,w_corner.y,normalput);
    w->title[4]=getimage(w_titlefillerm,w_corner.y,normalput);
    w->title[5]=getimage(w_titlefillerr,w_corner.y,normalput);
    cy+=w_corner.y;
    cx=0;        
    cbm=&w->l;
    //gets connector graphics for the outer and client edges.
    //for (i=0;i<8;i++)
    for (i=0;i<6;i++)
    {
        *cbm=getnewimage(w_cw,w_ch,normalput);
        cbm++;
    }
    
    w->l->rotate90();
    w->r->rotate90();
    w->il->rotate90();
    w->ir->rotate90();
    
    
}

void windowgfx::freeall()
{
    UINT8 i;
    tbitmap **cbm=&tl;
    
    for (i=0;i<28;i++)
    {
       delete *cbm;
       cbm++;
    }
       
}     


    

BOOL inittwin(char *rpath)
{
    int i;        
    strcpy(RESOURCE_PATH,rpath);
    TWINmodalstackhead=0;
    TWINmodalstack[0]=NULL;
   
    cx=0;cy=0;
    w_corner.assign(22,22);
    w_btn.assign(15,15);    
    w_titlebarl=4;
    w_titlebarm=44;
    w_titlebarr=4;
    w_titlefillerl=4;
    w_titlefillerm=44;
    w_titlefillerr=4;
    w_ch=4;
    w_cw=44;
    
    ns_btnh=10;
    ns_btnw=11; //number selector button width
    ns_spacerh=2; //number spacer/mouse scroller height
    drop_h=20;//drop bar height
    drop_btnw=16; //drop bar button width    
    
    sb_btn.assign(16,16);
    sb_basel=4;
    sb_basem=64;
    sb_baser=4;    
    sb_l=4;
    sb_m=12;
    sb_r=8;
    sb_shad=4;
    tab_l=tab_m=tab_r=12;
    tab_fill=4;
    tab_h=23;       
    
    t_nativepalette=windowcols;

    if (!(base=getresourceimage("gadgets.pcx",BMflat16,normalput,BMFdoublewordalign))) return FALSE;//BMFdontloadpalette|    
    
    memcpy(windowcolsrgb,getimage_rgbpalette(),256<<2);
    for (i=0;i<48;i++)    
        windowcols[i]=getcolour(windowcolsrgb[i].r,
                                windowcolsrgb[i].g,
                                windowcolsrgb[i].b);   
    
    base->palette=windowcols;
    getwindowbitmaps(&w_u);//unfocused window gadgets              
    //get the little corner sqaures        
    
    paper_gfx[0]=getimage(w_ch,w_ch,normalput);
    paper_gfx[1]=getimage(w_cw,w_ch,normalput);
    paper_gfx[2]=getimage(w_ch,w_ch,normalput);
    paper_gfx[3]=getimage(w_ch,w_ch,normalput);
    paper_gfx[4]=getimage(w_cw,w_ch,normalput);
    paper_gfx[5]=getimage(w_ch,w_ch,normalput);       
    paper_gfx[6]=new tbitmap(paper_gfx[1]);
    paper_gfx[6]->rotate90();
    paper_gfx[7]=new tbitmap(paper_gfx[4]);
    paper_gfx[7]->rotate90();
    ns_gfx[4]=getimage(ns_btnw,ns_spacerh,normalput);
    ns_gfx[5]=getimage(ns_btnw,ns_spacerh,normalput);    
    
    cy+=w_ch;
    cx=0;
    getwindowbitmaps(&w_s);//focused window gadgets
    cy+=w_ch;
    cx=0;       
    
    b_right0=getimage(sb_btn.x,sb_btn.y,normalput);
    b_right1=getimage(sb_btn.x,sb_btn.y,normalput);
    b_right2=getimage(sb_btn.x,sb_btn.y,normalput);
    b_down0=getimage(sb_btn.x,sb_btn.y,normalput);
    b_down1=getimage(sb_btn.x,sb_btn.y,normalput);
    b_down2=getimage(sb_btn.x,sb_btn.y,normalput);
    b_left0=getimage(sb_btn.x,sb_btn.y,normalput);
    b_left1=getimage(sb_btn.x,sb_btn.y,normalput);
    b_left2=getimage(sb_btn.x,sb_btn.y,normalput);
    b_up0=getimage(sb_btn.x,sb_btn.y,normalput);
    b_up1=getimage(sb_btn.x,sb_btn.y,normalput);
    b_up2=getimage(sb_btn.x,sb_btn.y,normalput);
    

    sb_basex[0]=getimage(sb_basel,sb_btn.y,normalput);
    sb_basex[1]=getimage(sb_basem,sb_btn.y,normalput);
    sb_basex[2]=getimage(sb_baser,sb_btn.y,normalput);
    sbx[0]=getimage(sb_l,sb_btn.y,normalput);
    sbx[1]=getimage(sb_m,sb_btn.y,normalput);
    sbx[2]=getimage(sb_r,sb_btn.y,normalput);
    
    sb_basey[0]=new tbitmap(sb_basex[0]);
    sb_basey[0]->rotate90();
    sb_basey[1]=new tbitmap(sb_basex[1]);
    sb_basey[1]->rotate90();
    sb_basey[2]=new tbitmap(sb_basex[2]);
    sb_basey[2]->rotate90();
    sby[0]=new tbitmap(sbx[0]);
    sby[0]->rotate90();
    sby[1]=new tbitmap(sbx[1]);
    sby[1]->rotate90();
    sby[2]=new tbitmap(sbx[2]);
    sby[2]->rotate90();

    //putbitmap(0,400,sby[0]);         
    
    //right is rotated 90 degrees and flipped        
        
    cx=0;
    cy+=sb_btn.y;    
    b_max0=getimage(w_btn.x,w_btn.y,chromaput);
    b_max1=getimage(w_btn.x,w_btn.y,chromaput);
    b_max2=getimage(w_btn.x,w_btn.y,chromaput);    
    b_close0=getimage(w_btn.x,w_btn.y,chromaput);
    b_close1=getimage(w_btn.x,w_btn.y,chromaput);
    b_close2=getimage(w_btn.x,w_btn.y,chromaput);
    b_min0=getimage(w_btn.x,w_btn.y,chromaput);
    b_min1=getimage(w_btn.x,w_btn.y,chromaput);
    b_min2=getimage(w_btn.x,w_btn.y,chromaput);
    cx=0;
    cy+=w_btn.y;
    tab_gfx[0]=getimage(tab_fill,tab_h,normalput);//selected
    tab_gfx[1]=getimage(tab_l,tab_h,normalput);//selected
    tab_gfx[2]=getimage(tab_m,tab_h,normalput);
    tab_gfx[3]=getimage(tab_r,tab_h,normalput);                
    tab_gfx[4]=getimage(tab_l,tab_h,normalput);//un selected
    tab_gfx[5]=getimage(tab_m,tab_h,normalput);
    tab_gfx[6]=getimage(tab_r,tab_h,normalput);
    tab_gfx[7]=getimage(4,tab_h,normalput);    //filler.. needs more work
    tab_gfx[8]=getimage(24,tab_h,normalput);
    tab_gfx[9]=getimage(4,tab_h,normalput);

    drop_gfx[0]=getimage(drop_btnw,drop_h,normalput);  //drop box graphics (just a button, uses paper gfx for main part)
    drop_gfx[1]=getimage(drop_btnw,drop_h,normalput);  //drop box graphics (just a button, uses paper gfx for main part)

    ns_gfx[0]=getimage(ns_btnw,ns_btnh,normalput);
    ns_gfx[1]=getimage(ns_btnw,ns_btnh,normalput);
    ns_gfx[2]=getimage(ns_btnw,ns_btnh,normalput);
    ns_gfx[3]=getimage(ns_btnw,ns_btnh,normalput);

    getch();        
    cy+=tab_h;
    cx=0;
    cy+=14; //add y offset for toggle buttons
    TWINBM_shadow[0]=getimage(16,10,subput);
    TWINBM_shadow[1]=getimage(16,10,subput);
    TWINBM_shadow[2]=getimage(6,10,subput);
    TWINBM_shadow[3]=getimage(10,10,subput);


    TWINBM_shadow[4]=new tbitmap(TWINBM_shadow[0]);
    TWINBM_shadow[4]->rotate90();
    TWINBM_shadow[5]=new tbitmap(TWINBM_shadow[1]);
    TWINBM_shadow[5]->rotate90();
    TWINBM_shadow[6]=new tbitmap(TWINBM_shadow[2]);
    TWINBM_shadow[6]->rotate90();            
           
    if (!(m_pointer=getresourceimage("mp1.pcx",SC.driver,chromaput))) return FALSE;
    if (!(m_waiting=getresourceimage("mp2.pcx",SC.driver,chromaput))) return FALSE; 
    if (!(b_toggle0=getresourceimage("toggle0.pcx",BMflat8,chromaput,BMFdontloadpalette|BMFdoublewordalign))) return FALSE;
    if (!(b_toggle1=getresourceimage("toggle1.pcx",BMflat8,chromaput,BMFdontloadpalette|BMFdoublewordalign))) return FALSE;
    b_toggle0->palette=windowcols;
    b_toggle1->palette=windowcols;    

        
    windowselectedcols=&windowcols[16];
    windowselectedcolsrgb=&windowcolsrgb[16];
    windowdarkcols=&windowcols[32];
    windowdarkcolsrgb=&windowcolsrgb[32];      
    if (!(fileicons=getresourceimage("fileicon.pcx",SC.driver,chromaput)))return FALSE;    
        
    MOUSEinit(m_pointer);
    MOUSEon();
    MOUSEsetbounds(0,0,SC.size.x-1,SC.size.y-1);
    MOUSEsetpos(0,0);
    rectbuffer.create(12*1000); //10k rect buffer
    
    selectcol=getcolour(255,0,0);
    //getch();
    twSTATE=  TSvisible;    
    return TRUE;
}

void setroot(tgroup *root_)
{
    root=root_;
    root->target=NULL;
    root->owner=NULL;
    
    global_selected=NULL;
    root->state|=SFvisible;
    root->select();
}

void closetwin()
{       
    delete root;
    rectbuffer.destroy();
    MOUSEoff();
    MOUSEshutdown();
    
    w_u.freeall();
    w_s.freeall();
    
    delete base;
    delete b_toggle0;delete b_toggle1;
    delete m_pointer;delete m_waiting;
    
    delete b_close0;delete b_close1;delete b_close2;
    delete b_min0;delete b_min1;delete b_min2;
    delete b_max0;delete b_max1;delete b_max2;
    delete b_up0;delete b_up1;delete b_up2;
    delete b_down0;delete b_down1;delete b_down2;
    delete b_right0;delete b_right1;delete b_right2;
    delete b_left0;delete b_left1;delete b_left2;
}


