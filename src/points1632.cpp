class tpointbase16
{
    public:
    INT16 x,y;
};

class tpoint16:public tpointbase16
{      
    public:
    tpoint16(){x=y=0;}
    tpoint16(INT16 x_,INT16 y_){x=x_;y=y_;}
    
    tpoint16 &operator =(INT16 val){x=y=val;return *this;}    
    void assign(INT16 x1,INT16 y1){x=x1;y=y1;};
    BOOL closeto(tpoint *other,INT32 t){return ((x>=other->x-t)&&(y>=other->y-t)&&(x<=other->x+t)&&(y<=other->y+t));}
    tpoint16 &operator +(tpoint16 &other){static tpoint16 r;r.x=x+other.x;r.y=y+other.y;return r;}
    tpoint16 &operator +(INT16 v){static tpoint16 r;r.x=x+v;r.y=y+v;return r;}
    tpoint16 &operator -(tpoint16 &other){static tpoint16 r;r.x=x-other.x;r.y=y-other.y;return r;}
    tpoint16 &operator -(INT16 v){static tpoint16 r;r.x=x-v;r.y=y-v;return r;}
    tpoint16 &operator *(tpoint16 &other){static tpoint16 r;r.x=x*other.x;r.y=y*other.y;return r;}
    tpoint16 &operator *(INT16 v){static tpoint16 r;r.x=x*v;r.y=y*v;return r;}
    tpoint16 &operator /(tpoint16 &other){static tpoint16 r;r.x=x/other.x;r.y=y/other.y;return r;}
    tpoint16 &operator /(INT16 v){static tpoint16 r;r.x=x/v;r.y=y/v;return r;}
    tpoint16 &operator +=(tpoint16 &other){x+=other.x;y+=other.y;return *this;}
    tpoint16 &operator +=(INT16 v){x+=v;y+=v;return *this;}
    tpoint16 &operator -=(tpoint16 &other){x-=other.x;y-=other.y;return *this;}
    tpoint16 &operator -=(INT16 v){x-=v;y-=v;return *this;}
    tpoint16 &operator *=(tpoint16 &other){x*=other.x;y*=other.y;return *this;}
    tpoint16 &operator *=(INT16 v){x*=v;y*=v;return *this;}
    tpoint16 &operator /=(tpoint16 &other){x/=other.x;y/=other.y;return *this;}
    tpoint16 &operator /=(INT16 v){x/=v;y/=v;return *this;}    
    
    BOOL operator==(tpoint16 &other){return (x==other.x)&&(y==other.y);};
    BOOL operator==(INT16 v){return (x==v)&&(y==v);};
    BOOL operator!=(tpoint16 &other){return (x!=other.x)||(y!=other.y);};
    BOOL operator!=(INT16 v){return (x!=v)||(y!=v);};
    BOOL aligned(tpoint16 &other){return((x==other.x)||(y==other.y));};
};

class tpointbase32
{
    public:
    INT32 x,y;
};

class tpoint32:public tpointbase32
{      
    public:
    tpoint32(){x=y=0;}
    tpoint32(INT32 x_,INT32 y_){x=x_;y=y_;}
    
    tpoint32 &operator =(INT32 val){x=y=val;return *this;}    
    void assign(INT32 x1,INT32 y1){x=x1;y=y1;};
    BOOL closeto(tpoint *other,INT32 t){return ((x>=other->x-t)&&(y>=other->y-t)&&(x<=other->x+t)&&(y<=other->y+t));}
    tpoint32 &operator +(tpoint32 &other){static tpoint32 r;r.x=x+other.x;r.y=y+other.y;return r;}
    tpoint32 &operator +(INT32 v){static tpoint32 r;r.x=x+v;r.y=y+v;return r;}
    tpoint32 &operator -(tpoint32 &other){static tpoint32 r;r.x=x-other.x;r.y=y-other.y;return r;}
    tpoint32 &operator -(INT32 v){static tpoint32 r;r.x=x-v;r.y=y-v;return r;}
    tpoint32 &operator *(tpoint32 &other){static tpoint32 r;r.x=x*other.x;r.y=y*other.y;return r;}
    tpoint32 &operator *(INT32 v){static tpoint32 r;r.x=x*v;r.y=y*v;return r;}
    tpoint32 &operator /(tpoint32 &other){static tpoint32 r;r.x=x/other.x;r.y=y/other.y;return r;}
    tpoint32 &operator /(INT32 v){static tpoint32 r;r.x=x/v;r.y=y/v;return r;}
    tpoint32 &operator +=(tpoint32 &other){x+=other.x;y+=other.y;return *this;}
    tpoint32 &operator +=(INT32 v){x+=v;y+=v;return *this;}
    tpoint32 &operator -=(tpoint32 &other){x-=other.x;y-=other.y;return *this;}
    tpoint32 &operator -=(INT32 v){x-=v;y-=v;return *this;}
    tpoint32 &operator *=(tpoint32 &other){x*=other.x;y*=other.y;return *this;}
    tpoint32 &operator *=(INT32 v){x*=v;y*=v;return *this;}
    tpoint32 &operator /=(tpoint32 &other){x/=other.x;y/=other.y;return *this;}
    tpoint32 &operator /=(INT32 v){x/=v;y/=v;return *this;}    
    
    BOOL operator==(tpoint32 &other){return (x==other.x)&&(y==other.y);};
    BOOL operator==(INT32 v){return (x==v)&&(y==v);};
    BOOL operator!=(tpoint32 &other){return (x!=other.x)||(y!=other.y);};
    BOOL operator!=(INT32 v){return (x!=v)||(y!=v);};
    BOOL aligned(tpoint32 &other){return((x==other.x)||(y==other.y));};
};


