#########################################################################
# Open Watcom 1.6 and above
# Makefile for atshell.exe
#
# Michael Greene <greenemk@cox.net>
# November 2006
#
########################################

# uncomment for debug version
#DEBUG = 1

# Machine type see ow docs
MACHINE= -6s

#Optimization Fastest possible -otexan
OPT = -otexan

# name to use with final exe
NAME = atshell

CC = wcc386
LD = wlink
RC = rc

INCLUDE = .\;$(%watcom)\h;$(%watcom)\h\os2

!ifdef DEBUG
CFLAGS  = -i=$(INCLUDE) -za99 -wx -od -DDEBUG $(MACHINE) -bm -bt=OS2 -mf
LDFLAGS = d all op map,symf
!else
CFLAGS  = -i=$(INCLUDE) -za99 -d0 -w4 -zq $(OPT) $(MACHINE) -bm -bt=OS2 -mf
LDFLAGS = op el
!endif

# pick up on recurse
!ifdef %NLV
NLV = $(%NLV)
!endif

# default
!ifndef NLV
NLV = 001
!endif

all: $(NAME).exe

# if the lang (ie - en de passed then set env and recurse wmake

en: .SYMBOLIC
  set NLV=001
  wmake -f makefile.wat

de: .SYMBOLIC
  set NLV=049
  wmake -f makefile.wat

# end lang section

$(NAME).exe: atshlres.dll atshell.hlp
  $(CC) $(CFLAGS) atshell.c
  $(LD) NAME $(NAME) SYS os2v2_pm $(LDFLAGS) FILE atshell.obj

atshlres.dll: atshlres.res
  $(CC) $(CFLAGS) -bd atshlres.c
  $(LD) NAME atshlres $(LDFLAGS) @atshlres.lnk FILE atshlres.obj
  $(RC) -x2 atshlres.res atshlres.dll

atshlres.res: .SYMBOLIC
  -@cd .\$(NLV)
  $(RC) -r -i .. atshlres.rc ..\atshlres.res
  -@cd ..

atshell.hlp: .SYMBOLIC
  ipfc -d:$(NLV) .\$(NLV)\atshell.ipf .\atshell.hlp

# clean the whole mess
clean : .SYMBOLIC
  -@rm *.exe
  -@rm *.dll
  -@rm *.hlp
  @%make cleanrel

# clean all except exe and dll for release
cleanrel : .PROCEDURE
  -@rm *.obj
  -@rm *.err
  -@rm *.lst
  -@rm *.sym
  -@rm *.map
  -@rm *.res

