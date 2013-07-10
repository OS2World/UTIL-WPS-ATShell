AT DESKTOP SHELL - README.TXT
| V0.5  2006-12-09

  The AT Desktop Shell (ATSHELL) is a simple RUNWORKPLACE replacement.  It
  allows an arbitrary executable to be run as the desktop program (in place
  of the Workplace Shell).  When this program exits, the ATSHELL dialog 
  (re)appears - allowing you to restart the desktop program, configure a 
  new program, open a command prompt, or shut down the system.

  An example of where this might be useful, for instance, is in setting up a
  'kiosk' type system to run just a web browser instead of a full desktop. 


DEFINITIONS

  ATSHELL allows you to configure two programs:

  The 'desktop program' refers to the program that gets run as the 'desktop'.  
  The default is PMSHELL.EXE, which runs the Workplace Shell.

  The 'startup program', which is optional, is a program that runs on startup,
  before the ATSHELL dialog first comes up.  This is conceptually like a
  second STARTUP.CMD; unlike STARTUP.CMD, however this has one potential
  advantage: ATSHELL waits until this program returns before doing anything
  (even before displaying its own dialog).  This could be useful if, for
  example, you wanted some security-critical program to finish running before
  the user has a chance to interact with the system.

  Both of these programs are configured via ATSHELL's right-click popup menu.
  (Therefore, you have to run ATSHELL once before the startup program can be
  changed from the default, unless you edit the INI file directly.)


INSTALLATION

  To install ATSHELL, place ATSHELL.EXE onto your PATH, ATSHLRES.DLL onto your
  LIBPATH, and ATSHELL.HLP onto your HELP path.  

  To use ATSHELL, you can edit CONFIG.SYS and replace the SET RUNWORKPLACE line
  with:
      SET RUNWORKPLACE=x:\path\ATSHELL.EXE
  where x:\path is wherever you placed ATSHELL.EXE.

| Alternatively, if you are using Security/2 with local logon (i.e. with 
| RUNWORKPLACE=SHPM2.EXE), you can set ATSHELL.EXE as a user's local shell in
| the passwd file.  


OPTIONS

  ATSHELL supports the following optional command-line parameters:

    /Q   This causes the desktop shell to be launched immediately when ATSHELL 
         first starts up, without displaying the ATSHELL dialog.  (The ATSHELL 
         dialog will appear, however, when/if you close the desktop shell 
         program.)
 
    /S   This enables a simple 'secure' mode.  When ATSHELL is started with
         this parameter, the command prompt and configuration options are
         disabled and inaccessible.  USE THIS OPTION WITH CAUTION -- if you
         don't have an alternative method of booting (such as boot diskettes
         or a maintenance partition), you won't have any way of changing the 
         configuration (or even removing ATSHELL from CONFIG.SYS), unless the 
         desktop (or startup) program itself gives you access to a command 
|        prompt.  Alternatively, you can use this in conjunction with the 
|        multi-user local logon feature of Security/2, by setting 'atshell /s' 
|        as the local shell for non-privileged users, and keeping the WPS (or 
         something else) for the system administrator.


REMARKS
 
  If the configured program requires DLLs or other files from its own directory, 
  make sure to set the program's working directory as needed.  Otherwise, the
  program may not run properly (if at all).


KNOWN ISSUES

  If the shell program is PMSHELL.EXE (i.e. the WPS), and XWorkplace (or the 
| eComStation "extended workplace features") is installed, the XWorkplace 
| daemon will stay resident when PMSHELL exits.  With some versions (not the
| latest?), this has the unfortunate side effect of preventing ATSHELL's system 
| shutdown function from working.  If this happens, you will either have to shut
| down from within the WPS instead, or open a command prompt and kill the 
| XWPDAEMN.EXE process (using some appropriate tool).  
 
| For similar reasons, if the XWorkplace pager is enabled, it will remain on the
| screen after the WPS closes.  


HISTORY

| 0.5 (2006-12-09)
| . Main button now shows the icon of the desktop program, where possible.
| . Fixed error message for when help file cannot be loaded.
| . Improved error message for when resource library cannot be loaded.
| . If ATSHLRES.DLL fails to load, you now have the opportunity to shut down the
|   system (thereby avoiding an endless loop when running as RUNWORKPLACE).
| . Build files for OpenWatcom are now included.

  0.41 (2006-11-27)
  . Fixed stupid bug in the INI filename logic caused by the rewritten string
    handling.
  . Removed a leftover free() that wasn't doing anything anymore, and cleaned 
    up a bunch of other no-longer-used variables from the code.

  0.4  (2006-11-26)
  . Rewrote internal string handling to use static arrays (attempt to eliminate
    occasional memory problems).
  . More updates to window layout.
  . Added a couple of language strings that weren't being loaded from the 
    resource file.
  . The configuration dialog now automatically strips any trailing backslash 
    from the specified working directory.
 
  0.3  (2006-04-06, not publically released)
  . Clicking anywhere on the client window now brings up the context menu.
  . Various bugfixes.
  . Modified window layout.
  . Cleaned up build system to use one makefile for all languages.

  0.21  (2005-10-11)
  . Added some missing language strings to ATSHLRES.DLL and updated ATSHELL.EXE
    to load them.
  . Updated product information dialog with correct license information.
  . Added German translation (courtesy of Andreas Kohl).

  0.2  (2005-07-15)
  . ATSHELL.INI is now created in the same directory as OS2.INI (wherever the
    USER_PROFILE environment points to).
  . Added buttons for Help and Exit functions.
  . Added support for '/S' parameter.

  0.1  
  . First (private) release


TODO

  Allow the configuration of environment variables (including BEGIN/ENDLIBPATH)
  which get passed to the program.

| Allow a command prompt to be opened if the resource DLL could not be loaded.

| Open the file dialog in the last used program directory.

| Give a more meaningful error message if a program cannot be started.

| (MAYBE?) Offer an option (in configuration) to kill XWPDAEMN.EXE when the WPS
| terminates?  (Note: this would have side-effects within XWorkplace's startup 
| behaviour.)


NOTES

  Thanks go to the following people:
    * Jim Read, for help with the context menu logic.
    * Andreas Kohl, for providing the German translation.  
|   * Mike Greene and Andy Willis for providing the OpenWatcom build files.
|   * Marty Amodeo and Alessandro Cantatore for help with the button-icon logic.
    * Nicky Morrow, for early feedback and testing.  

| (C) 2006 Alex Taylor.  Licensed under the GNU GPL (v2); see LICENSE.TXT for 
| details.
