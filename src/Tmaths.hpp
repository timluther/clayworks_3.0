#ifndef _tmaths_h_
#define _tmaths_h_

#include <math.h>
#include "ttypes.hpp"
#define PI 3.14159265359
#define HALFPI 1.570796326795
#define TWOPI 6.28318530718
#define FIXEDFLOATMAGIC 4503601774854144.000000
//((65536.0*65536.0*16+65536.0*0.5)
//xyz vector thang; used to define rotations and translations

#define doublefloatpresicion
#if defined doublefloatpresicion

inline int float2int(double f)
{
  f+=FIXEDFLOATMAGIC;
  return (*(int*)&f)-0x80000000;
}

inline int float2fixed(double f)
{
  f=(FIXEDFLOATMAGIC)+(f*65536.0);
  return (*(int*)&f)-0x80000000;
}
#else

inline int float2int(float f)
{
  return (int)f;
}

inline int float2fixed(float f)
{
  return (int)(f*65536.0f);
}

#endif


/*
char *vstrcat(char *first, ...)
                {
                        size_t len;
                        char *retbuf;
                        va_list argp;
                        char *p;

                        if(first == NULL)
                                return NULL;

                        len = strlen(first);

                        va_start(argp, first);

                        while((p = va_arg(argp, char *)) != NULL)
                                len += strlen(p);

                        va_end(argp);

                        retbuf = malloc(len + 1);       // +1 for trailing \0 

                        if(retbuf == NULL)
                                return NULL;            // error 

                        (void)strcpy(retbuf, first);

                        va_start(argp, first);          // restart; 2nd scan 

                        while((p = va_arg(argp, char *)) != NULL)
                                (void)strcat(retbuf, p);

                        va_end(argp);

                        return retbuf;
                }
*/
//close enough

//if(fabs(a - b) <= epsilon * fabs(a))

class matrix4x4;


class fixedp16x16
{
    INT32 data;
    fixedp16x16(UINT32 val){data=val << 16;}
//    fixedp16x16(fixedp16x16 val){data=val.data;}
    fixedp16x16(float val){data=(int)val;}
    fixedp16x16(double val){data=(int)val;}
    
    
    fixedp16x16 operator =(UINT32 other){data=other<<16;return *this;}
    //fixedp16x16 operator +(UINT32 other){return fixeddp16x16(data+other<<16);}
    //fixedp16x16 operator -(UINT32 other){data-other<<16;}
    //fixedp16x16 operator +(fixedp16x16 other){data+other.data;return *this;}
    //fixedp16x16 operator -(fixedp16x16 other){data-other.data;return *this;}
    //fixedp16x16 operator *(fixedp16x16 other){return data+other.data;}
    //fixedp16x16 operator /(fixedp16x16 other){return data-other.data;}    
};    

//UINT32 operator=(fixedp16x16 f)   {return f.data >> 16;}

class vector
{
    public:
    flt x,y,z;
    
    vector(flt x_,flt y_,flt z_){x=x_;y=y_;z=z_;}
    vector(flt v){x=y=z=v;}
    vector(){x=y=z=0;}
    void moveto(flt x_,flt y_,flt z_){x=x_;y=y_;z=z_;}
    
    vector &operator +=(vector &other){x+=other.x;y+=other.y;z+=other.z;return *this;}
    vector &operator -=(vector &other){x-=other.x;y-=other.y;z-=other.z;return *this;}
    vector &operator =(flt s){x=y=z=s;return *this;}
    vector &operator -=(flt s){x-=s;y-=s;z-=s;return *this;}
    vector &operator +=(flt s){x+=s;y+=s;z+=s;return *this;}
    vector &operator /=(flt s){s=1.0/s;x*=s;y*=s;z*=s;return *this;}
    vector &operator *=(flt s){x*=s;y*=s;z*=s;return *this;}    
    flt  operator^(vector &v2) {return (flt)x*v2.x + y*v2.y + z*v2.z;}


    bool operator ==(vector &other){return (x==other.x && y==other.y && z==other.z);}
    bool operator !=(vector &other){return (x!=other.x || y!=other.y || z!=other.z);}
    bool operator ==(flt val){return (x==val && y==val && z==val);}
    bool operator !=(flt val){return (x!=val && y!=val && z!=val);}
    
    vector &operator +(vector &other){static vector r;r.x=x+other.x;r.y=y+other.y;r.z=z+other.z; return r;}
    vector &operator -(vector &other){static vector r;r.x=x-other.x;r.y=y-other.y;r.z=z-other.z; return r;}
    vector &operator -(flt other){static vector r;r.x=x-other;r.y=y-other;r.z=z-other; return r;}
    vector &operator +(flt other){static vector r;r.x=x+other;r.y=y+other;r.z=z+other; return r;}
    vector &operator -(){static vector r;r.x=-x;r.y=-y;r.z=-z; return r;}
    vector &operator /(flt s){static vector r;s=1.0/s;r.x=x*s;r.y=y*s;r.z=z*s;return r;}
    vector &operator *(flt s){static vector r;r.x=x*s;r.y=y*s;r.z=z*s;return r;}
    
    //vector treated as magnitude vector from 0,0,0 to x,y,z
    flt magnitude(){return (x*x) + (y*y) + (z*z);}
    flt length(){return sqrt(magnitude());}
    
    //turn into unit length vector
    void normalize()
    {
        flt uli=sqrt((x*x) + (y*y) + (z*z));
        if (uli!=0)
        {
            uli=1.0/uli;     
            x*=uli;y*=uli;z*=uli;
        }            
    }
    vector *multmatrix(matrix4x4 *mat,vector *into);
    vector *multmatrixinv(matrix4x4 *mat,vector *into);
    vector *multmatrixtrans(matrix4x4 *mat,vector *into);
    vector *multmatrixtransinv(matrix4x4 *mat,vector *into);
};

class axisrotation:public vector
{
    public:    
    flt t;
};

typedef matrix4x4 *matrix4x4ptr;

enum matrixConcatType {POSTCONCAT,PRECONCAT,REPLACE};

class matrix4x4
{
    public:
    flt m00,m01,m02,
        m10,m11,m12,
        m20,m21,m22;
    vector p;

    matrix4x4(){m01=m02=m10=m12=m20=m21=p.x=p.y=p.z=0;m00=m11=m22=1;}                                             
};


vector planelineintersection(vector *n,float d,vector *p1,vector *p2);
BOOL concurrent(vector a,vector b);

vector *LMmatrixtransformvec(matrix4x4ptr mat,vector * vec,vector * result);

//matrix m2 is preconcatinated to matrix m1.. counterintuative :(
matrix4x4ptr LMconcatenatematrices(register matrix4x4ptr m1,register matrix4x4ptr m2,matrix4x4ptr result);
void LMtranspose(matrix4x4ptr mat);
void LMinvert(matrix4x4ptr mat);
matrix4x4ptr LMrottrans(matrix4x4ptr mat,flt rx,flt ry,flt rz,flt xt,flt yt,flt zt);
matrix4x4ptr LMtransrot(matrix4x4ptr mat,flt rx,flt ry,flt rz,flt xt,flt yt,flt zt);
matrix4x4ptr LMrottransinv(matrix4x4ptr mat,flt rx,flt ry,flt rz,flt xt,flt yt,flt zt);
matrix4x4ptr LMidentity(matrix4x4ptr mat);
matrix4x4ptr LMxrotation(register matrix4x4ptr mat,flt rotation);
matrix4x4ptr LMyrotation(register matrix4x4ptr mat,flt rotation);
matrix4x4ptr LMzrotation(register matrix4x4ptr mat,flt rotation);
matrix4x4ptr LMrst(register matrix4x4ptr m,flt xrotation,flt yrotation,flt zrotation,flt scx,flt scy,flt scz,flt tx,flt ty,flt tz);
matrix4x4ptr LMaddscale(register matrix4x4ptr m,register matrix4x4ptr mr,flt scx,flt scy,flt scz);
matrix4x4ptr LMxyzrotation(register matrix4x4ptr mat,flt xrotation,flt yrotation,flt zrotation);
matrix4x4ptr LMzyxrotation(register matrix4x4ptr mat,flt xrotation,flt yrotation,flt zrotation);
matrix4x4ptr LMscale(register matrix4x4ptr mat,flt xscale,flt yscale,flt zscale);
matrix4x4ptr LMtranslate(register matrix4x4ptr mat,flt xtrans,flt ytrans,flt ztrans);
matrix4x4ptr LMaxisrotation(register matrix4x4ptr mat,flt x,flt y,flt z,flt rotation);
matrix4x4ptr LMstretch(register matrix4x4ptr mat,vector *oldmin,vector *oldmax,vector *newmin,vector *newmax);
matrix4x4ptr LMrotatearound(matrix4x4ptr mat,flt rx,flt ry,flt rz,vector *vec);

void simpleyrot(float r,vector *v);
void simplexrot(float r,vector *v);
flt safepow(flt val,flt pow);
flt genpower (flt number, flt exponent);
flt ipow(flt val,int power);
flt dot(vector &vec1,vector &vec2);
vector cross(vector &vec1,vector &vec2);
flt distance(vector &vec1,vector &vec2);
void calcnormal(vector p1,vector &p2,vector p3,vector & pn);
//inlines for fixed point


#endif
