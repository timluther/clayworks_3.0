#include <dos.h>
#include <stdio.h>
#include <conio.h>

#include "ttypes.hpp"


typedef struct keystruct
{
    union {
        UINT16 s;
        UINT8 a[2];
    };
};

UINT8 keyflag = 0;        // 0 - standard  // 1 - extended        
UINT8 keys_pressed = 0;   // Number keys pressed
keystruct scancode;        // 0/1 - standard key/extended key
keystruct oldscancode;        // 0/1 - standard key/extended key
UINT8 keyup=1;
UINT8 keychanged=1;
UINT8 keybits;

#define KEYBOARD_INTERRUPT  0x09  // the keyboard interrupt number
#define KEY_BUFFER          0x60  // the port of the keyboard buffer
#define KEY_CONTROL         0x61  // the port of the keyboard controller
#define PIC_PORT            0x20  // the port of the programmable
                                  // interrupt controller (PIC)
#define KB_SUCCESS                              1
#define KB_ERROR                                0

// new interrupt handler
void __interrupt interupt_handler(void);
// pointer to original interrupt handler
void (_interrupt _far *orig_function)();

void __interrupt interupt_handler(void)
{     
     _enable();
     oldscancode=scancode;     
     scancode.a[keyflag] = inp(KEY_BUFFER);
     keyup=(scancode.a[0]&128)==128;          
     keyflag = (scancode.a[0] == 224);
     scancode.a[0] &= 127;
     keychanged=(scancode.s!=oldscancode.s);
     
     keybits = inp(KEY_CONTROL) | 0x82;
     outp(KEY_CONTROL, keybits);
     outp(KEY_CONTROL, (keybits & 0x7f));
     outp(PIC_PORT, 0x20);                
}

UINT8 tkbhit()
{
    return !keyup;
}

char tgetch()
{
    UINT8 oldkeyup=keyup;
    while (keyup==oldkeyup);
    return scancode.a[0];
}


void main()
{
    orig_function = _dos_getvect(9);
    // Install the new interrupt handler
    _dos_setvect(9, interupt_handler);
    printf("hi");
    char ch;
    while (scancode.a[0]!=1)
    {
        //if ((!keyup)&&(keychanged))
        ch=tgetch();
        printf("%c\n",ch);
        //printf("scancode :%i %i %i\n",scancode.a[0],scancode.a[1],keybits);
    }
    _dos_setvect(9, orig_function);
}
