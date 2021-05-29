
//Clayworks main source file
//Created 1/12/97 D/M/Y (or there abouts) 
//Copyright Tim Lewis 1997-Inf. All rights reserved, bugs n' all.

#include "stdinc.hpp"

//transformation mode change events
#define CMDstretchmode 600
#define CMDrotatemode 601
#define CMDtapermode 602
#define CMDtwistmode 603
#define CMDbendmode 604
  

//selection mode change events
#define CMDvertexmode 800
#define CMDedgemode 801
#define CMDpolygonmode 802
#define CMDobjectmode 803

//drawing mode change events
#define CMDselect   900
#define CMDline     901
#define CMDdot      902
#define CMDcube     903
#define CMDcylinder 904
#define CMDsphere   905
#define CMDgrid     906
#define CMDtext     907
#define CMDpolygon  908
#define CMDcone     910
#define CMDsquare   911
#define CMDcircle   912
#define CMDselipse  913

#define CMDapplymaterialtoselectedpolygons  920
#define CMDapplytexturetoselectedpolygons  921
#define CMDapplyattributetoselectedpolygons  922

#define CMDleave 999


#define CMDmaterialchanged 900


//these are temporay; more sophisticated functionality will make these redundant
#define CMDlight1 1001
#define CMDlight2 1002
#define CMDlight3 1003
#define CMDlight4 1004
#define CMDsnapgrid 1005
#define CMDviewgrid 1006
#define CMDlockcam 1501
#define CMDrendertype 1502
#define CMDtogspecular 1503
#define CMDtogfog 1504
#define CMDtogonlyshowselectednodes 1505
#define CMDeditfogcolour 1506
#define CMDfogchanged 1507
#define CMDtoggridchecker 1530

//redrawing events; causes different parts of the scene to be draw/not drawn

#define CMDscenealtered 1011            //everything's redrawn
#define CMDscenealteredorth 1012        //just orthographic views are redrawn
#define CMDundraw 1013                  //scene is redrawn without object specified in infoptr
#define CMDcalc3d 1014  //scene is redrawn without objects with selected vertices (should allow finer granularity; to vertex and polygon level)
#define CMDchangeobjectsphere 1020
#define CMDchangeobjectcube 1021
#define CMDchangeobjectcylinder 1022
#define CMDchangeobjectcone 1023

//default object parameter events
#define CMDcircvalchanged 1300
#define CMDspherexvalchanged 1301
#define CMDsphereyvalchanged 1302
#define CMDgridxvalchanged 1303
#define CMDgridyvalchanged 1304
#define CMDcylindervalchanged 1305
#define CMDconevalchanged 1306
#define CMDselipsexvalchanged 1307
#define CMDselipseyvalchanged 1308
#define CMDselipsep1changed   1309
#define CMDselipsep2changed   1310
#define CMDnumcopieschanged   1311
#define CMDtoggleselectcopy   1312
#define CMDtoggleselectextrude 1313
#define CMDtoggleselectkeeppolys 1314


#define CMDscreendump 1025
#define CMDselectionchanged 1026


#define CMDtogbase 2000
#define CMDtogsmooth CMDtogbase+paSHADED
#define CMDtogenvmap CMDtogbase+paENVMAP
#define CMDtogdouble CMDtogbase+paDOUBLESIDED
#define CMDtogtransparency CMDtogbase+paTRANSPARENT
#define CMDtognospectrans CMDtogbase+paNOSPECTRANS
#define CMDtoginterpolate CMDtogbase+paTEXTINTERPOLATED
#define CMDtogwireframe CMDtogbase+paOUTLINED
#define CMDtogtruecolour CMDtogbase+paTRUECOLOUR


#define CMDcancel        3000
#define CMDload3dobjectdialog 3001
#define CMDsave3dobjectdialog 3002
#define CMDappend3dobjectdialog 3003
#define CMDloadtexturedialog  3004
#define CMDload3dobject 3005
#define CMDsave3dobject 3006
#define CMDappend3dobject 3007
#define CMDloadtexture 3008


//for material editor, brings up colour selector
#define CMDeditambient 4000
#define CMDeditdiffuse 4001
#define CMDeditspecular 4002
#define CMDeditemmisive 4003

//commands that affect the selection 

#define CMDmergeselectedvertices 5001
#define CMDflipnormals 5002


#define PIDload3dobject 100
#define PIDloadtexture 101
#define PIDsave3dobject 102

#define BGMcolour 1
#define BGMgrid 2
#define BGMimage 3
#define BGMcamimage 4

#define DCgridon   0x1
#define DCdrawing  0x2

//drawing modes for tscene calc3dmode

#define CMdrawall 0             //draw all objects
#define CMsingleobj 1           //just draw selected object
#define CMlayeredsingleobj 2    //draw background and layer single object ontop
#define CMlayeredsingleobjclip 3//as above only clipped to bounds
#define CMdrawallclip 4         //draw all objects, clipped to bounds   
#define CMselectedonly 5        //only draw selected
#define CMlayeredselectedonly 6 //draw background and layer selected elements on top
#define CMinvselectedonly 7     //draw everything that isn't selected



//redraw levels
//These are the values for tscene::redrawlevel that index an array of vertex, polygon and object attribute filters defined bellow.
//combined with the tscene::calc3dmode variable, these provide a very high level of control over what vertices, polygons
//and objects will be processed and redrawn at a given time, increasing the speed of processing and reducing flicker.
//NB these values are only used if the current calc3dmode is either CMinvselectedonly, CMselectedonly or CMlayeredselectedonly:
        
#define RDLselectedvertices 0
#define RDLselectedpolygons 1
#define RDLcurrentvertex 2
#define RDLcurrentpolygon 3


//selection modes; these define what will happen when new elements are selected
//If a single element is selected and the mode is SMnewlist (by a click rather than a box selection) then it becomes the current
//element of that type and is subject to those rules
//If any of the other modes are active instead, the single element will be added to or subtracted from
//the list of that element type.
//mode 3 has the effect of selecting that single vertex only
#define SMnewlist 1
#define SMaddtolist 2
#define SMsubtractfromlist 3
#define SMsublistfromcurrentlist 4

//copying attributes. This defines what will happen when a selection is transformed
//if this is zero, it will just be transformed. Otherwise it will either be extruded or copied.
//if the STRNSkeepoldpolys flag is set, the polygons (if applicable) at the base of the extrusion
//will remain in place, otherwise they will be removed from the list and placed on the undo stack 
#define STRNScopy 1
#define STRNSextrude 2
#define STRNSkeepoldpolys 4

//extra extrude definitions
#define EXTRDsetdeleteflag 1  //flags original polygons for deletion

#define eltLOOP
#define eltSHARD

class tedgeloop
{
    public:
    UINT32 type;
    tedge **start;
    UINT32 count;
};

typedef struct undostruct
{
    UINT32 command;
    UINT32 datacount;
    UINT32 selectmode; //select mode at time of command    
    union
    {
        matrix4x4 *matrix;
        vertex *vertexlist;
        tpolygon *polygonlist;
        tobject3d *objectlist;
    };    
}undostruct;


class extrusion
{
    vector p;
    axisrotation o;
    flt s;
    matrix4x4 tmat;
    UINT32 pcount,ecount,vcount;
    vertex **vlist;
    tedge **elist;
    tpolygon **plist;

    extrusion(UINT32 pc,UINT32 ec,UINT32 vc);
};

//can either be applied to result or mask
class elementfilter
{
    public:
    UINT32 vertex,polygon,object; //vertex,polygon,object
};



listelembase T3Dselectionlevels[4]=
{{"Vertex",0,etVERTEX,0,0},{"Edge",0,etEDGE,0,0},{"Polygon",0,etPOLYGON,0,0},{"Object",0,etOBJECT,0,0}};

listelembase T3Dselectionfunctions[6]=
{{"Stretch",0,stSTRETCH,0,0},{"Rotate",0,stROTATE,0,0},{"Scale",0,stSCALE,0,0},{"Shear",0,stSHEAR,0,0},
{"Taper",0,stTAPER,0,0},{"Twist",0,stTWIST,0,0}};

listelembase T3Dselectioneffects[5]=
{{"New",0,bPUT,0,0},{"Add",0,bOR,0,0},{"Deselect",0,bNOT,0,0},{"Toggle",0,bXOR,0,0},{"Sub Select",0,bAND,0,0}};
listelembase T3Dselectiongeomerty[4]=
{{"Rectangle",0,0,0,0},{"Circle",0,0,0,0},{"Lasso",0,0,0,0},{"Polygon",0,0,0,0}};


//Index is T3Dselectmode of type etXXXXXX
UINT32 T3Dselectmasks[4]={naVISIBLE,0,paALLVERTICESVISIBLE,0};

elementfilter T3Dredrawmasks[4]={
                             {naSELECTED+naENABLED+naVISIBLE,paHASSELECTEDVERTICES+paENABLED,ofAFFECTEDBYSELECTION+ofENABLED},  //selected vertex list
                             {naSELECTED+naENABLED+naVISIBLE,paHASSELECTEDVERTICES+paENABLED,ofAFFECTEDBYSELECTION+ofENABLED}, //selected polygon list {naAFFECTEDBYSELECTION+naENABLED+naVISIBLE,paAFFECTEDBYSELECTION+paENABLED,ofAFFECTEDBYSELECTION+ofENABLED}
                             {naENABLED+naVISIBLE,paCONTAINSCURRENTVERTEX+paENABLED,ofENABLED},       //current vertex
                             {naENABLED+naVISIBLE,paENABLED,ofENABLED} //current polygon            
                             };


//bounding system is a little pants, needs some work
//Bounds are determined from scene->singleobj


////////////////////////////////////////////////////////////////////////////////////////
//TYPE SPECIFICATION

class spheredata
{
    public:
    UINT32 xsegs,ysegs;
};

class selipsedata
{
    public:
    UINT32 xsegs,ysegs;
    flt p1,p2;
};


class conedata
{
    public:
    UINT32 xsegs,ysegs;
};

class cylinderdata
{
    public:
    UINT32 xsegs,ysegs;
};

class griddata
{
    public:
    UINT32 xsegs,ysegs;
};


class circledata
{
    public:
    UINT32 segs;
};

class textdata
{
    public:
    char txt[255];
};


class edit3dcontainer;
class view3d;
class edit3d;

class logmessagestring:public telem
{
    public:
    tcolour colour;
    char string[255];    
};

class logmessagebox:public tview
{
    public:
    tlinkedlist *themessages;
    logmessagebox(INT16 x,INT16 y,INT16 w,INT16 h,tlinkedlist *messages);
    ~logmessagebox();
    UINT32 offset,sel,maxlines;
    void update();
    virtual void draw();
    virtual void handleevent(tevent *event);    
};


class teditscene:public tscene
{
    public:

    UINT32 redrawlevel;  //indexs an array of attribute filters for vertices, polygons and objects
    UINT32 sv_count,se_count,sp_count,so_count;    
    UINT32 av_count,ae_count,ap_count;   //affected (but not selected) counts
    UINT32 ce_count;                     //contained edge count; edges that are part of selection
                                         //but are not selected (i.e. they should be copied; these
                                         //are tagged onto the selected edge list just after the
                                         //edge that are actually selected)
    UINT32 el_count;
    theapmem sv_list;  //selected vertices list
    theapmem se_list;  //selected edge list
    theapmem sp_list;  //selected polygon list
    theapmem so_list;  //selected object list
    tedgeloop el_list[1000];  //edge loop list, references a sorted se_list
    
    tboundingbox3d *slbox;    
    UINT8 calc3dmode;
    
    tcursor3d *cursor;
    material *cmat;
    texture *ctex;
   // edit3dcontainer *views;
    
    vertex *cvertex;
    tpolygon *cpoly;
    tobject3d *singleobj;
    
    void setselectmode(UINT8 mode);
    void vectormult(vertex *vert,vector *src,matrix4x4 *mat);
    void preprocess_select(trect *cliprect,tsize *screensize,tcamera *viewcam,INT32 offx=0,INT32 offy=0);

    //these should be one routine when the lists are unified    
    void calcselvertexbounds();
    void copyoldselectedvertices();
    void transformselectedvertices(matrix4x4 *tmat);
    void transformselectedobjects(matrix4x4 *tmat);
    
    void calculatenormals(UINT32 objectmask,UINT32 polymask,UINT32 vertexmask);
    

    UINT32 removestrandedverticesandedges(UINT32 omask);
    UINT32 removeverticies(UINT32 omask,UINT32 vmask);
    UINT32 removeedges(UINT32 omask,UINT32 vmask);
    UINT32 removepolygons(UINT32 omask,UINT32 vmask);
    UINT32 removeobjects(UINT32 otmask,UINT32 omask);
    BOOL deleteselected(UINT32 mode);
    void flipselectedpolygons();
    vertex *setcurrentvertex(vertex *v);
    UINT32 selectelements(UINT8 elemtype,UINT8 boolmode,UINT32 omask,UINT32 emask);
    BOOL copyselpolygons(matrix4x4 *tmat,UINT32 attributes);
    BOOL extrudeselpolygons(matrix4x4 *tmat,UINT32 nsteps,UINT32 attributes); //tmat is a pointer to an array of matricies
    BOOL mergeselectedvertices(UINT8 posmode);
    UINT32 tidyuppolygons(UINT32 omask,UINT32 pmask);
        
    void resetrendermode();
    teditscene();

    ~teditscene();
    UINT32 undo();
    void setsp_material(material *mat);
    void setsp_texture(texture *tex);
    void setsp_attribute(UINT32 attrib,UINT8 mode);
    
    UINT32 alterscene(UINT16 command,void *param1,UINT32 param2);
    void buildselectionedge();
};


//abstract class for 3d views
class basic3dview:public tview
{
    public:        
    tcolour bg,fg;    
    teditscene *scene;
    tcamera *viewcam;
    UINT32 datasize;            
    tbitmap *scr;
    screenDataType scrdc;    
    //tobject3d *obj;
    basic3dview(INT16 x,INT16 y,INT16 x2,INT16 y2,teditscene *ltscene,char *scrdata);
    void resetdrawmode(UINT32 restorecommand);//sets the drawing mode for this scene to draw all object, it also renders the current object into the scene.
    void setsingleobjectdrawmode(tobject3d *obj);//draws the scene without drawing the specified object and then sets it up to layer that object on top
    void setselectdrawmode(tobject3d *obj);//draws the scene without drawing the specified object and then sets it up to layer that object on top
    virtual void calc3d(){;}
};

//these are used so that the edit window knows how to handle
//modal events (n.b. E3Ddrawingselectbox is also used in the draw method)

class edit3d:public basic3dview
{
    public:    
    edit3d(INT16 x,INT16 y,INT16 x2,INT16 y2,teditscene *ltscene,UINT8 axis_,char *scrdata);    
    ~edit3d();
    virtual void draw();
    void filter3dpos(flt x,flt y,flt z,vector *p);
    virtual void handleevent(tevent *event);
    void *handleselect(tevent *event,tobject3d *obj);
    virtual void changebounds(trect &bounds);
    virtual void initialupdate();
    virtual void calc3d();
    //needs acess to the real camera matrix
    void get2dpos(vector *p3d,vector *p2d);
    void get3dpos(INT16 x,INT16 y,vector *p);  //deals with a point, takes the window's position into account
    void get3dvec(INT16 x,INT16 y,vector *p);  //deals with a magnitude vector; convertes a 2d magnitude to a 3d one
    void changeoffsets(INT32 x,INT32 y);
    void setoffsets(double x,double y);
    void dostretch(bool corner);
    void startpolygon();
    void endmodal2();
    void startmodal(void (*modalfunc)(edit3d *ed,tevent *event));
};


////////////////////////////////////////////////////////////////////////
//container

class container:public tgroup
{
    public:
    void addbackview()
    {
        insert(new backview(a.x,a.y,b.x,b.y,windowcols[8]));
    }
    container(UINT8 flags=0,UINT32 id=PIDundefined):tgroup(0,0,1,1)
    {
        if (flags)
          addbackview();
        else          
          options|=OFnooverlap;        
        pid=id;
    }
    virtual void handleevent(tevent *event);
    
};

void container::handleevent(tevent *event)
{
   tgroup::handleevent(event);

   if (event->what==EVcommand)
   {       
       owner->owner->handleevent(event);
       //evcallback(event->command);       
   }
}

class leftbar:public tgroup
{
    public:
    ttoolbar *functionbar;
    leftbar(UINT16 x,UINT16 y,UINT16 height);
    virtual void handleevent(tevent *event);
    virtual void draw();    
};

class clay:public tgroup
{
    public:    
    tmenu *menu;
    container *selectctrl,*squarectrl,*circlectrl,*polyctrl,*cubectrl,*spherectrl,*conectrl,*gridctrl,*textctrl,*cylinderctrl,*selipsectrl;
    dropbox *selectionlevel,*selectionfunction,*selectioneffect;    
    tabbox *drawfuncs;
    tgroup *desktop;
    view3d *renderview;
    edit3d *orthxy,*orthxz,*orthzy;
    logmessagebox *messages;
    renderall();
    clay(INT16 x,INT16 y,INT16 x2,INT16 y2);
    virtual void handleevent(tevent *event);    
};

clay *view;


class view3d:public basic3dview
{
    public:
    UINT32 xoff,yoff;    
    char lockcam;
    char rendertype;    
    view3d(INT16 x,INT16 y,INT16 x2,INT16 y2,teditscene *ltscene,char *scrdata);
    ~view3d();
    virtual void draw();
    virtual void handleevent(tevent *event);
    virtual void changebounds(trect &bounds);
    virtual void initialupdate();
    virtual void calc3d();    
};



class scrollmaterialview:public tview
{
    public:
    tcolour bg,fg;
    //UINT8 backgroundmode;
    t3dmesh *obj;
    teditscene *scene;
    tcamera *viewcam;
    UINT32 datasize;
    
    tbitmap *scr[3];
    screenDataType scrdc[3];
    INT32 offset,sel,maxopts,idxoffset;
    UINT8 getoffsetfromsel();
    scrollmaterialview(INT16 x,INT16 y,teditscene *ltscene);
    void setoffset(INT32 offset);
    void calc3d(UINT8 calcmask);
    bool calc3dview(UINT8 idx);
    void calcinternalvars();
    virtual void draw();    
    virtual void handleevent(tevent *event);
    virtual void changebounds(trect &bounds);
};

class edit3dcontainer:public window
{
    tpoint quadcentre;
    edit3d *xy,*yz,*xz;
    view3d *pers;
    UINT8 *scr;
    edit3dcontainer(UINT16 x,UINT16 y,UINT16 w,UINT16 h,tscene *scn);
    virtual void draw();
    virtual void changebounds(trect &bounds);    
};

////////////////////////////////////////////////////////////////////////
//Preferences
//currently bellow to save my scrolling fingers..

////////////////////////////////////////////////////////////////////////
//material edit

class materialedit:public tgroup
{
    public:
    trgba editcolourrgb;
    material *mat;
    ttoolbar *objecttypes;
    tabbox *tabs;    
    tbutton *togsmooth;
    tbutton *togenvmap;
    tbutton *togdouble;
    tbutton *tognospectrans;
    tbutton *togtransparent;
    tbutton *toginterpolate;
    tbutton *togtruecolour;
    tbutton *togwireframe;    
    bordercontainer *colourcontainer;
    colourselector *editcolourselector;
    tbutton *colours[4];
    tbutton *applymaterial,*applytexture,*applyattribute,*applyall;
    tcolour buttonpalette[64];        
    numbox *transparency;
    numbox *specpower;
    numbox *specstr;
    listelem *texturelist;
    tlight *l;
    t3dmesh *obj;
    tscene *scene;
    scrollmaterialview *renderview;
    scrollcontainer *renderscroll;
    dropbox *textures,*materials;
    UINT8 objcount;
    UINT8 editcolour;    

    materialedit(INT16 x1,INT16 y1,INT16 x2,INT16 y2,material *mat_);
    ~materialedit();
    void calc3d(UINT8 mode);
    virtual void handleevent(tevent *event);
    void createtexturelist();
    void buildbuttonpalette(UINT8 ref,UINT8 r,UINT8 g,UINT8 b,bool setmat);
};




////////////////////////////////////////////////////////////////////////////////////
//GLOBAL VARIABLES

UINT8 T3Dcheckergrid;
UINT32 T3Dselectioneffect;
UINT32 T3Dcopyattributes;
tlinkedlist T3Dmessages;

fileviewstruct load3dobject;
fileviewstruct save3dobject;
fileviewstruct loadtexture;
spheredata spherevals;
selipsedata selipsevals;
conedata   conevals;
cylinderdata cylindervals;
griddata gridvals;
circledata circlevals;
textdata textvals;
tbitmap *toolbm,*toolbm2;
//material *pmat;
treebox *treeview;
tbitmap *sphere;
tlight *plight;
tlight *plight2;
tlight *plight3;
tlight *plight4;
materialedit *matedit;
UINT8 fin=0;

/*t3dmesh *obj;
tbitmap *flare;*/

//the scene being edited. maybe to be replaced by ed_scene for contextual stuff?
teditscene *thescene;

//would be static only it relies on external function pointers; oo methods are slightly broken I suppose.
//globals are alright for this because with modal variables, we cannot have two
//edit3d views doing the same thing at the same time.
//all global variables pertaining to modal sections for the edit3d view are prefixed with a 'ed_'
#define ed_maxpolyvertices 256
UINT32 ed_waitingforrelease;
vector ed_p;
tpolygon *ed_polygon;
vertex *ed_polygonvertex;
INT16 ed_xdiff,ed_ydiff;   
tpoint ed_oldmp;
flt ed_cx,ed_cy,ed_oldscale;
vector ed_p1;
UINT8 ed_scandown;
trect ed_tempbounds; //used in order to draw the temporay bounding box as it's being drawn
INT32 ed_boundcode;
vector ed_oldboundmin,ed_oldboundmax;
trectflt ed_sobjoldbounds;
teditscene *ed_scene;
t3dmesh *ed_obj;
bool ed_cornerstretch;
tboundingbox3d *ed_bounds;  //current bounds that are in use; set in setselectmode
vector ed_boundoldmin,ed_boundoldmax;
tpoint ed_addmin;
tpoint ed_addmax;

void (*ed_modalfunc)(edit3d *ed,tevent *event)=NULL;
char buf[255],exitstring[512];       



////////////////////////////////////////////////////////////////////////////////////
//THE CODE

T3Dgetinvertedobjectmatrix(matrix4x4 *invmat,tobject3d *obj)
{
    //should be single matrix operation, this is just to get it right
    matrix4x4 scaleinvmat;
    float sx=1.0f/obj->s.x;float sy=1.0f/obj->s.y;float sz=1.0f/obj->s.z;
    LMtranslate(invmat,-obj->p.x,-obj->p.y,-obj->p.z);
    LMrottrans(&scaleinvmat,obj->o.x,obj->o.y,obj->o.z,0,0,0);
    LMtranspose(&scaleinvmat);
    LMconcatenatematrices(&scaleinvmat,invmat,invmat);
    LMscale(&scaleinvmat,sx,sy,sz);                      
    LMconcatenatematrices(&scaleinvmat,invmat,invmat);            
}


void T3Daddtexturefunc(textureref *tref)
{
    if (matedit)
      matedit->createtexturelist();

}

void T3Daddmaterialfunc(material *mat)
{
    if (matedit)
    {
        matedit->renderview->calcinternalvars();
        matedit->renderview->drawview();            
    }
}


void T3Dresetelementmasks()
{
    T3Dobjectprocessmask=T3Dobjectprocessmaskresult=ofENABLED;    
    T3Dpolygonprocessmask=T3Dpolygonprocessmaskresult=paENABLED;    
    T3Dvertexprocessmask=T3Dvertexprocessmaskresult=naENABLED+naVISIBLE;
    T3Dobjecttypemask=0xFFFFFFFF;    
}


extern UINT16 copyscr(char *scr,char *offscr,UINT32 count);
#pragma aux copyscr=\
"shr ecx,2"\
"rep movsd"\
parm [edi][esi][ecx]

void writeraw(char *filename,UINT16 w,UINT16 h,UINT16 *data)
{
    UINT32 i;
    UINT8 r,g,b;
    FILE *f=fopen(filename,"wb");
    //fwrite( const void *__ptr, size_t __size, size_t __n, FILE *__fp );
    fwrite(&w,2,1,f);
    fwrite(&h,2,1,f);
    for (i=0;i<w*h;i++)      
    {
       UINT16 col=*data;
       data++;
       b=(col & 31)<<3;
       col>>=5;
       g=(col & 63)<<2;
       col>>=6;
       r=(col & 31)<<3;
       fwrite(&r,1,1,f);
       fwrite(&g,1,1,f);
       fwrite(&b,1,1,f);                      
    }
    fclose(f);
}

////////////////////////////////////////////////////////////////////////
//edit scene

teditscene::teditscene():tscene()
{
    T3Dselectionfunction=stSTRETCH;
    calc3dmode=CMdrawall;
    redrawlevel=RDLselectedvertices; //this is only used if we are drawing (or not drawing) the selected elements only
    T3Ddefaultobjectattributes &=~ofEDITABLE;
    grid=new tgrid3d(1.5,1.5,1.5);
    strcpy(grid->name,"THE GRID");
    cursor=new tcursor3d();
    sv_list.create(10000<<2);        
    sp_list.create(10000<<2);
    se_list.create(10000<<2);
    so_list.create(10000<<2);
    sp_count=0;
    se_count=0;
    sv_count=0;
    so_count=0;
    ed_bounds=slbox=new tboundingbox3d(&sv_count);
    setselectmode(etVERTEX);        
    
    addobject(NULL,grid);
    addobject(NULL,cursor);
    addobject(NULL,slbox);

    T3Ddefaultobjectattributes |=ofEDITABLE;
    cvertex=NULL;
    cpoly=NULL;
    cobj=singleobj=NULL;
    cmat=NULL;
    ctex=NULL;
    drawmode=CMDselect;
    //create editing polygon
    /*for (int i=0;i<ed_maxpolyvertices;i++)
    {
        ed_polygon.vlist[i]=&ed_polygonvertices[i];
    }*/
}

teditscene::~teditscene()
{
  sv_list.destroy();
}




BOOL teditscene::deleteselected(UINT32 selectmode)
{
   switch (selectmode)
   {
      case etVERTEX:
         if (sv_count!=0)
         {
             removepolygons(ofHASSELECTEDVERTICES,paHASSELECTEDVERTICES);
             removeverticies(ofHASSELECTEDVERTICES,naSELECTED);                     
             sv_count=0;
             sv_list.reset();
             return TRUE;
         }                                                                    
         break;
      case etPOLYGON:
         if (sp_count!=0)
         {
             removepolygons(ofHASSELECTEDPOLYGONS,paSELECTED);
             //should only remove 'standalone' vertices
             removestrandedverticesandedges(ofHASSELECTEDPOLYGONS);
             sp_count=0;
             sp_list.reset();
             return TRUE;
         }
         break;
      case etOBJECT:                   
             removeobjects(0xFFFFFFFF,ofSELECTED+ofEDITABLE);
             treeview->update();
             return TRUE;
         //break;      
   }
   return FALSE;
}

void teditscene::flipselectedpolygons()
{
    tpolygon **cp,**end;        
    for (end=&(cp=(tpolygon **)sp_list.base)[sp_count];cp<end;cp++)
    {
      (*cp)->reverse();
    }    
}

//unfortunatly, this needs globals.
//However, the removelots function is neat. it's faster than removing one element at a time
//and encapsulating the code inside the linkedlist class makes it smaller and neater.


UINT32 gbl_mask;
UINT32 gbl_rcount;

//this version checks to see if any edges reference this; if none do, it'll get deleted
//regardless of it's attribute

BOOL checkremovevertexfunc(telem *celem)
{
    return ((((vertex*)celem)->attribute & gbl_mask)==gbl_mask);
}

BOOL removevertexfunc(telem *celem)
{
    //((vertex*)celem)->deletereferences();
    gbl_rcount++;
    //add celem to vertex restore list here
    return TRUE;
}

//edges
BOOL checkremoveedgefunc(telem *celem)
{
    return ((((tedge*)celem)->attribute & gbl_mask)==gbl_mask);
}

BOOL removeedgefunc(telem *celem)
{
    ((tedge*)celem)->deletereferences();
    gbl_rcount++;
    //add celem to vertex restore list here
    return TRUE;
}

//polygons
BOOL checkremovepolygonfunc(telem *celem)
{
    return (((tpolygon*)celem)->attribute & gbl_mask)==gbl_mask;
}

BOOL removepolygonfunc(telem *celem)
{
    ((tpolygon*)celem)->removeedgereferences();
    if (celem==thescene->cpoly)
      thescene->cpoly=NULL;
    //add celem to polygon restore list here
    gbl_rcount++;
    return TRUE;
}

UINT32 teditscene::removestrandedverticesandedges(UINT32 omask)
{    
    t3dmesh *obj=(t3dmesh*)scenegraph.root;    
    gbl_rcount=0;    
    tpolygon *cp;
    
    //should recurse
    while (obj!=NULL)
    {
        if ((obj->type==otGEOM)&&((obj->attribute & omask)==omask))
        {
          //or ALL the attributes as deleted
          obj->or_vertexattributes(naDELETEME);
          obj->or_edgeattributes(eaDELETEME);
          cp=(tpolygon*)obj->polygonlist.first;
          //de-or those elements that are referenced
          while (cp!=NULL)
          {
            cp->and_vertexattributes(~naDELETEME);
            cp->and_edgeattributes(~eaDELETEME);            
            cp=(tpolygon *)cp->next;
          }
          //wipe out the elements still tagged for deletion.
          gbl_mask=naDELETEME;              
          obj->vertexlist.removelots(checkremovevertexfunc,removevertexfunc);
          gbl_mask=eaDELETEME;
          obj->edgelist.removelots(checkremoveedgefunc,removeedgefunc);
        }
        obj=(t3dmesh*)obj->next;
    }
    return gbl_rcount;
}

UINT32 teditscene::removeedges(UINT32 omask,UINT32 emask)
{    
    t3dmesh *obj=(t3dmesh*)scenegraph.root;    
    gbl_rcount=0;
    gbl_mask=emask;
    //should recurse
    while (obj!=NULL)
    {
        if ((obj->type==otGEOM)&&((obj->attribute & omask)==omask))        
            obj->edgelist.removelots(checkremoveedgefunc,removeedgefunc);                            
        obj=(t3dmesh*)obj->next;
    }
    return gbl_rcount;
}

UINT32 teditscene::removeverticies(UINT32 omask,UINT32 vmask)
{    
    t3dmesh *obj=(t3dmesh*)scenegraph.root;    
    gbl_rcount=0;
    gbl_mask=vmask;
    //should recurse
    while (obj!=NULL)
    {
        if ((obj->type==otGEOM)&&((obj->attribute & omask)==omask))        
            obj->vertexlist.removelots(checkremovevertexfunc,removevertexfunc);                            
        obj=(t3dmesh*)obj->next;
    }
    return gbl_rcount;
}



UINT32 teditscene::removepolygons(UINT32 omask,UINT32 pmask)
{   
    t3dmesh *obj=(t3dmesh*)scenegraph.root;
    gbl_rcount=0;
    gbl_mask=pmask;
    //should recurse
    while (obj!=NULL)
    {
        if ((obj->type==otGEOM)&&((obj->attribute & omask)==omask))        
            obj->polygonlist.removelots(checkremovepolygonfunc,removepolygonfunc);                                            
        obj=(t3dmesh*)obj->next;
    }
    return gbl_rcount;
}

UINT32 teditscene::removeobjects(UINT32 otmask,UINT32 omask)
{
    t3dmesh *obj=(t3dmesh*)scenegraph.root;
    t3dmesh *prev;
    UINT32 rcount;
    //should recurse
    prev=NULL;
    while (obj!=NULL)
    {
        if ((obj->type & otmask)&&((obj->attribute & omask)==omask))
        {
            //add obj to restore list here
            removeobject(obj);
            rcount++;
        }
        obj=(t3dmesh*)obj->next;
    }
    return rcount;
    //should now go through selected polygon list and check for enteries with the same
    //flags, removing where necissary    
}



//copies old polygon (for possible undo later) and removes duplicated vertices
//if all vertices are removed, the polygon is flagged for deletion
UINT32 tidyuppolygon(tpolygon *p)
{
    tpolyvertex *cpv,*oldcpv,*endpv,*npv;
    INT32 npoints=0;
    //this will be placed on a stack to restore the polygons; it knows which polygon to replace from the next field 
    tpolygon *sp=T3Dcreatepolygon(p,NULL);
    sp->next=(telem*)p;
    npv=p->vlist;
    endpv=&(cpv=sp->vlist)[sp->npoints];
    p->and_vertexrefattributes(~pvaRESERVED);
    while (cpv<endpv)
    {
        oldcpv=cpv;
        oldcpv++;
        if ((cpv->attribute & pvaRESERVED)==0)
        while (oldcpv<endpv)
        {
            if (oldcpv->v == cpv->v)            
              cpv->attribute |=pvaRESERVED;                          
            oldcpv++;                        
        }        
        if ((cpv->attribute & pvaRESERVED)==0)
        {
          *npv=*cpv;
          npv->attribute=0;
          npv++;
          npoints++;
        }
        cpv++;
    }
    p->npoints=npoints;
    //see which ones were removed. If not, tidy up vertex references    
    if (npoints<=2)
    {
        p->attribute |=paDELETEME;
        return 1;
    }
    
    return 0;          
}

UINT32 teditscene::tidyuppolygons(UINT32 omask,UINT32 pmask)
{
    UINT32 deletecount=0;
    tobject3d *obj;
    tpolygon *p;
    obj=(tobject3d*)scenegraph.root;    
    while (obj!=NULL)
    {
        if (obj->type==otGEOM &&((obj->attribute & omask)==omask))
        {
            p=(tpolygon*)((t3dmesh*)obj)->polygonlist.first;
            while (p!=NULL)
            {
                if ((p->attribute & pmask)==pmask);
                  deletecount+=tidyuppolygon(p);
                p=p->nxt();                
            }
        }
        obj=(tobject3d*)obj->next;
    }
    return deletecount;
}

//will merge vertices depending on attribute; can then be used to merge polygons
BOOL teditscene::mergeselectedvertices(UINT8 posmode)
{
/*    tpolygon *p;
    tpolyvertex *cpv,*endpv;
    vertex *cv;
    t3dmesh *obj;
    matrix4x4 invmat;
    if (sv_count<=1) return FALSE;
    vertex *v=*(vertex**)sv_list.base;
    obj=(t3dmesh*)scenegraph.root;    
    while (obj!=NULL)
    {                     
        T3Dgetinvertedobjectmatrix(&invmat,obj);
        if (obj->type==otGEOM && (obj->attribute & ofHASSELECTEDVERTICES))
        {
            cv=(vertex *)((t3dmesh*)obj)->vertexlist.first;
            while (((cv->attribute & naSELECTED)==0) && cv!=NULL)                            
                cv=cv->nxt();
            //deliberate lack of error checking here; if it crashes, the flags are wrong
            cv->attribute &=~naSELECTED;
            obj->tempdata=(void*)cv;
            cv->data3d=v->data3d;
            cv->deletereferences();
            //cv->data3d.multmatrixtrans(&invmat,&cv->data3d);
            p=(tpolygon*)((t3dmesh*)obj)->polygonlist.first;
            while (p!=NULL)
            {                
               for (endpv=&(cpv=p->vlist)[p->npoints];cpv<endpv;cpv++)
               {
                   if (cpv->v->attribute & naSELECTED)
                       cpv->v=cv;
               }
               p=p->nxt();
            }
        }
        obj=(t3dmesh*)obj->next;
    }    
    removeverticies(ofHASSELECTEDVERTICES,naSELECTED);
    UINT32 tidycount=tidyuppolygons(ofHASSELECTEDVERTICES,paHASSELECTEDVERTICES);
    if (tidycount>0)
    {
      char buf[100];
      removepolygons(ofHASSELECTEDVERTICES,paDELETEME);
      sprintf(buf,"%i polygons flaged for deletion",tidycount);
      logmessage((void*)&textcol, buf);
    }
    //now add polygons references to the resultant merged vertex
    //major typecasting stuff.. I will tye my brain in knots if this needs debuging
    obj=(t3dmesh*)scenegraph.root;    
    while (obj!=NULL)
    {                             
        if (obj->type==otGEOM && (obj->attribute & ofHASSELECTEDVERTICES))
        {
            p=(tpolygon*)((t3dmesh*)obj)->polygonlist.first;
            while (p!=NULL)
            {                
               if ((p->attribute & paHASSELECTEDVERTICES))
               {
                 tpolygonref *newref=new tpolygonref;
                 newref->p=p;                   
                 SLaddtostart( (telem**)&((vertex*)obj->tempdata)->prefs,(telem *)newref);
               }
               p=p->nxt();
            }
        }
        obj=(t3dmesh*)obj->next;
    }    
    calculatenormals(ofHASSELECTEDVERTICES,paHASSELECTEDVERTICES,0xFFFFFFFF);
    selectelements(etPOLYGON,bOR,ofHASSELECTEDPOLYGONS,paSELECTED);
    selectelements(etVERTEX,bOR,ofHASSELECTEDVERTICES,naSELECTED);                        */
    return TRUE;    
}
    
void teditscene::setsp_material(material *mat)
{
    tpolygon **cp,**end;
    for (end=&(cp=(tpolygon **)sp_list.base)[sp_count];cp<end;cp++)
      (*cp)->mat=mat;            
}

void teditscene::setsp_texture(texture *tex)
{
    tpolygon **cp,**end;        
    for (end=&(cp=(tpolygon **)sp_list.base)[sp_count];cp<end;cp++)
    {
      (*cp)->tex=tex;
      (*cp)->attribute|=paTEXTURED;
    }
}

void teditscene::setsp_attribute(UINT32 attrib,UINT8 mode)
{
    tpolygon **cp,**end,*p;

    for (end=&(cp=(tpolygon **)sp_list.base)[sp_count];cp<end;cp++)
    {
        p=*cp;
        switch (mode)
        {
          case bPUT:
            p->attribute&=paRESERVED_MASK+paTEXTURED;
            p->attribute|=attrib;
            break;
          case bXOR:
            p->attribute|=attrib;
            break;
          case bNOT:
            p->attribute&=(0xFFFFFFFF^attrib);
            break;
          case bAND:
            p->attribute&=(attrib);
            break;
        }        
    }
}



void teditscene::setselectmode(UINT8 mode)
{
   T3Dselectmode=mode;
}

vertex *teditscene::setcurrentvertex(vertex *cv)
{
    /*t3dmesh *obj=cv->parent;
    vertex *v=(vertex *)obj->vertexlist.first;
    tpolygon *p=(tpolygon*)obj->polygonlist.first;
    while (v!=NULL)
    {
        v->attribute &=~naAFFECTEDBYCURRENTVERTEX;
        v=v->nxt();
    }
    while (p!=NULL)
    {        
        if (p->contains(cv))
        {
            p->attribute |= paCONTAINSCURRENTVERTEX;
            p->or_vertexattributes(naAFFECTEDBYCURRENTVERTEX);
        }else
            p->attribute &= ~paCONTAINSCURRENTVERTEX;
        p=p->nxt();
    }

    cv->attribute |= naCURRENT;
    return (cvertex=cv);*/
    return NULL;
}

void teditscene::calcselvertexbounds()
{
    if (sv_count==0) return;
    vector min,max,t;
    min.moveto(1000000,1000000,1000000);
    max.moveto(-1000000,-1000000,-1000000);                            
    vertex **cv=(vertex**)sv_list.base,*v;
    
    for (int i=0;i<sv_count;i++)
    {
        v=*cv;
        v->data3d.multmatrixtrans(&v->parent->worldmat,&t);
        if (t.x>max.x) max.x=t.x;
        if (t.y>max.y) max.y=t.y;
        if (t.z>max.z) max.z=t.z;
                            
        if (t.x<min.x) min.x=t.x;
        if (t.y<min.y) min.y=t.y;
        if (t.z<min.z) min.z=t.z;
        cv++;        
    }
    slbox->min=min;
    slbox->max=max;        
}

void teditscene::calculatenormals(UINT32 objectmask,UINT32 polymask,UINT32 vertexmask)
{
    tobject3d *obj=(tobject3d*)scenegraph.root;
    while (obj!=NULL)
    {
        if ((obj->type==otGEOM)&&(obj->attribute & objectmask))
          ((t3dmesh*)obj)->calculatenormals(polymask,vertexmask);
        obj=(tobject3d*)obj->next;
    }
}

void teditscene::resetrendermode()
{
}

void vectormult(vertex *vert,vector *src,matrix4x4 *mat)
{
    src->multmatrixinv(mat,&vert->data3d);
}

tedge *T3Dcreateedge2(vertex *v,vertex *v1,UINT8 *mem)
{
    tedge *newe;/*=(mem!=NULL)?(tedge*)mem:(tedge*)malloc(sizeof(tedge));
    newe->v[0]=v;
    newe->v[1]=v1;*/
    /*if (!v->anedge)
      v->anedge=newe;    
    if (!v1->anedge)
      v1->anedge=newe;
    newe->writetag(0xFFFFFFFF);
    newe->prefs=NULL;    
    newe->attribute=eaENABLED;*/
    return newe;
}


BOOL teditscene::copyselpolygons(matrix4x4 *tmat,UINT32 attributes)
{
    UINT32 report;
    t3dmesh *obj=(t3dmesh*)scenegraph.root;
    tpolygon *newp;
    UINT32 ctag,ctag2;
    UINT32 tag=0;
    vector newv;
    tpolyvertex *pv;
    //cos they're  big, use the T3Dvisiblepolygons and T3Dvisibleedges blocks
    vertex **vlist=(vertex **)T3Dvisiblepolygons;
    vertex **cv=(vertex**)sv_list.base,*v,**endv;
    tedge **ce=(tedge**)se_list.base,*e,**ende;
    tedge **elist=T3Dvisibleedges;    
    endv=&cv[sv_count];        //vlist=(vertex **)T3Dvisiblepolygons;
    for (;cv<endv;cv++)
    {
        v=*cv;
        v->attribute&=~naSELECTED;
        v->writetag(tag);                               
        newv=v->data3d;
        
        vlist[tag]=v->parent->addvertex(newv.x,newv.y,newv.z);
        vlist[tag]->attribute |=naSELECTED;
        vlist[tag]->data3d=newv;
        tag++;
    }
    tag=0;
    ende=&ce[ce_count];
    for (;ce<ende;ce++)
    {
        e=*ce;
        e->writetag(tag);
        ctag=e->v[0]->readtag();
        ctag2=e->v[1]->readtag();
        t3dmesh *parent=e->parent();
        if (parent)
        {
            elist[tag]=parent->addedge(vlist[ctag],vlist[ctag2],NULL);            
            /*if ((report=elist[tag]->checkintegrity()))
                switch (report)
                {
                    case eiVERTEXPARENTSDAMAGED :logmessage((void*)&T3DCOLboundingbox,"Edges parents are inconsistant");break;
                    case eiVERTEXNULLS          :logmessage((void*)&T3DCOLboundingbox,"Null vertex found on edge");break;
                }*/
            tag++;
        }  
    }
    
    tpolygon **cp=(tpolygon**)sp_list.base,*p,**end;
    end=&cp[sp_count];
    //vlist=(vertex **)T3Dvisiblepolygons;
    for (;cp<end;cp++)
    {
        p=*cp;        
        newp=T3Dcreatepolygon(p->npoints,NULL);
        newp->tex=p->tex;
        newp->mat=p->mat;
        newp->attribute=p->attribute;
        p->attribute&=~paSELECTED;
        if (attributes & EXTRDsetdeleteflag)
          p->attribute |=paDELETEME;        
        pv=p->vlist;
        for (UINT32 i=0;i<p->npoints;i++)
        {            
            //ctag=pv->v->readtag();
            ctag2=pv->e->readtag();          
            if (ctag2<tag)
                T3Dsetpolygonvertex(newp,elist[ctag2],pv->texv.x,pv->texv.y,i,(pv->e->v[0]==pv->v)?0:pvaEDGEISANTICLOCKWISE);            
            pv++;
        }
        
        /*if ((report=p->checkintegrity()))
        {
            switch (report)
            {
            case piEDGESDAMAGED:logmessage((void*)&T3DCOLboundingbox,"Polygon edges are fucked up");break;
            case piVERTEXPARENTSDAMAGED:logmessage((void*)&T3DCOLboundingbox,"Polygon vertices have different parent objects (shock)");break;
            }
        }    
        else*/
          p->parent()->polygonlist.push(newp);                                    
    }    
    //calculatenormals(ofHASSELECTEDPOLYGONS,0xFFFFFFFF,0xFFFFFFFF);    
    return TRUE;
}

BOOL teditscene::extrudeselpolygons(matrix4x4 *tmat,UINT32 nsteps,UINT32 attributes)
{

    tpointflt tvecs[4];
    tvecs[0].assign(0.0f,0.0f);
    tvecs[1].assign(1.0f,0.0f);
    tvecs[2].assign(1.0f,1.0f);
    tvecs[2].assign(0.0f,1.0f);
    
    if (se_count>0)
    {
       UINT32 oldcfflags=T3Dpolycreationflags;
       UINT32 oldatflags=T3Ddefaultpolyattributes;
       copyselpolygons(tmat,attributes);
       tedge **elist=(tedge **)T3Dvisibleedges;
       tedge **ce=(tedge**)se_list.base,*e,**ende;
       tpolygon *p;       
       ende=&ce[se_count];
       for (;ce<ende;ce++)
       {          
          e=*ce;
          T3Dpolycreationflags =oldcfflags;
          T3Ddefaultpolyattributes= oldatflags;

          if (e->attribute & eaCLOCKWISESELECTION)          
            T3Dpolycreationflags =oldcfflags | cfANTICLOCKWISE;
          else
            T3Dpolycreationflags =oldcfflags  &~cfANTICLOCKWISE;
          if (e->attribute & eaDOUBLESIDEDEXTRUSION)
            T3Ddefaultpolyattributes|=paDOUBLESIDED;                               
          
          p=e->parent()->singleedgeconnect(e,elist[e->readtag()],tvecs);
          //if (ce!=ende-1)
                    
          //(ce->parent()->addquad(ce->from,ce->to,vlist[ce->to->readtag()],vlist[ce->from->readtag()],tvecs))->attribute |=paAFFECTEDBYSELECTEDPOLYGON;        
       }
       T3Dpolycreationflags =oldcfflags;
       T3Ddefaultpolyattributes=oldatflags;

       return TRUE;
    }
    return FALSE;
    //calculatenormals(ofHASSELECTEDPOLYGONS,0xFFFFFFFF,0xFFFFFFFF);    */
}



UINT8 elemcheckarray[7]={0,etOBJECT,etVERTEX+etEDGE+etPOLYGON+etOBJECT,etOBJECT,etOBJECT,0,0};

//minimal preprocess for selection
void teditscene::preprocess_select(trect *cliprect,tsize *screensize,tcamera *viewcam,INT32 offx,INT32 offy)
{
    trect oldvp=SC.viewport;
    tsize oldsize=SC.size;
    SC.viewport=*cliprect;
    SC.size=*screensize;
    preprocess(viewcam,offx,offy);
    SC.viewport=oldvp;
    SC.size=oldsize;      
}

void eswap(tedge **e1,tedge **e2)
{
    tedge *t;
    t=*e1;*e1=*e2;*e2=t;
}

void sortforedgeloops(teditscene *scene)
{

    INT32 i;
    vertex **vlist=(vertex**)scene->sv_list.base;
    for (i=0;i<scene->sv_count;i++)    
    {
        vlist[i]->num=i;
    }
    //scene->el_count=0;

    UINT32 el_count=0;
    UINT32 destcount=0;
    UINT32 count=scene->se_count;
    UINT32 ypos=20;
    tedge *ce,*ne,*pe;
    //tedge *slist=malloc(count << 1);
    tedge **dlist=(tedge **)scene->se_list.base;    
    //memcpy(slist,dlist,count << 1);
    UINT32 j,loopcount;
    tedge **start=&dlist[0];
    loopcount=1;
    BOOL edgefound=false;


    for (j=0;j<count;j++)
    {        
        ne=dlist[j];
        ne->edgeloopref=el_count;
        ne->edgeloopcount=loopcount-1;
        i=j+1;
        edgefound=false;
        
        while ((i<count) && (edgefound==false))
        {
            
            if (dlist[i]->sharesvertexwith(ne))
            {
              eswap(&dlist[i],&dlist[j+1]);              
              loopcount++;
             
              edgefound=true;
            }
                     
           ypos+=20;
           if (ypos>SC.size.y) ypos=20;

           i++;
            
        }
          
        foundedge:
        if (!edgefound)
        {                
           scene->el_list[el_count].start=start;
           scene->el_list[el_count].count=loopcount;
           el_count++;
           start=&dlist[j];
           loopcount=1;
        }
    }
    scene->el_count=el_count;
}

//#define eltLOOP
//#define eltSHARD



void doedgestuff(teditscene *scene,tedge *ce,BOOL connectifvertsselected)//UINT32 *selectededgecount,UINT32 *affectededgecount
{
    while (ce!=NULL)
    {
        tpolygonref *prefs=ce->prefs;
        UINT32 selcount=0;
        UINT32 polycount=0;
                                
        while (prefs!=NULL)
        {
             if (prefs->p->attribute & paSELECTED)
                 selcount++;
             polycount++;                                    
             prefs=(tpolygonref*)prefs->next;
        }
        if (selcount>0)
           ce->attribute |= eaHIGHLIGHTED;
        else
           ce->attribute &= ~eaHIGHLIGHTED;
                  
        if (selcount>0 && (polycount==1 || selcount<polycount) )  //<polycount || ((ce->selectedvertexcount()==2 && selcount==0)&& connectifvertsselected)
        {
            ce->attribute &=~(eaCLOCKWISESELECTION+eaDOUBLESIDEDEXTRUSION);
            switch (ce->isclockwise())
            {
               case TRUE:ce->attribute |=eaCLOCKWISESELECTION;break;            
               case NEITHERTRUENORFALSE:ce->attribute |=eaDOUBLESIDEDEXTRUSION;break;
            }

            ce->attribute |= eaSELECTED;
            scene->se_list.grabandsetpointer(ce);
            scene->se_count++;
        }
        else
           ce->attribute &= ~eaSELECTED;
        ce=(tedge *)ce->next;                                
    }
    sortforedgeloops(scene);
    char buf[100];
    sprintf(buf,"Edgeloops found:%i",scene->el_count);
    logmessage((void *)&textcol,buf);
    
}
//This function requires the scene to be preprocessed according to a certain view first
//it selects all the elements that could be seen the last time the scene's view was calculated
//This routine also handles all the manipulation of the selection lists and the flags pertaining
//to selection and the current vertex, polygon or object.
//If there are any problems with this (and the managment of all that can be a can of worms), the answer should lie in here
UINT32 teditscene::selectelements(UINT8 elemtype,UINT8 boolmode,UINT32 omask,UINT32 emask)
{
    //go through object list, clip bounds against viewmat
    //If object is of relevent type and bounds intersect, check each element.
    //update SVL/SEL/SPL/SOL as appropriate
    //return number of elements selected
    tobject3d *obj=(tobject3d*)scenegraph.root;    
    vertex *cv;
    tedge *ce;
    tpolygon *cp;    
    tobject3d **cobj,**cobjend;
    //s=selected, a=affected v=vertex e=edge p=polygon
    //this may be silly, I may have to ensure that older data is preserved...
    sp_count=se_count=sv_count=so_count=ap_count=ae_count=av_count=0;        
    sv_list.reset();
    se_list.reset();
    sp_list.reset();
    so_list.reset();
    
    while (obj!=NULL)
    {
        //check to see if this type contains the sort of element we're dealing with
        UINT8 type=obj->type;
        //if ((elemcheckarray[type]&elemtype)!=0)
        {            
            UINT32 globalvis=((obj->attribute & omask)==omask);            
            {                
                switch (type)
                {                    
                    case otGEOM:
                                                
                        switch (elemtype)
                        {
                        case etVERTEX:
                             //if it's not visible and it won't be affected, don't do any processing.
                             if (!globalvis && (obj->attribute & ofHASSELECTEDVERTICES)==0) break;
                             cv=(vertex*) (((t3dmesh*)obj)->vertexlist.first);
                             obj->attribute &=~(ofHASSELECTEDVERTICES+ofAFFECTEDBYSELECTION);                             
                             while (cv!=NULL)
                             {
                                //add, remove or toggle selected vertex
                                cv->attribute &=~naAFFECTEDBYSELECTION;
                                if ( ((cv->attribute & emask)==emask) && (globalvis))                                
                                    cv->attribute=SETBITBYMODE(cv->attribute,naSELECTED,boolmode);
                                else if (boolmode==bPUT || boolmode==bAND )
                                    cv->attribute &=~naSELECTED;
                                //check to see if it's selected after all that stuff
                                //if so, add to select list and mark object as affected
                                if (cv->attribute & naSELECTED)
                                {
                                    if ((obj->attribute & ofAFFECTEDBYSELECTION)==0)
                                    {                                                                                                                 
                                       so_list.grabandsetpointer(obj);
                                       so_count++;
                                    }
                                    obj->attribute|=ofAFFECTEDBYSELECTION+ofHASSELECTEDVERTICES;
                                    sv_list.grabandsetpointer(cv);
                                    sv_count++;                                    
                                }                                                   
                                cv=cv->nxt();
                            }
                            //now set the relevent polygon flags and select certain polygons
                            cp=(tpolygon*) (((t3dmesh*)obj)->polygonlist.first);                            
                            while (cp!=NULL)
                            {                                
                                UINT32 nselected=cp->checkallvertexattributes(naSELECTED);
                                //all vertices selected so the polygon is selected too                                
                                if (nselected==cp->npoints)
                                {
                                    cp->attribute |= (paALLVERTICESSELECTED+paHASSELECTEDVERTICES+paAFFECTEDBYSELECTION+paSELECTED);
                                    cp->or_vertexattributes(naAFFECTEDBYSELECTION+naREFERENCEDBYSELECTEDPOLYGON);
                                    obj->attribute|=ofHASSELECTEDPOLYGONS;
                                    sp_list.grabandsetpointer(cp);
                                    sp_count++;                                                                              
                                }
                                else
                                {
                                    cp->attribute &=~(paALLVERTICESSELECTED+paSELECTED);
                                    if (nselected==0)
                                    {
                                        cp->attribute &=~paHASSELECTEDVERTICES;
                                        cp->and_vertexattributes(~naAFFECTEDBYSELECTION);                                        
                                    }
                                    else
                                    {
                                       cp->attribute |=paHASSELECTEDVERTICES+paAFFECTEDBYSELECTION;
                                       cp->or_vertexattributes(naAFFECTEDBYSELECTION);
                                    }
                                }                                                              
                                cp=cp->nxt();
                            }
                            doedgestuff(this,(tedge*) (((t3dmesh*)obj)->edgelist.first),true);
                            
                        break;
                        case etPOLYGON:
                            //if it's not visible and it won't be affected, don't do any processing.
                            if (!globalvis && (obj->attribute & ofHASSELECTEDPOLYGONS)==0) break;
                            obj->attribute&=~(ofHASSELECTEDPOLYGONS+ofAFFECTEDBYSELECTION);
                            //clear down vertex flags pertaining to selected polygons
                            cv=(vertex*) (((t3dmesh*)obj)->vertexlist.first); 
                            while (cv!=NULL)
                            {
                                cv->attribute &=~ (naREFERENCEDBYSELECTEDPOLYGON+naAFFECTEDBYSELECTION+naSELECTED);
                                cv=cv->nxt();
                            }                    
                            cp=(tpolygon*) (((t3dmesh*)obj)->polygonlist.first);                                                         
                            while (cp!=NULL)
                            {
                                tpolyvertex *v=cp->vlist;
                                cp->attribute &=~(paALLVERTICESSELECTED+paHASSELECTEDVERTICES);                                
                                //add, remove or toggle polygons from selected list
                                if ( ((cp->attribute & emask)==emask) && (globalvis))                                                                
                                    cp->attribute=SETBITBYMODE(cp->attribute,paSELECTED+paAFFECTEDBYSELECTION+paHASSELECTEDVERTICES+paALLVERTICESSELECTED,boolmode);
                                else if (boolmode==bPUT || boolmode==bAND )                                    
                                    cp->attribute &=~(paSELECTED+paAFFECTEDBYSELECTION+paHASSELECTEDVERTICES+paALLVERTICESSELECTED);
                                //check if it's selected after all that...
                                if (cp->attribute & paSELECTED)
                                {
                                    //set flags in object as it's now affected by the selection
                                    if ((obj->attribute & ofAFFECTEDBYSELECTION)==0)
                                    {                                                                                                                 
                                       so_list.grabandsetpointer(obj);
                                       so_count++;                                       
                                    }
                                    sp_list.grabandsetpointer(cp);
                                    obj->attribute|=ofAFFECTEDBYSELECTION+ofHASSELECTEDPOLYGONS+ofHASSELECTEDVERTICES;
                                    sp_count++;
                                    //selects the verticies here
                                    cp->or_vertexattributes(naREFERENCEDBYSELECTEDPOLYGON+naSELECTED);
                                }                      
                                cp=cp->nxt();
                            }
                            //check to see if any vertices in the polygon list have been
                            //referenced by this selection; if so, set all of them to be affected
                            //bt this selection; 
                            //The same goes for the polygon; clear the polyflag if none touch it
                            if (globalvis)
                            {
                                cp=(tpolygon*) (((t3dmesh*)obj)->polygonlist.first);
                                while (cp!=NULL)
                                {
                                     if (cp->checkvertexattributes(naREFERENCEDBYSELECTEDPOLYGON))
                                     {
                                         cp->or_vertexattributes(naAFFECTEDBYSELECTION);
                                         cp->attribute |= paAFFECTEDBYSELECTION+paHASSELECTEDVERTICES;                                                                          
                                     }else                                     
                                         cp->attribute &=~paAFFECTEDBYSELECTION+paHASSELECTEDVERTICES;
                                     
                                     cp=cp->nxt();
                                }                                
                                //add selected vertices to selected vertex list
                                cv=(vertex*) (((t3dmesh*)obj)->vertexlist.first); 
                                while (cv!=NULL)
                                {
                                     if (cv->attribute & naSELECTED)
                                     {
                                         sv_list.grabandsetpointer(cv);
                                         sv_count++;
                                     }                                                                                
                                     cv=cv->nxt();
                                }
                                
                            }
                            doedgestuff(this,(tedge*) (((t3dmesh*)obj)->edgelist.first),false);                                                                    
                        break;    
                    }
                    
                    break;                        
                }                        
                //process this object and it's children
            }
        }
        obj=(tobject3d*)obj->next;
    }
    //don't panic, just goes through the object list
    //adds affected elements to the end of the selected lists
    ce_count=se_count;
    for (cobjend=(tobject3d**)&(cobj=(tobject3d**)so_list.base)[so_count];cobj<cobjend;cobj++)
      
    {
        //for now, it'll only be of type 'geom' but will include other types later
        //for selection of beizer nodes and surfaces amoungst other things
        obj=*cobj;
        switch (obj->type)
        {                    
            case otGEOM:            
                cv=(vertex*) (((t3dmesh*)obj)->vertexlist.first);
                while (cv)
                {
                    if (cv->attribute & naAFFECTEDBYSELECTION  && ((cv->attribute & naSELECTED)==0))
                    {
                        sv_list.grabandsetpointer(cv);
                        av_count++;
                    }
                    cv=(vertex*)cv->next;
                }
                ce=(tedge*) (((t3dmesh*)obj)->edgelist.first);
                //contained edges
                while (ce)
                {
                    if (ce->attribute & eaHIGHLIGHTED && ((ce->attribute & eaSELECTED)==0))                    
                    {
                        se_list.grabandsetpointer(ce);
                        ce_count++;
                    }
                    ce=(tedge*)ce->next;
                }
                
                sprintf(buf,"Contained edge count: %i selected edge count: %i",ce_count,se_count);
                logmessage((void*)&textcol,buf);
    
    
                //affected edges
                ce=(tedge*) (((t3dmesh*)obj)->edgelist.first);
                while (ce)
                {
                    if (ce->attribute & eaAFFECTEDBYSELECTION && ((ce->attribute & eaSELECTED+eaHIGHLIGHTED)==0))
                    {
                        se_list.grabandsetpointer(ce);
                        ae_count++;
                    }
                    ce=(tedge*)ce->next;
                }
                cp=(tpolygon*) (((t3dmesh*)obj)->polygonlist.first);
                while (cp)
                {
                    if (cp->attribute & paAFFECTEDBYSELECTION && ((cp->attribute & paSELECTED)==0))
                    {
                        sp_list.grabandsetpointer(cp);
                        ap_count++;
                    }
                    cp=(tpolygon*)cp->next;
                }
            break;
        }
    }            
    calcselvertexbounds();
    return sv_count;
}

void teditscene::copyoldselectedvertices()
{
    int i;
    vertex **cvref;
    cvref=(vertex**)sv_list.base;   
    for (i=0;i<sv_count;i++)
    {        
       (*cvref)->old=(*cvref)->data3d;        
       cvref++;
    }
}


void teditscene::transformselectedvertices(matrix4x4 *tmat)
{
    vertex **cvref=(vertex**)sv_list.base,*cv;
    int i;    
    matrix4x4 scaleinvmat,invmat;
    
    LMidentity(&scaleinvmat);       
    tobject3d *obj=NULL;
    for (i=0;i<sv_count;i++)
    {
        cv=*cvref;
        if (cv->parent!=obj)
        {
            obj=cv->parent;
            //contruct matrix; hopefully I can get this down to two function calls :)
            //sorting the vertexlist by parent object would also be a good idea
            T3Dgetinvertedobjectmatrix(&invmat,obj);
            LMconcatenatematrices(tmat,&obj->worldmat,&scaleinvmat);
            LMconcatenatematrices(&invmat,&scaleinvmat,&scaleinvmat);                                                                                                      
        }
        cv->old.multmatrixtrans(&scaleinvmat,&cv->data3d);
        cvref++;
    }        
}


/*void teditscene::transformselectedobjects(matrix4x4 *tmat)
o{
}*/


////////////////////////////////////////////////////////////////////////
//basic3dview abstract class

basic3dview::basic3dview(INT16 x,INT16 y,INT16 x2,INT16 y2,teditscene *ltscene,char *scrdata):tview(x,y,x2,y2)
{
    logmessage((void*)&textcol,"Entered basic 3d view constructor");
    INT32 w=width()+1;
    INT32 h=height()+1;
    INT32 offs=w << 1;
    bg=getcolour(20,80,120);
    fg=getcolour(180,220,255);
    datasize=(offs+(8-(offs & 7)))*h; 
    logmessage((void*)&textcol, "about to allocate screen data");
    if (scrdata==NULL)
    {

        scrdata=(char *)malloc(datasize);
    }
    scr=new tbitmap(w,h,BMflat16,normalput,BMFdoublewordalign,scrdata,NULL);
        
    scene=ltscene;
    createcontext(&scrdc,scr);
    //logmessage((void*)&textcol,"done");
}



void basic3dview::resetdrawmode(UINT32 restorecommand)
{    
     scene->calc3dmode=restorecommand;
     evcallback(CMDscenealteredorth);
     scene->calc3dmode=CMdrawall;    
     scene->singleobj=NULL;            
     //endmodal();
}

void basic3dview::setsingleobjectdrawmode(tobject3d *obj)
{
     if (!obj) return;
     scene->calc3dmode=CMdrawallclip;                
     scene->singleobj=obj;
     sendcommand(CMDundraw,owner,obj);                            
     scene->calc3dmode=CMlayeredsingleobj;
}

void basic3dview::setselectdrawmode(tobject3d *obj)
{
     if (!obj) return;
     scene->calc3dmode=CMinvselectedonly;     
     scene->singleobj=obj;//if dealing with multiple element selection, will be boundingbox. otherwise, the object containing the single element to be modified     
     evcallback(CMDcalc3d);     
     scene->calc3dmode=CMlayeredselectedonly;
}



//////////////////////////////////////////////////////////////////////////
//edit3d modal functions
//The edit3d class is going to be very large. I have split up it's modal operations into a set of functions
//in order to avoid case statements from hell and totally unreadable code
//each one takes a pointer to the calling edit3d class and the event that was recieved




void modal_waitformouse(edit3d *ed,tevent *event)
{
    //outtextxy(0,640,"Waiting for mouse");
    if (mb==0)
    {        
        ed->endmodal2();
    }
}

void modal_cursormove(edit3d *ed,tevent *event)
{
    if (mb==2)
    {
        if ((ms_moved)&&(!ed_scandown))
        {
            ed->get3dpos(mp.x,mp.y,&ed_p1);
            ed_scene->grid->snapto(&ed_p1);
            ed_scene->cursor->moveto(ed_p1.x,ed_p1.y,ed_p1.z);
            ed->evcallback(CMDscenealteredorth);
        }
    }else
    {
        ed->resetdrawmode(CMsingleobj);
        ed->endmodal2();                                 
    }
}

void modal_drawingselectbox(edit3d *ed,tevent *event)
{ 
   if (ms_moved)
   {
        ed_tempbounds.b.x=mp.x-ed->a.x;
        ed_tempbounds.b.y=mp.y-ed->a.y;
        ed->drawview();                  //draw the view, rectangle will be overlayed                  
   }
   if (mb==0)
   {
        ed->endmodal2();
        ed_tempbounds.sortelements();
        ed_scene->preprocess_select(&ed_tempbounds,&ed->scrdc.size,ed->viewcam,ed_tempbounds.a.x,ed_tempbounds.a.y);
        ed_scene->selectelements(T3Dselectmode,T3Dselectioneffect,ofVISIBLE+ofENABLED,T3Dselectmasks[T3Dselectmode]);
        ed->evcallback(CMDscenealtered);
        ed->evcallback(CMDselectionchanged);
   }
}


void modal_polygoncreate(edit3d *ed,tevent *event)
{    
    //search for or create vertex here
    if (!mb && ms_moved)
    {
        ed_waitingforrelease=false;
        ed->get3dpos(mp.x,mp.y,&ed_p1);
        ed_scene->grid->snapto(&ed_p1);
        ed_polygonvertex->data3d=ed_p1;
        ed_obj->calculatenormals();
        ed->evcallback(CMDscenealtered);
    }else if (!ed_waitingforrelease)
    {
        if (mb==1)
        {
           if (ed_polygon->npoints<ed_maxpolyvertices)
           {
              logmessage((void*)&textcol, "add point");             
              ed_polygonvertex->data3d=ed_p1;        
              ed_polygonvertex = ed_obj->addvertex(ed_p1.x,ed_p1.y,ed_p1.z);
              T3Dsetpolygonvertex(ed_polygon,ed_polygonvertex,0.0f,0.0f,ed_polygon->npoints++,0);           
              ed_waitingforrelease=true;
           }
        }else if (mb>1)
        {
           ed_polygon=T3Dreallocpoly(ed_polygon,ed_polygon->npoints);
           ed_obj->calculatenormals();
           ed->resetdrawmode(CMsingleobj);
           ed->endmodal2();
           treeview->update();           
        }
    }
    
}




void modal_objectcreate(edit3d *ed,tevent *event)
{
  //we're creating an object via scaling, this applies to all shapes that require it; polylines and curves use a different method
  if (mb==1)
  {
     if ((ms_moved)&&(!ed_scandown))
     {                                                
        ed_xdiff=mp.x-ed_oldmp.x;
        ed_ydiff=mp.y-ed_oldmp.y;
        if (abs(ed_xdiff-ed_ydiff)<10) ed_ydiff=ed_xdiff;                                 
        ed->filter3dpos((ed_xdiff)/T3Dscale,(ed_ydiff) / T3Dscale,(ed_xdiff)/T3Dscale,&ed_p);//(mp.x-oldmp.x) /T3Dscale
        ed_scene->grid->snapto(&ed_p);
        ed_obj->scale(ed_p.x,ed_p.y,ed_p.z);
        if (ed_cornerstretch)
        {
           vector s=ed_obj->s/2;
           switch (ed->viewcam->camtype)
           {
               case camORTHXY:
               s.z=0.0f;break;
               case camORTHXZ:
               s.y=0.0f;s.z=-s.z;break;
               case camORTHZY:
               s.x=0.0f;s.z=-s.z;break;
           }
           ed_obj->moveto(ed_p1.x+s.x,ed_p1.y+s.y,ed_p1.z+s.z);
        }
        //ed_obj->calculatenormals();                        
        ed->evcallback(CMDscenealtered);                        
     }
  }else
  {                    
     if (mb==0)
     {
        if (mp.x==ed_oldmp.x)                        
           ed_obj->scale(1,ed_obj->s.y,1);
        if (mp.y==ed_oldmp.y)                        
           ed_obj->scale(ed_obj->s.x,1,1);

        ed_obj->calculatenormals();                        
        ed->resetdrawmode(CMsingleobj);
                
        /*ed_scene->calc3dmode=CMsingleobj;    
        ed->evcallback(CMDscenealteredorth);                        
        ed_scene->singleobj=NULL;*/
        treeview->update();
        ed->endmodal2();                        
     }else if(mb==3)
     {                        
        ed_scene->calc3dmode=CMdrawall;    
        ed_scene->singleobj=NULL;                        
        ed_scene->removeobject(ed_obj);
        ed->evcallback(CMDscenealtered);
        ed_modalfunc=modal_waitformouse;
     }
     ed_obj=NULL;                  
   }                                     
}


//////////////////////////////////////////////////////////////////////////
//edit3d

void modal_resizebounds(edit3d *ed,tevent *event)
{    
    if (mb!=0)
    {
       if ((ms_moved)&&(!ed_scandown))    
       {       
          vector addminv,addmaxv;
          
          if (ed_boundcode & left)
          {
              ed_addmin.x+=mdiff.x;
              if (*scan & SMleftaltkey ) ed_addmax.x-=mdiff.x;
          }
          if ((ed_boundcode & above) != 0)
          {
              ed_addmin.y+=mdiff.y;          
              if (*scan & SMleftaltkey ) ed_addmax.y-=mdiff.y;
          }
          if ((ed_boundcode & right) != 0)
          {
              ed_addmax.x+=mdiff.x;          
              if (*scan & SMleftaltkey ) ed_addmin.x-=mdiff.x;
          }
          if ((ed_boundcode & bellow) != 0)
          {
              ed_addmax.y+=mdiff.y;          
              if (*scan & SMleftaltkey ) ed_addmin.y-=mdiff.y;
          }
          ed->get3dvec(ed_addmin.x,ed_addmin.y,&addminv);
          ed->get3dvec(ed_addmax.x,ed_addmax.y,&addmaxv);
          float temp=addmaxv.z;addmaxv.z=addminv.z;addminv.z=temp;
          
          matrix4x4 tmat;
          vector c;
          thescene->grid->snapto(&addminv);
          thescene->grid->snapto(&addmaxv);
          ed_bounds->min=ed_boundoldmin+addminv;
          
          if (ed_boundcode==middle)
          {
             ed_bounds->max=ed_boundoldmax+addminv;                  
             LMtranslate(&tmat,addminv.x,addminv.y,addminv.z);
          }
          else
          {         
             ed_bounds->max=ed_boundoldmax+addmaxv;         
             LMstretch(&tmat,&ed_boundoldmin,&ed_boundoldmax,&ed_bounds->min,&ed_bounds->max); 
          }
          ed_scene->transformselectedvertices(&tmat);
          ed_scene->calculatenormals(ofAFFECTEDBYSELECTION,paAFFECTEDBYSELECTION,naAFFECTEDBYSELECTION);
          //ed_scene->calculatenormals(ofAFFECTEDBYSELECTION,0xFFFFFFFF,0xFFFFFFFF);
          ed->evcallback(CMDscenealtered);             
       }
       /*if (mb==3)
       {
          
          if (!waitingforrelease)
          {
             
             ed_scene->extrudeselpolygons(NULL,1,0);
             //ed->evcallback(CMDscenealtered);
             waitingforrelease=true;
           }
       }else waitingforrelease=false;*/
    }else
    {
        ed_bounds->sortelements();            
        ed->endmodal2();
        ed->resetdrawmode(CMselectedonly);
    }
}  

void modal_rotatebounds(edit3d *ed,tevent *event)
{
    vector r;
    if (mb!=NULL)
    {
    if ((ms_moved)&&(!ed_scandown))    
    {
      T3Dboundingboxrotation+=mdiff.x/100.0f;
      ed_bounds->moveto(ed_scene->cursor->p.x,ed_scene->cursor->p.y,ed_scene->cursor->p.z);
      T3Dbbcentreofrotation=ed_scene->cursor->p;
      ed->filter3dpos(0,0,T3Dboundingboxrotation,&r);
      ed_bounds->rotate(r.x,r.y,r.z);   
      matrix4x4 tmat;
      vector p=-ed_scene->cursor->p;
      LMrotatearound(&tmat,r.x,r.y,r.z,&p);
      ed_scene->transformselectedvertices(&tmat);
      ed_scene->calculatenormals(ofAFFECTEDBYSELECTION,paAFFECTEDBYSELECTION,naAFFECTEDBYSELECTION);              
      ed->evcallback(CMDscenealtered);       
      
    }
    }else
    {
        T3Dboundingboxrotation=0.0f;
        T3Dbbcentreofrotation=vector(0,0,0);
        ed_bounds->moveto(0,0,0);
        ed_bounds->rotate(0,0,0);
        
        ed_scene->calcselvertexbounds();
        
        ed_bounds->sortelements();            
        ed->endmodal2();
        ed->resetdrawmode(CMselectedonly);
    }
}  
 


BOOL m_chkrect(INT16 x1,INT16 y1,INT16 x2,INT16 y2,tpoint &mp)
{
    return ((mp.x>=x1)&&(mp.x<=x2)&&(mp.y>=y1)&&(mp.y<=y2));
}

BOOL m_chkrect(float x1,float y1,float x2,float y2,tpointflt &mp)
{
    return ((mp.x>=x1)&&(mp.x<=x2)&&(mp.y>=y1)&&(mp.y<=y2));
}

void edit3d::startpolygon()
{
    scene->addobject(NULL,ed_obj);
    ed_obj->moveto(0,0,0);
    scene->grid->snapto(&ed_p1);
    scene->singleobj=ed_obj;
        
    ed_polygon=ed_obj->addpoly(ed_maxpolyvertices,NULL);
    ed_polygon->npoints=0;    
    ed_polygonvertex=ed_obj->addvertex(ed_p1.x,ed_p1.y,ed_p1.z);
    T3Dsetpolygonvertex(ed_polygon,ed_polygonvertex,0.0f,0.0f,ed_polygon->npoints++,0);//pvaNOTADDEDTOPLIST
    ed_polygonvertex=ed_obj->addvertex(ed_p1.x,ed_p1.y,ed_p1.z);
    T3Dsetpolygonvertex(ed_polygon,ed_polygonvertex,0.0f,0.0f,ed_polygon->npoints++,0);//pvaNOTADDEDTOPLIST
    scene->calc3dmode=CMlayeredsingleobj;    
    evcallback(CMDscenealtered);
    startmodal(modal_polygoncreate);
    //ed_waitingforrelease=true;    
}

void edit3d::dostretch(bool corner)
{
    ed_cornerstretch=corner;
    scene->addobject(NULL,ed_obj);
    scene->grid->snapto(&ed_p1);

    filter3dpos((mp.x-ed_oldmp.x)/T3Dscale,(mp.y-ed_oldmp.y) / T3Dscale,(mp.x-ed_oldmp.x)/T3Dscale,&ed_p);//(mp.x-oldmp.x) / T3Dscale    
    scene->grid->snapto(&ed_p);
        
    ed_obj->scale(ed_p.x,ed_p.y,ed_p.z);    
    if (!corner)
       ed_obj->moveto(ed_p1.x,ed_p1.y,ed_p1.z);
    else
    {
       vector s=ed_obj->s/2;
       switch (viewcam->camtype)
       {
           case camORTHXY:
           s.z=0.0f;break;
           case camORTHXZ:
           s.y=0.0f;s.z=-s.z;break;
           case camORTHZY:
           s.x=0.0f;s.z=-s.z;break;
       }
       ed_obj->moveto(ed_p1.x+s.x,ed_p1.y+s.y,ed_p1.z+s.z);
    }
    ed_obj->calculatenormals();
    scene->singleobj=ed_obj;
    scene->calc3dmode=CMlayeredsingleobj;    
    evcallback(CMDscenealtered);
    startmodal(modal_objectcreate);
}

void *edit3d::handleselect(tevent *event,tobject3d *obj)
{
    vertex *cv;
    UINT16 halfx,halfy;
    //tpolygon *cp;
    vector c;
    trectflt bounds;    
    tpointflt m;
    m.x=mp.x-a.x;m.y=mp.y-a.y;
    BOOL noelementsselected=TRUE;
    while ((obj!=NULL)&&(noelementsselected))
    {
        if ((obj->attribute & ofENABLED) &&(obj->bounds.rcontains(m)))
        {
            switch (obj->type)
            {
            case otGEOM:
              cv=(vertex*)((t3dmesh *)obj)->vertexlist.first;
              while (cv!=NULL)
              {
                  if (cv->point2d.closeto(&m,2.0f))
                  {
                      scene->setcurrentvertex(cv);
                      noelementsselected=FALSE;
                      matrix4x4 scaleinvmat,invmat,transmat;
                      float sx=1.0f/obj->s.x;float sy=1.0f/obj->s.y;float sz=1.0f/obj->s.z;
                      LMtranslate(&invmat,-obj->p.x,-obj->p.y,-obj->p.z);
                      LMrottrans(&scaleinvmat,obj->o.x,obj->o.y,obj->o.z,0,0,0);
                      LMtranspose(&scaleinvmat);
                      LMconcatenatematrices(&scaleinvmat,&invmat,&invmat);                      
                      LMscale(&scaleinvmat,sx,sy,sz);                      
                      LMconcatenatematrices(&scaleinvmat,&invmat,&invmat);                           
    
                      setsingleobjectdrawmode(obj);
                      
                      cv->old=cv->data3d;
                      int tx=0,ty=0;
                      while (mb!=0)
                      {
                          MOUSEpoll();
                          if (ms_moved)
                          {
                                                      
                          get3dvec(tx+=mdiff.x,ty+=mdiff.y,&c);
                          vector r;
                          LMtranslate(&transmat,c.x,c.y,c.z);
                          LMconcatenatematrices(&transmat,&obj->worldmat,&scaleinvmat);
                          LMconcatenatematrices(&invmat,&scaleinvmat,&scaleinvmat);
                                                    
                          cv->old.multmatrixtrans(&scaleinvmat,&cv->data3d);
                          ((t3dmesh*)obj)->calculatenormals(0xFFFFFFFF,0xFFFFFFFF);
                          evcallback(CMDscenealtered);
                          
                          }
                      }
                      resetdrawmode(CMsingleobj);                      
                      break;
                  }
                  cv=cv->nxt();
              }
            break;
            case otCAMERA:break;
            case otLIGHT:break;
            case otBOUNDINGBOX:
            if (*ed_bounds->count>0)
            {
                 if (T3Dcopyattributes & (STRNScopy+STRNSextrude))
                     {
                        UINT32 attribute=0;
                        if (!(T3Dcopyattributes & STRNSkeepoldpolys))
                          attribute|=EXTRDsetdeleteflag;
                        if (T3Dcopyattributes & STRNScopy)
                            scene->copyselpolygons(NULL,attribute);                                                                
                        else if (T3Dcopyattributes & STRNSextrude)
                            scene->extrudeselpolygons(NULL,1,attribute);
                        ed_scene->calculatenormals(ofAFFECTEDBYSELECTION,0xFFFFFFFF,0xFFFFFFFF);                      
                        scene->selectelements(etVERTEX,bOR,ofHASSELECTEDVERTICES,naSELECTED);
                        if (!(T3Dcopyattributes & STRNSkeepoldpolys))
                        {
                            scene->removepolygons(ofHASSELECTEDPOLYGONS,paDELETEME);
                            //should only remove 'standalone' vertices
                            scene->removestrandedverticesandedges(ofHASSELECTEDPOLYGONS);
                        }                          
                        
                     }
                 switch (T3Dselectionfunction)
                 {                  
                    case stSTRETCH:
                        ed_boundcode=0;
                        bounds=obj->bounds;
                        halfx=bounds.a.x+bounds.halfx();
                        halfy=bounds.a.y+bounds.halfy();
                                              
                        if (m_chkrect(bounds.a.x,bounds.a.y,bounds.a.x+bndctrlsize,bounds.a.y+bndctrlsize,m)) ed_boundcode=above_left; else
                        if (m_chkrect(bounds.b.x-bndctrlsize,bounds.a.y,bounds.b.x,bounds.a.y+bndctrlsize,m)) ed_boundcode=above_right; else
                        if (m_chkrect(bounds.a.x,bounds.b.y-bndctrlsize,bounds.a.x+bndctrlsize,bounds.b.y,m)) ed_boundcode=bellow_left; else
                        if (m_chkrect(bounds.b.x-bndctrlsize,bounds.b.y-bndctrlsize,bounds.b.x,bounds.b.y,m)) ed_boundcode=bellow_right; else              
                        if (m_chkrect(halfx-bndindent,bounds.a.y,halfx+bndindent,bounds.a.y+bndctrlsize,m)) ed_boundcode=above; else 
                        if (m_chkrect(halfx-bndindent,bounds.b.y-bndctrlsize,halfx+bndindent,bounds.b.y,m)) ed_boundcode=bellow; else             
                        if (m_chkrect(bounds.a.x,halfy-bndindent,bounds.a.x+bndctrlsize,halfy+bndindent,m)) ed_boundcode=left; else              
                        if (m_chkrect(bounds.b.x-bndctrlsize,halfy-bndindent,bounds.b.x,halfy+bndindent,m)) ed_boundcode=right; else                             
                        if (bounds.rcontains(m)) ed_boundcode=middle;//move                                                          
              
                        if (ed_boundcode)
                        {                                                                                       
                            setselectdrawmode(obj);//RDLselectedvertices
                            noelementsselected=FALSE;
                            startmodal(modal_resizebounds);
                            scene->copyoldselectedvertices();
                            ed_addmin=ed_addmax=0;
                            ed_boundoldmin=((tboundingbox3d*)obj)->min;
                            ed_boundoldmax=((tboundingbox3d*)obj)->max;                  
                        }
                        break;
                  case stROTATE:
                      T3Dboundingboxrotation=0.0f;
                      scene->getbounds(viewcam,scene->cursor,0,0);
                      T3D2drotationpivot=scene->cursor->p2d+tpointflt(a.x,a.y);
                      T3D2drotationpivot.x-=(SC.size.x >>1 );
                      T3D2drotationpivot.y-=(SC.size.y >>1 );

                      T3D2drotationpivot.x+=((b.x-a.x) >>1 );
                      T3D2drotationpivot.y+=((b.y-a.y) >>1 );
                      

                      
                      setselectdrawmode(obj);//RDLselectedvertices
                      noelementsselected=FALSE;
                      startmodal(modal_rotatebounds);
                      scene->copyoldselectedvertices();                          
                     
                      break;                                        
                 }
             }
             break;
          }
        }                
        obj=(tobject3d*)obj->next;
    }
    if (noelementsselected)
    {
        ed_tempbounds.a.x=mp.x-a.x;
        ed_tempbounds.a.y=mp.y-a.y;        
        startmodal(modal_drawingselectbox);   
    }
    
    //if (obj->child)
    //    handleselect(event,(tobject3d*)obj->child);
    return (void*)ed_modalfunc;
}


                      
void edit3d::handleevent(tevent *event)
{         
    tview::handleevent(event);
    UINT8 oldscandown=ed_scandown;
    ed_scandown=(*scan & (SMleftctrlkey+SMleftshiftkey));
    UINT32 what=event->what;
    //do modal
    if (state & SFmodal)
    {
        UINT8 oldcalcmode=scene->calc3dmode;
        scene->calc3dmode=CMdrawall;               
        if (ed_scandown)
        {
            //this section controls the zooming and panning behaviour; should work regardless of the current mode
            //so that the user can pan and scale whilst creating objects or manipulating the scene
            if (ms_moved)
            {                            
                if (*scan & SMleftctrlkey)
                {                   
                    ed_oldscale=T3Dscale;
                    ed_cx=viewcam->p.x/T3Dscale;
                    ed_cy=viewcam->p.y/T3Dscale;               
                    T3Dscale+=(mdiff.y*0.005)*(MAX(T3Dscale,0.005));if (T3Dscale<=0.00001f) T3Dscale=0.00001f;               
                    setoffsets((ed_cx*T3Dscale),(ed_cy*T3Dscale));
                    evcallback(CMDscenealteredorth);                   
                }else
                if (*scan & SMleftshiftkey)
                {                   
                    changeoffsets(mdiff.x,mdiff.y);
                    ed_oldmp+=mdiff;
                    calc3d();                    
                    drawview();                   
                }                        
            }
        }else
        {
           if (ed_modalfunc==NULL)
           {
               endmodal2();               
           }
           else if ((oldcalcmode==CMlayeredselectedonly)&&(oldscandown))//get back to previous state
           {
               setselectdrawmode(scene->singleobj);
           }
           else if ((scene->singleobj!=NULL)&&(oldscandown))
           {               
               sendcommand(CMDundraw,owner,scene->singleobj);
           }
        }
        scene->calc3dmode=oldcalcmode;
        if (ed_modalfunc)
            ed_modalfunc(this,event);                               
    }else
    {
    //SHOULD NOT HAVE THIS HERE-THIS IS A FUDGE AS THE MODE IS NOT BEING RESET CORRECTLY
    scene->calc3dmode=CMdrawall;
    if (what & EVmouse)
    {
        //get creation matrix here
        if ((mb==0)&&(ed_scandown))
        {
            ed_oldmp=mp;                        
            startmodal(NULL);
        }
                
        if (what&EVmousedown)
        {
                        
            if (mb==2)
            {
                setsingleobjectdrawmode(scene->cursor);
                get3dpos(mp.x,mp.y,&ed_p1);
                scene->grid->snapto(&ed_p1);
                scene->cursor->moveto(ed_p1.x,ed_p1.y,ed_p1.z);
                evcallback(CMDscenealteredorth);
                startmodal(modal_cursormove);   
            }
            else if (mb==1)
            {
                get3dpos(mp.x,mp.y,&ed_p1);    
                ed_oldmp=mp;     
                
                T3Dctm=&viewcam->localmattr; 
                switch (scene->drawmode)
                {
                   case CMDselect:
                       //here you should just check around the 4 pixel area of the mouse pointer
                       scene->preprocess_select(&scrdc.viewport,&scrdc.size,viewcam);                       
                       handleselect(event,(tobject3d*)scene->scenegraph.root);                                              
                       break;
                   case CMDcube:                                              
                       ed_obj=new t3dmesh();
                       mkcube(ed_obj,-0.5,-0.5,-0.5,0.5,0.5,0.5);                       
                       dostretch(true);
                       break;
                   case CMDsquare:
                       ed_obj=new t3dmesh();
                       mksquare(ed_obj,-0.5,-0.5,0.5,0.5,0);                       
                       dostretch(true);                                                                                
                       break;
                   case CMDgrid:
                       ed_obj=new t3dmesh();
                       mkgrid(ed_obj,-0.5,-0.5,0.5,0.5,0,NULL,gridvals.xsegs,gridvals.ysegs,T3Dcheckergrid);                       
                       dostretch(true);                                                                                
                       break;
                   case CMDcircle:
                       ed_obj=new t3dmesh();
                       mkcircle(ed_obj,0,0,0,1,1,0,circlevals.segs);                       
                       dostretch(false);                                                                                
                       break;                       
                   case CMDsphere:               
                       ed_obj=new t3dmesh();
                       mksphere(ed_obj,0,0,0,1,1,1,PI,spherevals.xsegs,spherevals.ysegs);                      
                       dostretch(false);
                       break;
                   case CMDselipse:               
                       ed_obj=new t3dmesh();
                       mksuperelipse(ed_obj,0,0,0,1,1,1,PI,selipsevals.xsegs,selipsevals.ysegs,selipsevals.p1/10.0,selipsevals.p2/10.0);                                              
                       dostretch(false);
                       break;                   
                   case CMDcylinder:
                       ed_obj=new t3dmesh();
                       mkcylinder(ed_obj,0,0,-0.5,0.5,1,1,PI,cylindervals.xsegs);                       
                       dostretch(false);                                                                                
                       break;
                   case CMDcone:
                       ed_obj=new t3dmesh();
                       mkcone(ed_obj,0,0,-0.5,0.5,1,1,PI,conevals.xsegs);                       
                       dostretch(false);                                                                                
                       break;
                   case CMDpolygon:
                       ed_obj=new t3dmesh(); //to be deleted if polygon grabs another vertex on an object                       
                       startpolygon();
                       break;
                }                
            }
        }
        
        event->what=0;
    }else
    if (what==EVcommand)
    switch (event->command)
    {
        case CMDxvalchanged:
             viewcam->p.x=-*(INT32*)event->infoptr;
             calc3d();
             drawview();
             break;
        case CMDyvalchanged:                     
             viewcam->p.y=-*(INT32*)event->infoptr;
             calc3d();
             drawview();
             break;
    }
    }
}


edit3d::edit3d(INT16 x,INT16 y,INT16 x2,INT16 y2,teditscene *ltscene,UINT8 axis_,char *scrdata):
basic3dview(x,y,x2,y2,ltscene,scrdata)
{
    viewcam= new tcamera();    
    viewcam->camtype=axis_;
    viewcam->p.moveto(0,0,0);
    
    switch (axis_)
    {
        case camORTHXY:viewcam->rotate(0,0,0);break;
        case camORTHZY:LMyrotation(&viewcam->localmattr,HALFPI);break;        
        case camORTHXZ:LMxrotation(&viewcam->localmattr,-HALFPI);break;
    }    
    calc3d();
}

edit3d::~edit3d()
{
    delete viewcam;
}

void edit3d::startmodal(void (*modalfunc)(edit3d *ed,tevent *event))
{
    setmodal(this);
    ed_modalfunc=modalfunc;
    ed_scene=scene;    
}

void edit3d::endmodal2()
{
    endmodal();
    ed_modalfunc=NULL;
    scene->calc3dmode=CMdrawall;
}

void setviewport()
{
    SC.viewport.a.x=float2int(T3Dviewport.a.x);
    SC.viewport.a.y=float2int(T3Dviewport.a.y);
    SC.viewport.b.x=float2int(T3Dviewport.b.x);
    SC.viewport.b.y=float2int(T3Dviewport.b.y);
    SC.viewport.rintersect(SC.size.x,SC.size.y);
}


void edit3d::calc3d()
{
    
    t_fillcol=bg;
    t_col=fg;
    if (scrdc.scrptr==NULL) return;
    tobject3d *obj=scene->singleobj;
    setoutput(&scrdc);
    SET(T3Dviewport,SC.viewport);
    //set standard element masks
    T3Dresetelementmasks();
    UINT32 calc3dmode=scene->calc3dmode;
    UINT32 redrawlevel=scene->redrawlevel;

    switch (calc3dmode)
    {
        case CMdrawallclip:
          if (!obj) goto exitgracefully;
          ed_sobjoldbounds=*scene->getbounds(viewcam,obj);//
          T3Dviewport.rintersect(ed_sobjoldbounds);
          setviewport();
        case CMdrawall:
          bar(0,0,SC.size.x,SC.size.y);
          //cleardevice();
          scene->preprocess(viewcam);
          if ((T3Dobjecttypemask & otGRID)&&(scene->grid))
             scene->grid->drawgrid();          
          scene->renderwire();
          scene->renderhandles(0,viewcam);

          break;
        case CMlayeredsingleobjclip:
          ed_sobjoldbounds=*scene->getbounds(viewcam,obj,a.x,a.y);//
          T3Dviewport.rintersect(ed_sobjoldbounds);                    
        case CMlayeredsingleobj:        
          //sobjoldbounds=obj->bounds;
          setviewport();
          scene->preprocess_singleobject(viewcam,obj,a.x,a.y); //drawing is done in draw at the moment.. probably going to use another scratch image to get rid of the flicker
                    
          break;
        case CMsingleobj:
          if (!obj) goto exitgracefully;
          setviewport();
          scene->preprocess_singleobject(viewcam,obj);
          scene->renderwire();
          obj->renderhandlesfull(0,viewcam);
          break;
        case CMinvselectedonly:
          bar(0,0,SC.size.x,SC.size.y);
        case CMselectedonly:          
        case CMlayeredselectedonly:
          //setup so that it only draws selected vertices and the polygons that reference them
          if (calc3dmode!=CMinvselectedonly)
          {
              T3Dobjecttypemask=otBOUNDINGBOX+otGEOM;
              T3Dobjectprocessmask=T3Dredrawmasks[redrawlevel].object;          
              T3Dobjectprocessmaskresult=T3Dredrawmasks[redrawlevel].object;          
              T3Dpolygonprocessmaskresult=T3Dredrawmasks[redrawlevel].polygon;
              T3Dvertexprocessmaskresult=T3Dredrawmasks[redrawlevel].vertex;
          }else
              T3Dobjecttypemask=0xFFFFFFFF^otBOUNDINGBOX;
                     
          T3Dpolygonprocessmask=T3Dredrawmasks[redrawlevel].polygon;          
          T3Dvertexprocessmask=T3Dredrawmasks[redrawlevel].vertex;
          tpoint offset;
          offset.x=offset.y=0;
          if (calc3dmode==CMlayeredselectedonly)
             offset=a;                        
          //cleardevice();
          scene->preprocess(viewcam,offset.x,offset.y);
          if (calc3dmode==CMinvselectedonly)
          if ((T3Dobjecttypemask & otGRID)&&(scene->grid))
             scene->grid->drawgrid();
          
          
          if (scene->calc3dmode!=CMlayeredselectedonly)
          {                          
             scene->renderwire();
             scene->renderhandles(0,viewcam);
          }
          
          break;
    }
exitgracefully:
    T3Dobjectprocessmask=ofENABLED;
    T3Dobjectprocessmaskresult=ofENABLED;    
    T3Dobjecttypemask=0xFFFFFFFF;
    
    setoutput(NULL);
    //halfandsmooth(scr);   
}



void edit3d::draw()
{

    if (scrdc.scrptr==NULL) return;
    tobject3d *obj=scene->singleobj;
    
    switch (scene->calc3dmode)
    {    
        case CMlayeredselectedonly:    
            T3Dobjectprocessmask=T3Dobjectprocessmaskresult=ofAFFECTEDBYSELECTION+ofENABLED;          
            T3Dpolygonprocessmask=T3Dpolygonprocessmaskresult=paHASSELECTEDVERTICES+paENABLED;          
            T3Dvertexprocessmask=T3Dvertexprocessmaskresult=naSELECTED+naVISIBLE+naENABLED;
            break;
        default:
            T3Dobjectprocessmask=T3Dobjectprocessmaskresult=ofENABLED;          
            T3Dpolygonprocessmask=T3Dpolygonprocessmaskresult=paENABLED;          
            T3Dvertexprocessmask=T3Dvertexprocessmaskresult=naENABLED+naVISIBLE;             
    }
    switch (scene->calc3dmode)
    {
       case CMlayeredsingleobj:
       case CMlayeredsingleobjclip:
       case CMlayeredselectedonly:
         //should clip to bounds here
         //ed_sobjoldbounds+=a;
         //SC.viewport.rintersect(ed_sobjoldbounds);
         putbitmap(a.x,a.y,scr);
         scene->renderwire();
         if (scene->calc3dmode!=CMlayeredselectedonly)
             obj->renderhandlesfull(0,viewcam);
         else
             scene->renderhandles(0,viewcam);
         //draw guide circle
         if ((state & SFmodal)&&(ed_modalfunc==modal_objectcreate)&&(ed_xdiff==ed_ydiff))
         {
              t_col=getcolour(255,255,0);
              circle(ed_oldmp.x+ed_xdiff,ed_oldmp.y+ed_ydiff,5);
         }
       break;
       default:
          putbitmap(a.x,a.y,scr);
    }
    //draw selection box, if applicable
    if ((state & SFmodal)&&(ed_modalfunc==modal_drawingselectbox))
    {
        t_col=T3DCOLboundingbox;
        rectangle(a.x+ed_tempbounds.a.x,a.y+ed_tempbounds.a.y,a.x+ed_tempbounds.b.x,a.y+ed_tempbounds.b.y);
    }
    t_fillcol=bg;
    if (a.x+scr->size.x<b.x)
      bar(a.x+scr->size.x,a.y,b.x,b.y);
    if (a.y+scr->size.y<b.y)
      bar(a.x,a.y+scr->size.y,a.x+scr->size.x-1,b.y);
    //reset
    T3Dobjectprocessmask=ofENABLED;
    T3Dobjectprocessmaskresult=ofENABLED;    
    T3Dobjecttypemask=0xFFFFFFFF;
}

void edit3d::changebounds(trect &bounds)
{
    if ((bounds.width()!=width())||(bounds.height()!=height()))
    {
       UINT16 nwidth=bounds.width()+1;
       UINT16 nheight=bounds.height()+1;
        
       if (((nwidth+nwidth)*nheight) <=datasize)         
           scr->resize(nwidth,nheight);
       createcontext(&scrdc,scr);
       calc3d();
    }
    tview::changebounds(bounds);    
}

void edit3d::get2dpos(vector *p3d,vector *p2d)
{
    //build matrix
        
}

void edit3d::filter3dpos(flt x,flt y,flt z,vector *p)
{
    switch (viewcam->camtype)
    {
    case camORTHXY:
      p->moveto(x,y,z);break;
    case camORTHXZ:
      p->moveto(x,z,y);break;
    case camORTHZY:
      p->moveto(z,y,x);break;
    }    
}

void edit3d::get3dpos(INT16 x,INT16 y,vector *p)
{
    flt posx=(x-a.x-halfx()-viewcam->p.x)/T3Dscale;
    flt posy=(y-a.y-halfy()-viewcam->p.y)/T3Dscale;
    vector c=scene->cursor->p;
    switch (viewcam->camtype)
    {
    case camORTHXY:
      p->moveto(posx,posy,c.z);break;
    case camORTHXZ:
      p->moveto(posx,c.y,-posy);break;
    case camORTHZY:
      p->moveto(c.x,posy,-posx);break;
    }                
}

void edit3d::get3dvec(INT16 x,INT16 y,vector *p)
{
    flt posx=x/T3Dscale;
    flt posy=y/T3Dscale;
    
    switch (viewcam->camtype)
    {
    case camORTHXY:
      p->moveto(posx,posy,0.0f);break;
    case camORTHXZ:
      p->moveto(posx,0.0f,-posy);break;
    case camORTHZY:
      p->moveto(0.0f,posy,-posx);break;
    }                
}



//called when inserted; usefull especially for setting the
//slider paramaters on a containers parent
void edit3d::initialupdate()
{
    valuedata valdata;
    valdata.min=-500;valdata.max=500;valdata.val=0;    
    sendcommand(CMDxdeltachanged,owner,&valdata);
    sendcommand(CMDydeltachanged,owner,&valdata);
}

void edit3d::setoffsets(double x,double y)
{
    UINT32 val;    
    viewcam->p.x=x;
    viewcam->p.y=y;
    val=-viewcam->p.x;
    sendcommand(CMDxdeltavalchanged,owner,&val);
    val=-viewcam->p.y;
    sendcommand(CMDydeltavalchanged,owner,&val);        
}

void edit3d::changeoffsets(INT32 x,INT32 y)
{
    UINT32 val;    
    viewcam->p.x+=x;
    viewcam->p.y+=y;
    val=-viewcam->p.x;
    sendcommand(CMDxdeltavalchanged,owner,&val);
    val=-viewcam->p.y;
    sendcommand(CMDydeltavalchanged,owner,&val);        
}


////////////////////////////////////////////////////////////////////////
//view3d
view3d::view3d(INT16 x,INT16 y,INT16 x2,INT16 y2,teditscene *ltscene,char *scrdata):
basic3dview(x,y,x2,y2,ltscene,scrdata)
{
    logmessage((void*)&textcol, "Entered View3d constructor");
    lockcam=0;
    rendertype=0;
    //bg=getcolour(0,0,20);
    
    viewcam= new tcamera();
    viewcam->camtype=cam3D;
    viewcam->moveto(0,0,0);    
    
    calc3d();
    //logmessage((void*)&textcol,"Done");
}

view3d::~view3d()
{
}



void view3d::calc3d()
{
    if (scrdc.scrptr==NULL) return;
    setoutput(&scrdc);
    t_fillcol=bg;
    t_col=fg;
    //char buf[20];
    cleardevice();
        
    if ((scene->cobj)&&(lockcam))
        viewcam->lookat(scene->cobj->p,PI/2);
    T3Dedgeprocessmask=T3Dedgeprocessmaskresult=eaHIGHLIGHTED;             
    scene->preprocess(viewcam);
    T3Dedgeprocessmask=T3Dedgeprocessmaskresult=eaENABLED;             
    t_col=getcolour(255,255,255);
    //getch();
    scene->render();
    scene->renderwire();
    scene->renderhandles(0,viewcam);
    setoutput(NULL);
}    

void view3d::draw()
{
    char buf[10];
    if (scrdc.scrptr==NULL) return;
    tpointflt fa((float)a.x,(float)a.y);
    
    putbitmap(a.x,a.y,scr);
    t_fillcol=bg;
    if (a.x+scr->size.x<b.x)
      bar(a.x+scr->size.x,a.y,b.x,b.y);
    if (a.y+scr->size.y<b.y)
      bar(a.x,a.y+scr->size.y,a.x+scr->size.x,b.y);
    if (scene->cpoly && (scene->cpoly->attribute & paVISIBLE))
    {
        t_col=T3DCOLboundingbox;
        tpolyvertex *vr,*pv,*end;
        tpointflt p,np;
        vr=scene->cpoly->vlist;       
        pv=(end=&vr[scene->cpoly->npoints])-1;                                              
        for (;vr<end;pv=vr++)
        {                        
           p=vr->v->point2d+fa;
           np=pv->v->point2d+fa;
           line(float2int(p.x),float2int(p.y),float2int(np.x),float2int(np.y));            
        }

        itoa((UINT32)scene->cpoly->npoints,buf,10);
        outtextxy(a.x,a.y,buf);
        itoa(T3Dpolycount,buf,10);
        outtextxy(a.x,a.y+20,buf);
        switch (scene->cpoly->checkintegrity())
            {
            case piEDGESDAMAGED:sprintf(buf,"Polygon edges damaged");break;
            case piVERTEXPARENTSDAMAGED:sprintf(buf,"Vertex parent error");break;
            case piMISSINGEDGEREFERENCE:sprintf(buf,"missing edge back-reference");break;
            case piOK:sprintf(buf,"Polygon integrity ok");break;
        }

        outtextxy(a.x,a.y+40,buf);

    }
}

void view3d::changebounds(trect &bounds)
{
    if ((bounds.width()!=width())||(bounds.height()!=height()))
    {
       UINT16 nwidth=bounds.width()+1;
       UINT16 nheight=bounds.height()+1;
        
       if (((nwidth+nwidth)*nheight) <=datasize)         
           scr->resize(nwidth,nheight);
       createcontext(&scrdc,scr);
       calc3d();
    }
    tview::changebounds(bounds);    
}

matrix4x4ptr LMrotatearound(matrix4x4ptr mat,flt rx,flt ry,vector *vec)
{
  matrix4x4 tmpmat;
  matrix4x4 tmpmat2;
  
  LMtranslate(&tmpmat,-vec->x,-vec->y,-vec->z);  
  LMrottrans(&tmpmat2,rx,ry,0,0,0,0);  
  LMconcatenatematrices(&tmpmat2,&tmpmat,mat);  
  return mat;
}


void view3d::handleevent(tevent *event)
{
    static UINT8 scanmodal=0;
    static UINT8 selectmodal=0;
    static vector dist;
    tpolygon *oldpoly;
    matrix4x4 rmat;
    vector moveme;
    vector p,p2;
    static flt theta,fee;
    moveme.x=moveme.y=moveme.z=0;
    tview::handleevent(event);
    char scandown=(*scan & (SMleftctrlkey+SMleftshiftkey));
    tobject3d *obj=scene->cobj;
    if (state & SFmodal)
    {
        if (selectmodal &&(event->what & EVmouse))
        {
           tpoint ap=mp-a;
           ed_tempbounds.rassign(ap.x,ap.y,ap.x+1,ap.y+1);
           //ed_tempbounds.rassign(50,50,scrdc.size.x-20,scrdc.size.y-20);
           scene->preprocess_select(&ed_tempbounds,&scrdc.size,viewcam);
           oldpoly=scene->cpoly;
           if (T3Dpolycount>0)
           {                                      
              scene->cpoly=T3Dvisiblepolygons[T3Dpolycount-1];
              if (scene->cpoly->attribute & paCLIPPEDPOLY)                   
                 scene->cpoly=scene->cpoly->nxt();
              if (scene->cpoly!=oldpoly)
              {
                 scene->cpoly->attribute |= paCURRENT;
                 scene->cpoly->parent()->attribute |=ofCONTAINSCURRENTPOLYGON;
                 ed_scene->selectelements(T3Dselectmode,T3Dselectioneffect,ofVISIBLE+ofENABLED+ofCONTAINSCURRENTPOLYGON,paCURRENT+paENABLED);
                 scene->cpoly->attribute &= ~paCURRENT;
                 scene->cpoly->parent()->attribute &=~ofCONTAINSCURRENTPOLYGON;
                 evcallback(CMDscenealtered);
              }
            }else
            {
                 scene->cpoly=NULL;    
                 if (oldpoly!=scene->cpoly)                                                           
                   drawview();               
            }
        }else
        if (ms_moved)
        {            
            switch (mb)
            {            
            case 1:
                
                if ((viewcam->camtype==cam3D)&&(obj))
                {
                   vector dist=obj->p-viewcam->p;                   
                   float temp=dist.length();;
                   float FOV=viewcam->FOV;                   
                   
                   moveme.x=(temp*mdiff.x)/FOV;
                   moveme.y=(temp*mdiff.y)/FOV;
                   if (!lockcam)
                     moveme.multmatrix(&viewcam->worldmat,&p);
                   else
                     p=moveme;
                   obj->moveto(obj->p.x+p.x,obj->p.y+p.y,obj->p.z+p.z);                   
                   scene->calcselvertexbounds();    
                   evcallback(CMDscenealtered);
                }
                break;
                
            case 2:
                if ((viewcam->camtype==cam3D)&&(obj))
                {                   
                   moveme.z=mdiff.y;
                   if (!lockcam)
                     moveme.multmatrix(&viewcam->worldmat,&p);
                   else
                     p=moveme;
                   obj->moveto(obj->p.x+p.x,obj->p.y+p.y,obj->p.z+p.z);
                   if (obj->attribute&ofHASSELECTEDVERTICES)
                      scene->calcselvertexbounds();                                         
                   evcallback(CMDscenealtered);
                }
                break;
            case 3:if (obj)
                {
                    obj->rotate(obj->o.x+(flt)mdiff.y/100.0f,obj->o.y+(flt)mdiff.x/100.0f,obj->o.z);                    
                    scene->calcselvertexbounds();                       
                    evcallback(CMDscenealtered);
                }
                break;
            case 0:                
                if (scandown)
                {
                   
                   if ((*scan & (SMleftctrlkey + SMleftshiftkey))==(SMleftctrlkey + SMleftshiftkey))
                   {
                       p=viewcam->p-thescene->cursor->p;
                       flt r=p.length();
                       dist=vector(0,0,-r);
                                                                                       
                       fee=viewcam->o.x-mdiff.y/50.0f;                       
                       theta=viewcam->o.y-mdiff.x/50.0f;
                       //as dist is only facing in the z axis, these two lines could
                       //be made far simpler                     
                       LMrotatearound(&rmat,fee,theta,0,&thescene->cursor->p);//theta
                       dist.multmatrix(&rmat,&viewcam->p);
                       viewcam->p+=thescene->cursor->p;
                       viewcam->rotate(fee,theta,0);                                                   
                   }else
                   {
                       if (*scan & SMleftctrlkey)
                       {                   
                           moveme.z=mdiff.y;                                            
                       }else
                       if (*scan & SMleftshiftkey)
                       {
                           moveme.x=-mdiff.x;
                           moveme.y=-mdiff.y;                      
                       }

                       moveme.multmatrix(&viewcam->localmat,&p);
                       viewcam->moveto(viewcam->p.x+p.x,viewcam->p.y+p.y,viewcam->p.z+p.z);                       
                   }
                   evcallback(CMDscenealtered);
                }                break;
            }
            
        }
        if (scanmodal)
        {           
           if (!scandown)
           {
               endmodal();
               resetdrawmode(CMsingleobj);
           }
        }else                          
        if ((ms_released))
        {
        
            endmodal();            
            resetdrawmode(selectmodal?CMselectedonly:CMsingleobj);
        }
        
            
    }else
    {        
        if ((event->what&EVmousedown)||(scandown))
        {
            scanmodal=scandown;
            selectmodal=0;
            if (!scanmodal)
            {
              if (scene->cpoly)
              {
                  setselectdrawmode(scene->slbox);//RDLselectedvertices
                  selectmodal=1;
                  scene->cpoly=NULL;
              }
              else 
                  setsingleobjectdrawmode(scene->cobj);                                
            }
            else
            {              
              setsingleobjectdrawmode(viewcam);
            }            
            setmodal(this);
        }
        else if (ms_moved)
        {     
            if (T3Dselectmode==etPOLYGON)
            {
               tpoint ap=mp-a;
               ed_tempbounds.rassign(ap.x,ap.y,ap.x+1,ap.y+1);
               //ed_tempbounds.rassign(50,50,scrdc.size.x-20,scrdc.size.y-20);
               scene->preprocess_select(&ed_tempbounds,&scrdc.size,viewcam);
               oldpoly=scene->cpoly;
               if (T3Dpolycount>0)
               {                                      
                   scene->cpoly=T3Dvisiblepolygons[T3Dpolycount-1];
                   if (scene->cpoly->attribute & paCLIPPEDPOLY)                   
                       scene->cpoly=scene->cpoly->nxt();
               }else
                 scene->cpoly=NULL;    
               if (oldpoly!=scene->cpoly)                                                           
                 drawview();                                                                                            
            }
        }
        event->what=0;
    }
}

//called when inserted; usefull especially for setting the
//slider paramaters on a containers parent
void view3d::initialupdate()
{
    valuedata valdata;
    valdata.min=0;valdata.max=100;valdata.val=0;    
    sendcommand(CMDxdeltachanged,owner,&valdata);
}




////////////////////////////////////////////////////////////////////////
//scrollmaterialview : Multiple renders in a single control; only 3 visible at a
//time         

#define SVCMselected 1
#define SVCMallvisible 2

#define SVDMselected 1
#define SVDMallvisible 2

#define svwidth 100
#define svheight 98

scrollmaterialview::scrollmaterialview(INT16 x,INT16 y,teditscene *ltscene):tview(x,y,x+100,y+200)
{
    INT32 w=svwidth-4 ;
    INT32 h=svheight-2 ;
    INT32 offs=w << 1;
    bg=getcolour(20,80,120);
    fg=getcolour(180,220,255);    
    //logmessage((void*)&textcol, "about to allocate screen data");
    for (int i=0;i<3;i++)
    {       
       scr[i]=new tbitmap(w,h,BMflat16,normalput,BMFdoublewordalign);
       createcontext(&scrdc[i],scr[i]);
    }
    viewcam= new tcamera();
    viewcam->camtype=cam3D;
    viewcam->moveto(0,0,-130);
    offset=sel=idxoffset=0;
    sel=T3Dmaterialbank.getindex((telem*)T3Dmaterial);//get current index.
    maxopts=T3Dmaterialbank.count;
    
    scene=ltscene;          
    logmessage((void*)&textcol,"created scroll3d view");
    getch();
    //drawmode=SVDMallvisible;
    
}

void scrollmaterialview::calcinternalvars()
{
    valuedata valdata;
    maxopts=T3Dmaterialbank.count;    
    INT16 temp=(svheight*maxopts) - height();
    valdata.min=0;valdata.max=temp;valdata.val=offset;    
    sendcommand(CMDydeltachanged,owner,&valdata);    
}

void scrollmaterialview::changebounds(trect &bounds)
{
    tview::changebounds(bounds);
    calcinternalvars();
}

void scrollmaterialview::setoffset(INT32 offset_)
{
    if (offset!=offset_)
    {       
       INT32 off;              
       INT32 oldidxoffset=idxoffset;
       INT32 oldoffset=offset;       
       offset=offset_;
       idxoffset=offset/svheight;
       if (offset>oldoffset)
       {
           oldidxoffset++;
           off=a.y+(oldidxoffset*svheight)-oldoffset;              
           while (off<b.y)
           {
              off+=svheight;
              oldidxoffset++;
           }
           while (calc3dview(oldidxoffset++));
       }else if (offset<oldoffset)
       {
           oldidxoffset--;
           off=a.y+(svheight+(oldidxoffset*svheight))-oldoffset;              
           while (off>a.y)
           {
              off-=svheight;
              oldidxoffset--;
           }           
           while (calc3dview(oldidxoffset--));           
       }
           
       //do calculations for newly visible views
       drawview();
    }
    
}

void scrollmaterialview::draw()
{

    INT16 idx=idxoffset;
    INT16 ypos=a.y- (offset % svheight);
    for (int i=0;i<3;i++)
    {
        t_fillcol=(idx==sel)?selectcol:windowcols[8];
        bar(a.x,ypos+2,a.x+1,ypos+svheight-1);
        bar(b.x-1,ypos+2,b.x,ypos+svheight-1);
             
        t_fillcol=((!idx && !sel)||((idx-1)==sel)||((idx)==sel))?selectcol:windowcols[8];
        bar(a.x,ypos,b.x,ypos+1);
        //bar(a.x,ypos+svheight,b.x,ypos+svheight+2);
        
        
        //thickrectangle(a.x,ypos,b.x,ypos+svheight,2);
        putbitmap(a.x+2,ypos+2,scr[idx % 3]);
        idx++;
        ypos+=svheight;
    }
}

void scrollmaterialview::calc3d(UINT8 calcmask)
{
    UINT8 idx=offset/svheight;
    if (calcmask==SVCMselected)
      calc3dview(sel);
    else
    {       
      calc3dview(idx+0);
      calc3dview(idx+1);
      calc3dview(idx+2);
    }
}

bool scrollmaterialview::calc3dview(UINT8 idx)
{
    char buf[6];
    INT32 off=a.y+(idx*svheight)-offset;    
    if ((off+svheight>a.y)&&(off<(b.y)))
    {
        material *oldmat=T3Dmaterial;
        T3Dmaterial=(material *)T3Dmaterialbank.findelement(idx);
    
        //tsound(200);
        itoa(idx,buf,10);
        idx %=3;
        if (scrdc[idx].scrptr==NULL) return false;
        t3dmesh *obj=(t3dmesh*)scene->cobj;
    
        setoutput(&scrdc[idx]);
        t_fillcol=bg;
        t_col=fg;
        colourgrid2(0,0,25,25,5,5);
        scene->preprocess(viewcam);
        scene->render();
        t_col=getcolour(255,255,255);
        outtextxy(2,2,buf);

        setoutput(NULL);
        
        T3Dmaterial=oldmat;
        //tsoundoff();
        return true;
    }else
        return false;

}

void scrollmaterialview::handleevent(tevent *event)
{
    //static 
    tview::handleevent(event);
    tobject3d *obj=scene->cobj;
    if (state & SFmodal)
    {                      
        if ((ms_moved)&&(mb==2)&&(obj))
        {                                    
            obj->rotate(obj->o.x+(flt)mdiff.y/100.0f,obj->o.y+(flt)mdiff.x/100.0f,obj->o.z);
            calc3d(SVCMselected);
            drawview();               
        }                 
        if (ms_released)        
            endmodal();                       
    }else
    {        
        if (event->what&EVmousedown)
        {
            
                UINT32 oldsel=sel;
                sel=((offset+(mp.y-a.y))/svheight);
                if (oldsel!=sel)
                {
                   T3Dmaterial=(material*)T3Dmaterialbank.findelement(sel);
                   evcallback(CMDmaterialchanged);
                   drawview();
                }
            
                setmodal(this);
            event->what=0;
        }else if (event->what == EVcommand)
        {
          switch (event->command)
          {          
          case CMDyvalchanged:
              
              setoffset(*(UINT32*)event->infoptr);
              
              break;                
          }          
        }
    }
    
}

////////////////////////////////////////////////////////////////////////////////////
//Preferences

#define bheight 16
class preferences:public tgroup
{
    public:
    trgba editcolourrgb;
    bordercontainer *colourcontainer;
    colourselector *editcolourselector;
    
    tabbox *tabs;
    tcolour buttonpalette[16];
    tbutton *fogtoggle,*ok,*cancel,*fogcolour;
    numbox *nearfog;
    numbox *farfog;
    preferences(INT16 x,INT16 y);
    virtual void handleevent(tevent *event);
    void buildbuttonpalette(UINT8 r,UINT8 g,UINT8 b);
};

void preferences::buildbuttonpalette(UINT8 r,UINT8 g,UINT8 b)
{   
   COLgrad(buttonpalette,0,8,trgba(0,0,0,0),trgba(r,g,b,0));
   COLgrad(buttonpalette,8,14,trgba(r,g,b,0),trgba(255,255,255,0));
}   

preferences::preferences(INT16 x,INT16 y):tgroup(x,y,x+320,y+200)
{
    
    tabs=new tabbox(a.x+5,a.y+5,b.x-5,b.y-35);
    container *sceneprefs=new container();   
    editcolourselector=new colourselector(0,0,&editcolourrgb);
    colourcontainer=new bordercontainer(0,0,editcolourselector);
    
    TWINdefaultgrowmode=TGdragy1+TGdragx1;
    UINT16 flags=BAnooffset|BAycentre|BAnobox;
    
    tbutton *camlocktoggle= new tbutton(200,10,100,bheight,NULL,0,BMtoggleaction,CMDlockcam,13,flags |(T3Dsceneattributes & sfFOG),"lock camera",b_toggle0,b_toggle0,b_toggle1);
    fogtoggle= new tbutton(5,10,100,bheight,NULL,0,BMtoggleaction,CMDtogfog,13,flags |(T3Dsceneattributes & sfFOG),"Fog Enable",b_toggle0,b_toggle0,b_toggle1);
    nearfog= new numbox(5,35,45,CMDvalchanged,0,200000,0);
    farfog= new numbox(5,70,45,CMDvalchanged,0,200000,1000);    
    fogcolour=new tbutton(5,100,40,20,buttonpalette,1,BMtoggleaction,CMDeditfogcolour);
    TWINdefaultgrowmode=TGdragy2+TGdragx2;
    ok=new tbutton(b.x-60,b.y-30,45,25,windowcols,1,BMreleaseaction,0,13,BAxcentre+BAycentre,"Ok");
    cancel=new tbutton(b.x-60,b.y-30,45,25,windowcols,1,BMreleaseaction,0,13,BAxcentre+BAycentre,"Ok");
    sceneprefs->insertrel(fogtoggle);
    sceneprefs->insertrel(camlocktoggle);
    
    sceneprefs->insertrel(nearfog);
    sceneprefs->insertrel(farfog);
    sceneprefs->insertrel(fogcolour);
    sceneprefs->addbackview();
    tabs->addtabcontrol(sceneprefs,"Scene");
    insert(new backview(a.x,a.y,b.x,b.y,windowcols[8]));
    insert(tabs);
    insert(ok);
    insert(cancel);
    buildbuttonpalette(thescene->fogcolour.r*255,thescene->fogcolour.g*255,thescene->fogcolour.b*255);
    TWINdefaultgrowmode=TGgrowall;    
}

void preferences::handleevent(tevent *event)
{
    tgroup::handleevent(event);
    bool redrawscene=false;
    if (event->what==EVcommand)
    {
        switch (event->command)
        {

        case CMDvalchanged:
          thescene->setfog(nearfog->val,farfog->val);
          //check for overlap
          redrawscene=true;
          
          break;
        case CMDeditfogcolour:          
          editcolourselector->mute();
          editcolourselector->setcolour(thescene->fogcolour.r*255,thescene->fogcolour.g*255,thescene->fogcolour.b*255);
          editcolourselector->unmute();
          colourcontainer->tmoveto(fogcolour->b.x,fogcolour->a.y);
          root->insert(colourcontainer);
          colourcontainer->target=this;
          colourcontainer->drawview();
          setmodal(colourcontainer);                             
          break;
        case CMDcolourschanged:
          editcolourrgb=((colourselector*)event->infoptr)->col;
          buildbuttonpalette(editcolourrgb.r,editcolourrgb.g,editcolourrgb.b);
          fogcolour->drawview();
          view->renderview->bg=getcolour(editcolourrgb.r,editcolourrgb.g,editcolourrgb.b);
          thescene->fogcolour=editcolourrgb;
          redrawscene=true;
          break;            
        case CMDtogfog:
          T3Dsceneattributes ^= sfFOG;
          redrawscene=true;
          break;
        case CMDcontainermodalended:
          colourcontainer->remove();
          if (fogcolour->isdown())
             fogcolour->setup();
          break;
        case CMDlockcam:
          view->renderview->lockcam^=1;break;       
        }
        
    }
    if (redrawscene)
    {
        view->renderview->calc3d();
        view->renderview->drawview();
    }       
}

////////////////////////////////////////////////////////////////////////////////////
//Material edit

materialedit::materialedit(INT16 x1,INT16 y1,INT16 x2,INT16 y2,material *mat_):tgroup(x1,y1,x2,y2)
{
    
    scene=new tscene();
    scene->setfog(3000,3000);
    editcolour=0;
    l=new tlight();
    l->rotate(-HALFPI,-HALFPI,-HALFPI);
    l->o.normalize();
    //l->o=vector(-1500,-1000,-500);
    
    l->ambient=trgbfloat(0.10,0.10,0.10);
    l->diffuse=trgbfloat(0.7,0.7,0.7);
    l->specular=trgbfloat(1.0,1.0,1.0);
    
    editcolourselector=new colourselector(0,0,&editcolourrgb);
    colourcontainer=new bordercontainer(0,0,editcolourselector);
    
    TWINdefaultgrowmode=TGdragy1+TGdragx1;
    
    mat=mat_;
    T3Dmaterial=NULL;
    T3Dtexture=NULL;
    T3Ddefaultpolyattributes=paENABLED;//paSHADED;//|paTRANSPARENT;
    objcount=0;
    T3Dpolycreationflags|=cfWRAPTEXTURE;
    scene->cobj=obj=new t3dmesh;    
    mksphere(obj,0,0,0,10,10,10,PI,30,30);
    //logmessage((void*)&textcol,"created sphere");
    obj->calculatenormals();         
    T3Dmaterial=mat;
    scene->addobject(NULL,obj);
    scene->addobject(NULL,l);
        
    obj->moveto(0,0,0);
    
    logmessage((void*)&textcol,"Creating viewer window");
    
    renderview=new scrollmaterialview(0,0,(teditscene*)scene);
    renderscroll=new scrollcontainer(10,10,109+sb_btn.x,200,renderview,CFybar);

    getch();
    //logmessage((void*)&textcol,"Done");    
    //an error was occuring here, unknown reason

    tabs=new tabbox(a.x+140,a.y+10,b.x-3,a.y+200);    
    container *matc=new container(1);
    container *texc=new container(1);
    container *attribc=new container();

    textures=new dropbox(10,120,10,NULL,0);
    createtexturelist();
    
    UINT16 ypos=0;
    int i;
    trgbfloat *col=&mat->ambient;
    for (i=0;i<4;i++)
    {
       buildbuttonpalette(i,col->r*255.0f,col->g*255.0f,col->b*255.0f,false);
       
       colours[i]=new tbutton(5,ypos,40,20,&buttonpalette[i<<4],1,BMtoggleaction,CMDeditambient+i);
       matc->insertrel(colours[i]);
       ypos+=35;
       col++;
    }
    ypos=0;
    transparency=new numbox(65,ypos+=35,45,CMDvalchanged,0,255,0);        
    specpower=new numbox(65,ypos+=35,45,CMDvalchanged,0,128,0);    
    specstr=new numbox(65,ypos+=35,45,CMDvalchanged,0,255,0);    
    ypos=4;    
    UINT16 flags=BAnooffset|BAycentre|BAnobox;
    togsmooth = new tbutton(10,ypos,200,bheight,NULL,0,BMtoggleaction,CMDtogsmooth,13,flags|(T3Ddefaultpolyattributes & paSHADED),"Smooth shaded",b_toggle0,b_toggle0,b_toggle1);    
    togenvmap = new tbutton(10,ypos+=bheight,200,bheight,NULL,0,BMtoggleaction,CMDtogenvmap,13,flags|(T3Ddefaultpolyattributes & paENVMAP),"Environment Mapped UV",b_toggle0,b_toggle0,b_toggle1);    
    togdouble = new tbutton(10,ypos+=bheight,200,bheight,NULL,0,BMtoggleaction,CMDtogdouble,13,flags|(T3Ddefaultpolyattributes & paDOUBLESIDED),"Double sided",b_toggle0,b_toggle0,b_toggle1);    
    togtransparent= new tbutton(10,ypos+=bheight,200,bheight,NULL,0,BMtoggleaction,CMDtogtransparency,13,flags|(T3Ddefaultpolyattributes & paTRANSPARENT),"Transparency",b_toggle0,b_toggle0,b_toggle1);    
    tognospectrans = new tbutton(10,ypos+=bheight,200,bheight,NULL,0,BMtoggleaction,CMDtognospectrans,13,flags|(T3Ddefaultpolyattributes & paNOSPECTRANS),"Transparent hightlights",b_toggle0,b_toggle0,b_toggle1);    
    toginterpolate = new tbutton(10,ypos+=bheight,200,bheight,NULL,0,BMtoggleaction,CMDtoginterpolate,13,flags|(T3Ddefaultpolyattributes & paTEXTINTERPOLATED),"Interpolated Textures",b_toggle0,b_toggle0,b_toggle1);
    togwireframe = new tbutton(10,ypos+=bheight,200,bheight,NULL,0,BMtoggleaction,CMDtogwireframe,13,flags|(T3Ddefaultpolyattributes & paOUTLINED),"Wire frame",b_toggle0,b_toggle0,b_toggle1);
    togtruecolour = new tbutton(10,ypos+=bheight,200,bheight,NULL,0,BMtoggleaction,CMDtogtruecolour,13,flags|(T3Ddefaultpolyattributes & paTRUECOLOUR),"Solid colour",b_toggle0,b_toggle0,b_toggle1);
    applymaterial=new tbutton(170,135,60,20,windowcols,0,BMreleaseaction,CMDapplymaterialtoselectedpolygons,13,BAycentre|BAxcentre,"Apply");
    applymaterial->target=this;
    applytexture=new tbutton(170,135,60,20,windowcols,0,BMreleaseaction,CMDapplytexturetoselectedpolygons,13,BAycentre|BAxcentre,"Apply");
    applytexture->target=this;
    applyattribute=new tbutton(170,135,60,20,windowcols,0,BMreleaseaction,CMDapplyattributetoselectedpolygons,13,BAycentre|BAxcentre,"Apply");
    applyattribute->target=this;
    //applymaterial=new tbutton(100,130,60,20,NULL,0,BMclickaction,CMDapplymaterialtoselectedpolygons);  ,                
    insert(new backview(x1,y1,x2,y2,windowcols[8]));
    insertrel(renderscroll);
    objecttypes=new ttoolbar(a.y+10,renderscroll->b.y+2,(24*4)-1,23,toolbm2,8,24,24);
    objecttypes->addbutton(TBAglobaltoggle ,1,CMDchangeobjectsphere);
    objecttypes->addbutton(TBAglobaltoggle ,3,CMDchangeobjectcube);
    objecttypes->addbutton(TBAglobaltoggle ,6,CMDchangeobjectcylinder);
    objecttypes->addbutton(TBAglobaltoggle ,5,CMDchangeobjectcone);    
    
    insert(objecttypes);
    insert(tabs);
    transparency->setval(mat->transparency * 255);
    specpower->setval(mat->specpower);
    specstr->setval(mat->st*255);                
    
    matc->insertrel(transparency);
    matc->insertrel(specpower);
    matc->insertrel(specstr);
    matc->insertrel(applymaterial);
       
    texc->insertrel(textures);
    texc->insertrel(applytexture);
    texc->insertrel(new tbutton(170,105,60,20,windowcols,0,BMreleaseaction,CMDloadtexturedialog,13,BAycentre|BAxcentre,"Load"));
    
    attribc->insertrel(togsmooth);
    attribc->insertrel(togenvmap);
    attribc->insertrel(togdouble);
    attribc->insertrel(togtransparent);
    attribc->insertrel(tognospectrans);
    attribc->insertrel(toginterpolate);
    attribc->insertrel(togwireframe);
    attribc->insertrel(togtruecolour);
    attribc->insertrel(applyattribute);
    attribc->addbackview();
    
    tabs->addtabcontrol(matc,"Material");
    tabs->addtabcontrol(texc,"Texture");
    tabs->addtabcontrol(attribc,"Attribute");
    renderview->calc3d(SVCMallvisible );

    TWINdefaultgrowmode=TGgrowall;
    
}

materialedit::~materialedit()
{
    delete scene;
    delete texturelist;
}

void materialedit::buildbuttonpalette(UINT8 ref,UINT8 r,UINT8 g,UINT8 b,bool setmat)
{
   UINT8 start=ref <<4;
   COLgrad(buttonpalette,start,start+8,trgba(0,0,0,0),trgba(r,g,b,0));
   COLgrad(buttonpalette,start+8,start+14,trgba(r,g,b,0),trgba(255,255,255,0));
   /*for (int i=start;i<start+15;i++)
     buttonpalette[i]=getcolour(r,g,b);*/
   if (setmat)
   {
      trgbfloat *col=&mat->ambient;
      col[ref]=trgbfloat(r/255.0f,g/255.0f,b/255.0f);      
   }
}



void materialedit::createtexturelist()
{
    UINT32 i;
    textures->list->makelist(T3Dtexturebank.count+1,NULL,0);
    textureref *texref=(textureref*)T3Dtexturebank.first;
    textures->list->setitem(0,"<none>",0,0,0,0);
    i=1;
    while (texref!=NULL)
    {
        textures->list->setitem(i++,texref->name,0,0,0,0);
        texref=texref->nextt();
    }
    textures->list->drawview();
}

void materialedit::calc3d(UINT8 mode)
{
    if (renderview)
    {
         renderview->calc3d(mode);
         renderview->drawview();
    }
}        

void materialedit::handleevent(tevent *event)
{
    trgbfloat *col;
    int i;
    bool updatescene=false;
    tgroup::handleevent(event);
    if ((state & SFdeaf)==0)    
    if (event->what & EVcommand)
    {
        switch (event->command)
        {            
            case CMDselectedchanged:
               T3Dsettexture(T3Dgettexture(textures->list->sel));
               obj->settexture(T3Dtexture,1);
               calc3d(SVCMallvisible);
            break;
            case CMDmaterialchanged:
              deafen();
              mat=T3Dmaterial;
              col=&mat->ambient;
              for (i=0;i<4;i++)
              {
                 buildbuttonpalette(i,col->r*255.0f,col->g*255.0f,col->b*255.0f,false);
                 colours[i]->drawview();
                 col++;
              }              
              transparency->setval(mat->transparency * 255);
              specpower->setval(mat->specpower);
              specstr->setval(mat->st*255);
              undeafen();
              break;
            case CMDeditambient:                              
            case CMDeditdiffuse:               
            case CMDeditspecular:               
            case CMDeditemmisive:                              
               editcolour=event->command-CMDeditambient;
               if (colours[editcolour]->isdown())
               {
                   col=&mat->ambient;                              
                   editcolourselector->mute();
                   editcolourselector->setcolour(col[editcolour].r*255,col[editcolour].g*255,col[editcolour].b*255);
                   editcolourselector->unmute();
                   colourcontainer->tmoveto(colours[editcolour]->b.x,colours[editcolour]->a.y);
                   root->insert(colourcontainer);
                   colourcontainer->target=this;
                   colourcontainer->drawview();
                   setmodal(colourcontainer);
                   for (int i=0;i<4;i++)                   
                       if ((i!=editcolour)&&(colours[i]->isdown()))                       
                           colours[i]->setup();                                                                  
                }
               break;            
            case CMDcontainermodalended:
               colourcontainer->remove();
               if (colours[editcolour]->isdown())
                   colours[editcolour]->setup();               
               break;
            case CMDcolourschanged:
               editcolourrgb=((colourselector*)event->infoptr)->col;
               buildbuttonpalette(editcolour,editcolourrgb.r,editcolourrgb.g,editcolourrgb.b,true);               
               colours[editcolour]->drawview();
               calc3d(SVCMselected);               
               break;
            case CMDnumboxreleased:
            case CMDcolourschangedrelease:
               updatescene=true;
               break;
            case CMDvalchanged:
               mat->specpower=specpower->val;
               mat->transparency=transparency->val/255.0f;
               mat->st=specstr->val/255.0f;
               calc3d(SVCMselected);
            break;
            case CMDapplymaterialtoselectedpolygons:thescene->setsp_material(T3Dmaterial);updatescene=true;  break;
            case CMDapplytexturetoselectedpolygons:thescene->setsp_texture(T3Dtexture);updatescene=true;   break;
            case CMDapplyattributetoselectedpolygons:thescene->setsp_attribute(T3Ddefaultpolyattributes,bPUT);updatescene=true;   break;
            case CMDchangeobjectsphere:
            case CMDchangeobjectcube:
            case CMDchangeobjectcylinder:
            case CMDchangeobjectcone:            
            objcount=event->command-CMDchangeobjectsphere;
            //t3dmesh needs updated remove procedures
            //obj->removevertex(0);
            T3Dmaterial=NULL;
            T3Dsettexture(T3Dgettexture(textures->list->sel));
            switch(objcount)
            {
               case 0:T3Dpolycreationflags|=cfWRAPTEXTURE;mksphere(obj,0,0,0,10,10,10,0,30,30);break;
               case 1:T3Dpolycreationflags&=~cfWRAPTEXTURE;mkcube(obj,-5.5,-5.5,-5.5,5.5,5.5,5.5);break;
               case 2:T3Dpolycreationflags|=cfWRAPTEXTURE;mkcylinder(obj,0,0,-5,5,10,10,0,22);break;
               case 3:T3Dpolycreationflags|=cfWRAPTEXTURE;mkcone(obj,0,0,-5,5,10,10,0,22);break;
            }
            T3Dmaterial=mat;
            obj->calculatenormals();
            calc3d(SVCMallvisible);
            break;            
            
            default:
            if ((event->command>=CMDtogbase)&&(event->command<=CMDtogbase+4096))
            {
                T3Ddefaultpolyattributes^=(event->command-CMDtogbase);
                obj->setpolyattributes(T3Ddefaultpolyattributes,0);
                calc3d(SVCMallvisible);
            }
            break;
            case CMDloadtexturedialog:root->add_and_draw(new window(0,20,380,260,"Load Texture",new fileview(0,0,&loadtexture),CFnoresize),VPcentre,PIDloadtexture);break;                    
        }
        if (updatescene)
        {
            view->renderview->calc3d();
            view->renderview->drawview();
        }                
    }        

}

//extern unsigned _dos_findfirst(const char *__path,unsigned __attr,struct find_t *__buf );
//extern unsigned _dos_findnext( struct find_t *__buf );

/*
struct find_t {
        char reserved[21];      /* reserved for use by DOS    
        char attrib;            /* attribute byte for file    
        unsigned short wr_time; /* time of last write to file 
        unsigned short wr_date; /* date of last write to file
        unsigned long  size;    /* length of file in bytes   
#if defined(__OS2__) || defined(__NT__)
        char name[256];         /* null-terminated filename  
#else
        char name[13];          /* null-terminated filename   
#endif
};
#define _find_t find_t
*/

UINT32 numoffiles(char *path,unsigned attrib)
{
    find_t file;
    UINT32 count=0;
    _dos_findfirst(path,attrib,&file);
    while (_dos_findnext(&file)==0)
    {
        count++;
    }                        
    return count;
}
/*
void inputdir(char *path,char *filter, UINT16 attrfilter,UINT16 *count):stringarray;
DirInfo:SearchRec;
    listelem *info;
    UINT32 maxopts;

  maxopts=numoffiles(path,filter,attrfilter);
  info=(listelem *)malloc((1+maxopts)*sizeof(listelem));

  *count=0;
  if (attrfilter==anyfile) 
    info^[0]:='No files'
  else begin
    info^[0]:='No dirs';
    {info^[count+1]:='[.]';
    inc(count);
    info^[count+1]:='[..]';
    inc(count);}
  end;

  {if dirs then inc(count);}
  {oldcount:=count;}
  FindFirst(path+filter, attrfilter, DirInfo);
  while DosError=0 do begin
    if(dirinfo.attr=attrfilter)or
    ((attrfilter=anyfile)and(dirinfo.attr<>directory))
    and(count<255)  then begin
      if DirInfo.attr=directory then
        info^[count+1]:=DirInfo.Name
      else
        info^[count+1]:=DirInfo.Name;
      inc(count);
    end;
    FindNext(DirInfo);
  end;
  sortarray(info,stringsize+1,maxopts);
  inputdir:=info;
End;


class fileview:public tgroup
{
    char dirname[255];
    treebox *dir;
    listbox *files;
    tbutton *ok,*cancel;
    dropbox *ext;
    edittextbox *name;

    fileview(INT16 x1,INT16 y1,char *dir_,listelem *ext_,UINT16 cancelcommand,UINT16 okcommand);
    ~fileview();
    
    changedir(char *dir_);
};




fileview::fileview(INT16 x1,INT16 y1,char *dir_,listelem *ext_,UINT16 cancelcommand,UINT16 okcommand)
:tgroup(x1,y1,x1+400,y1+300)
{
    dir=new treebox(b.x-130,a.y+10,b.x-20,a.y+14+t_charset.ysize * 8);
        
}*/


leftbar::leftbar(UINT16 x,UINT16 y,UINT16 height):tgroup(x,y,x+95,y+height)
{
    functionbar=new ttoolbar(a.x,a.y+75,(24*4)-1,(24*3)-1,toolbm,13,24,24);
    functionbar->addbutton(TBAtoggle+((thescene->grid->attribute & ofGRIDSNAP)!=0),4,CMDsnapgrid);
    functionbar->addbutton(TBAtoggle+((thescene->grid->attribute & ofVISIBLE)!=0),5,CMDviewgrid);
    functionbar->addbutton(0,6,0);  //undo
    functionbar->addbutton(0,7,0);  //redo
    
    functionbar->addbutton(TBAtoggle ,8,CMDtogonlyshowselectednodes);    
    functionbar->addbutton(TBAglobaltoggle ,9,0);
    functionbar->addbutton(TBAglobaltoggle ,8,0);    
    functionbar->addbutton(0,9,CMDlight1);
    functionbar->addbutton(0,8,CMDlight2);    
    functionbar->addbutton(0,9,CMDlight3);
    functionbar->addbutton(0,8,CMDlight4);    
    functionbar->addbutton(0,9,CMDscreendump);
    
    
    insert(functionbar);
}

void leftbar::handleevent(tevent *event)
{
    tgroup::handleevent(event);
    if (event->what==EVcommand)
    {
        owner->handleevent(event);
    }
}

void leftbar::draw()
{
    tgroup::draw();    
    t_nativepalette=windowcols;
    
    outboxcol(a.x,a.y,b.x,functionbar->a.y-1);
    inboxcol(a.x,functionbar->b.y+1,b.x,b.y);//outbox inbox
    
}



clay::clay(INT16 x,INT16 y,INT16 x2,INT16 y2):tgroup(x,y,x2,y2)
{
    INT16 xpos;
    trect desktop;
    tpoint halfpoint;
    tsize windowsize;
    insert(new backview(96,y,x2,y2,getcolour(5,30,100)));       
    menu=new tmenu(0,0,MAxorient);
    tmenu *filemenu=new tmenu(0,0,MApopup);
    tmenu *editmenu=new tmenu(0,0,MApopup);
    tmenu *windowmenu=new tmenu(0,0,MApopup);
    tmenu *polymenu=new tmenu(0,0,MApopup);
    tmenu *subpolymenu=new tmenu(0,0,MApopup);

    selipsevals.xsegs=selipsevals.ysegs=spherevals.xsegs=spherevals.ysegs=circlevals.segs=conevals.xsegs=cylindervals.xsegs=gridvals.xsegs=gridvals.ysegs=20;   
    selipsevals.p1=selipsevals.p2=1;
    menu->additem("File",0,MIA3dlook,filemenu,NULL,1);
    menu->additem("Edit",0,MIA3dlook,editmenu,NULL,1);
    menu->additem("Selected",0,MIA3dlook,polymenu,NULL,1);
    menu->additem("Window",0,MIA3dlook,windowmenu,NULL,1);
    menu->b.x=SC.size.x-1;
    filemenu->additem("Open..",CMDload3dobjectdialog,0,NULL,NULL,1);
    filemenu->additem("Save",CMDsave3dobjectdialog,0,NULL,NULL,1);
    filemenu->additem("Save as..",0,0,NULL,NULL,2);
    filemenu->additem("Merge..",0,0,NULL,NULL,1);
    filemenu->additem(NULL,0,MIAseperator);
    filemenu->additem("Exit..",CMDleave,0,NULL,NULL,2);
    
    editmenu->additem("Copy",0);
    editmenu->additem("Paste",0);

    windowmenu->additem("Tile",0);
    windowmenu->additem("Cascade",0);
    windowmenu->additem("Close",0);
    windowmenu->additem("Close all",0);
    windowmenu->additem("Previous (F6)",0);
    windowmenu->additem("Next (F7)",0);

    subpolymenu->additem("Double sided",0);
    subpolymenu->additem("Transparency",0);
    subpolymenu->additem("Outlined",0);
    subpolymenu->additem("Specular trans",0);    
    
    polymenu->additem("Delete",0);
    polymenu->additem("Flip normals",0);
    polymenu->additem("Set attribute",0,0,subpolymenu);
    polymenu->additem("Windowize",0);
    polymenu->additem("Merge",CMDmergeselectedvertices);
    polymenu->additem("Flip normals",CMDflipnormals);
    //drawing functions tab boxes
    drawfuncs=new tabbox(96,menu->b.y+1,SC.size.x-2,menu->b.y+85);
    
    drawfuncs->addtabcontrol(selectctrl=new container(0,CMDselect),"Select");
    drawfuncs->addtabcontrol(squarectrl=new container(0,CMDsquare),"Square");
    drawfuncs->addtabcontrol(circlectrl=new container(0,CMDcircle),"Circle");
    drawfuncs->addtabcontrol(polyctrl=new container(0,CMDpolygon),"Polygon");
    drawfuncs->addtabcontrol(cubectrl=new container(0,CMDcube),"Cube");
    drawfuncs->addtabcontrol(spherectrl=new container(0,CMDsphere),"Sphere");
    drawfuncs->addtabcontrol(selipsectrl=new container(0,CMDselipse),"S-Elipse");
    drawfuncs->addtabcontrol(cylinderctrl=new container(0,CMDcylinder),"Cylinder");
    drawfuncs->addtabcontrol(conectrl=new container(0,CMDcone),"Cone");
    drawfuncs->addtabcontrol(gridctrl=new container(0,CMDgrid),"Grid");
    drawfuncs->addtabcontrol(textctrl=new container(0,CMDtext),"Text");
    numbox* newbox;
    insertrel(menu);    
    
    insertrel(drawfuncs);
    
    circlectrl->insertrel(newbox=new numbox(10,0,45,CMDcircvalchanged,3,100,circlevals.segs));
    newbox->target=this;
    circlectrl->addbackview();


    xpos=5;
    UINT32 len=findlongesttext(T3Dselectionlevels,4)+8+drop_btnw;
    selectionlevel=new dropbox(xpos,xpos+len,15,(listelem*)T3Dselectionlevels,4);
    xpos+=len+5;
    len=findlongesttext(T3Dselectionfunctions,6)+8+drop_btnw;
    selectionfunction=new dropbox(xpos,xpos+len,15,(listelem*)T3Dselectionfunctions,6);
    xpos+=len+5;
    len=findlongesttext(T3Dselectioneffects,5)+8+drop_btnw;
    selectioneffect=new dropbox(xpos,xpos+len,15,(listelem*)T3Dselectioneffects,5);
    xpos+=len+5;
    //ttoolbar *bar=new ttoolbar(5,5,(24*4)-1,23,toolbm,13,24,24);
    //ttoolbar *transmodebar=new ttoolbar(135,5,(24*3)-1,23,toolbm,13,24,24);
    /*
    bar->addbutton(TBAglobaltoggle|TBAdown ,0,CMDvertexmode);    
    bar->addbutton(TBAglobaltoggle ,1,CMDedgemode);
    bar->addbutton(TBAglobaltoggle ,2,CMDpolygonmode);
    bar->addbutton(TBAglobaltoggle ,3,CMDobjectmode);
  
    transmodebar->addbutton(TBAglobaltoggle|TBAdown,10,CMDstretchmode);    
    transmodebar->addbutton(TBAglobaltoggle ,11,CMDrotatemode);
    transmodebar->addbutton(TBAglobaltoggle ,12,CMDtapermode);
    */
    cubectrl->addbackview();
    textctrl->addbackview();
    squarectrl->addbackview();
    polyctrl->addbackview();

    selectctrl->insertrel(selectionlevel);

    selectctrl->insertrel(selectionfunction);
    selectctrl->insertrel(selectioneffect);
    selectctrl->insertrel(new tbutton(xpos,0,100,bheight,NULL,0,BMtoggleaction,CMDtoggleselectcopy,13,BAnooffset|BAycentre|BAnobox|T3Dcheckergrid,"Copy",b_toggle0,b_toggle0,b_toggle1));    
    selectctrl->insertrel(new tbutton(xpos,15,100,bheight,NULL,0,BMtoggleaction,CMDtoggleselectextrude,13,BAnooffset|BAycentre|BAnobox|T3Dcheckergrid,"Extrude",b_toggle0,b_toggle0,b_toggle1));
    selectctrl->insertrel(new tbutton(xpos,30,100,bheight,NULL,0,BMtoggleaction,CMDtoggleselectkeeppolys,13,BAnooffset|BAycentre|BAnobox|T3Dcheckergrid,"Keep polys",b_toggle0,b_toggle0,b_toggle1));
    xpos+=105;
    
    selectctrl->insertrel(new numbox(xpos,15,45,CMDnumcopieschanged,1,100,1));
    selectctrl->addbackview();

    spherectrl->insertrel(new numbox(10,0,45,CMDspherexvalchanged,3,100,spherevals.xsegs));
    spherectrl->insertrel(new numbox(64,0,45,CMDsphereyvalchanged,3,100,spherevals.ysegs));
    spherectrl->addbackview();

    selipsectrl->insertrel(new numbox(10,0,45,CMDselipsexvalchanged,3,100,selipsevals.xsegs));
    selipsectrl->insertrel(new numbox(64,0,45,CMDselipseyvalchanged,3,100,selipsevals.ysegs));
    selipsectrl->insertrel(new numbox(118,0,45,CMDselipsep1changed,0,100,selipsevals.p1));
    selipsectrl->insertrel(new numbox(172,0,45,CMDselipsep2changed,0,100,selipsevals.p2));
    selipsectrl->addbackview();

    gridctrl->insertrel(new numbox(10,0,45,CMDgridxvalchanged,3,100,gridvals.xsegs));
    gridctrl->insertrel(new numbox(64,0,45,CMDgridyvalchanged,3,100,gridvals.ysegs));
    gridctrl->insertrel(new tbutton(200,10,100,bheight,NULL,0,BMtoggleaction,CMDtoggridchecker,13,BAnooffset|BAycentre|BAnobox|T3Dcheckergrid,"Checkered",b_toggle0,b_toggle0,b_toggle1));
    gridctrl->addbackview();
    
    cylinderctrl->insertrel(new numbox(10,0,45,CMDcylindervalchanged,3,100,cylindervals.xsegs));
    cylinderctrl->addbackview();
    conectrl->insertrel(new numbox(10,0,45,CMDconevalchanged,3,100,conevals.xsegs));
    conectrl->addbackview();
    
    insert(new leftbar(0,menu->b.y+1,y2));      
    desktop.rassign(drawfuncs->a.x+2,drawfuncs->b.y+2,SC.size.x-2,SC.size.y-2);
    halfpoint.assign(desktop.a.x+desktop.halfx(),desktop.a.y+desktop.halfy());
    windowsize.assign(desktop.halfx()-2,desktop.halfy()-2);

    messages=new logmessagebox(0,0,1,1,&T3Dmessages);
    insert (new scrollcontainer(desktop.a.x,SC.size.y-80,desktop.b.x,SC.size.y-1,messages,CFybar));
    
    orthxy=new edit3d(0,0,SC.size.x,SC.size.y,thescene,camORTHXY,NULL);
    orthxz=new edit3d(0,0,SC.size.x,SC.size.y,thescene,camORTHXZ,NULL);
    orthzy=new edit3d(0,0,SC.size.x,SC.size.y,thescene,camORTHZY,NULL);
    
    renderview=new view3d(0,0,SC.size.x,SC.size.y,thescene,NULL);
    renderview->viewcam->setname("3d camera");
    renderview->viewcam->moveto(0,0,-100);
    thescene->addobject(NULL,renderview->viewcam);
    renderview->viewcam->attribute &= ~ofEDITABLE;
    
    //renderview->viewcam->camtype=camORTHXY;
    window *windowof3d;
    
    if (orthxy)
    {
       windowof3d=new window(desktop.a.x,desktop.a.y,windowsize.x,windowsize.y,"XYview",orthxy,CFclientedge|CFxbar|CFybar);
       insertrel(windowof3d);
    }

    if (orthzy)
    {                        
       windowof3d=new window(halfpoint.x,desktop.a.y,windowsize.x,windowsize.y,"ZYview",orthzy,CFclientedge|CFxbar|CFybar);
       insertrel(windowof3d);
    }
    if (orthxz)
    {
       windowof3d=new window(desktop.a.x,halfpoint.y,windowsize.x,windowsize.y,"XZview",orthxz,CFclientedge|CFxbar|CFybar);
       insertrel(windowof3d);
    }
    if (renderview)
    {
       //renderview->bg=getcolour(0,0,30);
       windowof3d=new window(halfpoint.x,halfpoint.y,windowsize.x,windowsize.y,"3d view",renderview,CFclientedge|CFxbar|CFybar);
       insertrel(windowof3d);
    }   
    //logmessage((void*)&textcol,"Created rendering window");
    //getch();
    
}

clay::renderall()
{
   renderview->calc3d();
   renderview->drawview();
  // if (changed==2)
  // {
       orthxy->calc3d();
       orthxy->drawview();
       orthzy->calc3d();
       orthzy->drawview();
       orthxz->calc3d();
       orthxz->drawview();
  // }
}


char *getfilename(char *path,char *dest)
{
   char dummy[128];
   _splitpath(path, dummy,dummy,dest,dummy);
   return dest;
}


void clay::handleevent(tevent *event)
{
    tbitmap *tex=NULL;
    UINT8 changed=0;
    if (event->what&EVcommand)
    {
        UINT32 ival=*(UINT32*)event->infoptr;
        switch (event->command)
        {
        case CMDdropboxclosed:
          if (event->infoptr==selectionlevel)
          {        
             thescene->setselectmode(selectionlevel->getselcontextattrib());changed=3;
          }else if (event->infoptr==selectionfunction)
          {
              T3Dselectionfunction=selectionfunction->getselcontextattrib();changed=2;
          }else if (event->infoptr==selectioneffect)          
              T3Dselectioneffect=selectioneffect->getselcontextattrib();
          break;
        case CMDedgemode:
           break;
        case CMDpolygonmode:
           thescene->setselectmode(etPOLYGON);changed=3;break;
        case CMDobjectmode:
           //T3Dselectmodebreak=etOBJECTS;
           break;
        //copy mode attribute events

        case CMDtoggleselectcopy:T3Dcopyattributes ^=STRNScopy;break;
        case CMDtoggleselectextrude:T3Dcopyattributes ^=STRNSextrude;break;
        case CMDtoggleselectkeeppolys:T3Dcopyattributes ^=STRNSkeepoldpolys;break;
                
        
        case CMDmergeselectedvertices:thescene->mergeselectedvertices(1);changed=3;break;
        case CMDflipnormals:thescene->flipselectedpolygons();changed=3;break;   
        case CMDleave:fin=1;break;
        case CMDtoggridchecker:T3Dcheckergrid^=TRUE;break;
        case CMDcircvalchanged:circlevals.segs=ival;break;
        case CMDspherexvalchanged:spherevals.xsegs=ival;break;
        case CMDsphereyvalchanged:spherevals.ysegs=ival;break;
        case CMDselipsexvalchanged:selipsevals.xsegs=ival;break;
        case CMDselipseyvalchanged:selipsevals.ysegs=ival;break;
        case CMDselipsep1changed:selipsevals.p1=ival;break;
        case CMDselipsep2changed:selipsevals.p2=ival;break;
        case CMDgridxvalchanged:gridvals.xsegs=ival;break;
        case CMDgridyvalchanged:gridvals.ysegs=ival;break;
        case CMDcylindervalchanged:cylindervals.xsegs=ival;break;
        case CMDconevalchanged:conevals.xsegs=ival;break;
        case CMDscreendump:
         writeraw("\\dump.raw",SC.size.x,SC.size.y,(UINT16*)SC.scrptr);break;        
        case CMDlight1:plight->attribute^=ofENABLED;changed=1;break;
        case CMDlight2:plight2->attribute^=ofENABLED;changed=1;break;
        case CMDlight3:plight3->attribute^=ofENABLED;changed=1;break;
        case CMDlight4:plight4->attribute^=ofENABLED;changed=1;break;
        case CMDsnapgrid:thescene->grid->attribute^=ofGRIDSNAP;break;
        case CMDviewgrid:thescene->grid->attribute^=ofVISIBLE;changed=3;break;
        case CMDrendertype:renderview->rendertype^=1;break;
        case CMDtogspecular:T3Dsceneattributes^=sfSPECULARHIGHLIGHTS;changed=1;break;
        case CMDtogfog:T3Dsceneattributes^=sfFOG;changed=1;break;        
        case CMDtogonlyshowselectednodes:T3Dsceneattributes^=sfONLYSHOWSELECTEDNODES;changed=2;break;
        
        case CMDtabchanged:thescene->drawmode=((tabbox*)event->infotview)->getseltabviewid();break;        
        case CMDselectionchanged:break;
        case CMDselectedchanged:
          if (thescene->cobj)
            thescene->cobj->attribute &=~ofSELECTED;                    
          thescene->cobj= (tobject3d*)treeview->currsel;
          if (thescene->cobj)
            thescene->cobj->attribute |=ofSELECTED;                    

        break;
        case CMDcalc3d:           
           orthxy->calc3d();
           orthxz->calc3d();
           orthzy->calc3d();           
        break;
        case CMDundraw:
           ival=((tobject3d*)event->infoptr)->attribute;
           ((tobject3d*)event->infoptr)->attribute&=~ofENABLED;           
           orthxy->calc3d();
           orthxz->calc3d();
           orthzy->calc3d();
           ((tobject3d*)event->infoptr)->attribute=ival;
           
        break;
        
        case CMDscenealtered:changed=3;break;
        case CMDscenealteredorth:changed=2;break;
        case CMDsave3dobjectdialog:add_and_draw(new window(0,20,380,260,"Save 3d Object",new fileview(0,0,&save3dobject),CFnoresize),VPcentre,PIDsave3dobject);break;
        case CMDload3dobjectdialog:add_and_draw(new window(0,20,380,260,"Load 3d Object",new fileview(0,0,&load3dobject),CFnoresize),VPcentre,PIDload3dobject);break;
        case CMDsave3dobject:
          char ch;
          ch='y';
          if (fexist(save3dobject.fullpath))
          {
            logmessage((void*)&T3DCOLboundingbox, "Save y/n?");            
            ch=getch();
          }
          if (ch=='y')
          {
             switch (save3dobject.filterindex)
             {                                  
                 case 0:T3Dsavenewclay(save3dobject.fullpath,thescene);
                 logmessage((void*)&textcol,"Saved Clayworks file %s:",save3dobject.fullpath);
                 break;
                 case 1:T3Dsavejava(save3dobject.fullpath,thescene);
                 logmessage((void*)&textcol,"Saved ClayJava source file %s:",save3dobject.fullpath);
                 break;
             }
          }else
             logmessage((void*)&textcol,"Canceled");            
          break;
        case CMDloadtexture:
              switch (loadtexture.filterindex)
              {                                  
                 case 0:tex=(tbitmap*)readbmp(loadtexture.fullpath,BMflat16);break;
                 case 1:tex=(tbitmap*)readpcx(loadtexture.fullpath,BMflat16);break;
              }
              char tempbuf[8];
              if (tex)
                T3Daddtexture((texture*)tex,getfilename(loadtexture.fullpath,tempbuf));                                     
              break;                   
        case CMDload3dobject:
           t3dmesh *obj=NULL;
           switch (load3dobject.filterindex)
           {
               case 0:
               thescene->removeobjects(0xFFFFFFFF,ofEDITABLE);
               if (T3Dloadnewclay(load3dobject.fullpath,thescene))
               {
                  changed=3;
                  thescene->selectelements(etVERTEX,bOR,ofHASSELECTEDVERTICES,naSELECTED);
                  thescene->selectelements(etPOLYGON,bOR,ofHASSELECTEDPOLYGONS,paSELECTED);                                                            
                  treeview->update();
               }else
                  obj=T3Dloadclay(load3dobject.fullpath);
               break;               
               case 1:obj=T3Dloadmr(load3dobject.fullpath);break;
               case 2:obj=T3Dloadlw(load3dobject.fullpath);break;
           }
           
           if (obj)
           {
               obj->calculatenormals();
               thescene->addobject(NULL,obj);
               getfilename(load3dobject.fullpath,obj->name);
               
               treeview->update();               
               changed=3;
           }           
           break;          
        }
        
        event->what=0;
    }else if (event->what&EVkeyboard)
    {
        switch (charcode)
        {
           case 0:
           switch (scancode)
           {
             case delkey:
             if (thescene->deleteselected(T3Dselectmode))changed=3;             
             break;
           }
           break;
        }
        event->what=0;
    } 

    /*if(*scan & SMleftaltkey))
    {
        //handle shortcuts in here        
        menu->handleevent(event);        
        tsoundoff();
    }*/
    if (changed)
        {
            if ((changed&1)&&(renderview)&&(renderview->isvisible()))
            {
                renderview->calc3d();
                renderview->drawview();
            }            
            if (changed&2)
            {
               if ((orthxy)&&(orthxy->isvisible()))
               {                            
                  orthxy->calc3d();
                  orthxy->drawview();
               }
               if ((orthzy)&&(orthzy->isvisible()))
               {                                           
                 orthzy->calc3d();
                 orthzy->drawview();
               }
               if ((orthxz)&&(orthxz->isvisible()))
               {                                           
                 orthxz->calc3d();
                 orthxz->drawview();
               }
            }           
        }


 
    if (event->what!=0)
       tgroup::handleevent(event);
    

}


BOOL mouseinstalled;



logmessagebox::logmessagebox(INT16 x,INT16 y,INT16 w,INT16 h,tlinkedlist *messages):tview(x,y,w,h)
{
    offset=sel=0;
    themessages=messages;
}

logmessagebox::~logmessagebox()
{
    //tlinkedlist.destroy();
}

void logmessagebox::update()
{
    INT16 addsize=t_charset.ysize+2;
    valuedata valdata;
    maxlines=themessages->count-(height()/addsize);
    offset=maxlines;
    valdata.min=0;valdata.max=maxlines;valdata.val=offset;
    //this could cause too many redraws as changebounds uses it also and
    //it invokes the scrollbar's setrange procedure
    //this then forces a redraw; need to find some way around this that's not too messy
    sendcommand(CMDydeltachanged,owner,&valdata);
    drawview();
}

void logmessagebox::draw()
{
    INT16 addsize=t_charset.ysize+2;
    textboxfilledb(a.x,a.y,b.x,b.y);
    tcolour oldcolour=t_col;
    

    logmessagestring *celem=(logmessagestring*)themessages->first;
    INT16 ypos=(b.y-addsize) + (maxlines-offset) * addsize;
    while (celem!=NULL)
    {
        t_col=celem->colour;
        outtextxy(a.x+2,ypos,celem->string);
        ypos-=addsize;
        celem=(logmessagestring*)celem->next;
    }
    t_col=oldcolour;
}
    
void logmessagebox::handleevent(tevent *event)
{
    tview::handleevent(event);
    if (event->what == EVcommand)
      switch (event->command)
      {
          case CMDyvalchanged:
              UINT32 oldoff=offset;
              offset=*(UINT32*)event->infoptr;
              if (oldoff!=offset)                                
                  drawview();                                
              break;                
      }
}


void setcolours()
{
    textcol=getcolour(5,5,5);   //text colour//
    titletextcol=getcolour(200,200,200); //for window titles//
    scrollcol=getcolour(120,100,130); //scroller background//
    scrollhighlight=getcolour(140,120,150); //scroller background//
    lightcol=getcolour(160,160,170);  //highlight colour//
    midcol=getcolour(138,138,138);    //middle colour//
    mid_darkcol=getcolour(100,100,100);   //shadow colour//
    mid_darkcol2=getcolour(60,60,60);   //shadow colour//
    darkcol=getcolour(20,20,20);   //shadow colour//
    backcol=getcolour(50,20,20);   //background colour//
    gridcol=getcolour(50,200,40);    //grid colour//
    selectcol=getcolour(200,40,40);  //selection colour//
    titlecol=getcolour(50,200,40);   //titlebar colour//
    disabledcol=getcolour(50,200,40);
    menuselcolback=getcolour(100,120,150); 
    menuselcolfore=getcolour(255,255,255);
    textboxcol=getcolour(190,190,190);
    highlight=getcolour(255,255,255); //for icons e.t.c.//
}


/*void genscenegraphnode(tobject3d *cobj,treebox *treeview,tree_elem *cparent)
{
    tree_elem *newelem;
    while (cobj!=NULL)
    {
        newelem=treeview->addelem(cparent,cobj->name,sphere,0,cobj,0);
        if (cobj->child!=NULL)
           genscenegraphnode((tobject3d*)cobj->child,treeview,newelem);        
        cobj=(tobject3d*)cobj->next;
    } 
}

void genscenegraph(tscene *scn,treebox *treeview)
{
    tobject3d *cobj=(tobject3d*)scn->scenegraph.root;
    tree_elem *cparent=NULL;
    genscenegraphnode(cobj,treeview,cparent);                
}
*/



void printlight(tlight *l,UINT16 x,UINT16 y)
{    
    sprintf(buf,"ambient r:%.2f g:%.2f b:%.2f ",
                l->ambient.r,
                l->ambient.g,
                l->ambient.b);
                                
    outtextxy(x,y,buf);
    y+=15;
    sprintf(buf,"diffuse r:%.2f g:%.2f b:%.2f",
                l->diffuse.r,
                l->diffuse.g,
                l->diffuse.b);

    outtextxy(x,y,buf);
    y+=15;                
                
    sprintf(buf,"specular r:&.2f g:%.2f b:%.2f",
                l->specular.r,
                l->specular.g,
                l->specular.b);

    outtextxy(x,y,buf);
    y+=15;

    sprintf(buf,"Orientation x:%.2f y:%.2f z:%.2f",
                l->o.x,
                l->o.y,
                l->o.z);

    outtextxy(x,y,buf);
    y+=15;

    sprintf(buf,"Position x:%.2f y:%.2f z:%.2f",
                l->p.x,
                l->p.y,
                l->p.z);

    outtextxy(x,y,buf);
    y+=15;
    
}    
void gettreedatafromobj(ttreenode *node,char **txt,tbitmap **pic,UINT32 **attrib)
{
    UINT32 type = ((tobject3d*)node)->type;
    *txt=(char*)&((tobject3d*)node)->name;
    *attrib=&((tobject3d*)node)->treeattrib;
    *pic=sphere;    
}

void logmessagefunc(void *data,const char *format,...)
{
    static ypos=0;
    logmessagestring* msg=new logmessagestring;
    strcpy(msg->string,format);
    msg->colour=*(tcolour *)data;
    T3Dmessages.addtofront((telem *)msg);
    if (view)
    {
        view->messages->update();
    }
    else if (GFX_INITIALIZED)
    {
       t_col=*(tcolour *)&data;
       t_fillcol=getcolour(0,0,0);
       outtextxy(0,ypos+=t_charset.ysize,msg->string);
    }else
    {
       printf(msg->string);
       printf("\n");       
    }        
}

inline float SUB_PIX(const float input)
{
    float   retCode;
    static double half=0.5f;
    _asm
    {    
        fld     input
        fadd    dword ptr half
        frndint        
        fld     input
        fsubp   st(1), st
        fstp    retCode
    }
    
    return retCode;        
}

void main(int argc, char *argv[])
{
    UINT16 *adrs;
    trgbfloat black;
    window *matwin;
    window *prefwin;     
    window *scenetree;
    tcolour bgcol;
    preferences *prefs;
    matrix4x4 cmat;        
    char dummy[255];
    char ext[5];

    
    T3Dmessages.init(sizeof(logmessagestring));
    initlog(logmessagefunc);    

    logmessage((void*)&T3DCOLboundingbox,"Starting Clayworks 3.0a for DOS");
    logmessage((void*)&T3DCOLboundingbox,"Make sure you are in fullscreen mode and then press any key");
    float val=SUB_PIX(1.5);
    sprintf(buf,"value is %f",val);
    logmessage((void*)&T3DCOLboundingbox,buf);    
                
                
    tcamera *cam;
    flt ry=0.0f,rx=0.0f,rz=0.0f;
    flt spiny=0.0f,spinx=0.0f,spinz=0.0f;    
    getch();
    makepath(EXE_PATH,argv[0]);
    sprintf(buf,"%ssansf16b.fnt",EXE_PATH);
    if (!FNTload(buf,&t_charset))
    {

        sprintf(exitstring,"Cannot find font file, exiting");
        goto shutdown;
    }
                         
    if (!initgraph(SVGA1024x768x64k))
    {
        sprintf(exitstring,"Could not set the video mode, Try using scitech's display doctor from www.scitechsoftware.com");
        goto shutdown;
        /*if (!lfb)
        {
        printf("Your card does not support a linear frame buffer\n");
        printf("Please run Scitech display doctor from DOS and try again\n");
        printf("You can get display doctor from www.scitech.com\n");
        }
        */        
    };
    
    //tbitmap *pic=read;
    adrs = (UINT16*)SC.scrptr;       
    T3Dinit(100000,1024*512);
    t_col=getcolour(0,0,0);
    plight=new tlight;
    plight2=new tlight;
    plight3=new tlight;
    plight4=new tlight;
    
    cam=new tcamera();
    strcpy(plight4->name,"Green");
    plight4->rotate(0,1,0);
    plight4->o.normalize();
    plight4->ambient=trgbfloat(0.0,0.0,0.0);
    plight4->diffuse=trgbfloat(0.0,0.3,0.0);    
    plight4->specular=trgbfloat(0.0,0.4,0.0);

    strcpy(plight2->name,"Cyan");
    plight2->o=vector(-1,0,0);
    plight2->o.normalize();      
    plight2->ambient=trgbfloat(0.0,0.0,0.0);    
    plight2->diffuse=trgbfloat(0.0,0.2,0.3);
    plight2->specular=trgbfloat(0.0,0.6,0.8);

    strcpy(plight3->name,"Red");
    plight3->lighttype=ltPOINT;
    plight3->o=vector(2,1,0);
    plight3->o.normalize();
    plight3->ambient=trgbfloat(0.001,0.0,0.0);
    plight3->diffuse=trgbfloat(0.5,0.0,0.0);    
    plight3->specular=trgbfloat(0.8,0.0,0.0);

    strcpy(plight->name,"White");
    plight->lighttype=ltPOINT;
    plight->rotate(0,0,0);
    plight->moveto(-60,-60,-60);
    
    //plight->o.normalize();
    plight->ambient=trgbfloat(0.0,0.0,0.0);
    plight->diffuse=trgbfloat(0.7,0.7,0.7);
    plight->specular=trgbfloat(1.0,1.0,1.0);    
    //cam->moveto(0,0,0);//700);    
    T3Dctm=&cmat;    
    LMidentity(&cmat);
    logmessage((void*)&textcol,"About to initialize windowing system");
    getch();
    
    if (!inittwin(EXE_PATH))
    {
        sprintf(exitstring,"Failed to initialize windowing system, probably due to a missing resource file");
        goto shutdown;
    }else
    {
        logmessage((void*)&textcol,"TWIN initialization successfull");
    }
    cleardevice();
    getch();

    drawarrow(100,100,200,20,1.0,0.7,20);
    getch();
    T3Dpolycreationflags|=cfWRAPTEXTURE;    
    _splitpath(argv[1],dummy,dummy,dummy,ext);
    lowcase(ext);

    thescene =new teditscene;

    thescene->setfog(400,3000);
    thescene->fogcolour=trgbfloat(20/255.0f,80/255.0f,120/255.0f);
    T3Dsceneattributes|=sfFOG;
        
    getch();
      
    black=trgbfloat(0.0f,0.0f,0.0f);

    T3Daddmaterial(black,trgbfloat(1.0,0.2,0.2),trgbfloat(0.5,0.2,1.0),black,0,0);
    T3Daddmaterial(black,trgbfloat(0.2,1.0,0.2),trgbfloat(0.3,0.7,0.7),black,0,0);
    T3Daddmaterial(black,trgbfloat(0.2,0.2,1.0),trgbfloat(0.5,1.0,1.4),black,0,0);
    T3Daddmaterial(black,trgbfloat(0.2,0.6,0.2),trgbfloat(0.5,0.2,1.0),black,0,0);
    T3Daddmaterial(black,trgbfloat(0.6,1.0,1.0),trgbfloat(0.3,0.7,0.7),black,0,0);
    T3Daddmaterial(black,trgbfloat(1.0,0.2,1.0),trgbfloat(0.5,1.0,1.4),black,0,0);
    
    T3Dmaterial=T3Daddmaterial(black,trgbfloat(0.9,0.9,0.9),trgbfloat(0.7,0.7,0.7),black,0,0);

    

    thescene->addobject(NULL,plight);
    plight->enable(TRUE);
    thescene->addobject(NULL,plight2);
    plight2->enable(FALSE);
    thescene->addobject(NULL,plight3);
    plight3->enable(FALSE);
    thescene->addobject(NULL,plight4);        
    plight4->enable(FALSE);
    
    
    getch();
    
   
    //T3Dprintstats(200,200);
    bgcol=getcolour(0,0,80);
    
   
    sprintf(buf,"%ssphere.pcx",EXE_PATH);
    sphere=readpcx(buf,SC.driver,chromaput);


    sprintf(buf,"%stoolbar1.pcx",EXE_PATH);
    toolbm=readpcx(buf,SC.driver,normalput);
    sprintf(buf,"%stoolbar2.pcx",EXE_PATH);
    toolbm2=readpcx(buf,SC.driver,normalput);                      
    
    
    thescene->cobj=(tobject3d*)thescene->scenegraph.root;

    view=new clay(0,0,SC.size.x,SC.size.y);

    getch();


    setroot(view);
    
    treeview=new treebox(340,70,200,4+(6*t_charset.ysize),&thescene->scenegraph,gettreedatafromobj);
    t_alpha=100;         
    
    matedit=new materialedit(0,0,392,255,T3Dmaterial);
    T3Daddtexturecallback=T3Daddtexturefunc;    
    

    strcpy(load3dobject.fullpath,EXE_PATH);
    strcat(load3dobject.fullpath,"*.3d");
    load3dobject.filtercount=3;
    load3dobject.presetfilters=new listelem[load3dobject.filtercount];
    load3dobject.presetfilters[0].set("*.3d Clayworks",0,0,0,0);
    load3dobject.presetfilters[1].set("*.odt Mindformer",0,0,0,0);
    load3dobject.presetfilters[2].set("*.lwo Lightwave",0,0,0,0);
    load3dobject.filterindex=1;
    load3dobject.type=FDTopen;
    load3dobject.fileindex=0;
    load3dobject.okcommand=CMDload3dobject;
    load3dobject.cancelcommand=CMDcancel;


    strcpy(save3dobject.fullpath,EXE_PATH);
    strcat(save3dobject.fullpath,"*.3d");
    save3dobject.filtercount=1;
    save3dobject.presetfilters=new listelem[save3dobject.filtercount];
    save3dobject.presetfilters[0].set("*.3d Clayworks",0,0,0,0);
    save3dobject.filterindex=0;
    save3dobject.type=FDTsave;
    save3dobject.fileindex=0;
    save3dobject.okcommand=CMDsave3dobject;
    save3dobject.cancelcommand=CMDcancel;
    

    strcpy(loadtexture.fullpath,EXE_PATH);
    strcat(loadtexture.fullpath,"*.pcx");
    
    loadtexture.filtercount=2;
    loadtexture.presetfilters=new listelem[loadtexture.filtercount];
    loadtexture.presetfilters[0].set("*.bmp Microsoft",0,0,0,0);
    loadtexture.presetfilters[1].set("*.pcx Zsoft",0,0,0,0);
    loadtexture.filterindex=1;
    loadtexture.type=FDTopen;
    loadtexture.fileindex=0;
    loadtexture.okcommand=CMDloadtexture;
    loadtexture.cancelcommand=CMDcancel;
    
    prefs=new preferences(0,0);
    prefwin=new window(500,120,330,260,"Preferences",prefs,CFnoresize);
    view->insert(prefwin);

    scenetree=new window(500,120,300,300,"Scene",treeview,CFybar);
    view->insert(scenetree);
    
    matwin=new window(228,120,400,255,"Edit Material",matedit,CFnoresize);        
    view->insert(matwin);    
    
    
    getch();
    
    
   //SC.viewport=SC.size;
    
    root->drawview();
    
   
   while (fin==0)
   {
       eventmpos();
       eventkey();       
   }   
   charcode=0;
  

shutdown:
  setoutput();
  T3Dshutdown();
  closegraph();
  view=NULL;
  logmessage((void*)&T3DCOLboundingbox,exitstring);
  logmessage((void*)&textcol,"Thank you for using this early beta of Clayworks.");
  logmessage((void*)&textcol,"Please report any bugs you may have found to luther2001@hotmail.com");
  logmessage((void*)&textcol,"Press any key to continue");
  getch();
}


