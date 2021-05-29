#ifndef _ttypes_h_
#define _ttypes_h_

#define _WIN_VER_

#ifndef NULL
#define NULL 0
#endif
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#define NEITHERTRUENORFALSE -1

//logical modes
#define bPUT 0   //just write the bits, no logical operation
#define bAND 1
#define bOR  2
#define bXOR 3
#define bNOT 4


//#define doublefloatpresicion
#if defined doublefloatpresicion
   #define flt double  
#else
   #define flt float
#endif

//The standard integer type for defining pixel values
#define PIXINT INT32
#define MIN_INT16 -32767
#define MAX_INT16 32767
#define MIN_INT32 -2147483648
#define MAX_INT32 2147483648


#define right  0x01
#define bellow 0x02
#define left   0x04
#define above  0x08

#define above_right  above+right
#define bellow_right bellow+right
#define above_left   above+left
#define bellow_left  bellow+left
#define middle       above+bellow+right+left     


#define SMleftshiftkey 2
#define SMleftctrlkey 4
#define SMleftaltkey 8
#define SMrightshiftkey 2
#define SMrightctrlkey 4
#define SMrightaltkey 8

#define tabkey 9  
#define insertkey 0
#define rightkey 77
#define rightkeyctrl 116 
#define leftkey 75 
#define leftkeyctrl 115 
#define upkey 72 
#define downkey 80 
#define pageupkey 73 
#define pagedownkey 81 
#define homekey 71 
#define delkey 83 
#define endkey 79 
#define backspacekey 8 
#define escapekey 27 
#define returnkey 13
#define stringsize 20

#ifndef BOOL
typedef int BOOL;
#endif

typedef unsigned char UINT8;
typedef signed char INT8;
typedef unsigned short UINT16 ;
typedef signed short INT16;
typedef unsigned int UINT32  ;
typedef signed int INT32 ;
//typedef unsigned int UINT64;
//typedef int INT64;


//typedef char[stringsize] stdstring;
//typedef UINT8 bool;
typedef double *doubleptr;
typedef float *floatptr;
typedef UINT8 *UINT8ptr;
typedef INT8 *INT8ptr;
typedef UINT16 *UINT16ptr;
typedef INT16 *INT16ptr;
typedef UINT32 *UINT32ptr;
typedef INT32 *INT32ptr;


class ttreenode
{
    public:
    ttreenode *next,*child,*parent;
};


class ttree
{
public:
    UINT16 count,size;
    ttreenode* root;
};

//Basic Linked list type



class telem
{
    public:
    telem *next;    
};

typedef telem *telemptr;



class telemdouble
{
    public:
    telemdouble *next,*prev;
};

typedef telemdouble *telemdoubleptr;



typedef BOOL (*linkcriteriafunction)(telem *elem);
typedef BOOL (*linkcleanupfunction)(telem *elem);


class tlinkedlist
{
public:
    UINT32 count;
    UINT32 size;
    telemptr first,last;
    
    init(UINT16 size);
    destroy(void (*cleanup)(telem *element)=NULL);
    reset(){first=last=NULL;count=0;}
    void remove(telemptr popped,void (*cleanup)(telem *element)=NULL);
    BOOL removelots(linkcriteriafunction lfunc,linkcleanupfunction cfunc=NULL);
    void push(telemptr newelement);
    void addtofront(telemptr newelement);
    UINT32 getindex(telemptr elem);  
    telemptr findelement(INT32 count);
    telemptr pop(telemptr popped,void (*cleanup)(telem *element)=NULL);
    telemptr prev(telemptr elem);
    telemptr prevwrap(telemptr elem);
    telemptr nextwrap(telemptr elem);
    telemptr getlast(){return last;};
    telemptr getnext_check(telemptr elem){return elem?elem->next:first;};
    void repairlast();
};

//simple list stuff, no tlinkedlist structure. Less flexible, less storage space.

telem *SLadd(telem **prevelem,telemptr newelem);
void SLremove(telemptr prev,telemptr removeelem);
BOOL SLremovelots(telem** first,linkcriteriafunction lfunc,linkcleanupfunction cfunc);
telemptr SLprev(telemptr first,telemptr current);
telemptr SLfindlast(telemptr first);
void SLaddtoend(telemptr *first,telemptr newe);
void SLdeletelist(telemptr first);
void SLreverselist(telemptr first);
UINT32 SLgetlength(telemptr first);
telemptr SLcountfrom(telemptr start,UINT32 count);

class theapmem
{
    public:
    char *base,*curr;
    UINT32 size,currindex;

    char *create(UINT32 size_);
    void destroy();
    void reset();
    char *grab(UINT32 size);
    void *grabandsetpointer(void *indata);
    char *ungrab(UINT32 size);
};

//Tree Functions
ttreenode *TTtraversetonext(ttreenode *currnode,ttreenode *stopat);
void TTinittree(ttree *tree,UINT32 size);
ttreenode *TTcreatettreenode(ttree *tree);
ttreenode *TTaddchildtonode(ttree *tree,ttreenode *parent,ttreenode *child);
ttreenode *TTgetprevnode(ttree *tree,ttreenode *node); //get previous node on the level of 'node'
ttreenode *TTgetlastnode(ttreenode *node); //get last node entry on the level of 'node'
void TTdestroytree(ttreenode *tree);
UINT32 TTgetdepth(ttreenode *node);
void TTremovenode(ttree *tree,ttreenode *node);

typedef void* voidptr ;

#define MAKE_ID(A,B,C,D)        (((UINT32)A) | ((UINT32)(B)<<8) | ((UINT32)(C)<<16) | ((UINT32)(D)<<24))
#define NOT(X)                  ((X)==0)
#define ABS(X)                  ((X) < 0 ? -(X) : X)
#define MIN(A,B)                ((A) < (B) ? (A) : (B))
#define MAX(A,B)                ((A) > (B) ? (A) : (B))
#define BOUND(X,MI,MA)  {if(X<MI)X=MI;if(X>MA)X=MA;}

#define SETBIT(val,mask,setreset){(setreset?val|mask:val&~mask);}

inline UINT32 SETBITBYMODE(UINT32 val,UINT32 mask,UINT32 mode)
{
    switch (mode)
    {
        case bNOT:val &=~ mask;break;
        case bXOR:val ^= mask;break;
        case bOR:
        case bPUT:val |= mask;break;                                      
    }
    return val;
}

//#define swap(a,b){(a)^=(b);(b)^=(a);(b)^=(a);}
//#define SWAP(a,b){(a)^=(b);(b)^=(a);(b)^=(a);}

//#define swap(a,b) (a)=(a)^(b);(b)=(b)^(a);(a)=(a)^(b)

inline void swap(UINT32 &a, UINT32 &b){UINT32 t;t=a;a=b;b=t;}
inline void swap(UINT16 &a, UINT16 &b){UINT32 t;t=a;a=b;b=t;}
inline void swap(UINT8 &a, UINT8 &b){UINT32 t;t=a;a=b;b=t;}
inline void swap(INT32 &a, INT32 &b){INT32 t;t=a;a=b;b=t;}
inline void swap(INT16 &a, INT16 &b){INT32 t;t=a;a=b;b=t;}
inline void swap(INT8 &a, INT8 &b){INT32 t;t=a;a=b;b=t;}
inline void swap(voidptr &a, voidptr &b){voidptr t;t=a;a=b;b=t;}


//extern void fswapi(void *a,void *b,UINT32 len);
//string operations
void fswapi(void *a,void *b,UINT32 len);
bool strcontains(char *str,char c);
char *getdir(char *dir,char *dest);
UINT32 getdepth(char *dir);
char *setdepth(char *dir,UINT32 depth);
char *truncatepathto(char *path,char *dir);
char *copyfilter(char *tgt,char *src);
char strgreater(char *a,char *b);

#define UNZERO(A){if (A==0) A=1;}
#define SWAPCHECK(a,b){if (a>b) SWAP(a,b);}






#endif
