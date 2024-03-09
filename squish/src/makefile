#########################################################################
# Memory Model
#########################################################################

# This defines the memory model used for the 16-bit DOS versions of the
# MsgAPI.

# If you are using Microsoft C, this should be either
# "S", "M", "C" or "L", for the small, medium, compact, or large
# memory models, respectively.  Note that the model character
# must be uppercase when compiling with a Microsoft compiler.
#
# If you are using any other compiler, this should be either
# "s", "m", "c" or "l", again for the small, medium, compact
# or large memory models.  Note that the model character must
# be lowercase when using a non-Microsoft compiler.

MODEL = l

#########################################################################
# OS/2 Compilers
#########################################################################

#
# OS/2 - WATCOM C/16
#

CC              = wcc /dOS_2 /2 /j/zp1/oaxte100/zu/r/bd/ml/zq/w4/zp1/I=..\h
ASC             = masm /t /Mx /Zi /dOS_2
LD              = wlink @msgapi16
LIB             = wlib msgapi16.lib -+msgapi.dll
OBJTARGET       = msgapi.dll
FINALTARGET     = msgapi16.lib
COMPOBJ         = wc_misc.obj

#
# OS/2 - WATCOM C/32
#
#
#CC             = wcc386 /dOS_2 /j/zp1/4/fp3/onmaxte100/bd/mf/zq/w4/zp1/I=..\h
#ASC            = masm /t /Mx /Zi /dOS_2 /d__FLAT__
#LD             = wlink @msgapi32
#LIB            = wlib msgapi32.lib -+msgapi32.dll
#OBJTARGET      = msgapi32.dll
#FINALTARGET    = msgapi32.lib
#COMPOBJ        = w3_misc.obj

#
# OS/2 - Microsft C
#
#
#CC              = cl /nologo /DOS_2 /c /Oaxzr /W4 /Gs /Gr /Zi /ML /AL /I..\h
#ASC             = masm /t /Mx /Zi /dOS_2
#LD              = link @link.lst,msgapi.dll,nul,,msgapi.def
#LIB             = implib msgapi.lib msgapi.dll
#OBJTARGET       = msgapi.dll
#FINALTARGET     = msgapi16.lib
#COMPOBJ         = mc_misc.obj


#########################################################################
# DOS Compilers
#########################################################################

#
# DOS - Microsoft C
#
#
#CC              = cl /nologo /c /Oaxzr /W4 /Gs /Gr /Zi /A$(MODEL) /I..\h
#ASC             = masm /t /Mx /Zi
#LD              = lib msgapi.lib -+mc_misc @liball.lst
#LIB             = asdf
#OBJTARGET       = msgapi.lib
#FINALTARGET     = msgapi.lib
#COMPOBJ         = mc_misc.obj


#
# DOS - WATCOM C/16
#
#
#CC             = wcc /oals/s/j/zp1/m$(MODEL)/zq/w4/zp1/I=..\h
#ASC            = masm /t /Mx /Zi
#LD             = wlib msgapi.lib @lib1.lst @lib2.lst @lib3.lst @lib4.lst @lib5.lst @lib6.lst @lib7.lst -+wc_misc
#LIB            = asdf
#OBJTARGET      = msgapi.lib
#FINALTARGET    = msgapi.lib
#COMPOBJ        = wc_misc.obj


#
# DOS - WATCOM C/32
#
#
#CC             = wcc386 /j/zp1/4/fp3/onmaxte100/mf/zq/w4/zp1/I=..\h
#ASC            = masm /t /Mx /Zi /d__FLAT__
#LD             = wlib msgapi.lib @lib1.lst @lib2.lst @lib3.lst @lib4.lst @lib5.lst @lib6.lst @lib7.lst -+w3_misc
#LIB            = asdf
#OBJTARGET      = msgapi.lib
#FINALTARGET    = msgapi.lib
#COMPOBJ        = w3_misc.obj

#
# DOS - Turbo C
#
#CC             = tcc -c -d- -G- -p -v -I..\h -m$(MODEL)
#ASC            = tasm /t /Mx /Zi
#LD             = tlib msgapi.lib @lib1.lst @lib2.lst @lib3.lst @lib4.lst @lib5.lst @lib6.lst @lib7.lst -+tc_misc
#LIB            = asdf
#OBJTARGET      = msgapi.lib
#FINALTARGET    = msgapi.lib
#COMPOBJ        = tc_misc.obj

#
# DOS - Borland C
#
#CC             = bcc -c -d- -G- -p -v -I..\h -m$(MODEL)
#ASC            = tasm /t /Mx /Zi
#LD             = tlib msgapi.lib @lib1.lst @lib2.lst @lib3.lst @lib4.lst @lib5.lst @lib6.lst @lib7.lst -+bc_misc
#LIB            = asdf
#OBJTARGET      = msgapi.lib
#FINALTARGET    = msgapi.lib
#COMPOBJ        = bc_misc.obj


#########################################################################
# Object files in the MsgAPI build
#########################################################################

OBJS =  api_sdm.obj     bld.obj         msgapi.obj      sq_area.obj     \
        sq_help.obj     sq_idx.obj      sq_kill.obj     sq_lock.obj     \
        sq_misc.obj     sq_msg.obj      sq_read.obj     sq_uid.obj      \
        sq_write.obj    sqasm.obj

SLIB =  1stchar.obj     cvtdate.obj     date2bin.obj    dosdate.obj     \
        fexist.obj      ffind.obj       flush.obj       months.obj      \
        parsenn.obj     qksort.obj      stristr.obj     strocpy.obj     \
        trail.obj       _ctype.obj      nopen.obj       setfsize.obj    \
        tdelay.obj      strftim.obj     months.obj      ffinda.obj      \
        weekday.obj     share.obj       $(COMPOBJ)

#########################################################################
# Make commands
#########################################################################


all: $(FINALTARGET)
        -echo Done >all

$(OBJTARGET):   $(OBJS) $(SLIB)
        $(LD)

msgapi16.lib: msgapi.dll
        $(LIB)

msgapi32.lib: msgapi32.dll
        $(LIB)

.c.obj:
        $(CC) $<

.asm.obj:
        $(ASC) $<;

clean:
        -del *.obj
        -del *.lib
        -del *.dll
        -del *.map

