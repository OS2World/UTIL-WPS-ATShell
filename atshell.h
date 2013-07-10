#define INCL_DOSERRORS
#define INCL_DOSMISC
#define INCL_DOSMODULEMGR
#define INCL_DOSRESOURCES
#define INCL_DOSSESMGR
#define INCL_GPI
#define INCL_WIN
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "atshlres.h"

// ----------------------------------------------------------------------------
// CONSTANTS

#define RESOURCES               "atshlres.dll"
#define PROFILE                 "ATSHELL.INI"
#define HELP_FILE               "atshell.hlp"

#define OS2_SHELL               "PMSHELL.EXE"  // default shell if none defined

#define PRF_DEFAULT_PROGRAM     "DefaultProgram"
#define PRF_STARTUP_PROGRAM     "StartupProgram"
#define PRF_KEY_PROGRAM         "Executable"
#define PRF_KEY_PARAMS          "Parameters"
#define PRF_KEY_DIR             "Directory"
#define PRF_KEY_ENV             "Environment"

#define STRING_RES_LIMIT        255   // max. length of a string resource
#define PARAMETER_LIMIT         255   // max. length of a program parameters string
#define ENVIRONMENT_LIMIT       255   // max. length of a single environment variable


// ----------------------------------------------------------------------------
// MACROS

#define ErrorPopup( text ) \
    WinMessageBox( HWND_DESKTOP, HWND_DESKTOP, text, "Error", 0, MB_OK | MB_ERROR )


// ----------------------------------------------------------------------------
// TYPEDEFS

typedef struct _Global_Data {
    HAB     hab;                            // anchor-block handle
    HMQ     hmq;                            // main message queue
    HMODULE hResources;                     // handle to resource dll
    HINI    hIni;                           // handle to INI file
    BOOL    fRunSilent,                     // run silently?
            fRunSecure;                     // run securely (restricted GUI)?
    CHAR    szProgram[ CCHMAXPATH + 1 ],    // default program filespec
            szParams[ PARAMETER_LIMIT + 1], // default program parameters
            szDir[ CCHMAXPATH + 1 ],        // default program startup directory
            szEnv[ ENVIRONMENT_LIMIT + 1 ], // default program environment variables
            szSProgram[ CCHMAXPATH + 1 ],   // startup program filespec
            szSParams[ PARAMETER_LIMIT + 1],// startup program parameters
            szSDir[ CCHMAXPATH + 1 ],       // startup program startup directory
            szSEnv[ ENVIRONMENT_LIMIT + 1 ];// startup program environment variables
} ATSHGLOBAL, *PATSHGLOBAL;

// window data for configuration dialog
typedef struct _AppConfig_Data {
    HAB     hab;
    HMODULE hResources;
    BOOL    fChanged;                       // indicates if dialog options were changed
    CHAR    szProgram[ CCHMAXPATH + 1 ],
            szParams[ PARAMETER_LIMIT + 1],
            szDir[ CCHMAXPATH + 1 ],
            szEnv[ ENVIRONMENT_LIMIT + 1 ];
} ATSHAPPCONFIG, *PATSHAPPCONFIG;


// ----------------------------------------------------------------------------
// FUNCTIONS

MRESULT EXPENTRY ClientWndProc( HWND, ULONG, MPARAM, MPARAM );
void             LocateProfile( PSZ pszProfile );
void             LoadProfileSetting( PSZ pszData, HINI hIni, PSZ pszApp, PSZ pszKey );
void             CentreWindow( HWND hwnd );
void             SetButtonIcon( HWND hwnd, PSZ pszExecutable, HMODULE hResources );
void             LaunchPrompt( HWND hwnd );
void             LaunchProgram( HWND hwnd, PSZ pszProgram, PSZ pszParams, PSZ pszDir, PSZ pszEnv, BOOL fHide );
void             DlgConfigure( HWND hwnd, USHORT usProgram );
MRESULT EXPENTRY AppCfgDlgProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );
void             BrowseProgram( HWND hwnd, HAB hab, HMODULE hResources );
MRESULT EXPENTRY AboutDlgProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );

