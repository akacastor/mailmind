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
; SHARE.ASM - function call to determine whether or not SHARE.EXE
; is loaded (DOS only)

IFNDEF OS_2

IFNDEF __FLAT__
        .model large, pascal
        .code

        public SHARELOADED

SHARELOADED proc

        mov     ax,1000h                ; Check for SHARE.EXE installation
        int     2fh                     ; DOS multiplexer interrupt
        cmp     al,0ffh                 ; ffh = SHARE loaded
        je      GetOut

        xor     ax,ax
GetOut: ret

SHARELOADED endp

ELSE            ; __FLAT__
        .386p
        .model small, pascal
        .code
        public SHARELOADED

SHARELOADED proc
        mov     eax,1000h               ; Check for SHARE.EXE installation
        int     2fh                     ; DOS multiplexer interrupt
        cmp     al,0ffh                 ; ffh = SHARE loaded
        je      GetOut

        xor     eax,eax
GetOut: ret

SHARELOADED endp

ENDIF ; __FLAT__

ENDIF ; !OS_2

end


