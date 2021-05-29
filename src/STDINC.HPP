//standard includes for TWIN 
//Tim Lewis 28/11/98

#ifndef _STD_INC_H_
#define _STD_INC_H_

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <dos.h>

#include "ttypes.hpp"    //basic definitions and types
#include "tmaths.hpp"    //Definitions for extended maths routines; including vectors, matricies and (soon) quaternions.
#include "strtable.hpp"  //all strings in project
#include "gbasics.hpp"   //core graphics functions/definitions
#include "dosstuff.hpp"  //Mostly bios stuff; miscelanous PC specific commands

#include "gprim.hpp"     //extended graphics functions; uses core graphics, device independant
#include "imgloadr.hpp"  //header for all image loading routines

#include "basic3d.hpp"   //3d engine

#include "views.hpp"     //core windows funcions
#include "widgets.hpp"   //higher level window functions
#include "dialogs.hpp"   //higher level window functions
#include "msmouse.hpp"   //controls mouse input and display 

#endif


