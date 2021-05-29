//Alphanumeric character set handling code. T.Lewis 1996

#include <string.h>
#include <stdio.h>
#include "ttypes.hpp"
#include "gbasics.hpp"


//cattrib flags

UINT16 FNTgetcharsize(tfontchar *ch)
{ 
  return (ch->height*ch->bytewidth);
}

UINT16 FNTgetcharstructsize(tfontchar *ch)
{
  return (sizeof(tfontchar)+(ch->height*ch->bytewidth)-1);
}

tfontchar *FNTcreatechar(UINT8 h,UINT8 w)
{
    tfontchar *ch;
   
    ch=new tfontchar[h*(w>>3)];
    ch->height=h;
    ch->width=w;
    ch->bytewidth=w >> 3;
    return ch;
}



UINT32 FNTgetwidth(char *txt)
{
  if (!txt) return 0;
  UINT32 totalwidth=0;
  if ((t_charset.cattrib & CTproportional)==0)
    return t_charset.xsize*strlen(txt);
    
  while (*txt!=0)    
    totalwidth+=t_charset.offsets[*(txt++)].width;

  
  
  return totalwidth;
}

UINT32 FNTgetwidthlength(char *txt,UINT32 length)
{  
  if ((t_charset.cattrib & CTproportional)==0)  
    return t_charset.xsize*MIN(strlen(txt),length);
    
  UINT32 totalwidth=0;    
  while ((*txt!=0)&&(length>0))
  {
    totalwidth+=t_charset.offsets[*txt].width;
    txt++;
    length--;
  }
  return totalwidth;
}

UINT32 FNTgetheight(char *txt)
{
  UINT32 totalheight=0,currmaxheight=0;
  while (txt!=NULL)
  {
    if (currmaxheight<t_charset.offsets[*txt].height)
      currmaxheight=t_charset.offsets[*txt].height;
    if ((txt==NULL)||(*txt==returnkey))
    totalheight+=currmaxheight;
    txt++;
  }
  return totalheight;
}

INT16 FNTgetindexfrompos(char *txt,int x)
{
  INT16 currlen=0;
  UINT8 count=0;
  while (x>currlen)
  {
    currlen+=t_charset.offsets[*txt].width;   
    txt++;
    count++;
    if (*txt==0) return count;
  }
  return count; 
}

INT16 FNTgetindexbeforepos(char *txt,int x)
{
  INT16 currlen=0;
  UINT8 count=0;
  while (x>currlen)
  {
    currlen+=t_charset.offsets[*txt].width;   
    txt++;
    count++;
    if (*txt==0) return count;
  }
  return count; 
}


void FNTproportionalize(tfont *ch)
{
  char *currbyte;  
  charStruct *currch;
  char bytelop,bitlop,currbit;
  UINT32 i;

  currch=ch->offsets;
  ch->cattrib=CTproportional;
  for (i=0;i<255;i++)
  {
    currch->width=0;
    currbyte=currch->data; //currch->heights
    for (bytelop=0;bytelop<16;bytelop++)
    {
    
      currbit=0;
      bitlop=7;
      while (bitlop>0)
      {            
        if ((*currbyte & (1 << bitlop))!=0)
          currbit=bitlop^7;
        bitlop--;
      }
      if (currbit>currch->width)
          currch->width=currbit;

      currbyte++;
    }
    currch->width+=2;
    currch++;
  }  
  ch->offsets[32].width=6;
    
}

tfont *FNTload(char *filename, tfont *chlist)
{
  if (!chlist) return NULL;
  FILE *f;
  char height;
  UINT16 i;
  if (!(f=fopen(filename,"rb")))return NULL;
  height=fgetc(f);
  //fgets(&height,1,f);
  //fseek(f,1);
  height=16;
  chlist->ysize=height;
  chlist->yextsize=height-3;
  chlist->xbytesize=1;
  chlist->xsize=8;
    
  for (i=0;i<256;i++)
  {
      //load data into buffer and proportionalize here   
      //ch=chlist->offsets[i]=createchar(8,height);
      chlist->offsets[i].width=8;
      chlist->offsets[i].bytewidth=1;
      chlist->offsets[i].height=height;
      fread(chlist->offsets[i].data,height,1,f);
      //fgets(chlist->offsets[i].data,height,f);
  }

  //blockread(f,ch.typeface,256*scans,norecs);
  FNTproportionalize(chlist);
  fclose(f);
  return chlist;
}
/*
Procedure savefontlist(filename:string;var ch:charsettype);
Var scans:byte;norecs:word;
    f:file;
Begin
  norecs:=1;
  assign(f,filename);
  Rewrite(f,norecs);
  scans:=Fontheight;
  blockwrite(f,scans,1,norecs);
  blockwrite(f,ch.typeface,256*Fontheight,norecs);
  close(f);
end;

Procedure proportionalize(var ch:charsettype);
var
currwidth,currbyte:byteptr;
fontlop,bytelop,bitlop,currbit:byte;
begin
  currwidth:=@ch.widths;
  ch.attrib:=ctproportional;
  for fontlop:=0 to 255 do begin

    currwidth^:=0;
    currbyte:=@ch.typeface[fontlop];
    for bytelop:=0 to fontmax do begin
      currbit:=0;
      for bitlop:=7 downto 0 do begin
        if currbyte^ and (1 shl bitlop)<>0 then
          currbit:=bitlop xor 7;
      end;
      if currbit>currwidth^ then
        currwidth^:=currbit;
      inc(currbyte);
    end;
    inc(currwidth^,2);
    inc(currwidth);
    
  end;
  ch.widths[32]:=6;
end;



{Procedure loadfontlistb(filename:string;var ch:charsettype);
Var lop,lop2,scans:byte;f:text;temp:char;tempstr:string;code:integer;
Begin
  Assign(f,filename);
  Reset(f);
  readln(f,tempstr);
  tempstr:=copy(tempstr,1,9);
  val(tempstr,scans,code);
  for lop:=0 to 255 do
    for lop2:=1 to Fontheight do
    begin
      read(f,temp);ch^[lop][lop2]:=ord(temp);
    end;
  close(f);
end;

{the above is the old byte by byte method
below is the dynamicly sized array saver}

{Procedure loadfontlistc(filename:string;var ch:charsettype);
Var scans,lop:byte;f:file;norecs:word;
Begin
  norecs:=1;
  Assign(f,filename);
  Reset(f,norecs);
  blockread(f,scans,1,norecs);
  for lop:=0 to 255 do
    blockread(f,ch^[lop],scans,norecs);
  close(f);
end;}

procedure rorblock(block:pointer;count:word;shiftby:byte);assembler;
asm
  les di,block
  mov cl,shiftby
  mov bx,count

  dec bx
  add di,bx

  {do first byte, save in dx}
  mov dl,es:[di]
  xor dh,dh
  ror dx,cl
  mov es:[di],dl
  dec di
  cmp bx,0
  jz @lastbyte

  @shiftloop:
    mov al,es:[di]
    xor ah,ah
    ror ax,cl
    mov es:[di],al
    or es:[di+1],ah
    dec di
    dec bx
  jnz @shiftloop
  {do last byte}
  @lastbyte:
  or es:[di+1],dh
end;

procedure rolblock(block:pointer;count:word;shiftby:byte);assembler;
asm
  les di,block
  mov cl,shiftby
  mov bx,count
  dec bx
  {do first byte, save in dx}
  mov dh,es:[di]
  xor dl,dl
  rol dx,cl
  mov es:[di],dh
  inc di
  cmp bx,0
  jz @lastbyte

  @shiftloop:
    mov ah,es:[di]
    xor al,al
    rol ax,cl
    mov es:[di],ah
    or es:[di-1],al
    inc di
    dec bx
  jnz @shiftloop
  {do last byte}
  @lastbyte:
  or es:[di-1],dl
end;

procedure rol_font(font:bytearray;fontwidth,fontheight:word);
var lop:byte;
begin
  for lop:=0 to fontheight-1 do
    rolblock(@font^[lop*(fontwidth shr 3)],fontwidth shr 3,1);
end;

procedure rou_font(font:bytearray;fontwidth,fontheight:word);
var lop:word;
    xbytes:byte;
    temp:^byte;

begin
  getmem(temp,xbytes);
  xbytes:=fontwidth shr 3;
  move(font^[0],temp^,xbytes);
  for lop:=0 to fontheight-2 do
    move(font^[(lop+1)*xbytes],font^[lop*xbytes],xbytes);
  move(temp^,font^[(fontheight-1)*xbytes],xbytes);

end;

procedure ror_font(font:bytearray;fontwidth,fontheight:word);
var lop:byte;
begin
  for lop:=0 to fontheight-1 do
    rorblock(@font^[lop*(fontwidth shr 3)],fontwidth shr 3,1);
end;

procedure rod_font(font:bytearray;fontwidth,fontheight:word);
var lop:word;
    xbytes:byte;
    temp:^byte;

begin
  xbytes:=fontwidth shr 3;
  getmem(temp,xbytes);

  move(font^[(fontheight-1)*xbytes],temp^,xbytes);
  for lop:=fontheight-1 downto 1  do
    move(font^[(lop-1)*xbytes],font^[(lop)*xbytes],xbytes);
  move(temp^,font^[0],xbytes);
  freemem(temp,xbytes);
  

end;
{assembler;
asm
  les di,font

  mov cx,Fontheight
  dec cx
  mov ah,es:[di]
  @shiftlop:
    mov al,es:[di+1]
    mov es:[di],al
    inc di
  loop @shiftlop
  mov es:[di],ah
end;}

procedure not_font(font:bytearray;fontsize:word);assembler;
asm
  les di,font
  mov cx,Fontsize
  @notlop:
    mov al,es:[di]
    not al
    mov es:[di],al
    inc di
  loop @notlop
end;

procedure flipbyte(var c:byte);assembler;
asm
  les di,c
  mov al,es:[di]
  mov cx,8
  @bitlop:
    rcr al,1
    adc ah,0
    cmp cl,1
    jz @noshift
      shl ah,1
    @noshift:
  loop @bitlop
  mov es:[di],ah
end;

procedure flipx_font(font:bytearray;fontwidth,fontheight:word);
var xlop,ylop,xbytes,byte1,byte2:word;
begin
  xbytes:=(fontwidth shr 3);
  for ylop:=0 to xbytes*fontheight do
    flipbyte(font^[ylop]);
  if xbytes>1 then
  for ylop:=0 to fontheight-1 do
    for xlop:=0 to (xbytes-1) shr 1 do
      fswap(font^[(ylop*xbytes)+xlop],font^[(ylop*xbytes)+xbytes-1-xlop],1);
end;

procedure flipy_font(font:bytearray;fontwidth,fontheight:word);
var lop,xbytes:byte;
begin
  xbytes:=fontwidth shr 3;
  for lop:=0 to (Fontheight-1) shr 1 do
    fswap(font^[lop*xbytes],font^[(fontheight-1-lop)*xbytes],xbytes);
end;
End.
*/
