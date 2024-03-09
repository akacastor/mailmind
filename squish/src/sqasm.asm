;***************************************************************************
;*                                                                         *
;*  Squish Developers Kit Source, Version 2.00                             *
;*  Copyright 1989-1994 by SCI Communications.  All rights reserved.       *
;*                                                                         *
;*  USE OF THIS FILE IS SUBJECT TO THE RESTRICTIONS CONTAINED IN THE       *
;*  SQUISH DEVELOPERS KIT LICENSING AGREEMENT IN SQDEV.PRN.  IF YOU DO NOT *
;*  FIND THE TEXT OF THIS AGREEMENT IN THE AFOREMENTIONED FILE, OR IF YOU  *
;*  DO NOT HAVE THIS FILE, YOU SHOULD IMMEDIATELY CONTACT THE AUTHOR AT    *
;*  ONE OF THE ADDRESSES LISTED BELOW.  IN NO EVENT SHOULD YOU PROCEED TO  *
;*  USE THIS FILE WITHOUT HAVING ACCEPTED THE TERMS OF THE SQUISH          *
;*  DEVELOPERS KIT LICENSING AGREEMENT, OR SUCH OTHER AGREEMENT AS YOU ARE *
;*  ABLE TO REACH WITH THE AUTHOR.                                         *
;*                                                                         *
;*  You can contact the author at one of the address listed below:         *
;*                                                                         *
;*  Scott Dudley       FidoNet     1:249/106                               *
;*  777 Downing St.    Internet    sjd@f106.n249.z1.fidonet.org            *
;*  Kingston, Ont.     CompuServe  >INTERNET:sjd@f106.n249.z1.fidonet.org  *
;*  Canada  K7M 5N3    BBS         1-613-634-3058, V.32bis                 *
;*                                                                         *
;***************************************************************************
;
; $Id: SQASM.ASM 1.2 1994/02/06 05:55:47 sjd Exp sjd $

IFNDEF OS_2

IFNDEF __FLAT__

        .model large, pascal
        .code

        public FARWRITE, FARREAD

; int far pascal farread(int handle,char far *buf,unsigned int len)
; int far pascal farwrite(int handle,char far *buf,unsigned int len)
; int far pascal shareloaded(void);

FARREAD proc uses ds, handle:word, buf:dword, len:word
        mov     bx,handle               ; Load file handle
        mov     cx,len                  ; Load length

        mov     ax,word ptr [buf+2]     ; Load segment into AX (and then DS)
        mov     dx,word ptr buf         ; Load offset into DX

        mov     ds,ax                   ; Move AX (the segment) into DS

        mov     ah,3fh                  ; Do it
        int     21h
        jnc     okay

        mov     ax, -1
okay:

        ret
FARREAD endp

FARWRITE proc uses ds, handle:word, buf:dword, len:word
        mov     bx,handle               ; Load file handle
        mov     cx,len                  ; Length of write

        mov     ax,word ptr [buf+2]     ; Load segment into DS

        mov     dx,word ptr buf         ; Load offset into DX

        mov     ds,ax                   ; Move AX (the segment) into DS

        mov     ah,40h                  ; Call it
        int     21h
        jnc     doneit                  ; Return # of bytes if no error

        mov     ax,-1                   ; Otherwise, return -1

doneit:
        ret
FARWRITE endp

ELSE            ; __FLAT__

        .386p
        .model small, pascal
        .code

        public FARWRITE, FARREAD

; int far pascal farread(int handle,char far *buf,unsigned int len)
; int far pascal farwrite(int handle,char far *buf,unsigned int len)
; int far pascal shareloaded(void);

FARREAD proc
        mov     ecx,[esp+4]             ; Load length

        mov     edx, [esp+8]            ; Load offset
        mov     ebx, [esp+12]           ; Load file handle

        mov     ah,3fh                  ; Do it
        int     21h
        jnc     okay

        mov     eax, -1
okay:   ret 0ch
FARREAD endp

FARWRITE proc
        mov     ebx,[esp+12]            ; Load file handle
        mov     ecx,[esp+4]             ; Length of write

        mov     edx,[esp+8]             ; Load offset into DX

        mov     ah,40h                  ; Call it
        int     21h
        jnc     doneit                  ; Return # of bytes if no error

        mov     eax,-1                  ; Otherwise, return -1

doneit: ret 0ch
FARWRITE endp


ENDIF ; FLAT

ENDIF ; !OS_2

end

