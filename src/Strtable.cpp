#include "stdafx.h"
#include "strtable.hpp"
char EXE_PATH[255];
//Error strings
unsigned short ERR_LEVEL;
char ERR_STRINGS[10][50]={"Everything is fine",
                          "VESA driver not available",
                          "No Linear buffer",
                          "Requested is mode not available",
                          "Not enough low (DOS) memory"};
char noyesstr[2][10]={"No","Yes"};
char str_file[]="File";
char str_directories[]="Directories";

//char okcancelstr[2][7]={"Ok","Cancel"};



                          

