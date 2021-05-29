#include "stdafx.h"
#include <stdlib.h>
#include <math.h>
#include <conio.h>
#include <string.h>
#include <stdio.h>
#include "gbasics.hpp"
#include "tmaths.hpp"
#include "basic3d.hpp"
#include "imgloadr.hpp"
#include "strtable.hpp"
#include "dosstuff.hpp"
#include "gprim.hpp"
//device specific you silly sod!! refers to setpolygonmode
#include "svga64k.hpp"

#define COLOURSCALE 255.0f * 65536.0f

char nodebm[5]={124,68,68,68,124}; //pattern for nodes
//

char TEXTURE_PATH[255];

#define CLIP_PLANE_EPSILON  0.0000001      //fudge factor for clipplanes
#define NUM_FRUSTUM_PLANES 5            //the amount of clip planes

flt T3Dboundingboxrotation;
tpointflt T3D2drotationpivot;
tpolygon **T3Dvisiblepolygons,**T3Dcvisiblepoly,**T3Dlastpolygon; //visible polygon list
tedge **T3Dvisibleedges,**T3Dcvisibleedge;
UINT32 T3Dmaxedges,T3Dedgecount;
UINT32 T3Dmaxpolys,T3Dpolycount,T3Dselpolycount,T3Dtotalpolys;    //maximum polygon that can be displayed
UINT32 T3Dmaxpolyedges,T3Dmaxdrawnedges;
theapmem tempspace; //this will be used for clipped polygons and other stuff that's cleared down every frame.

//GLOBALS, mostly concearned with object creation, a bit like openGL.
//In other words, it's not badly coded, it's just a state machine!

char T3Dtempstr[256];
void (*T3Daddtexturecallback)(textureref *tref)=NULL;
void (*T3Daddmaterialcallback)(material *mat)=NULL;
tsize T3Dsize;
UINT32 T3Dsceneattributes=sfSPECULARHIGHLIGHTS|sfONLYSHOWSELECTEDNODES;   //global scene attributes
UINT32 T3Dselectmode=0;    //the current selection mode; do3d functions need to be aware of this.
UINT32 T3Dselectionfunction;  //the type of manipulation; e.g. stretch/translate, rotate, twist. etc.
UINT32 T3Dpolycreationflags,T3Ddefaultpolyattributes,T3Ddefaultobjectattributes; //polycreation flags controlsetup parameters,attributes flags determine the properties
tpolygon *T3Dselectedpoly; //currently selected poly
tpolygon *T3Dlastpolycreated;  //the last polygon that was created and added to an object
UINT32 T3Dobjecttypemask=0xFFFFFFFF;
UINT32 T3Dobjectprocessmask=ofENABLED; //if a bit test between the object attribute and this mask !=T3Dobjectprocessmaskresult, process the object
UINT32 T3Dobjectprocessmaskresult=ofENABLED;
UINT32 T3Dpolygonprocessmask=paENABLED; //if a bit test between the object attribute and this mask !=T3Dobjectprocessmaskresult, process the object
UINT32 T3Dpolygonprocessmaskresult=paENABLED;
UINT32 T3Dedgeprocessmask=eaENABLED; //if a bit test between the object attribute and this mask !=T3Dobjectprocessmaskresult, process the object
UINT32 T3Dedgeprocessmaskresult=eaENABLED;
UINT32 T3Dvertexprocessmask=naENABLED; //if a bit test between the object attribute and this mask !=T3Dobjectprocessmaskresult, process the object
UINT32 T3Dvertexprocessmaskresult=naENABLED;
UINT32 T3Dpolygonclipped;




tcolour T3DCOLgridmajor;
tcolour T3DCOLgrid;
tcolour T3DCOLwire;        //colour of wireframe polygons
tcolour T3DCOLselectedwire;//colour of selected wireframe polygons
tcolour T3DCOLselectedwireedge;
tcolour T3DCOLboundingbox; //bounding box colour
tcolour T3DCOLcursor;      //cursor colour
tcolour T3DCOLnode;        //node colour
tcolour T3DCOLselectednode;//selected node colour
flt T3Dresistance=0.5f;    //air resistance
flt T3Dspecdp=0.0f;        //internal, used in lighting calcs
vector T3Dlookv;           //the current view direction, valid after a call to tscene::preprocess 
vector T3Dcampos;          //the current camera position, valid after a call to tscene::preprocess
vector T3Dbbcentreofrotation;
flt T3Dscale=1.0;          //Scaling factor in orthographic viws
UINT8 T3Dcliphint;         //Clipping hint; generated from the sphere/frustum test
matrix4x4 T3Dviewmat;      //current view matrix

matrix4x4 UNITMAT;         //unit matrix
material WHITEMATERIAL;    //standard white material
tpointflt T3Doffset;           //x/y integer offset into the centre of the screen
matrix4x4 *T3Dctm;         //Current transformation matrix
texture *T3Dtexture;       //current texture
material *T3Dmaterial;     //current material
material *T3Dmaterialbg;   //current background material

tlinkedlist T3Dtexturebank;  //list of all textures
tlinkedlist T3Dmaterialbank; //list of all materials
  
flt FOV;                   //current field of vision, valid after a call to tscene::preprocess
tpointflt T3Dscreenscale,T3Dscreencentre; //2d scren scaling and centre
flt T3Dmaxscale;                            //biggest of x/y in screen scale
flt T3Dmaxscreenscaleinv;                   //1.0f / T3Dmaxscale. these are used to get the aspect ratio right

flt NEARZ;                                  //near clipping plane
tplane frustumplanes[5];                    //the frustum planes
trectflt T3Dviewport;
//NUM_FRUSTUM_PLANES

trgbfloat T3Dambientdefault=trgbfloat(0.1f,0.1f,0.1f);  //default ambient, specular and emmisive values
trgbfloat T3Dspeculardefault=trgbfloat(1.0f,1.0f,1.0f);
trgbfloat T3Demmisivedefault=trgbfloat(0.0f,0.0f,0.0f);

_Packed class materialentry
{
    public:
    float ar,ag,ab,
           dr,dg,db,
           sr,sg,sb,
           er,eg,eb;
    UINT8 sp;
    float st;
    float t;
};


//////////////////////////////////////////
// Engine Initialization and shutdown



//this will do something slightly more useful...
UINT32 T3Dseterror(UINT32 error)
{
    return error;
}



BOOL tedge::isclockwise()
{
   tpolygonref *pref=prefs;

   while (pref!=NULL)
   {
       if (pref->p->attribute & paSELECTED)
       {
           tpolyvertex *vp,*end,*pv;
           vp=pref->p->vlist;
           pv=&vp[pref->p->npoints-1];
           for (end=&vp[pref->p->npoints];vp<end;pv=vp++)
           {
               if (pv->v==v[0] && vp->v==v[1]) return TRUE;
               if (pv->v==v[1] && vp->v==v[0]) return FALSE;               
               /*if (v->e==this)               
                   return (v->attribute & pvaEDGEISANTICLOCKWISE)?FALSE:TRUE;                 */
           }
       }
       pref=(tpolygonref*)pref->next;
   }
   return NEITHERTRUENORFALSE;
}

tedge *T3Dcreateedge(vertex *v,vertex *v1,UINT8 *mem)
{
    tedge *newe=(mem!=NULL)?(tedge*)mem:(tedge*)malloc(sizeof(tedge));
    newe->v[0]=v;
    newe->v[1]=v1;
    if (!v->anedge)
      v->anedge=newe;    
    if (!v1->anedge)
      v1->anedge=newe;
    newe->writetag(0xFFFFFFFF);
    newe->edgeloopref=0;
    newe->edgeloopcount=0;
    newe->prefs=NULL;    
    newe->attribute=eaENABLED;
    return newe;
}

UINT32 tedge::checkintegrity()
{
    if (!v[0] || !v[1]) return eiVERTEXNULLS;
    if (v[0]->parent!=v[1]->parent) return eiVERTEXPARENTSDAMAGED;
    return eiOK;
}

BOOL tedge::sharesvertexwith(tedge *e)
{
    //if (e==this) return FALSE;
    if ((v[0]==e->v[0]) || (v[0]==e->v[1])|| (v[1]==e->v[0]) || (v[1]==e->v[1])) return TRUE;
    //if (v[0]->num==e->v[0]->num || v[0]->num==e->v[1]->num || v[1]->num==e->v[1]->num) return TRUE;
    return FALSE;
}
 

inline UINT32 T3Dpolymemsize(UINT32 npoints)
{
    return sizeof(tpolygon)+(sizeof(tpolyvertex)*(npoints-1));
}
    
tpolygon *T3Dcreatepolygon(UINT32 npoints,UINT8 *mem)
{    
    tpolygon *retpoly=(mem!=NULL)?(tpolygon*)mem:(tpolygon*)malloc(T3Dpolymemsize(npoints));    
    retpoly->tex=T3Dtexture;
    retpoly->mat=T3Dmaterial;
    retpoly->attribute=T3Ddefaultpolyattributes;
    retpoly->npoints=npoints;    
    return retpoly;        
}
//create polygon from another polygon; it is assumed to be a clipped polygon
tpolygon *T3Dcreatepolygon(tpolygon *inpoly,UINT32 npoints,UINT8 *mem)
{    
    tpolygon *retpoly=(mem!=NULL)?(tpolygon*)mem:(tpolygon*)malloc(T3Dpolymemsize(npoints));
    retpoly->next=(telem*)inpoly;
    retpoly->normal=inpoly->normal;
    retpoly->tempnormal=inpoly->tempnormal;
    //retpoly->centre=inpoly->centre;
    retpoly->tex=inpoly->tex;
    retpoly->mat=inpoly->mat;
    retpoly->attribute=inpoly->attribute|paCLIPPEDPOLY;
    retpoly->npoints=npoints;    
    return retpoly;        
}

//makes a copy of a polygon
tpolygon *T3Dcreatepolygon(tpolygon *inpoly,UINT8 *mem)
{
    UINT32 size=T3Dpolymemsize(inpoly->npoints);
    tpolygon *retpoly=(tpolygon*) ((mem!=NULL)?mem:malloc(size));
    memcpy(retpoly,inpoly,size);
    return retpoly;
}

tpolygon *T3Dreallocpoly(tpolygon *oldp,UINT32 npoints)
{
    tpolygon *p=(tpolygon*)realloc(oldp,T3Dpolymemsize(npoints));
    if (!p) return NULL;    
    //if realloc had to make a new pointer, we have to do this stuff
    if (p!=oldp)
    {
        logmessage( (void*)&textcol, "Realloc failed;pointer has been changed");
        if (T3Dselectedpoly==oldp)
           T3Dselectedpoly=p;
        //go through selected vertices and restate connections
        if ((p->attribute & paSELECTED)!=0)
        {
            //find entry in selected list and restate.
        }
        if (oldp->parent())
        {
            tpolygon *prev=(tpolygon*)p->parent()->polygonlist.prev((telem*)oldp);
            if (prev)
              prev->next=p;
        }
    }
    return p;    
}

/*tpolygon *T3Daddvertextopolygon(tpolygon *p,vertex *v,tpointflt *texv)
{
   UINT8 np=p->npoints++;
   if (!T3Dreallocpoly(p,np)) return NULL;
   p->vlist[np].texv=*texv;
   p->vlist[np].v=v;
   //add reference
   return p;
}

tpolygon *T3Dremovevertexfrompolygon(tpolygon *p,vertex *v)
{
   UINT8 vi=0;   
   INT16 np=p->npoints--;
   if (np<0) return NULL;  //can't delete something that isn't there...
   tpolyvertex *vlist=p->vlist;
   while (vlist[vi].v!=v)
   {
       if (vi>np) return NULL;    //Vertex not in the polygon so exit 
       vi++;       
   }   
   memcpy(&vlist[vi],&vlist[vi+1],(np-vi)*sizeof(tpolyvertex));  //copies over the vertex to be deleted
   //delete reference;
   return T3Dreallocpoly(p,np);   
}*/

tpolygon *T3Dsetpolygonvertex(tpolygon *p,vertex *v,flt tu,flt tv,UINT32 ref,UINT32 attribute,tedge *e)//,trgbfloat *col
{
    if (ref<p->npoints)
    {
       p->vlist[ref].texv.x=(float)tu;
       p->vlist[ref].texv.y=(float)tv;
       p->vlist[ref].v=v;
       p->vlist[ref].e=e;       
       p->vlist[ref].attribute=attribute;
       if (e) e->addreference(p);      
       return p;
    }else
       return NULL;
}

tpolygon *T3Dsetpolygonvertex(tpolygon *p,tedge *e,flt tu,flt tv,UINT32 ref,UINT32 attribute)//,trgbfloat *col
{

    if (ref<p->npoints)
    {
       p->vlist[ref].texv.x=(float)tu;
       p->vlist[ref].texv.y=(float)tv;
       vertex *v=p->vlist[ref].v=e->v[attribute & pvaEDGEISANTICLOCKWISE];
       p->vlist[ref].e=e;       
       p->vlist[ref].attribute=attribute;     
       e->addreference(p);
       return p;
    }else
       return NULL;
}


//returns false if point is less that 0
BOOL T3Dproject(vector *p3d,tpointflt *p2d,tcamera *cam)
{
    if (cam->camtype==cam3D)
    {
         
      flt temp=p3d->z;
      if (temp<NEARZ)
        return FALSE;
          //FOV can be put in the matrix...
      p2d->x=(float)((FOV*(p3d->x)/temp)+T3Doffset.x);
      p2d->y=(float)((FOV*(p3d->y)/temp)+T3Doffset.y);
    }else
    {       
       p2d->x=(float)(p3d->x);
       p2d->y=(float)(p3d->y);
    }
    return TRUE;
}



/*void T3Dsort(INT32 l,INT32 r)
{
  INT32 lop1,lop2;
  tpolygon *c;
  tpolygon **a,**b;
  flt lop3;
  //if (lop2<0) return;
       //  if (lop1>T3Dpolycount-1) return;

  lop1=l; lop2=r; lop3=T3Dvisiblepolygons[((l+r) >> 1) ]->tempcentre.z;
  a=&T3Dvisiblepolygons[lop1];
  b=&T3Dvisiblepolygons[lop2];
  
  while (lop1<=lop2)
  {
      while ((*a)->tempcentre.z>lop3)  {lop1++;a++;}
      while (lop3>(*b)->tempcentre.z)  {lop2--;b--;}
      if (lop1<=lop2)
      {
         c=*a;*a=*b;*b=c;
         a++;b--;
         lop1++;lop2--;         
      }      
  }
  if (l<lop2) T3Dsort(l,lop2);
  if (lop1<r) T3Dsort(lop1,r);
}*/

void T3Dsort(INT32 l,INT32 r)
{  
  tpolygon *c;
  tpolygon **lp,**rp,**base,**oldlp,**oldrp;
  flt lop3;
  //if (lop2<0) return;
       //  if (lop1>T3Dpolycount-1) return;

  lop3=T3Dvisiblepolygons[((l+r) >> 1) ]->centre.z;
  oldlp=lp=&T3Dvisiblepolygons[l];
  oldrp=rp=&T3Dvisiblepolygons[r];
  base=T3Dvisiblepolygons;
  
  while (lp<=rp)
  {
      while ((*lp)->centre.z>lop3)  {lp++;}
      while (lop3>(*rp)->centre.z)  {rp--;}
      if (lp<=rp)
      {
         c=*lp;*lp=*rp;*rp=c;
         lp++;rp--;         
      }      
  }
  if (oldlp<rp) T3Dsort(l,(INT32)(rp-base));
  if (lp<oldrp) T3Dsort((INT32)(lp-base),r);
}




/*class vertex:public telem
{
    public:

    UINT8 attribute; //attribute byte for latter use, not used in this proggie
    vector data3d;
    vector temp3d;
    vector normal;
    vector tempnormal;
    tpoint point2d;
    polygonref *plist;

    vertex *nxt(){return (vertex*)next;}
  */ 

//add a temporary vertexreference and new vertex to be clipped...
//this does not add a polygon reference to the new vertex; it is not needed.
//it also does not update the 'npoints' field of the polygon, it is assumed that this is setup correctly to start with.

tpolyvertex *T3Daddtempvertex(tpolygon *poly,tclipvert *vrefin,tpolyvertex *pv,UINT8 *mem)
{    
    //pv->texv=vrefin->v;

    if (vrefin->oldv==NULL)
    {           
       vertex *newv=(vertex *)mem;       
       pv->v=newv;        
       newv->tempnormal=vrefin->n;
       newv->attribute=naENABLED+naVISIBLE+naTEMPORARY;
       newv->temp3d=vrefin->v;
       //because the new clipped vertex is in world space coords, just do this.
       flt temp=vrefin->v.z;    
       newv->point2d.x=((vrefin->v.x*FOV)/temp)+T3Doffset.x;
       newv->point2d.y=((vrefin->v.y*FOV)/temp)+T3Doffset.y;       
    }
    else
       pv->v=vrefin->oldv->v;        
    return pv;
}


/////////////////////////////////////////////////////////////////////
// Set up a clip plane with the specified normal.1
/////////////////////////////////////////////////////////////////////


//Ax + By + Cz + D = 0
//where a,b and c are the normal x,y,z and distance

//Where A B C D are the co-efficients of the plane, and xyz is a point on the plane.
//Recalling that the equation of a ray is: 

//Origin + t*Direction

flt clipscale(tplane *p,vector *v1,vector *v2)
{
    vector dir=*v2-*v1;
    
    return -(p->normal.x*v1->x + p->normal.x*v1->y + p->normal.x*v1->z + p->distance)/
             (p->normal.x*dir.x + p->normal.y*dir.y + p->normal.y*dir.z);
}          

void SetWorldspaceClipPlane(vector &normal,tplane &plane)
{
    // Rotate the plane normal into worldspace
    plane.normal=normal;
    vector zero=vector(0,0,0);
    plane.distance =dot(zero,plane.normal)+CLIP_PLANE_EPSILON;
}

/*BOOL insideviewvol(tcamera *cam,vector &v)
{
    flt FOV2=cam->FOV*2;
    flt sw=(flt)(SC.size.x+1)*v.z;
    flt sh=(flt)(SC.size.y+1)*v.z;

    return ((v.z>cam->NEARZ)&&(v.z<cam->FARZ)
    &&(v.x<=(sw/FOV2))&&(v.x>=(-sw/FOV2))&&
      (v.y<=(sh/FOV2))&&(v.y>=(-sh/FOV2)));
} */   

/////////////////////////////////////////////////////////////////////
// Set up the planes of the frustum, in worldspace coordinates.
/////////////////////////////////////////////////////////////////////
void T3Dsetupfrustum()
{
    double angle, s, c;
    vector normal;
    angle = atan(2.0 * (FOV/(SC.viewport.width()+0.5)) );
    s = sin(angle);
    c = cos(angle);
    // Left clip plane
    normal.x = s;
    normal.y = 0;
    normal.z = c;
    SetWorldspaceClipPlane(normal, frustumplanes[0]);

    // Right clip plane
    normal.x = -s;
    SetWorldspaceClipPlane(normal, frustumplanes[1]);

    angle = atan(2.0 * (FOV/ (SC.viewport.height()+0.5)));
    s = sin(angle);
    c = cos(angle);

    // Bottom clip plane
    normal.x = 0;
    normal.y = s;
    normal.z = c;
    SetWorldspaceClipPlane(normal, frustumplanes[2]);

    // Top clip plane    
    normal.y = -s;
    SetWorldspaceClipPlane(normal, frustumplanes[3]);


    normal.x = 0;
    normal.y = 0;
    normal.z = 1;
        
    // Front clip plane*/
    frustumplanes[4].normal.x = 0;
    frustumplanes[4].normal.y = 0;
    frustumplanes[4].normal.z = 1;
    frustumplanes[4].distance = NEARZ;        
}


void T3Dsetupfrustumviewp()
{
    flt angle,temp;
    vector normal;

    ///////////////////////////////////////////////////////
    // X
    temp=T3Dscreencentre.x-SC.viewport.b.x;
    angle = atan(FOV/temp);
    // Left clip plane
    normal.y = 0;
    if (temp<0)
    {
       normal.x = sin(angle);
       normal.z = cos(angle);
    }else
    {
       normal.x = -sin(angle);
       normal.z = -cos(angle);
    }
    SetWorldspaceClipPlane(normal, frustumplanes[0]);

    //Right clipplane
    temp=T3Dscreencentre.x-SC.viewport.a.x;

    angle = atan(FOV/temp);
    if (temp<0)
    {
       normal.x = -sin(angle);
       normal.z = -cos(angle);
    }else
    {
       normal.x = sin(angle);
       normal.z = cos(angle);
    }
    SetWorldspaceClipPlane(normal, frustumplanes[1]);
    //frustumplanes[1].distance=-frustumplanes[1].distance;

    

    ///////////////////////////////////////////////////////
    // Y
    //bottom clipplane
    temp=T3Dscreencentre.y-SC.viewport.b.y;
    angle = atan(FOV/temp);
    normal.x = 0;
    if (temp<0)
    {
        normal.y = sin(angle);
        normal.z = cos(angle);
    }else
    {
        normal.y = -sin(angle);
        normal.z = -cos(angle);
    }
    
    SetWorldspaceClipPlane(normal, frustumplanes[2]);
    //frustumplanes[2].distance=frustumplanes[2].distance;
    temp=T3Dscreencentre.y-SC.viewport.a.y;
    
    angle = atan(FOV/temp);
    // Top clip plane
    if (temp<0)
    {
        normal.y = -sin(angle);
        normal.z = -cos(angle);
    }else
    {
        normal.y = sin(angle);
        normal.z = cos(angle);
    }
    SetWorldspaceClipPlane(normal, frustumplanes[3]);
    //frustumplanes[3].distance=-frustumplanes[3].distance;

    ///////////////////////////////////////////////////////
    // Z
    // Front clip plane
    normal.x = 0;
    normal.y = 0;
    normal.z = 1;
        
    
    frustumplanes[4].normal.x = 0;
    frustumplanes[4].normal.y = 0;
    frustumplanes[4].normal.z = 1;
    frustumplanes[4].distance = NEARZ;        
}


UINT8 T3Dsphereclipped(vector *c,flt radius)
{
    //char buf[100];
    UINT8 cliphint=0;
    for (UINT32 i=0 ; i<(NUM_FRUSTUM_PLANES);i++)//
    {
        flt dist=frustumplanes[i].distance-dot(*c, frustumplanes[i].normal);
        if ((dist>radius))   
            return scOUTSIDE;
        else if ((dist>-radius))
            cliphint|=(1<<i);
    }
    return cliphint;    
}

int T3Dcliptoplane(tclippoly *pin, tplane *pplane, tclippoly *pout)
{
    int     curin, nextin;
    flt curdot, nextdot, scale,temp;
    tclipvert *pinvert,*poutvert,*nxtvert,*end;
        
    //inside/outside test
    pout->npoints=0;
    nxtvert=pin->p;
    poutvert=pout->p;
    //distance of plane algo basically
    pinvert=(end=&nxtvert[pin->npoints])-1;
    curdot = dot(pinvert->v, pplane->normal);
    curin = (curdot >= pplane->distance);

    for (;nxtvert<end;pinvert=nxtvert++)
    {

        // Keep the current vertex if it's inside the plane        // add a new vertex here ref
        if (curin)
        {
            *poutvert=*pinvert;
            poutvert++;
            pout->npoints++;                   
        }else
           poutvert->oldv=NULL;
        //inside/outside test
        nextdot = dot(nxtvert->v, pplane->normal);
        nextin = (nextdot >= pplane->distance);        
        // Add a clipped vertex if one end of the current edge is
        // inside the plane and the other is outside
        if (curin != nextin)
        {
            //add new vertex and reference to temp poly       
            temp=nextdot-curdot;
            //get scale value from dot product minus distance of plane from origin
            scale = (pplane->distance-curdot) / temp;            
            poutvert->v.x= pinvert->v.x + ((nxtvert->v.x - pinvert->v.x) * scale);
            poutvert->v.y= pinvert->v.y + ((nxtvert->v.y - pinvert->v.y) * scale);
            poutvert->v.z= pinvert->v.z + ((nxtvert->v.z - pinvert->v.z) * scale);
            poutvert->n.x= pinvert->n.x + ((nxtvert->n.x - pinvert->n.x) * scale);
            poutvert->n.y= pinvert->n.y + ((nxtvert->n.y - pinvert->n.y) * scale);
            poutvert->n.z= pinvert->n.z + ((nxtvert->n.z - pinvert->n.z) * scale);
            poutvert->t.x= (float)pinvert->t.x + ((nxtvert->t.x - pinvert->t.x) * (float)scale);
            poutvert->t.y= (float)pinvert->t.y + ((nxtvert->t.y - pinvert->t.y) * (float)scale);            
            poutvert->oldv=NULL;
            poutvert++;
            pout->npoints++;
            T3Dpolygonclipped=true;
        }

        curdot = nextdot;
        curin = nextin;
        //get next vertex 
        pinvert++;
    }

    if (pout->npoints < 3)
        return 0;

    return 1;

}

tpolygon *T3Dcliptofrustum(tpolygon *pin)
{
    int         i, curpoly;
    tpolygon   *newpoly;
    tclippoly *ppoly;
    static tclippoly tpoly[2];  //perhaps I should have a method of rendering these things raw? Who am I asking?
    
    tpolyvertex *poutvertref,*end;
    vertex *poutvert;
    tclipvert *clipv;

    curpoly = 0;
    ppoly = &tpoly[1];
    //load the normal polygon into the clip structure    
    clipv=ppoly->p;
    ppoly->npoints=pin->npoints;
    
    for (end=&(poutvertref = pin->vlist)[pin->npoints];poutvertref<end;poutvertref++)
    {
        poutvert = poutvertref->v;        
        clipv->v=poutvert->temp3d;
        clipv->n=poutvert->tempnormal;
        clipv->e=poutvertref->e;
        clipv->t.x=poutvertref->texv.x;
        clipv->t.y=poutvertref->texv.y;
        clipv->oldv=poutvertref;
        clipv++;        
    }
    T3Dpolygonclipped=false;
    for (i=0 ; i<(NUM_FRUSTUM_PLANES);i++)
    {
        if ((T3Dcliphint & (1<<i))!=0)
        {
        if (!T3Dcliptoplane(ppoly,
                         &frustumplanes[i],
                         &tpoly[curpoly & 1]))                
            return NULL;
        
        ppoly = &tpoly[curpoly];
        curpoly ^= 1;
        }
    }
    if (!T3Dpolygonclipped)
      return(pin);
    clipv=ppoly->p;
    UINT32 polysize=T3Dpolymemsize(ppoly->npoints);
    UINT8 *mem=(UINT8*)tempspace.grab(polysize+(ppoly->npoints*sizeof(vertex)));
    if (!mem) return NULL;
    newpoly=T3Dcreatepolygon(pin,ppoly->npoints,mem);
    vector newcentre(0.0);
    newpoly->next=(telem *)pin;
    mem+=polysize;
    if (newpoly)
    {
        for (i=0;i<ppoly->npoints;i++)
        {
            T3Daddtempvertex(newpoly,clipv,&newpoly->vlist[i],mem);
            newcentre+=clipv->v;
            newpoly->vlist[i].attribute=pin->vlist[i].attribute;
            clipv->e->attribute |=eaVISIBLE; //this edge is visible

            
            newpoly->vlist[i].e=clipv->e;
            clipv++;
            mem+=sizeof(vertex);
        }        
        newpoly->centre=newcentre;
    }
    return newpoly;        
}


UINT32 T3Dgettextureindex(texture *tex)
{
    UINT32 count=0;
    textureref *ctex=(textureref*)T3Dtexturebank.first;
    while (ctex!=NULL)
    {
        if (ctex->t==tex)
          return count;
        count++;
        ctex=(textureref*)ctex->next;
    }
    return 0;
}


UINT32 T3Dgetmaterialindex(material *mat)
{
    UINT32 count=0;
    material *cmat=(material*)T3Dmaterialbank.first;
    while (cmat!=NULL)
    {
        if (cmat==mat)
          return count;
        count++;
        cmat=(material*)cmat->next;
    }
    return 0;
}


material *T3Daddmaterialifnotfound(material *mat,BOOL makenew)
{
    material *mref=(material*)T3Dmaterialbank.first;
    while (mref!=NULL)
    {
        if (*mref==*mat)                    
            return mref;        
        mref=mref->nextm();
    }
    material *nmat;
    if (makenew)
    {
       nmat=new material;
       *nmat=*mat;
    }else    
       nmat=mat;
       
    T3Dmaterialbank.push((telemptr)nmat);
    if (T3Daddmaterialcallback)
       T3Daddmaterialcallback(nmat);
    return nmat;
}

material *T3Daddmaterial(materialentry *mat)
{
    material lmat;
    lmat.ambient=trgbfloat(mat->ar,mat->ag,mat->ab);
    lmat.diffuse=trgbfloat(mat->dr,mat->dg,mat->db);
    lmat.specular=trgbfloat(mat->sr,mat->sg,mat->sb);
    lmat.emmisive=trgbfloat(mat->er,mat->eg,mat->eb);
    lmat.specpower=mat->sp;
    lmat.st=mat->st;
    lmat.transparency=mat->t;        
    return T3Daddmaterialifnotfound(&lmat,TRUE);
}


material *T3Daddmaterial(trgbfloat a,trgbfloat d,trgbfloat s,trgbfloat e,UINT8 specpower,flt t)
{
    material lmat;    
    lmat.next=NULL;
    lmat.st=0.13;
    lmat.ambient=a;
    lmat.diffuse=d;
    lmat.specular=s;
    lmat.emmisive=e;
    lmat.specpower=specpower;
    lmat.transparency=t;       
    return T3Daddmaterialifnotfound(&lmat,TRUE);    
}         

material *T3Daddmaterial(trgbfloat &d,UINT8 specpower,flt t)
{       
    material lmat;
    lmat.ambient=T3Dambientdefault;
    lmat.diffuse=d;
    lmat.st=0.5;
    lmat.specular=T3Dspeculardefault;
    lmat.emmisive=T3Demmisivedefault;
    lmat.specpower=specpower;
    lmat.transparency=t;
    lmat.next=NULL;
    return T3Daddmaterialifnotfound(&lmat,TRUE);
}

material *T3Daddmaterial(material *mat)
{
    return T3Daddmaterialifnotfound(mat,FALSE);
}





texture *T3Daddtexture(texture *pic,char *name)
{
    textureref *tref=(textureref*)T3Dtexturebank.first;        
    /*while (tref!=NULL)
    {
        if (!strcmp(name,tref->name))        
            return tref->t;        
        tref=tref->nextt();
    }*/
    tref=new textureref;
    if ((!tref)||(!pic)) return NULL;
    tref->t=pic;
    strcpy(tref->name,name);        
    T3Dtexturebank.push((telemptr)tref);
    if (T3Daddtexturecallback)
       T3Daddtexturecallback(tref);    

    return tref->t;
}    

void T3Dsettexture(tbitmap *pic)
{
    T3Dtexture=(texture*)pic;
    (pic!=NULL)?T3Ddefaultpolyattributes|=paTEXTURED:T3Ddefaultpolyattributes&=~paTEXTURED;
    //SETBIT(T3Ddefaultpolyattributes,paTEXTURED,pic!=NULL);    
}

textureref *T3Dgettextureref(UINT32 idx)
{
    if (idx==0) return NULL;
    idx--;
    textureref *retref=(textureref*)T3Dtexturebank.first;
    while ((retref!=NULL)&&(idx!=0))
    {
        retref=retref->nextt();
        idx--;        
    }
    return retref;
}

texture *T3Dgettexture(UINT32 idx)
{
    if (idx==0) return NULL;
    idx--;
    textureref *retref=(textureref*)T3Dtexturebank.first;
    while ((retref!=NULL)&&(idx!=0))
    {
        retref=retref->nextt();
        idx--;        
    }
    return retref->t;
}
    

void T3Dprintstats(INT16 x,INT16 y)
{
    char buf[100];
    sprintf(buf,"Polycount %i/%i",T3Dpolycount,T3Dtotalpolys);
    outtextxy(x,y,buf);
    sprintf(buf,"clip status %i %i %i %i %i %i",T3Dcliphint & 1,T3Dcliphint & 2,T3Dcliphint & 4,T3Dcliphint & 8,T3Dcliphint & 16,T3Dcliphint & 32);\
    outtextxy(x,y+=20,buf);
}    
   
UINT32 T3Dinit(UINT32 polyblocksize,UINT32 tempspacesize)
{
    T3Dmaxpolys=polyblocksize;
    T3Dmaxedges=polyblocksize<<2;
    T3Dpolycount=T3Dtotalpolys=0;
    T3Dedgecount=0;
    T3Dvisiblepolygons=(tpolygon**)malloc(polyblocksize<<2);
    T3Dvisibleedges=(tedge**)malloc(T3Dmaxedges<<2);
    if (!T3Dvisiblepolygons) return T3Dseterror(0);
    strcpy(TEXTURE_PATH,getenv("VRBITMAPS"));
    if (TEXTURE_PATH[0]==0)
       strcpy(TEXTURE_PATH,EXE_PATH);   
    if (TEXTURE_PATH[strlen(TEXTURE_PATH)-1]!='\\')
       strcat(TEXTURE_PATH,"\\");
    T3Dcvisiblepoly=T3Dvisiblepolygons;

    T3Dlastpolygon=T3Dvisiblepolygons+(polyblocksize<<2);

    T3Dtexturebank.init(sizeof(textureref));
    T3Dmaterialbank.init(sizeof(material));
    
    WHITEMATERIAL.ambient=trgbfloat(0.0f,0.0f,0.0f);
    WHITEMATERIAL.diffuse=trgbfloat(0.5f,0.5f,0.5f);
    WHITEMATERIAL.specular=trgbfloat(1.0f,1.0f,1.0f);
    WHITEMATERIAL.specpower=2;
    WHITEMATERIAL.st=0.5;
    WHITEMATERIAL.emmisive=trgbfloat(0.02f,0.02f,0.02f);
    WHITEMATERIAL.transparency=0.0f;
    T3Dpolycreationflags=cfANTICLOCKWISE;
    T3Ddefaultpolyattributes=paENABLED;
    T3Ddefaultobjectattributes=ofENABLED|ofVISIBLE|ofEDITABLE;
    if (GFX_INITIALIZED)
    {
    T3DCOLwire=getcolour(200,200,255);
    T3DCOLselectedwireedge=getcolour(0,255,255);
    T3DCOLselectedwire=getcolour(255,255,50);
    T3DCOLboundingbox=getcolour(255,0,0);
    T3DCOLcursor=getcolour(255,0,0);    
    T3DCOLnode=getcolour(250,250,255);
    T3DCOLselectednode=getcolour(255,255,0);
    T3DCOLgrid=getcolour(50,140,140);
    T3DCOLgridmajor=getcolour(70,180,180);
    }      
    T3Dtexture=NULL;
    T3Dmaterial=&WHITEMATERIAL;

    if (!tempspace.create(tempspacesize))return 0;
    return 1;
}

void destroytexturecontents(telem *tex)
{
    delete (((textureref*)tex)->t);
}    



//You should also sort the polygons on insert (it's faster) by several modes:
//Nearest Z (best case for zbuffer)
//Minz and Maxz; make a judgement based on these two values, polys with larger
//spans should go first
//And none; Don't sort polygon list



inline tpolygon *T3Daddvisiblepolygon(tpolygon *poly)
{
    if ((T3Dcvisiblepoly+1)>T3Dlastpolygon) return NULL;    
    *(T3Dcvisiblepoly++)=poly;
    
    T3Dpolycount++;
    if (poly->attribute & paSELECTED)
      T3Dselpolycount++;          
    return poly;
}


                

inline tpolygon *T3Daddvisibleedges(tpolygon *poly)
{
    tpolyvertex *vr=poly->vlist;
    tpolyvertex *end=&vr[poly->npoints];
    tedge *e;
    
    UINT32 attribute;
    for (;vr<end;vr++)
    {
        e=vr->e;
        if (e)
        {
             attribute=e->attribute;             
             if (((attribute & T3Dedgeprocessmask)==T3Dedgeprocessmaskresult)&& (attribute & eaNOTDRAWNYET) )
             {
                 e->attribute &=~eaNOTDRAWNYET;
                 *(T3Dcvisibleedge++)=e;
                 T3Dedgecount++;
             }
        }
    }
    return poly;
}


void T3Dresetbuffers()
{
    T3Dpolycount=T3Dtotalpolys=T3Dselpolycount=0;
    T3Dmaxpolyedges=T3Dmaxdrawnedges=0;
    T3Dedgecount=0;
    tempspace.reset();
    T3Dcvisiblepoly=T3Dvisiblepolygons;
    T3Dcvisibleedge=T3Dvisibleedges;    
}


void T3Dshutdown()
{
    free(T3Dvisiblepolygons);

    T3Dtexturebank.destroy(destroytexturecontents);
    T3Dmaterialbank.destroy();    
    tempspace.destroy();
}



///////////////////////////////////////////////////////////////////////////////////////
// TPolygon: will replace polygon; better as it doesn't have so many mad linked lists!

t3dmesh *tpolygon::parent()
{
    if (npoints==0) return NULL;
    t3dmesh *mesh=vlist[0].v->parent;
    //have to check to see if each vertex has the same owner; otherwise it could cause problems
    for (UINT32 i=1;i<npoints;i++)
    {
        if (vlist[i].v->parent!=mesh) return NULL;
        //should pick this up and then check each possible candidate for parenthood
    }
    return mesh;
}


void tpolygon::copyoldvertices()
{
    tpolyvertex *v,*end;
    for (end=&(v=vlist)[npoints];v<end;v++)
        v->v->old=v->v->data3d;                      
}

BOOL tpolygon::removeedgereferences()
{
    tpolyvertex *v,*end;
    for (end=&(v=vlist)[npoints];v<end;v++)    
        v->e->deletereferences(this);            
    return TRUE;
}

UINT32 tpolygon::checkintegrity()
{    
    tpolygonref *plist;
    tpolyvertex *v,*end;
    v=vlist;
    t3dmesh *parent=v->v->parent;
    UINT32 edgereport;
    for (end=&v[npoints];v<end;v++)
    {
        if (v->v->parent!=parent)
          return piVERTEXPARENTSDAMAGED;
        edgereport=v->e->checkintegrity();
        if (edgereport!=0)
          return piEDGESDAMAGED;
        plist=(tpolygonref*)v->e->prefs;
        while (plist!=NULL)
        {
            if (plist->p==this) break;
            plist=(tpolygonref *)plist->next;
        }
        if (plist==NULL)
          return piMISSINGEDGEREFERENCE;
       
    }
    return piOK;
}
 

void tpolygon::calcpolynormal()
{
    if (npoints>2)
    {
        calcnormal(vlist[npoints-1].v->data3d,vlist[0].v->data3d,vlist[1].v->data3d,normal);
   }
}

void tpolygon::reverse()
{
    tpolyvertex swapme;
    
    for (UINT32 i=0;i< npoints>>1;i++)
    {
        swapme=vlist[i];
        vlist[i]=vlist[npoints-i-1];
        vlist[npoints-i-1]=swapme;                        
    }
    normal.x=-normal.x;
    normal.y=-normal.y;
    normal.z=-normal.z;       
}
    

BOOL tpolygon::contains(vertex *cv)
{
    tpolyvertex *v=vlist;
    for (int i=npoints;i>0;i--,v++)    
        if (v->v==cv) return TRUE;

    
    return FALSE;

}

BOOL tpolygon::checkvertexattributes(UINT32 attr)
{
    tpolyvertex *v=vlist;
    for (int i=npoints;i>0;i--,v++)    
      if (v->v->attribute & attr) return TRUE;
    return FALSE;    
}
//returns the number of vertices that matched the criteria
UINT32 tpolygon::checkallvertexattributes(UINT32 attr)
{
    tpolyvertex *v=vlist;
    UINT32 count=0;
    for (int i=npoints;i>0;i--,v++)    
      if (v->v->attribute & attr) count++;
    return count;    
}


void tpolygon::or_vertexrefattributes(UINT32 attr)
{
    tpolyvertex *v,*end;
    for (end=&(v=vlist)[npoints];v<end;v++)
        v->attribute |=attr;
}
void tpolygon::and_vertexrefattributes(UINT32 attr)
{
    tpolyvertex *v,*end;
    for (end=&(v=vlist)[npoints];v<end;v++)
        v->attribute &=attr;
}

void tpolygon::xor_vertexrefattributes(UINT32 attr)
{
    tpolyvertex *v,*end;
    for (end=&(v=vlist)[npoints];v<end;v++)
        v->attribute ^=attr;
}
void tpolygon::or_vertexattributes(UINT32 attr)
{
    tpolyvertex *v,*end;
    for (end=&(v=vlist)[npoints];v<end;v++)
        v->v->attribute |=attr;
}
void tpolygon::and_vertexattributes(UINT32 attr)
{
    tpolyvertex *v,*end;
    for (end=&(v=vlist)[npoints];v<end;v++)
        v->v->attribute &=attr;
}

void tpolygon::xor_vertexattributes(UINT32 attr)
{
    tpolyvertex *v,*end;
    for (end=&(v=vlist)[npoints];v<end;v++)
        v->v->attribute ^=attr;
}

void tpolygon::or_edgeattributes(UINT32 attr)
{
    tpolyvertex *v,*end;
    for (end=&(v=vlist)[npoints];v<end;v++)
        v->e->attribute |=attr;
}


void tpolygon::and_edgeattributes(UINT32 attr)
{
    tpolyvertex *v,*end;
    for (end=&(v=vlist)[npoints];v<end;v++)
        v->e->attribute &=attr;
}

void tpolygon::xor_edgeattributes(UINT32 attr)
{
    tpolyvertex *v,*end;
    for (end=&(v=vlist)[npoints];v<end;v++)
        v->e->attribute ^=attr;
}



BOOL tpolygon::allverticesclipped()
{
    tpolyvertex *v=vlist;
    for (int i=npoints;i>0;i--,v++)    
        if ((v->v->attribute & naVISIBLE)!=0) return FALSE;
    return TRUE;
}


BOOL tpolygon::someverticesclipped()
{
    tpolyvertex *v=vlist;
    for (int i=npoints;i>0;i--,v++)    
        if ((v->v->attribute & naVISIBLE)!=0) return TRUE;
    return FALSE;
}

BOOL tpolygon::noverticesclipped()
{
    tpolyvertex *v=vlist;
    for (int i=npoints;i>0;i--,v++)    
        if ((v->v->attribute & naVISIBLE)==0) return FALSE;
    return TRUE;
}


BOOL tpolygon::containsedge(vertex *from,vertex *to)
{
    tpolyvertex *v=vlist;
    tpolyvertex *vj,*end;
    vj=end=&vlist[npoints];
    vj--;
    while (v<end)
    {
        if ( (vj->v==from && v->v==to) || (vj->v==to && v->v==from))
          return true;
        vj=v;
        v++;        
    }
    return false;
}


//////////////////////////////////////////
// Basic 3d object

tobject3d::tobject3d()
{        
    //setup for tree stuff  
    next = parent = child = NULL;
    attribute=T3Ddefaultobjectattributes;    
    unitpos();
    //mass=1.0;
    //maxspeed=30.0f;
    //minspeed=-30.0f;
    min=max=0;
    radius=0;
}

tobject3d::~tobject3d()
{
    
}


char *tobject3d::setname(char *name_)
{
    strcpy(name,name_);
    return name;
}

//speed, 
#define MAXSPEED 300

void tobject3d::updatedatafull()
{
  //updatedata();
  tobject3d *currobj=(tobject3d*)child;  
  while (currobj!=NULL)
  {
      currobj->updatedatafull();
      currobj=(tobject3d*)currobj->next;    
  }
}


void tobject3d::updatedata()
{
  vector thrust,diff;
  flt decby=mass*T3Dresistance;
  vector dp=vector(a,0,0);
  dp.multmatrix(&localmat,&thrust);
  d+=thrust;
  spd=d.length();
  if (spd>0)
  {
    spd-=decby;
    if (spd<0) spd=0;
  }
  else
  {
    spd+=decby;
    if (spd>0) spd=0;
  }
  BOUND(spd,minspeed,maxspeed);
  d.normalize();
  d*=spd;

  localmat.p+=d;
  p=localmat.p;
}


//when parents change, must call another function to traverse down the heirarchy
//also looks down the heirarchy to sort out any misbehaving children.
//this should be called whenever the localmat matrix is changed!

trectflt *tobject3d::getbounds(matrix4x4 *cmat,tcamera*cam)
{
    return &bounds;
}

void tobject3d::calcworldmat()
{
   if (parent)
   {
     LMconcatenatematrices(&tparent()->localmat,&localmat,&worldmat);
     LMconcatenatematrices(&tparent()->localmattr,&localmattr,&worldmattr);
     //LMconcatenatematrices(&tparent()->localmattr,&localmattr,&localmattr);   
   }else
   {
     worldmat=localmat;
     worldmattr=localmattr;
   }
   tobject3d *childloop=tchild();
   while (childloop!=NULL)
   {
     tchild()->calcworldmat();
     childloop=childloop->tnext();
   }    
  
}


void tobject3d::lookat(vector pt,flt twist)
{        
    flt r, theta, fee;
    pt-=p;
    r = pt.length();
    if (r==0) return;
    //the following code works for rottrans() rather than the inverted versiom
    //This is specifically for camera transforms;
    //Y rotation first feels more intuative.
    fee = atan2 (pt.y,pt.z);             
    simplexrot((float)-fee,&pt);
    theta =-atan2 (pt.x, pt.z);    

    /*theta =-atan2 (pt.x, pt.z);    
    simpleyrot(-theta,&pt);
    fee = atan2 (pt.y,pt.z);             */
    
    rotate(fee,theta,o.z);
}

void tobject3d::lookat2(vector pt,flt twist)
{        
    flt r, theta, fee;
    pt-=p;
    r = pt.length();
    if (r==0) return;
    //the following code works for rottrans() rather than the inverted versiom
    //This is specifically for camera transforms;
    //Y rotation first feels more intuative.

    theta =-atan2 (pt.x, pt.z);    
    simpleyrot((float)-theta,&pt);
    fee = atan2 (pt.y,pt.z);
        
    o.moveto(fee,theta,0);    
    LMrottrans(&localmattr,o.x,o.y,o.z,p.x,p.y,p.z);
    localmat=localmattr;
    calcworldmat();
        
}



/*void tobject3d::lookat2(vector pt,flt twist)
{
    matrix4x4 zmat;
    vector oldp=pt;
    flt r, theta, fee;//, ct, st, cf, sf;
    // vector zero length, collapse the world!
    pt-=p;
    r = pt.length();
    if (r==0) return;
    

    fee = atan2 (pt.y,pt.z);             
    doxrot(-theta,&pt);
    theta = -atan2 (pt.x, pt.z);
    

    /*sprintf(buf,"ydiff:%0.4f zdiff:%0.4f",pt.y,pt.z);
    label(0,0,buf);
    sprintf(buf,"theta :%0.4f fee:%0.4f",theta,fee);
    label(0,20,buf);*/
      
    /*if (pt.z>=0)      
       fee   = asin (pt.y/r);
    else
       fee   = 0;//acos (pt.y/r);*/
    
    //fee = -asin (pt.y/r);    

    //rotate(-fee,-theta,twist);

    //rotate(theta,fee,twist);
    /*ct = cos (theta);
    st = sin (theta);
    cf = cos (fee);
    sf = sin (fee);

    localmat.m00 = -st;  localmat.m01 = -ct*cf ;  localmat.m02 = ct*sf;
    localmat.m10 = ct;  localmat.m11 = -st*cf;  localmat.m12 = st*sf; 
    localmat.m20 = 0.0f;  localmat.m21 = sf;  localmat.m22 =  cf;*
    
    LMzrotation(&zmat,twist);
    o.x=fee;
    o.y=theta;
    o.z=twist;
    
    LMconcatenatematrices(&localmat,&zmat,&localmat);    */



void tobject3d::do3d(matrix4x4 *cmat,matrix4x4 *cmattr,tcamera *cam)
{    
    
}


//whereas do3d just calculates the 3d for that object,
//this wrapper method with traverse down the heirarchy.
//NOTE: the root node must be traversed in an external loop
//as this only processes the children of a given node
void tobject3d::do3dfull(matrix4x4 *cmat ,matrix4x4 *cmattr,tcamera *cam)
{
  //updatedata();
  //loadup data into the temporary matrix
  //LMconcatenatematrices(cmat,&localmat,&worldmat);
  
  //then use this to calculate 2d coordinates, clipping and so on.
  if ((type & T3Dobjecttypemask)!=0)
  if ((attribute & T3Dobjectprocessmask)==T3Dobjectprocessmaskresult)//
  {
     matrix4x4 tmpmat;
     matrix4x4 tmpmattr;
     //preconcatinate cameraspace matrix to objects world matrix
     LMconcatenatematrices(cmat,&worldmat,&tmpmat);
     LMconcatenatematrices(cmattr,&worldmattr,&tmpmattr);   
     do3d(&tmpmat,&tmpmattr,cam);
     tobject3d *currobj=(tobject3d*)child;
     while (currobj!=NULL)
     {
        currobj->do3dfull(cmat,cmattr,cam);
        currobj=(tobject3d*)currobj->next;    
     }
  }
  
  //LMtranspose(&tmpmat);  
}

//as with the above function this wrapper method with traverse down the heirarchy
//instead for the UI components of 3d objects (polygons are handled seperately although
//they can also be considered UI elements when selecting by polygons in the 3d view.
//sometime paradigms aren't as neat as they should be :(
//NOTE: the root node must be traversed in an external loop
//as this only processes the children of a given node

void tobject3d::renderhandlesfull(UINT8 attr,tcamera *cam)
{
  if ((type & T3Dobjecttypemask)!=0)
  if ((attribute & T3Dobjectprocessmask)==T3Dobjectprocessmaskresult)
  {
      renderhandles(attr,cam);
      tobject3d *currobj=(tobject3d*)child;  
      while (currobj!=NULL)
      {
          currobj->renderhandlesfull(attr,cam);
          currobj=(tobject3d*)currobj->next;    
      }
  }
}





void tobject3d::unitpos()
{
    //spd=a=0.0f;
    o=p=vector(0.0f,0.0f,0.0f);
    s=vector(1.0f,1.0f,1.0f);    
    LMidentity(&localmat);
}
    
void tobject3d::rotate(flt x,flt y,flt z)
{
    o.moveto(x,y,z);        
    LMrottrans(&localmattr,o.x,o.y,o.z,p.x,p.y,p.z);
    LMaddscale(&localmattr,&localmat,s.x,s.y,s.z);     
    calcworldmat();
}
    
void tobject3d::scale(flt x,flt y,flt z)
{
    s.moveto(x,y,z);
    LMrottrans(&localmattr,o.x,o.y,o.z,p.x,p.y,p.z);
    LMaddscale(&localmattr,&localmat,s.x,s.y,s.z);     
    calcworldmat();
}

void tobject3d::moveto(flt x,flt y,flt z)
{
    p.moveto(x,y,z);
    LMrottrans(&localmattr,o.x,o.y,o.z,p.x,p.y,p.z);
    LMaddscale(&localmattr,&localmat,s.x,s.y,s.z);     
    calcworldmat();
}

void tobject3d::oncollision()
{
}

//used for rendering nodes on an object mesh, the object centre (perhaps) and other user interactable parts
void tobject3d::renderhandles(UINT8 attrib,tcamera *cam)
{
    
}

//////////////////////////////////////////
// Geometry



t3dmesh::t3dmesh()
{
    sprintf(name,"geom");
    type=otGEOM;       
    vertexlist.init(sizeof(vertex));    
    polygonlist.init(sizeof(tpolygon));
    edgelist.init(sizeof(tedge));
}

t3dmesh::~t3dmesh()
{
   vertexlist.destroy(NULL);   
   polygonlist.destroy(NULL);
   edgelist.destroy(NULL);
}




void calc2dbound(tpoint *p,vector v,tpoint *min2d,tpoint *max2d,matrix4x4 *m)
{
    p->x=float2int((v.x*m->m00 +v.y*m->m01 +v.z*m->m02) +m->p.x);
    p->y=float2int((v.x*m->m10 +v.y*m->m11 +v.z*m->m12) +m->p.y);
    if (p->x<min2d->x) min2d->x=p->x;
    if (p->y<min2d->y) min2d->y=p->y;
    if (p->x>max2d->x) max2d->x=p->x;
    if (p->y>max2d->y) max2d->y=p->y;

}

void calc2d(tpoint *p,vector *v,matrix4x4 *m)
{
    p->x=float2int((v->x*m->m00 +v->y*m->m01 +v->z*m->m02) +m->p.x);
    p->y=float2int((v->x*m->m10 +v->y*m->m11 +v->z*m->m12) +m->p.y);
}


//calculate objects radius and 2d bounds
trectflt *t3dmesh::getbounds(matrix4x4 *cmat,tcamera* cam)
{
    UINT32 intrad;
    flt bs=0;
    vector trad;
    if (bs<fabs(s.x)) bs=fabs(s.x);
    if (bs<fabs(s.y)) bs=fabs(s.y);
    if (bs<fabs(s.z)) bs=fabs(s.z);
    
  
    if (cam->camtype==cam3D)
    {
        flt temp=cmat->p.z;
        if (temp<NEARZ)          
           temp=NEARZ;
        //FOV can be put in the matrix...
        p2d.x=((FOV*(cmat->p.x)/temp)+T3Doffset.x);
        p2d.y=((FOV*(cmat->p.y)/temp)+T3Doffset.y);
        intrad=float2int((tempradius=radius*bs));
        //intrad=FOV*(tempradius)/temp;

    }else
    {
        p2d.x=(cmat->p.x);
        p2d.y=(cmat->p.y);
                intrad=float2int((tempradius=T3Dscale*radius*bs));       
    }
    intrad+=4;  //selection nodes cause it to go over the bounds slightly; this compensates.
    bounds.rassign(p2d.x-intrad,p2d.y-intrad,p2d.x+intrad,p2d.y+intrad);
    return &bounds;      
}




/*                               V = Any_vertex - Eye_position
                               N = Polygon normal

                               If(N.V < 0) back_facing triangle
                               Else front_facing triangle

                               Note these vectors do not need to be normalised as we are only interested in the sign of the dot
                               product, not the magnitude. It's always worked for me.
*/

void t3dmesh::clearvertexandedgeflags()
{
   vertex *v=(vertex*)vertexlist.first;  
   tedge *e=(tedge *)edgelist.first;

    while (v!=NULL)
    {
       v->attribute &=~(naPERSPECTIVECALCED+naVISIBLE);
       v=v->nxt();
    }
    while (e!=NULL)
    {
       e->attribute |= eaNOTDRAWNYET;
       e=(tedge*)e->next;
    }
}

void t3dmesh::do3d(matrix4x4 *cmat,matrix4x4 *cmattr,tcamera *cam)
{
  UINT8 n_clipped; //counter for clipped verticies.  
  flt temp;
  
  //INT16 dx1,dy1,dx2,dy2;
  tpolygon *pl=(tpolygon*)polygonlist.first;
  tpolygon *wpl;//working poly  
  tpolyvertex *vr,*end;
  tpoint p1,p2,p3;
  
  vector p=cam->p;  
  vector data3d,normal;
  matrix4x4 cmatfov=*cmat;
  //cmattr.m00/=s.x;
  //cmattr.m11/=s.y;
  //cmattr.m22/=s.z;


  //this rotates the camera normal so that it is in un-transformed object space
  //the dot product of this and the untransformed polygon normals are then used for backface removal
  //using 3 muls and 3 adds. Possibly more expensive than the 2 adds, 2 subs and 2 muls in the usual
  //method but then there's less setup code and less complexity so it's probably faster (and less buggy!)
  //learned of a new method using the viewers eye POSITION, must try it out.
  //also, backface culling could be performed before the vertices are transformed!
  vector lv;
  T3Dlookv.multmatrixinv(&worldmattr,&lv);
  
  /*lv.x=(-worldmat.m20);
  lv.y=(-worldmat.m21);
  lv.z=(-worldmat.m22);*/
              
 
  //test object bounds against tcamera view fustrum in here
  T3Dtotalpolys+=polygonlist.count;                                      
  getbounds(cmat,cam);  
  attribute &=~ofVISIBLE; //clear the flag, in case it was set the last time around
  
  vertex *v;  

  
  if (cam->camtype==cam3D)
  {      
      T3Dcliphint=T3Dsphereclipped(&cmat->p,tempradius);
      if (T3Dcliphint!=scOUTSIDE)
      {
          attribute |=ofVISIBLE;
          clearvertexandedgeflags();
          while (pl!=NULL)
          {
              pl->attribute &= ~(paBACKFACING+paVISIBLE);                           

              vector vp=lv;//-pl->vlist->v->data3d;
              UINT8 facing = (dot(pl->normal,vp)>=0.0f);
              if ((facing)||(pl->attribute & paDOUBLESIDED))
              {
                  if (!facing)
                     pl->attribute |=paBACKFACING;                                        

                  wpl=NULL;
                  n_clipped=0;                                    
                  /*V = Any_vertex - Eye_position
                               N = Polygon normal

                               If(N.V < 0) back_facing triangle
                               Else front_facing triangle*/
                  pl->centre=0.0f;
                  
                  //vector centre=vector(0.0f,0.0f,0.0f);
                  vr=pl->vlist;
                                                                   
                  for (end=&(vr=pl->vlist)[pl->npoints];vr<end;vr++)
                  {
                      v=vr->v;
                      vr->e->attribute &=~(eaVISIBLE);                      
                      if ((v->attribute & naPERSPECTIVECALCED)==0)
                      {
                          v->normal.multmatrix(cmattr,&v->tempnormal);
                          v->data3d.multmatrixtrans(cmat,&v->temp3d);
                          temp=v->temp3d.z;
                          if (temp>NEARZ)
                          {
                              temp=1.0/temp *FOV;
                              v->point2d.x=(v->temp3d.x*temp)+T3Doffset.x;
                              v->point2d.y=(v->temp3d.y*temp)+T3Doffset.y;
                              if ((T3Dcliphint==scINSIDE)||(T3Dviewport.rcontains(v->point2d)))
                                 v->attribute |= naVISIBLE;                             
                              else
                              {
                                 v->attribute &= ~naVISIBLE;
                                 n_clipped++;
                              }
                          }else
                          {
                              v->attribute &= ~naVISIBLE;
                              n_clipped++;
                          }
                          v->attribute|=naPERSPECTIVECALCED;
                      }else if ((v->attribute & naVISIBLE)==0)
                          n_clipped++;                      
                      pl->centre+=v->temp3d;                                                              
                  }
                  /*pl->normal.multmatrix(cmattr,&pl->tempnormal);
                  vector vp=T3Dlookv-pl->vlist->v->temp3d;
                  UINT8 facing = (dot(pl->tempnormal,vp)>=0.0f);
                  if (!facing && pl->attribute & paDOUBLESIDED)
                    pl->attribute |=paBACKFACING;                                        
                  if (facing)
                  {*/
                          
                  if ((T3Dcliphint==scINSIDE)||(n_clipped==0))
                  {            
                     wpl=pl;
                     pl->attribute |=paALLVERTICESVISIBLE;
                  }else                     
                     wpl=T3Dcliptofrustum(pl);
                  if (wpl!=NULL)              
                  {
                      pl->attribute|=paVISIBLE;
                      wpl->attribute|=paVISIBLE;                                            
                      vector normal=pl->normal;
                      normal.multmatrix(cmattr,&wpl->tempnormal);                      
                      wpl->centre/=wpl->npoints;
                      #ifdef SHOW_NORMALS                      
                      temp=1.0/wpl->centre.z*FOV;
                      wpl->c2d.x=(wpl->centre.x*temp)+T3Doffset.x;
                      wpl->c2d.y=(wpl->centre.y*temp)+T3Doffset.y;                                                     
                      
                      centre=wpl->centre+(wpl->tempnormal*4);
                      temp=1.0/centre.z*FOV;

                      wpl->n2d.x=(centre.x*temp)+T3Doffset.x;
                      wpl->n2d.y=(centre.y*temp)+T3Doffset.y;                                                     
                      #endif                      
                      T3Daddvisiblepolygon(wpl);
                      //T3Daddvisibleedges(wpl);
                  }
              }                                            
              pl=pl->nxt();
          }
      }
  }else
  {     
     if (T3Dviewport.roverlap(bounds))
     {
         attribute |=ofVISIBLE;
         clearvertexandedgeflags();
         BOOL allinside=bounds.rinside(T3Dviewport);  //Only check for clipping if bounds overlaps the edges of Sc.viewport         
         trectflt prect;
         v=(vertex*)vertexlist.first;
         while (v!=NULL)
         {
             if ((v->attribute & naPERSPECTIVECALCED)==0)
             {
                 data3d=v->data3d;
                 v->point2d.x=(data3d.x*cmatfov.m00 +data3d.y*cmatfov.m01 +data3d.z*cmatfov.m02) +cmatfov.p.x;
                 v->point2d.y=(data3d.x*cmatfov.m10 +data3d.y*cmatfov.m11 +data3d.z*cmatfov.m12) +cmatfov.p.y;
                 if (allinside)                         
                    v->attribute |= naVISIBLE;                             
                 else
                 {
                    if ((T3Dviewport.rcontains(v->point2d)))                                                                               
                        v->attribute |= naVISIBLE;                             
                    else                            
                        v->attribute &= ~naVISIBLE;                                                           
                 }
                 v->attribute|=naPERSPECTIVECALCED;
             } 
             v=v->nxt();    
         }
         while (pl!=NULL)
         {
             pl->attribute &=~paVISIBLE;                     
             if ((pl->attribute & T3Dpolygonprocessmask)==T3Dpolygonprocessmaskresult)
             {
                 prect.rassign(32000.0f,32000.0f,-32000.0f,-32000.0f);                                                   
                 //calculate 3d coordinates
                 pl->attribute|=paALLVERTICESVISIBLE;
                 for (end=&(vr=pl->vlist)[pl->npoints];vr<end;vr++)
                 {
                     v=vr->v;
                     if (!allinside)
                     {                          
                        tpointflt p=v->point2d;                                     
                        if (p.x<prect.a.x)
                           prect.a.x=p.x;
                        else if (p.x>prect.b.x)
                           prect.b.x=p.x;                        
                        if (p.y<prect.a.y)
                           prect.a.y=p.y;
                        else if (p.y>prect.b.y)
                           prect.b.y=p.y;
                        if ((v->attribute & naVISIBLE)==0)
                           pl->attribute&=~paALLVERTICESVISIBLE;
                     } 
                 }
                 
                 if ((allinside)||(T3Dviewport.roverlap(prect)))
                 {
                     pl->attribute |=paVISIBLE;                                         
                     T3Daddvisiblepolygon(pl);
                     T3Daddvisibleedges(pl);                     
                 }
             }
             pl=pl->nxt();
         }
     }else     
         pl=NULL;           
  }                                         

}
//draws the nodes, object centre etc.
void t3dmesh::renderhandles(UINT8 attrib,tcamera *cam)
{
    tpointflt c;
    t_col=T3DCOLselectednode;
    if (cam->camtype!=cam3D)        
       circle(float2int(p2d.x),float2int(p2d.y),3);
    
    
    vertex *v=(vertex*)vertexlist.first;
    //circle(p2d.x,p2d.y,tempradius);
    //rectangle(bounds);
    
    /*line(cube[0].x,cube[0].y,cube[1].x,cube[1].y);
    line(cube[1].x,cube[1].y,cube[2].x,cube[2].y);
    line(cube[2].x,cube[2].y,cube[3].x,cube[3].y);
    line(cube[3].x,cube[3].y,cube[0].x,cube[0].y);

    line(cube[4].x,cube[4].y,cube[5].x,cube[5].y);
    line(cube[5].x,cube[5].y,cube[6].x,cube[6].y);
    line(cube[6].x,cube[6].y,cube[7].x,cube[7].y);
    line(cube[7].x,cube[7].y,cube[4].x,cube[4].y);*/
    
    if (T3Dselectmode==etVERTEX)
    if (attribute&ofVISIBLE) //anything visible?
    {
       //do check on attrib (ie, render all nodes or render only selected ones)
       while (v!=NULL)
       {
           if ((v->attribute & T3Dvertexprocessmask)==T3Dvertexprocessmaskresult)
           {
               if ((v->attribute & naSELECTED)!=0) //individual check
               {
                   t_col=T3DCOLselectednode;
                   c.x=float2int(v->point2d.x);
                   c.y=float2int(v->point2d.y);
                   drawbytes(float2int(c.x-3.0),float2int(c.y-3.0),1,5,nodebm);
               }else if ((T3Dsceneattributes & sfONLYSHOWSELECTEDNODES)==0)
               {
                   t_col=T3DCOLnode;
                   c.x=float2int(v->point2d.x);
                   c.y=float2int(v->point2d.y);
                   drawbytes(float2int(c.x-3.0),float2int(c.y-3.0),1,5,nodebm);
               }
               
           }
           v=v->nxt();
       }
    }
}

void t3dmesh::settexture(texture *tex,UINT8 selectedonly)
{
    tpolygon *cpoly;
    cpoly=(tpolygon*)polygonlist.first;
    while (cpoly!=NULL)
    {
        //if (((cpoly->attribute&paSELECTED)!=0)||(selectedonly==0))
        cpoly->tex=tex;
        (tex!=NULL)?cpoly->attribute|=paTEXTURED:cpoly->attribute&=~paTEXTURED;
        cpoly=cpoly->nxt();
    }    
}

void t3dmesh::setmaterial(material *mat,UINT8 selectedonly)
{
    tpolygon *cpoly;
    cpoly=(tpolygon*)polygonlist.first;
    while (cpoly!=NULL)
    {
        if (((cpoly->attribute&paSELECTED)!=0)||(selectedonly==0))
           cpoly->mat=mat;
        cpoly=cpoly->nxt();
    }
}

void t3dmesh::setpolyattributes(UINT32 attrib,UINT8 selectedonly)
{
    tpolygon *cpoly;
    cpoly=(tpolygon*)polygonlist.first;
    while (cpoly!=NULL)
    {
        if (((cpoly->attribute&paSELECTED)!=0)||(selectedonly==0))
        cpoly->attribute=attrib;
        cpoly=cpoly->nxt();
    }
}

void t3dmesh::setvertexattributes(UINT32 attrib,UINT8 selectedonly)
{
    vertex *cvertex=(vertex*)vertexlist.first;
    while (cvertex!=NULL)
    {
        if (((cvertex->attribute&naSELECTED)!=0)||(selectedonly==0))
        cvertex->attribute=attrib;
        cvertex=(vertex*)cvertex->next;
    }    
}

void t3dmesh::and_vertexattributes(UINT32 attribute)
{
    vertex *cvertex=(vertex*)vertexlist.first;
    while (cvertex!=NULL)
    {        
        cvertex->attribute&=attribute;
        cvertex=(vertex*)cvertex->next;
    }
}

void t3dmesh::or_vertexattributes(UINT32 attribute)
{
    vertex *cvertex=(vertex*)vertexlist.first;
    while (cvertex!=NULL)
    {        
        cvertex->attribute|=attribute;
        cvertex=(vertex*)cvertex->next;
    }
}

void t3dmesh::and_edgeattributes(UINT32 attribute)
{
    tedge *cedge=(tedge *)edgelist.first;
    while (cedge!=NULL)
    {        
        cedge->attribute&=attribute;
        cedge=(tedge*)cedge->next;
    }
}

void t3dmesh::or_edgeattributes(UINT32 attribute)
{
    tedge *cedge=(tedge *)edgelist.first;
    while (cedge!=NULL)
    {        
        cedge->attribute|=attribute;
        cedge=(tedge*)cedge->next;
    }
}




void t3dmesh::calculatenormals(UINT32 polymask,UINT32 vertexmask)
{    
    tpolygon *cpoly=(tpolygon*)polygonlist.first;
    
    vertex *cvertex=(vertex*)vertexlist.first;
    
    vector n=vector(0,0,0);
    
    while (cvertex)
    {
        if ((cvertex->attribute & vertexmask)!=0)
            cvertex->normal=n;
        cvertex=cvertex->nxt();
    }
    while (cpoly)
    {
        if ((cpoly->attribute & polymask)!=0)
        {
             cpoly->calcpolynormal();
             for (UINT32 i=0;i<cpoly->npoints;i++)
             {
                vertex *v=cpoly->vlist[i].v;
                if (v->attribute & vertexmask)                                
                   v->normal+=cpoly->normal;
             }
        }
        cpoly=cpoly->nxt();        
    }
    vector calced;
    min.moveto(30000,30000,30000);
    max.moveto(-30000,-30000,-30000);    
    radius=0;
    flt tmp;
    cvertex=(vertex*)vertexlist.first;
    while (cvertex)
    {
        calced=cvertex->data3d;               
        if (min.x>calced.x) min.x=calced.x;
        if (min.y>calced.y) min.y=calced.y;
        if (min.z>calced.z) min.z=calced.z;
        if (max.x<calced.x) max.x=calced.x;
        if (max.y<calced.y) max.y=calced.y;
        if (max.z<calced.z) max.z=calced.z;
        
        tmp=calced.magnitude();
        if (radius<tmp)
            radius=tmp;                
        if (cvertex->attribute & vertexmask)                
            cvertex->normal.normalize();
                            
        cvertex=cvertex->nxt();
    }
    //the almighty fudge factor
    
    radius=sqrt(radius);
    radius+=radius*0.05;
    /*vector absmax=vector(MAX(abs(min.x),max.x),
                         MAX(abs(min.y),max.y),
                         MAX(abs(min.z),max.z));
    radius=absmax.length();*/
    
    //radius*=2.0;
   
}

vertex *vertex::nxt()
{
    if (this==NULL)
    {
        logmessage( (void*)&textcol, "bad call to next pointer");
        return NULL;
    }
    return (vertex*)next;
}

UINT32 tedge::selectedvertexcount()
{
    UINT32 retcount=0;
    if (v[0]->attribute & naSELECTED) retcount++;
    if (v[1]->attribute & naSELECTED) retcount++;
    return retcount;
}

tpolygon *tedge::addreference(tpolygon *p)
{
    if (!p) return NULL;
    tpolygonref *pr=prefs;
    while (pr)
    {
        if (pr->p==p) return NULL;
        pr=(tpolygonref*)pr->next;        
    }    
    SLadd((telem**)&prefs,new tpolygonref(p));
    return p;
}

BOOL tedge::deletereferences(UINT32 attribute)
{
    tpolygonref *pref,*prev,*next;
    prev=NULL;
    pref=prefs;
    while (pref!=NULL)
    {
        next=(tpolygonref*)pref->next;
        if ((pref->p->attribute & attribute)==attribute)
        {
          if (prev)
            prev->next=(tpolygonref *)next;
          else
            prefs=(tpolygonref *)next;
          delete pref;
        }
        prev=pref;
        pref=(tpolygonref *)next;
    }
    return TRUE;
}

BOOL tedge ::deletereferences(tpolygon *poly)
{
    tpolygonref *pref,*prev,*next;
    prev=NULL;
    pref=prefs;
    while (pref!=NULL)
    {
        next=(tpolygonref*)pref->next;
        if (pref->p==poly)
        {
          if (prev)
            prev->next=(tpolygonref *)next;
          else
            prefs=(tpolygonref *)next;
          delete pref;
          return TRUE;
        }
        prev=pref;
        pref=(tpolygonref *)next;
    }
    return FALSE;
}

vertex *t3dmesh::getvertex(UINT32 idx)
{
    vertex *cvertex=(vertex*)vertexlist.first;
    while (idx>0)
    {        
        cvertex=cvertex->nxt();
        idx--;
    }
    return cvertex;     
}

UINT32 vertcount=0; 

vertex *t3dmesh::addvertex(flt x1,flt y1,flt z1,vertex *addvertex)
{    
    vector calced(x1,y1,z1);        
    addvertex->parent=this;
    addvertex->attribute=naENABLED;
    addvertex->anedge=NULL;
    addvertex->num=vertcount++;
    if (T3Dctm)
       LMmatrixtransformvec(T3Dctm,&calced,&addvertex->data3d);
    else
       addvertex->data3d=calced;
    vertexlist.push(addvertex);    
    return addvertex; //was successfull, return the obj
}

vertex *t3dmesh::addvertex(flt x1,flt y1,flt z1)
{
    vertex *addvertex=new vertex();
    vector calced(x1,y1,z1);        
    addvertex->parent=this;
    addvertex->attribute=naENABLED;
    addvertex->anedge=NULL;
    addvertex->num=vertcount++;
    if (T3Dctm)
       LMmatrixtransformvec(T3Dctm,&calced,&addvertex->data3d);
    else
       addvertex->data3d=calced;
    vertexlist.push(addvertex);    
    return addvertex; //was successfull, return the obj
}

//cutdown quick version
vertex *t3dmesh::addvertexsimp(flt x1,flt y1,flt z1,vertex *addvertex)
{    
    addvertex->data3d.moveto(x1,y1,z1);      
    addvertex->parent=this;
    addvertex->num=vertcount++;
    addvertex->anedge=NULL;
    addvertex->attribute=naENABLED;           
    vertexlist.push(addvertex);    
    return addvertex; //was successfull, return the obj
}



tpointflt *RotateTextureVecs(tpointflt *txvecs,UINT16 count,UINT16 angle)
{

    UINT16 i;
    UINT16 iv;
    tpointflt temppnt;
    if (!txvecs) return NULL;
    for (i=0; i<angle;i++)
    {
        temppnt=txvecs[count-1];
        for (iv=0;iv<count-1;iv++)
            txvecs[iv+1]=txvecs[iv];
        txvecs[0]=temppnt;
    }
    return txvecs;
}

tpointflt *FlipTextureVecsX(tpointflt *txvecs,UINT16 count, texture *tex)
{
    int i;
    if ((!txvecs)||(!tex)) return NULL;
    for (i=0;i<count;i++)
      txvecs[i].x=tex->size.x-txvecs[i].x;
    return txvecs;  
}    
        

tpointflt *FlipTextureVecsY(tpointflt *txvecs,int count, texture *tex)
{
    int i;
    if ((!txvecs)||(!tex)) return NULL;
    for (i=0;i<count;i++)
      txvecs[i].y=tex->size.y-txvecs[i].y;
    return txvecs;  
}            

void T3Dapplytexturemanip(tpointflt *txvecs,int count,texture *tex,int T3Dpolycreationflags)
{
    if (T3Dpolycreationflags & cfFLIPTEXTUREX)
        FlipTextureVecsX(txvecs,count,tex);
    if (T3Dpolycreationflags & cfFLIPTEXTUREY)
        FlipTextureVecsY(txvecs,count,tex);
    if (T3Dpolycreationflags & cfROTATETEXTUREMASK)
        RotateTextureVecs(txvecs,count,(T3Dpolycreationflags & cfROTATETEXTUREMASK)>>cfROTATETEXTURESHIFTRIGHT);
}
    
    


//this adds a vertex reference to the polygon list and also add the polygon to the vertex's reference list.
//UV values are also loaded

tedge *t3dmesh::addedge(vertex *v,vertex *v1,UINT8 *mem)
{
    tedge *newe=T3Dcreateedge(v,v1,mem);
    edgelist.push(newe);
    return newe;
}
/*
tedge *t3dmesh::addedgesearch(vertex *v,vertex *v1,UINT8 *mem,tedge **list,UINT32 count)
{
    tedge *newe=T3Dcreateedge(v,v1,mem);
    edgelist.push(newe);
    return newe;
}*/

tedge *t3dmesh::addedgesearch(vertex *v,vertex *v1,UINT8 *reversed,UINT8 *mem)
{
    *reversed=0;
    tedge *newe=(tedge*)edgelist.first;    

    while (newe!=NULL)
    {
        if (newe->v[0]==v && newe->v[1]==v1) return newe;
        if (newe->v[1]==v && newe->v[0]==v1) {*reversed=pvaEDGEISANTICLOCKWISE;return newe;};        
        newe=(tedge *)newe->next;
    }
    newe=T3Dcreateedge(v,v1,mem);
    edgelist.push(newe);
    return newe;
}


tedge *t3dmesh::addedgeloop(vertex *v,UINT32 count,UINT8 *mem)
{
    
    if (count<2 || v==NULL) return NULL;
    UINT32 i;
    tedge *retedge;
    vertex *nextv=(vertex *)v->next;
    if (!nextv) return NULL;
    vertex *firstv=v;    
    count--;
    retedge=(tedge*)edgelist.getlast();
    for(i=0;i<count;i++)
    {        
        addedge(v,nextv,mem);
        if (mem) mem+=sizeof(tedge);        
        v=nextv;
        nextv=nextv->nxt();
    }
    addedge(v,firstv,mem);
    return (tedge*)edgelist.getnext_check(retedge);
}

BOOL assignedgelisttopolygon(tedge *edge,tpolygon *p,BOOL reverse)
{
    int i=p->npoints;
    tpolyvertex *v=p->vlist;
    if (reverse)
      v=&v[i-1];
    while (i--)
    {        
        if (edge->v[0]==v->v)
          v->attribute &= ~pvaEDGEISANTICLOCKWISE;
        else if (edge->v[1]==v->v)        
          v->attribute |= pvaEDGEISANTICLOCKWISE;
        else
          return FALSE;
        v->e=edge;
        edge->addreference(p);
        edge=(tedge*)edge->next;
        if (reverse)
          v--;
        else
          v++;
    }
    return TRUE;
}



tpolygon* t3dmesh::addpoly(UINT32 count,UINT8 *mem)
{
    tpolygon *newp=T3Dcreatepolygon(count,mem);
    T3Dlastpolycreated=newp;
    polygonlist.push(newp);
    return newp;      
}    

tpolygon* t3dmesh::addpoly(vertex *startv,tpointflt *txvecs,UINT32 count,UINT8 *mem)
{
    UINT32 i;
    tpolygon *newp=T3Dcreatepolygon(count,mem);
    T3Dlastpolycreated=newp;
    for (i=0;i<count;i++)
    {
        T3Dsetpolygonvertex(newp,startv,txvecs->x,txvecs->y,i,0);
        startv=(vertex*)startv->nxt();
        txvecs++;
    }
    
    if (T3Dpolycreationflags & cfANTICLOCKWISE)
       newp->reverse();
    polygonlist.push(newp);    
    return newp;      
}


tpolygon* t3dmesh::addquad(vertex *v1,vertex *v2,vertex *v3,vertex *v4,tpointflt *txvecs,UINT8 *mem)
{
    tpolygon *newp=T3Dcreatepolygon(4,mem);       

    T3Dsetpolygonvertex(newp,v1,txvecs[0].x,txvecs[0].y,0,0);
    T3Dsetpolygonvertex(newp,v2,txvecs[1].x,txvecs[1].y,1,0);
    T3Dsetpolygonvertex(newp,v3,txvecs[2].x,txvecs[2].y,2,0);
    T3Dsetpolygonvertex(newp,v4,txvecs[3].x,txvecs[3].y,3,0);
    
    if (T3Dpolycreationflags & cfANTICLOCKWISE)
       newp->reverse();        
    polygonlist.push(newp);

    return newp;       
}

tpolygon* t3dmesh::addtriangle(vertex *v1,vertex *v2,vertex *v3,tpointflt *txvecs,UINT8 *mem)
{    
    tpolygon *newp=T3Dcreatepolygon(3,mem);
    T3Dlastpolycreated=newp;

    T3Dsetpolygonvertex(newp,v1,txvecs[0].x,txvecs[0].y,0,0);
    T3Dsetpolygonvertex(newp,v2,txvecs[1].x,txvecs[1].y,1,0);
    T3Dsetpolygonvertex(newp,v3,txvecs[2].x,txvecs[2].y,2,0);
    
    if (T3Dpolycreationflags & cfANTICLOCKWISE)
       newp->reverse();        
    polygonlist.push(newp);    
    return newp;
}



void printvertex(vertex *vert,int i)
{
    /*if (vert==NULL)        
        //outconsole("It's a null");
    else
    {*/
        char txt[255];
        sprintf(txt,"pos no:%i= %d %d %d",i,vert->data3d.x,vert->data3d.y,vert->data3d.z);
        //outconsole(txt);
    //}  
}

//for now, this assumes that e1 and e2 are connectable; i.e. the polygon created between them will
//be clockwise
tpolygon *t3dmesh::singleedgeconnect(tedge *lefte,tedge *righte,tpointflt *texv,tedge *tope,tedge *bottome)
{
    tpolygon *newp=T3Dcreatepolygon(4,NULL);
    UINT8 reversed=0;
    //this sets the edges given above to the polygon vertices
    //the edges are given as top,bottom,left and right;
    //If they don't share vertices, you'll know about it because it'll look all wrong!
    //edges top and bottom must be given in the correct order
    //thus, this is a private routine! (Danger Will Robinson, etc)
    if (!tope)
      tope=addedgesearch(lefte->v[0],righte->v[0],&reversed);    
    T3Dsetpolygonvertex(newp,tope,texv[0].x,texv[0].y,0,reversed);
    reversed=0;
    if (!bottome)
      bottome=addedgesearch(lefte->v[1],righte->v[1],&reversed);        
    T3Dsetpolygonvertex(newp,bottome,texv[2].x,texv[2].y,2,reversed^pvaEDGEISANTICLOCKWISE);    
    T3Dsetpolygonvertex(newp,righte,texv[1].x,texv[1].y,1,0);    
    T3Dsetpolygonvertex(newp,lefte,texv[3].x,texv[3].y,3,pvaEDGEISANTICLOCKWISE);
    
    if (T3Dpolycreationflags & cfANTICLOCKWISE)
       newp->reverse();        
    polygonlist.push(newp);
    return newp;       
}

//for now, this assumes that e1 and e2 are connectable; i.e. the polygon created between them will
//be clockwise
tpolygon *t3dmesh::singleedgeconnect(tedge *lefte,tpointflt *texv,tedge *tope,tedge *bottome)
{
    tpolygon *newp=T3Dcreatepolygon(3,NULL);
    T3Dsetpolygonvertex(newp,tope,texv[0].x,texv[0].y,2,pvaEDGEISANTICLOCKWISE);
    T3Dsetpolygonvertex(newp,bottome,texv[1].x,texv[1].y,1,0);
    T3Dsetpolygonvertex(newp,lefte,texv[2].x,texv[2].y,0,0);
    
    if (T3Dpolycreationflags & cfANTICLOCKWISE)
       newp->reverse();        
    polygonlist.push(newp);
    return newp;       
}


void t3dmesh::edgeconnect(tedge *start1,tedge *start2,tpointflt *between,UINT32 edgecount)
{    
    UINT32 i;
    UINT8 ti;
    tpointflt txvecs[3];
    flt addby;
    flt txw=1.0f,txh=1.0f;
    if ((start1==NULL)||(start2==NULL))return;    
    tedge *topedge=NULL,*bottomedge=NULL,*firsttopedge;    
    addby = txw / (flt)edgecount;
    txvecs[1].y=txvecs[2].y=txh * between->y;
    txvecs[0].y=txvecs[3].y=txw * between->x;    
    txvecs[1].x=txvecs[0].x=0;
    //hehe, work this one out.... I love C.
    //bascially, if the wrap texture flag is set, it sets the
    //'x' texture coords (to the right) to either a fraction of the width, or the width itself
    txvecs[2].x=txvecs[3].x=((T3Dpolycreationflags & cfWRAPTEXTURE)!=0)?addby:txw;
    firsttopedge=topedge=addedge(start1->v[0],start2->v[0]);

    for (i=0;i<edgecount-1;i++)
    {
        bottomedge=addedge(start1->v[1],start2->v[1]);        
        singleedgeconnect(start1,start2,txvecs,topedge,bottomedge);
        topedge=bottomedge;
        start1=(tedge *)start1->next;
        start2=(tedge *)start2->next;
        if (T3Dpolycreationflags & cfWRAPTEXTURE) 
        for(ti=1;ti<4;ti++)
                txvecs[ti].x+=addby;                                     
    }        
    singleedgeconnect(start1,start2,txvecs,bottomedge,firsttopedge);
    
}

void t3dmesh::edgeconnect(tedge *start1,vertex *v,tpointflt *between,UINT32 edgecount)
{    
    UINT32 i;
    UINT8 ti;
    tpointflt txvecs[3];
    flt addby;
    flt txw=1.0f,txh=1.0f;
    if ((start1==NULL)||(v==NULL))return;    
    tedge *topedge,*bottomedge,*firsttopedge;    
    addby = txw / (flt)edgecount;
    txvecs[0].y=txw * between->x;
    txvecs[1].y=txvecs[2].y=txh * between->y;    
    txvecs[1].x=txvecs[0].x=0;    
    txvecs[2].x=((T3Dpolycreationflags & cfWRAPTEXTURE)!=0)?addby:txw;
    firsttopedge=topedge=addedge(start1->v[0],v);    
    for (i=0;i<edgecount-1;i++)
    {
        bottomedge=addedge(start1->v[1],v);    
        singleedgeconnect(start1,txvecs,topedge,bottomedge);
        topedge=bottomedge;
        start1=(tedge *)start1->next;
        if (T3Dpolycreationflags & cfWRAPTEXTURE) 
        for(ti=1;ti<3;ti++)
             txvecs[ti].x+=addby;                                     
    }

    singleedgeconnect(start1,txvecs,bottomedge,firsttopedge);    
}

tedge *mksquare(t3dmesh *obj,flt px1,flt py1,flt px2,flt py2,flt pz)
{
    tpointflt txvecs[4];
    vertex *firstvertex;
    
        
    firstvertex=obj->addvertex(px1,py1,pz);
    obj->addvertex(px2,py1,pz);
    obj->addvertex(px2,py2,pz);
    obj->addvertex(px1,py2,pz);
    txvecs[0].x=0.0f;  txvecs[0].y=0.0f;
    txvecs[1].x=1.0f;  txvecs[1].y=0.0f;
    txvecs[2].x=1.0f;  txvecs[2].y=1.0f;
    txvecs[3].x=0.0f;  txvecs[3].y=1.0f;
    tedge *e;tpolygon *p;

    e=obj->addedgeloop(firstvertex,4);
    if (!(T3Dpolycreationflags & cfNOPOLYS))
    {               
        p=obj->addpoly(firstvertex,txvecs,4);
        assignedgelisttopolygon(e,p,T3Dpolycreationflags & cfANTICLOCKWISE);            
    }
     
    return e;    
}

tedge *mkcircle(t3dmesh *obj,flt cx,flt cy,flt cz,flt rx,flt ry,flt angle,int edgecount)
{
    return mksuperelipse2d(obj,cx,cy,cz,rx,ry,angle,edgecount,1.0f,1.0f);
}




vertex *mkcube(t3dmesh *obj,flt x1, flt y1,flt z1,flt x2, flt y2,flt z2)
{
   UINT32 OLDCF=T3Dpolycreationflags;
   tpointflt between;   
   tedge *e1,*e2;
   between.x=0;between.y=1;
   if (z1<z2) T3Dpolycreationflags ^= cfANTICLOCKWISE;
   
   e1=mksquare(obj,x1,y1,x2,y2,z1);   
   T3Dpolycreationflags ^= cfANTICLOCKWISE | cfFLIPTEXTUREY;
   e2=mksquare(obj,x1,y1,x2,y2,z2);
   T3Dpolycreationflags ^= cfANTICLOCKWISE | cfFLIPTEXTUREY;
   
   obj->edgeconnect(e1,e2,&between,4);
   T3Dpolycreationflags=OLDCF;
   return NULL;
}                                       


vertex *mkcylinder(t3dmesh *obj,flt x1, flt y1,flt z1,flt z2,flt rx, flt ry, flt angle,int edgecount)                                     
{
   UINT32 OLDCF=T3Dpolycreationflags;
   
   tpointflt between(0.0f,1.0f);
   
   tedge *e1,*e2;
      
   if (z1>z2) T3Dpolycreationflags ^= cfANTICLOCKWISE;
   T3Dpolycreationflags|=cfNOPOLYS;   
   e1=mkcircle(obj,x1,y1,z1,rx,ry,angle,edgecount);
   e2=mkcircle(obj,x1,y1,z2,rx,ry,angle,edgecount);
   T3Dpolycreationflags ^=cfNOPOLYS;   
   obj->edgeconnect(e1,e2,&between,edgecount);   
   mkcircle(obj,x1,y1,z1,rx,ry,angle,edgecount);
   T3Dpolycreationflags ^=cfANTICLOCKWISE|cfFLIPTEXTUREX;   
   mkcircle(obj,x1,y1,z2,rx,ry,angle,edgecount);
   T3Dpolycreationflags = OLDCF; //restore polyflags
   return NULL;
}

vertex *mkring(t3dmesh *obj,flt x1, flt y1,flt z1,flt rx, flt ry,flt rx2, flt ry2,flt angle,int edgecount)                                     
{
   UINT32 OLDCF=T3Dpolycreationflags;   
   tpointflt between(0.0f,1.0f);
   tedge *e1,*e2;
   
   T3Dpolycreationflags|=cfNOPOLYS;
   T3Dpolycreationflags|=cfFLIPTEXTUREY|cfFLIPTEXTUREX;   
   e1=mkcircle(obj,x1,y1,z1,rx,ry,angle,edgecount);      
   e2=mkcircle(obj,x1,y1,z1,rx2,ry2,angle,edgecount);   
   T3Dpolycreationflags&=~cfNOPOLYS;
   obj->edgeconnect(e1,e2,&between,edgecount);
   T3Dpolycreationflags = OLDCF; //restore polyflags
   return NULL;
}


                                
vertex *mksphere(t3dmesh *obj,flt x1, flt y1,flt z1,flt rx, flt ry, flt rz, flt angle,int radial,int laterial)
{
    return mksuperelipse(obj,x1,y1,z1,rx,ry,rz,angle,radial,laterial,1.0,1.0);
}

flt safepow(flt val,flt power)
{
    if (power==0.0) return val;
    if (val<0)
      return -pow(-val,power);    
    return pow(val,power);
}


tedge *mksuperelipse2d(t3dmesh *obj,flt cx,flt cy,flt cz,flt rx,flt ry,flt cval1,int edgecount,flt n1,flt n2)
{
    if (edgecount<3) return NULL;
    //logmessage( (void*)&textcol,"entered mkcircle");
    tpointflt txvecs[255];
    int i;
    flt txw=2*PI,txh=2*PI;
    vertex *firstvertex,*cv;
        
    flt cval2=-PI;
    flt delta2= (2*PI)/edgecount;
    //logmessage( (void*)&textcol,"entering main loop mkcircle");
    firstvertex=(vertex*)obj->vertexlist.getlast();    
    for (i=0;i<edgecount;i++)
    {

        if ((T3Dpolycreationflags & cfNOPOLYS)==0)
        {   
           txvecs[i].x=(sin(cval2)/txw)+0.5;BOUND(txvecs[i].x,0.0f,1.0f);
           txvecs[i].y=(cos(cval2)/txh)+0.5;BOUND(txvecs[i].y,0.0f,1.0f);           
        }
        
        flt pwrcos1=safepow(cos(cval1),n1);
        cv=obj->addvertex(cx+pwrcos1*safepow(cos(cval2),n2)*rx,cy+pwrcos1*safepow(sin(cval2),n2)*ry,cz);
        cval2+=delta2;
        //logmessage( (void*)&textcol,"added vertex");         
    }
    firstvertex=(vertex*)obj->vertexlist.getnext_check(firstvertex);
    tedge *e;tpolygon *p;
    e=obj->addedgeloop(firstvertex,edgecount);
    if (!(T3Dpolycreationflags & cfNOPOLYS))
    {
        p=obj->addpoly(firstvertex,txvecs,edgecount);
        assignedgelisttopolygon(e,p,T3Dpolycreationflags & cfANTICLOCKWISE);
    }//logmessage( (void*)&textcol,"Done");                 
    return e;                               
}

vertex *mksuperelipse(t3dmesh *obj,flt x1, flt y1,flt z1,flt rx, flt ry, flt rz, flt angle,int radial,int laterial,flt n1,flt n2)
{  
   UINT32 OLDCF=T3Dpolycreationflags;
   tedge *e1,*e2=NULL;
   vertex *v;
   flt addby,cval,delta,zpos;

   int i;
   laterial--;
   addby=(1.0f / (flt)laterial);
   tpointflt between(1.0,1.0-addby);  
   
   delta= (PI)/laterial;
   cval=-PI/2.0f;   
   zpos=z1 +(safepow(sin(cval),n1) *rz);
   //logmessage( (void*)&textcol,"entered mksphere");
   
   v=obj->addvertex(x1,y1,zpos);
   cval+=delta;       
   
   T3Dpolycreationflags |= cfNOPOLYS;
   T3Dpolycreationflags ^= cfANTICLOCKWISE;   
   
   for (i=1;i<laterial;i++)
   {
       zpos=z1 + (safepow(sin(cval),n1) *rz);
       e1=e2;       
       e2=mksuperelipse2d(obj,x1,y1,zpos,rx,ry,cval,radial,n1,n2);       
       if (i==1)
          obj->edgeconnect(e2,v,&between,radial);       
       if (i>1)
          obj->edgeconnect(e1,e2,&between,radial);
       between.x-=addby;
       between.y-=addby; 
       cval+=delta;       
   }
   zpos=z1 + (safepow(sin(cval),n1) *rz);
   v=obj->addvertex(x1,y1,zpos);
   T3Dpolycreationflags=OLDCF; 
   obj->edgeconnect(e2,v,&between,radial); 
   
   //logmessage( (void*)&textcol,"done\n");       
   return NULL;    
}


vertex *mkgrid(t3dmesh *obj,flt x1,flt y1,flt x2,flt y2,flt z,flt (*zfunc)(flt x,flt y),int gridx,int gridy,bool checkergrid)
{
    /*tpointflt txvecs[4];
    material *mats[2];
    mats[0]=T3Dmaterial;
    if (T3Dmaterial->nextm())
      mats[1]=T3Dmaterial->nextm();
    else
      mats[1]=(material *)T3Dmaterialbank.first;
    
    flt xdiff=x2-x1,ydiff=y2-y1;
    tpointflt step;
    vertex *firstrow=(vertex*)obj->vertexlist.last;
    vertex *secondrow,*v;
    int yi,xi,ti;
    for (yi=0;yi<gridy;yi++)
    {
        step.y=yi/(float)gridy;
        
        for (xi=0;xi<gridx;xi++)
        {            
            step.x=xi/(float)gridx;                                                           
            v=obj->addvertex(x1+(xdiff*step.x),y1+(ydiff*step.y),(zfunc)?z+zfunc(step.x,step.y):z);
            if (xi==0)
            {
            if (yi==0)
               firstrow=v;
            else if (yi==1)            
               secondrow=v;
            }
        }        
    }
    step.x=1.0f/gridx;
    step.y=1.0f/gridy;
    
    txvecs[0].y=step.y;    
    txvecs[1].y=step.y;    
    txvecs[2].y=0.0f;
    txvecs[3].y=0.0f;    

    vertex *firstvertex=firstrow;
    //vertex *firstvertex=firstrow=firstrow->nxt();
    //secondrow=secondrow->nxt();
    if (!firstrow) return NULL;
    for (yi=0;yi<gridy-1;yi++)
    {
        txvecs[0].x=0.0f;txvecs[1].x=step.x;txvecs[2].x=step.x;txvecs[3].x=0.0f;

        int index=yi & 1;
        for (xi=0;xi<gridx-1;xi++)
        {
            if (checkergrid)
              T3Dmaterial=mats[index];
            obj->addquad(secondrow,secondrow->nxt(),firstrow->nxt(),firstrow,txvecs);
            
            index ^=1;            
            firstrow =firstrow ->nxt();
            secondrow=secondrow->nxt();
            for (ti=0;ti<4;ti++)
                txvecs[ti].x+=step.x;
        }
        for (ti=0;ti<4;ti++)
           txvecs[ti].y+=step.y;
        firstrow =firstrow ->nxt();
        secondrow=secondrow->nxt();
    }
    return firstvertex;    */
    return NULL;
}     
                    

/*vertex *t3dmesh::mktorus(vector centre,flt rx1,flt ry1,flt rx2,flt ry2,flt angle,int lat,int lon)
{
    int lati
    for (lati=0;lati<lat;lati++)
       for (loni=0;loni<lon;loni++)
       {
       }
} */          
  
vertex *mkcone(t3dmesh *obj,flt x1, flt y1,flt z1,flt z2,flt rx, flt ry,flt angle,int edgecount)
{
   UINT32 OLDCF=T3Dpolycreationflags;
   tedge *e;vertex *v;
   tpointflt between(0.0,1.0);
   if (z2<z1) T3Dpolycreationflags ^= cfANTICLOCKWISE;
   v=obj->addvertex(x1,y1,z1);
   T3Dpolycreationflags^=cfANTICLOCKWISE|cfFLIPTEXTUREX;
   e=mkcircle(obj,x1,y1,z2,rx,ry,angle,edgecount);   
   T3Dpolycreationflags=OLDCF;
   obj->edgeconnect(e,v,&between,edgecount);
   //mkcircle(x1,y1,z2,rx,ry,angle,edgecount);
   
   return NULL;
}




//////////////////////////////////////////
// tcamera

tcamera::tcamera(vector p_,vector o_,flt fov)
{
    camtype=cam3D;
    p=p_;
    o=o_;
    type=otCAMERA;
    FOV=fov;
    NEARZ=5.0f;
    FARZ=5000.0f;    
}


tcamera::tcamera()
{
    camtype=cam3D;
    type=otCAMERA;
    FOV=500.0f;
    NEARZ=5.0f;
    FARZ=5000.0f;    
}
        
tcamera::~tcamera()
{
}

void tcamera::do3d(matrix4x4 *cmat,matrix4x4 *cmattr,tcamera *cam)
{
    getbounds(cmat,cam);
    T3Dviewport.roverlap(bounds)?attribute|=ofVISIBLE:attribute &=~ofVISIBLE;
}


//draws a graphical representation of the camera
void tcamera::renderhandles(UINT8 attrib,tcamera *cam)
{
    if (cam->camtype!=cam3D)
    {
    t_col=getcolour(255,255,255);
    tpoint p2di(float2int(p2d.x),float2int(p2d.y));    
    hline(p2di.x-5,p2di.x+5,p2di.y);
    vline(p2di.x,p2di.y-5,p2di.y+5);    
    }
}
    
void tcamera::rotate(flt x,flt y,flt z)
{
    o.moveto(x,y,z);    
    LMrottransinv(&localmattr,o.x,o.y,o.z,p.x,p.y,p.z);
    localmat=localmattr;
    calcworldmat();
}
    
void tcamera::scale(flt x,flt y,flt z)
{    
}


void tcamera::moveto(flt x,flt y,flt z)
{
    p.moveto(x,y,z);
    localmattr.p=p;
    
    localmat=localmattr;
    calcworldmat();
}

trectflt *tcamera::getbounds(matrix4x4 *cmat,tcamera* cam)
{
    vector wp=cmat->p;            
    T3Dproject(&wp,&p2d,cam)?attribute |=ofVISIBLE:attribute &=~ofVISIBLE;
    //p2d.x=(wp.x*cmat->m00 +wp.y*cmat->m01 +wp.z*cmat->m02) +cmat->p.x;
    //p2d.y=(wp.x*cmat->m10 +wp.y*cmat->m11 +wp.z*cmat->m12) +cmat->p.y;
    tempradius=1.0f;
    bounds.rassign(p2d.x-6,p2d.y-6,p2d.x+6,p2d.y+6);
    return &bounds;
}


/*Hi, omatase.
I've just got an example;
When the angle that the point a (a1,b1,c1) and b(b1,b2,b3) makes is,
the angle should be solved like this;
(1)
<a,b>=a1*b1+a2*b2+a3*b3
(2)
llall=root(a1+b1+c1)
(3) cos'x' equals
(1) divided by ( |a| * |b| )
*/

/*void tcamera::lookat(vector &lookat,flt twist)
{
    //do some pythagarous shit in here
    //rotation on x axis = degrees (atan((CamY-ObjY)/(CamZ-ObjZ)))   
    //     rotation on y axis = degrees (atan((CamZ-ObjZ)/(CamX-ObjX)))
    //     rotation on z axis = degrees (atan((CamX-ObjX)/(CamY-ObjY)))
    vector a=lookat-p;
    flt r=a.length();
    if (r==0.0f){rotate(0,0,0);return;}
    //* Function: ATAN2 returns the angle in  radian  between the  positive
    //  part of the x-axis and the line with origin in (0,0) which
    //  contains the point (x, y).    
    flt theta = atan2 (a.x, a.y);
    flt fee = acos(a.z/r);
    //atan(a.z/a.x)                
    
} */   

/*
Matrix AimZ (const Point &pt)
{
        flt r, theta, fee, ct, st, cf, sf;

        // vector zero length, collapse the world!
        r = pt.Length ();
        if (IS_ZERO (r))
                return IdentityMatrix;

        // vector already points in the z direction
        if (IS_ZERO (pt[0])&&IS_ZERO (pt[1]))
                return pt[2]>0.0f?IdentityMatrix:MirrorYZ;

        theta = atan2 (pt[1], pt[0]);
        fee = acos (pt[2]/r);
        ct = cos (theta);
        st = sin (theta);
        cf = cos (fee);
        sf = sin (fee);
    
        return Matrix (
                   -st, -ct*cf, ct*sf,
                    ct, -st*cf, st*sf,
                 0.0f,      sf,    cf,
                 0.0f,  0.0f,   0.0f);
}

Matrix AimViewer (
        const Point &position,
        const Point &target,
        flt fov,              // field of view
        flt roll)             // roll around tcamera z axis
{
        flt scale = 1.0/tan (RAD (fov)*0.5);

        return
                Translate (-position)*
                AimZ (target-position)*
                MirrorX*        // turn to left-handed
                RotateZ (RAD (roll))*
                Scale (scale, scale, 1.0);
}
/*
Call AimViewer with the tcamera particulars, and it will return a
matrix which you can then use to transform all points in world space
in to tcamera space.

A few notes,

tcamera space is left handed. Increasing X is to the right, increasing
Y is up and increasing Z is forward.

The matrix is used to post multiply the vectors (i.e. r = v*m).

I'am assuming you can figure out how to write Translate, MirrorX,
RotateZ and Scale functions (they are trivial, but if you need help
let me know and I will supply them).
*/

char *getpossiblelabel(char *txt,FILE *f)
{
    fscanf(f,"%s",txt);
    char ch[2]="\0";
                
    if (txt[0]=='{')   //surface has a label
    {        
        if (!strcontains(txt,'}'))
        while (ch[0]!='}')
        {
            ch[0]=fgetc(f);
            //strcat(txt,ch);          
        }
        return txt;
    }
    else return NULL;               
}


//////////////////////////////////////////
// tlights


tlight::tlight()
{
    type=otLIGHT;
    diffuse=trgbfloat(1.0f,1.0f,1.0f);
    ambient=trgbfloat(0.0f,0.0f,0.0f);
    specular=trgbfloat(1.0f,1.0f,1.0f);
    
    lighttype=ltDIRECTIONAL;
    conesize=1.0;
    intensity=1.0;        
}

    
tlight::~tlight()
{
}

void tlight::do3d(matrix4x4 *cmat,matrix4x4 *cmattr,tcamera*cam)
{
    //rotate temporary tlight rotation
    o.multmatrix(cmat,&no);
    no.normalize();
    getbounds(cmat,cam);
    T3Dviewport.roverlap(bounds)?attribute|=ofVISIBLE:attribute &=~ofVISIBLE;
}



trectflt *tlight::getbounds(matrix4x4 *cmat,tcamera*cam)
{
    wp=cmat->p;    
    T3Dproject(&wp,&p2d,cam)?attribute |=ofVISIBLE:attribute &=~ofVISIBLE;
    tempradius=1.0f;
    bounds.rassign(p2d.x-6,p2d.y-6,p2d.x+6,p2d.y+6);
    return &bounds;
}

//draws a graphical representation of the light
void tlight::renderhandles(UINT8 attrib,tcamera *cam)
{
    t_col=getcolour(float2int(diffuse.r*255.0f),float2int(diffuse.g*255.0f),float2int(diffuse.b*255.0f));
    tpoint p2di(float2int(p2d.x),float2int(p2d.y));    

    hline(p2di.x-5,p2di.x+5,p2di.y);
    vline(p2di.x,p2di.y-5,p2di.y+5);
    line(p2di.x-5,p2di.y-5,p2di.x+5,p2di.y+5);
    line(p2di.x+5,p2di.y-5,p2di.x-5,p2di.y+5);    
}

//////////////////////////////////////////
// bounding box


tgrid3d::tgrid3d():tobject3d()
{
    attribute |= ofGRIDSNAP;
    type=otGRID;
    gridx=gridy=gridz=1.5;
    strcpy(name,"grid3d");
}    


tgrid3d::tgrid3d(float gridspacex,float gridspacey,float gridspacez):tobject3d()
{
    type=otGRID;
    gridx=gridspacex;
    gridy=gridspacey;
    gridz=gridspacez;
}

trectflt *tgrid3d::getbounds(matrix4x4 *cmat,tcamera*cam)
{
    return &T3Dviewport;
    
}

flt closestmultiple(flt f,flt s)
{
    INT32 i_f=float2int(f*10000);
    INT32 i_s=float2int(s*10000);
    i_f %= i_s;   
    return f-i_f / 10000.0f;            
}

void tgrid3d::snapto(vector *v)
{
    if (attribute & ofGRIDSNAP)
    {
      v->x=closestmultiple(v->x,gridx);
      v->y=closestmultiple(v->y,gridy);
      v->z=closestmultiple(v->z,gridz);
    }
}

void tgrid3d::do3d(matrix4x4 *cmat,matrix4x4 *cmattr,tcamera*cam)
{    
    p=cmat->p;
    if (cam->camtype!=cam3D)
    {
      switch (cam->camtype)
      {
        case camORTHXY:
        tempgridx=gridx*T3Dscale;
        tempgridy=gridy*T3Dscale;
        break;
        case camORTHXZ:
        tempgridx=gridx*T3Dscale;
        tempgridy=gridz*T3Dscale;
        break;
        case camORTHZY:
        tempgridx=gridz*T3Dscale;
        tempgridy=gridy*T3Dscale;
        break;
      }
      
    }
}

void tgrid3d::drawgrid()
{
    flt px,py;
    
    t_col=T3DCOLgridmajor;
    hline(float2int(T3Dviewport.a.x),float2int(T3Dviewport.b.x),float2int(p.y));
    vline(float2int(p.x),float2int(T3Dviewport.a.y),float2int(T3Dviewport.b.y));
    if (attribute & ofVISIBLE)
    {    
    t_col=T3DCOLgrid;
         
    if (tempgridx>=1.0)
    {
        px=p.x;
        while (px>=T3Dviewport.a.x)
            vline(float2int(px-=tempgridx),float2int(SC.viewport.a.y),float2int(SC.viewport.b.y));                    
        px=p.x;
        while (px<=T3Dviewport.b.x)      
            vline(float2int(px+=tempgridx),float2int(SC.viewport.a.y),float2int(SC.viewport.b.y));                
    }
    if (tempgridy>=1.0)
    { 
        py=p.y;
        while (py>=SC.viewport.a.y)
            hline(float2int(SC.viewport.a.x),float2int(SC.viewport.b.x),float2int(py-=tempgridy));
        py=p.y;
        while (py<=SC.viewport.b.y)        
            hline(float2int(SC.viewport.a.x),float2int(SC.viewport.b.x),float2int(py+=tempgridy));                    
    }
    }
}

        

//////////////////////////////////////////
// bounding box

/*class tboundingbox3d:public tobject3d
{
    trect bounds;
    virtual void renderhandles(UINT8 attrib);    
    virtual void do3d(matrix4x4 *,tcamera*);  
};*/


tboundingbox3d::tboundingbox3d(UINT32 *count_)
{
    count=count_;
    sprintf(name,"boundingbox");
    type=otBOUNDINGBOX;
    attribute|=ofAFFECTEDBYSELECTION;
}


void tboundingbox3d::do3d(matrix4x4 *cmat,matrix4x4 *cmattr,tcamera *cam)
{
   
   vertex *cv,*end=&v[8];
   getbounds(cmat,cam);
   //if visible
   if (T3Dselectionfunction==stROTATE)
   {
       vector minb,maxb;
       //-boundingboxrange
       //+boundingboxrange
       v[0].data3d=minb=(min)-T3Dbbcentreofrotation;
       v[6].data3d=maxb=(max)-T3Dbbcentreofrotation;
       v[1].data3d.moveto(maxb.x,minb.y,minb.z);
       v[2].data3d.moveto(maxb.x,maxb.y,minb.z);
       v[3].data3d.moveto(minb.x,maxb.y,minb.z);
       v[4].data3d.moveto(minb.x,minb.y,maxb.z);
       v[5].data3d.moveto(maxb.x,minb.y,maxb.z);       
       v[7].data3d.moveto(minb.x,maxb.y,maxb.z);                 
       for (cv=v;cv<end;cv++)
       {
          cv->data3d.multmatrixtrans(cmat,&cv->temp3d);
          minb=cv->temp3d;
          T3Dproject(&minb,&cv->point2d,cam);
       }
   }
}

trectflt *tboundingbox3d::getbounds(matrix4x4 *cmat,tcamera*cam)
{
    tempradius=MAX(fabs(min.length()),fabs(max.length()));
    bounds.a.x=((min.x*cmat->m00 +min.y*cmat->m01 +max.z*cmat->m02) +cmat->p.x)-boundingboxrange;
    bounds.a.y=((min.x*cmat->m10 +min.y*cmat->m11 +max.z*cmat->m12) +cmat->p.y)-boundingboxrange;
    bounds.b.x=((max.x*cmat->m00 +max.y*cmat->m01 +min.z*cmat->m02) +cmat->p.x)+boundingboxrange;
    bounds.b.y=((max.x*cmat->m10 +max.y*cmat->m11 +min.z*cmat->m12) +cmat->p.y)+boundingboxrange;
    return &bounds;
}


void tboundingbox3d::sortelements()
{
  flt temp;
  if ((temp=min.x)>max.x){min.x=max.x;max.x=temp;}
  if ((temp=min.y)>max.y){min.y=max.y;max.y=temp;}
  if ((temp=min.z)>max.z){min.z=max.z;max.z=temp;}
}
         

void tboundingbox3d::renderhandles(UINT8 attrib,tcamera *cam)
{
    t_col=T3DCOLboundingbox;
    trectflt b;
    flt half;
    tpointflt t,b1,b2,b3,b4;
    if (cam->camtype!=cam3D)        
    if ((*count>0)&&((attribute& ofENABLED)!=0))
    {
       switch (T3Dselectionfunction)
       {
         case stSTRETCH:
            b=bounds;       
            if (b.a.x+boundingboxrangex2 >b.b.x) {b.b.x=bounds.a.x+boundingboxrangex2 ;b.a.x=bounds.b.x-boundingboxrangex2;}
            if (b.a.y+boundingboxrangex2 >b.b.y) {b.b.y=bounds.a.y+boundingboxrangex2 ;b.a.y=bounds.b.y-boundingboxrangex2;}
            //bounds.sortelements();    
            rectanglef(b.a.x+bndindent,b.a.y+bndindent,b.b.x-bndindent,b.b.y-bndindent);
            rectanglef(b.a.x,b.a.y,b.a.x+bndctrlsize,b.a.y+bndctrlsize);
            rectanglef(b.b.x-bndctrlsize,b.a.y,b.b.x,b.a.y+bndctrlsize);
            rectanglef(b.a.x,b.b.y-bndctrlsize,b.a.x+bndctrlsize,b.b.y);
            rectanglef(b.b.x-bndctrlsize,b.b.y-bndctrlsize,b.b.x,b.b.y);
            half=b.a.x+b.halfx();
            rectanglef(half-bndindent,b.a.y,half+bndindent,b.a.y+bndctrlsize);
            rectanglef(half-bndindent,b.b.y-bndctrlsize,half+bndindent,b.b.y);
            half=b.a.y+b.halfy();
            rectanglef(b.a.x,half-bndindent,b.a.x+bndctrlsize,half+bndindent);    
            rectanglef(b.b.x-bndctrlsize,half-bndindent,b.b.x,half+bndindent);
            break;
       case stROTATE:
           linef(v[0].point2d.x,v[0].point2d.y,v[1].point2d.x,v[1].point2d.y);
           linef(v[1].point2d.x,v[1].point2d.y,v[2].point2d.x,v[2].point2d.y);
           linef(v[2].point2d.x,v[2].point2d.y,v[3].point2d.x,v[3].point2d.y);
           linef(v[3].point2d.x,v[3].point2d.y,v[0].point2d.x,v[0].point2d.y);

           linef(v[4].point2d.x,v[4].point2d.y,v[5].point2d.x,v[5].point2d.y);
           linef(v[5].point2d.x,v[5].point2d.y,v[6].point2d.x,v[6].point2d.y);
           linef(v[6].point2d.x,v[6].point2d.y,v[7].point2d.x,v[7].point2d.y);
           linef(v[7].point2d.x,v[7].point2d.y,v[4].point2d.x,v[4].point2d.y);

           linef(v[0].point2d.x,v[0].point2d.y,v[4].point2d.x,v[4].point2d.y);
           linef(v[1].point2d.x,v[1].point2d.y,v[5].point2d.x,v[5].point2d.y);
           linef(v[2].point2d.x,v[2].point2d.y,v[6].point2d.x,v[6].point2d.y);
           linef(v[3].point2d.x,v[3].point2d.y,v[7].point2d.x,v[7].point2d.y);                      
           /*t.x=bounds.a.x;t.y=bounds.a.y;
           zrotatepointaround(t,&b1,T3Dboundingboxrotation,&T3D2drotationpivot);

           t.x=bounds.b.x;t.y=bounds.a.y;
           zrotatepointaround(t,&b2,T3Dboundingboxrotation,&T3D2drotationpivot);           
           
           t.x=bounds.b.x;t.y=bounds.b.y;
           zrotatepointaround(t,&b3,T3Dboundingboxrotation,&T3D2drotationpivot);

           t.x=bounds.a.x;t.y=bounds.b.y;
           zrotatepointaround(t,&b4,T3Dboundingboxrotation,&T3D2drotationpivot);

           line(b1.x,b1.y,b2.x,b2.y);
           line(b2.x,b2.y,b3.x,b3.y);
           line(b3.x,b3.y,b4.x,b4.y);
           line(b4.x,b4.y,b1.x,b1.y);                                  
           break;       */
       }
    }
}

//////////////////////////////////////////
// The 3d cursor

tcursor3d::tcursor3d()
{
    strcpy(name,"3dcursor");
    type=otCURSOR;           
}

void tcursor3d::renderhandles(UINT8 attrib,tcamera *cam)
{    
    t_fillcol=T3DCOLcursor;
    tpoint p2di(float2int(p2d.x),float2int(p2d.y));    
    bar(p2di.x-6,p2di.y-1,p2di.x+6,p2di.y+1);
    bar(p2di.x-1,p2di.y-6,p2di.x+1,p2di.y+6);  
}

void tcursor3d::do3d(matrix4x4 *cmat,matrix4x4 *cmattr,tcamera *cam)
{        
    getbounds(cmat,cam);    
    T3Dviewport.roverlap(bounds)?attribute|=ofVISIBLE:attribute &=~ofVISIBLE;
}

trectflt *tcursor3d::getbounds(matrix4x4 *cmat,tcamera*cam)
{
    wp=cmat->p;
    T3Dproject(&wp,&p2d,cam);
    bounds.rassign(p2d.x-6,p2d.y-6,p2d.x+6,p2d.y+6);
    tempradius=6.0f;
    return &bounds;
}

//////////////////////////////////////////
// The Scene

tscene::tscene()
{
    //since this is all OO, the size could be anything so pass 0    
    TTinittree(&scenegraph,0);
    tlights.init(sizeof(tlightref));
    polyattribmask=0xFFFFFFFF;    
}

tscene::~tscene()
{
    TTdestroytree(scenegraph.root);
    tlights.destroy();
}

//process the scene for a full redraw
void tscene::setupview(tcamera *cam,UINT16 offx,UINT16 offy)
{
    matrix4x4 tempmat;    
    
    T3Doffset.x=(flt)((SC.size.x >> 1)+offx);
    T3Doffset.y=(flt)((SC.size.y >> 1)+offy);
    
    SC.viewport.a.x+=offx;
    SC.viewport.a.y+=offy;
    SC.viewport.b.x+=offx;
    SC.viewport.b.y+=offy;
    SET(T3Dviewport,SC.viewport);
    
    if (cam->camtype==cam3D)
    {
        FOV=cam->FOV;
        NEARZ=cam->NEARZ;
        T3Dscreenscale.x=SC.size.x / FOV;
        T3Dscreenscale.y=SC.size.y / FOV;
        T3Dmaxscale = MAX(T3Dscreenscale.x, T3Dscreenscale.y);
        T3Dmaxscreenscaleinv = 1.0f / T3Dmaxscale;
        T3Dscreencentre.x= SC.size.x  / 2.0f - 0.5f;
        T3Dscreencentre.y= SC.size.y  / 2.0f - 0.5f;       
        T3Dsetupfrustumviewp();        
        vector uvec=vector(0,0,-1);
        T3Dviewmat=cam->localmat;
        uvec.multmatrix(&T3Dviewmat,&T3Dlookv);
        
        LMinvert(&T3Dviewmat);
        T3Dcampos=cam->worldmat.p;                        
    }else
    {
        //stored orthographic camera stuff doesn't affect it at the moment.. calculating each frame.. eeks.
        //T3Dviewmat=cam->mat;
        //0.5 sorts out weird inaccuracy shit    
        LMtranslate(&T3Dviewmat,cam->p.x+T3Doffset.x+0.5,cam->p.y+T3Doffset.y+0.5,0);
        LMscale(&tempmat,T3Dscale,T3Dscale,T3Dscale);
        LMconcatenatematrices(&T3Dviewmat,&tempmat,&T3Dviewmat);
        switch (cam->camtype)
        {
            case camORTHXY:LMidentity(&tempmat);break;
            case camORTHZY:LMyrotation(&tempmat,HALFPI);break;
            case camORTHXZ:LMxrotation(&tempmat,-HALFPI);break;
        }        
        LMconcatenatematrices(&T3Dviewmat,&tempmat,&T3Dviewmat);  
    }
}


void tscene::setfog(flt min,flt max)
{
    fogminz=min;
    fogmaxz=max;
    fogdepth=max-min;       
}


void tscene::preprocess(tcamera *cam,UINT16 offx,UINT16 offy)
{
    //static tcamera *oldcam=NULL;
    
    matrix4x4 tempmat;
    tobject3d *root=(tobject3d*)scenegraph.root;
    T3Dresetbuffers();              
    setupview(cam,offx,offy);   
    //oldcam=cam;
    tobject3d *cobj=root;    
    //matrix4x4 startmat;
    //LMunit(&startmat);
    while (cobj!=NULL)
    {        
        cobj->do3dfull(&T3Dviewmat,&T3Dviewmat,cam);
        cobj=(tobject3d*)cobj->next;
    }
    if (cam->camtype==cam3D)
        T3Dsort(0,T3Dpolycount-1);

}


void tscene::preprocess_singleobject(tcamera *cam,tobject3d *obj,UINT16 offx,UINT16 offy)
{
  T3Dresetbuffers();              
  setupview(cam,offx,offy);
  obj->do3dfull(&T3Dviewmat,&T3Dviewmat,cam);
  if (cam->camtype==cam3D)
      T3Dsort(0,T3Dpolycount-1);

}


/*void tscene::preprocess(tcamera *cam,UINT16 offx,UINT16 offy)
{
    static tcamera *oldcam=NULL;
    tobject3d *root;
    matrix4x4 tempmat;

    T3Ds();
    root=(tobject3d*)scenegraph.root;
    T3Doffset=SC.size;            
    T3Doffset.x>>=1;
    T3Doffset.y>>=1;

    
    if (cam->camtype==cam3D)
    {
        FOV=cam->FOV;
        NEARZ=cam->NEARZ;
        xscreenscale=SC.viewport.width() / FOV;
        yscreenscale=SC.viewport.height() / FOV;
        maxscale = MAX(xscreenscale, yscreenscale);
        maxscreenscaleinv = 1.0f / maxscale;
        xcenter = SC.size.x  / 2.0f - 0.5f;
        ycenter = SC.size.y  / 2.0f - 0.5f;
            
        T3Dsetupfrustum();
        vector uvec=vector(0,0,-1);
        T3Dviewmat=cam->localmat;
        LMinvert(&T3Dviewmat);
        uvec.multmatrix(&T3Dviewmat,&T3Dlookv);                
    }else
    {
        //orthographic camera shit doesn't affect it at the moment..
        //T3Dviewmat=cam->mat;
        //0.5 sorts out weird inaccuracy shit        
        LMtranslate(&T3Dviewmat,cam->p.x+T3Doffset.x+0.5,cam->p.y+T3Doffset.y+0.5,0);
        LMscale(&tempmat,T3Dscale,T3Dscale,T3Dscale);
        LMconcatenatematrices(&T3Dviewmat,&tempmat,&T3Dviewmat);
        switch (cam->camtype)
        {
            case camORTHXY:LMidentity(&tempmat);break;
            case camORTHZY:LMyrotation(&tempmat,HALFPI);break;
            case camORTHXZ:LMxrotation(&tempmat,HALFPI);break;
        }        
        LMconcatenatematrices(&T3Dviewmat,&tempmat,&T3Dviewmat);  
    }
    oldcam=cam;
    tobject3d *cobj=root;    
    //matrix4x4 startmat;
    //LMunit(&startmat);
    while (cobj!=NULL)
    {        
        cobj->do3dfull(&T3Dviewmat,cam);
        cobj=(tobject3d*)cobj->next;
    }
    if (cam->camtype==cam3D)
        T3Dsort(0,polycount-1);
}

//process scene for a single object and it's children
void tscene::preprocess_singleobject(tcamera *cam,tobject3d *obj,UINT16 offx,UINT16 offy)
{   
    matrix4x4 tempmat;

    T3Ds();    
    T3Doffset=SC.size;            
    T3Doffset.x>>=1;
    T3Doffset.y>>=1;
    T3Doffset.x+=offx;
    T3Doffset.y+=offy;
    SC.viewport.a.x+=offx;
    SC.viewport.a.y+=offy;
    SC.viewport.b.x+=offx;
    SC.viewport.b.y+=offy;
    
    
   if (cam->camtype==cam3D)
    {
        FOV=cam->FOV;
        NEARZ=cam->NEARZ;
        xscreenscale=SC.viewport.width() / FOV;
        yscreenscale=SC.viewport.height() / FOV;
        maxscale = MAX(xscreenscale, yscreenscale);
        maxscreenscaleinv = 1.0f / maxscale;
        xcenter = SC.size.x  / 2.0f - 0.5f;
        ycenter = SC.size.y  / 2.0f - 0.5f;
        
        T3Dsetupfrustum();
        vector uvec=vector(0,0,-1);
        T3Dviewmat=cam->localmat;
        LMinvert(&T3Dviewmat);
        uvec.multmatrix(&T3Dviewmat,&T3Dlookv);                
    }else
    {
        //stored orthographic camera stuff doesn't affect it at the moment.. calculating each frame.. eeks.
        //T3Dviewmat=cam->mat;
        //0.5 sorts out weird inaccuracy shit    
        LMtranslate(&T3Dviewmat,cam->p.x+T3Doffset.x+0.5,cam->p.y+T3Doffset.y+0.5,0);
        LMscale(&tempmat,T3Dscale,T3Dscale,T3Dscale);
        LMconcatenatematrices(&T3Dviewmat,&tempmat,&T3Dviewmat);
        switch (cam->camtype)
        {
            case camORTHXY:LMidentity(&tempmat);break;
            case camORTHZY:LMyrotation(&tempmat,HALFPI);break;
            case camORTHXZ:LMxrotation(&tempmat,HALFPI);break;
        }        
        LMconcatenatematrices(&T3Dviewmat,&tempmat,&T3Dviewmat);  
    }
  obj->do3dfull(&T3Dviewmat,cam);
  if (cam->camtype==cam3D)
      T3Dsort(0,polycount-1);
}

*/
//process scene for a single object and it's children
trectflt *tscene::getbounds(tcamera *cam,tobject3d *obj,UINT16 offx,UINT16 offy)
{
    matrix4x4 tmpmat;
    //preconcatinate cameraspace matrix to objects world matrix
    setupview(cam,offx,offy);
    LMconcatenatematrices(&T3Dviewmat,&obj->worldmat,&tmpmat);
    
    return obj->getbounds(&tmpmat,cam);    
}



void tscene::renderhandles(UINT8 attrib,tcamera *cam)
{
    t_col=T3DCOLnode;        
    tobject3d *obj=(tobject3d*)scenegraph.root;
    while (obj!=NULL)
    {
        obj->renderhandlesfull(attrib,cam);
        obj=(tobject3d*)obj->next;
    }
}

void tscene::removeobject(tobject3d* object)
{
    switch (object->type)
    {
      case otLIGHT:
      //search for light reference, remove it
      tlightref *lref=(tlightref*)tlights.first;
      while ((lref!=NULL)&&(lref->l!=(tlight*)object))
         lref=(tlightref*)lref->next;
      if (lref)
         tlights.remove(lref);            
      break;
    }
    
    TTremovenode(&scenegraph,(ttreenode*)object);    
    //delete object;    
}

tobject3d *tscene::addobject(tobject3d* parentobject,tobject3d* newobject)
{
    switch (newobject->type)
    {
      case otLIGHT:
      tlightref *lr=new tlightref((tlight*)newobject);
      tlights.push((telemptr)lr);
      break;
    }
    TTaddchildtonode(&scenegraph,(ttreenode*)parentobject,(ttreenode*)newobject);
    //force matrix update
    newobject->moveto(newobject->p.x,newobject->p.y,newobject->p.z);

//    newobject->calcworldmat();
    return newobject;
}

INT16 intypos;
void tscene::intensity(vector &n,vector &p,material &m,trgbfloat &result)
{
    tlightref *lr;
    flt specdot=0.0f;
    tlight *l;
    flt dp;
    result.r=result.g=result.b=0.0f;
    vector o,r,v;
    v=T3Dlookv;    
    v.normalize();
    //v.x=-v.x;
    //v.y=-v.y;
    v.z=-v.z;
    v=vector(0,0,-1);
    lr=(tlightref*)tlights.first;
    T3Dspecdp=0.0f;    
    result=m.emmisive;
    if ((T3Dsceneattributes & sfFOG)!=0)           
        if (p.z>fogmaxz)
        {
            result=fogcolour;
            return;
        }        
    
    while (lr!=NULL)
    {
        l=lr->l;
        if ((l->attribute&ofENABLED)!=0)
        {
            switch (l->lighttype)
            {
                case ltDIRECTIONAL:
                    o=l->no;
           
                    break;
                case ltPOINT:
                    o=l->wp-p;                
                    o.normalize();
                    break;               
            };
            //n.normalize();
            dp=dot(n,o);            
            
            //R'= 2N'(N'.L')-L
            specdot=0.0f;
            if (dp>0.0f) //any light here?
            {            
                if ((T3Dsceneattributes & sfSPECULARHIGHLIGHTS)!=0)
                {
                      //calculate reflection vector
                   /*r.x=(2.0f*n.x*dp)-o.x;
                   r.y=(2.0f*n.y*dp)-o.y;
                   r.z=(2.0f*n.z*dp)-o.z;                                        
                   specdot=dot(r,v);*/                   
                   specdot=-((2.0f*n.z*dp)-o.z);
                   if ((specdot>0.0f))
                   {
                       
                       if (m.specpower==0)
                         specdot*=m.st;
                       else                                            
                         specdot=ipow(specdot,m.specpower)*m.st;
                         
                       T3Dspecdp+=specdot;                     
                   }else
                       specdot=0.0f;
                }                           
                result.r+=l->ambient.r+(l->ambient.r*m.ambient.r)+(l->diffuse.r*m.diffuse.r*(float)dp)+((l->specular.r*m.specular.r)*(float)specdot);
                result.g+=l->ambient.g+(l->ambient.g*m.ambient.g)+(l->diffuse.g*m.diffuse.g*(float)dp)+((l->specular.g*m.specular.g)*(float)specdot);
                result.b+=l->ambient.b+(l->ambient.b*m.ambient.b)+(l->diffuse.b*m.diffuse.b*(float)dp)+((l->specular.b*m.specular.b)*(float)specdot);
            }else
            {
                //if no light, just add ambient values
                result.r+=l->ambient.r+(l->ambient.r*m.ambient.r);
                result.g+=l->ambient.g+(l->ambient.g*m.ambient.g);
                result.b+=l->ambient.b+(l->ambient.b*m.ambient.b);
            }
        }
        lr=(tlightref*)lr->next;
    }
    BOUND(result.r,0.01f,1.0f);
    BOUND(result.g,0.01f,1.0f);
    BOUND(result.b,0.01f,1.0f);   
    //put fogging in here
    if ((T3Dsceneattributes & sfFOG)!=0)
    {        
        if (p.z>fogminz)
        {
            float temp=(float)(p.z-fogminz)/fogdepth;
            float invtemp=1.0f-temp;
            result.r=(result.r*invtemp)+(fogcolour.r*temp);
            result.g=(result.g*invtemp)+(fogcolour.g*temp);
            result.b=(result.b*invtemp)+(fogcolour.b*temp);            
        }
    }        
}

void tscene::renderwire()
{

    tcolour ecol[255];
    tcolour *ncol=ecol;
    UINT32 i;
    *(ncol++)=getcolour(255,0,0);
    *(ncol++)=getcolour(0,255,0);
    *(ncol++)=getcolour(0,0,255);
    *(ncol++)=getcolour(255,255,0);
    *(ncol++)=getcolour(0,255,255);
    *(ncol++)=getcolour(255,0,255);
    *(ncol++)=getcolour(255,0,0);
    *(ncol++)=getcolour(0,255,0);
    *(ncol++)=getcolour(0,0,255);
    *(ncol++)=getcolour(255,255,0);
    *(ncol++)=getcolour(0,255,255);
    *(ncol++)=getcolour(255,0,255);
    

    static int colours[]={T3DCOLwire.c32val,T3DCOLselectedwireedge.c32val,T3DCOLselectedwire.c32val,T3DCOLselectedwireedge.c32val};
    tedge **ce=T3Dvisibleedges;    
    static tpointflt p,np;
    static tedge *e;    
    
    for (i=0;i<T3Dedgecount;i++) 
    {
        e=*ce;
        if (e->attribute & eaSELECTED)
        {
            t_col=ecol[e->edgeloopref];
        }else
        t_col.c32val=colours[e->attribute & 3];
        p=e->v[0]->point2d;
        np=e->v[1]->point2d;

        if (e->attribute & eaSELECTED)
        {
            drawarrow(p.x,p.y,np.x,np.y,0.5,0.4,5);
        }else
            line(float2int(p.x),float2int(p.y),float2int(np.x),float2int(np.y)); 
        
        ce++;
    }
    
}
    
void tscene::render()
{
    static UINT32 i,iv;
    static UINT32 at;
    static INT32 addby;
    static UINT32 nsides;
    static vector tempnormal;
    static t3dmesh *obj=(t3dmesh *)scenegraph.root;
    static tpolyvertex *vr;
    static tpoint p,np;
    static vertex *vp;
    static tpolygon **cpolyref;//(polygon*)obj->polygonlist.first;
    static tpolygon *cpoly;
    static material *mat;
    static float tr;
    static float tempt;            
    static trgbfloat col;
    //for polygons
    static trivert points[256],*cpoint;
  
    cpolyref=T3Dvisiblepolygons;
    for (i=0;i<T3Dpolycount;i++)
    {
        //renderpolygon
        cpoly=*cpolyref;
        
        at=cpoly->attribute & polyattribmask;        
        nsides=cpoly->npoints;                
        //vr=cpoly->vlist;               
        
        cpoint=points;
        settexture(cpoly->tex);
        //t_texture=;

        mat=cpoly->mat;
        if (!mat)
          mat=T3Dmaterial;
        
        //transparency failsafe.
        if (at & paBACKFACING)
        {
           vr=&cpoly->vlist[nsides-1];
           addby=-1;
        }else
        {
           vr=&cpoly->vlist[0];
           addby=1;
        }
        tr=(float)(mat->transparency*255.0);
        
        if (tr<=1.0f)
           at&=~paTRANSPARENT;
                              
        if (!t_texture)
           at &=~paTEXTURED;
                   
        //set up point details
        if (at & paTEXTURETRUE)
        {
            t_colrgba.r=t_colrgba.g=t_colrgba.b=128;
            at&=~paSHADED;
            if (at & paTRANSPARENT)                            
                t_colrgba.a=float2int(tr);                        
        }else
        if (at & paTRUECOLOUR)
        {
            trgbfloat d=mat->diffuse;
            t_colrgba.r=float2int(d.r*255);t_colrgba.g=float2int(d.g*255);t_colrgba.b=float2int(d.b*255);
            t_col=getcolour(t_colrgba.r,t_colrgba.g,t_colrgba.b);
            at&=~paSHADED;
            if (at & paTRANSPARENT)                            
                t_colrgba.a=float2int(tr);            
        }
        else if ((at & paSHADED)==0)
        {
            tempnormal=(at & paBACKFACING)?-cpoly->tempnormal:cpoly->tempnormal;
            intensity(tempnormal,cpoly->centre,*mat,col);
            t_colrgba.r=float2int(col.r*255);t_colrgba.g=float2int(col.g*255);t_colrgba.b=float2int(col.b*255);
            t_col=getcolour(t_colrgba.r,t_colrgba.g,t_colrgba.b);
            if (at & paTRANSPARENT)
            {
                if (at & paNOSPECTRANS)
                    t_colrgba.a=float2int(tr);                
                else
                {
                    tempt=mat->transparency-(T3Dspecdp*1.5f);
                    BOUND(tempt,0.0f,mat->transparency);
                    t_colrgba.a=float2int(tempt*255);
                }
            }                
        }


        
        for (iv=0;iv<nsides;iv++)
        {
            vp=vr->v;                        
            cpoint->x=(float)vp->point2d.x;
            cpoint->y=(float)vp->point2d.y;            
            tempnormal=(at & paBACKFACING)?-vp->tempnormal:vp->tempnormal;
        
            if (at & paSHADED)
            {
                
                intensity(tempnormal,vp->temp3d,*mat,col);                    
                cpoint->r=col.r*COLOURSCALE ;cpoint->g=col.g*COLOURSCALE ;cpoint->b=col.b*COLOURSCALE ;
                if (at & paTRANSPARENT)
                {
                    if (at & paNOSPECTRANS)
                       cpoint->a=tr;             
                    else
                    {
                       tempt=(float)mat->transparency-(T3Dspecdp*1.5f);
                       BOUND(tempt,0.0f,mat->transparency);
                       cpoint->a=tempt*COLOURSCALE;
                    }
                }
                
            }            
            
            //cpoint->u = max_u/2 + normal.x*(max_u/2)
            //cpoint->v = max_v/2 + normal.y*(max_v/2) 
            if (at & paTEXTURED)
            {
                if (at & paENVMAP)
                {
                    cpoint->u = (float)MAX(0.01f,(0.5f + tempnormal.x*0.5f)*(t_texturewidthscale));
                    cpoint->v = (float)MAX(0.01f,(0.5f + tempnormal.y*0.5f)*(t_textureheightscale));
                }else
                {
                    BOUND(vr->texv.x,0.01f,1.000f);
                    BOUND(vr->texv.y,0.01f,1.000f);
                    cpoint->u=vr->texv.x*(t_texturewidthscale);
                    cpoint->v=vr->texv.y*(t_textureheightscale);
                }
            }
            //if zbuffer...            
            cpoint++;
            vr+=addby;            
            //vr++;
        }        
        //set render mode
        svga64k_scanmode(at & 15);
        //and render...
        UINT32 id=0;
        if (nsides>3)
        {
            for (;id<(nsides >> 1)-1;id++)
            {
                tri(&points[id],&points[id+1],&points[nsides-id-1]);
                tri(&points[id+1],&points[nsides-id-2],&points[nsides-id-1]);                            
            }                                                               
        }                                                               
        if ((nsides & 1)==1) 
            tri(&points[id],&points[id+1],&points[id+2]);
                //convexpolygon(points,nsides);                        
        //Draw surface normals
        #ifdef SHOW_NORMALS
        line(cpoly->c2d.x,cpoly->c2d.y,
             cpoly->n2d.x,cpoly->n2d.y);
        #endif
        cpolyref++;
    } 
}



/*
void tscene::render()
{
    UINT32 i,iv;
    UINT32 at;
    INT32 addby;
    UINT32 nsides;
    vector tempnormal;
    t3dmesh *obj=(t3dmesh *)scenegraph.root;
    tpolyvertex *vr;
    vertex *vp;
    tpolygon **cpolyref=T3Dvisiblepolygons;//(polygon*)obj->polygonlist.first;
    tpolygon *cpoly;
    material *mat;
    UINT8 tr;
    flt tempt;
    
        
    trgbfloat col; 
    //for polygons
    static gpoint points[256],*cpoint;    
    for (i=0;i<T3Dpolycount;i++)
    {
        //renderpolygon
        cpoly=*cpolyref;        
        at=paSHADED;
        nsides=cpoly->npoints;        

        cpoint=points;

        mat=cpoly->mat;
        if (at & paBACKFACING)
        {
           vr=&cpoly->vlist[nsides-1];
           addby=-1;
        }else
        {
           vr=&cpoly->vlist[0];
           addby=1;
        }
        
        for (iv=0;iv<nsides;iv++)
        {
            vp=vr->v;                        
            cpoint->x=vp->point2d.x;
            cpoint->y=vp->point2d.y;            
            tempnormal=(at & paBACKFACING)?-vp->tempnormal:vp->tempnormal;                                    
            intensity(tempnormal,vp->temp3d,*mat,col);                    
            cpoint->r=col.r*255;cpoint->g=col.g*255;cpoint->b=col.b*255;
            cpoint->r&=255;cpoint->g&=255;cpoint->b&=255;                                  
            cpoint++;
            vr+=addby;            
        }        
        //set render mode
        svga64k_setpolymode(at & 15);
        //and render...       
        convexpolygon(points,nsides);       
        cpolyref++;
    } 
}*/

char *getpath(char *fullpath,char *justpath)
{
    char dummy[255];
    //char dir[255];
    _splitpath(fullpath,dummy,justpath , dummy, dummy);
        
    return justpath;        
}
UINT32 gettricount(t3dmesh *obj)
{
    UINT32 count=0;
    INT32 vcount;
    tpolygon *p=(tpolygon*)obj->polygonlist.first;
    while (p!=NULL)
    {
        vcount=p->npoints-2;
        if (vcount>0)
          count+=vcount;
        p=p->nxt();
          
    }
    return count;
}


BOOL T3Dsavejava(char *filename,tscene *scene)
{
    //UINT16 minorv=0,majorv=1;
    //UINT32 blockpos,endpos,codeword,blocksize;   
    material *oldmat;
    UINT32 oldattribute;
    //ttexture *oldtex;
    vertex *v;
    tpolygon *p;
    tpolyvertex *pv,*end,*pvr;
    UINT32 counter;
    UINT32 tricount;
        
    FILE *f=fopen(filename,"wa");
    if (!f)
    {        //set ERR_LEVEL
        return NULL;
    }
    t3dmesh *obj=(t3dmesh*)scene->scenegraph.root;
    
    while (obj!=NULL)
    {
        if (obj->type==otGEOM)
        {
        tricount=0;
        counter=0;
        v=(vertex*)obj->vertexlist.first;
        p=(tpolygon*)obj->polygonlist.first;
        fprintf(f,"class %s extends t3dmesh\n",obj->name);
        fprintf(f,"%s()\n{\n",obj->name);
        fprintf(f,"    super(%i,%i);\n",obj->vertexlist.count,gettricount(obj));\
        while (v!=NULL)
        {
             //should and this with a mask;
             fprintf(f,"    addvertex(%ff,%ff,%ff);\n",v->data3d.x,v->data3d.y,v->data3d.z);
             //T3Dwritevector3f(f,&v->data3d);
             v->writetag(counter++);
             v=v->nxt();        
        }
        oldmat=NULL;
        //oldtex=NULL;
        oldattribute=0xFFFFFFFF;
        while (p!=NULL)
        {
            UINT32 npoints=p->npoints;            
            if (p->attribute !=oldattribute)
            {
              fprintf(f,"    t3dmesh.cattribute=0");
              if (p->attribute & paDOUBLESIDED)
                fprintf(f,"|ttriangle.TAdoublesided");
              if (p->attribute & paSHADED)
                fprintf(f,"|ttriangle.TAsmooth");
              fprintf(f,";\n");
            }
            if (p->mat!=oldmat)
            {
                if (p->mat->diffuse!=oldmat->diffuse)                
                   fprintf(f,"    t3dmesh.cred=%ff;t3dmesh.cgreen=%ff;t3dmesh.cblue=%ff;\n",p->mat->diffuse.r,p->mat->diffuse.g,p->mat->diffuse.b);
                if (p->mat->emmisive!=oldmat->emmisive)                
                   fprintf(f,"    t3dmesh.cered=%ff;t3dmesh.cegreen=%ff;t3dmesh.ceblue=%ff;\n",p->mat->emmisive.r,p->mat->emmisive.g,p->mat->emmisive.b);
            }

            /*id=0;
            if (count>3)
            {
               for (;id<(count >> 1)-1;id++)
               {                                               
                  trianglebuffer[polycount++]=
                  addtemptriangle(pclipv[id],pclipv[id+1],pclipv[count-id-1],ct);
                  trianglebuffer[polycount++]=
                  addtemptriangle(pclipv[id+1],pclipv[count-id-2],pclipv[count-id-1],ct);                                                                 
               }                                                               
            }                                                               
            if ((count & 1)==1) 
               trianglebuffer[polycount++]=addtemptriangle(pclipv[id],pclipv[id+1],pclipv[id+2],ct); */
            pv=p->vlist;
            end=&pv[(npoints >>1)-1];
            pvr=&pv[npoints-1];
            if (npoints>3)
               for (;pv<end;pv++,pvr--)
               {                   
                  fprintf(f,"    addquad(%i,%i,%i,%i);\n",pv->v->readtag(),(pv+1)->v->readtag(),(pvr-1)->v->readtag(),pvr->v->readtag());
                  tricount+=2;
               }
            if (npoints & 1)
            {
               fprintf(f,"    addtriangle(%i,%i,%i);\n",pv->v->readtag(),(pv+1)->v->readtag(),pvr->v->readtag());
               tricount++;
            }
            oldmat=p->mat;
            oldattribute=p->attribute;
            p=p->nxt();
        }
        
        fprintf(f,"    }//tricount=%i\n",tricount);
        fprintf(f,"}\n");
        }
        obj=(t3dmesh*)obj->next;
    }
    fclose(f);
    return TRUE;
}


/*B_flags: 

bit 0 : tiled flag 

     0 is not tiled 

     1 is tiled

bit 1 : animated flag 

     0 = static texture 

     1 = animated texture
*/
#define MRTEXTURE_TILED 0x1
#define MRTEXTURE_ANIM 0x2
#define MRTEXTURE_TRUECOL 0x1

#define MRPOLY_SHADED 0x1
#define MRPOLY_TINTED 0x2
#define MRPOLY_WIREFRAME 0x4
#define MRPOLY_DOUBLESIDED 0x8
#define MRPOLY_RESERVED 0x10
#define MRPOLY_TEXTUREBACKGROUND 0x20 //transparent or seethrough
#define MRPOLY_TEXTURETYPE 0x40 //transparent or seethrough
#define MRPOLY_INVISIBLE 0x80 //visible or not
#define MRPOLY_NOTSELECTABLE 0x100 
#define MRPOLY_TINTTYPE 0x200 //hash or not (redundant)
#define MRPOLY_TINTPHASE 0x400 //tintphase(redundant)
#define MRPOLY_ENVMAP 0x800  //env mapping
#define MRPOLY_NOPHONG 0x1000 //Phong
#define MRPOLY_TRUECOLOUR 0x2000 //true colour

t3dmesh *T3Dloadmr(char *filename)
{
    
    
    
    UINT8 *textureflagsa;
    UINT8 ctexflaga;        
    textureptr *texturetable;
    char T3Dtempstr[256];
    char buf[256];    
    char pathname[256];        
    int polyrefs[256];
    tpointflt tvecs[64];
    
    float tfloat;
    float tvecx,tvecy,tvecz;
    tpolygon *tpoly;
    trgb tcol;
    trgbfloat tcolfloat;    
    int maxbsp,maxtextures,dummy,flags,maxvecs,maxpolys,maxsurfs,maxpolypoints,i,i2,i3,hastexture;    
    UINT32 polyflags;
    UINT32 oldpolyflags=T3Ddefaultpolyattributes;
    FILE *f;
    
    //logmessage( (void*)&textcol,"inside loadmr");
    f=fopen(filename,"rb");
    
    if (!f)
    {        //set ERR_LEVEL
        return NULL;
    }            
    fscanf(f,"%s",T3Dtempstr);
    if (!strcmp(T3Dtempstr,"#MINDRENDER"))
    {    
       fscanf(f,"%f",&tfloat);
       fscanf(f,"%i",&maxbsp);
    }else
    {
       sscanf(T3Dtempstr,"%i",&maxbsp);
    }          
    if (maxbsp<0) return NULL;    
    for (i=0;i<maxbsp;i++)
    {
        fscanf(f,"%s",T3Dtempstr);                
        fscanf(f,"%s",T3Dtempstr);                               
    }    
    fscanf(f,"%i",&maxtextures);
    if (maxtextures<0) return NULL;
    
    //logmessage( (void*)&textcol,"getting textures");

    if (maxtextures>0)
    {        
        texturetable=new textureptr[maxtextures];
        textureflagsa=(UINT8*)malloc(maxtextures);
        for (i=0;i<maxtextures;i++)
        {
            fscanf(f,"%s",T3Dtempstr);
            tbitmap *tex=NULL;  
            sprintf(pathname,"%s%s.bmp",TEXTURE_PATH,T3Dtempstr);      
            if (!fexist(pathname))
            {
               
               if (!fexist(pathname))               
                   sprintf(pathname,"%s%s.bmp",EXE_PATH,T3Dtempstr);                                 
            }
            
            if (fexist(pathname))
            {
                tex=(tbitmap*)readbmp(pathname,BMflat16);
                T3Daddtexture((texture*)tex,T3Dtempstr);            
            }
                       
            texturetable[i]=(texture*)tex;
         
         
            //load texture
            fscanf(f,"%i",&textureflagsa[i]);
            
            fscanf(f,"%i",&dummy);
            fscanf(f,"%i",&flags);
            
            
            fscanf(f,"%i",&dummy);fscanf(f,"%i",&dummy);
            
            if (flags & MRTEXTURE_ANIM)
            {
              fscanf(f,"%i",&dummy);fscanf(f,"%i",&dummy);
              fscanf(f,"%i",&dummy);fscanf(f,"%i",&dummy);
            }                          
              
        }        
    }    
    //sprintf(buf,"maxtextures: %i",T3Dtexturebank.count);
    //logmessage( (void*)&textcol,buf);
    
    
    //5.0 volts 1 amp...what kind of random comment is this??
    
    fscanf(f,"%i",&dummy);
    fscanf(f,"%i",&dummy);
    fscanf(f,"%i",&dummy);
    fscanf(f,"%i",&dummy);
    fscanf(f,"%i",&maxvecs);

    //logmessage( (void*)&textcol,"allocating object");
    
    t3dmesh *newobj=new t3dmesh();
    if (maxvecs<0) return NULL;
    if (maxvecs>0)
    for (i=0;i<maxvecs;i++)
    {        
        fscanf(f,"%s",&T3Dtempstr);
        if (T3Dtempstr[0]=='i')  //previous vertex has intensity value
        {            
           fscanf(f,"%f",&tfloat);
           fscanf(f,"%f",&tvecx);
        }           
        else
           sscanf(T3Dtempstr,"%f",&tvecx);                     
        
        fscanf(f,"%f",&tvecy);
        fscanf(f,"%f",&tvecz);
        newobj->addvertex(tvecx,-tvecy,tvecz);        
    }
    fscanf(f,"%i",&maxsurfs);
    if (maxsurfs<0) return NULL;  
    sprintf(buf,"No. surfaces: %i",maxsurfs);
    logmessage( (void*)&textcol,buf);
    
    
    
    for (i2=0;i2<maxsurfs;i2++)
    {
        //check for label (if any) here        
        if (getpossiblelabel(T3Dtempstr,f))
        {
            logmessage(  (void*)&textcol,T3Dtempstr);
            
            fscanf(f,"%i",&dummy);
        }else
            sscanf(T3Dtempstr,"%i",&dummy); //node atachment        
        fscanf(f,"%i",&dummy); //phong
        fscanf(f,"%i",&dummy); //render type
        fscanf(f,"%i",&maxpolys);
        if (maxpolys<0) return NULL;  
        sprintf(buf,"No. polygons for surface %i:%i",i2,maxpolys);
        logmessage( (void*)&textcol,buf);
        
        for (i=0;i<maxpolys;i++)
        {                        
            T3Ddefaultpolyattributes=paENABLED;            
            if (getpossiblelabel(T3Dtempstr,f))
            {
                logmessage( (void*)&textcol,T3Dtempstr);                
                     
                fscanf(f,"%i",&hastexture);                                
            }else
                sscanf(T3Dtempstr,"%i",&hastexture);
                                             
            if (hastexture>0)
            {
                if (maxtextures)
                {
                   T3Dsettexture(texturetable[hastexture-1]);
                   ctexflaga=textureflagsa[hastexture-1];                                      
                }
                fscanf(f,"%i",&dummy);  //don't know what this first param is yet
            }else
            {
                T3Dsettexture(NULL);
                ctexflaga=0;
            }
            fscanf(f,"%u",&polyflags);  //polygon flags

            if(polyflags&0x400) T3Ddefaultpolyattributes|=paENVMAP;
            if(polyflags&MRPOLY_TINTED) T3Ddefaultpolyattributes|=paTRANSPARENT;
            if(polyflags&MRPOLY_SHADED) T3Ddefaultpolyattributes|=paSHADED;
            if(polyflags&MRPOLY_TRUECOLOUR) T3Ddefaultpolyattributes|=paTRUECOLOUR;
            if(polyflags&MRPOLY_DOUBLESIDED) T3Ddefaultpolyattributes|=paDOUBLESIDED;
            
            if(ctexflaga&MRTEXTURE_TRUECOL) T3Ddefaultpolyattributes|=paTEXTURETRUE;
            
                        
            fscanf(f,"%i %i %i ",&tcol.r,&tcol.g,&tcol.b);  //colour
            tcolfloat.r=tcol.r/255.0f;
            tcolfloat.g=tcol.g/255.0f;
            tcolfloat.b=tcol.b/255.0f;            
            T3Dmaterial=T3Daddmaterial(tcolfloat,8,((T3Ddefaultpolyattributes&paTRANSPARENT)!=0)?0.5f:0.0f);
            T3Dmaterial->st=1.0;
            
            fscanf(f,"%i",&maxpolypoints);     //max polygon points
            if (maxpolypoints<0) return NULL;  
            tpoly=newobj->addpoly(maxpolypoints);            
            for (i3=0;i3<maxpolypoints;i3++)  //read in vertex references            
            {
                fscanf(f,"%i",&polyrefs[i3]);                  
            }
            if (hastexture>0)                 //read in texture coords
                for (i3=0;i3<maxpolypoints;i3++)
                {
                    fscanf(f,"%f %f",&tvecs[i3].x,&tvecs[i3].y);
                    tvecs[i3].x=(tvecs[i3].x/T3Dtexture->size.x);
                    tvecs[i3].y=(tvecs[i3].y/T3Dtexture->size.y);
                    BOUND(tvecs[i3].x,0.0f,1.0f);
                    BOUND(tvecs[i3].y,0.0f,1.0f);
                }

            for (i3=0;i3<maxpolypoints;i3++)
            {
               T3Dsetpolygonvertex(tpoly,newobj->getvertex(polyrefs[i3]),tvecs[i3].x,tvecs[i3].y,maxpolypoints-i3-1,0);               
            }
                            

        }        
    }                
    if (maxtextures>0)
    {
       free(texturetable);
       free(textureflagsa);
    }
    //logmessage( (void*)&textcol, (void*)&textcol,"done");
    fclose(f);    
    T3Ddefaultpolyattributes=oldpolyflags;
    return newobj;    
}

extern UINT32 intel_mot(UINT32 val);
#pragma aux intel_mot=\
"bswap eax"\
parm [eax]\
value [eax]

extern void intel_motflt(float *val);
#pragma aux intel_motflt=\
"mov eax,[edi]"\
"bswap eax"\
"mov [edi],eax"\
parm [edi]

extern UINT16 intel_mot16(UINT16 val);
#pragma aux intel_mot16=\
"ror ax,8"\
parm [ax]\
value [ax]



typedef _Packed struct pasreal
{
   UINT8 d[6];
}pasreal;
/*
typedef _Packed struct pasreal
{
    UINT16 t[3];
}pasreal;
*/

typedef _Packed struct claypoly
{
  char numsides;
  UINT16 p[4];
  char colour;
  char attrib;
}claypoly;

//[4]

typedef _Packed struct clayjoin
{
    UINT16 f,t;
}clayjoin;
/*
extern void fldz();
#pragma aux fldz=\
"fldz"

extern void fld(long double *f);
#pragma aux fld=\
"fld [edi]"\
parm[edi]


//#pragma warn -rvl

double real2double(UINT16 r[3])
{
   union {
     UINT16 w[5];
     long double e;
   } ld;
  //fldz();
   if ( (r[0]&0xff)==0 )
     return 0.0f;
   
     ld.w[4] = (r[0]&0x00ff)+(r[2]&0x8000)+0x3f7e;
     ld.w[3] = r[2]|0x8000;
     ld.w[2] = r[1];
     ld.w[1] = r[0]&0xff00;
     ld.w[0] = 0;
     fld(&ld.e);   
   return ld.e;
}

//<iretd> if 0 < e <= 255, then v = (-1)^s * 2^(e-129) * (1.f)
double real2double(pasreal &r)
{
    //UINT32 frac=intel_mot(r.frac);
    //power(-1,r.sign & 0x1)*power(2,r.exp-129)*
    if (r.sign &0x1)
       return pow(2,r.exp-129)*(1.0f+(flt)r.frac/0xFFFFFFFF);
    return -(pow(2,r.exp-129)*(1.0f+(flt)r.frac/0xFFFFFFFF));
    
}


/*

Below is some code cut out of the TD log, if your C compiler supports in-line
code, it might be useful...

Input of the round and trunc functions is a 6-byte real in dx:bx:ax.
(s-mant-exp), the return a longint in dx:ax.

FYI a Borland 6 byte real looks like this, and this info is in the manual..

5  4  3  2  1  0    {byte order, exponet is lowest byte!}
 1     39      8    {width in bits}
+-+------------+---+
|s|    f       | e | (f has an implied msb of 1)
+-+------------+---+
   msb   lsb m   l

The value n of the number is determined by

 - if e = 0, then n = 0
 - if 0 < e <= 255, then n = (-1)^s * 2^(e - 129) * (1.f)
*/
flt real2float(pasreal r)
{
   union {
     UINT32 clearme;
     UINT8 m[4];
     float f;
   } ld;
   if (r.d[0]==0) return 0.0f;
   ld.clearme=0;
   r.d[0]-=2;
   ld.m[1] = r.d[4];
   ld.m[0] = r.d[3];
   ld.m[2] = r.d[5]&0x7F;
   if ((r.d[0] & 1)!=0)
      ld.m[2] |= 0x80;   
   
   ld.m[3] = (r.d[0]&0xFE) >> 1;
   ld.m[3] |= (r.d[5]&0x80);
   return ld.f;
}


void printfloat(float f)
{
    union {
     UINT8 m[4];
     float f;
    } ld;
    
    ld.f=f;
    sprintf(T3Dtempstr,"the bytes in val:%f are %u %u %u %u",ld.f,ld.m[0],ld.m[1],ld.m[2],ld.m[3]);
    //outconsole(T3Dtempstr);
}

/*char T3Dsignature[4]={'C','W','3','D'};*/
//UINT32 T3Dsignature=0x43573344;

UINT32 T3Dsignature=0x44335743;


//just used for file
#define otSCENE 0x80


void T3Dwritefloat(FILE *f,flt fltval)
{
    static float fval;
    fval=(float)fltval;
    fwrite(&fval,4,1,f);
}

inline UINT32 T3DwriteUINT8(FILE *f,UINT8 *n)
{
    fwrite(n,1,1,f);
    return 1;    
}

inline UINT32 T3DwriteUINT32(FILE *f,UINT32 *n)
{
    fwrite(n,4,1,f);
    return 4;
}

inline UINT32 T3DwriteUINT16(FILE *f,UINT16 *n)
{
    fwrite(n,4,1,f);
    return 4;
}


UINT32 T3Dwritecolour3f(FILE *f,trgbfloat *col)
{
    T3Dwritefloat(f,col->r);
    T3Dwritefloat(f,col->g);
    T3Dwritefloat(f,col->b);
    return 12;
}

inline UINT32 T3Dwritematerialbank(FILE *f)
{
    material *cmat;
    UINT32 temp=otMATERIALBANK;
    UINT32 materialcount=T3Dmaterialbank.count;
    T3DwriteUINT32(f,&temp);
    temp=8+(materialcount * 57);
    T3DwriteUINT32(f,&temp);
    T3DwriteUINT32(f,&materialcount);
    cmat=(material*)T3Dmaterialbank.first;
    while (cmat)
    {
        T3Dwritecolour3f(f,&cmat->ambient);
        T3Dwritecolour3f(f,&cmat->diffuse);
        T3Dwritecolour3f(f,&cmat->specular);
        T3Dwritecolour3f(f,&cmat->emmisive);
        T3DwriteUINT8(f,&cmat->specpower);
        T3Dwritefloat(f,cmat->st);
        T3Dwritefloat(f,cmat->transparency);                        
        cmat=(material*)cmat->next;
    }
    return TRUE;
}



UINT32 T3Dwritevector3f(FILE *f,vector *v)
{
    T3Dwritefloat(f,v->x);
    T3Dwritefloat(f,v->y);
    T3Dwritefloat(f,v->z);
    return 12;
}

#define T3Dcolour3fsize (12)
#define T3Dbasic3dobjectsize (32+16)
#define T3Dlightsize (T3Dbasic3dobjectsize+1+(T3Dcolour3fsize*3)+4+4)

/*
    trgbfloat ambient,diffuse,specular,emmisive;
    UINT8 specpower;
    flt st; //shine strength.. replacing the '2.0f' of r'=(2.0f*n'*dp)-o' maybe incorrect
    flt transparency;
*/

UINT32 T3Dwritebasic3dobject(FILE *f,tobject3d *obj)
{
    fwrite(obj->name,32,1,f);
    T3DwriteUINT32(f,&obj->attribute);
    T3Dwritevector3f(f,&obj->p);
    T3Dwritevector3f(f,&obj->o);
    T3Dwritevector3f(f,&obj->s);
    return TRUE;
}


inline UINT32 T3Dwritemesh(FILE *f,t3dmesh *mesh)
{        
    T3Dwritebasic3dobject(f,(tobject3d *)mesh);
    UINT32 vcount=mesh->vertexlist.count;
    UINT32 pcount=mesh->polygonlist.count;
        
    vertex *v=(vertex *)mesh->vertexlist.first;
    tpolygon *p=(tpolygon *)mesh->polygonlist.first;
    
    UINT32 counter=0;

    T3DwriteUINT32(f,&vcount);
    while (v!=NULL)
    {
        //should and this with a mask;
        T3DwriteUINT32(f,&v->attribute);
        T3Dwritevector3f(f,&v->data3d);
        v->writetag(counter++);
        v=v->nxt();        
    }
    counter=0;
    fwrite(&pcount,4,1,f);
    while (p!=NULL)
    {
        UINT32 npoints=p->npoints;
        UINT32 vref;

        T3DwriteUINT32(f,&p->attribute);
        T3DwriteUINT32(f,&npoints);
        counter=T3Dgettextureindex(p->tex);
        tpolyvertex *pv=p->vlist;
        tpolyvertex *end=&pv[npoints];
        T3DwriteUINT32(f,&counter);               
        counter=T3Dgetmaterialindex(p->mat);
        for (;pv<end;pv++)
        {
            T3DwriteUINT32(f,&pv->attribute);
            //write material index; this will be pervertex lighting soon so might as well plan ahead
            T3DwriteUINT32(f,&counter);
            T3Dwritefloat(f,pv->texv.x);
            T3Dwritefloat(f,pv->texv.y);
            vref=pv->v->readtag();
            T3DwriteUINT32(f,&vref);                                                                               
        }                
        p=p->nxt();
    }
    return TRUE;    
}



inline UINT32 T3Dwritelight(FILE *f,tlight *light)
{  
    T3Dwritebasic3dobject(f,(tobject3d *)light);
    T3DwriteUINT8(f,&light->lighttype);
    T3Dwritecolour3f(f,&light->diffuse);
    T3Dwritecolour3f(f,&light->ambient);
    T3Dwritecolour3f(f,&light->specular);
    T3Dwritefloat(f,light->conesize);
    T3Dwritefloat(f,light->intensity);
    return TRUE;
}

BOOL T3Dsavenewclay(char *filename,tscene *scene)
{
    UINT32 codeword;
    UINT32 blocksize;
    UINT16 minorv=0;
    UINT16 majorv=1;
    UINT32 blockpos,endpos;
        
    FILE *f=fopen(filename,"wb");
    if (!f)
    {        //set ERR_LEVEL
        return NULL;
    }
    tobject3d *obj=(tobject3d*)scene->scenegraph.root;
    fwrite(&T3Dsignature,4,1,f);
    fwrite(&majorv,2,1,f);
    fwrite(&minorv,2,1,f);
    //start of sceneprefs block
    codeword=otSCENE;
    fwrite(&codeword,4,1,f);
    blocksize=12+T3Dcolour3fsize;
    T3DwriteUINT32(f,&blocksize);
    T3DwriteUINT32(f,&T3Dsceneattributes);    
    T3Dwritefloat(f,scene->fogminz);
    T3Dwritefloat(f,scene->fogmaxz);   
    T3Dwritecolour3f(f,&scene->fogcolour);
    //end of scene block
    T3Dwritematerialbank(f);
    while (obj!=NULL)
    {        
        codeword=obj->type;
        if ((codeword & (otGEOM+otLIGHT))!=0)
        {
          T3DwriteUINT32(f,&codeword);
          //remember here.... 
          blockpos=ftell(f);
          T3DwriteUINT32(f,&blocksize);
          
          switch (codeword)
          {
              case otGEOM:T3Dwritemesh(f,(t3dmesh *)obj);break;
              case otLIGHT:T3Dwritelight(f,(tlight *)obj);break;       
          }
          endpos=ftell(f);          
          blocksize=endpos-blockpos-4;

          fseek(f,blockpos,SEEK_SET);
          T3DwriteUINT32(f,&blocksize);        
          fseek(f,endpos,SEEK_SET);
        }                    
        obj=(tobject3d*)obj->next;
    };
    codeword=otEOF;
    T3DwriteUINT32(f,&codeword);
    fclose(f);
    return TRUE;
}



void T3Dreadfloat(FILE *f,float *fltval)
{
    static float fval;
    fread(&fval,4,1,f);
    *fltval=fval;    
}


void T3Dreadfloat(FILE *f,double *fltval)
{
    static float fval;
    fread(&fval,4,1,f);
    *fltval=fval;    
}

inline UINT32 T3DreadUINT8(FILE *f,UINT8 *n)
{
    fread(n,1,1,f);
    return 1;    
}

inline UINT32 T3DreadUINT32(FILE *f,UINT32 *n)
{
    fread(n,4,1,f);
    return 4;
}

UINT32 T3Dreadcolour3f(FILE *f,trgbfloat *col)
{
    float val[3];
    fread(val,12,1,f);
    col->r=val[0];
    col->g=val[1];
    col->b=val[2];
    return 12;
}

UINT32 T3Dreadvector3f(FILE *f,vector *v)
{
    float val[3];
    fread(val,12,1,f);
    v->x=val[0];
    v->y=val[1];
    v->z=val[2];
    return 12;
}

UINT32 T3Dreadbasicobject(FILE *f,tobject3d *obj)
{    
    fread(obj->name,32,1,f);
    fread(&obj->attribute,4,1,f);
    
    T3Dreadvector3f(f,&obj->p);
    T3Dreadvector3f(f,&obj->o);
    T3Dreadvector3f(f,&obj->s);
    return TRUE;
}

void logcolour(trgbfloat *col)
{
    char buf[100];
    sprintf(buf,"Red: %f Green: %f Blue %f",col->r,col->g,col->b);
    logmessage( (void*)&textcol,buf);
}    

tlight *T3Dreadlight(FILE *f)
{
    tlight *light=new tlight();
    T3Dreadbasicobject(f,(tobject3d*)light);
    T3DreadUINT8(f,&light->lighttype);
    T3Dreadcolour3f(f,&light->diffuse);
    T3Dreadcolour3f(f,&light->ambient);
    T3Dreadcolour3f(f,&light->specular);
    T3Dreadfloat(f,&light->conesize);
    T3Dreadfloat(f,&light->intensity);
    logcolour(&light->diffuse);
    logcolour(&light->ambient);
    logcolour(&light->specular);
    
    
    
    return light;
}




material **objectmatbank;

BOOL T3Dreadmaterialbank(FILE *f)
{
    char buf[100];
    materialentry cmat;
    
    UINT32 materialcount;

    fread(&materialcount,4,1,f);
    material **cmatref=objectmatbank=(material**)malloc(materialcount<<2);
    
    
    sprintf(buf,"material count:%i",materialcount);
    logmessage( (void*)&textcol,buf);
    
    while (materialcount>0)
    {
       fread(&cmat,sizeof(cmat),1,f);
       //sprintf(buf,"material data: %f %f %f %f %f %f %f %f %f %f %f %f %i %f %f",cmat.ar,cmat.ag,cmat.ab,cmat.dr,cmat.dg,cmat.db,cmat.sr,cmat.sg,cmat.sb,cmat.er,cmat.eg,cmat.eb,cmat.sp,cmat.st,cmat.t);
       //logmessage( (void*)&textcol, (void*)&textcol,buf);                                                        
       //add material
       
       *cmatref=T3Daddmaterial(&cmat);
       materialcount--;
       //sprintf(buf,"material count:%i",materialcount);
       //logmessage( (void*)&textcol, (void*)&textcol,buf);
       cmatref++;
       
    }
    return TRUE;
}

t3dmesh *T3Dreadmesh(FILE *f)
{
    char buf[20];
    vector v;
    UINT32 pcount,vcount,attribute,npoints,texref,matref;
    t3dmesh *mesh=new t3dmesh();
    T3Dreadbasicobject(f,(tobject3d*)mesh);
    T3DreadUINT32(f,&vcount);
    tpolygon *cpoly;
    tpointflt texv;
        
    UINT32 counter=0;
   
    T3DwriteUINT32(f,&vcount);
    sprintf(buf,"vertex count: %i",vcount);
    logmessage( (void*)&textcol,buf);
    
    while (vcount>0)
    {
        //should and this with a mask;
        T3DreadUINT32(f,&attribute);
        T3Dreadvector3f(f,&v);        
        (mesh->addvertex(v.x,v.y,v.z))->attribute=attribute;
        
        vcount--;
    }
    counter=0;
    fread(&pcount,4,1,f);

    sprintf(buf,"polygon count: %i",pcount);
    logmessage( (void*)&textcol,buf);
    
    while (pcount>0)
    {        
        T3DreadUINT32(f,&T3Ddefaultpolyattributes);
        T3DreadUINT32(f,&npoints);        
        T3DreadUINT32(f,&texref);
        cpoly=mesh->addpoly(npoints,NULL);
        cpoly->tex=NULL;
        for (UINT32 i=0;i<npoints;i++)
        {
            T3DreadUINT32(f,&attribute);
            T3DreadUINT32(f,&matref);
            cpoly->mat=objectmatbank[matref];
            T3Dreadfloat(f,&texv.x);
            T3Dreadfloat(f,&texv.y);            
            T3DreadUINT32(f,&vcount);            
            T3Dsetpolygonvertex(cpoly,mesh->getvertex(vcount),texv.x,texv.y,i,0);            
        }                
        pcount--;
    }
    logmessage( (void*)&textcol,"done");
    return mesh;    
}



BOOL T3Dloadnewclay(char *filename,tscene *scene)
{
    char buf[100];
    tlight *l;
    t3dmesh *m;
    tobject3d *retobj=NULL;
    UINT32 codeword;
    UINT32 blocksize;
    UINT32 sig;
    UINT8 majorv,minorv;
    FILE *f;
    BOOL ret=TRUE;
    f=fopen(filename,"rb");
    objectmatbank=NULL;
    if (!f) return NULL;
    UINT32 olddefaultpolyattributes=T3Ddefaultpolyattributes;
    fread(&sig,4,1,f);

    itoa((UINT32)sig,buf,16);
    logmessage( (void*)&textcol,buf);
    if (sig!=T3Dsignature)
    {        
        logmessage( (void*)&textcol,"Signature was not correct, trying old format");
        ret=FALSE;
        goto exitgracefully;
    }

    
    
    fread(&majorv,2,1,f);
    fread(&minorv,2,1,f);
    sprintf(buf,"Version: %i.%i",majorv,minorv);
    logmessage( (void*)&textcol,buf);    
    fread(&codeword,4,1,f);
    if (codeword==otSCENE)
      logmessage( (void*)&textcol,"Found Scene tag");
    else
    {
      logmessage( (void*)&textcol,"Scene tag not found");
      ret=FALSE;
      goto exitgracefully;
    }
    fread(&blocksize,4,1,f);
    fseek(f,blocksize,SEEK_CUR);
    
    fread(&codeword,4,1,f);
    if (codeword==otMATERIALBANK)
      logmessage( (void*)&textcol,"Found Material bank");
    else
    {
      logmessage( (void*)&textcol,"Material bank not found");
      ret=FALSE;
      goto exitgracefully;
    }
    fread(&blocksize,4,1,f);
    T3Dreadmaterialbank(f);
    //fseek(f,blocksize,SEEK_CUR);

    while (codeword!=otEOF)
    {
       fread(&codeword,4,1,f);
       fread(&blocksize,4,1,f);
       if (codeword==otLIGHT)
       {
          logmessage( (void*)&textcol,"found light");
          
          l=T3Dreadlight(f);
          if (l)
            scene->addobject(NULL,l);
       }else if (codeword==otGEOM)
       {
          //fseek(f,blocksize,SEEK_CUR);
          logmessage( (void*)&textcol,"found mesh");
          
          
          m=T3Dreadmesh(f);
       
          if (m)
          {
            m->calculatenormals();        
            scene->addobject(NULL,m);
          }
       }else
       {
          logmessage( (void*)&textcol,"Tag not recognized");
          sprintf(buf,"Tag code: %i",codeword);
          logmessage( (void*)&textcol,buf);
          ret=FALSE;
          goto exitgracefully;
       }
       
       
              
    }
exitgracefully:
    if (objectmatbank)
      free(objectmatbank);
    fclose(f);
    T3Ddefaultpolyattributes=olddefaultpolyattributes;
    return ret;    
    
}

#define CLAYvisible 128
#define CLAYabsolute 32
#define CLAYconnected 16
#define CLAYoutlined 8
#define CLAYsmooth 2
#define CLAYdoublesided 1


trgbbase pal16[]={{0,0,0},
{24,24,32},{50,50,70},{70,70,82},{118,119,123},
{130,130,133},{150,150,155},{180,180,180},{23,24,80},
{89,96,140},{120,230,255},{0,140,0},{255,240,0},
{240,190,8},{130,0,0},{240,240,220}};

t3dmesh *T3Dloadclay(char *filename)
{

    logmessage( (void*)&textcol,"attempting to load clayworks file");
    
    tpointflt texvec;
    UINT16 maxvecs,maxlines,maxpolys,i;
    UINT8 vi;    
    pasreal pvec;//pasreal pvec;
    UINT32 oldpolyflags=T3Ddefaultpolyattributes;


    tpolygon *tpoly;
    claypoly ppoly;
    vector ivec;
    FILE *f;
    f=fopen(filename,"rb");
    if (!f)
    {        //set ERR_LEVEL
        return NULL;
    }
    t3dmesh *obj=new t3dmesh();            
    fread(&maxvecs,2,1,f);
    fread(&maxlines,2,1,f);
    fread(&maxpolys,2,1,f);


    if (fgetsize(filename)!=6+(maxvecs*sizeof(pasreal)*3)+(maxlines*sizeof(clayjoin))+(maxpolys*sizeof(claypoly)))
       return NULL;
 
    for (i=0;i<maxvecs;i++)
    {
        fread(&pvec,6,1,f);
        ivec.x=real2float(pvec);
        fread(&pvec,6,1,f);
        ivec.y=real2float(pvec);
        fread(&pvec,6,1,f);
        ivec.z=real2float(pvec);

        obj->addvertex(ivec.x,ivec.y,ivec.z);
    }
  
    fseek(f,4*maxlines,SEEK_CUR);
    
    for (i=0;i<maxpolys;i++)
    {
        fread(&ppoly,1,sizeof(claypoly),f);
        trgbfloat col;
        col.r=(float)pal16[ppoly.colour].r/255.0f;
        col.g=(float)pal16[ppoly.colour].g/255.0f;
        col.b=(float)pal16[ppoly.colour].b/255.0f;
            
        T3Dmaterial=T3Daddmaterial(col,2,0.0f);
        T3Ddefaultpolyattributes=paENABLED;
        if((ppoly.attrib&CLAYsmooth)!=0) T3Ddefaultpolyattributes|=paSHADED;
        if((ppoly.attrib&CLAYabsolute)!=0) T3Ddefaultpolyattributes|=paTRUECOLOUR;
        if((ppoly.attrib&CLAYdoublesided)!=0) T3Ddefaultpolyattributes|=paDOUBLESIDED;
        tpoly=obj->addpoly(ppoly.numsides);
        
        for (vi=0;vi<ppoly.numsides;vi++)
        {          
            if (ppoly.p[vi]>obj->vertexlist.count) return NULL;
            T3Dsetpolygonvertex(tpoly,obj->getvertex(ppoly.p[vi]),texvec.x,texvec.y,vi,0);               
        }
    }
    fclose(f);
    T3Ddefaultpolyattributes=oldpolyflags;
    return obj;
}


#define IFFNOTYPE 0
#define IFFFORM 1297239878
#define IFFLWOB 1112495948
#define IFFPNTS 1398034000
#define IFFSRFS 1397117523
#define IFFPOLS 1397509968
#define IFFSURF 1179899815

#define IFFCOLR 1380732739
#define IFFFLAG 1195461702
#define IFFLUMI 1229804876
#define IFFDIFF 1179011396
#define IFFSPEC 1128616019
#define IFFREFL 1279673682
#define IFFTRAN 1312903764
#define IFFGLOS 1397705799
#define IFFRIMG 1196247378
#define IFFRSAN 1312904018
#define IFFRIND 1145981266
#define IFFEDDE 1162101829
#define IFFSMAN 1312902483

/*
char IFF_LUMI[4]={'L','U','M','I'};
char IFF_DIFF[4]={'D','I','F','F'};
char IFF_SPEC[4]={'S','P','E','C'};
char IFF_REFL[4]={'R','E','F','L'};
char IFF_TRAN[4]={'T','R','A','N'};
char IFF_GLOS[4]={'G','L','O','S'};
char IFF_RIMG[4]={'R','I','M','G'};
char IFF_RSAN[4]={'R','S','A','N'};
char IFF_RIND[4]={'R','I','N','D'};
char IFF_EDGE[4]={'E','D','D','E'};
char IFF_SMAN[4]={'S','M','A','N'};    
        
//    CTEX, DTEX, STEX, RTEX, TTEX, and BTEX
//      TIMG TFLG TSIZ, TCTR, TFAL, and TVEL TCLR TVAL TFRQ TSP0, TSP1, and TSP2

/*
extern int      fgetpos( FILE *__fp, fpos_t *__pos );
extern int      fseek( FILE *__fp, long int __T3Doffset, int __whence );
extern int      fsetpos( FILE *__fp, const fpos_t *__pos );
*/


/*WARNING, HAS MANY BUGS, THIS IS NOT STABLE!!!!!!!!!*/

t3dmesh *T3Dloadlw(char *filename)
{
    fpos_t oldpos;
    UINT32 chunk;
    float cfloatx,cfloaty,cfloatz;
    tpointflt texvec;
    UINT32 chunksize;
    INT16 cint16,numppoints;
    INT32 numpoints,i;//numpolys,numsurfs;
    t3dmesh *obj=NULL;
    tpolygon *tpoly;
    UINT32 oldpolyflags=T3Ddefaultpolyattributes;
    
    FILE *f=fopen(filename,"rb");
    if (!f)
    {        //set ERR_LEVEL
        return NULL;
    }
    obj=new t3dmesh();         
    while (!feof(f))
    {
        fread(&chunk,4,1,f);
                

        
        fread(&chunksize,4,1,f);
        chunksize=intel_mot(chunksize);                
        switch (chunk)
        {
           case IFFFORM:
              fread(&chunk,4,1,f);                     
              
              if (chunk!=IFFLWOB)
              {
                 return NULL;
              }
              
              break;
           case IFFSRFS:
               fseek(f,chunksize,SEEK_CUR);
               break;

           case IFFPOLS:           
               //fseek(f,chunksize,SEEK_CUR);
               while (chunksize>0)
               {
                   T3Ddefaultpolyattributes=paENABLED;                                                          
                   fread(&numppoints,2,1,f);
                   
                   numppoints=intel_mot16(numppoints);
                   if (numpoints>256)
                   {
                       delete obj;
                       goto exitgracefully;
                   }
                   tpoly=obj->addpoly(numppoints);    
                   
                   chunksize-=2;
                   for (i=0;i<numppoints;i++)
                   {
                        fread(&cint16,2,1,f);
                        cint16=intel_mot16(cint16);
                        if (cint16>obj->vertexlist.count)
                        {
                            delete obj;
                            goto exitgracefully;
                        }
                        T3Dsetpolygonvertex(tpoly,obj->getvertex(cint16),texvec.x,texvec.y,numpoints-i-1,0);
                        
                        chunksize-=2;
                   }

                   fread(&cint16,2,1,f);
                   cint16=intel_mot16(cint16);                   
                   chunksize-=2;
                   tpoly->mat=&WHITEMATERIAL;
                   if (cint16<0)
                   {                       
                       fread(&cint16,2,1,f);
                       cint16=intel_mot16(cint16);
                       chunksize-=2;
                       cint16=abs(cint16);
                   }
               }
                                                 
               fclose(f);
               return obj;               
           case IFFPNTS:
               if (obj==NULL) return NULL;               
               
               numpoints=chunksize/12;
              
               
               
               fgetpos( f, &oldpos);
               
               for (i=0;i<numpoints;i++)
               {
                   fread(&cfloatx,4,1,f);
                   intel_motflt(&cfloatx);
                   fread(&cfloaty,4,1,f);
                   intel_motflt(&cfloaty);
                   fread(&cfloatz,4,1,f);
                   intel_motflt(&cfloatz);                   
                   obj->addvertex(cfloatx*100.0f,cfloaty*100.0f,cfloatz*100.0f);
               }
               fsetpos( f,&oldpos);
               fseek(f,chunksize,SEEK_SET);
               break;
               default:
               //delete obj;
               //return NULL;
               fseek(f,chunksize,SEEK_SET);
               break;
        }
        
        
    }
exitgracefully:
    T3Ddefaultpolyattributes=oldpolyflags;
    fclose(f);
    return obj;
   
}    


t3dmesh *T3Dload3ds(char *filename)
{


    logmessage( (void*)&textcol,"attempting to load 3ds file");
    
    tpointflt texvec;
    UINT16 maxvecs,maxlines,maxpolys,i;
    UINT32 vi;    
    pasreal pvec;//pasreal pvec;
    UINT32 oldpolyflags=T3Ddefaultpolyattributes;


    tpolygon *tpoly;
    claypoly ppoly;
    vector ivec;
    FILE *f;
    f=fopen(filename,"rb");
    if (!f)
    {        //set ERR_LEVEL
        return NULL;
    }
    t3dmesh *obj=new t3dmesh();            
    fread(&maxvecs,2,1,f);
    fread(&maxlines,2,1,f);
    fread(&maxpolys,2,1,f);


    if (fgetsize(filename)!=6+(maxvecs*sizeof(pasreal)*3)+(maxlines*sizeof(clayjoin))+(maxpolys*sizeof(claypoly)))
       return NULL;
 
    for (i=0;i<maxvecs;i++)
    {
        fread(&pvec,6,1,f);
        ivec.x=real2float(pvec);
        fread(&pvec,6,1,f);
        ivec.y=real2float(pvec);
        fread(&pvec,6,1,f);
        ivec.z=real2float(pvec);

        obj->addvertex(ivec.x,ivec.y,ivec.z);
    }
  
    fseek(f,4*maxlines,SEEK_CUR);
    
    for (i=0;i<maxpolys;i++)
    {
        fread(&ppoly,1,sizeof(claypoly),f);
        trgbfloat col;
        col.r=(float)pal16[ppoly.colour].r/255.0f;
        col.g=(float)pal16[ppoly.colour].g/255.0f;
        col.b=(float)pal16[ppoly.colour].b/255.0f;
            
        T3Dmaterial=T3Daddmaterial(col,2,0.0f);
        T3Ddefaultpolyattributes=paENABLED;
        if((ppoly.attrib&CLAYsmooth)!=0) T3Ddefaultpolyattributes|=paSHADED;
        if((ppoly.attrib&CLAYabsolute)!=0) T3Ddefaultpolyattributes|=paTRUECOLOUR;
        if((ppoly.attrib&CLAYdoublesided)!=0) T3Ddefaultpolyattributes|=paDOUBLESIDED;
        tpoly=obj->addpoly(ppoly.numsides);                
        for (vi=0;vi<ppoly.numsides;vi++)
        {          
            if (ppoly.p[vi]>obj->vertexlist.count) return NULL;
            T3Dsetpolygonvertex(tpoly,obj->getvertex(ppoly.p[vi]),texvec.x,texvec.y,vi,0);               
        }
    }
    fclose(f);
    T3Ddefaultpolyattributes=oldpolyflags;
    return obj;
}

/*
 * poly_tri.c
 *
 * Program to take a polygon definition and convert it into triangles
 * that may then be rendered by the standard triangle rendering
 * algorithms.  This assumes all transformations have been performed

 * already and cuts them up into optimal triangles based on their
 * screen-space representation.
 *
 *      Copyright (c) 1988, Evans & Sutherland Computer Corporation
 *
 * Permission to use all or part of this program without fee is
 * granted provided that it is not used or distributed for direct
 * commercial gain, the above copyright notice appears, and
 * notice is given that use is by permission of Evans & Sutherland
 * Computer Corporation.
 *
 *      Written by Reid Judd and Scott R. Nelson at
 *      Evans & Sutherland Computer Corporation (January, 1988)
 *
 * To use this program, either write your own "draw_triangle" routine
 * that can draw triangles from the definitions below, or modify the
 * code to call your own triangle or polygon rendering code.  Call
 * "draw_poly" from your main program.
 */
/*

// A single vertex 

typedef struct {
        int color;              // RGB 
        float x;
        float y;
        float z;
} vertex;


// A triangle made up of three vertices 

typedef vertex triangle[3];


#define V_MAX 100       // Maximum number of vertices allowed (arbitrary) 

#define BIG 1.0e30      // A number bigger than we expect to find here 

#define COUNTER_CLOCKWISE 0
#define CLOCKWISE 1



//
// orientation
//
// Return either clockwise or counter_clockwise for the orientation
// of the polygon.
 

int orientation( n, v )
    int n;                      // Number of vertices 
    vertex v[];                 // The vertex list 
{
    float area;
    int i;

    // Do the wrap-around first 
    area = v[n-1].x * v[0].y - v[0].x * v[n-1].y;

    // Compute the area (times 2) of the polygon 
    for (i = 0; i < n-1; i++)
        area += v[i].x * v[i+1].y - v[i+1].x * v[i].y;

    if (area >= 0.0)
        return COUNTER_CLOCKWISE;
    else
        return CLOCKWISE;
} // End of orientation 



//
// * determinant
// *
// * Computes the determinant of the three points.
// * Returns whether the triangle is clockwise or counter-clockwise.
 

int determinant( p1, p2, p3, v )
    int p1, p2, p3;             // The vertices to consider 
    vertex v[];                 // The vertex list 
{
    float x1, x2, x3, y1, y2, y3;
    float determ;

    x1 = v[p1].x;
    y1 = v[p1].y;
    x2 = v[p2].x;
    y2 = v[p2].y;
    x3 = v[p3].x;
    y3 = v[p3].y;

    determ = (x2 - x1) * (y3 - y1) - (x3 - x1) * (y2 - y1);
    if (determ >= 0.0)
        return COUNTER_CLOCKWISE;
    else
        return CLOCKWISE;
} // End of determinant 



//
//  distance2
// 
//  Returns the square of the distance between the two points
 

float distance2( x1, y1, x2, y2 )
    float x1, y1, x2, y2;
{
    float xd, yd;               // The distances in X and Y 
    float dist2;                // The square of the actual distance 

    xd = x1 - x2;
    yd = y1 - y2;
    dist2 = xd * xd + yd * yd;

    return dist2;
} // End of distance2 




// no_interior
//
/  Returns 1 if no other point in the vertex list is inside
// the triangle specified by the three points.  Returns
// 0 otherwise.
 

int no_interior( p1, p2, p3, v, vp, n, poly_or )
    int p1, p2, p3;             // The vertices to consider 
    vertex v[];                 // The vertex list 
    int vp[];                   // The vertex pointers (which are left) 
    int n;                      // Number of vertices 
    int poly_or;                // Polygon orientation 
{
    int i;                      // Iterative counter 
    int p;                      // The test point 

    for (i = 0; i < n; i++) {
        p = vp[i];              // The point to test 
        if ((p == p1) || (p == p2) || (p == p3))
            continue;           // Don't bother checking against yourself 
        if (   (determinant( p2, p1, p, v ) == poly_or)
            || (determinant( p1, p3, p, v ) == poly_or)
            || (determinant( p3, p2, p, v ) == poly_or) ) {
            continue;           // This point is outside 
        } else {
            return 0;           // The point is inside 
        }
    }
    return 1;                   // No points inside this triangle 
} // End of no_interior 



//
// * draw_poly
// *
// * Call this procedure with a polygon, this divides it into triangles
// * and calls the triangle routine once for each triangle.
// *
// * Note that this does not work for polygons with holes or self
// * penetrations.
//

draw_poly( n, v )
    int n;                      // Number of vertices in triangle 
    vertex v[];                 // The vertex list (implicit closure) 
{
    int prev, cur, next;        // Three points currently being considered 
    int vp[V_MAX];              // Pointers to vertices still left 
    int count;                  // How many vertices left 
    int min_vert;               // Vertex with minimum distance 
    int i;                      // Iterative counter 
    float dist;                 // Distance across this one 
    float min_dist;             // Minimum distance so far 
    int poly_orientation;       // Polygon orientation 
    triangle t;                 // Triangle structure 

    if (n > V_MAX) {
        fprintf( stderr, "Error, more than %d vertices.\n", V_MAX);
        return;
    }

    poly_orientation = orientation( n, v );

    for (i = 0; i < n; i++)
        vp[i] = i;              // Put vertices in order to begin 

// Slice off clean triangles until nothing remains 

    count = n;
    while (count > 3) {
        min_dist = BIG;         // A real big number 
        min_vert = 0;           // Just in case we don't find one... 
        for (cur = 0; cur < count; cur++) {
            prev = cur - 1;
            next = cur + 1;
            if (cur == 0)       // Wrap around on the ends 
                prev = count - 1;
            else if (cur == count)
                next = 0;
            // Pick out shortest distance that forms a good triangle 
            if (   (determinant( vp[prev], vp[cur], vp[next], v ) == poly_orientation)
                    // Same orientation as polygon 
                && no_interior( vp[prev], vp[cur], vp[next], v, vp, count, poly_orientation )
                    // No points inside 
                && ((dist = distance2( v[vp[prev]].x, v[vp[prev]].y,
                                       v[vp[next]].x, v[vp[next]].y )) < min_dist) )
                    // Better than any so far 
            {
                min_dist = dist;
                min_vert = cur;
            }
        } // End of for each vertex (cur) 

        // The following error should "never happen". 
        if (min_dist == BIG)
            fprintf( stderr, "Error: Didn't find a triangle.\n" );

        prev = min_vert - 1;
        next = min_vert + 1;
        if (min_vert == 0)      // Wrap around on the ends 
            prev = count - 1;
        else if (min_vert == count)
            next = 0;

// Output this triangle 

        t[0].x = v[vp[prev]].x;
        t[0].y = v[vp[prev]].y;
        t[0].z = v[vp[prev]].z;
        t[0].color = v[vp[prev]].color;
        t[1].x = v[vp[min_vert]].x;
        t[1].y = v[vp[min_vert]].y;
        t[1].z = v[vp[min_vert]].z;
        t[1].color = v[vp[min_vert]].color;
        t[2].x = v[vp[next]].x;
        t[2].y = v[vp[next]].y;
        t[2].z = v[vp[next]].z;
        t[2].color = v[vp[next]].color;

        draw_triangle( t );

// Remove the triangle from the polygon 

        count -= 1;
        for (i = min_vert; i < count; i++)
            vp[i] = vp[i+1];
    }

// Output the final triangle 

        t[0].x = v[vp[0]].x;
        t[0].y = v[vp[0]].y;
        t[0].z = v[vp[0]].z;
        t[0].color = v[vp[0]].color;
        t[1].x = v[vp[1]].x;
        t[1].y = v[vp[1]].y;
        t[1].z = v[vp[1]].z;
        t[1].color = v[vp[1]].color;
        t[2].x = v[vp[2]].x;
        t[2].y = v[vp[2]].y;
        t[2].z = v[vp[2]].z;
        t[2].color = v[vp[2]].color;

        draw_triangle( t );

} // End of draw_poly 

// End of poly_tri.c */
