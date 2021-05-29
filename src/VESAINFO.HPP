#ifndef _vesainfo_h_
#define _vesainfo_h_

#include "ttypes.hpp"
#include "gbasics.hpp"


#define SVGAok      0x004F// Function is supported}
#define SVGAErr2    0x4F00// Function is not supported}
#define SVGAErr3            0x0000// Function call successful}
#define SVGAErr4            0x0100// Function call failed}
#define SVGAErr5            0x0200// Software supports this function, but the hardware does not}
#define SVGAErr6            0x0300//Function call invalid in current video mode}

#define VESAMODEATTRIB_NOTSUPPORTED 0x1
#define VESAMODEATTRIB_RESERVED1 0x2
#define VESAMODEATTRIB_TTYOUTPUT 0x4
#define VESAMODEATTRIB_COLOUR 0x8
#define VESAMODEATTRIB_GRAPHICS 0x10
#define VESAMODEATTRIB_NOTVGACOMPATABLE 0x20
#define VESAMODEATTRIB_NOTWINDOWED 0x40
#define VESAMODEATTRIB_LINEARBUFFER 0x80



//Errstrings:array[0..5] of string=('Function is supported'}
#define TEST_SVGA640x400x256 5
#define TEST_SVGA640x480x256 1
#define TEST_SVGA800x600x256 3
#define TEST_SVGA1024x768x256 5 


/* capabilities
 D0     = 0     DAC is fixed width, with 6 bits per primary color
            = 1     DAC width is switchable to 8 bits per primary color
     D1     = 0     Controller is VGA compatible
            = 1     Controller is not VGA compatible
     D2     = 0     Normal RAMDAC operation
            = 1     When programming large blocks of information to the
                    RAMDAC use blank bit in Function 09h. i.e. RAMDAC
                    recommends programming during blank period only.
*/

#define VIDEOLISTERMINATOR 0x0FFFF
/*
struct VBE_VbeInfoBlock
{
  char           vbeSignature[4];
  short   vbeVersion;
  char *         OemStringPtr;
  unsigned long  Capabilities;
  unsigned short * VideoModePtr;
  unsigned short TotalMemory;
  unsigned short OemSoftwareRev;
  char *         OemVendorNamePtr;
  char *         OemProductNamePtr;
  char *         OemProductRevPtr;
  char           Reserved[222];
  char           OemData[256];
};
*/
typedef _Packed struct headerblocktype
{
  char sig[4];
  UINT16 version;
  char *         OEMStringPtr;
  unsigned long  Capabilities;
  unsigned short * VideoModePtr;
  unsigned short TotalMemory;
  unsigned short OemSoftwareRev;
  char *         OemVendorNamePtr;
  char *         OemProductNamePtr;
  char *         OemProductRevPtr;
  char           Reserved[222];
  char           OemData[256]; 
} headerblocktype;





typedef _Packed struct
{
  // Mandatory information for all VBE revisions:

  unsigned short  ModeAttributes;        // mode attributes
  unsigned char   WinAAttributes;        // window A attributes
  unsigned char   WinBAttributes;        // window B attributes
  unsigned short  WinGranularity;        // window granularity
  unsigned short  WinSize;               // window size
  unsigned short  WinASegment;           // window A start segment
  unsigned short  WinBSegment;           // window B start segment
  unsigned long   WinFuncPtr;            // pointer to window function
  unsigned short  BytesPerScanLine;      // bytes per scan line

  // Mandatory information for VBE 1.2 and above:

  unsigned short  XResolution;           // horizontal resolution in pixels or chars
  unsigned short  YResolution;           // vertical resolution in pixels or chars
  unsigned char   XCharSize;             // character cell width in pixels
  unsigned char   YCharSize;             // character cell height in pixels
  unsigned char   NumberOfPlanes;        // number of memory planes
  unsigned char   BitsPerPixel;          // bits per pixel
  unsigned char   NumberOfBanks;         // number of banks
  unsigned char   MemoryModel;           // memory model type
  unsigned char   BankSize;              // bank size in KB
  unsigned char   NumberOfImagePages;    // number of images
  unsigned char   _Reserved;             // reserved for page function

  // Direct Color fields (required for direct/6 and YUV/7 memory models)

  unsigned char   RedMaskSize;           // size of direct color red mask in bits
  unsigned char   RedFieldPosition;      // bit position of lsb of red mask
  unsigned char   GreenMaskSize;         // size of direct color green mask in bits
  unsigned char   GreenFieldPosition;    // bit position of lsb of green mask
  unsigned char   BlueMaskSize;          // size of direct color blue mask in bits
  unsigned char   BlueFieldPosition;     // bit position of lsb of blue mask
  unsigned char   RsvdMaskSize;          // size of direct color reserved mask in bits
  unsigned char   RsvdFieldPosition;     // bit position of lsb of reserved mask
  unsigned char   DirectColorModeInfo;   // direct color mode attributes

  // Mandatory information for VBE 2.0 and above:

  unsigned long   PhysBasePtr;           // physical address for flat frame buffer
  unsigned long   OffScreenMemOffset;    // pointer to start of off screen memory
  unsigned short  OffScreenMemSize;      // amount of off screen memory in 1k units
  char            __Reserved[206];
} infoblocktype;

extern BOOL lfb;
void vesa_copyscreen(char *dest,char *src,UINT32 size);
char vesa_setmode(screenIDType &sid);
void vesa_close(void);

#endif
