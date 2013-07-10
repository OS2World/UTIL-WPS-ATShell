/*
 *  ATSHELL - OS/2 RUNWORKPLACE replacement.
 *  Copyright (C) 2005 Alex Taylor
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "atshell.h"

/* ------------------------------------------------------------------------- *
 * Main program loop.                                                        *
 * ------------------------------------------------------------------------- */
int main( int argc, char *argv[] )
{
    HAB        hab;                       // anchor block handle
    HMQ        hmq;                       // message queue handle
    HWND       hwndFrame,                 // window handle
               hwndAccel,                 // acceleration table
               hwndHelp;                  // help instance
    QMSG       qmsg;                      // message queue
    BOOL       fInitFailure = FALSE;
    CHAR       szModule[ 16 ] = "",
               szIni[ CCHMAXPATH + 1 ],   // name of program INI file
               szRes[ STRING_RES_LIMIT + 1 ],
               szError[ STRING_RES_LIMIT + 16 ];
    ULONG      ulrc   = 0,
               ulErr  = 0;
    USHORT     i;
    HMODULE    hResources;                // resource module handle
    HELPINIT   helpInit;                  // help init structure
    ATSHGLOBAL global;


    hab = WinInitialize( 0 );
    if ( hab == NULLHANDLE ) {
        sprintf( szError, "WinInitialize() failed.");
        fInitFailure = TRUE;
    }

    if ( ! fInitFailure ) {
        hmq = WinCreateMsgQueue( hab, 0 );
        if ( hmq == NULLHANDLE ) {
            ulErr = WinGetLastError( hab );
            sprintf( szError, "Unable to create message queue:\n\nError severity:\t 0x%02X\nError code:\t 0x%02X",
                     ERRORIDSEV( ulErr ), ERRORIDERROR( ulErr )                                                    );
            fInitFailure = TRUE;
        }
    }

    if ( ! fInitFailure ) {
        ulrc = DosLoadModule( (PSZ) szModule, sizeof(szModule), RESOURCES, &hResources );
        if ( ulrc != NO_ERROR ) {
            sprintf( szError, "Failed to load resource library \"%s\".\n\nThe return code from DosLoadModule() was %d.\nThe name of the failing object is \"%s\".",
                     strupr(RESOURCES), ulrc, szModule );
            fInitFailure = TRUE;
        }
    }

    if ( ! fInitFailure ) {

        global.hab        = hab;
        global.hmq        = hmq;
        global.hResources = hResources;
        global.fRunSilent = FALSE;
        global.fRunSecure = FALSE;

        // Initialize the global data
        LocateProfile( szIni );
        global.hIni = PrfOpenProfile( hab, szIni );

        for ( i = 1; i < argc; i++ ) {
            if ( strnicmp( argv[ i ], "/q", 2 ) == 0 ) global.fRunSilent = TRUE;
            if ( strnicmp( argv[ i ], "/s", 2 ) == 0 ) global.fRunSecure = TRUE;
        }

        memset( global.szProgram,  0, sizeof(global.szProgram)  );
        memset( global.szParams,   0, sizeof(global.szParams)   );
        memset( global.szDir,      0, sizeof(global.szDir)      );
        memset( global.szEnv,      0, sizeof(global.szEnv)      );
        memset( global.szSProgram, 0, sizeof(global.szSProgram) );
        memset( global.szSProgram, 0, sizeof(global.szSParams)  );
        memset( global.szSProgram, 0, sizeof(global.szSDir)     );
        memset( global.szSProgram, 0, sizeof(global.szSEnv)     );

        // Read settings from the profile (INI) file
        LoadProfileSetting( global.szProgram,  global.hIni, PRF_DEFAULT_PROGRAM, PRF_KEY_PROGRAM );
        LoadProfileSetting( global.szParams,   global.hIni, PRF_DEFAULT_PROGRAM, PRF_KEY_PARAMS  );
        LoadProfileSetting( global.szDir,      global.hIni, PRF_DEFAULT_PROGRAM, PRF_KEY_DIR     );
        LoadProfileSetting( global.szEnv,      global.hIni, PRF_DEFAULT_PROGRAM, PRF_KEY_ENV     );
        LoadProfileSetting( global.szSProgram, global.hIni, PRF_STARTUP_PROGRAM, PRF_KEY_PROGRAM );
        LoadProfileSetting( global.szSParams,  global.hIni, PRF_STARTUP_PROGRAM, PRF_KEY_PARAMS  );
        LoadProfileSetting( global.szSDir,     global.hIni, PRF_STARTUP_PROGRAM, PRF_KEY_DIR     );
        LoadProfileSetting( global.szSEnv,     global.hIni, PRF_STARTUP_PROGRAM, PRF_KEY_ENV     );
        if ( strlen( global.szProgram ) == 0 ) {
            strncpy( global.szProgram, OS2_SHELL, strlen(OS2_SHELL) );
        }

        // Now load the main dialog window
        hwndFrame = WinLoadDlg( HWND_DESKTOP, HWND_DESKTOP, ClientWndProc,
                                hResources, ID_MAINPROGRAM, &global        );

        if ( hwndFrame == NULLHANDLE ) {
            ulErr = WinGetLastError( hab );
            sprintf( szError,
                     "Failed to load dialog resource.\n\nError severity:\t 0x%02X\nError code:\t 0x%02X",
                     ERRORIDSEV( ulErr ), ERRORIDERROR( ulErr )                                           );
            fInitFailure = TRUE;
        }
    }

    if (( fInitFailure == TRUE ) || ( hwndFrame == NULLHANDLE ))
    {
        WinMessageBox( HWND_DESKTOP, HWND_DESKTOP, szError, "Program Initialization Error", 0, MB_CANCEL | MB_ERROR );
        if (( hmq != NULLHANDLE ) && ( hab != NULLHANDLE )) {
            ulrc = WinMessageBox( HWND_DESKTOP, HWND_DESKTOP,
                                  "The program window could not be created.\n\nIf the shell is running as the desktop process, it will be restarted automatically.  Do you wish to shut down instead?",
                                  "Shut Down?", 0, MB_YESNO | MB_ERROR );
            if ( ulrc == MBID_YES ) {
                WinCancelShutdown( hmq, TRUE );
                WinShutdownSystem( hab, hmq );
            }
        }

    } else {

        // Initialize acceleration table
        hwndAccel = WinLoadAccelTable( hab, global.hResources, ID_MAINPROGRAM );
        WinSetAccelTable( hab, hwndAccel, hwndFrame );

        // Initialize online help
        helpInit.cb                       = sizeof( HELPINIT );
        helpInit.pszTutorialName          = NULL;
        helpInit.phtHelpTable             = (PHELPTABLE) MAKELONG( ID_MAINPROGRAM, 0xFFFF );
        helpInit.hmodHelpTableModule      = global.hResources;
        helpInit.hmodAccelActionBarModule = global.hResources;
        helpInit.fShowPanelId             = 0;
        helpInit.idAccelTable             = 0;
        helpInit.idActionBar              = 0;
        helpInit.pszHelpWindowTitle       = "Help";
        helpInit.pszHelpLibraryName       = HELP_FILE;

        hwndHelp = WinCreateHelpInstance( hab, &helpInit );
        if ( hwndHelp == NULLHANDLE ) {
            WinLoadString( hab, hResources, SZ_HELP_LOADERROR, STRING_RES_LIMIT, szRes );
            sprintf( szError, szRes, HELP_FILE );
            WinMessageBox( HWND_DESKTOP, HWND_DESKTOP, szError, "", 0, MB_OK | MB_WARNING );
        } else
            WinAssociateHelpInstance( hwndHelp, hwndFrame );

        while ( WinGetMsg( hab, &qmsg, 0, 0, 0 )) WinDispatchMsg( hab, &qmsg );

    }
    PrfCloseProfile( global.hIni );
    WinDestroyWindow( hwndFrame );
    WinDestroyMsgQueue( hmq );
    WinTerminate( hab );

    return ( 0 );
}


/* ------------------------------------------------------------------------- *
 * ------------------------------------------------------------------------- */
MRESULT EXPENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
    static    PATSHGLOBAL pGlobal;
    static    HWND        hwndMenu;
    static    BOOL        fRunNow = FALSE;
    HPOINTER  hicon;
    POINTL    mouse;
    HWND      hwndHelp;
    BOOL      fSuccess;
    ULONG     ulAppRet,
              ulRc;
    UCHAR     szError[ 256 ],
              szMsgText[ STRING_RES_LIMIT + 1 ],
              szMsgTitle[ STRING_RES_LIMIT + 1 ];


    switch( msg ) {

        case WM_INITDLG:
            // Save the global data to a window word
            pGlobal = (PATSHGLOBAL) mp2;
            WinSetWindowPtr( hwnd, 0, pGlobal );

            // Set the window mini-icon
            hicon = WinLoadPointer( HWND_DESKTOP, pGlobal->hResources, ID_MAINPROGRAM );
            WinSendMsg( hwnd, WM_SETICON, MPFROMP( hicon ), NULL );

            // Load the popup menu
            hwndMenu = WinLoadMenu( hwnd, pGlobal->hResources, ID_MAINPROGRAM );

            // Disable "secure" controls if requested
            if ( pGlobal->fRunSecure ) {

                WinEnableWindow( WinWindowFromID( hwnd, ID_CONFIGURE ), FALSE );
                WinSendMsg( hwndMenu, MM_SETITEMATTR,
                            MPFROM2SHORT( ID_CONFIGURE, TRUE ), MPFROM2SHORT( MIA_DISABLED, MIA_DISABLED ));
                WinShowWindow( WinWindowFromID( hwnd, ID_CONFIGURE ), FALSE );

                WinEnableWindow( WinWindowFromID( hwnd, ID_STARTUP ), FALSE );
                WinSendMsg( hwndMenu, MM_SETITEMATTR,
                            MPFROM2SHORT( ID_STARTUP, TRUE ), MPFROM2SHORT( MIA_DISABLED, MIA_DISABLED ));

                WinEnableWindow( WinWindowFromID( hwnd, ID_PROMPT ), FALSE );
                WinSendMsg( hwndMenu, MM_SETITEMATTR,
                            MPFROM2SHORT( ID_PROMPT, TRUE ), MPFROM2SHORT( MIA_DISABLED, MIA_DISABLED ));
                WinShowWindow( WinWindowFromID( hwnd, ID_PROMPT ), FALSE );

            }
            // Set up the entry field
            WinSendDlgItemMsg( hwnd, ID_STATUS, EM_SETTEXTLIMIT, (MPARAM) CCHMAXPATH, (MPARAM) 0 );
            WinSetDlgItemText( hwnd, ID_STATUS, pGlobal->szProgram );

            // Set the program icon
            if ( strlen( pGlobal->szProgram ) > 0 ) {
                SetButtonIcon( hwnd, pGlobal->szProgram, pGlobal->hResources );
            }

            // If there's a startup program defined, run it
            if ( strlen( pGlobal->szSProgram ) > 0 ) {
                LaunchProgram( hwnd, pGlobal->szSProgram, pGlobal->szSParams, pGlobal->szSDir, pGlobal->szSEnv, TRUE );
                fRunNow = pGlobal->fRunSilent;
                return (MRESULT) FALSE;         // preserve the /q option if specified
            }

            // Now either show the GUI or run the program (as requested)
            if ( pGlobal->fRunSilent )
                LaunchProgram( hwnd, pGlobal->szProgram, pGlobal->szParams, pGlobal->szDir, pGlobal->szEnv, TRUE );
            else
                WinShowWindow( hwnd, TRUE );

            return (MRESULT) FALSE;


        case WM_COMMAND:
            switch( SHORT1FROMMP( mp1 )) {

                case ID_ABOUT:
                    WinDlgBox( HWND_DESKTOP, hwnd, (PFNWP) AboutDlgProc, pGlobal->hResources, IDD_ABOUT, NULL );
                    break;

                case ID_CONFIGURE:              // Configure the default program
                    if ( ! pGlobal->fRunSecure )
                        DlgConfigure( hwnd, 0 );
                    break;

                case ID_LAUNCH:                 // Launch the default program
                    LaunchProgram( hwnd, pGlobal->szProgram, pGlobal->szParams, pGlobal->szDir, pGlobal->szEnv, TRUE );
                    break;

                case ID_PROMPT:
                    if ( ! pGlobal->fRunSecure )
                        LaunchPrompt( hwnd );   // Open a CMD.EXE window
                    break;

                case ID_SHUTDOWN:               // Shut down the system
                    ulRc = WinLoadString( pGlobal->hab, pGlobal->hResources,
                                          SZ_SHUTDOWN_CONFIRM, STRING_RES_LIMIT, szMsgText );
                    ulRc = WinLoadString( pGlobal->hab, pGlobal->hResources,
                                          SZ_SHUTDOWN_TITLE, STRING_RES_LIMIT, szMsgTitle );
                    if ( WinMessageBox( HWND_DESKTOP, hwnd, szMsgText, szMsgTitle,
                                        0, MB_YESNO | MB_QUERY | MB_MOVEABLE ) == MBID_YES )
                    {
                        WinCancelShutdown( pGlobal->hmq, TRUE );
                        fSuccess = WinShutdownSystem( pGlobal->hab, pGlobal->hmq );
                        if ( fSuccess ) WinPostMsg( hwnd, WM_QUIT, 0, 0 );
                    }
                    return (MRESULT) 0;

                case ID_STARTUP:                // Configure the startup program
                    DlgConfigure( hwnd, 1 );
                    break;

                case ID_QUIT:                   // Exit the program
                    WinPostMsg( hwnd, WM_CLOSE, 0, 0 );
                    return (MRESULT) 0;

                default: break;

            } // end WM_COMMAND messages
            return (MRESULT) 0;


        case WM_CONTROL:
            switch( SHORT1FROMMP( mp1 )) {
                default: break;
            } // end WM_CONTROL messages
            return (MRESULT) 0;

/*
        case WM_CONTEXTMENU:
            if SHORT2FROMMP( mp2 )
                WinQueryPointerPos( HWND_DESKTOP, &mouse );
            else {
                mouse.x = SHORT1FROMMP( mp1 );
                mouse.y = SHORT2FROMMP( mp1 );
                WinMapWindowPoints( hwnd, HWND_DESKTOP, &mouse, 1 );
            }
            WinPopupMenu( HWND_DESKTOP, hwnd, hwndMenu, mouse.x, mouse.y, ID_MAINPROGRAM,
                          PU_HCONSTRAIN | PU_VCONSTRAIN | PU_MOUSEBUTTON1 | PU_MOUSEBUTTON2 | PU_KEYBOARD );
            return 0;
*/

        case WM_BUTTON2UP:
            mouse.x = SHORT1FROMMP( mp1 );
            mouse.y = SHORT2FROMMP( mp1 );
            WinMapWindowPoints( hwnd, HWND_DESKTOP, &mouse, 1 );
            WinPopupMenu( HWND_DESKTOP, hwnd, hwndMenu, mouse.x, mouse.y, ID_MAINPROGRAM,
                          PU_HCONSTRAIN | PU_VCONSTRAIN | PU_MOUSEBUTTON1 | PU_MOUSEBUTTON2 | PU_KEYBOARD );
            return (MRESULT) 0;


        case WM_APPTERMINATENOTIFY:
            ulAppRet = (ULONG) mp2;
            if ( fRunNow ) {
                // fRunNow is set when fRunSilent is true AND a startup program was executed.
                // It basically preserves the /q option once the startup program exits.
                fRunNow = FALSE;
                LaunchProgram( hwnd, pGlobal->szProgram, pGlobal->szParams, pGlobal->szDir, pGlobal->szEnv, TRUE );
            } else
                WinShowWindow( hwnd, TRUE );
            break;


        case WM_HELP:
            if (( hwndHelp = WinQueryHelpInstance( hwnd )) != NULLHANDLE )
                WinSendMsg( hwndHelp, HM_GENERAL_HELP, 0, 0 );
            return (MRESULT) 0;


        case WM_CLOSE:
            ulRc = WinLoadString( pGlobal->hab, pGlobal->hResources,
                                  SZ_CLOSE_CONFIRM, STRING_RES_LIMIT, szMsgText );
            ulRc = WinLoadString( pGlobal->hab, pGlobal->hResources,
                                  SZ_CLOSE_TITLE, STRING_RES_LIMIT, szMsgTitle );
            if ( WinMessageBox( HWND_DESKTOP, hwnd, szMsgText, szMsgTitle,
                                0, MB_OKCANCEL | MB_QUERY | MB_MOVEABLE ) != MBID_CANCEL )
                WinPostMsg( hwnd, WM_QUIT, 0, 0 );
            return (MRESULT) 0;


    } // end event handlers

    return WinDefDlgProc( hwnd, msg, mp1, mp2 );
}


/* ------------------------------------------------------------------------- *
 * LocateProfile                                                             *
 *                                                                           *
 * Figure out where to place our INI file.  This will be in the same         *
 * directory as OS2.INI (the OS/2 user profile).                             *
 *                                                                           *
 * ARGUMENTS:                                                                *
 *     PSZ pszProfile : Character buffer to receive the INI filename         *
 *                                                                           *
 * RETURNS: N/A                                                              *
 * ------------------------------------------------------------------------- */
void LocateProfile( PSZ pszProfile )
{
    ULONG  ulRc;
    SHORT  x;
    PSZ    pszOs2Ini;

    // Query the %USER_INI% environment variable which points to OS2.INI
    ulRc = DosScanEnv("USER_INI", &pszOs2Ini );
    strncpy( pszProfile, pszOs2Ini, CCHMAXPATH );

    // Strip out the filename portion (just keep the path) of %USER_INI%
    x = strlen( pszProfile ) - 1;
    while (( x >= 0 ) && ( pszProfile[x] != '\\')) x--;
    pszProfile[ x+1 ] = '\0';

    // Now define our own INI filename
    strncat( pszProfile, PROFILE, CCHMAXPATH );

}


/* ------------------------------------------------------------------------- *
 * LoadProfileSetting                                                        *
 *                                                                           *
 * Retrieve a value from the program INI file.                               *
 *                                                                           *
 * ARGUMENTS:                                                                *
 *     PSZ  pszData : Character buffer to place the data in.                 *
 *     HINI hIni    : Handle to the INI file.                                *
 *     PSZ  pszApp  : INI application name.                                  *
 *     PSZ  pszKey  : INI key name.                                          *
 *                                                                           *
 * RETURNS: N/A                                                              *
 * ------------------------------------------------------------------------- */
void LoadProfileSetting( PSZ pszData, HINI hIni, PSZ pszApp, PSZ pszKey )
{
    BOOL  fOK;
    ULONG ulBytes;

    fOK = PrfQueryProfileSize( hIni, pszApp, pszKey, &ulBytes );
    if ( fOK && ( ulBytes > 0 )) {
        PrfQueryProfileData( hIni, pszApp, pszKey, pszData, &ulBytes );
    }
}


/* ------------------------------------------------------------------------- *
 * CentreWindow                                                              *
 *                                                                           *
 * Centres the given window on the screen.                                   *
 *                                                                           *
 * ARGUMENTS:                                                                *
 *     HWND hwnd: handle of the window to be centred.                        *
 *                                                                           *
 * RETURNS: N/A                                                              *
 * ------------------------------------------------------------------------- */
void CentreWindow( HWND hwnd )
{
    LONG scr_width, scr_height;
    LONG x, y;
    SWP wp;

    scr_width = WinQuerySysValue( HWND_DESKTOP, SV_CXSCREEN );
    scr_height = WinQuerySysValue( HWND_DESKTOP, SV_CYSCREEN );

    if ( WinQueryWindowPos( hwnd, &wp )) {
        x = ( scr_width - wp.cx ) / 2;
        y = ( scr_height - wp.cy ) / 2;
        WinSetWindowPos( hwnd, HWND_TOP, x, y, wp.cx, wp.cy, SWP_MOVE | SWP_ACTIVATE );
    }

}


/* ------------------------------------------------------------------------- *
 * SetButtonIcon                                                             *
 *                                                                           *
 * Changes the icon of the main desktop launch button to the icon of the     *
 * specified program file (as loaded using standard PM logic).               *
 *                                                                           *
 * ARGUMENTS:                                                                *
 *     HWND hwnd      : handle of the main program window.                   *
 *     PSZ  pszProgram: the program file whose icon will be used.            *
 *                                                                           *
 * RETURNS: N/A                                                              *
 * ------------------------------------------------------------------------- */
void SetButtonIcon( HWND hwnd, PSZ pszProgram, HMODULE hResources )
{
    HPOINTER     hicon    = 0,
                 hiconOld = 0;
    HDIR         hdir  = HDIR_SYSTEM;
    FILEFINDBUF3 fbuf3 = { 0 };
    BTNCDATA     ctrlData;
    WNDPARAMS    wndParams;
    CHAR         szExePath[ CCHMAXPATH + 1 ];
    ULONG        ulFC = 1,
                 ulRC;


    if ( strnicmp( pszProgram, "PMSHELL.EXE", CCHMAXPATH ) == 0 )
        hicon = WinLoadPointer( HWND_DESKTOP, hResources, IDI_ATSHELL );
    else {
        ulRC = DosFindFirst( pszProgram, &hdir, FILE_NORMAL,
                             &fbuf3, sizeof(fbuf3), &ulFC, FIL_STANDARD );
        if ( ulRC == NO_ERROR ) hicon = WinLoadFileIcon( pszProgram, FALSE );
        else {
            ulRC = DosSearchPath( SEARCH_IGNORENETERRS | SEARCH_ENVIRONMENT | SEARCH_CUR_DIRECTORY,
                                  "PATH", pszProgram, szExePath, CCHMAXPATH + 1 );
            if ( ulRC == NO_ERROR ) hicon = WinLoadFileIcon( szExePath, FALSE );
        }
    }

    if ( !hicon ) hicon = WinLoadPointer( HWND_DESKTOP, hResources, IDI_ATSHELL );
    if ( hicon ) {
        wndParams.fsStatus  = WPM_CBCTLDATA | WPM_CTLDATA;
        wndParams.cbCtlData = sizeof( BTNCDATA );
        wndParams.pCtlData  = &ctrlData;
        if ( ! WinSendDlgItemMsg( hwnd, ID_LAUNCH, WM_QUERYWINDOWPARAMS, MPFROMP( &wndParams ), (MPARAM) 0 ))
            return;
        hiconOld        = ctrlData.hImage;
        ctrlData.hImage = hicon;
        if ( hiconOld ) WinDestroyPointer( hiconOld );
        WinSendDlgItemMsg( hwnd, ID_LAUNCH, WM_SETWINDOWPARAMS, MPFROMP( &wndParams ), (MPARAM) 0 );
    }

}


/* ------------------------------------------------------------------------- *
 * LaunchPrompt                                                              *
 *                                                                           *
 * Launches a windowable CMD.EXE session, with the working directory set to  *
 * the root of the boot drive.                                               *
 *                                                                           *
 * ARGUMENTS:                                                                *
 *     HWND hwnd: handle of the main application window.                     *
 *                                                                           *
 * RETURNS: N/A                                                              *
 * ------------------------------------------------------------------------- */
void LaunchPrompt( HWND hwnd )
{
    ULONG ulRc,
          ulOrd;
    UCHAR szBootDrv[ 4 ];


    ulRc = DosQuerySysInfo( QSV_BOOT_DRIVE, QSV_BOOT_DRIVE, &ulOrd, sizeof(ULONG) );
    if (( ulRc != NO_ERROR ) || ( ulOrd > 26 ) || ( ulOrd < 1 ))
        LaunchProgram( hwnd, "CMD.EXE", NULL, NULL, NULL, FALSE );
    else {
        sprintf( szBootDrv, "%c:\\", 64 + ulOrd );
        LaunchProgram( hwnd, "CMD.EXE", NULL, szBootDrv, NULL, FALSE );
    }
}


/* ------------------------------------------------------------------------- *
 * LaunchProgram                                                             *
 *                                                                           *
 * Launches the selected program.                                            *
 *                                                                           *
 * ARGUMENTS:                                                                *
 *     HWND hwnd      : handle of the main application window.               *
 *     PSZ pszProgram : program path and filename.                           *
 *     PSZ pszParams  : program parameters.                                  *
 *     PSZ pszDir     : program startup directory.                           *
 *     PSZ pszEnv     : string to append to program environment.             *
 *     BOOL fHide     : determines if ATSHELL should hide when program runs. *
 *                                                                           *
 * RETURNS: N/A                                                              *
 * ------------------------------------------------------------------------- */
void LaunchProgram( HWND hwnd, PSZ pszProgram, PSZ pszParams, PSZ pszDir, PSZ pszEnv, BOOL fHide )
{
    PATSHGLOBAL  pGlobal;
    PPROGDETAILS pDetails;
    HAPP         happ;
    UCHAR        szError[ STRING_RES_LIMIT + 20 ],
                 szMsgText[ STRING_RES_LIMIT + 1 ],
                 szMsgTitle[ STRING_RES_LIMIT + 1 ];
    ULONG        ulErr;
    LONG         ulRc;


    // TODO: get the current environment and append pszEnv.

    pGlobal = WinQueryWindowPtr( hwnd, 0 );

    pDetails                              = (PPROGDETAILS) malloc( sizeof(PROGDETAILS) );
    pDetails->Length                      = sizeof( PROGDETAILS );
    pDetails->progt.progc                 = PROG_DEFAULT;
    pDetails->progt.fbVisible             = SHE_VISIBLE;
    pDetails->pszTitle                    = "";
    pDetails->pszExecutable               = pszProgram;
    pDetails->pszParameters               = pszParams;
    pDetails->pszStartupDir               = pszDir;
    pDetails->pszIcon                     = NULL;
    pDetails->pszEnvironment              = pszEnv;
    pDetails->swpInitial.fl               = SWP_ACTIVATE | SWP_ZORDER | SWP_SHOW;
    pDetails->swpInitial.cy               = 0;
    pDetails->swpInitial.cx               = 0;
    pDetails->swpInitial.y                = 0;
    pDetails->swpInitial.x                = 0;
    pDetails->swpInitial.hwndInsertBehind = HWND_TOP;
    pDetails->swpInitial.hwnd             = hwnd;
    pDetails->swpInitial.ulReserved1      = 0;
    pDetails->swpInitial.ulReserved2      = 0;

    happ = WinStartApp( hwnd, pDetails, NULL, NULL, SAF_INSTALLEDCMDLINE | SAF_STARTCHILDAPP );
    if ( happ == NULLHANDLE ) {
        ulErr = WinGetLastError( pGlobal->hab );
        ulRc = WinLoadString( pGlobal->hab, pGlobal->hResources,
                              SZ_EXECUTE_ERROR, STRING_RES_LIMIT, szMsgText );
        ulRc = WinLoadString( pGlobal->hab, pGlobal->hResources,
                              SZ_EXECUTE_TITLE, STRING_RES_LIMIT, szMsgTitle );
        sprintf( szError, szMsgText, ERRORIDSEV( ulErr ), ERRORIDERROR( ulErr ));
        WinMessageBox( HWND_DESKTOP, HWND_DESKTOP, szError, szMsgTitle, 0, MB_CANCEL | MB_ERROR );
        WinShowWindow( hwnd, TRUE );
    }
    else if ( fHide) WinShowWindow( hwnd, FALSE );

    free( pDetails );
}


/* ------------------------------------------------------------------------- *
 * DlgConfigure                                                              *
 *                                                                           *
 * Opens the program configuration dialog.                                   *
 *                                                                           *
 * ARGUMENTS:                                                                *
 *     HWND hwnd        : handle of the main application window.             *
 *     USHORT usProgram : identifies which program will be configured.       *
 *                        0 = default program; 1 = startup program           *
 *                                                                           *
 * RETURNS: N/A                                                              *
 * ------------------------------------------------------------------------- */
void DlgConfigure( HWND hwnd, USHORT usProgram )
{
    PATSHGLOBAL   pGlobal;
    ATSHAPPCONFIG dlgOptions;


    pGlobal = WinQueryWindowPtr( hwnd, 0 );

    dlgOptions.hab         = pGlobal->hab;
    dlgOptions.hResources  = pGlobal->hResources;
    dlgOptions.fChanged    = FALSE;

    if ( usProgram > 0 ) {
        strcpy( dlgOptions.szProgram, pGlobal->szSProgram );
        strcpy( dlgOptions.szParams,  pGlobal->szSParams  );
        strcpy( dlgOptions.szDir,     pGlobal->szSDir     );
        strcpy( dlgOptions.szEnv,     pGlobal->szSEnv     );
    } else {
        strcpy( dlgOptions.szProgram, pGlobal->szProgram );
        strcpy( dlgOptions.szParams,  pGlobal->szParams  );
        strcpy( dlgOptions.szDir,     pGlobal->szDir     );
        strcpy( dlgOptions.szEnv,     pGlobal->szEnv     );
    }

    WinDlgBox( HWND_DESKTOP, hwnd, (PFNWP) AppCfgDlgProc,
               pGlobal->hResources, IDD_PROGCONFIG, &dlgOptions );

    if ( dlgOptions.fChanged ) {
        if ( usProgram > 0 ) {
            strcpy( pGlobal->szSProgram, dlgOptions.szProgram );
            strcpy( pGlobal->szSParams,  dlgOptions.szParams  );
            strcpy( pGlobal->szSDir,     dlgOptions.szDir     );
            strcpy( pGlobal->szSEnv,     dlgOptions.szEnv     );
            // Save settings to profile
            PrfWriteProfileString( pGlobal->hIni, PRF_STARTUP_PROGRAM,
                                   PRF_KEY_PROGRAM, pGlobal->szSProgram );
            PrfWriteProfileString( pGlobal->hIni, PRF_STARTUP_PROGRAM,
                                   PRF_KEY_PARAMS, pGlobal->szSParams   );
            PrfWriteProfileString( pGlobal->hIni, PRF_STARTUP_PROGRAM,
                                   PRF_KEY_DIR, pGlobal->szSDir         );
            PrfWriteProfileString( pGlobal->hIni, PRF_STARTUP_PROGRAM,
                                   PRF_KEY_ENV, pGlobal->szSEnv         );
        } else {

            // TODO: should set pszProgram to PMSHELL if field is blank
            strcpy( pGlobal->szProgram, dlgOptions.szProgram );
            strcpy( pGlobal->szParams,  dlgOptions.szParams );
            strcpy( pGlobal->szDir,     dlgOptions.szDir );
            strcpy( pGlobal->szEnv,     dlgOptions.szEnv );
            WinSetDlgItemText( hwnd, ID_STATUS, pGlobal->szProgram );
            SetButtonIcon( hwnd, pGlobal->szProgram, pGlobal->hResources );
            // Save settings to profile
            PrfWriteProfileString( pGlobal->hIni, PRF_DEFAULT_PROGRAM,
                                   PRF_KEY_PROGRAM, pGlobal->szProgram );
            PrfWriteProfileString( pGlobal->hIni, PRF_DEFAULT_PROGRAM,
                                   PRF_KEY_PARAMS, pGlobal->szParams   );
            PrfWriteProfileString( pGlobal->hIni, PRF_DEFAULT_PROGRAM,
                                   PRF_KEY_DIR, pGlobal->szDir         );
            PrfWriteProfileString( pGlobal->hIni, PRF_DEFAULT_PROGRAM,
                                   PRF_KEY_ENV, pGlobal->szEnv         );
        }
    }

}


/* ------------------------------------------------------------------------- *
 * AppCfgDlgProc                                                             *
 *                                                                           *
 * Dialog procedure for the program configuration dialog.                    *
 * ------------------------------------------------------------------------- */
MRESULT EXPENTRY AppCfgDlgProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
    static PATSHAPPCONFIG pOptions;
    CHAR   szProgram[ CCHMAXPATH + 1 ],
           szParams[ PARAMETER_LIMIT + 1 ],
           szDir[ CCHMAXPATH + 1 ],
           szEnv[ ENVIRONMENT_LIMIT + 1 ];
    ULONG  ulItem,
           c;


    switch ( msg ) {

        case WM_INITDLG:
            pOptions = (PATSHAPPCONFIG) mp2;
            CentreWindow( hwnd );
            WinSendDlgItemMsg( hwnd, IDD_PROGNAME,  EM_SETTEXTLIMIT, MPFROMLONG(CCHMAXPATH),         0 );
            WinSendDlgItemMsg( hwnd, IDD_PROGPARMS, EM_SETTEXTLIMIT, MPFROMSHORT(PARAMETER_LIMIT),   0 );
            WinSendDlgItemMsg( hwnd, IDD_PROGDIR,   EM_SETTEXTLIMIT, MPFROMLONG(CCHMAXPATH),         0 );
            WinSendDlgItemMsg( hwnd, IDD_PROGENV,   EM_SETTEXTLIMIT, MPFROMSHORT(ENVIRONMENT_LIMIT), 0 );
            WinSetDlgItemText( hwnd, IDD_PROGNAME,  pOptions->szProgram );
            WinSetDlgItemText( hwnd, IDD_PROGPARMS, pOptions->szParams );
            WinSetDlgItemText( hwnd, IDD_PROGDIR,   pOptions->szDir );
            // WinSetDlgItemText( hwnd, IDD_PROGENV,   pOptions->pszEnv );
            break;

        case WM_COMMAND:
            switch ( SHORT1FROMMP( mp1 )) {

                case IDD_PROGBROWSE:
                    BrowseProgram( hwnd, pOptions->hab, pOptions->hResources );
                    return (MRESULT) 0;

                case DID_OK:
                    memset( szProgram, 0, sizeof(szProgram) );
                    memset( szParams,  0, sizeof(szParams) );
                    memset( szDir,     0, sizeof(szDir) );
                    memset( szEnv,     0, sizeof(szEnv) );

                    // Query program filespec
                    WinQueryDlgItemText( hwnd, IDD_PROGNAME, CCHMAXPATH, szProgram );
                    if ( strncmp( pOptions->szProgram, szProgram, CCHMAXPATH ) != 0 ) {
                        strncpy( pOptions->szProgram, szProgram, CCHMAXPATH );
                        pOptions->fChanged  = TRUE;
                    }
                    // Query parameters
                    WinQueryDlgItemText( hwnd, IDD_PROGPARMS, PARAMETER_LIMIT, szParams );
                    if ( strncmp( pOptions->szParams, szParams, PARAMETER_LIMIT ) != 0 ) {
                        strncpy( pOptions->szParams, szParams, PARAMETER_LIMIT );
                        pOptions->fChanged = TRUE;
                    }
                    // Query startup directory
                    WinQueryDlgItemText( hwnd, IDD_PROGDIR, CCHMAXPATH, szDir );
                    // Strip any trailing backslash
                    c = strlen( szDir ) - 1;
                    if ( szDir[ c ] == '\\') szDir[ c ] = '\0';
                    if ( strncmp( pOptions->szDir, szDir, CCHMAXPATH ) != 0 ) {
                        strncpy( pOptions->szDir, szDir, CCHMAXPATH );
                        pOptions->fChanged = TRUE;
                    }
                    // TODO: Query environment variable additions from combo-box and concatenate them together with \0.
                    break;

                case IDD_PROGENVADD:
                    WinQueryDlgItemText( hwnd, IDD_PROGENV, ENVIRONMENT_LIMIT, szEnv );
                    WinSendDlgItemMsg( hwnd, IDD_PROGENV, LM_INSERTITEM, MPFROMSHORT( LIT_END ), MPFROMP( szEnv ));
                    return (MRESULT) 0;

                case IDD_PROGENVDEL:
                    ulItem = (ULONG) WinSendDlgItemMsg( hwnd, IDD_PROGENV, LM_QUERYSELECTION, MPFROMSHORT( LIT_CURSOR ), 0 );
                    if ( ulItem != LIT_NONE )
                        WinSendDlgItemMsg( hwnd, IDD_PROGENV, LM_DELETEITEM, MPFROMSHORT( ulItem ), 0 );
                    return (MRESULT) 0;

                default: break;
            }
            break;

        default: break;
    }

    return WinDefDlgProc( hwnd, msg, mp1, mp2 );
}


/* ------------------------------------------------------------------------- *
 * BrowseProgram                                                             *
 *                                                                           *
 * Opens a File dialog for the user to select a program after clicking on    *
 * 'Browse' from the program configuration dialog.                           *
 *                                                                           *
 * ARGUMENTS:                                                                *
 *     HWND hwnd: handle of the main application window.                     *
 *                                                                           *
 * RETURNS: N/A                                                              *
 * ------------------------------------------------------------------------- */
void BrowseProgram( HWND hwnd, HAB hab, HMODULE hResources )
{
    FILEDLG fild;
    HWND    hwndDlg;
    ULONG   ulRc;
    CHAR    szTitle[ STRING_RES_LIMIT + 1 ];


    ulRc = WinLoadString( hab, hResources, SZ_BROWSE_TITLE, STRING_RES_LIMIT, szTitle );
    if ( ulRc != NO_ERROR ) sprintf( szTitle, "Select Program");

    memset( &fild, 0, sizeof(FILEDLG) );
    fild.cbSize   = sizeof( FILEDLG );
    fild.fl       = FDS_HELPBUTTON | FDS_CENTER | FDS_OPEN_DIALOG;
    fild.pszTitle = szTitle;
    strcpy( fild.szFullFile, "*.EXE");

    hwndDlg = WinFileDlg( HWND_DESKTOP, hwnd, &fild );
    if ( hwndDlg && (fild.lReturn == DID_OK ))
    {
        WinSetDlgItemText( hwnd, IDD_PROGNAME, fild.szFullFile );
    }

}


/* ------------------------------------------------------------------------- *
 * AboutDlgProc                                                              *
 *                                                                           *
 * Dialog procedure for the product information dialog.                      *
 * ------------------------------------------------------------------------- */
MRESULT EXPENTRY AboutDlgProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
    switch ( msg ) {

        case WM_INITDLG:
            CentreWindow( hwnd );
            break;

        default: break;
    }

    return WinDefDlgProc( hwnd, msg, mp1, mp2 );
}

