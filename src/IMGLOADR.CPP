//imgloadr.cpp generic interface for image loading

#include "gbasics.hpp"
trgba lcmaprgb[256];
tcolour lcmap[256];


trgba *getimage_rgbpalette()
{
    return lcmaprgb;
}

tcolour *getimage_nativepalette()
{
    return lcmap;
}

//09032009219
