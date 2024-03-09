#
#  Makefile Mailmind compilation with OpenWatcom
#

#which memory model and processor to use by default
PROCESSOR =     # for the 16-bit compilers

# name the compiler
CC = wcc$(PROCESSOR)
FC = wfc$(PROCESSOR)
ASM = wasm

MODEL = l       # large

# options to use
INC =  -i=..\squish\h -i=..\max
CFLAGS = -m$(MODEL) -zq -wx $(INC)
AFLAGS = -zq
#LNKOPT = option quiet

.EXTENSIONS:
.EXTENSIONS: .exe .rex .lib .obj .wbj .asm .c .for

.BEFORE
        @set INCLUDE=.;$(%watcom)\h;$(%watcom)\h\win
        @set DOS4G=QUIET

exes = mailmind.exe


# explicit rules

all :   most .SYMBOLIC
        @%null

most :  $(exes) .SYMBOLIC
        @%null

mailmind.exe : mailmind.obj
        wlink $(LNKOPT) option map option stack=8192 name mailmind file mailmind lib squish/lib/wc_dos_l.lib


run_all:    .SYMBOLIC


clean : .SYMBOLIC
        if exist *.obj call rm *.obj
        if exist *.wbj call rm *.wbj
        if exist *.rex call rm *.rex
        if exist *.exe call rm *.exe
        if exist *.exp call rm *.exp

# implicit rules

.c.obj :
        $(CC) $^& $(CFLAGS)

.c.wbj :
        $(CC) $^& $(CFLAGS) -zw -fo=$^&.wbj

.asm.obj :
        $(ASM) $(AFLAGS) $^&

