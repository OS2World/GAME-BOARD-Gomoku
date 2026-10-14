/*--------------------------------------------------------------------------*/
/* gomoku.c - Gomoku for OS/2                                               */
/*                                                                          */
/* Original author: Jasper de Keijzer (game logic ported from Pascal)      */
/* OpenWatcom port: OS2World, 2026                                          */
/*                                                                          */
/* change --- date ----who----Description-----------------------------------*/
/*   1       040328    JdK    Algorithm fix for diagonal lines.             */
/*   2       260911    OS2W   OpenWatcom port; standard menu/About dialog.  */
/*--------------------------------------------------------------------------*/
#define INCL_WIN
#define INCL_GPI
#define INCL_WINHEAP
#define INCL_WINDIALOGS
#define INCL_DOSPROCESS
#define INCL_WINSHELLDATA

#define DIVISIONS 19

#include <os2.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

/* Resource and menu IDs (keep in sync with gomoku.h / gomoku.rc) */
#define ID_RESOURCE         1
#define IB_BOARD            2
#define IDD_ABOUT           501
#define IDD_KEYMAP          502
#define IDC_KM_HINT         510
#define IDC_KM_LABEL        511
#define IDC_KM_ENTRY        512
#define IDC_KM_DEFAULT      513
#define IDM_NEW             100
#define IDM_EXIT            110
#define IDM_SAVEONEXIT      200
#define IDM_KEYMAP          205
#define IDM_FRAME           210
#define IDM_LANG_EN         300
#define IDM_LANG_ES         301
#define IDM_LANG_NL         302
#define IDM_LANG_DE         303
#define IDM_LANG_FR         304
#define IDM_LANG_IT         305
#define IDM_ABOUT           999
#define IDM_SUBMENU_GAME    1000
#define IDM_SUBMENU_OPTIONS 1001
#define IDM_SUBMENU_HELP    1002
#define IDM_SUBMENU_LANG    1003

/* Language support (plan.txt sec.8) */
#define LANG_EN    0
#define LANG_ES    1
#define LANG_NL    2
#define LANG_DE    3
#define LANG_FR    4
#define LANG_IT    5
#define LANG_COUNT 6

enum {
    STR_MENU_GAME=0, STR_MENU_NEW, STR_MENU_EXIT,
    STR_MENU_OPTIONS, STR_MENU_LANGUAGE, STR_MENU_SAVEONEXIT,
    STR_MENU_FRAME, STR_MENU_HELP, STR_MENU_ABOUT,
    STR_MSG_IWON, STR_TITLE_IWON,
    STR_MSG_YOUWON, STR_TITLE_YOUWON,
    STR_MENU_KEYMAP, STR_KM_TITLE, STR_KM_HINT, STR_KM_ACT0,
    STR_KM_BTN1, STR_KM_BTN2, STR_KM_BTN3, STR_KM_DEFAULT,
    STR_DLG_OK, STR_DLG_CANCEL,
    STR_COUNT
};

int current_lang = LANG_EN;

const char *lang_strings[LANG_COUNT][STR_COUNT] = {
  { /* EN */
    "~Game",
    "~New Game\tCtrl+N",
    "E~xit\tCtrl+X",
    "~Options",
    "~Language",
    "~Save settings on exit",
    "~Frame Controls\tCtrl+F",
    "~Help",
    "~About...",
    "I won!", "Ha Ha",
    "You won!", "Congratulations",
    "~Key Mapping...", "Key Mapping", "Click the box, then click it again with the mouse button to use.", "Place a piece", "Left button", "Right button", "Middle button", "Default", "OK", "Cancel"
  },
  { /* ES */
    "~Juego",
    "~Nueva Partida\tCtrl+N",
    "Sa~lir\tCtrl+X",
    "~Opciones",
    "~Idioma",
    "~Guardar ajustes al salir",
    "~Controles de Marco\tCtrl+F",
    "~Ayuda",
    "~About...",
    "Yo gane!", "Ja Ja",
    "Has ganado!", "Felicitaciones",
    "~Asignacion de teclas...", "Asignacion de teclas", "Haga clic en el cuadro y de nuevo con el boton del raton que quiera usar.", "Colocar una pieza", "Boton izquierdo", "Boton derecho", "Boton central", "Por defecto", "Aceptar", "Cancelar"
  },
  { /* NL */
    "~Spel",
    "~Nieuw Spel\tCtrl+N",
    "A~fsluiten\tCtrl+X",
    "~Opties",
    "~Taal",
    "~Bewaar instelling bij afsluiten",
    "~Kaderinstelling\tCtrl+F",
    "~Help",
    "~About...",
    "Ik heb gewonnen!", "Ha Ha",
    "Jij hebt gewonnen!", "Gefeliciteerd",
    "Toetsen toe~wijzen...", "Toetsen toewijzen", "Klik op het vak en klik er nogmaals op met de muisknop die u wilt gebruiken.", "Steen plaatsen", "Linkerknop", "Rechterknop", "Middelste knop", "Standaard", "OK", "Annuleren"
  },
  { /* DE */
    "~Spiel",
    "~Neu\tCtrl+N",
    "~Beenden\tCtrl+X",
    "~Optionen",
    "~Sprache",
    "~Einstellungen speichern",
    "~Rahmensteuerung\tCtrl+F",
    "~Hilfe",
    "~About...",
    "Ich habe gewonnen!", "Ha Ha",
    "Du hast gewonnen!", "Herzlichen Glueckwunsch",
    "~Tastenzuordnung...", "Tastenzuordnung", "Klicken Sie auf das Feld und dann erneut mit der gewuenschten Maustaste.", "Stein setzen", "Linke Taste", "Rechte Taste", "Mittlere Taste", "Standard", "OK", "Abbrechen"
  },
  { /* FR */
    "~Jeu",
    "~Nouvelle Partie\tCtrl+N",
    "~Quitter\tCtrl+X",
    "~Options",
    "~Langue",
    "~Sauvegarder a la fermeture",
    "~Controles du Cadre\tCtrl+F",
    "~Aide",
    "~About...",
    "J'ai gagne!", "Ha Ha",
    "Vous avez gagne!", "Felicitations",
    "~Affectation des touches...", "Affectation des touches", "Cliquez sur la case, puis cliquez dessus avec le bouton de souris voulu.", "Poser un pion", "Bouton gauche", "Bouton droit", "Bouton central", "Defaut", "OK", "Annuler"
  },
  { /* IT */
    "~Gioco",
    "~Nuova Partita\tCtrl+N",
    "~Esci\tCtrl+X",
    "~Opzioni",
    "~Lingua",
    "~Salva alla chiusura",
    "~Controllo Cornice\tCtrl+F",
    "~Aiuto",
    "~About...",
    "Ho vinto!", "Ha Ha",
    "Hai vinto!", "Congratulazioni",
    "Assegnazione ~tasti...", "Assegnazione tasti", "Fai clic sulla casella, poi di nuovo con il pulsante del mouse da usare.", "Posiziona una pedina", "Pulsante sinistro", "Pulsante destro", "Pulsante centrale", "Predefinito", "OK", "Annulla"
  }
};

#define tr(id) ((char*)lang_strings[current_lang][(id)])

static const char bldlevel[] =
    "@#Jasper de Keijzer:1.2#@##1## 13 Oct 2026 00:00:00      "
    "ARCAOS:::0::::@@Gomoku for OS/2 - 5-in-a-row board game\r\n\x1a";

MRESULT EXPENTRY ClientWndProc(HWND, ULONG, MPARAM, MPARAM);
MRESULT EXPENTRY AboutDlgProc(HWND, ULONG, MPARAM, MPARAM);
MRESULT EXPENTRY KeyMapDlgProc(HWND, ULONG, MPARAM, MPARAM);

HWND hwndFrame, hwndClient;

static HWND hwndTitleBar = NULLHANDLE;
static HWND hwndSysMenu  = NULLHANDLE;
static HWND hwndMinMax   = NULLHANDLE;
static HWND hwndMenuBar  = NULLHANDLE;
static BOOL bFrameHidden = FALSE;

static CHAR szClientClass[] = "GOMOKU";

#define BITMAPSIZE  40

#define kGameBoardSize  19

#define Horiz     1
#define DownLeft  2
#define DownRight 3
#define Vert      4

#define NR_LINES  5

/***********************************************************************
 *   Types
 ***********************************************************************/
typedef enum {
    kStateIdle  = 0,
    kStateIWon,
    kStateYouWon
} StateEnumType;

typedef enum {
    kSquareEmpty     = 0,
    kSquareCross     = 1,
    kSquareNought    = 2,
    kSquareTemporary = 4
} SquareEnumType;

/***********************************************************************
 *   Global variables
 ***********************************************************************/
static StateEnumType  gState;
static SquareEnumType gGameBoard[kGameBoardSize+5][kGameBoardSize+5];

static int  Player;
static long TotalLines;
static BOOL GameWon;
static BOOL autoPlay;
static int  Line[NR_LINES][kGameBoardSize][kGameBoardSize][2];
static int  Value[kGameBoardSize][kGameBoardSize][2];
static int  AttackFactor;
static int  WinningLine;
static int  xWinPos, yWinPos;
static long Weight[] = { 0, 0, 4, 20, 100, 500, 0 };

static BOOL bSaveOnExit = TRUE;     /* "Save settings on exit" is on by default */

/* Mouse button (1 left, 2 right, 3 middle) that places a piece (Key Mapping) */
static int btn_place = 1;

/*--------------------------------------------------------------------------*/
static int opponentColor(void)
{
    if (Player == kSquareCross)
        return kSquareNought;
    else
        return kSquareCross;
}

/*--------------------------------------------------------------------------*/
static int valueAdd(int Num)
{
    Num++;
    if (Num == 1)  TotalLines--;
    if (Num >= 5)  GameWon = TRUE;
    return Num;
}

/*--------------------------------------------------------------------------*/
static void globalUpdate(int a, int b, int c, int d, int e, int Opponent)
{
    if (Line[a][b][c][Opponent - kSquareCross] == 0) {
        Value[d][e][Player - kSquareCross] +=
            Weight[Line[a][b][c][Player - kSquareCross] + 1] -
            Weight[Line[a][b][c][Player - kSquareCross]];
    } else if (Line[a][b][c][Player - kSquareCross] == 1) {
        Value[d][e][Opponent - kSquareCross] -=
            Weight[Line[a][b][c][Opponent - kSquareCross]];
    }
}

/*--------------------------------------------------------------------------*/
static void MakeMove(int X, int Y)
{
    int X1, Y1, K, L;
    RECTL rcl;
    int Opponent = opponentColor();

    GameWon    = FALSE;
    WinningLine = 0;

    for (K = 0; K < 5; K++) {          /* Horizontal */
        X1 = X - K;  Y1 = Y;
        if (X1 >= 0) {
            Line[0][X1][Y1][Player - kSquareCross] =
                valueAdd(Line[0][X1][Y1][Player - kSquareCross]);
            if (GameWon && WinningLine == 0)
                { xWinPos = X1; yWinPos = Y1; WinningLine = Horiz; }
            for (L = 0; L < 5; L++)
                globalUpdate(0, X1, Y1, X1+L, Y1, Opponent);
        }
    }

    for (K = 0; K < 5; K++) {          /* Diagonal down-right to upper-left */
        X1 = X - K;  Y1 = Y - K;
        if (X1 >= 0 && Y1 >= 0) {
            Line[1][X1][Y1][Player - kSquareCross] =
                valueAdd(Line[1][X1][Y1][Player - kSquareCross]);
            if (GameWon && WinningLine == 0)
                { xWinPos = X1; yWinPos = Y1; WinningLine = DownRight; }
            for (L = 0; L < 5; L++)
                globalUpdate(1, X1, Y1, X1+L, Y1+L, Opponent);
        }
    }

    for (K = 0; K < 5; K++) {          /* Vertical */
        X1 = X;  Y1 = Y - K;
        if (X1 >= 0 && Y1 >= 0) {
            Line[2][X1][Y1][Player - kSquareCross] =
                valueAdd(Line[2][X1][Y1][Player - kSquareCross]);
            if (GameWon && WinningLine == 0)
                { xWinPos = X1; yWinPos = Y1; WinningLine = Vert; }
            for (L = 0; L < 5; L++)
                globalUpdate(2, X1, Y1, X1, Y1+L, Opponent);
        }
    }

    for (K = 0; K < 5; K++) {          /* Diagonal lower-left to upper-right */
        X1 = X - K;  Y1 = Y + K;
        if (X1 >= 0 && Y1 < kGameBoardSize) {
            Line[3][X1][Y1][Player - kSquareCross] =
                valueAdd(Line[3][X1][Y1][Player - kSquareCross]);
            if (GameWon && WinningLine == 0)
                { xWinPos = X1; yWinPos = Y1; WinningLine = DownLeft; }
            for (L = 0; L < 5; L++)
                globalUpdate(3, X1, Y1, X1+L, Y1-L, Opponent);
        }
    }

    if (Player == kSquareCross)
        gGameBoard[X][Y] = kSquareCross;
    else
        gGameBoard[X][Y] = kSquareNought;

    if (!GameWon)
        Player = Opponent;

    rcl.xRight = BITMAPSIZE + (rcl.xLeft   = X * BITMAPSIZE);
    rcl.yTop   = BITMAPSIZE + (rcl.yBottom = Y * BITMAPSIZE);
    WinInvalidateRect(hwndClient, &rcl, FALSE);
}

/*--------------------------------------------------------------------------*/
static void FindMove(int *xPos, int *yPos)
{
    int Opponent = opponentColor();
    int I, J, Max, Valu;

    Max   = -10000;
    *xPos = (kGameBoardSize + 1) / 2;
    *yPos = (kGameBoardSize + 1) / 2;
    if (gGameBoard[*xPos][*yPos] == kSquareEmpty)
        Max = 4;

    for (I = 0; I < kGameBoardSize; I++) {
        for (J = 0; J < kGameBoardSize; J++) {
            if (gGameBoard[I][J] == kSquareEmpty) {
                Valu = Value[I][J][Player - kSquareCross] * (AttackFactor + 16) / 16 +
                       Value[I][J][Opponent - kSquareCross] +
                       (int)(rand() % 4 + 1);
                if (Valu > Max) {
                    *xPos = I;  *yPos = J;  Max = Valu;
                }
            }
        }
    }
}

/*--------------------------------------------------------------------------*/
static void ResetGame(void)
{
    int I, J, C, D;

    WinningLine = 0;
    GameWon     = FALSE;
    autoPlay    = FALSE;

    for (I = 0; I < kGameBoardSize; I++) {
        for (J = 0; J < kGameBoardSize; J++) {
            gGameBoard[I][J] = 0;
            for (C = 0; C < 2; C++) {
                Value[I][J][C] = 0;
                for (D = 0; D < NR_LINES; D++)
                    Line[D][I][J][C] = 0;
            }
        }
    }
    Player = kSquareCross;
    TotalLines = (long)(kGameBoardSize * (kGameBoardSize - 4) +
                 (kGameBoardSize - 4) * (kGameBoardSize - 4)) * 4;
    WinInvalidateRect(hwndClient, (RECTL *)0, TRUE);
}

/*--------------------------------------------------------------------------*/
static BOOL GameOver(void)
{
    return (GameWon || TotalLines <= 0);
}

/*--------------------------------------------------------------------------*/
static void ProgramMove(void)
{
    int X, Y;
    FindMove(&X, &Y);
    MakeMove(X, Y);
    if (GameWon) {
        WinMessageBox(HWND_DESKTOP, hwndClient,
                      tr(STR_MSG_IWON), tr(STR_TITLE_IWON),
                      0, MB_OK | MB_APPLMODAL | MB_INFORMATION);
    }
}

/*--------------------------------------------------------------------------*/
static VOID DrawPiece(HPS hps, RECTL *rcl, SquareEnumType C)
{
    POINTL ptl;
    LONG   Clr = (C == kSquareCross) ? CLR_GREEN : CLR_RED;

    ptl.x = rcl->xLeft + 4;
    ptl.y = rcl->yTop  - 4;
    GpiMove(hps, &ptl);
    ptl.x = rcl->xRight  - 4;
    ptl.y = rcl->yBottom + 4;
    GpiSetColor(hps, Clr);
    GpiBox(hps, DRO_FILL, &ptl, 8, 8);
}

/*--------------------------------------------------------------------------*/
static void set_language(int lang)
{
    HWND   hMenu, hSub;
    MENUITEM mi;
    int    i;

    if (lang < 0 || lang >= LANG_COUNT) lang = LANG_EN;
    current_lang = lang;

    hMenu = WinWindowFromID(hwndFrame, FID_MENU);
    if (!hMenu) return;

    WinSendMsg(hMenu, MM_SETITEMTEXT,
               MPFROMSHORT(IDM_SUBMENU_GAME),    MPFROMP(tr(STR_MENU_GAME)));
    WinSendMsg(hMenu, MM_SETITEMTEXT,
               MPFROMSHORT(IDM_SUBMENU_OPTIONS), MPFROMP(tr(STR_MENU_OPTIONS)));
    WinSendMsg(hMenu, MM_SETITEMTEXT,
               MPFROMSHORT(IDM_SUBMENU_HELP),    MPFROMP(tr(STR_MENU_HELP)));

    memset(&mi, 0, sizeof(mi));
    WinSendMsg(hMenu, MM_QUERYITEM,
               MPFROM2SHORT(IDM_SUBMENU_GAME, TRUE), MPFROMP(&mi));
    hSub = mi.hwndSubMenu;
    if (hSub) {
        WinSendMsg(hSub, MM_SETITEMTEXT,
                   MPFROMSHORT(IDM_NEW),  MPFROMP(tr(STR_MENU_NEW)));
        WinSendMsg(hSub, MM_SETITEMTEXT,
                   MPFROMSHORT(IDM_EXIT), MPFROMP(tr(STR_MENU_EXIT)));
    }

    memset(&mi, 0, sizeof(mi));
    WinSendMsg(hMenu, MM_QUERYITEM,
               MPFROM2SHORT(IDM_SUBMENU_OPTIONS, TRUE), MPFROMP(&mi));
    hSub = mi.hwndSubMenu;
    if (hSub) {
        WinSendMsg(hSub, MM_SETITEMTEXT,
                   MPFROMSHORT(IDM_KEYMAP),       MPFROMP(tr(STR_MENU_KEYMAP)));
        WinSendMsg(hSub, MM_SETITEMTEXT,
                   MPFROMSHORT(IDM_SUBMENU_LANG), MPFROMP(tr(STR_MENU_LANGUAGE)));
        WinSendMsg(hSub, MM_SETITEMTEXT,
                   MPFROMSHORT(IDM_SAVEONEXIT),   MPFROMP(tr(STR_MENU_SAVEONEXIT)));
        WinSendMsg(hSub, MM_SETITEMTEXT,
                   MPFROMSHORT(IDM_FRAME),        MPFROMP(tr(STR_MENU_FRAME)));

        memset(&mi, 0, sizeof(mi));
        WinSendMsg(hSub, MM_QUERYITEM,
                   MPFROM2SHORT(IDM_SUBMENU_LANG, TRUE), MPFROMP(&mi));
        if (mi.hwndSubMenu) {
            for (i = 0; i < LANG_COUNT; i++)
                WinCheckMenuItem(mi.hwndSubMenu, IDM_LANG_EN + i, (i == lang));
        }
    }
}

/*--------------------------------------------------------------------------*/
static void toggle_frame(void)
{
    RECTL rcl;
    SWP   swp;
    LONG  cw, ch, nw, nh;
    HWND  hMenu;

    /* the client area keeps its size: no gray bar where the title bar was */
    WinQueryWindowRect(hwndClient, &rcl);
    cw = rcl.xRight - rcl.xLeft;
    ch = rcl.yTop   - rcl.yBottom;

    if (!bFrameHidden) {
        WinSetParent(hwndTitleBar, HWND_OBJECT, FALSE);
        WinSetParent(hwndSysMenu,  HWND_OBJECT, FALSE);
        WinSetParent(hwndMinMax,   HWND_OBJECT, FALSE);
        WinSetParent(hwndMenuBar,  HWND_OBJECT, FALSE);
        bFrameHidden = TRUE;
    } else {
        /* PM clears WS_VISIBLE on a reparent to the object window */
        WinSetParent(hwndTitleBar, hwndFrame, FALSE);
        WinSetParent(hwndSysMenu,  hwndFrame, FALSE);
        WinSetParent(hwndMinMax,   hwndFrame, FALSE);
        WinSetParent(hwndMenuBar,  hwndFrame, FALSE);
        WinShowWindow(hwndTitleBar, TRUE);
        WinShowWindow(hwndSysMenu,  TRUE);
        WinShowWindow(hwndMinMax,   TRUE);
        WinShowWindow(hwndMenuBar,  TRUE);
        bFrameHidden = FALSE;
    }
    WinSendMsg(hwndFrame, WM_UPDATEFRAME,
               (MPARAM)(FCF_TITLEBAR | FCF_SYSMENU | FCF_MINBUTTON | FCF_MENU), NULL);

    /* resize the frame around the unchanged client, keeping its top left corner */
    rcl.xLeft = 0;  rcl.yBottom = 0;  rcl.xRight = cw;  rcl.yTop = ch;
    WinCalcFrameRect(hwndFrame, &rcl, FALSE);
    nw = rcl.xRight - rcl.xLeft;
    nh = rcl.yTop   - rcl.yBottom;
    WinQueryWindowPos(hwndFrame, &swp);
    WinSetWindowPos(hwndFrame, HWND_TOP, swp.x, swp.y + swp.cy - nh, nw, nh,
                    SWP_MOVE | SWP_SIZE);
    WinInvalidateRect(hwndFrame, NULL, TRUE);
    WinUpdateWindow(hwndFrame);

    hMenu = WinWindowFromID(hwndFrame, FID_MENU);
    if (hMenu)
        WinCheckMenuItem(hMenu, IDM_FRAME, bFrameHidden);
}

/*--------------------------------------------------------------------------*/
static void load_settings(void)
{
    ULONG ul, cb;

    ul = 0;  cb = sizeof(ul);
    if (PrfQueryProfileData(HINI_USERPROFILE, "Gomoku", "Language", &ul, &cb))
        current_lang = (int)ul;
    if (current_lang < 0 || current_lang >= LANG_COUNT) current_lang = LANG_EN;

    ul = 1;  cb = sizeof(ul);                      /* on by default */
    if (PrfQueryProfileData(HINI_USERPROFILE, "Gomoku", "SaveOnExit", &ul, &cb))
        bSaveOnExit = (ul != 0);

    ul = 1;  cb = sizeof(ul);
    if (PrfQueryProfileData(HINI_USERPROFILE, "Gomoku", "Button", &ul, &cb))
        btn_place = (int)ul;
    if (btn_place < 1 || btn_place > 3) btn_place = 1;
}

/* the "Save settings on exit" choice itself is always remembered */
static void save_saveonexit(void)
{
    ULONG ul = bSaveOnExit ? 1 : 0;
    PrfWriteProfileData(HINI_USERPROFILE, "Gomoku", "SaveOnExit", &ul, sizeof(ul));
}

static void save_settings(void)
{
    ULONG ul;

    ul = (ULONG)current_lang;
    PrfWriteProfileData(HINI_USERPROFILE, "Gomoku", "Language", &ul, sizeof(ul));
    ul = (ULONG)btn_place;
    PrfWriteProfileData(HINI_USERPROFILE, "Gomoku", "Button", &ul, sizeof(ul));
    save_saveonexit();
}

/*--------------------------------------------------------------------------*/
/* Key Mapping: the mouse button that places a piece                         */
#define WM_KM_INPUT  (WM_USER + 2)

static PFNWP pfnKmEntry;

static int BtnOf(ULONG msg)
{
    switch (msg) {
        case WM_BUTTON1DOWN: case WM_BUTTON1DBLCLK: return 1;
        case WM_BUTTON2DOWN: case WM_BUTTON2DBLCLK: return 2;
        case WM_BUTTON3DOWN: case WM_BUTTON3DBLCLK: return 3;
    }
    return 0;
}

/* Capture box: the first click only focuses it, a click on the focused box
 * assigns the mouse button used */
static MRESULT EXPENTRY KmEntryProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    SHORT btn = 0;

    switch (msg) {
        case WM_BUTTON1DOWN: btn = 1; break;
        case WM_BUTTON2DOWN: btn = 2; break;
        case WM_BUTTON3DOWN: btn = 3; break;
        case WM_CHAR:
            if (!(SHORT1FROMMP(mp1) & KC_KEYUP) &&
                (SHORT1FROMMP(mp1) & KC_VIRTUALKEY) &&
                (SHORT2FROMMP(mp2) == VK_TAB   || SHORT2FROMMP(mp2) == VK_BACKTAB ||
                 SHORT2FROMMP(mp2) == VK_ENTER || SHORT2FROMMP(mp2) == VK_NEWLINE ||
                 SHORT2FROMMP(mp2) == VK_ESC))
                break;                      /* the dialog handles these */
            return (MRESULT)TRUE;           /* swallow every other key  */
    }
    if (btn && WinQueryFocus(HWND_DESKTOP) == hwnd) {
        WinPostMsg(WinQueryWindow(hwnd, QW_PARENT), WM_KM_INPUT,
                   MPFROMLONG(btn), MPVOID);
        return (MRESULT)TRUE;
    }
    return pfnKmEntry(hwnd, msg, mp1, mp2);
}

MRESULT EXPENTRY KeyMapDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    static int tmp;

    switch (msg) {
        case WM_INITDLG:
            tmp = btn_place;
            WinSetWindowText(hwnd, tr(STR_KM_TITLE));
            WinSetDlgItemText(hwnd, IDC_KM_HINT,    tr(STR_KM_HINT));
            WinSetDlgItemText(hwnd, IDC_KM_LABEL,   tr(STR_KM_ACT0));
            WinSetDlgItemText(hwnd, DID_OK,         tr(STR_DLG_OK));
            WinSetDlgItemText(hwnd, DID_CANCEL,     tr(STR_DLG_CANCEL));
            WinSetDlgItemText(hwnd, IDC_KM_DEFAULT, tr(STR_KM_DEFAULT));
            pfnKmEntry = WinSubclassWindow(WinWindowFromID(hwnd, IDC_KM_ENTRY), KmEntryProc);
            WinSetDlgItemText(hwnd, IDC_KM_ENTRY, tr(STR_KM_BTN1 + tmp - 1));
            return (MRESULT)FALSE;

        case WM_KM_INPUT:
            tmp = (int)LONGFROMMP(mp1);
            WinSetDlgItemText(hwnd, IDC_KM_ENTRY, tr(STR_KM_BTN1 + tmp - 1));
            return 0;

        case WM_COMMAND:
            switch (COMMANDMSG(&msg)->cmd) {
                case DID_OK:
                    btn_place = tmp;
                    WinDismissDlg(hwnd, TRUE);
                    return 0;
                case DID_CANCEL:
                    WinDismissDlg(hwnd, FALSE);
                    return 0;
                case IDC_KM_DEFAULT:
                    tmp = 1;
                    WinSetDlgItemText(hwnd, IDC_KM_ENTRY, tr(STR_KM_BTN1));
                    return 0;
            }
            break;
    }
    return WinDefDlgProc(hwnd, msg, mp1, mp2);
}

/*--------------------------------------------------------------------------*/
MRESULT EXPENTRY AboutDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    switch (msg) {
        case WM_COMMAND:
            switch (COMMANDMSG(&msg)->cmd) {
                case DID_OK:
                case DID_CANCEL:
                    WinDismissDlg(hwnd, TRUE);
                    return (MRESULT)0;
            }
            break;
        default:
            return WinDefDlgProc(hwnd, msg, mp1, mp2);
    }
    return (MRESULT)0;
}

/*--------------------------------------------------------------------------*/
MRESULT EXPENTRY ClientWndProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    HPS          hps;
    RECTL        rcl;
    POINTL       ptl;
    SHORT        x, y;

    switch (msg) {
        case WM_CREATE:
            ResetGame();
            return 0;

        case WM_COMMAND:
            switch (SHORT1FROMMP(mp1)) {
                case IDM_NEW:
                    ResetGame();
                    break;
                case IDM_SAVEONEXIT:
                    bSaveOnExit = !bSaveOnExit;
                    WinCheckMenuItem(
                        WinWindowFromID(hwndFrame, FID_MENU),
                        IDM_SAVEONEXIT, bSaveOnExit);
                    save_saveonexit();
                    break;
                case IDM_KEYMAP:
                    WinDlgBox(HWND_DESKTOP, hwndFrame,
                              KeyMapDlgProc, (HMODULE)0,
                              IDD_KEYMAP, NULL);
                    break;
                case IDM_FRAME:
                    toggle_frame();
                    break;
                case IDM_LANG_EN:
                case IDM_LANG_ES:
                case IDM_LANG_NL:
                case IDM_LANG_DE:
                case IDM_LANG_FR:
                case IDM_LANG_IT:
                    set_language(SHORT1FROMMP(mp1) - IDM_LANG_EN);
                    break;
                case IDM_ABOUT:
                    WinDlgBox(HWND_DESKTOP, hwndFrame,
                              AboutDlgProc, (HMODULE)0,
                              IDD_ABOUT, NULL);
                    break;
                case IDM_EXIT:
                    WinPostMsg(hwndFrame, WM_QUIT, 0, 0);
                    break;
            }
            return (MRESULT)0;

        case WM_BUTTON1DOWN:
        case WM_BUTTON1DBLCLK:
        case WM_BUTTON2DOWN:
        case WM_BUTTON2DBLCLK:
        case WM_BUTTON3DOWN:
        case WM_BUTTON3DBLCLK:
            if (BtnOf(msg) != btn_place)            /* not the mapped button */
                break;
            x = SHORT1FROMMP(mp1) / BITMAPSIZE;
            y = SHORT2FROMMP(mp1) / BITMAPSIZE;
            if (x < kGameBoardSize && y < kGameBoardSize) {
                if (gGameBoard[x][y] == kSquareEmpty && !GameWon) {
                    MakeMove(x, y);
                    if (GameWon)
                        WinMessageBox(HWND_DESKTOP, hwnd,
                                      tr(STR_MSG_YOUWON), tr(STR_TITLE_YOUWON),
                                      0, MB_OK | MB_APPLMODAL | MB_INFORMATION);
                    else
                        ProgramMove();
                } else {
                    DosBeep(880, 40);
                }
            }
            break;

        case WM_PAINT:
            hps = WinBeginPaint(hwnd, (HPS)0, &rcl);
            WinFillRect(hps, &rcl, CLR_PALEGRAY);
            WinQueryWindowRect(hwnd, &rcl);
            for (x = 0; x < kGameBoardSize; x++) {
                for (y = 0; y < kGameBoardSize; y++) {
                    rcl.xRight = BITMAPSIZE + (rcl.xLeft   = x * BITMAPSIZE);
                    rcl.yTop   = BITMAPSIZE + (rcl.yBottom = y * BITMAPSIZE);
                    GpiSetColor(hps, CLR_BLUE);
                    ptl.x = rcl.xLeft + 2;  ptl.y = rcl.yBottom + 2;
                    GpiMove(hps, &ptl);
                    ptl.x = rcl.xRight - 3;  ptl.y = rcl.yTop - 3;
                    GpiBox(hps, DRO_OUTLINE, &ptl, 4, 4);
                    if (gGameBoard[x][y] != kSquareEmpty)
                        DrawPiece(hps, &rcl, gGameBoard[x][y]);
                }
            }
            WinEndPaint(hps);
            return 0;
    }
    return WinDefWindowProc(hwnd, msg, mp1, mp2);
}

/*--------------------------------------------------------------------------*/
int main(void)
{
    static ULONG flFlags = FCF_TITLEBAR   | FCF_SYSMENU  |
                           FCF_DLGBORDER  | FCF_MINBUTTON |
                           FCF_SHELLPOSITION | FCF_TASKLIST |
                           FCF_ICON       | FCF_MENU     | FCF_ACCELTABLE;
    HAB  hab;
    HMQ  hmq;
    QMSG qmsg;
    RECTL rcl;

    srand((unsigned)time(NULL));

    hab = WinInitialize(0);
    hmq = WinCreateMsgQueue(hab, 0);

    WinRegisterClass(hab, (PSZ)szClientClass,
                     (PFNWP)ClientWndProc,
                     CS_SIZEREDRAW, 0);

    hwndFrame = WinCreateStdWindow(HWND_DESKTOP, 0L,
                                   &flFlags,
                                   szClientClass,
                                   "Gomoku",
                                   0L,
                                   (HMODULE)0L,
                                   ID_RESOURCE,
                                   &hwndClient);

    /* Capture frame control handles (must be after WinCreateStdWindow) */
    hwndTitleBar = WinWindowFromID(hwndFrame, FID_TITLEBAR);
    hwndSysMenu  = WinWindowFromID(hwndFrame, FID_SYSMENU);
    hwndMinMax   = WinWindowFromID(hwndFrame, FID_MINMAX);
    hwndMenuBar  = WinWindowFromID(hwndFrame, FID_MENU);

    /* Apply persisted settings */
    load_settings();
    set_language(current_lang);
    WinCheckMenuItem(hwndMenuBar, IDM_SAVEONEXIT, bSaveOnExit);

    /* Size window to fit the board exactly */
    rcl.xLeft = 0;  rcl.yBottom = 0;
    rcl.xRight = kGameBoardSize * BITMAPSIZE;
    rcl.yTop   = kGameBoardSize * BITMAPSIZE;
    WinCalcFrameRect(hwndFrame, &rcl, FALSE);

    {
        LONG cxScreen = WinQuerySysValue(HWND_DESKTOP, SV_CXSCREEN);
        LONG cyScreen = WinQuerySysValue(HWND_DESKTOP, SV_CYSCREEN);
        LONG winW = rcl.xRight - rcl.xLeft;
        LONG winH = rcl.yTop   - rcl.yBottom;
        LONG x    = (cxScreen - winW) / 2;
        LONG y    = (cyScreen - winH) / 2;
        WinSetWindowPos(hwndFrame, HWND_TOP, x, y, winW, winH,
                        SWP_SIZE | SWP_MOVE | SWP_ACTIVATE | SWP_SHOW);
    }

    while (WinGetMsg(hab, &qmsg, 0L, 0, 0))
        WinDispatchMsg(hab, &qmsg);

    if (bSaveOnExit)
        save_settings();

    WinDestroyWindow(hwndFrame);
    WinDestroyMsgQueue(hmq);
    WinTerminate(hab);
    return 0;
}
