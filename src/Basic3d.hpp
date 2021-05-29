#ifndef _basic3d_h_
#define _basic3d_h_


//bitmaks and enumerations use the format xxXXXXX)
//#define SHOW_NORMALS
#define camORTHXY 1
#define camORTHXZ 2
#define camORTHZY 3
#define cam3D 4

//GLOBAL SCENE FLAGS
#define sfSPECULARHIGHLIGHTS 1
#define sfFOG 14
#define sfLINEARFOG 2
#define sfCOSINEFOG 4
#define sfVOLUMEFOG 8
#define sfONLYSHOWSELECTEDNODES 16

#define sfSELECTINGVERTICES 8
#define sfSELECTINGEDGES 16
#define sfSELECTINGPOLYGONS 32
#define sfSELECTINGOBJECTS 64
#define sfSELECTING 120

//Object types; An array of element masks (used for selection validifcation) relies on the order
//of these so it's not a good idea to change it.

#define otCAMERA 0x1
#define otGEOM 0x2
#define otINSTANCE 0x4
#define otLIGHT 0x8
#define otBOUNDINGBOX 0x10
#define otCURSOR 0x20
#define otGRID 0x40

//just used for file
#define otSCENE 0x80
#define otMATERIALBANK 0x100
#define otTEXTUREBANK 0x200
#define otEOF 0xFFFFF

#define stSTRETCH 1
#define stROTATE  2
#define stSCALE   3
#define stSHEAR   4
#define stTAPER   5
#define stTWIST   6

//object flags
#define ofENABLED 0x1             //can recieve messages and collisions
#define ofVISIBLE 0x4             //object can be seen
#define ofHASOWNHEAP 0x8          //uses internal memory heap; good for objects that need to be recalculated quickly
#define ofSELECTED 0x10           //uses internal memory heap; good for objects that need to be recalculated quickly
#define ofEDITABLE 0x20           //internal object; modification restrictions apply
#define ofAFFECTEDBYSELECTION 0x10000
#define ofHASSELECTEDVERTICES 0x20000
#define ofHASSELECTEDEDGES    0x40000
#define ofHASSELECTEDPOLYGONS 0x80000
#define ofCONTAINSCURRENTVERTEX 0x100000
#define ofCONTAINSCURRENTEDGE 0x200000
#define ofCONTAINSCURRENTPOLYGON 0x400000
#define ofAFFECTEDBYCURRENT  0x800000
//used for grids only
#define ofGRIDSNAP 0x1000000
//Object integrety
#define oiOK                   0x0



#define ltCONE 0x1
#define ltPOINT 0x2
#define ltDIRECTIONAL 0x3

#define perspective 400

//for sphere clip check
#define scINSIDE 0  //all inside
#define scOUTSIDE 255   //all outside
#define scLEFT 1        //intersects left
#define scRIGHT 2
#define scUP 4
#define scDOWN 8
#define scFRONT 16
#define scBACK 32


//element types, used in selection. T3Dselectmode uses these.
#define etVERTEX 0x0  //for geom meshes and things with vertex like handles like nurb curves and surfaces
#define etEDGE 0x1 //for geom meshes only
#define etPOLYGON 0x2 //for geom meshes only
#define etOBJECT 0x3  //all selectable objects (lights, meshes, nurb objects, blobs, cameras ect)

//for cone and cylinder
//Marvelous, multipurpose, sequin studed **Creation Flags** 
#define cfANTICLOCKWISE 0x1
#define cfNOPOLYS 0x2
#define cfTOPPOLY 0x4
#define cfBOTTOMPOLY 0x8
#define cfSIDEPOLYS 0x10
#define cfALLPOLYS  0x20
//Needs a specific length array for the 'cmat' (mindrender material) flag, 2 for cone, 3 for cylinder 
#define cfCOLOURBYPART 0x40
//texture mucking about
#define cfWRAPTEXTURE 0x80
#define cfFLIPTEXTUREX 0x100
#define cfFLIPTEXTUREY 0x200
#define cfROTATETEXTURE90 0x400
#define cfROTATETEXTURE180 0x800
#define cfROTATETEXTURE270 0x1000
#define cfROTATETEXTURE360 0x2000
#define cfROTATETEXTUREMASK 0x4000
#define cfROTATETEXTURESHIFTRIGHT 0x8000
#define cfNOEDGES 0x10000


//polygon vertex attributes
#define pvaEDGEISANTICLOCKWISE 0x1  //the edge that is polygon vertex referes to is clockwise or not, relative to the polygon
#define pvaRESERVED 0x2


//edge attributes
#define eaSELECTED 0x1             //It's a selected edge and will be extruded
#define eaHIGHLIGHTED 0x2          //It's not selected but is part of a polygon selection
#define eaENABLED 0x4              //turned on
#define eaAFFECTEDBYSELECTION 0x8  //Not selected but affected by a selection; i.e. it will be redrawn
#define eaVISIBLE 0x10  //Not selected but affected by a selection; i.e. it will be redrawn
#define eaNOTDRAWNYET 0x20
#define eaCLOCKWISESELECTION 0x40
#define eaDOUBLESIDEDEXTRUSION 0x80
#define eaDELETEME             0x100
//upper 24bits are used for tagging, thus you can only have 8bits for state
//and a total of 16million edges selected at any one time. Enough for anybody?
#define eaTAGMASK 0xFFFFFF00
#define eaTAGSHIFT 0x8


//Edge integrety
#define eiOK                   0x0
#define eiVERTEXPARENTSDAMAGED 0x2
#define eiVERTEXNULLS          0x4


//polygon attributes
#define paSHADED                    0x1
#define paTEXTURED                  0x2
#define paTRANSPARENT               0x4
#define paADDALPHA                  0x8
#define paTEXTINTERPOLATED          0x10 // 0=perspective texture, 1=interpolated texture
#define paOUTLINED                  0x20 // 0=polygon, 1=vector
#define paDOUBLESIDED               0x40 // 0=single sided, 1=double sided
#define paVISIBLE                   0x80 //it's visible
#define paNOSELECT                  0x100 // 0=selectable, 1=not selectable
#define paENVMAP                    0x200 // 0=standard UV texture, 1=environment mapping
#define paTRUECOLOUR                0x400 // 1=use poly->colour, 0=use light sourcing.
#define paNOSPECTRANS               0x8000
#define paPARTICLE                  0x10000
#define paTEXTURETRUE               0x20000

#define paSELECTED                  0x40000
#define paCLIPPEDPOLY               0x80000 // This is a temporary clipped poly; the next pointer points to the original polygon
#define paBACKFACING                0x100000 // This polygon failed the backface test
#define paHASSELECTEDVERTICES       0x200000  // Reserved flag: so that you don't have to check all the vertices each time; set when vertices are selected
#define paALLVERTICESSELECTED       0x400000 // Reserved flag: so that you don't have to check all the vertices each time; set when vertices are selected
#define paCURRENT                   0x800000 // Reserved flag to show that this is the current poly
#define paCONTAINSCURRENTVERTEX     0x1000000 // Reserved flag to show that this is the current poly
#define paAFFECTEDBYCURRENTPOLYGON  0x2000000 // Reserved flag to show that this is the current poly
#define paAFFECTEDBYSELECTION       0x4000000 // This is either a selected polygon or connected to one
#define paENABLED                   0x8000000
#define paALLVERTICESVISIBLE        0x10000000
#define paDELETEME                  0x40000000
#define paNEWEXTRUSION              0x80000000

#define paSELECTEDVERTEXMASK    paHASSELECTEDVERTICES+paALLVERTICESSELECTED
#define paRESERVED_MASK         paSELECTED+paCLIPPEDPOLY+paBACKFACING+paHASSELECTEDVERTICES+paALLVERTICESSELECTED+paCURRENT+paCONTAINSCURRENTVERTEX+paAFFECTEDBYSELECTION+paENABLED+paVISIBLE+paDELETEME+paNEWEXTRUSION
#define paATTRIBUTE_MASK        0xFFFFFFFF^paRESERVED

//Polygon integrety
#define piOK                   0x0
#define piEDGESDAMAGED         0x1
#define piVERTEXPARENTSDAMAGED 0x2
#define piVERTEXNULLS          0x4
#define piEDGENULLS            0x8
#define piMISSINGEDGEREFERENCE 0x10


//only used in the EdgeConnect function
#define cmTOPOINT  1            //the first parameter is a single point  (as in a cone)                             
#define cmTOPOINTLIST 0         //both params define a list of verticies (as in a cylinder)

//node attributes
#define naVISIBLE 0x1 //clip or not?
#define naDRAWNALREADY 0x2 //For efficient drawing. Clear after each draw.
#define naCLEARMASK 0x2
#define naTEMPORARY 0x4 //it is a volatile, temporary vertex
#define naPERSPECTIVECALCED  0x8 //perspective has already been calculated, no need to do it again
#define naSELECTED 0x10 //if set, this vertex is selected
#define naCURRENT 0x20 //if set, this vertex is the current vertex
#define naAFFECTEDBYSELECTION         0x40 //if set, it's not selected but it's normal will need updating
#define naREFERENCEDBYSELECTEDPOLYGON 0x80 //This vertex is inside a selected vertex
#define naREFERENCEDBYCURRENTPOLYGON 0x200 //a selected polygon references this vertex
#define naAFFECTEDBYCURRENT 0x400  //a polygon conected to current vertex contains this vertex
#define naTRANSFORMCALCED 0x1000
#define naENABLED 0x2000
#define naDELETEME 0x4000

//Edge integrety
#define niOK                   0x0


#define boundingboxrangex2 26.0
#define boundingboxrange 13.0
#define bndctrlsize 8.0
#define bndindent 4.0

//predefs
class tpolygon;
class t3dmesh;
class tpolyvertex;
class tclipvert;
class tedge;
class material;
//material, embeded with next pointer

class material:public telem
{
    public:
    trgbfloat ambient,diffuse,specular,emmisive;
    UINT8 specpower;
    flt st; //shine strength.. replacing the '2.0f' of r'=(2.0f*n'*dp)-o' maybe incorrect
    flt transparency;
    bool operator ==(material &other){return ((ambient==other.ambient)&&
                                          (diffuse==other.diffuse)&&
                                          (specular==other.specular)&&
                                          (emmisive==other.emmisive)&&
                                          (specpower==other.specpower)&&
                                          (transparency==other.transparency));}
    bool operator !=(material &other){return ((ambient!=other.ambient)||
                                          (diffuse!=other.diffuse)||
                                          (specular!=other.specular)||
                                          (emmisive!=other.emmisive)||
                                          (specpower!=other.specpower)||
                                          (transparency!=other.transparency));}
    material *nextm(){return (material*)next;}                                              
};

class texture:public tbitmap
{
    //public:
    //tpointflt offset;
    //UINT32 attribute;
};

typedef texture *textureptr;

class textureref:public telem
{
    public:
    texture *t;
    char name[20];
    textureref *nextt(){return (textureref*)next;}
};        



class tpolygonref:public telem
{
    public:
    tpolygon *p;
    tpolygonref(tpolygon *p_){p=p_;}
};

class vertex:public telem
{
    public:
    UINT32 attribute; 
    vector data3d;
    vector temp3d;
    vector normal;    
    vector tempnormal;
    vector old;       //used to avoid compound errors when moving a selection
    tpointflt point2d;
    t3dmesh *parent; //to what object does this belong
    UINT32 num;  //temporary, for debugging
    //tpolygonref *prefs;
    tedge *anedge;
    //tclipvert *clipv;
    void writetag(UINT32 tag){ *((UINT32*)&temp3d.x)=tag;}
    UINT32 readtag(){ return *((UINT32*)&temp3d.x);}
    
    
    vertex *nxt();   
};

class tedge:public telem
{
    public:
    UINT32 attribute;
    vertex *v[2];
    tpolygonref *prefs;
    UINT32 tag;
    UINT32 edgeloopref,edgeloopcount;
    tedge(vertex *v1,vertex *v2);
    BOOL deletereferences(){SLdeletelist(prefs);prefs=NULL;return TRUE;}
    BOOL deletereferences(UINT32 attribute);
    BOOL deletereferences(tpolygon *poly);
    tpolygon *addreference(tpolygon *poly);
    t3dmesh *parent(){return v[0]->parent;};
    /*void writetag(UINT32 tag){ attribute |=(tag << eaTAGSHIFT);}
    UINT32 readtag(){ return attribute >> eaTAGSHIFT;}*/
    void writetag(UINT32 tag_){tag=tag_;}
    UINT32 readtag(){ return tag;}
    UINT32 checkintegrity();
    UINT32 selectedvertexcount();
    BOOL sharesvertexwith(tedge *e);
    BOOL isclockwise();
};



class tclipvert
{
    public:
    vector v,n;
    //tclipvert *v,*v1;
    tedge *e;
    tpointflt t;
    tpolyvertex *oldv; //if the old one isn't outside, don't clip it!
};

/*class tclipedge
{
    public:
    
};*/

class tpolyvertex
{
    public:
    UINT32 attribute;
    tpointflt texv;
    vertex *v; //vertex at this point
    tedge* e;   //flag in attribute determines if this edge is clockwise or not
    material *mat;  
};





class tclippoly
{
    public:
    UINT8 npoints;
    tclipvert p[256];
};


//offset to tempcentre is important!
class tpolygon:public telem
{
    public:
    UINT32 attribute;        
    vector normal,tempnormal;    
    vector centre;
    #ifdef SHOW_NORMALS
    
    tpointflt c2d,n2d;
    #endif
    
    texture *tex;
    material *mat;    
    UINT32 npoints;    
    tpolyvertex vlist[1];
    
    void calcpolynormal();
    void reverse();
    t3dmesh *parent();    
    BOOL removeedgereferences();
    void or_vertexattributes(UINT32 attr);//on/off/toggle
    void and_vertexattributes(UINT32 attr);//on/off/toggle
    void xor_vertexattributes(UINT32 attr);//on/off/toggle
    void or_vertexrefattributes(UINT32 attr);//on/off/toggle
    void and_vertexrefattributes(UINT32 attr);//on/off/toggle    
    void xor_vertexrefattributes(UINT32 attr);//on/off/toggle
    void or_edgeattributes(UINT32 attr);
    void and_edgeattributes(UINT32 attr);
    void xor_edgeattributes(UINT32 attr);    
    BOOL checkvertexattributes(UINT32 attr);
    UINT32 checkallvertexattributes(UINT32 attr);//returns the amount of vertices who's attributes matched the critera
    BOOL contains(vertex *v);    
    BOOL allverticesclipped();
    BOOL someverticesclipped();
    BOOL noverticesclipped();
    void copyoldvertices();
    BOOL containsedge(vertex *from,vertex *to);
    UINT32 checkintegrity();
    tpolygon *nxt(){return (tpolygon*)next;}
};



class tcamera;

class tobject3d:public ttreenode
{
    public:
    char name[32];
    //tpoint cube[8];
    UINT32 attribute;
    UINT8 type;
    UINT32 treeattrib;
    vector p,o,s;        //position, orientation, scale. These are in local space, used to reconstruct the matricies
    tpointflt p2d;           //2d projection of centre
    vector min,max;       //defines bounding cube
    flt radius,tempradius;  //maximum radius of the object and it's temporary projection
    trectflt bounds;//objects 2d projected bounds -- VOLATILE
    vector d; //direction
    flt a,spd,mass,maxspeed,minspeed;             //acceleration
    matrix4x4 localmat,worldmat,localmattr,worldmattr; //local space matrix and world space matrix. camera space matrix is not stored.
    //localmattr does not include scaling (for use with normals)
    //the above data is essentially repeated in here in matrix form, both are included for ease of access.
    //perhaps this is all a little too big...
    // generates a 2d clipping rectangle using the current projection;
    char *setname(char *name_);
    void updatedatafull();
    virtual void updatedata();
    virtual trectflt *getbounds(matrix4x4 *,tcamera*);    
    void updatefull();
    void do3dfull(matrix4x4 *cmat ,matrix4x4 *cmattr,tcamera *cam);
    void renderhandlesfull(UINT8 attr,tcamera *cam);
    void unitpos();
    void lookat(vector pt,flt twist);
    void lookat2(vector pt,flt twist);
    
    void calcworldmat();  //calculate world space matrix from parent

    virtual void do3d(matrix4x4 *,matrix4x4 *,tcamera*);
    //virtual void updatedata();  //do AI stuff in here, hapenns inside do3d
    virtual void rotate(flt x,flt y,flt z);
    virtual void scale(flt x,flt y,flt z);
    virtual void moveto(flt x,flt y,flt z);
    virtual void oncollision();
    virtual void renderhandles(UINT8 attrib,tcamera *cam);
    
    void enable(BOOL onoff){(onoff)?attribute|=ofENABLED:attribute&=~ofENABLED;}
    tobject3d *tparent(){return (tobject3d*)parent;}
    tobject3d *tnext(){return (tobject3d*)next;}
    tobject3d *tchild(){return (tobject3d*)child;}    
    
    tobject3d();
    ~tobject3d();        
};

class t3dmesh:public tobject3d
{   
    public:    
    tlinkedlist vertexlist;    
    tlinkedlist polygonlist;
    tlinkedlist edgelist;

    //nasty little fix for merged vertices.. more complex than anticipated.
    void *tempdata;
    //theapmem memblock;//this is not always needed, some object however need to clear down their memory quickly
    
    t3dmesh();
    ~t3dmesh();
    //presentation methods        
    //  virtual void updatedata();  //do AI stuff in here
    virtual trectflt *getbounds(matrix4x4 *,tcamera*);
    virtual void do3d(matrix4x4 *,matrix4x4 *,tcamera*);
    virtual void renderhandles(UINT8,tcamera *cam);
    void clearvertexandedgeflags();
    void calculatenormals(UINT32 polymask=0xFFFFFFFF,UINT32 vertexmask=0xFFFFFFFF);
    //object construction methods
    void settexture(texture *,UINT8 selectedonly=1);
    void setmaterial(material *,UINT8 selectedonly=1);
    void setpolyattributes(UINT32 ,UINT8 selectedonly=1);
    void setvertexattributes(UINT32 ,UINT8 selectedonly=1);

    void and_vertexattributes(UINT32 attribute);
    void or_vertexattributes(UINT32 attribute);
    void and_edgeattributes(UINT32 attribute);
    void or_edgeattributes(UINT32 attribute);

    vertex *getvertex(UINT32);
    vertex *addvertex(flt,flt,flt);
    vertex *addvertex(flt,flt,flt,vertex *addvertex);//quick version, does not transform with CTM
    vertex *addvertexsimp(flt x1,flt y1,flt z1,vertex *addvertex);
    vertex *addvertex(vector*v){return addvertex(v->x,v->y,v->z);}
    tedge  *addedge(vertex *v,vertex *v1,UINT8 *mem=NULL);
    tedge *addedgesearch(vertex *v,vertex *v1,UINT8 *reversed,UINT8 *mem=NULL);
    tedge  *addedgeloop(vertex *startv,UINT32 count,UINT8 *mem=NULL);
    
    tpolygon*  addpoly(UINT32 count,UINT8 *mem=NULL);
    tpolygon*  addpoly(vertex *startv,tpointflt *txvecs,UINT32 count,UINT8 *mem=NULL);

    tpolygon* addquad(vertex *v1,vertex *v2,vertex *v3,vertex *v4,tpointflt *txvecs,UINT8 *mem=NULL);
    tpolygon*  addtriangle(vertex *v1,vertex *v2,vertex *v3,tpointflt *txvecs,UINT8 *mem=NULL);    
    tpolygon *singleedgeconnect(tedge *lefte,tedge *righte,tpointflt *texv,tedge *tope=NULL,tedge *bottome=NULL);
    tpolygon *singleedgeconnect(tedge *lefte,tpointflt *texv,tedge *tope=NULL,tedge *bottome=NULL);
    void edgeconnect(tedge*,tedge*,tpointflt*,UINT32);
    void edgeconnect(tedge*,vertex*,tpointflt*,UINT32);
};


class tplane
{
    public:
    double distance;
    vector normal;
};

class tcamera:public tobject3d
{
    public:
    //define the view fustrum;
    flt FOV;
    flt NEARZ,FARZ,NEARX,NEARY;
    
    UINT8 camtype;
    tcamera();
    virtual trectflt *getbounds(matrix4x4 *,tcamera*);
    tcamera(vector centre_,vector orientation_,flt FOV_);

    ~tcamera();
    virtual void renderhandles(UINT8 attrib,tcamera *cam);
    virtual void rotate(flt x,flt y,flt z);
    virtual void scale(flt x,flt y,flt z);
    virtual void moveto(flt x,flt y,flt z);
    virtual void do3d(matrix4x4 *,matrix4x4 *,tcamera*);
};


//typedef enum tlighttype{ltCONE=1,ltSPOT=2,ltINF=3}

class tlight:public tobject3d
{
    public:
    UINT8 lighttype;
    trgbfloat diffuse,ambient,specular;
    flt conesize,intensity;
    vector no,wp;  //projected rotation and worldposition
    
    
    virtual trectflt *getbounds(matrix4x4 *,tcamera*);
    virtual void do3d(matrix4x4 *,matrix4x4 *,tcamera*);
    virtual void renderhandles(UINT8 attrib,tcamera *cam);
    tlight(vector p,vector o,UINT8 type,trgbfloat *colvals);    
    tlight();    
    ~tlight();    
};



class tlightref:public telem
{
    public:
    tlight *l;
    tlightref(tlight *l_){l=l_;};
    tlightref(){l=NULL;};    
};    

//this class is the 3d cursor, used for many modelling tasks.
//pivotal Clayworks UI concept
class tcursor3d:public tobject3d
{
    public:
    vector wp;
    tcursor3d();
    virtual trectflt *getbounds(matrix4x4 *,tcamera*);
    virtual void renderhandles(UINT8 attrib,tcamera *cam);
    virtual void do3d(matrix4x4 *,matrix4x4 *,tcamera*);
};

class tgrid3d:public tobject3d
{
    public:
    flt tempgridx,gridx,tempgridy,gridy,gridz;    
    vector p;

    tgrid3d();
    void snapto(vector *v);
    tgrid3d(float gridspacex,float gridspacey,float gridspacez);
    virtual trectflt *getbounds(matrix4x4 *,tcamera*);
    void drawgrid();
    virtual void do3d(matrix4x4 *,matrix4x4 *,tcamera*);
};

//This class represents a bounding box
//The editscene class contains at least 3 of these objects 
//this class needs major revision; It should be aware of extrusion levels some how and
//provide manipulation tools for those


class tboundingbox3d:public tobject3d
{
    public:
    //trect bounds;
    vertex v[8];
    tpolygon p[6];    
    UINT32 *count;//pointer to the element count for this object
    tboundingbox3d(UINT32 *count);
    virtual trectflt *getbounds(matrix4x4 *,tcamera*);
    virtual void renderhandles(UINT8 attrib,tcamera *cam);
    virtual void do3d(matrix4x4 *,matrix4x4 *,tcamera*);
    void sortelements();
};

class tscene
{
    public:
    UINT16 drawmode;    
    UINT32 polyattribmask;//attribute
    ttree scenegraph;
    tlinkedlist tlights;
    vector cursor;
    tgrid3d *grid;
    flt fogminz;            //near fog boundry
    flt fogmaxz;            //far fog boundry
    flt fogdepth;           //depth of fog
    trgbfloat fogcolour;
    void setfog(flt,flt);    
    
    tobject3d *addobject(tobject3d* parentobject,tobject3d* newobject);
    updateselectednormalsbyvertex();  //goes first through the polygon list, then through the SVL.
    updateselectednormalsbypolygon();  //goes first through the polygon list, then through the SPL.
    updatenormals();  //update all (visible?)noramls in scene, regardless of selection
    void removeobject(tobject3d* object);

    tobject3d *cobj;  //the currently selected object
    tscene();
    ~tscene();       
    //calculation for the rgb intensity of a point at position p and normal n
    void intensity(vector &n,vector &p,material &m,trgbfloat &result);
    //perform clipping and other stuff in here
    //retrieves the bounding box for an object with minimal calculation invloved
    void setupview(tcamera *cam,UINT16 offx=0,UINT16 offy=0);
    trectflt *getbounds(tcamera *cam,tobject3d *obj,UINT16 offx=0,UINT16 offy=0);
    void preprocess(tcamera *cam,UINT16 offx=0,UINT16 offy=0);
    void preprocess_singleobject(tcamera *cam,tobject3d *obj,UINT16 offx=0,UINT16 offy=0);
    //perform rasterization into the current display context
    void render();     //full on dogs bollocks render
    void renderwire(); //tiddley wires; should use funky hash table thing to determine in lines have already been drawn
    void renderhandles(UINT8 attrib,tcamera *cam); //Mainly draws extra UI elements of objects in the scene. 
};    

//GLOBALS, mostly concerned with object creation, a bit like openGL.
//In other words, it's not badly coded, it's just a state machine!
extern UINT32 T3Dmaxpolyedges,T3Dmaxdrawnedges;
extern tcolour T3DCOLboundingbox;
extern tcolour T3DCOLselectedwire;
extern UINT32 T3Dsceneattributes,T3Dpolycreationflags,T3Ddefaultpolyattributes,T3Dselectmode,T3Ddefaultobjectattributes;
extern flt T3Dscale;
extern tpolygon *T3Dselectedpoly;
extern tlinkedlist T3Dtexturebank;
extern tlinkedlist T3Dmaterialbank;
extern matrix4x4 *T3Dctm;
extern texture *T3Dtexture;
extern material *T3Dmaterial;
extern UINT32 T3Dpolycount;
extern vector T3Dlookv;
extern tpointflt T3Doffset;
extern vector T3Dbbcentreofrotation;
extern matrix4x4 T3Dviewmat;
extern flt T3Dresistance;
extern UINT32 T3Dobjecttypemask;
extern UINT32 T3Dobjectprocessmask;
extern UINT32 T3Dobjectprocessmaskresult;
extern UINT32 T3Dedgeprocessmask;
extern UINT32 T3Dedgeprocessmaskresult;
extern UINT32 T3Dpolygonprocessmask;
extern UINT32 T3Dpolygonprocessmaskresult;
extern UINT32 T3Dvertexprocessmask; //if a bit test between the object attribute and this mask !=T3Dobjectprocessmaskresult, process the object
extern UINT32 T3Dvertexprocessmaskresult;
extern tpolygon **T3Dvisiblepolygons;
extern tedge **T3Dvisibleedges;
//this is where the 'safe' edge adding routines starts looking for
//previously added edges

extern flt T3Dboundingboxrotation;
extern tpointflt T3D2drotationpivot;
//VARIABLES DECLARED IN CLAYCP.CPP
extern UINT32 T3Dselectionfunction;
extern tpointflt T3Dscreenscale,T3Dscreencentre; 
extern trectflt T3Dviewport;

//Global library calls
UINT32 T3Dinit(UINT32 polyblocksize,UINT32 tempspacesize);
void T3Dprintstats(INT16 x,INT16 y);
void T3Dresetbuffers();
void T3Dshutdown();
void T3Dsettexture(tbitmap *pic);
textureref *T3Dgettextureref(UINT32 idx);
texture *T3Dgettexture(UINT32 idx);
texture *T3Daddtexture(texture *pic,char *name);
material *T3Daddmaterial(trgbfloat a,trgbfloat d,trgbfloat s,trgbfloat e,UINT8 specpower,flt t);

t3dmesh *T3Dloadmr(char *filename);
t3dmesh *T3Dloadlw(char *filename);
t3dmesh *T3Dloadvrml(char *filename);
t3dmesh *T3Dloadclay(char *filename);
BOOL T3Dloadnewclay(char *filename,tscene *scene);
t3dmesh *T3Dloaddxf(char *filename);
t3dmesh *T3Dload3ds(char *filename);
t3dmesh *T3Dloadplg(char *filename);
t3dmesh *T3Dloadoff(char *filename);

BOOL T3Dsavenewclay(char *filename,tscene *scene);
BOOL T3Dsavemr(char *filename,t3dmesh *obj);
BOOL T3Dsavelw(char *filename,t3dmesh *obj);
BOOL T3Dsavevrml(char *filename,t3dmesh *obj);
BOOL T3Dsaveclay(char *filename,t3dmesh *obj);
BOOL T3Dsavedxf(char *filename,t3dmesh *obj);
BOOL T3Dsave3ds(char *filename,t3dmesh *obj);
BOOL T3Dsavejava(char *filename,tscene *scene);

extern void (*T3Daddtexturecallback)(textureref *tref);
extern void (*T3Daddmaterialcallback)(material *mat);

extern tedge *T3Dcreateedge(vertex *v,vertex *v1,UINT8 *mem);
extern tpolygon *T3Dcreatepolygon(tpolygon *inpoly,UINT8 *mem);
extern tpolygon *T3Dcreatepolygon(UINT32 npoints,UINT8 *mem);
extern tpolygon *T3Dreallocpoly(tpolygon *p,UINT32 npoints);

extern tpolygon *T3Daddvertextopolygon(tpolygon *p,vertex *v,tpointflt *texv);
extern tpolygon *T3Dremovevertexfrompolygon(tpolygon *p,vertex *v);
extern tpolygon *T3Dsetpolygonvertex(tpolygon *p,vertex *v,flt tu,flt tv,UINT32 ref,UINT32 attribute,tedge *e=NULL);
extern tpolygon *T3Dsetpolygonvertex(tpolygon *p,tedge *e,flt tu,flt tv,UINT32 ref,UINT32 attribute);



//these functions add primatives to the basic shape
//as I want a "parametric object representations until otherwise screwed" scheme, more work may need to be done
//on the placement of these routines.
tedge *mksquare(t3dmesh*,flt,flt,flt,flt,flt);
tedge *mkcircle(t3dmesh*,flt cx,flt cy,flt cz,flt rx,flt ry,flt angle,int edges);
tedge *mksuperelipse2d(t3dmesh*,flt cx,flt cy,flt cz,flt rx,flt ry,flt cval1,int edges,flt n1,flt n2);                                                                                                          
vertex *mkcube(t3dmesh*,flt x1, flt y1,flt z1,flt x2, flt y2,flt z2);
vertex *mkcylinder(t3dmesh*,flt x1, flt y1,flt z1,flt z2,flt rx, flt ry, flt angle,int edgecount);
vertex *mkring(t3dmesh*,flt x1, flt y1,flt z1,flt rx, flt ry,flt rx2, flt ry2,flt angle,int edgecount);                                     
vertex *mksphere(t3dmesh*,flt x1, flt y1,flt z1,flt rx, flt ry, flt rz, flt angle,int radial,int laterial);
//super quadratics are sphereoid like shapes
//only their method of creation uses a few more paramters (powers of sin and cos, parameters n1 and n2)
//used to create more interesting shapes
//note that superelipse2d works like the mkcircle command only the 'angle' parameter controls the depth 
                                   
vertex *mksuperelipse(t3dmesh*,flt x1, flt y1,flt z1,flt rx, flt ry, flt rz, flt angle,int radial,int laterial,flt n1,flt n2);
vertex *mkcone(t3dmesh*,flt x1, flt y1,flt z1,flt z2,flt rx, flt ry,flt angle,int edgecount);
vertex *mkgrid(t3dmesh *obj,flt x1,flt y1,flt x2,flt y2,flt z,flt (*zfunc)(flt x,flt y),int gridx,int gridy,bool checkergrid);



#endif
