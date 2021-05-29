//TWINf basic widgets code
#include "stdafx.h"
#include <string.h>
#include <stdlib.h>
#include <conio.h>
#include "views.hpp"
#include "ttypes.hpp"
#include "gbasics.hpp"
#include "msmouse.hpp"
#include "dosstuff.hpp"
#include "widgets.hpp"
#include "gprim.hpp"



char sliderblob[4]= {48,120,120,48};
char leftarrowb[8]= {8,24,56,120,56,24,8,0};
char rightarrowb[8]= {32,48,56,60,56,48,32,0};

void textboxb(INT16 x1,INT16 y1,INT16 x2,INT16 y2)
{
    captilebitmapx(x1,y1,x2+1,paper_gfx);
    captilebitmapx(x1,y2-3,x2+1,&paper_gfx[3]);
    tilebitmapy(x1,y1+3,y2-3,paper_gfx[6]);
    tilebitmapy(x2-3,y1+3,y2-3,paper_gfx[7]);
}


void textboxfilledb(INT16 x1,INT16 y1,INT16 x2,INT16 y2)
{
    captilebitmapx(x1,y1,x2+1,paper_gfx);
    captilebitmapx(x1,y2-3,x2+1,&paper_gfx[3]);
    tilebitmapy(x1,y1+3,y2-3,paper_gfx[6]);
    tilebitmapy(x2-3,y1+3,y2-3,paper_gfx[7]);
    t_fillcol=t_nativepalette[11];
    bar(x1+4,y1+4,x2-4,y2-4);
}
////////////////////////////////////////////////////////////////////////////////////////
// LIST BOX TYPE


UINT32 findlongesttext(listelembase *list,UINT32 nopts)
{
    UINT32 len=0;
    UINT32 clen;
    while (nopts>0)
    {
        clen=FNTgetwidth(list->txt);        
        if (clen>len) len=clen;
        list++;
        nopts--;
    }
    return len;
};

listbox::listbox(INT16 x,INT16 y,INT16 w,INT16 h,listelem *info_,INT16 maxopts_,char destroyold_)
:tview(x,y,x+w,y+h)
{
    options|= (OFautoraise+OFtabcycleselect+OFnooverlap);
    images=NULL;
    imgwidth=0;
    rightclickfunc=NULL;
    destroyold=destroyold_;    
    makelist(maxopts_,info_,1);
    drawmode=listdrawall;
}

listbox::~listbox()
{
    kill_list();
}

void listbox::setitem(INT32 opt,char *txt,INT16 offset,UINT32 attrib,UINT32 contextattrib,UINT8 img)
{
    if (opt<maxopts)
       info[opt].set(txt,offset,attrib,contextattrib,img);                    
}
 
void listbox::makelist(INT32 nopts,listelem *list,char drawit)
{
    maxopts=nopts;
    if (nopts==0)
    {
       list=NULL;
    }else
    {
      if (list!=NULL)
        info=list;
      else
      {
        info=new listelem[maxopts+1];
        //for (int i=0;i<maxopts;i++)
        //  setitem(i,"blank",0,0,0,0);
      }
    }
    //memset(info,sizeof(listelem)*(maxopts+1),0);
    changed=FALSE;
    sel=off=oldsel=0;
    calcinternalvars();
    if (drawit)
       drawview();             
}


void listbox::calcinternalvars()
{
    valuedata valdata;
    maxylen=calcylen();
    INT32 temp=maxopts-maxylen;
    if (temp<0) temp=0;
    valdata.min=0;valdata.max=temp;valdata.val=off;
    //this could cause too many redraws as changebounds uses it also and
    //it invokes the scrollbar's setrange procedure
    //this then forces a redraw; need to find some way around this that's not too messy
    sendcommand(CMDydeltachanged,owner,&valdata);
    
}

INT16 listbox::calcylen()
{
   return MIN(1+((b.y-a.y-8) / t_charset.ysize),maxopts);
}

void listbox::changebounds(trect &bounds)
{
    INT16 oldheight=height(); 
    tview::changebounds(bounds);    
    if (height()!=oldheight)
    {
       twSTATE&=~TSvisible;
       calcinternalvars();
       twSTATE|=TSvisible;     
    }
}

void listbox::setlist(INT32 nopts,listelem *list,char drawit)
{
    kill_list(FALSE);
    makelist(nopts,list,drawit);        
}

void listbox::kill_list(char drawit)
{
    if ((maxopts>0)&&(destroyold))
       delete info;
    maxopts=0;
    if (drawit)
       drawview();
}

void listbox::setval(INT32 val_)
{
    INT32 oldoff;
    if (val_<0) val_=0;
    if (val_>maxopts-1) val_=maxopts-1;
    if ((sel!=val_) || (!changed))
    {
        changed=TRUE;        
        
        oldoff=off;
        oldsel=sel;
        sel=val_;
        if (sel>off+(maxylen-1))
            off=sel-(maxylen-1);
        if (sel<off)
            off=sel;
        if (off<0) off=0;
        if (oldoff!=off)
        {
            drawmode=listdrawlist;            
            sendcommand(CMDydeltavalchanged,owner,&off);                                    
        }else        
            drawmode=listdrawchange;
        
        drawview();
        drawmode=listdrawall; 
        evcallback(CMDselectedchanged);
       
    }                       
}

void listbox::drawitem(INT32 itemno)
{
    INT32 yoff;
    INT32 xoff;
    if ((itemno>=off)&&(itemno<=maxopts))
    {

        yoff=a.y+4+(itemno-off)*t_charset.ysize;
        xoff=a.x+4;   
        t_fillcol=t_nativepalette[11];
        if (itemno==sel)
        {
            if ((state & SFselected)!=0)
              t_fillcol=t_nativepalette[15];           
            t_col=t_nativepalette[14];
        }else
           t_col=textcol;
        if (info[itemno].offset>0)
        {
            bar(xoff,yoff,xoff+info[itemno].offset-1,yoff+t_charset.ysize-1);
            xoff+=info[itemno].offset;
        }
        
        bar(xoff,yoff,b.x-4,yoff+t_charset.ysize-1);
        
        if ((info[itemno].image)&&(images))
        {
            UINT32 offset=((info[itemno].image-1)* imgwidth);
            trect oldvp=SC.viewport;    
            SC.viewport.rintersect(xoff,yoff,xoff+imgwidth-1,yoff+images->size.y-1);                 
            putbitmap(xoff-offset,yoff,images);
            SC.viewport=oldvp;   
            xoff+=imgwidth+4;
        }        
        outtextxy(xoff,yoff,info[itemno].txt);
    }
}
      
void listbox::handleevent(tevent *event)
{
    if (maxopts<=0) return;
    UINT32 oldoff,listi;
    tview::handleevent(event);
    if ((state & SFmodal)!=0)
    {
        setval(off+(mp.y-a.y-4) / t_charset.ysize);
        if (ms_released)
        {
            evcallback(CMDdragselectstopped);
            endmodal();
        }
    }else    
    {
        
        if ((event->what & EVmousepressed)!=0)
        {
            if ((mb==2) && (rightclickfunc))
              rightclickfunc(this,event);              
            else
            {
              setmodal(this);            
              if (ms_doubleclicked)            
                evcallback(CMDitemdclicked);              
            }
            event->what=0;                       
        }           
      if (event->what == EVcommand)
      switch (event->command)
      {
          case CMDyvalchanged:
              oldoff=off;
              off=*(UINT32*)event->infoptr;
              if (oldoff!=off)
              {
                  drawmode=listdrawlist;
                  drawview();
                  drawmode=listdrawall; 
              }
              break;                
      }  
   }
   if (event->what & EVkeyboard)
        {
           switch (scancode)
           {
               case upkey:setval(sel-1);event->what=0;break;
               case downkey:setval(sel+1);event->what=0;break;
               case pageupkey:setval(0);event->what=0;break;
               case pagedownkey:setval(maxopts);event->what=0;break;        
           }        
           switch (charcode)
           {
              case returnkey:              
                 evcallback(CMDitemdclicked);
                 event->what=0;
                 break;
              default:
                 if ((charcode>0x21))
                 {                     
                     if ((info[sel].txt[0]==charcode)&&(sel<maxopts-1))
                       if (info[sel+1].txt[0]==charcode)
                       {
                         setval(sel+1);
                         event->what=0;
                         return;
                       }                                          
                     listi=0;                     
                     while ((listi<maxopts)&&(info[listi++].txt[0]!=charcode));                                                 
                     if (listi<maxopts)
                         setval(listi);
                     event->what=0;
                 }              
           }
      } 
}


void listbox::draw()
{
    INT32 i;
    trect ir=trect(a.x+4,a.y+4,b.x-4,b.y-4);
    t_nativepalette=windowcols;
    if (drawmode==listdrawall)
    {        
        textboxb(a.x,a.y,b.x,b.y);                
        //thickrectangle(ir.a.x,ir.a.y,ir.b.x,ir.b.y,2);
    }
    
    trect oldvp=SC.viewport;    
    SC.viewport.rintersect(ir);
    t_fillcol=t_nativepalette[11];        
    if (maxopts==0)
    {
       t_col=textcol;       
       bar(ir.a.x,ir.a.y,ir.b.x,ir.b.y);              
       outtextxy(a.x+4,a.y+4,"this view is empty");
    }else 
    switch (drawmode)
    {
      case listdrawall:
      case listdrawlist:

          i=a.y+4+(maxylen*t_charset.ysize);
          if (i<ir.b.y)                        
              bar(ir.a.x,i,ir.b.x,ir.b.y);              
                          
          for (i=off;i<off+maxylen;i++)
              drawitem(i);
          
          
        break;
      case listdrawchange:
          drawitem(oldsel);      
      case listdrawselected:
          drawitem(sel);
      break;
    }
    SC.viewport=oldvp;        
}

char listbox::setstate(UINT16 astate, char enable)
{
    if (tview::setstate(astate,enable))
    {        
        if ((maxopts!=0)&&(astate & SFselected))
        {
            drawmode=listdrawselected;
            drawview();
            drawmode=listdrawall; 
        }        
        return 1;
    }else
        return 0;    
}

inline void swapb(INT8 &a, INT8 &b){INT32 t;t=a;a=b;b=t;}


bool listbox::find(UINT32 from,char *txt)
{
    for (UINT32 i=from;i<maxopts;i++)    
        if (!strcmp(info[i].txt,txt)) return true;
    
    return false;
}
void listbox::sortlist(UINT32 from,UINT32 to)
{

  if (from<maxopts)
  for (UINT32 i1=from;i1<maxopts;i1++)
    for (UINT32 i2=i1;i2<maxopts;i2++)
    {    
      if (strgreater(info[i2].txt,info[i1].txt)==0)
        fswapi(&info[i2],&info[i1],sizeof(listelem));
    }
 }






////////////////////////////////////////////////////////////////////////////////////////
// TREEBOX TYPE

/*TTinittree(ttree *tree,UINT32 size);
ttreenode *TTcreatettreenode(ttree *tree);
ttreenode *TTaddchildtonode(ttree *tree,ttreenode *parent,ttreenode *child);
ttreenode *TTgetprevnode(ttreenode *node); //get previous node on the level of 'node'
ttreenode *TTgetlastnode(ttreenode *node); //get last node entry on the level of 'node'
*/

//    void (*gettreeitemdata)(ttreenode *item,char **txt,tbitmap **pic,tcolour *col);
  //  ;

treebox::treebox(INT16 x,INT16 y,INT16 w,INT16 h,ttree *tree_,gettreeitemdatafunc gettreeitemdata_):tview(x,y,x+w,y+h)
{
    currsel=NULL;
    tree=tree_;
    gettreeitemdata=gettreeitemdata_;
    options|= OFautoraise+OFtabcycleselect+OFnooverlap;
    drawmode=listdrawall;
    height=t_charset.ysize;
    irect.rassign(a.x+4,a.y+4,b.x-4,b.y-4);
    off=0;
}

treebox::~treebox()
{
    TTdestroytree(tree->root);    
}

void treebox::changebounds(trect &bounds)
{
  INT16 oldheight=tview::height();
  tview::changebounds(bounds);  
  irect.rassign(a.x+4,a.y+4,b.x-4,b.y-4);

  if (tview::height()!=oldheight)
  {
      twSTATE&=~TSvisible;
      setbarrange();
      twSTATE|=TSvisible;
  }
}

UINT32 *treebox::getattrib(ttreenode *node)
{
    void *dummy;
    UINT32 *attrib;
    gettreeitemdata(node,(char **)&dummy,(tbitmap **)&dummy,&attrib);
    return attrib;
}


void treebox::setsel(ttreenode *newelem)
{
    if ((newelem!=currsel)&&(newelem!=NULL))
    {
        INT32 ypos=getypos(newelem);
        INT32 maxvisopts=irect.height()/height;
        INT32 oldoff=off;
        if (ypos<0)                
          off+=ypos;
        else if (ypos>maxvisopts)
          off+=(ypos-maxvisopts);
        if (off<0) off=0;
        
        oldsel=currsel;
        currsel=newelem;
        if (oldoff!=off)
        {
            drawmode=listdrawlist;
            sendcommand(CMDydeltavalchanged,owner,&off);            
        }else
            drawmode=listdrawchange;
        
        drawview();
        drawmode=listdrawall;
        evcallback(CMDselectedchanged);
    }
}

/*tree_elem *treebox::addelem(tree_elem *parent,tree_elem *newelem,char drawit)
{    
    (tree_elem*)TTaddchildtonode(&tree,(ttreenode*)parent,(ttreenode*)newelem);
    if (drawit)
    {        
        drawmode=listdrawlist;
        drawview();
        drawmode=listdrawall;
    }
    setbarrange();
    return newelem;
}

tree_elem *treebox::addelem(tree_elem *parent,char *txt,tbitmap *pic,UINT8 tattrib,void *data,char drawit)
{
    ref=new tree_elem(txt,pic,tattrib,data);    
    return addelem(parent,ref,drawit);
}


{
    TTdestroytree(node);
    if (node==(tree_elem*)tree.root)
      tree.root=NULL;
    if (drawit)
       drawview();
}*/

INT32 treebox::getypos(ttreenode *celem)
{
    INT32 ypos=-off;

    ttreenode *ccelem=tree->root;
    while ((ccelem!=celem))
    {
        if (ccelem==NULL) return -1;
        ccelem=getnextitem(ccelem);
        ypos++;     
    }   
    return ypos;
}


void treebox::update()
{
    calcylen();
    drawview();
}

INT32 treebox::calcylen()
{
    INT32 count=0;
    ttreenode *celem=tree->root;
    while (celem!=NULL)
    {        
        celem=getnextitem(celem);        
        count++;     
    }
    return count;
}

void treebox::setbarrange()
{        
    INT32 temp=calcylen()-(irect.height()/height);
        
    if (temp<0) temp=0;
    BOUND(off,0,temp);
    valuedata valdata;
    valdata.min=0;valdata.max=temp;valdata.val=off;
    sendcommand(CMDydeltachanged,owner,&valdata);
}

void treebox::drawitem(ttreenode *elem,INT32 ypos,bool justtext)
{
    if ((elem==NULL)) return;
    tbitmap *pic;char *txt;UINT32 *attrib;
    gettreeitemdata(elem,&txt,&pic,&attrib);
    t_fillcol=t_nativepalette[11];    
    t_col=textcol;
    INT16 hlf=height >> 1;
    INT16 textlen=FNTgetwidth(txt);
    INT16 xpos,i;
    INT16 depth=TTgetdepth(elem->parent);
    
    ttreenode *parent=elem->parent;

    xpos=irect.a.x+(depth << 4);  
    if (!justtext)
    {        
        if ( (currsel==elem)&&((state & SFselected)!=0) )
        {
            bar(irect.a.x,ypos,xpos+32,ypos+height-1);
            bar(xpos+textlen+34,ypos,irect.b.x,ypos+height-1);
            t_fillcol=t_nativepalette[15];            
            bar(xpos+33,ypos,xpos+textlen+33,ypos+height-1);        
        }else
            bar(irect.a.x,ypos,irect.b.x,ypos+height-1);
        for (i=0;i < depth;i++)
        {
            if (parent->next!=NULL)
                vline(xpos-8,ypos,ypos+height-1);
            xpos-=16;
            parent=parent->parent;
        }
    
        xpos=irect.a.x+(depth << 4);        
    
        if (elem->child==NULL)
        {
            if (elem!=tree->root)
                vline(xpos+8,ypos,ypos+hlf);
            hline(xpos+9,xpos+24,ypos+hlf);
            if (elem->next!=NULL)
                vline(xpos+8,ypos+hlf,ypos+height-1);
        }
        else        
        {
            if (elem!=tree->root)
                vline(xpos+8,ypos,ypos+hlf-squaresize);
            rectangle(xpos+8-squaresize,ypos+hlf-squaresize,xpos+8+squaresize,ypos+hlf+squaresize);
            hline(xpos+10-squaresize,xpos+6+squaresize,ypos+hlf);
            if ((*attrib & TEcollapsed)!=0)
            {
                vline(xpos+8,ypos+hlf-(squaresize-2),ypos+hlf+(squaresize-2));
                hline(xpos+hlf+squaresize,xpos+24,ypos+hlf);
            }
            else
            {
                vline(xpos+24,ypos+hlf,ypos+height-1);       //linking down to next
                hline(xpos+hlf+squaresize,xpos+23,ypos+hlf); //horizontal link
            }
            if (elem->next!=NULL)
                vline(xpos+8,ypos+hlf+squaresize,ypos+height-1);                        
        }
        if (pic!=NULL)                        
            putbitmap(xpos+16,ypos+hlf-8,pic);
    }else
    {
        if ( (currsel==elem)&&((state & SFselected)!=0) )        
            t_fillcol=t_nativepalette[15];
        bar(xpos+33,ypos,xpos+textlen+33,ypos+height-1);
    }
    xpos=irect.a.x+(depth << 4)+16;
    if (currsel==elem)
        t_col=t_nativepalette[14];        
    
    outtextxy(xpos+18,ypos+(hlf-(t_charset.yextsize >> 1)),txt);
}

void treebox::draw()
{
    INT16 ypos;
    ttreenode *celem;
    trect oldvp=SC.viewport;
    t_nativepalette=windowcols;        
    if (drawmode==listdrawall)
    {
        textboxb(a.x,a.y,b.x,b.y);
        t_col=t_nativepalette[11];
        //thickrectangle(a.x+2,a.y+2,b.x-2,b.y-2,2);        
    }
    
    SC.viewport.rintersect(irect);
    
    switch (drawmode)
    {
    case listdrawchange:               
        drawitem(oldsel,irect.a.y+getypos(oldsel)*height,TRUE);                
    case listdrawselected:        
        drawitem(currsel,irect.a.y+getypos(currsel)*height,TRUE);        
        break;
    case listdrawafter:               
    case listdrawall:
    case listdrawlist:
        if (drawmode==listdrawafter)
        {
            celem=ref;       
            ypos=irect.a.y+(getypos(ref)*height);
        }else
        {
            ypos=irect.a.y;
            celem=tree->root;    
            INT32 i;
            for (i=0;i<off;i++)
               celem=getnextitem(celem);
        }    
        while ((celem!=NULL)&&(ypos<irect.b.y))
        {
            drawitem(celem,ypos);
            celem=getnextitem(celem);
            ypos+=height;     
        }
        if (ypos<irect.b.y)
        {
            t_fillcol=t_nativepalette[11];    
            bar(irect.a.x,ypos,irect.b.x,irect.b.y);
        }       
        break;
    }
    SC.viewport=oldvp;
}

ttreenode *treebox::getprevitem(ttreenode *celem)
{
    ttreenode  *ccelem=tree->root;
    ttreenode  *retelem=NULL;
    while ((ccelem!=celem)&&(ccelem!=NULL))
    {
       retelem=ccelem;
       ccelem=getnextitem(ccelem);
    }
    return retelem;
}

ttreenode  *treebox::getnextitem(ttreenode  *celem)
{
    
    
    if ((celem->child!=NULL)&&((*getattrib(celem) & TEcollapsed)==0))        
        celem=celem->child;        
    else if (celem->next!=NULL)        
        celem=celem->next;        
    else if (celem->parent!=NULL)        
        celem=celem->parent->next;                     
    else        
        celem=NULL;
    return celem;
}            

ttreenode  *treebox::getitem(INT32 y)
{
    INT32 ypos=-off;
    ttreenode  *celem=tree->root;
    while (celem!=NULL)
    {
        if (y==ypos) return celem;
        celem=getnextitem(celem);        
        ypos++;     
    }
    return NULL;
}

         
void treebox::handleevent(tevent *event)
{
  INT32 yval;
  INT32 oldoff;
  tview::handleevent(event);
  ttreenode  *sel;
  if ((state & SFmodal)!=0)
  {
      yval=(mp.y-irect.a.y) / height;
      sel=getitem(yval);
      if (sel!=0)
      setsel(sel);
      if (ms_released)
          endmodal();
      event->what=0;
  }else
  if (tree->root!=NULL)
  {      
      if ((event->what & EVmousepressed)!=0)
      {          
            yval=(mp.y-irect.a.y) / height;
            sel=getitem(yval);
            if (sel!=NULL)
            {         
              if (sel->child!=NULL)
              {
                  //calculate the little rectangle for expanding, contracting and check it against the mouse position
                  INT16 depth=TTgetdepth(sel);
                  INT16 xpos=irect.a.x+((depth-1) << 4)+8;
                  INT16 ypos=irect.a.y+(yval*height)+(height >> 1);    
                  trect rect;               
                  rect.rassign(xpos-squaresize,ypos-squaresize,xpos+squaresize,ypos+squaresize);
                  if (rect.rcontains(mp.x,mp.y))
                  {
                       *getattrib(sel)^=TEcollapsed;
                       ref=sel;                                              
                       drawmode=listdrawafter;
                       drawview();
                       drawmode=listdrawall;                       
                       setbarrange();                       
                  }else
                     setsel(sel);
                  setmodal(this);
              }else
              {              
                  setsel(sel);
                  setmodal(this);
              }
              
                //setmodal(this);            
              if (ms_doubleclicked)            
                  evcallback(CMDitemdclicked);
              event->what=0;          
        }
     }else
     if (event->what == EVkeydown)
     {
         switch (scancode)
         {
             case upkey:setsel(getprevitem(currsel));event->what=0;break;
             case downkey:setsel(getnextitem(currsel));event->what=0;break;
             case pageupkey:setsel(tree->root);event->what=0;break;
             case pagedownkey:;break;        
         }        
         switch (charcode)
         {
            case returnkey:              
               evcallback(CMDitemdclicked);
               event->what=0;
               break;
         }
     }else if (event->what == EVcommand)
     switch (event->command)
     {
         case CMDyvalchanged:
             oldoff=off;
             off=*(INT32*)event->infoptr;
             if (oldoff!=off)
             {
                 drawmode=listdrawlist;
                 drawview();
                 drawmode=listdrawall; 
             }
             break;                
     }        
  }
  
}

char treebox::setstate(UINT16 astate, char enable)
{
    //check for difference
    if (tview::setstate(astate,enable))
    {
        if ((astate & SFselected)!=0)
        {
            drawmode=listdrawselected;
            drawview();
            drawmode=listdrawall;
        }
        return 1;
    }else
        return 0;
}





                 

 
tbutton::tbutton(INT16 x,INT16 y,INT16 w,INT16 h,
                 tcolour  *_palette,UINT8 _borderwidth,
                 UINT8 _action,UINT16 _docommand,
                 UINT8 hotkey,char dstate_,char *txt,tbitmap *bm_,tbitmap *bms_,tbitmap *bmd_)
:tview(x,y,x+w,y+h)
{
  options|=OFkeygrab;
  if (_borderwidth>0)
     options|=OFautoraise;
  hotkeycode=hotkey;
  palette=_palette;
  docommand=_docommand;
  action=_action;
  borderwidth=_borderwidth;
  bm=bm_;
  bmd=(bmd_)?bmd_:bm;
  bms=(bms_)?bms_:bm;
  dstate=dstate_;
  text=txt;
}

tbutton::tbutton(INT16 x,INT16 y,
                 UINT8 _borderwidth,
                 UINT8 _action,UINT16 _docommand,
                 UINT8 hotkey,tbitmap *bm_,tbitmap *bms_,tbitmap *bmd_):
                 tview(x,y,x+bm_->size.x,y+bm_->size.y)
{
  
  options|=OFkeygrab;
  if (_borderwidth>0)
     options|=OFautoraise;
  hotkeycode=hotkey;
  palette=windowcols;
  docommand=_docommand;
  action=_action;
  borderwidth=_borderwidth;
  bm=bm_;
  bmd=(bmd_)?bmd_:bm;
  bms=(bms_)?bms_:bm;
  dstate=BAnobox|BAnooffset;
  text=NULL;
}

 
#define threshold 1

void tbutton::handleevent(tevent *event)
{
  char inviewpc;
  static UINT16 mcount;

  tview::handleevent(event);

  if ((state & SFmodal)!=0)
  {
      inviewpc=inview(mp.x,mp.y);

      if (action==BMperiodaction)
      {
          if ((*timer-mcount)>threshold)
          {
             mcount=*timer;
             evcallback(docommand);
          }
      }
         
      if ((ms_released)||(!inviewpc))
      {
         
         if ((action!=BMtoggleaction)&&(action!=BMglobaltoggleaction))
         {
           if ((inviewpc)&&(action!=BMperiodaction))
               evcallback(docommand);
         
           dstate^= BAdown;
           drawview();
         }  
         endmodal();
      }
                            
  } else   
  if ((event->what & EVmousepressed)!=0)
  {
      if ((action==BMglobaltoggleaction)&&(dstate&BAdown))
      {
          event->what=0;
          return;
      }
      
        dstate^=BAdown;
        drawview();
      
        // outtextxy(200,200,itoa(rect_count,buf,10));
        switch (action) {
          case BMglobaltoggleaction:
          case BMtoggleaction:
            evcallback(docommand);
            setmodal(this);                
          break;
          case BMclickaction:
            evcallback(docommand);
            dstate^= BAdown;
            drawview();
          break;
          case BMreleaseaction:
            setmodal(this);          
            break;  
          case BMperiodaction:
            evcallback(docommand);          
            mcount=*timer+threshold;          
            setmodal(this);          
          
          break;
        
        } //} of switch
         
      event->what=0;
      
    } else if (event->what & (EVkeydown))
      if  ( ((state & SFselected)!=0) && (charcode==13) )
      {
        if (action==BMtoggleaction)
        {
          dstate^= BAdown;
          drawview();
        }
        evcallback(docommand);
        event->what=0;
      }  


  
}

void tbutton::draw()
{
  tpoint pos;
  UINT8 down=(dstate & BAdown);
  UINT8 downoffset=((dstate & BAnooffset)==0)?down:0;
  UINT8 sel=(state & SFselected)!=0;
  UINT8 nobox=(dstate & BAnobox)!=0;
  UINT8 xcentre=(dstate & BAxcentre)!=0;
  UINT8 ycentre=(dstate & BAycentre)!=0;
    
  if (borderwidth>0)
  {
    if (sel) 
      t_col=selectcol;
    else
      t_col=darkcol;
    thickrectangle(a.x,a.y,b.x,b.y,borderwidth);
  }
  
  if (((dstate & BAdrawonlyborder)==0)||(bms!=NULL))
  {
     if (!nobox)
     {
        t_nativepalette=palette;
        if (down)
           inboxcol(a.x+borderwidth,a.y+borderwidth,b.x-borderwidth,b.y-borderwidth);        
        else
           outboxcol(a.x+borderwidth,a.y+borderwidth,b.x-borderwidth,b.y-borderwidth);
     }
     t_col=textcol;
     
     UINT16 fntwidth=(text!=NULL)?FNTgetwidth(text)>>1:0;     
     UINT16 bmwidth=(bm!=NULL)?(bm->size.x >> 1):0;
     if ((fntwidth!=0)&&(bmwidth!=0))fntwidth++;

     pos.x=downoffset+borderwidth+a.x;
     if (xcentre)                           
         pos.x+=halfx()-(fntwidth+bmwidth);                                
                    
     if (bm!=NULL)
     {                      
          tbitmap *cbm;
          if (down)
              cbm=bmd;
          else if (sel)
              cbm=bms;
          else
              cbm=bm;
          pos.y=borderwidth+downoffset+a.y;
          if (ycentre)
              pos.y+=halfy()-(cbm->size.y>>1); 
                    
          putbitmap(pos.x,pos.y,cbm);
          pos.x+=bm->size.x+1;
      }
      
      if (text)
      {
          pos.y=downoffset+a.y;
          if (ycentre)
              pos.y+=halfy()-(t_charset.yextsize>>1);
          outtextxy(pos.x,pos.y,text);
      }
  }
}

char tbutton::setstate(UINT16 astate,char enable)
{
  if (tview::setstate(astate,enable))
  {
      if ((astate & SFselected) != 0)
      {
          if ((dstate & BAnobox)==0);
              dstate|=BAdrawonlyborder;
          drawview();
          dstate&=~BAdrawonlyborder;
      }
      return 1;
  }else
      return 0;   
}

////////////////////////////////////////////////////////////////////////////////////////
// BUTTON COLLECTION; used as radio buttons


tcollection::tcollection(INT16 x1,INT16 y1,INT16 x2,INT16 y2):tgroup(x1,y1,x2,y2)
{
    options |= OFnooverlap;
    lastselected=NULL;
}

tcollection::~tcollection()
{
  
}

void tcollection::handleevent(tevent *event)
{
    tgroup::handleevent(event);
    if (event->what==EVcommand)
    {
        if (event->infoptr!=lastselected)
        {
            if (lastselected!=NULL)
            {
                lastselected->dstate &= ~BAdown;
                lastselected->drawview();                
            }
            lastselected=(tbutton*)event->infoptr;                            
        }     
        evcallback(event->command);
    }
}

void tcollection::draw()
{
    t_fillcol=windowcols[8];
    bar(a.x,a.y,b.x,b.y);
    tgroup::draw();
}


////////////////////////////////////////////////////////////////////////////////////////
// VALUE TYPE

  
valuetype::valuetype(INT32 minval_,INT32 maxval_,INT32 val_):tgroup()
{    
    valuetype::setrange(minval_,maxval_,val_);
}    

bool valuetype::setval(INT32 val_)
{
    setrange(minval,maxval,val_);
    return val==val_;
}

bool valuetype::setvalmute(INT32 val_)
{
    state |= SFmute;
    setrange(minval,maxval,val_);
    state &= ~SFmute;
    return val==val_;
}

void valuetype::setrange(INT32 minval_,INT32 maxval_,INT32 val_)
{
    maxval=maxval_;
    minval=minval_;
    BOUND(val_,minval,maxval);
    val=val_;
}    



////////////////////////////////////////////////////////////////////////////////////////
// SLIDER TYPE

slidertype::slidertype(INT16 x,INT16 y,INT16 l,
                       INT32 minval_,INT32 maxval_,char attrib_,UINT32 updatecommand_,
                       INT32 val_):valuetype(minval_,maxval_,val_)
{
   attrib=attrib_; 
   options|=OFnooverlap+OFkeygrab+OFautoraise;
   updatecommand=updatecommand_;
   
   if ((attrib_ && SAxorient)!=0)
   {
       growmode=TGgrowallx+TGdragy1;            
       buttonsize=sb_btn.y;
       rassign(x,y,x+l,y+sb_btn.y-1);
       
       insert(new tbutton(a.x,a.y,0,BMperiodaction,CMDdecval,leftkey,b_left0,b_left1,b_left2));
       firstchild->growmode=TGdragx1|TGgrowally;       
       insert(new tbutton(b.x-sb_btn.x,y,0,BMperiodaction,CMDincval,rightkey,b_right0,b_right1,b_right2));
       firstchild->growmode=TGdragx2|TGgrowally;       
   }else
   {       
       growmode=TGgrowally+TGdragx1;
       buttonsize=sb_btn.y;
       rassign(x,y,x+sb_btn.x-1,y+l);
       
       insert(new tbutton(a.x,a.y,0,BMperiodaction,CMDdecval,leftkey,b_up0,b_up1,b_up2));
       firstchild->growmode=TGdragy1|TGgrowallx;
       insert(new tbutton(a.x,b.y-buttonsize,0,BMperiodaction,CMDincval,rightkey,b_down0,b_down1,b_down2));
       firstchild->growmode=TGdragy2|TGgrowallx;       
   }
   firstchild->options&=~(OFselectable);
   lastchild->options&=~(OFselectable);
   calcslidepos();
}


void slidertype::handleevent(tevent *event)
{
    INT32 diff;
    INT32 range;
    INT32 incby;
    trect srect;
            
    tpoint p;
    attrib&SAxorient?p.assign(a.x+buttonsize,b.x-buttonsize):p.assign(a.y+buttonsize,b.y-buttonsize);
        
    if ((state & SFmodal)!=0)
    {
        if (ms_released)
        {
            MOUSEsetbounds(0,0,SC.size.x-1,SC.size.y-1);
            attrib&=~SAmodalslide;
            endmodal();
            return;
        }
            
        if ((attrib & SAmodalslide)!=0) //we're on a modal slide
        {
            if (event->what & (EVmousemove))
            {
                diff=(p.y-p.x)-barminsize;                    
                if (diff==0)
                   diff=1;
                range=maxval-minval;
                if ((attrib & SAxorient)!=0)
                    incby=(mp.x-p.x)-offset;
                else
                    incby=(mp.y-p.x)-offset;
                if (range>=diff)
                    incby=(incby * range) /diff;
                incby+=minval;
                setval(incby);                
                
                event->what=0;
            }
        }
        else  //mouse not in slider, jump up or down
        {
            getsrect(srect);
            if ((mp.x>srect.b.x)||(mp.y>srect.b.y))    
                 setval(val+2);
            else if ((mp.x<srect.a.x)||(mp.y<srect.a.y))    
                 setval(val-2);
            else
                 event->what=0;
        }              
    }else
    {    
    tgroup::handleevent(event);
    if (event->what==0) return;
    if ((event->what & (EVmousepressed))!=0)
    {                   
        getsrect(srect);
        if (mb>0)
        {
            if (srect.rcontains(mp.x,mp.y))
            {
                attrib |= SAmodalslide;
                if ((attrib & SAxorient)!=0)
                {
                    offset=mp.x-srect.a.x;         
                    MOUSEsetbounds(p.x+offset,mp.y,p.y-(s2-s1-offset)+1,mp.y);              
                }
                else
                {
                    offset=mp.y-srect.a.y;         
                    MOUSEsetbounds(mp.x,p.x+offset,mp.x,p.y-(s2-s1-offset)+1);
              
                }                              
            }
            setmodal(this);
        }        
        event->what=0;      
   }else if ((event->what & EVcommand) != 0)
   {
      incby=1;
      switch (event->command)
      {
          case CMDincval:setval(val+incby);break;
          case CMDdecval:setval(val-incby);break;
      }
   }else if ((event->what==EVkeydown) && (scancode!=0))
   {
      event->what=0;
      if ((attrib & SAxorient)!=0)      
      switch (scancode)
      {
          case rightkeyctrl: 
          case endkey:      scancode=pagedownkey;
          case leftkeyctrl: scancode=pageupkey;break;
          case rightkey:    scancode=downkey;break;
          case leftkey:     scancode=upkey;break;
      }      
      switch (scancode)
      {
          case pageupkey:   setval(minval);break;
          case pagedownkey: setval(maxval);break;
          case upkey:       setval(val-1);break;
          case downkey:     setval(val+1);break;
          default:
          event->what=EVkeydown;
      }               
   }
   }
}

void slidertype::setrange(INT32 minval_,INT32 maxval_,INT32 val_)
{
    trect oldvp,tmp;
    INT32 oldval;
    char changed;

    oldval=val;
    if ((maxval_-minval_)<0)
    {
        minval_=maxval_=0;
    }
    changed=(minval_!=minval)||(maxval_!=maxval);
    valuetype::setrange(minval_,maxval_,val_);
    if ((val!=oldval)|| changed)
    {
        if ((state & SFvisible)!=0)
        {            
            oldvp=SC.viewport;
            getsrect(SC.viewport);            
            calcslidepos();
            getsrect(tmp);
            SC.viewport.runion(tmp);
            SC.viewport.rgrow(3,3);
            SC.viewport.rintersect(oldvp);
            attrib |=SAdontdrawbuttons;
            drawview();
            attrib &=~SAdontdrawbuttons;
            SC.viewport=oldvp;
        }else
            calcslidepos();
        // out message to parent}
        sendcommand(updatecommand,owner,&val);
    }
}


void slidertype::draw()
{
    
    if ((attrib & SAdontdrawbuttons)==0)
        tgroup::draw();
    char c=0;
    INT16 old2,old1,b1,b2;
    t_nativepalette=windowcols;
    t_nativebitmappalette=t_nativepalette;
    tpoint p;
    if ((attrib & SAxorient)!=0)
    {
        old1=SC.viewport.a.x;
        old2=SC.viewport.b.x;
        p.assign(a.x+buttonsize,b.x-buttonsize-1);
        b1=p.x+s1;
        b2=p.x+s2;
        if (old2>b1)
          SC.viewport.b.x=b1;        
        captilebitmapx(p.x,a.y,p.y,sb_basex);
        SC.viewport.b.x=(old2>p.y)?p.y:old2;
                  
        captilebitmapx(b1,a.y,b2+2,sbx);
        if (old1<b2)        
          SC.viewport.a.x=b2+2;          
        captilebitmapx(p.x,a.y,p.y+1,sb_basex);                  
        SC.viewport.b.x=old2;
        SC.viewport.a.x=old1;
        
    } else
    {
        
        old1=SC.viewport.a.y;
        old2=SC.viewport.b.y;
        p.assign(a.y+buttonsize,b.y-buttonsize-1);
        b1=p.x+s1;
        b2=p.x+s2;
        if (old2>b1)
          SC.viewport.b.y=b1;        
        captilebitmapy(a.x,p.x,p.y,sb_basey);
        SC.viewport.b.y=(old2>p.y)?p.y:old2;
                  
        captilebitmapy(a.x,b1,b2+2,sby);
        if (old1<b2)        
          SC.viewport.a.y=b2+2;          
        captilebitmapy(a.x,p.x,p.y+1,sb_basey);                  
        SC.viewport.b.y=old2;
        SC.viewport.a.y=old1;
    }
           
}

void slidertype::calcslidepos()
{
  INT32 diff,valdiff,size;
  UINT32 calc;

  //calculate the size of the bar in pixels...
  tpoint p;
  attrib&SAxorient?p.assign(a.x+buttonsize,b.x-buttonsize+1):p.assign(a.y+buttonsize,b.y-buttonsize+1);

  diff=(p.y-p.x);
  //calculate the range of the bar...      
  valdiff=maxval-minval;
  //check if we're smaller than the minimum size...  
  if (valdiff<(diff-barminsize)) 
      size=diff-valdiff;
  else
      size=barminsize;  //default to minimum size....
  if (valdiff!=0)
  {
      calc=val-minval;
      s1=(calc*(diff-size))  / valdiff;
  } else
  {
      s1=0;
      size=diff;
  }
  s2=s1+size;
}



void slidertype::changebounds(trect &bounds)
{
  tgroup::changebounds(bounds);
  calcslidepos();
}

void slidertype::getsrect(trect &srect)
{
  if ((attrib & SAxorient)!=0) 
      srect.rassign(a.x+buttonsize+s1,a.y,a.x+buttonsize+s2,b.y);
  else
      srect.rassign(a.x,a.y+buttonsize+s1,b.x,a.y+buttonsize+s2);
}


////////////////////////////////////////////////////////////////////////////////////////
// NUMBOX TYPE


numbox::numbox(INT16 x,INT16 y,INT16 w,UINT32 updatecommand_,INT32 minval_,INT32 maxval_,INT32 val_):valuetype(minval_,maxval_,val_)
{
   
   UINT16 hgt=ns_btnh+ns_btnh-1;
   updatecommand=updatecommand_;
   rassign(x,y,x+w,y+hgt);
   options|=OFnooverlap+OFkeygrab+OFautoraise;
   //create a textbox 
   insert(edit=new edittextbox(x,y,TBnumeric,NULL,255,w-ns_btnw,hgt));
   edit->setval(val);
   dmode=0;
}

numbox::~numbox()
{
}

void numbox::handleevent(tevent *event)
{
    static UINT16 mcount;
    if ((state & SFmodal)!=0)
    {
        if (ms_released)
        {
            dmode=0;
            drawview();
            endmodal();
            evcallback(CMDnumboxreleased);
            return;
        }
        if (dmode!=NSsliding)
        {
           if (abs(mdiff.y)>3)
           {
               dmode=NSsliding;
               drawview();
           }
        }
        if (dmode==NSdown) //we're on a modal slide
        {
            if ((*timer-mcount)>threshold)
            {
              mcount=*timer;
              setval(val-1);              
            }
        }  
        else if (dmode==NSup)
        {
            if ((*timer-mcount)>threshold)
            {
              mcount=*timer;
              setval(val+1);
            }
        }else
        {
            setval(val-mdiff.y);
        }                        
        
           
        
                     
    }else
    {    
    tgroup::handleevent(event);
    if (event->what==0) return;    
    if ((event->what & (EVmousepressed))!=0)
    {                   
        if (mb>0)
        {
            mcount=*timer+threshold;
            if (mp.x>b.x-ns_btnw)
            {
                if (mp.y<(a.y+ns_btnw-1))
                {
                  setval(val+1);
                  
                  dmode=NSup;
                }else if (mp.y>(1+b.y-ns_btnw))
                {                  
                  setval(val-1);
                  
                  dmode=NSdown;                  
                }else                             
                  dmode=NSsliding;
                drawview();
            }
            
            setmodal(this);
        }        
        event->what=0;      
   }else if (event->what == EVkeydown)
   {
       event->what=0;
       switch (scancode)
       {
           case upkey:setval(val-1);break;
           case downkey:setval(val+1);break;
           case pageupkey:setval(minval);break;
           case pagedownkey:setval(maxval);break;        
       }                 
   }else if ((event->what & EVcommand) != 0)
   {
       switch (event->command)
       {
       case CMDeditstringaltered:
         UINT32 oldval=edit->getvalint();
         setval(oldval);         
         break;
       }
   }
   }
   
}

void numbox::draw()
{
    edit->drawview();
    UINT16 ypos=a.y;
    putbitmap(b.x-(ns_btnw-1),ypos,ns_gfx[2+((dmode & NSup)!=0)]);ypos+=ns_btnh;    
    //putbitmap(b.x-(ns_btnw-1),ypos,ns_gfx[4+((dmode & NSsliding)!=0)]);ypos+=ns_spacerh;        
    putbitmap(b.x-(ns_btnw-1),ypos,ns_gfx[0+((dmode & NSdown)!=0)]);

    //draw the buttons
    //
}


void numbox::setrange(INT32 minval_,INT32 maxval_,INT32 val_)
{
     
    
    UINT32 oldval=val;  
    valuetype::setrange(minval_,maxval_,val_);    
    if (val!=oldval)
    {
      sendcommand(updatecommand,owner,&val);
      edit->setval(val);
    }
}
    

////////////////////////////////////////////////////////////////////////////////////////
// EDITTEXT TYPE

edittextbox::edittextbox(INT16 x,INT16 y,UINT8 attr,char *txt_,INT32 slen_,INT16 w,INT16 h):
                           tview(x,y,x+w,y+h)
{
  tbattrib=attr;
  strmem=slen_;
  xlen=w;
  txt=(char*)malloc(strmem);
  insertmode=1;
  memset(txt,0,strmem+1);
  if (txt_)
  {
      slen=MIN(strlen(txt_),strmem);
      strcpy(txt,txt_);
  } else
      slen=0;
    
  vistext=txt;
  curspos=curslen=c1=c2=cp1=cp2=off=0;
  lastchar=FNTgetindexfrompos(vistext,xlen-ETeditindex_x2);    //Get the closest character index to 'xlen'  
  options|=OFautoraise+OFtabcycleselect;
  
}

edittextbox::edittextbox(INT16 x,INT16 y,UINT8 attr,char *txt_,INT32 slen_,INT16 w)
{
    edittextbox(x,y,attr,txt_,slen_,w,t_charset.ysize+ETeditindent_y);    
}

edittextbox::~edittextbox()
{
    free(txt);
}

void edittextbox::settext(char *txt_,bool selectall)
{
  if (strcmp(txt_,txt))
  {    
      slen=MIN(strlen(txt_),strmem);
      if (selectall)
      {
        curspos=slen;
        curslen=0;
      }else
      {
        if (curspos>slen)  curspos=slen;
        curslen=curspos;
      }
      memcpy(txt,txt_,slen);
      txt[slen]=0;
      calcinternalvars();
      drawview();
  }  
}

void edittextbox::selectall()
{
    curspos=slen;
    curslen=0;
    calcinternalvars();
    drawview();
}

void edittextbox::calcinternalvars()
{
    vistext=&txt[off];
    lastchar=MIN(slen,FNTgetindexfrompos(vistext,xlen-ETeditindex_x2));    //Get the closest character index to 'xlen'    

    c1=MIN(curspos,curslen)-off;
    c2=MAX(curspos,curslen)-off;
    BOUND(c1,0,lastchar);  //'clip' c1 and c2 to the visible region of characters
    BOUND(c2,0,lastchar);     
    lastchar--;           
    cp1=FNTgetwidthlength(vistext,c1);  //Get the pixel x position of 'curslen'
    cp2=FNTgetwidthlength(vistext,c2);  //Get the pixel x position of 'curslen'
    //txt[slen+1]=0;
}    

char edittextbox::setcursorpos(INT32 pos,char followtail)
{
    INT32 oldoff,newcurslen;  
    //trect oldvp;
    BOUND(pos,0,slen);
    if (!followtail)
       newcurslen=pos;
    else
       newcurslen=curslen;
    BOUND(newcurslen,0,slen);   
  
    if ((pos!=curspos)||(newcurslen!=curslen))
    {
        curspos=pos;
        curslen=newcurslen;         
        oldoff=off;                                          //store old offset
        //do a limited calculation of internal variables so that we can check to see if the cursor outside of the view
        vistext=&txt[off];
        cp1=FNTgetwidthlength(vistext,curspos-off);  //Get the pixel x position of 'curpos'    
        lastchar=FNTgetindexfrompos(vistext,xlen-ETeditindex_x2);    //Get the closest character index to 'xlen'           
        if (curspos<off)                                         //Is the cursor to the left of the textboxb? 
            off=curspos;                                          // recaluate the character offset so that is just inside to the left
        else if ((cp1>=(xlen-ETeditindex_x2)))                                       //Is the cursor to the right of the textboxb?      
            off=curspos-lastchar;                                      //If so  recaluate the char offset so that it is just inside to right
            
        calcinternalvars();       
        if (oldoff!=off)                                    //If the offset has changed  we need to redraw everthing
        {
            drawview();
        }else
        {          
            drawview();                                       
        }
        return 1;
    }
        
    return 0;
}
/*

void edittextype::update;
{
static lasttime:word=0;
char onoff=0;

char oldmode;

  if (*timmer-lasttime>100)
  {
    onoff=onoff xor true;
    oldmode=drawmode;
    drawmode=

  }
}
*/

void edittextbox::draw()
{
  textboxb(a.x,a.y,b.x,b.y);    
  t_nativepalette=windowcols;
  t_fillcol=t_nativepalette[11];
  t_col=textcol;        
  
  INT16 dx=a.x+ETeditindent_x,dy=a.y+ETeditindent_y;    
  if (c1!=c2)
  {                
      if ((state & SFselected)!=0)
      {
          bar(a.x+4,a.y+4,dx+cp1,b.y-4);
          bar(dx+cp2,a.y+4,b.x-4,b.y-4);
          t_fillcol=t_nativepalette[15];
          bar(dx+cp1,a.y+4,dx+cp2,b.y-4);
          
      }else
          bar(a.x+4,a.y+4,b.x-4,b.y-4);

      INT32 templen;
      if (c1>0)
          outtextxy(dx,dy,vistext,c1);                        //draw the first portion of non-selected text
      templen=MIN(slen,lastchar)-c2;
      if(templen>=0)      
          outtextxy(dx+cp2,dy,&vistext[c2],templen);          //the trailing portion of non selected text
      
      if ((templen=MIN(c2-1,lastchar)-c1)>=0)
      {
          t_col=t_nativepalette[14];
          outtextxy(dx+cp1,dy,&vistext[c1],templen);          //draw the selected text.*/
      }    
  } else
  {
      bar(a.x+4,a.y+4,b.x-4,b.y-4);  
      outtextxy(dx,dy,vistext,lastchar);
  }              
  
  
  if ((state & SFselected)!=0)
  {
      t_col=textcol;
      int cpos;
      if (curspos>curslen)
        cpos=cp2;
      else
        cpos=cp1;  
      textcurs(dx+cpos,dy,height()-5);
  }
 
  /*diagnositcs
  SC.viewport=SC.screenport;

  char tmp[10];
  bar(10,10,50,80);

  outtextxy(10,10,itoa(cp1,tmp,10));
  outtextxy(10,20,itoa(cp2,tmp,10));
  outtextxy(10,30,itoa(c1,tmp,10));
  outtextxy(10,40,itoa(c2,tmp,10));
  outtextxy(10,50,itoa(off,tmp,10));
  outtextxy(10,60,itoa(lastchar,tmp,10));
  */
  
}

void edittextbox::handleevent(tevent *event)
{
    char followtail;
    char *vistext;
    tview::handleevent(event);

    if ((state & SFmodal)!=0)
    {
        vistext=txt;//&txt[off];  
        if (ms_released)
        {
            endmodal();
            return;
        }    
        if (mp.x<a.x+ETeditindent_x)
            setcursorpos(curspos-1,1);
        else if(mp.x>b.x-ETeditindent_x)
            setcursorpos(curspos+1,1);
        else          
            setcursorpos(off+FNTgetindexfrompos(vistext,mp.x-ETeditindent_x-a.x),1);
    }else    
    if ((event->what & EVmousepressed)!=0)
    {
        vistext=txt;//&txt[off];  
        setcursorpos(off+FNTgetindexfrompos(vistext,mp.x-ETeditindent_x-a.x),0);
        setmodal(this);
   
        event->what=0;
    }
  
  
    if (event->what==EVkeydown)
    {
      event->what=0;
      switch (charcode)
      {        
        case backspacekey:        
          if ((curspos==curslen)&&(curspos>0))
             deletetext(curspos-1,1);
          else          
             deletetext(MIN(curspos,curslen),abs(curslen-curspos));                       
        break;
        case escapekey:break;;
        case returnkey:
            evcallback(CMDeditreturnpressed);           
            break;
        default:
            //do checking in here for numeric or other specialized text cases.
            //if (curspos!=curslen)
            //   deletetext(MIN(curspos,curslen),abs(curslen-curspos));                       
            if ((tbattrib & TBnumeric)!=0)
            {
                if ((charcode>='0')&&(charcode<='9'))
                  inserttext(curspos,&charcode,1);
            }else if (charcode>30)        
                inserttext(curspos,&charcode,1);                          
        break;

      }
      followtail=(*scan & SMleftshiftkey)!=0;
      if (charcode==0)
      {
        event->what=0;
        switch (scancode)
        {
          case homekey:
          case leftkeyctrl:
                setcursorpos(0,followtail);break;
          case endkey:
          case rightkeyctrl:
                setcursorpos(slen,followtail);break;
          case rightkey:
                setcursorpos(curspos+1,followtail);break;
          case leftkey:
                setcursorpos(curspos-1,followtail);break;
          case delkey:
             if (curspos==curslen)
             {
                UINT8 oldslen=slen;
                deletetext(curspos,1);
                if (oldslen!=0)
                  drawview();
             }
             else 
                deletetext(MIN(curspos,curslen),abs(curslen-curspos));                                              
          default: break;//event->what=EVkeydown;
        }
      }
    }
}

/*
extern void *memchr( const void *__s, int __c, size_t __n );
extern int   memcmp( const void *__s1, const void *__s2, size_t __n );
extern void *memcpy( void *__s1, const void *__s2, size_t __n );
extern void *memmove( void *__s1, const void *__s2, size_t __n );
extern void *memset( void *__s, int __c, size_t __n );
extern char *strcat( char *__s1, const char *__s2 );
extern char *strchr( const char *__s, int __c );
extern int strcmp( const char *__s1, const char *__s2 );
extern int strcoll( const char *__s1, const char *__s2 );
extern size_t strxfrm( char *__s1, const char *__s2, size_t __n );
extern char *strcpy( char *__s1, const char *__s2 );
extern size_t strcspn( const char *__s1, const char *__s2 );
extern char *strerror( int __errnum );
extern size_t strlen( const char *__s );
extern char *strncat( char *__s1, const char *__s2, size_t __n );
extern int strncmp( const char *__s1, const char *__s2, size_t __n );
extern char *strncpy( char *__s1, const char *__s2, size_t __n );
extern char *strpbrk( const char *__s1, const char *__s2 );
extern char *strrchr( const char *__s, int __c );
extern size_t strspn( const char *__s1, const char *__s2 );
extern char *strstr( const char *__s1, const char *__s2 );
extern char *strtok( char *__s1, const char *__s2 );

*/
char edittextbox::setstate(UINT16 astate,char enable)
{
  if (tview::setstate(astate,enable))
  {
      if ((astate & SFselected) != 0 )
         drawview();
      return 1;
  }else
      return 0;    
}
    
void edittextbox::inserttext(INT32 pos,char *txt_,INT32 length)
{    
    length=MIN(length,pos+strmem);
    pos=MIN(pos,slen);
    if (insertmode==0)
    {
        memcpy(&txt[pos],txt_,length);        
    }else
    {
        INT32 length_to_end=MAX(0,slen-pos);
        char *charbuf=new char[length_to_end];
        
        memcpy(charbuf,&txt[pos],length_to_end);                
        memcpy(&txt[pos+length],charbuf,length_to_end);
        delete charbuf;
        memcpy(&txt[pos],txt_,length);      
    }
    //if (curspos==slen)
    //{    
    slen=MIN(slen+length,strmem);
    setcursorpos(pos+length,0);
    evcallback(CMDeditstringaltered);
    //} 
        
    /*memcpy(txt_,&newstr[pos],length);
    memcpy(&txt[pos],&newstr[pos+length],slen-pos);*/
    
    //slen+=length;       
}

bool edittextbox::setval(UINT32 val)
{
    //if ((tbattrib & TBnumeric)!=0) return FALSE;
    UINT32 oldval=getvalint();
    
    memset(txt,0,strmem);
    itoa(val,txt,10);
    vistext=txt; 
    slen=strlen(txt);
    calcinternalvars();    
    lastchar=slen-1;    
    if (oldval!=val)
      drawview();
    return TRUE;
}


UINT32 edittextbox::getvalint()
{
    return atoi(txt);
}


    
void edittextbox::deletetext(INT32 pos,INT32 length)
{
    //when you've done some sort of clipboard implement it in here
    if (pos+length>slen) return;
    length=MIN(length,pos+strmem);
    INT32 length_to_end=MAX(0,slen-pos);
    memcpy(&txt[pos],&txt[pos+length],length_to_end);
    slen-=length;
    if (slen<0) slen=0;
    txt[slen]=0;       
    setcursorpos(pos,0);
        //drawview();
    evcallback(CMDeditstringaltered);
}

////////////////////////////////////////////////////////////////////////////////////////
// TOOL BAR
// Easy to use toolbar; uses a single image for the buttons
// 


ttoolbutton::ttoolbutton(UINT32 at,UINT32 img,UINT32 cmnd)
{
    attribute=at;
    image=img;
    command=cmnd;
}

ttoolbar::ttoolbar(INT16 x,INT16 y,INT16 w,INT16 h,tbitmap *imgs,UINT8 numimgs,UINT16 btnx,UINT16 btny):
tview(x,y,x+w,y+h)
{
    numimages=numimgs;
    images=imgs;
    
    bsize.x=MAX(1,btnx);bsize.y=MAX(1,btny);
    lasttoggledown=modalbutton=NULL;
    buttons.init(sizeof(ttoolbutton));
    calculatedimensions();
}

ttoolbar::addbutton(UINT32 attribute,UINT32 image,UINT32 command)
{
    ttoolbutton *btn=new ttoolbutton(attribute,image,command);
    if ((attribute & (TBAglobaltoggle+TBAdown))==(TBAglobaltoggle+TBAdown))
    {
        if (lasttoggledown!=NULL)
           lasttoggledown->attribute &=~TBAdown;        
        lasttoggledown=btn;
    }
    
    buttons.push(btn);    
    calculatedimensions();
}


bool ttoolbar::switchbuttonbyref(UINT32 buttonref)
{
    return switchbutton((ttoolbutton*)buttons.findelement(buttonref));
}

bool ttoolbar::switchbutton(ttoolbutton *button)
{    
    if (button==NULL) return FALSE;    
    if (button->attribute & TBAglobaltoggle)
    {
        if (lasttoggledown!=NULL)
           lasttoggledown->attribute &=~TBAdown;
        button->attribute |=TBAdown;
        lasttoggledown=button;
        evcallback(button->command);
    }else if (button->attribute & TBAtoggle)
    {
       button->attribute^=TBAdown;
       evcallback(button->command);
    }else
    {
       button->attribute^=TBAdown;
       modalbutton=button;       
    } 

    return TRUE;
}

ttoolbutton *ttoolbar::getbuttonat(UINT16 x,UINT16 y)
{
    x-=a.x;y-=a.y;
    INT32 count=((x/bsize.x)+((y/bsize.y)*cols));
    telem *btn=buttons.first;
    while ((--count>=0)&&(btn!=NULL))
      btn=btn->next;
    return (ttoolbutton *) btn;               
}
    
void ttoolbar::calculatedimensions()
{
    
    cols=MIN(buttons.count,1+((b.x-a.x)/bsize.x));
    if (cols==0)
       rows=0;
    else
       rows=1+(buttons.count/cols);
    //cols=3;rows=3;
}

//
void ttoolbar::draw()
{
    INT16 currx,curry=a.y,ix,iy;
    trect oldvp=SC.viewport;
    ttoolbutton *btn=(ttoolbutton *)buttons.first;
    
    for (iy=0;iy<rows;iy++)
    {
        currx=a.x;
        for (ix=0;ix<cols;ix++)
        {
            if (btn)
            {
            UINT8 st=btn->attribute;
            //if ((btn->attribute & TBAseperator)==0)
            {
               SC.viewport=oldvp;
               SC.viewport.rintersect(currx,curry,currx+bsize.x-1,curry+bsize.y-1);
               UINT32 offset=((btn->image+((st & TBAdown)*(numimages))) * bsize.x);
               putbitmap(currx-offset,curry,images);//-
            }
            
            btn=(ttoolbutton*)btn->next;
            }
            currx+=bsize.x;            
        }
        curry+=bsize.y;
        
    }
    SC.viewport=oldvp;
}

void ttoolbar::handleevent(tevent *event)
{
  tview::handleevent(event);
  if ((state & SFmodal)!=0)
  {
      //inviewpc=inview(mp.x,mp.y);      
         
      if (ms_released)
      {
         if (modalbutton)
         {
             evcallback(modalbutton->command);
             modalbutton->attribute&=~TBAdown;
             modalbutton=NULL;
             drawview();
         }              
         endmodal();
      }
                            
  } else   

  if ((event->what & EVmousepressed)!=0)
  {
        if (switchbutton(getbuttonat(mp.x,mp.y)))
        {
        drawview();
        setmodal(this);
        }
        event->what=0;
  }
}

char ttoolbar::setstate(UINT16 astate,char enable)
{
  //return (tview::setstate(astate,enable));
  if (tview::setstate(astate,enable))
  {      
      /*if ((astate & SFfocused) != 0 )
      {
         drawmode=tabdrawborder;
         drawview();
         drawmode=tabdrawall;
      }*/
      return 1;
  }else
      return 0;
}



////////////////////////////////////////////////////////////////////////////////////////
// DROP BOX
// USE: this widget provides the functionality of a listbox but takes up much less space
//      with the use of shadowing (soon) it will also look pretty cool

dropbox::dropbox(INT16 x1,INT16 x2,INT16 y1,listelem *info_,INT16 maxopts_):tview(x1,y1,x2,y1+drop_h-1)
{
    growmode=TGdragx1|TGdragy1;
    drawmode=DBtopbit;
    options |= OFnooverlap;    
    list=new listbox(0,0,1,1,info_,maxopts_,FALSE);
    UINT32 numvals=maxopts_;
    BOUND(numvals,4,20);
    
    container=new scrollcontainer(x1,y1+drop_h,x2,y1+drop_h+4+(t_charset.ysize*numvals),list,CFybar+CFshadow);    
    container->growmode=TGdragonx2|TGdragony1;
    list->target=this;  
}



void dropbox::draw()
{
     textboxfilledb(a.x,a.y,b.x-drop_btnw,a.y+drop_h-1);                      
     t_col=textcol;
     outtextxy(a.x+4,a.y+3,list->info[list->sel].txt);
     putbitmap(b.x-drop_btnw+1,a.y,drop_gfx[(drawmode & DBlist)!=0]);
}

dropbox::~dropbox()
{    
    if ((drawmode & DBlist)==0)    
      container->remove();
    delete container;
}

void dropbox::toggledrop()
{    
    drawmode &=DBlist;
    drawmode^=DBlist;
    trect newbounds;
    if (drawmode & DBlist)
    {
        root->insert(container);        
        container->target=this;
        container->tmoveto(a.x,b.y+1);
        container->draw();
        container->state |=SFwaitingforrelease;
        list->select();
        setmodal(container);        
    }
    else
    {   
        container->remove();
        evcallback(CMDdropboxclosed);
    }
    drawview();
    drawmode|=DBtopbit;    
}


void dropbox::changebounds(trect &bounds)
{
    tview::changebounds(bounds);
    if ((drawmode & DBlist)!=0)
        container->tmoveto(a.x,b.y+1);               
}

void dropbox::handleevent(tevent *event)
{
    tview::handleevent(event);
    if (event->what&EVmousedown)
    {
        dropbox::toggledrop();
    }else
    if (event->what & EVcommand)
    {
        switch (event->command)
        {
        case CMDdragselectstopped:
            endmodal();
        case CMDcontainermodalended:        
            toggledrop();
            break;
        case CMDselectedchanged:            
            UINT8 olddrawmode=drawmode;
            drawmode=DBtext;
            drawview();
            drawmode=olddrawmode;
            evcallback(CMDselectedchanged);
            break;                   
        }
    }if (event->what & EVkeyboard)
    {
        list->handleevent(event);
    }
}

////////////////////////////////////////////////////////////////////////////////////////
// TAB BOX
// USE: 



tabbox::tabbox(INT16 x1,INT16 y1,INT16 x2,INT16 y2):tgroup(x1,y1,x2,y2)
{
    drawmode=tabdrawall;
    rootelem=NULL;
    seltab=NULL;
}

tabbox::~tabbox()
{
    SLdeletelist((telem*)rootelem);    
}

void tabbox::addtabcontrol(tview *view,char *name)
{
    seltab= new tabelement;
    strcpy(seltab->name,name);
    seltab->view=view;    
    SLaddtoend((telem**)&rootelem,(telem*)seltab);
    trect viewrect=trect(a.x+4,a.y+26,b.x-4,b.y-4);
    /*if ((createflags & CFclientedge)!=0)
    {
       viewrect.rgrow(-2,-2);
       options|=OFclientedge;
    }
    if (view)
    {*/
        insert(view);
        view->makeinvisible();
        view->growmode=TGgrowall;
        view->changebounds(viewrect);
        view->makevisible();
    //}
    
}


    
void tabbox::drawtab(tabelement *ctab,INT16 x1,INT16 x2)
{
    if (!ctab)return;
    UINT8 sel=(seltab==ctab);
    tbitmap **tabgfx=&tab_gfx[sel?1:4];        
    captilebitmapx(x1,a.y,x2,tabgfx);
    t_col=windowcols[8+(sel << 1)];
    //outtextxy(x1+13,a.y+3,"hi");//ctab->name
    outtextxy(x1+13,a.y+3,ctab->name);
}

void tabbox::draw()
{    
    tabelement *ctab=rootelem;        
    INT16 x1,x2,selx1,selx2;
    INT16 tx1,tx2,ty1,ty2;
    putbitmap(a.x,a.y,tab_gfx[0]);
    x1=a.x+tab_fill;
    t_nativepalette=t_nativebitmappalette=windowcols;
    while (ctab!=NULL)
    {        
        x2=x1+FNTgetwidth(ctab->name)+24;
        if ((drawmode!=tabdrawselected)||(ctab==seltab))
        drawtab(ctab,x1,x2);                        
        if (ctab==seltab)
        {
            selx1=x1;
            selx2=x2-2;
        }
        ctab=(tabelement*)ctab->next;
        x1=x2;
    }          
    //draw the 'dividing line in here
    putbitmap(1+b.x-w_corner.x,1+b.y-w_corner.y,w_u.br,1);
    putbitmap(a.x,1+b.y-w_corner.x,w_u.bl,1);
    //shadowy bit    
    ty1=a.y+22;ty2=b.y-22;
    tx1=a.x+22;tx2=b.x-22;
    if (x2<b.x)    
        captilebitmapx(x2,a.y,b.x+1,&tab_gfx[7]);
    tilebitmapx(tx1,1+b.y-w_ch,tx2,w_u.b,1);
    tilebitmapy(a.x,ty1,ty2,w_u.l,1);
    tilebitmapy(1+b.x-w_ch,ty1,ty2,w_u.r,1);
    
    t_fillcol=t_nativepalette[8];    
    bar(a.x+3,a.y+22,b.x-3,a.y+25);
    
          
    if (drawmode==tabdrawall)
    if (seltab!=NULL)
      seltab->view->drawview();
}

void tabbox::handleevent(tevent *event)
{    
    tview::handleevent(event);
    if (event->what&EVmousedown)
    {
        if (mp.y<a.y+20)
        {
            tabelement *ctab=rootelem;        
            INT16 x1,x2;
            x1=a.x;
            UINT8 found=0;
            while ((ctab!=NULL)&&(found==0))
            {                
                x2=x1+FNTgetwidth(ctab->name)+24;
                found=((mp.x>=x1)&&(mp.x<=x2));
                if (!found)
                ctab=(tabelement*)ctab->next;
                x1=x2;                
            }
            if ((ctab!=NULL)&&(ctab!=seltab))
            {               
               seltab=ctab;
               tview *cview=ctab->view;
               if (cview!=firstchild)
               {
               cview->prev()->next=cview->next;  
               cview->next=firstchild;
               firstchild=cview;
               drawview();
               evcallback(CMDtabchanged);
               }
            }
            event->what=0;
        }
    }
    
    if (seltab!=NULL)
        seltab->view->handleevent(event);
}

char tabbox::setstate(UINT16 astate,char enable)
{
  //return (tview::setstate(astate,enable));
  if (tview::setstate(astate,enable))
  {      
      /*if ((astate & SFfocused) != 0 )
      {
         drawmode=tabdrawborder;
         drawview();
         drawmode=tabdrawall;
      }*/
      return 1;
  }else
      return 0;
}





////////////////////////////////////////////////////////////////////////////////////////
// WINDOW
// USE: this widget is the container class for all things that move around on the desktop
//      It resizes and changes colour when focused. Various flags control it's starting state
//      including if it's got scrollbars or not and which buttons it has added: 
//      an optional menu across the top and flags for no resizing (for dialog boxes)



window::window(INT16 x,INT16 y,INT16 w,INT16 h,char *txt,tview *cview,UINT16 createflags):tgroup(x,y,x+w,y+h)
{
    text=txt;
    trect viewrect(x+4,y+22,w+x-4,h+y-4);    //needs to be using those magic numbers!
    drawmode=windowdrawall;
    options|=OFtopselect|OFnooverlap;
    
    if ((createflags & CFxbar)!=0)
    {
        viewrect.b.y-=(sb_btn.y-1);
        xbar=new slidertype(viewrect.a.x,viewrect.b.y,1+viewrect.width()-sb_btn.x,0,0,SAxorient,CMDxvalchanged,NULL);
        xbar->growmode=TGdragony2|TGgrowallx;
        viewrect.b.y--;
        insert(xbar);        
    }else
        xbar=NULL;
        
    if ((createflags & CFybar)!=0)
    {
        viewrect.b.x-=(sb_btn.x-1);
        ybar=new slidertype(viewrect.b.x,viewrect.a.y,1+viewrect.height(),0,0,0,CMDyvalchanged,NULL);
        ybar->growmode=TGdragonx2|TGgrowally;
        viewrect.b.x--;
        insert(ybar);
    }else
        ybar=NULL;
    close=new tbutton(a.x+5,a.y+4,0,BMreleaseaction,CMDclose,0,b_close0,b_close0,b_close2);
    close->growmode=TGdragonx1|TGdragony1|TGgrowx1|TGgrowy1;
    //close->options|=OFautoraise;

    min=new tbutton(b.x-19,a.y+4,0,BMreleaseaction,CMDminimizewindow,0,b_min0,b_min0,b_min2);
    min->growmode=TGdragonx2|TGdragony1|TGgrowx2|TGgrowy1;
    view=cview;
    if ((createflags & CFnoresize)!=0) options|=OFnoresize;
    if ((createflags & CFclientedge)!=0)
    {
       viewrect.rgrow(-2,-2);
       options|=OFclientedge;
    }
    
    if (view)
    {        
        cview->makeinvisible();
        cview->growmode=TGgrowall;
        cview->changebounds(viewrect);
        cview->makevisible();
        insert(cview);
    }
    insert(min);
    insert(close);
    //min->options|=OFautoraise;
}    
//windows draw their borders in a special way...
/*
void drawvedge(UINT16 x1,UINT16 y1,UINT16 y2,tcolour c1,tcolour c2,tcolour c3,tcolour c4)
{
    t_col=c1;vline(x1,y1,y2);
    t_col=c2;vline(x1+1,y1,y2);
    t_col=c3;vline(x1+2,y1,y2);
    t_col=c4;vline(x1+3,y1,y2);   
}

void drawhedge(UINT16 x1,UINT16 y1,UINT16 y2,tcolour c1,tcolour c2,tcolour c3,tcolour c4)
{
    t_col=c1;hline(x1,x2,y1);
    t_col=c2;hline(x1,x2,y1+1);
    t_col=c3;hline(x1,x2,y1+2);
    t_col=c4;hline(x1,x2,y1+3);   
}
*/

void drawborders(INT16 x1,INT16 y1,INT16 x2,INT16 y2,windowgfx *wg,char *text,UINT32 options,bool resizecorner)
{
    INT16 tx1,tx2,tx3;
    UINT16 ty1,ty2;
    UINT16 txtwidth=FNTgetwidth(text)+10;
    
    putbitmap(x1,y1,wg->lb,1);
    putbitmap(1+x2-w_corner.x,y1,wg->rb,1);
    
    //shadowy bit    
    ty1=y1+w_corner.y;ty2=y2-20;    
    //light bits
    tx1=x1+w_corner.x;tx2=tx1+txtwidth;
    tx3=x2-21;
    if (tx2>tx3) tx2=tx3;
      captilebitmapx(tx1,y1,tx2,&wg->title[3],0);
    if (tx2<tx3)    
      captilebitmapx(tx1+txtwidth,y1,tx3,wg->title,0);

    t_col=t_nativepalette[4];               
    INT16 oldx2=SC.viewport.b.x;
    tx2-=6;
    if (SC.viewport.b.x>tx2)
      SC.viewport.b.x=tx2;    
    outtextxy(tx1+5,y1+5,text);    
    SC.viewport.b.x=oldx2;    
    putbitmap(1+x2-w_corner.x,1+y2-w_corner.y,resizecorner?wg->br:wg->br0,1);
    putbitmap(x1,1+y2-w_corner.y,wg->bl,1);
    tilebitmapx(tx1,1+y2-w_ch,tx3,wg->b,1);
    tilebitmapy(x1,ty1,ty2,wg->l,1);
    tilebitmapy(1+x2-w_ch,ty1,ty2,wg->r,1);        
}

void drawclientedge(INT16 x1,INT16 y1,INT16 x2,INT16 y2,windowgfx *wg)
{
    /*x1-=4;
    y1-=4;
    x2+=4;
    y2+=4;
    
    putbitmap(x1,y1,wg->itl,1);
    putbitmap((x2-w_corner.x),y1,wg->itr,1);
    putbitmap(x1,y2-w_corner.x,wg->ibl,1);
    putbitmap((x2-w_corner.x),y1,wg->itr,1);
    putbitmap((x2-w_corner.x),y2-w_corner.x,wg->ibr,1);
        
    
    tilebitmapx(x1+w_corner.x,y1,(x2-w_corner.y),wg->it,1);
    tilebitmapx(x1+w_corner.x,y2-w_corner.x,(x2-w_corner.y),wg->ib,1);
    tilebitmapy(x1,y1+w_corner.x,(y2-w_corner.y),wg->il,1);
    tilebitmapy(x2,y1+w_corner.x,(y2-w_corner.y),wg->il,1);*/
    
    t_col=t_nativepalette[1];    
    hline(x1-1,x2+1,y1-1);
    vline(x1-1,y1-1,y2+1);
    t_col=t_nativepalette[6];
    hline(x1-2,x2+2,y1-2);
    vline(x1-2,y1-2,y2+2);
    t_col=t_nativepalette[9];    
    hline(x1-1,x2+1,y2+1);
    vline(x2+1,y1-1,y2+1);
    t_col=t_nativepalette[11];
    hline(x1-2,x2+2,y2+2);
    vline(x2+2,y1-2,y2+2);
}

void drawouteredge(INT16 x1,INT16 y1,INT16 x2,INT16 y2,windowgfx *wg)
{
    t_col=t_nativepalette[11];    
    hline(x1-1,x2+1,y1-1);
    vline(x1-1,y1-1,y2+1);
    t_col=t_nativepalette[9];
    hline(x1-2,x2+2,y1-2);
    vline(x1-2,y1-2,y2+2);
    t_col=t_nativepalette[6];    
    hline(x1-1,x2+1,y2+1);
    vline(x2+1,y1-1,y2+1);
    t_col=t_nativepalette[1];
    hline(x1-2,x2+2,y2+2);
    vline(x2+2,y1-2,y2+2);
}


void window::draw()
{
    windowgfx *wg;   
    if (state & (SFfocused+SFmouseover))
    {
      t_nativepalette=windowcols;
      wg=&w_s;
    }else
    {
      t_nativepalette=windowcols;
      wg=&w_u;
    }
    t_nativebitmappalette=t_nativepalette;
    drawborders(a.x,a.y,b.x,b.y,wg,text,options,((xbar==NULL)||(ybar==NULL)));
    if ((options & OFclientedge)!=0)
      drawclientedge(view->a.x,view->a.y,view->b.x,view->b.y,wg);
      
    if (drawmode==windowdrawall)
      tgroup::draw();
}

char window::setstate(UINT16 astate,char enable)
{
  //return (tview::setstate(astate,enable));
  bool ison=((state & (SFfocused+SFmouseover))!=0);
  if (tview::setstate(astate,enable))
  {
      
      if (ison!=((state & (SFfocused+SFmouseover))!=0)) 
      {
         drawmode=windowdrawborder;
         drawview();
         drawmode=windowdrawall;
      }
      return 1;
  }else
      return 0;
}



void window::handleevent(tevent *event)
{
#define MINWINX 100
#define MINWINY 100

    if ( ((state & SFmodal)!=0)&&((event->what & EVmouse)!=0)  )
    {
        
        if (resizemode==TGgrowall)        
            tmove(mdiff.x,mdiff.y);
        else
        {
        trect tmp_loader;
        getbounds(&tmp_loader);
        if ((resizemode&TGgrowx1)!=0)
        {
          tmp_loader.a.x+=mdiff.x;
          if (tmp_loader.width()<MINWINY) tmp_loader.a.x=tmp_loader.b.x-MINWINX;
        }
        if ((resizemode&TGgrowx2)!=0)
        {
          tmp_loader.b.x+=mdiff.x;          
          if (tmp_loader.width()<MINWINX) tmp_loader.b.x=tmp_loader.a.x+MINWINX;
        }
        if ((resizemode&TGgrowy1)!=0)
        {
          tmp_loader.a.y+=mdiff.y;
          if (tmp_loader.height()<MINWINY) tmp_loader.a.y=tmp_loader.b.y-MINWINY;
        }
        if ((resizemode&TGgrowy2)!=0)
        {
          tmp_loader.b.y+=mdiff.y;
          if (tmp_loader.height()<MINWINY) tmp_loader.b.y=tmp_loader.a.y+MINWINY;
        }
        reposview(tmp_loader);
        }
        if (ms_released)        
            endmodal();           
        
    }else
    {

    tgroup::handleevent(event);
    if ((event->what&EVmousedown)!=0)
    {
        if ((options & OFnoresize)!=0)
           resizemode=TGgrowall;
        else
        if ((mp.x>b.x-21)&&(mp.y>b.y-21))
        {           
           resizemode=TGgrowx2|TGgrowy2;
        }else if (mp.x>b.x-4)
        {
            resizemode=TGgrowx2;
            if (mp.y<a.y+21)
              resizemode|=TGgrowy1;                                    
        }else if (mp.x<a.x+4)
        {
            resizemode=TGgrowx1;
            if (mp.y<a.y+21)
              resizemode|=TGgrowy1;
            if (mp.y>b.y-21)
              resizemode|=TGgrowy2;              
        }else if (mp.y>b.y-4)
        {
            resizemode=TGgrowy2;
            if (mp.x<a.y+21)
              resizemode|=TGgrowx1;                                    
        }else if (mp.y<a.y+4)
        {
            resizemode=TGgrowy1;
            if (mp.x<a.x+21)
              resizemode|=TGgrowx1;
            if (mp.x>b.x-21)
              resizemode|=TGgrowx2;              
        }else                
            resizemode=TGgrowall;                   
        setmodal(this);
        event->what=0;
    }else if(event->what==EVcommand)
    {
        valuedata *data=(valuedata*)event->infoptr;
        event->what=0;
        switch(event->command)
        {
            case CMDxvalchanged:sendcommand(CMDxvalchanged,view,data);break;
            case CMDyvalchanged:sendcommand(CMDyvalchanged,view,data);break;
            case CMDxdeltavalchanged:if (xbar!=NULL){xbar->mute();xbar->setval(data->val);xbar->unmute();};break;
            case CMDydeltavalchanged:if (ybar!=NULL){ybar->mute();ybar->setval(data->val);ybar->unmute();};break;
            case CMDxdeltachanged:if (xbar!=NULL){xbar->mute();xbar->setrange(data->min,data->max,data->val);xbar->unmute();};break;
            case CMDydeltachanged:if (ybar!=NULL){ybar->mute();ybar->setrange(data->min,data->max,data->val);ybar->unmute();};break;
            default:
                 event->what=EVcommand;
                 sendcommand(event->command,owner,data);
        }       
    }
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////
// MENU

tmenu::tmenu(INT16 x,INT16 y,UINT32 attrib_,tbitmap *sideimg):tview(x,y,x+7,y+7)
{    
    options|=OFselectable;
    mattrib=attrib_;
    sideimage=sideimg;
    sel=oldsel=NULL;
    parentmenu=NULL;
    data.init(sizeof(tmenuitem));
    drawmode=menudrawall;
    if ((mattrib & MAxorient)==0)    
      b.x+=100;            
    else
      b.y+=20;
    if (mattrib & MApopup)
    {
        b+=16;
    }
      
}

tmenu::~tmenu()
{
    data.destroy(NULL);
}

tmenu *tmenu::findroot()
{
    tmenu *parent=parentmenu;
    if (!parent) return NULL;
    while (parent->parentmenu!=NULL)        
        parent=parent->parentmenu;
    return parent;      
}

tmenuitem *tmenu::additem(char *text,UINT32 command,UINT32 iattrib,tmenu *submenu,tbitmap *bm,UINT8 shortkey_)
{
    tmenuitem *newitem = new tmenuitem;
    tsize size;
    if (!newitem) return NULL;
    strcpy(newitem->text,text);
    newitem->command=command;
    newitem->attrib=iattrib;
    newitem->bitmap=bm;
    newitem->shortkey=shortkey_;
    newitem->submenu=submenu;
    
    
    if (submenu!=NULL)
      submenu->parentmenu=this;
    
    data.push((telem*)newitem);

    if ((newitem->attrib & MIAseperator)!=0)
       size.assign(8,4);
    else if (newitem->bitmap==NULL)
       size.assign(8+FNTgetwidth(newitem->text),4+t_charset.ysize);
    else    
       size.assign(10+newitem->bitmap->size.x+FNTgetwidth(newitem->text),4+MAX(newitem->bitmap->size.y,t_charset.ysize));
    newitem->size=size;
    if ((mattrib & MAxorient)!=0)
    {
        b.x+=size.x+1;
        if (height()<size.y)
          b.y=a.y+size.y;
    }else
    {
        b.y+=size.y+1;
        if (width()<size.x)
          b.x=a.x+size.x;
    }
    return newitem;
}

void tmenu::drawitem(INT16 xpos,INT16 ypos,INT16 w,INT16 h,tmenuitem *item)
{
    if (!item) return;
    UINT32 iattrib=item->attrib;
    t_nativepalette=windowcols;
    tbitmap *bitmap=item->bitmap;
    UINT8 selected=((sel==item));//&&((state & SFfocused)!=0)
    if ((iattrib & MIA3dlook)!=0)
    {       
       if (selected)
       {          
          inboxcol(xpos,ypos,xpos+w,ypos+h);
          xpos++;ypos++;
       }else
          outboxcol(xpos,ypos,xpos+w,ypos+h);
    }
    else
    {
       if (selected)
       {
          t_fillcol=t_nativepalette[15];
          t_col=t_nativepalette[14];
       }
       else
       {
          t_fillcol=t_nativepalette[8];
          t_col=t_nativepalette[1];
       }
                 
       bar(xpos,ypos,xpos+w,ypos+h);
    }
    h>>=1;
    INT16 tempy=ypos+ (h-(t_charset.yextsize >> 1));
    if ((iattrib & MIAseperator)!=0)
    {
        t_col=t_nativepalette[9];
        hline(xpos+5,xpos+w-7,ypos+h+1);
        t_col=t_nativepalette[2];
        hline(xpos+4,xpos+w-8,ypos+h);
        return;        
    }
    UINT16 x2=xpos+w;
    if (bitmap!=NULL)
    {
        putbitmap(xpos+4,ypos+(h-(bitmap->size.y>>1)),bitmap);
        xpos+=bitmap->size.x;
    }
    
    if ((iattrib & MIAdisabled)!=0)
    {
        t_col=t_nativepalette[9];
        t_fillcol=t_nativepalette[2];
       outtextxyu3d(xpos+4,tempy+1,item->text,item->shortkey);
    }else
    {
       outtextxyu(xpos+4,tempy+1,item->text,item->shortkey);
       //outtextxy(xpos+4,tempy+1,item->text);
    }
    /*
    if ((item->submenu)&&((mattrib & MAxorient)==0))
      outtextxy(x2-8,tempy,">",1);
    */
}

tpoint tmenu::getitempos(tmenuitem *item)
{
    tpoint retval(0,0);
    tmenuitem *citem=(tmenuitem*)data.first;
    while (citem!=NULL)
    {
       if (citem==item)
          return retval;
        retval.x+=citem->size.x;
        retval.y+=citem->size.y;
        
        citem=(tmenuitem *)citem->next;
    }
    return retval;
}

tmenuitem *tmenu::getitematpos(tpoint p)
{
    INT16 pos=4;
    p-=a;
    tmenuitem *item=(tmenuitem *)data.first;
    while (item!=NULL)
    {
       if ((mattrib & MAxorient)!=0)
       {
           pos+=item->size.x+1;
           if (p.x<=pos) return item;                                
       }else
       {
           pos+=item->size.y+1;
           if (p.y<=pos) return item;                                
       }
       item=(tmenuitem *)item->next;
    }
    return NULL;
        
}

void tmenu::draw()
{
    tmenuitem *item=(tmenuitem*)data.first;
    INT16 xpos=a.x+4,ypos=a.y+4;
    INT16 w=width()-7,h=height()-7,temp;
    if (mattrib & MApopup)
    {
      w-=16;
      h-=16;
    }
    t_nativepalette=windowcols;
    if (drawmode==menudrawall)       
        drawborder(a.x,a.y,a.x+w+7,a.y+h+7,4);        
    //draw border
    while (item!=NULL)
    {
        UINT8 drawit=( (drawmode==menudrawall)||
                       ( (item==sel)||(item==oldsel) )
                     );
        if ((mattrib & MAxorient)!=0)
        {
           temp=item->size.x;
           if (drawit)
              drawitem(xpos,ypos,temp,h,item);
           xpos+=temp+1;
        }
        else
        {
            temp=item->size.y;
            if (drawit)
               drawitem(xpos,ypos,w,temp,item);
            ypos+=temp+1;
        }        
        item=(tmenuitem*)item->next;
    }
             
    //fill in the gaps if there are any
    t_nativepalette=windowcols;
    if (drawmode==menudrawall)
    {
        if (mattrib & MApopup)
        {
           captilebitmapx(a.x,b.y-16,b.x-16,TWINBM_shadow);
           putbitmap(b.x-16,b.y-16,TWINBM_shadow[3]);
           captilebitmapy(b.x-16,a.y,b.y-16,&TWINBM_shadow[4]);           
        }
        t_fillcol=t_nativepalette[8];
        if (mattrib & MAxorient)
        {
            temp=a.x+w+4;
            if (xpos<temp)
              bar(xpos,ypos,temp,ypos+h);                        
        }else
        {
            temp=a.y+h+4;
            if (ypos<temp)
              bar(xpos,ypos,xpos+w,temp);                        
        }
    }
    

}

//should only set/unset modal in one place.. hmmm
//combination of keypresses and mouse moves causes it to get stuck
//and then a crash

bool tmenu::opensubmenu(tmenuitem *item,bool askey)
{
    if (!item) return FALSE;
    tmenu *submenu=item->submenu;
    tpoint itempos;
    UINT8 notopen;
    if (submenu)
    {                
        itempos=getitempos(sel);
        notopen=(submenu->owner==NULL);        
        if (notopen)
        {
           if ((mattrib & MAxorient)!=0)               
               submenu->tmoveto(a.x+4+itempos.x,b.y+1);
           else
               submenu->tmoveto(b.x+1,a.y+4+itempos.y);                                  
           root->insert(submenu);           
        }
        
        submenu->mattrib|=MAactive;           
        if (askey)
        {
            if (!ismodal(submenu)) setmodal(submenu);        
            submenu->oldsel=submenu->sel;
            submenu->sel=(tmenuitem*)submenu->data.first;
            if (!notopen)//just to force a redraw
            {
                 submenu->drawmode=menudrawchange;            
                 notopen=TRUE;
            }            
        }
        if (notopen)        
            submenu->drawview();                
        submenu->drawmode=menudrawall;
        submenu->select();
        return TRUE;
     }
     return FALSE;
}

bool tmenu::setitem(tmenuitem *item,bool askey)
{
    if ((item!=NULL)&&(item!=sel)&&((item->attrib & MIAseperator+MIAdisabled)==0))
    {           
        
        if (sel!=NULL)                    
          closesubmenus();                                
        oldsel=sel;
        sel=item;
        drawmode=menudrawchange;                      
        drawview();
        drawmode=menudrawall;
        if (sel!=NULL)
        {        
           if ((mattrib & MApopup) && askey)
             return TRUE;
           opensubmenu(sel,askey);           
        }
        return TRUE;
    }else
        return FALSE;
}

tmenuitem *tmenu::getnextitem(tmenuitem *item)
{
    tmenuitem *nextitem;
    if (item==NULL)
       nextitem=(tmenuitem *)data.first;
    else
       nextitem=(tmenuitem*)data.nextwrap(item);
    while ((nextitem->attrib & MIAseperator)!=0)    
        nextitem=(tmenuitem*)data.nextwrap(nextitem);
    setitem(nextitem,TRUE);
    return nextitem;    
}

tmenuitem *tmenu::getprevitem(tmenuitem *item)
{
    tmenuitem *previtem;
    if (item==NULL)
       previtem=(tmenuitem *)data.first;
    else
       previtem=(tmenuitem*)data.prevwrap(item);
    while ((previtem->attrib & MIAseperator)!=0)    
        previtem=(tmenuitem*)data.prevwrap(previtem);
    setitem(previtem,TRUE);
    return previtem;    
}

void tmenu::handleevent(tevent *event)
{
   tmenu *menu;
   tmenuitem *item;
   bool docommand=true;

   tview::handleevent(event);
   if (event->what & EVkeydown)
       {
           /*t_col=getcolour(255,0,0);
           rectangle(a.x,a.y,b.x,b.y);
           t_col=getcolour(255,255,0);
           if (global_selected)
             rectangle(global_selected->a.x+1,global_selected->a.y+1,global_selected->b.x-1,global_selected->b.y-1);
           */  
           if (mattrib & MAxorient)
           switch (scancode)
           {
               case upkey:scancode=leftkey;break;
               case downkey:scancode=rightkey;break;
               case leftkey:scancode=upkey;break;
               case rightkey:scancode=downkey;break;
           }                                                                          
           switch (scancode)
           {
               case upkey:
                   getprevitem(sel);break;
               case downkey:
                   getnextitem(sel);break;
               case rightkey:
                  if (sel->submenu!=NULL)//&&((mattrib & MApopup)!=0)                  
                      opensubmenu(sel,TRUE);                      
                  else if (parentmenu)
                  {                      
                      parentmenu->getnextitem(parentmenu->sel);
                      parentmenu->select();
                  }
                  break;
                  
               case leftkey:
                  if (parentmenu)
                  {
                    parentmenu->getprevitem(parentmenu->sel);
                    parentmenu->select();
                    
                  }
               break;              
           }
           if (charcode==13 && sel->submenu==NULL)  //if it has a submenu, don't collapse
               colapse(TRUE,this,NULL);
           else if (charcode!=0)
           {
               item=(tmenuitem*)data.first;
               //charcode=upcase(charcode);
               UINT8 shortkey;
               while (item!=NULL)               
               {
                   shortkey=item->shortkey;
                   if (shortkey!=0)
                   if (item->text[shortkey-1]==charcode)
                   {
                     setitem(item,TRUE);
                     break;            
                   }
                   item=(tmenuitem*)item->next;
               }
           }                                     
           
       }else if (state & SFmodal)
       {
           /*t_col=getcolour(0,0,255);
           rectangle(a.x,a.y,b.x,b.y);
           t_col=getcolour(255,0,255);*/
           //if (mattrib & MAactive)
           {
               
               mattrib|=MAactive;
               
               if (rcontains(mp.x,mp.y))
               {
                  item=getitematpos(mp);
                  if (item)               
                    setitem(item,FALSE);
                  else if (parentmenu && (mb!=0)&&((mattrib & MApopup)==0))     //if you click outside the button area, it closes the submenus
                    parentmenu->closesubmenus();
               }else
               {
                   if (sel)
                   {
                       menu=sel->submenu;
                       if ((menu)&&(menu->rcontains(mp.x,mp.y)))
                          menu->handleevent(event);
                   }
                   menu=parentmenu;
                   if ((menu)&&(menu->rcontains(mp.x,mp.y)))                                         
                      menu->handleevent(event);
                   else
                      docommand=false;                                  
               }
               if (ms_released && sel->submenu==NULL)  //if it has a submenu, don't collapse
               
                     colapse(docommand,this,NULL);                                 
           }           
       }else
       if ((event->what & EVmouse)!=0)
       {
           //if mouse is released and menu control hasn't changed, mode changes; check for click rather than release.
           if (((mattrib & MAactive)||(mb!=0))&& getitematpos(mp))
           {
               if (!ismodal(this))
                 setmodal(this);

               /*t_col=getcolour(0,255,0);
               rectangle(a.x,a.y,b.x,b.y);
               t_col=getcolour(0,255,255);*/
               mattrib|=MAactive;                              
           }           
       }
       event->what=0;    
}

//find open leaf node; has to be a leaf node!
tmenu* tmenu::findopenleaf()
{
    tmenu *selmenu=sel->submenu;
    
    while (selmenu!=NULL)
    {
      if (selmenu->sel && selmenu->sel->submenu && (selmenu->sel->submenu->mattrib & MAactive))      
          selmenu=selmenu->sel->submenu;
      else
          return selmenu;          
    }
    return selmenu;    
}


//go down and close
void tmenu::closesubmenus()
{    
    colapse(FALSE,findopenleaf(),this);
}

//go up and close

void tmenu::colapse(UINT8 docommand,tmenu *submenu,tmenu *stopat)
{            
       while (submenu!=NULL && submenu!=stopat)  //clear down menus and call command
       {
           if (docommand)
             evcallback(submenu->sel->command);
           submenu->oldsel=submenu->sel;
           submenu->sel=NULL;
           
           if ((submenu->mattrib & MApopup)!=0)                          
               submenu->remove();           
           else
           {               
               submenu->drawmode=menudrawchange;
               submenu->drawview();
               submenu->drawmode=menudrawall;
           }
           submenu->mattrib &=~MAactive;  //have to click to get it back
           //should be called in reverse order so that all modal selections are cleared
           if (submenu->state & SFmodal)
             endmodal();
           submenu=submenu->parentmenu;
       }       
}           


char tmenu::setstate(UINT16 astate, char enable)
{
    
    if (!enable)
    {
        //do higlighting stuff in here
    }
    char retval=tview::setstate(astate,enable);
    return retval;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SCROLL CONTAINER

scrollcontainer::scrollcontainer(INT16 x1,INT16 y1,INT16 x2,INT16 y2,tview *view_,UINT32 attrib_):
tgroup(x1,y1,x2,y2)
{
    //add scroll bars
    //resize view
    trect viewrect(x1,y1,x2,y2);
    attrib=attrib_;
    if ((attrib&CFxbar)!=0)
    {
        viewrect.b.y-=(sb_btn.y-1);
        xbar=new slidertype(a.x,viewrect.b.y,width(),0,0,SAxorient,CMDxvalchanged,NULL);
        xbar->growmode=TGdragony2|TGgrowallx;
        viewrect.b.y--;
        insert(xbar);    
    }else
        xbar=NULL;
    if ((attrib&CFybar)!=0)
    {
        viewrect.b.x-=(sb_btn.x-1);
        ybar=new slidertype(viewrect.b.x,a.y,height()+1,0,0,0,CMDyvalchanged,NULL);
        ybar->growmode=TGdragonx2|TGgrowally;
        viewrect.b.x--;  
        insert(ybar);        
    }else
        ybar=NULL;
    view=view_;
    if (view)
    {        
        view->growmode=TGgrowall;
        insert(view);        
        view->changebounds(viewrect);        
    }
    if (attrib & CFshadow)
    {
        b.x+=16;
        b.y+=16;
    }
    //insert view (calls initialupdate to view) allowing it to call back and resize the scroll bars    
}

/*scrollcontainer(INT16 x1,INT16 y1,tview *view_,UINT32 attrib)::
tgroup(x1,y1,view_->width()+(attrib&CFxbar
{
    //add scroll bars
    //resize view
    trect viewrect(x1,y1,x2,y2);
    if ((attrib&CFxbar))
    {
        viewrect.b.y-=(sb_btn.y-1);
        xbar=new slidertype(a.x,viewrect.b.y,width(),0,0,SAxorient,CMDxvalchanged,NULL);
        xbar->growmode=TGdragony2|TGgrowallx;
        viewrect.b.y--;
        insert(xbar);    
    }else
        xbar=NULL;
    if ((attrib&CFybar)!=0)
    {
        viewrect.b.x-=(sb_btn.x-1);
        ybar=new slidertype(viewrect.b.x,a.y,height()+1,0,0,0,CMDyvalchanged,NULL);
        ybar->growmode=TGdragonx2|TGgrowally;
        viewrect.b.x--;  
        insert(ybar);        
    }else
        ybar=NULL;
    view=view_;
    if (view)
    {        
        view->growmode=TGgrowall;
        insert(view);        
        view->changebounds(viewrect);        
    }
    //insert view (calls initialupdate to view) allowing it to call back and resize the scroll bars    
}
*/
void scrollcontainer::handleevent(tevent *event)
{
    tgroup::handleevent(event);
    if (event->what & (EVmouse + EVkeyboard))
    {
        if (state & SFmodal)
        {
            if (state & SFwaitingforrelease)
            {
                if (mb==0) state &=~SFwaitingforrelease;
            }else
            if ((mb!=0) && (!inview(mp.x,mp.y)))
            {
                endmodal();
                evcallback(CMDcontainermodalended);                
            }                
            else
                tgroup::handleevent(event);              
        }else
            tgroup::handleevent(event);
    }else if (event->what==EVcommand)        
    {
        valuedata *data=(valuedata*)event->infoptr;
        event->what=0;
        switch(event->command)
        {            
            case CMDxvalchanged:sendcommand(CMDxvalchanged,view,data);break;
            case CMDyvalchanged:sendcommand(CMDyvalchanged,view,data);break;
            case CMDxdeltavalchanged:if (xbar!=NULL){xbar->mute();xbar->setval(data->val);xbar->unmute();};break;
            case CMDydeltavalchanged:if (ybar!=NULL){ybar->mute();ybar->setval(data->val);ybar->unmute();};break;
            case CMDxdeltachanged:if (xbar!=NULL){xbar->mute();xbar->setrange(data->min,data->max,data->val);xbar->unmute();};break;
            case CMDydeltachanged:if (ybar!=NULL){ybar->mute();ybar->setrange(data->min,data->max,data->val);ybar->unmute();};break;
     /*     case CMDmakebarsinvisible:if (ybar!=NULL) ybar->makeinvisible();if (xbar!=NULL) xbar->makeinvisible();break;
            case CMDmakebarsvisible:if (ybar!=NULL) ybar->makevisible();if (xbar!=NULL) xbar->makevisible();break;
       */     
            default:
                 event->what=EVcommand;
                 target->handleevent(event);    
        }
    }
}

void scrollcontainer::draw()
{
    tgroup::draw();
    if (attrib & CFshadow)
    {
       captilebitmapx(a.x,b.y-16,b.x-16,TWINBM_shadow);
       putbitmap(b.x-16,b.y-16,TWINBM_shadow[3]);
       captilebitmapy(b.x-16,a.y,b.y-16,&TWINBM_shadow[4]);
    }
}


bordercontainer::bordercontainer(INT16 x,INT16 y,INT16 w,INT16 h,tview *inner):tgroup(x,y,x+w,y+h)
{
    trect be;
    be.rassign(a.x+2,a.y+2,b.x-2,b.y-2);
    inner->changebounds(be);
    insert(inner);
}

bordercontainer::bordercontainer(INT16 x,INT16 y,tview *inner):tgroup(x,y,inner->width()+4,inner->height()+4)
{
    trect be;
    be.rassign(a.x+2,a.y+2,b.x-2,b.y-2);
    inner->changebounds(be);
    insert(inner);
}

void bordercontainer::draw()
{
    windowgfx *wg;
    t_nativepalette=windowcols;
    if (state & (SFfocused+SFmouseover))          
      wg=&w_s;
    else          
      wg=&w_u;    
    
    drawouteredge(a.x+2,a.y+2,b.x-2,b.y-2,wg);
    tgroup::draw();
}

void bordercontainer::handleevent(tevent *event)
{
    if (event->what & EVmouse + EVkeyboard)
    {
        if (state & SFmodal)
        {
            
            if ((mb!=0) && (!inview(mp.x,mp.y)))
            {
                endmodal();
                evcallback(CMDcontainermodalended);
            }                
            else
                tgroup::handleevent(event);
              
         }else
            tgroup::handleevent(event);
    }else if (event->what==EVcommand)
         target->handleevent(event);
}


