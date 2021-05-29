#include <dos.h>
#include <i86.h>
#include <conio.h>
#include "key.hpp"
#include "ttypes.hpp"
#include "dosstuff.hpp"

/*1   ESC                               44   Z
 2   1                                 45   X
 3   2                                 46   C
 4   3                                 47   V
 5   4                                 48   B
 6   5                                 49   N
 7   6                                 50   M
 8   7                                 51   , <
 9   8                                 52   . >
10   9                                 53   / ?
11   0                                 54   RIGHT SHIFT
12   - _                               55   *            (KEYPAD)
13   = +                               56   LEFT ALT
14   BACKSPACE                         57   SPACEBAR
15   TAB                               58   CAPSLOCK
16   Q                                 59   F1
17   W                                 60   F2
18   E                                 61   F3
19   R                                 62   F4
20   T                                 63   F5
21   Y                                 64   F6
22   U                                 65   F7
23   I                                 66   F8
24   O                                 67   F9
25   P                                 68   F10
26   [ {                               69   NUMLOCK      (KEYPAD)
27   ] }                               70   SCROLL LOCK
28   ENTER (RETURN)                    71   7 HOME       (KEYPAD)
29   LEFT CONTROL                      72   8 UP         (KEYPAD)
30   A                                 73   9 PGUP       (KEYPAD)
31   S                                 74   -            (KEYPAD)
32   D                                 75   4 LEFT       (KEYPAD)
33   F                                 76   5            (KEYPAD)
34   G                                 77   6 RIGHT      (KEYPAD)
35   H                                 78   +            (KEYPAD)
36   J                                 79   1 END        (KEYPAD)
37   K                                 80   2 DOWN       (KEYPAD)
38   L                                 81   3 PGDN       (KEYPAD)
39   ; :                               82   0 INSERT     (KEYPAD)
40   ' "                               83   . DEL        (KEYPAD)
41   ` ~                               87   F11
42   LEFT SHIFT                        88   F12
*/

char charcodes[]={0,27,'1','2','3','4','5','6','7','8','9','0','-','=',KEYBACKSPACE,0,'q','w','e','r','t','y','u','i','o','p','[',']',KEYRETURN,0,'a','b','c','d','e','f','g','h','i','j','k','l',';','#','z','x','c','v','b','n','m',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
char shiftcharcodes[]={0,27,'!','"','£','$','%','^','&','*','(',')','_','+',KEYBACKSPACE,0,'Q','W','E','R','T','Y','U','I','O','P','[',']',KEYRETURN,0,'A','B','C','D','E','F','G','H','I','J','K','L',':','@','Z','X','C','V','B','N','M','<','>','/',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

//char scanwordcodes[32]={

char *key;    //pointer to the current key
char numcodewords=0;
char keyadd=0;
char keys[256];
char aliaskey[256];
char keycode,oldkeycode;
char codebufferctr=0;
char codebuffer[KBDbuffersize];
void (__interrupt __far *kbhbioshandler)();
char kbhinstalled=0;
char kbhkeydown=0;
char kbhhitonce=0;
UINT32 countdown=0;

char kbhhit(void)
{
  return kbhkeydown;
  
}

/*void delay(UINT16 count)
{
    UINT16 start=*timer+count;
    while (start>*timer);              
}*/
char kbhwaitkey()
{
    while (kbhkeydown==0);
    return kbhgetch();
}

char kbhgetch()
{
   if (kbhkeydown==0) return 0;
   if (kbhhitonce==1)
   {             
      kbhhitonce=0;      
      return keycode;
   }   
   else
   {      
      if (*timer>=countdown)
      {
         countdown=*timer+KBDrepeatwait;
         return keycode;
      }
      else
      {
         return 0;
      }
      
   }
   
}


void kbhclearkeys()
{
   short i;
   char *key=keys;
   for(i=0;i<256;i++)
   {
      *key=KFsinglehitok;
      key++;
   }
}



void __interrupt __far kbhhandler()
{
    oldkeycode=keycode;    
    keycode=inportb(0x60); 
    outportb(0x20,0x20); //acknoldege interrupt 
 
    if(keycode==0xE0)  //add 128 to keycode for next time around..
        keyadd=128;
    else
    {        
        if(keycode>127)  //if key is up, reset key to zero, set global flag to zero
        {  
            keycode&=127;
            keycode+=keyadd;
            keys[keycode]=KFsinglehitok;
            kbhkeydown=0;            
        }
        else
        {
            //if (oldkeycode!=keycode)            
            kbhkeydown=1;                           
            keycode+=keyadd;
            key=&keys[keycode];            

            if((*key & KFkeydown)==0)
            {  
                *key|=KFkeydown;       
                if(*key & KFsinglehitok)         //add to buffer
                {
                    kbhhitonce=1;
                    countdown=*timer+KBDfirstwait;
                    *key &=~KFsinglehitok; //not to be added to buffer again, till cleared
                    codebuffer[codebufferctr]=keycode;
                    codebufferctr++;
                    if(codebufferctr>=KBDbuffersize)
                        codebufferctr=0;
                }
            }
        }
        keyadd=0;
    }
    //kbhbioshandler();
}

void memset(void *dataptr,char data,UINT32 count);

  
#pragma aux memset=\
"mov ah,al"\
"mov bx,ax"\
"shl eax,16"\
"mov ax,bx"\
"rep stosb"\
parm [edi][al][ecx]\
modify [bx eax]


void kbhinstall()
{
    if(kbhinstalled)return;

    memset(keys,KFsinglehitok,256);
    memset(aliaskey,0,256);
    memset(codebuffer,0,KBDbuffersize);
    numcodewords=0;
    codebufferctr=0;
    kbhbioshandler=_dos_getvect(0x09);
    _dos_setvect(0x09,kbhhandler);
    kbhinstalled=1;
}


void kbhremove()
{ 
    if(kbhinstalled)
       _dos_setvect(0x09,kbhbioshandler);
    kbhinstalled=0;
}

char kbhctrlccheck()
{
    return ((keys[KEY_C]&KFkeydown)&&(keys[KEY_LEFTCTRL]&KFkeydown));     
}

char kbhbreakcheck()
{    
    long i0,i1;
    //long i;for(i=0;i<16;i++){printf("%d ",codebuffer[i]);}printf("\n");
    i0=codebufferctr-2;
    if(i0<0)i0=16+i0;
    i1=codebufferctr-1;
    if(i1<0)i1=16+i1;
    if((codebuffer[i0]==KEY_LEFTCTRL)&&(codebuffer[i1]==KEY_NUMLOCK))    
        if((keys[KEY_LEFTCTRL]&KFkeydown)==0) return 1;    
    return 0;
}

char kbhcheckkey(short keynum)
{
    if(keynum>255)
        keynum=aliaskey[keynum-256];
    if(keynum==KEY_BREAK)
       return kbhbreakcheck();
    return (key[keynum]&KFkeydown)!=0;
}


void kbhsinglehitprocess()
{
   short i;
   key=keys;
   char testmask=KFsinglekeyhit+KFkeydown;
   for(i=0;i<256;i++)
   {
       if((*key&testmask)!=0)          
           *key=0;
       key++;
   }
}

void kbhtogglesinglekeyhit(short keynum)
{
    if(keynum>255)    
       keynum=aliaskey[keynum-256];

    key[keynum]^=KFsinglekeyhit;
}

void kbhsetaliaskey(short alias,char keynum)
{
    aliaskey[alias-256]=keynum;
}






