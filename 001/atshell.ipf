:userdoc.

.* ****************************************************************************
:h1 x=left y=bottom width=100% height=100% res=001 id=intro.The AT Desktop Shell
:p.The AT Desktop Shell (ATSHELL) is a simple RUNWORKPLACE replacement.  It
allows an arbitrary executable to be run as the desktop program (in place of the
Workplace Shell).  When this program exits, the ATSHELL dialog (re)appears -
allowing you to restart the desktop program, configure a new program, open a
command prompt, or shut down the system.

:p.An example of where this might be useful, for instance, is in setting up a
'kiosk' type system to run just a web browser instead of a full desktop.

:p.You can configure two different programs with ATSHELL&colon.
:dl break=all.
:dt.The desktop program
:dd.This is the program that gets run as the 'desktop'.  The default is
PMSHELL.EXE, which runs the Workplace Shell.
:dt.A startup program (optional)
:dd.You can also configure a program to run on startup, before the ATSHELL
dialog first comes up.  Think of this as an alternative STARTUP.CMD, except
that it runs synchronously to the desktop initialization.  In other words, the
ATSHELL dialog does not appear (and the desktop program does not get loaded)
until this startup program returns, which is a potential advantage under some
circumstances.
:edl.

:p.To configure either of these programs, right-click on an empty part of the
ATSHELL dialog, and select the appropriate action from the popup menu.  Changes
to the desktop program take effect immediately; changes to the startup program
will take effect the next time ATSHELL is started.

:p.To start the desktop program, select the large button on the left.  The two
smaller buttons allow you to open a windowed command prompt, and shut down the
system, respectively.


.* ****************************************************************************
:h1 x=left y=bottom width=100% height=100% res=010.Using the AT Desktop Shell
:p.This is main window of the AT Desktop Shell.  From here you can launch the
desktop program or access various tasks with the popup menu or the action
buttons.

:p.For a description of what the AT Desktop Shell does, see
:link reftype=hd refid=intro.here:elink..

.* ----------------------------------------------------------------------------
:h2 x=left y=bottom width=100% height=100% res=011.The program configuration dialog
:p.This dialog allows you to configure the program to be run.

:dl break=all.
:dt.Path and file name
:dd.Enter the fully-qualified name of the program (or use the Browse button to
locate it).  This may be a binary executable or a command file.
:dt.Parameters
:dd.Optional.  Enter any parameters you wish to have passed to the program.
:dt.Working directory
:dd.Optional.  Enter the working directory of the program.  The program will
execute with this as the 'current' directory.
:edl.

:euserdoc.

