
#include <math.h>

#include "stdafx.h"
#include "ttypes.hpp"
#include "tmaths.hpp"

/*        
Here's another way of keeping error out of your rotation matrices:
don't store rotations as a matrix; instead, convert to a matrix only when
you have to, and store rotations as 'Euler paramaters'.
        Euler paramaters take up 1/4 of the room of a rotation matrix (4 floats
instead of 16), and are a lot easier to normalize.
        So, here's the code.  I know that it works; I've used it to spin a cube
at 30 frames/second for over a week, with no deformation of the cube.  The
only thing missing is the vector routines, which are boring anyway, and
are left as an exercise for the reader.  Note that this code is far from
optimized; in my case, optimization wasn't necessary, since most of the
program's time was spent either interacting with the user or drawing the
objects to be displayed.
        If you really want me to, I could dig up the reference to Euler
paramaters.  And, if you are really lazy, I could even be convinced to
send you the source to the vector routines.
*/
/*
class eular
{
    
};
/*
// * Given an axis and angle, compute euler paramaters
// * a is the axis (3 floats), phi is angle in radians, and
// * e is where to put the resulting Euler paramaters (4 floats)
 
void
axis_to_euler(vector *a, float phi, euler *e)
{
        vnormal(a);     // Normalize axis 
        vcopy(a, e);
        vscale(e, fsin(phi/2.0));
        e[3] = fcos(phi/2.0);
}

// *      Given two rotations, e1 and e2, expressed as Euler paramaters,
// * figure out the equivalent single rotation and stuff it into dest.
// * 
// * This routine also normalizes the result every COUNT times it is
// * called, to keep error from creeping in.
//
#define COUNT 100
void
add_eulers(float *e1, float *e2, float *dest)
{
        static int count=0;
        register int i;
        float t1[3], t2[3], t3[3];
        float tf[4];

        vcopy(e1, t1); vscale(t1, e2[3]);
        vcopy(e2, t2); vscale(t2, e1[3]);
        vcross(e2, e1, t3);     // t3 = e2 cross e1 
        vadd(t1, t2, tf);       // tf = t1 + t2, if this was C++ 
        vadd(t3, tf, tf);       // tf += t3 
        tf[3] = e1[3] * e2[3] - vdot(e1, e2);

        // I use tf[] so that e1 or e2 can be the same as dest 
        for (i=0; i < 4; i++) dest[i] = tf[i] ;
        if (++count > COUNT)
        {
                count = 0;
                normalize_euler(dest);
        }
}
//
// * Euler paramaters always obey:  a^2 + b^2 + c^2 + d^2 = 1.0
//*  We'll normalize based on this formula.  Also, normalize greatest
// * component, to avoid problems that occur when the component we're
// * normalizing gets close to zero (and the other components may add up
// * to more than 1.0 because of rounding error).
//
void
normalize_euler(float *e)
{       // Normalize result 
        int which, i;
        float gr;

        which = 0;
        gr = e[which];
        for (i = 1 ; i < 4 ; i++)
        {
                if (fabs(e[i]) > fabs(gr))
                {
                        gr = e[i];
                        which = i;
                }
        }
        e[which] = 0.0; // So it doesn't affect next operation 

        e[which] = fsqrt(1.0 - (e[0]*e[0] + e[1]*e[1] +
                e[2]*e[2] + e[3]*e[3]));

        //Check to see if we need negative square root 
        if (gr < 0.0)
                e[which] = -e[which];
}

//
// Build a rotation matrix, given Euler paramaters.
//
 
void
build_rotmatrix(Matrix m, float *e)
{
        m[0][0] = 1 - 2.0 * (e[1] * e[1] + e[2] * e[2]);
        m[0][1] = 2.0 * (e[0] * e[1] - e[2] * e[3]);
        m[0][2] = 2.0 * (e[2] * e[0] + e[1] * e[3]);
        m[0][3] = 0.0;

        m[1][0] = 2.0 * (e[0] * e[1] + e[2] * e[3]);
        m[1][1] = 1 - 2.0 * (e[2] * e[2] + e[0] * e[0]);
        m[1][2] = 2.0 * (e[1] * e[2] - e[0] * e[3]);
        m[1][3] = 0.0;

        m[2][0] = 2.0 * (e[2] * e[0] - e[1] * e[3]);
        m[2][1] = 2.0 * (e[1] * e[2] + e[0] * e[3]);
        m[2][2] = 1 - 2.0 * (e[1] * e[1] + e[0] * e[0]);
        m[2][3] = 0.0;

        m[3][0] = 0.0;
        m[3][1] = 0.0;
        m[3][2] = 0.0;
        m[3][3] = 1.0;
}*/

/*Vector operator+(Vector v1,Vector v2) {return Vector(v1.x+v2.x,v1.y+v2.y,v1.z+v2.z);}
Vector operator-(Vector v1,Vector v2) {return Vector(v1.x-v2.x,v1.y-v2.y,v1.z-v2.z);}
Vector operator-(Vector v)            {return Vector(-v.x,-v.y,-v.z);}
Vector operator*(Vector v1,float s)   {return Vector(v1.x*s,v1.y*s,v1.z*s);}
Vector operator*(float s, Vector v1)  {return Vector(v1.x*s,v1.y*s,v1.z*s);}
Vector operator/(Vector v1,float s)   {return v1*(1.0f/s);}
float  operator^(Vector v1,Vector v2) {return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;}
Vector operator*(Vector v1,Vector v2) {
    return Vector(
                v1.y * v2.z - v1.z*v2.y,
                v1.z * v2.x - v1.x*v2.z,
                v1.x * v2.y - v1.y*v2.x);
}*/

//fixed point ciel
inline UINT32 ceil(UINT32 x)
{

     x += 0xffff;
     return x >> 16;

}


//simple rotation routines
void simpleyrot(float r,vector *v)
{
    float cr=(float)cos(r);    
    float sr=(float)sin(r);
    float tx=(float)v->x;
    v->x=v->x*cr + v->z*-sr;
    v->z=tx*sr +  v->z*cr;    
}

void simplexrot(float r,vector *v)
{
    float cr=(float)cos(r);    
    float sr=(float)sin(r);    
    float ty=(float)v->y;
    v->y=(v->y*cr) + (v->z*sr);
    v->z=(ty*-sr)  + (v->z*cr);
}    



vector planelineintersection(vector n,flt d,vector p1,vector p2)
{
        // returns the point where the line p1-p2 intersects the plane n&d
        vector dif  = p2-p1;
        flt dn= n^dif;
        flt  t = -(d+(n^p1) )/dn;
        return p1 + (dif*t);
}

BOOL concurrent(vector a,vector b)
{
        return(a.x==b.x && a.y==b.y && a.z==b.z);
}


//from hulio elasis
flt linear_interpolate(flt a, flt b, flt x)
{
      return  a*(1-x) + b*x;
}

// Cosine Interpolation: 
//This method gives a much smother curve than Linear
// Interpolation. It's clearly better and worth the effort if you can afford
//the very slight loss in speed.  
flt cosine_interpolate(flt a, flt b, flt x)
{
    //=a * (1.0- ((1.0-cos(x*PI))/2) + b* (1.0-cos(x*PI))/2)
    flt f = (1.0 - cos(x * 3.1415927)) * 0.5;
    return  a*(1.0-f) + b*f;
}

flt intnoise(UINT32 x)                   
{
   x = (x<<13) ^ x;
   return ( 1.0 - ( (x * (x * x * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);       
}

vector *vector::multmatrix(matrix4x4 *mat,vector *into)
{
    into->x=(x*mat->m00 +y*mat->m01 +z*mat->m02);
    into->y=(x*mat->m10 +y*mat->m11 +z*mat->m12);
    into->z=(x*mat->m20 +y*mat->m21 +z*mat->m22);
    return into;
}

vector *vector::multmatrixtrans(matrix4x4 *mat,vector *into)
{
    into->x=(x*mat->m00 +y*mat->m01 +z*mat->m02)+mat->p.x;
    into->y=(x*mat->m10 +y*mat->m11 +z*mat->m12)+mat->p.y;
    into->z=(x*mat->m20 +y*mat->m21 +z*mat->m22)+mat->p.z;
    return into;
}


vector *vector::multmatrixinv(matrix4x4 *mat,vector *into)
{
    into->x=(x*mat->m00 +y*mat->m10 +z*mat->m20);
    into->y=(x*mat->m01 +y*mat->m11 +z*mat->m21);
    into->z=(x*mat->m02 +y*mat->m12 +z*mat->m22);
    return into;
}

vector *vector::multmatrixtransinv(matrix4x4 *mat,vector *into)
{
    into->x=(x*mat->m00 +y*mat->m10 +z*mat->m20)-mat->p.x;
    into->y=(x*mat->m01 +y*mat->m11 +z*mat->m21)-mat->p.y;
    into->z=(x*mat->m02 +y*mat->m12 +z*mat->m22)-mat->p.z;
    return into;
}


matrix4x4ptr LMrottrans(matrix4x4ptr m,flt rx,flt ry,flt rz,flt xt,flt yt,flt zt)
{
  flt cx=cos(rx);
  flt sx=sin(rx);
  flt cy=cos(ry);
  flt sy=sin(ry);
  flt cz=cos(rz);
  flt sz=sin(rz);

  m->m00=cy*cz;          m->m01=cy*sz;          m->m02=-sy;    
  m->m10=sx*sy*cz-cx*sz; m->m11=sx*sy*sz+cx*cz; m->m12=sx*cy;    
  m->m20=cx*sy*cz+sx*sz; m->m21=cx*sy*sz-sx*cz; m->m22=cx*cy;
  m->p.moveto(xt,yt,zt);
  return m;
}

matrix4x4ptr LMrottransinv(matrix4x4ptr m,flt rx,flt ry,flt rz,flt xt,flt yt,flt zt)
{
  /*flt cx=cos(rx);
  flt sx=sin(rx);
  flt cy=cos(ry);
  flt sy=sin(ry);
  flt cz=cos(rz);
  flt sz=sin(rz);
    
  m->m00=cy*cz;          m->m01=cy*sz;         m->m02=-sy;  
  m->m10=sx*sy*cz+cx*-sz;m->m11=sx*sy*sz+cx*cz;m->m12=sx*cy;
  m->m20=cx*sy*cz-sx*-sz;m->m21=cx*sy*sz-sx*cz;m->m22=cx*cy;*/

  matrix4x4 tmpmat;
  LMxrotation(m,rx);
  LMyrotation(&tmpmat,ry);
  LMconcatenatematrices(&tmpmat,m,m);
  LMzrotation(&tmpmat,rz);
  LMconcatenatematrices(&tmpmat,m,m);
  m->p.moveto(xt,yt,zt);    
  return m;
}

matrix4x4ptr LMrotatearound(matrix4x4ptr mat,flt rx,flt ry,flt rz,vector *vec)
{
  matrix4x4 tmpmat;
  matrix4x4 tmpmat2;
  
  LMtranslate(mat,-vec->x,-vec->y,-vec->z);  
  LMrottransinv(&tmpmat,rx,ry,rz,0,0,0);
  LMconcatenatematrices(mat,&tmpmat,&tmpmat2);
  LMtranslate(&tmpmat,vec->x,vec->y,vec->z);
  LMconcatenatematrices(&tmpmat2,&tmpmat,mat);
  return mat;
}


matrix4x4ptr LMtransrot(matrix4x4ptr mat,flt rx,flt ry,flt rz,flt xt,flt yt,flt zt)
{
  matrix4x4 tmpmat;
  LMtranslate(mat,xt,yt,zt);  
  LMxrotation(&tmpmat,rx);
  LMconcatenatematrices(&tmpmat,mat,mat);
  LMyrotation(&tmpmat,ry);
  LMconcatenatematrices(&tmpmat,mat,mat);
  LMzrotation(&tmpmat,rz);
  LMconcatenatematrices(&tmpmat,mat,mat);
  return mat;
}


void LMtranspose(matrix4x4ptr mat)
{
   matrix4x4 tmpmat;
   tmpmat=*mat;
   mat->m01=tmpmat.m10;
   mat->m02=tmpmat.m20;
   mat->m10=tmpmat.m01;
   mat->m12=tmpmat.m21;
   mat->m20=tmpmat.m02;
   mat->m21=tmpmat.m12;
}

void LMinvert(matrix4x4ptr mat)
{
   flt tmppx,tmppy;
   LMtranspose(mat);
   tmppx=mat->p.x;tmppy=mat->p.y;
   mat->p.x =(-mat->m00*tmppx)+(-mat->m01*tmppy)+(-mat->m02*mat->p.z);
   mat->p.y =(-mat->m10*tmppx)+(-mat->m11*tmppy)+(-mat->m12*mat->p.z);
   mat->p.z =(-mat->m20*tmppx)+(-mat->m21*tmppy)+(-mat->m22*mat->p.z);
}


matrix4x4ptr LMconcatenatematrices(register matrix4x4ptr m1,register matrix4x4ptr m2,matrix4x4ptr result)
{
  static matrix4x4 temp;
  //check the pointers, if they are the same, DO SOMETHING! 
  //if (m1==m2) return m1;
  if (m2==result)
  {
     temp=*m2;
     m2=&temp;
  }else if (m1==result)
  {
     temp=*m1;
     m1=&temp;
  }
  result->m00=(m1->m00*m2->m00 + m1->m01*m2->m10 + m1->m02*m2->m20);
  result->m10=(m1->m10*m2->m00 + m1->m11*m2->m10 + m1->m12*m2->m20);
  result->m20=(m1->m20*m2->m00 + m1->m21*m2->m10 + m1->m22*m2->m20);

  result->m01=(m1->m00*m2->m01 + m1->m01*m2->m11 + m1->m02*m2->m21);
  result->m11=(m1->m10*m2->m01 + m1->m11*m2->m11 + m1->m12*m2->m21);
  result->m21=(m1->m20*m2->m01 + m1->m21*m2->m11 + m1->m22*m2->m21);

  result->m02=(m1->m00*m2->m02 + m1->m01*m2->m12 + m1->m02*m2->m22);
  result->m12=(m1->m10*m2->m02 + m1->m11*m2->m12 + m1->m12*m2->m22);
  result->m22=(m1->m20*m2->m02 + m1->m21*m2->m12 + m1->m22*m2->m22);

  result->p.x=(m1->m00*m2->p.x + m1->m01*m2->p.y + m1->m02*m2->p.z)+m1->p.x;
  result->p.y=(m1->m10*m2->p.x + m1->m11*m2->p.y + m1->m12*m2->p.z)+m1->p.y;
  result->p.z=(m1->m20*m2->p.x + m1->m21*m2->p.y + m1->m22*m2->p.z)+m1->p.z;

/*  result->p.x=m1->p.x*m2->m00 + m1->p.y*m2->m10 + m1->p.z*m2->m20 + m2->p.x;
  result->p.y=m1->p.x*m2->m01 + m1->p.y*m2->m11 + m1->p.z*m2->m21 + m2->p.y;
  result->p.z=m1->p.x*m2->m02 + m1->p.y*m2->m12 + m1->p.z*m2->m22 + m2->p.z;

  */
  /*
  result->m00=m1->m00*m2->m00 + m1->m01*m2->m10 + m1->m02*m2->m20;
  result->m01=m1->m00*m2->m01 + m1->m01*m2->m11 + m1->m02*m2->m21;
  result->m02=m1->m00*m2->m02 + m1->m01*m2->m12 + m1->m02*m2->m22;
  result->m03=0;
  
  result->m10=m1->m10*m2->m00 + m1->m11*m2->m10 + m1->m12*m2->m20;
  result->m11=m1->m10*m2->m01 + m1->m11*m2->m11 + m1->m12*m2->m21;
  result->m12=m1->m10*m2->m02 + m1->m11*m2->m12 + m1->m12*m2->m22;

  
  result->m20=m1->m20*m2->m00 + m1->m21*m2->m10 + m1->m22*m2->m20;
  result->m21=m1->m20*m2->m01 + m1->m21*m2->m11 + m1->m22*m2->m21;
  result->m22=m1->m20*m2->m02 + m1->m21*m2->m12 + m1->m22*m2->m22;
  result->m23=0;
  
  result->p.x=m1->p.x*m2->m00 + m1->p.y*m2->m10 + m1->p.z*m2->m20 + m2->p.x;
  result->p.y=m1->p.x*m2->m01 + m1->p.y*m2->m11 + m1->p.z*m2->m21 + m2->p.y;
  result->p.z=m1->p.x*m2->m02 + m1->p.y*m2->m12 + m1->p.z*m2->m22 + m2->p.z;
  result->m33=1;
  */
  return result;
}

/*

*/



vector *LMmatrixtransformvec(register matrix4x4ptr mat,register vector *vec,vector *result)
{
    static vector tempvec;
    if (vec==result)
    {
        tempvec=*vec;
        vec=&tempvec;
    }
    result->x=(mat->m00*vec->x) + (mat->m10*vec->y) + (mat->m20*vec->z) +mat->p.x;
    result->y=(mat->m01*vec->x) + (mat->m11*vec->y) + (mat->m21*vec->z) +mat->p.y;
    result->z=(mat->m02*vec->x) + (mat->m12*vec->y) + (mat->m22*vec->z) +mat->p.z;
    
    return result;
}

matrix4x4ptr LMinversematrix(matrix4x4ptr mat)
{
      return mat;
}    

//[1 0 0 0]
//[0 1 0 0]
//[0 0 1 0]
//[0 0 0 1]

matrix4x4ptr LMidentity(matrix4x4ptr mat)
{
    
    mat->m01 = mat->m02 = 
    mat->m10 = mat->m12 = 
    mat->m20 = mat->m21 = 
    mat->p.x = mat->p.y = mat->p.z=0;
    
    //now the diagonal entries
    mat->m00=mat->m11=mat->m22=1;
    return mat;
}


//[1  0   0   0]
//[0  cr  sr  0]
//[0  -sr cr  0]
//[0  0   0   1]

matrix4x4ptr LMxrotation(register matrix4x4ptr mat,flt rotation)
{
    flt cr=cos(rotation);
    flt sr=sin(rotation);
       
    mat->m11=cr;     mat->m12=sr; 
    mat->m21=-sr;    mat->m22=cr; 
    
    //the rest are as in an identity matrix
    mat->m01 = mat->m02 =  mat->m10 = mat->m20 =
    mat->p.x = mat->p.y = mat->p.z=0;        
    //remaining identity diagonals (1's)
    mat->m00=1;
    return mat;
}


//[cr 0  -sr 0]
//[0  1  0   0]
//[sr 0  cr  0]
//[0  0  0   1]


matrix4x4ptr LMyrotation(register matrix4x4ptr mat,flt rotation)
{
    
    flt cr=cos(rotation);
    flt sr=sin(rotation);
    
    mat->m00=cr;  mat->m02=-sr;
    mat->m20=sr;  mat->m22=cr;
   
    //the rest are as in an identity matrix
    mat->m01 = mat->m10 = mat->m12 = mat->m21 =       
    mat->p.x = mat->p.y = mat->p.z=0;             
    //remaining identity diagonals (1's)
    mat->m11 =1;
    return mat;
}

//[cr  sr  0  0]
//[-sr cr  0  0]
//[0   0   1  0]
//[0   0   0  1]

matrix4x4ptr LMzrotation(register matrix4x4ptr mat,flt rotation)
{
    
    flt cr=cos(rotation);
    flt sr=sin(rotation);
    
    mat->m00=cr;  mat->m01=sr; 
    mat->m10=-sr; mat->m11=cr; 
   
    //the rest are as in an identity matrix
    mat->m02 = mat->m12 = mat->m20 = 
    mat->m21 = mat->p.x = mat->p.y = mat->p.z=0; 
    //remaining identity diagonals (1's)
    mat->m22 = 1;
    return mat;
}
matrix4x4ptr LMxyzrotation(register matrix4x4ptr m,flt xrotation,flt yrotation,flt zrotation)
{
    /*matrix4x4 temp;
    LMxrotation(mat,xrotation);
    LMyrotation(&temp,yrotation);
    LMconcatenatematrices(mat,&temp,mat);
    LMzrotation(&temp,zrotation);
    LMconcatenatematrices(mat,&temp,mat);*/
    
    flt cx=cos(xrotation);
    flt sx=sin(xrotation);
    flt cy=cos(yrotation);
    flt sy=sin(yrotation);
    flt cz=cos(zrotation);
    flt sz=sin(zrotation);

    m->m00=cy*cz;          m->m01=cy*sz;         m->m02=-sy;   
    m->m10=sx*sy*cz-cx*sz; m->m11=sx*sy*sz+cx*cz;m->m12=sx*cy;
    m->m20=cx*sy*cz+sx*sz; m->m21=cx*sy*sz-sx*cz;m->m22=cx*cy;

    m->p=0;        
    return m;
}

matrix4x4ptr LMaddscale(register matrix4x4ptr m,register matrix4x4ptr mr,flt scx,flt scy,flt scz)
{
    mr->m00=m->m00*scx; mr->m01=m->m01*scy; mr->m02=m->m02*scz;
    mr->m10=m->m10*scx; mr->m11=m->m11*scy; mr->m12=m->m12*scz;
    mr->m20=m->m20*scx; mr->m21=m->m21*scy; mr->m22=m->m22*scz;
    mr->p=m->p;
    return mr;
}

matrix4x4ptr LMrst(register matrix4x4ptr m,flt xrotation,flt yrotation,flt zrotation,flt scx,flt scy,flt scz,flt tx,flt ty,flt tz)
{    
    flt cx=cos(xrotation);
    flt sx=sin(xrotation);
    flt cy=cos(yrotation);
    flt sy=sin(yrotation);
    flt cz=cos(zrotation);
    flt sz=sin(zrotation);

    m->m00=cy*cz*scx;          m->m01=cy*sz*scy;         m->m02=-sy*scz;  
    m->m10=sx*sy*cz+cx*-sz*scx;m->m11=sx*sy*sz+cx*cz*scy;m->m12=sx*cy*scz;
    m->m20=cx*sy*cz-sx*-sz*scx;m->m21=cx*sy*sz-sx*cz*scy;m->m22=cx*cy*scz;
    m->p.moveto(tx,ty,tz);    
    
    return m;
}


//pythag: a =sqrt( b*b + c*c )
//theta=sin(opp/adj)
flt getangle(INT16 ax,INT16 ay,INT16 bx,INT16 by)
{
    ax-=bx;
    ay-=by;
    
    return atan(ay/ax);
}

matrix4x4ptr LMaxisrotation(register matrix4x4ptr mat,flt x,flt y,flt z,flt rotation)
{
    flt c=cos(rotation),s=sin(rotation),t=1-c,
    txz=t*x*z,tyz=t*y*z,txy=t*x*y,
    sx=s*x,sy=s*y,sz=s*z;
    
    if ((x==0)&&(y==0)&&(z==0)){
       LMidentity(mat);
       return mat;
    }

   
    mat->m00=t*x*x+c;   mat->m01=txy+sz;    mat->m02=txz-sy;
    mat->m10=txy-sz;    mat->m11=t*y*y+c;   mat->m12=tyz+sx;
    mat->m20=txz+sy;    mat->m21=tyz-sx;    mat->m22=t*z*z+c;
    
    mat->p.x=mat->p.y=mat->p.z=0;
    return mat;
}    
    
matrix4x4ptr LMzyxrotation(register matrix4x4ptr m,flt xrotation,flt yrotation,flt zrotation)
{
    flt cx=cos(xrotation);
    flt sx=sin(xrotation);
    flt cy=cos(yrotation);
    flt sy=sin(yrotation);
    flt cz=cos(zrotation);
    flt sz=sin(zrotation);
    
    m->m00=cy*cz;          m->m01=cy*sz;         m->m02=-sy;  
    m->m10=sx*sy*cz+cx*-sz;m->m11=sx*sy*sz+cx*cz;m->m12=sx*cy;
    m->m20=cx*sy*cz-sx*-sz;m->m21=cx*sy*sz-sx*cz;m->m22=cx*cy;
    m->p.moveto(0,0,0);    
    return m;
}    
        


//[x 0 0 0]
//[0 y 0 0]
//[0 0 z 0]
//[0 0 0 1]

matrix4x4ptr LMscale(register matrix4x4ptr mat,flt xscale,flt yscale,flt zscale)
{  
   
   mat->m01 = mat->m02 = 
   mat->m10 = mat->m12 = 
   mat->m20 = mat->m21 = 
   mat->p.x = mat->p.y = mat->p.z=0;
    
    //now the diagonal entries
   mat->m00 = xscale;
   mat->m11 = yscale; 
   mat->m22 = zscale;

   return mat;
}


/*  multby:=oldmax;
  multby.sub(oldmin.x,oldmin.y,oldmin.z);
  with multby do begin
    if x=0 then x:=1;
    x:=(newmax.x-newmin.x)/x;
    if y=0 then y:=1;
    y:=(newmax.y-newmin.y)/y;
    if z=0 then z:=1;
    z:=(newmax.z-newmin.z)/z;
  end;
  with mat^ do begin
    m00:=multby.x; m01:=0;        m02:=0;
    m10:=0;        m11:=multby.y; m12:=0;
    m20:=0;        m21:=0;        m22:=multby.z;
    p.x:=(-oldmin.x)*multby.x+newmin.x;
    p.y:=(-oldmin.y)*multby.y+newmin.y;
    p.z:=(-oldmin.z)*multby.z+newmin.z;
*/
matrix4x4ptr LMstretch(register matrix4x4ptr mat,vector *oldmin,vector *oldmax,vector *newmin,vector *newmax)
{
   vector multby;
   matrix4x4 tmpmat;
   multby.x=oldmax->x-oldmin->x;
   multby.y=oldmax->y-oldmin->y;
   multby.z=oldmax->z-oldmin->z;
   
   if (multby.x==0) multby.x=1;
   multby.x=(newmax->x-newmin->x)/multby.x;
   if (multby.y==0) multby.y=1;
   multby.y=(newmax->y-newmin->y)/multby.y;
   if (multby.z==0) multby.z=1;
   multby.z=(newmax->z-newmin->z)/multby.z;   
  
   mat->m00=multby.x; mat->m01=0;        mat->m02=0;
   mat->m10=0;        mat->m11=multby.y; mat->m12=0;
   mat->m20=0;        mat->m21=0;        mat->m22=multby.z;
   mat->p.x=(-oldmin->x)*multby.x+newmin->x;
   mat->p.y=(-oldmin->y)*multby.y+newmin->y;
   mat->p.z=(-oldmin->z)*multby.z+newmin->z;
   return mat;
};


//[1 0 0 0]
//[0 1 0 0]
//[0 0 1 0]
//[x y z 1]

matrix4x4ptr LMtranslate(register matrix4x4ptr mat,flt xtrans,flt ytrans,flt ztrans)
{
   mat->p.moveto(xtrans,ytrans,ztrans);
   
   mat->m01 = mat->m02 =
   mat->m10 = mat->m12 =
   mat->m20 = mat->m21 = 0;    
    //now the diagonal entries
   mat->m00 = mat->m11 = mat->m22 =1;
   return mat;
}



void calcnormal(vector p1,vector &p2,vector p3,vector & pn)
{
    //make magnitude vectors, P2 being the base}
    p3-=p2;
    p1-=p2;

    pn.x=(p1.y*p3.z) - (p1.z*p3.y);
    pn.y=(p1.z*p3.x) - (p1.x*p3.z);
    pn.z=(p1.x*p3.y) - (p1.y*p3.x);
    pn.normalize();
}

//Generalized power function by Prof. Timo Salmi *)
flt genpower (flt number, flt exponent )
{
    return exp(exponent*log(number));
/*     if (exponent == 0.0f)
       return 1.0f
     else if (number == 0.0)
       return 0.0f
     else if (abs(exponent*ln(abs(number))) > 87.498 then
       begin writeln ('Overflow in GENPOWFN expression'); halt; end
     else if (number > 0.0)
       
     else if ((number < 0.0) && (frac(exponent) == 0.0))
       if Odd(Round(exponent)) then
         genpowfn := -GENPOWFN (-number, exponent)
       else
         genpowfn :=  GENPOWFN (-number, exponent)
     else
       begin writeln ('Invalid GENPOWFN expression'); halt; end;
   end;*/
}


flt ipow(flt val,int power)
{
    //return exp((flt)power*log(val));
    //if (power==1) return 1.0f;
    UINT8 n=power<0;
    flt rval=val;
    power=ABS(power);    
    while (power>1)
    {
         rval*=val;        
         power--;
    }
    if (n)
      return 1.0f/rval;
    return rval;                
}    
    
        


flt dot(vector &vec1,vector &vec2)
{
    return  ((vec1.x*vec2.x) +
              (vec1.y*vec2.y) +
              (vec1.z*vec2.z));
}

vector cross(vector &vec1,vector &vec2)
{
    vector rvec;
    rvec.x=(vec1.y*vec2.z) - (vec1.z*vec2.y);
    rvec.y=(vec1.z*vec2.x) - (vec1.x*vec2.z);
    rvec.z=(vec1.x*vec2.y) - (vec1.y*vec2.x);
    return rvec;
}    

void reflection(vector &vec1,vector &vec2,vector &rvec)
{
    flt dp=dot(vec1,vec2);

    rvec.x=vec1.x - 2*dp*vec2.x;
    rvec.y=vec1.y - 2*dp*vec2.y;
    rvec.z=vec1.z - 2*dp*vec2.z;        
}
    
flt distance(vector &vec1,vector &vec2)
{
    vector diff=(vec1-vec2);    
    return sqrt((diff.x*diff.x)+(diff.y*diff.y)+(diff.z*diff.z)); 
}    



