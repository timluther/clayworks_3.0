class trational4dpoint
{
   public:    
   flt      x;
   flt      y;
   flt      z;
   flt      w;
};

class tnurbcurve
{
    public:
    UINT32         order;               // Order of the curve
    UINT32 numPoints;            // Number of control points
    trational4dpoint   *ctrlpts;      // Array of control points
    flt              *knots;               // Array of knots.. knots control the range of the point effect

    
};
