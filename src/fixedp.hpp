#ifndef _fixedpointmacros_
#define _fixedpointmacros_

extern INT32 imul(INT32,INT32);
#pragma aux imul=\
"imul edx"\
"shrd eax,edx,16"\
parm [eax][edx]


extern INT32 idiv(INT32,INT32);
#pragma aux idiv=\
"mov edx,eax"\
"sar edx,16"\
"shl eax,16"\
"idiv ebx"\
parm [eax][ebx]\
modify[edx]


extern INT32 idivs(INT32,INT32);
#pragma aux idivs=\
"cdq"\
"idiv ebx"\
parm [eax][ebx]\
modify[edx]

inline INT32 fdiv(INT32 a,INT32 b)
{
    return (a/b);    
}

/*   d= fmul(v[0].x - v[2].x,diffy1_y2) - fmul(v[1].x - v[2].x,diffy0_y2);

extern INT32 getd(INT32,INT32,INT32,INT32);
#pragma aux imul=\
"imul ebx"\
"shr eax,16"\
"sal edx,16"\
"or eax,edx"\
parm [eax][ebx]\
modify[edx]*/



extern INT32 imuls(INT32,INT32);
#pragma aux imuls=\
"imul edx"\
parm [eax][edx]


extern INT32 satadd(INT32,INT32);
#pragma aux satadd=\
"add al,dl"\
"sbb cl,cl"\
"or al,cl"\
parm [eax][edx]\
modify [ecx]

extern INT32 satsub(INT32,INT32);
#pragma aux satsub=\
"sub al,dl"\
"mov cl,255"\
"adc cl,0"\
"and al,cl"\
parm [eax][edx]\
modify [ecx]



/*inline INT32 fmul(INT32 a,INT32 b)
{
    return (a*b)>>16;    
}*/




extern INT32 iceil(INT32);
#pragma aux iceil=\
"add eax,0xFFFF"\
"shr eax,16"\
parm [eax]
      


extern INT32 iwholeint(INT32);
#pragma aux iwholeint=\
"add eax,0xFFFF"\
"and eax,0xFFFF0000"\
parm [eax]

//Works out the amount of error between the whole number part
//and the fractional number
extern INT32 iinterror(INT32);
#pragma aux iinterror=\
"mov ebx,eax"\
"add eax,0xFFFF"\
"and eax,0xFFFF0000"\
"sub eax,ebx"\
parm [eax]\
modify [ebx]



#endif
