# Makefile for building ATSHELL with IBM C/C++ (VisualAge) & NMAKE32.
#
# To compile other languages, set the 'NLV' environment variable to the
# appropriate country code before running NMAKE32.  A directory with that
# name must exist, and must have the correct language files.
#

CC     = icc.exe
RC     = rc.exe
LINK   = ilink.exe
IPFC   = ipfc.exe
CFLAGS = /Gm+ /Sp /Ss+ /Q
RFLAGS = -x2 -n
LFLAGS = /NOE /PMTYPE:PM /NOLOGO
NAME   = atshell
OBJS   = $(NAME).obj
MRI    = atshlres

!ifndef NLV
    NLV = 001
!endif

!ifdef DEBUG
    CFLAGS   = $(CFLAGS) /Ti+ /Tm+ /Wuse
    LFLAGS   = $(LFLAGS) /DEBUG
!endif

all         : $(NAME).exe $(MRI).dll $(NAME).hlp

$(NAME).exe : $(OBJS) $(NAME).h $(MRI).h $(NAME).res
                $(LINK) $(LFLAGS) $(OBJS) $(LIBS)
                $(RC) $(RFLAGS) $(NAME).res $@

$(NAME).res : $(NAME).rc
                $(RC) -r $(NAME).rc $@

$(MRI).dll  : $(MRI).obj $(MRI).res
                $(LINK) $(LFLAGS) /DLL $< /OUT:$@
                $(RC) $(RFLAGS) $(MRI).res $@

$(MRI).obj  : $(MRI).c $(MRI).def
                $(CC) $(CFLAGS) /Ge- $**

$(MRI).res  : {$(NLV)}$(MRI).rc {$(NLV)}$(MRI).dlg $(MRI).h
                %cd $(NLV)
                $(RC) -i .. -r $(MRI).rc ..\$@
                %cd ..

$(NAME).hlp : {$(NLV)}$(NAME).ipf
                $(IPFC) -d:$(NLV) $< $@

nlvclean    :
                del $(MRI).obj $(MRI).dll $(MRI).res $(NAME).hlp >NUL

clean       :
                del $(OBJS) $(MRI).obj $(MRI).dll $(MRI).res $(NAME).exe $(NAME).hlp $(NAME).res >NUL

