/*--------------------------------------------------------------------------*/
/* Source : gomoku.c for OS/2 Warp.                                         */
/*                                                                          */
/* Author : Jasper de Keijzer (Game part ported from old pascal source )    */
/*                                                                          */
/* change --- date ----who----Description-----------------------------------*/
/*   1       040328    JdK    Algoritm problem solved for diagonal lines.   */
/*--------------------------------------------------------------------------*/
#define INCL_WIN
#define INCL_GPI
#define INCL_WINHEAP
#define INCL_WINDIALOGS
#define INCL_DOSPROCESS

#define DIVISIONS 19

#include <os2.h>
#include <stdlib.h>
#include "gomoku.h"

MRESULT EXPENTRY ClientWndProc (HWND, USHORT, MPARAM, MPARAM);

HWND hwndFrame, hwndClient;

CHAR szClientClass[]    = "GOMOKU";
static CHAR szHelp[] = "Try to put 5 pieces in a row on the board.\n"
                       "This may be done in any direction\n"
                       "The computer plays with red.\n\n"
                       "Have fun, Jasper de Keijzer.";

#define BITMAPSIZE  20   /* pixels...*/
/* macro to get a random integer within a specified range */
#define randint(min,max) ( ( rand() % (int)( (max+1 - min) + min)))

#define kGameBoardSize  18

#define Horiz     1
#define DownLeft  2
#define DownRight 3
#define Vert      4

#define NR_LINES  5
/***********************************************************************
 *   Types
 ***********************************************************************/

typedef enum {
    kStateIdle 	= 0,     // No game in progress
    kStateIWon,    
    kStateYouWon  
} StateEnumType, *StateEnumPtr;

typedef enum {  
    kSquareEmpty    = 0,
    kSquareCross   = 1, // The initiator plays with circles
    kSquareNought   = 2, // The acceptor plays with crosses
    kSquareTemporary= 4  // Temporary visual effect
} SquareEnumType, *SquareEnumPtr;
/***********************************************************************
 *   Global variables
 ***********************************************************************/
StateEnumType   gState; // Current state, as defined above
SquareEnumType  gGameBoard [kGameBoardSize+5][kGameBoardSize+5]; // The game board

int Player;          /* The player whose move is next */
long TotalLines;     /* The number of empty lines left */
BOOL GameWon;     /* Set if one of the players has won */
BOOL autoPlay;
char IndexType;             /* Index to the board */
int Line[NR_LINES][kGameBoardSize][kGameBoardSize][2];       /* Value of square for each player */
int Value[kGameBoardSize][kGameBoardSize][2];
int AttackFactor;
int WinningLine;
int xWinPos,yWinPos;        /* Winning possition for showing the winning line*/  
/* Value of having 0, 1,2,3,4 or 5 pieces in line */
long Weight[] = {  0, 0, 4, 20, 100, 500, 0 };    



/* macro to get a random integer within a specified range */
#define randint(min,max) ( ( rand() % (int)( (max+1 - min) + min)))
typedef char MaxString[256];

/*--------------------------------------------------------------------------*/
static int opponentColor(void)
{
    if (Player == kSquareCross)
        return kSquareNought;
    else
        return kSquareCross;
}
/*--------------------------------------------------------------------------*/
static int valueAdd( int Num )
{ 
  Num++;
  if ( Num == 1 )
     TotalLines--;
  if ( Num >= 5)
     GameWon = TRUE;  
  return Num;
}
/*--------------------------------------------------------------------------*/
/* globalUpdate															    */
/*																			*/
/* Description : Called from an inner loop                                  */
/*																			*/
/*         for (L = 0; L < 5; L++)									        */
/*               /* Update value for the 5 squares in the line 				*/
/*                globalUpdate(0,X1,Y1+L,X1 + L,Y1,Opponent);				*/
/*																			*/
/* X1 & Y1 starts from tap point and is in/de creased 5 times.              */
/* 																			*/
/* example : tap point 10,10                                                */
/* x1 & y1 become for diagonal lines 10,9,8,7,6 so 'b' and 'c' parameter    */
/* become this.																*/
/* For each value 5 times a globalUpdate is done							*/
/* making 'd' and 'e' for X1 = 10 ->10,11,12,13,14							*/
/*                        X1 =  9 -> 9,10,11,12,13							*/
/*						  x1 =  8 -> 8, 9 10,11,12							*/
/*																			*/
/* Line addressed by 'a' == 0                                               */
/* Suppose we play kSquareCross == 1  so Opponent = kSquareNought = 2		*/
/*																			*/
/*--------------------------------------------------------------------------*/
static void globalUpdate(int a,int b, int c, int d, int e , int Opponent)
{
  /* Updates the value of a square for each player, taking into
     account that player has placed an extra piece in the square.
     The value of a square in a usable line is Weight[Lin[Player]+1]
     where Lin[Player] is the number of pieces already placed
     in the line */

    if (Line[a][b][c][Opponent - kSquareCross] == 0)
    {
	  /* 
	   * If the opponent has no pieces in the line, then simply
	   * update the value for player, by adding the new weight and
	   * removing the previous one....?
	   */    	
        Value[d][e][Player - kSquareCross] +=  Weight[Line[a][b][c][Player - kSquareCross] + 1] - 
                                               Weight[Line[a][b][c][Player - kSquareCross]];
    }
    else if (Line[a][b][c][Player - kSquareCross] == 1)
    {
        /* 
        ** If it is the first piece in the line, then the line is
        ** spoiled for the opponent 
        */
        Value[d][e][Opponent - kSquareCross] -= Weight[Line[a][b][c][Opponent - kSquareCross]];
    }
}  /* Update */
/*---------------------------------------------------------------------------*/
static void MakeMove(int X, int Y)
{
/* Performs the move X,Y for player, and updates the global variables
    (Board, Line, Value, Player, GameWon, TotalLines and the screen)   */
    int X1, Y1;
    int K, L;
    RECTL rcl;
    int Opponent = opponentColor();

    GameWon = FALSE;

    
    
    WinningLine = 0;
    
    /* Each square of the board is part of 20 different lines.
    The procedure adds one to the number of pieces in each
    of these lines. Then it updates the value for each of the 5
    squares in each of the 20 lines. Finally Board is updated, and
    the move is printed on the screen. */
    
    for (K = 0; K < 5; K++)    /* Horizontal lines, from left to right */
    {
        /*
        ** click position X uptil X - 5
        */
        X1 = X - K;   /* Calculate starting point */
        Y1 = Y;
        if (X1 >= 0) 
        {   /* 
            ** Check starting point and add one to Line.
            */
            Line[0][X1][Y1][Player - kSquareCross] = valueAdd(Line[0][X1][Y1][Player - kSquareCross]);

            if (GameWon && WinningLine == 0)   /* Save winning line */
            {
                xWinPos = X1;
                yWinPos = Y1;
                WinningLine = Horiz;
            }
            for (L = 0; L < 5; L++)
                /* Update value for the 5 squares in the line */
                globalUpdate(0,X1,Y1,X1 + L,Y1,Opponent);
        }
    }  /* for */
    
    for (K = 0; K < 5; K++) /* Diagonal lines, down right to upper left */
    {   
        X1 = X - K;
        Y1 = Y - K;
		if (X1 >= 0 && Y1 >= 0 ) 
		{
            Line[1][X1][Y1][Player - kSquareCross] = valueAdd(Line[1][X1][Y1][Player - kSquareCross]);
            if (GameWon && WinningLine == 0)   /* Save winning line */
            {
                xWinPos = X1;
                yWinPos = Y1;
                WinningLine = DownRight;
            }
            for (L = 0; L < 5; L++)
                globalUpdate(1,X1,Y1,X1 + L,Y1 + L, Opponent);
        }
    }  /* for */
    
    for (K = 0; K < 5; K++) {   /* Vertical lines, from down to up */
        X1 = X;
        Y1 = Y - K;
		if (X1 >=  0 && Y1 >= 0 ) 
        {
            Line[2][X1][Y1][Player - kSquareCross] = valueAdd(Line[2][X1][Y1][Player - kSquareCross]);
            if (GameWon && WinningLine == 0)   /* Save winning line */
            {
                xWinPos = X1;
                yWinPos = Y1;
                WinningLine = Vert;
            }
            for (L = 0; L < 5; L++)
                globalUpdate(2,X1,Y1,X1,Y1 + L,Opponent);
        }
    }  /* for */
    
    for (K = 0; K < 5; K++) {  
        /* Diagonal lines, from lower left to upper right */
        X1 = X - K;
        Y1 = Y + K;
        if (X1 >= 0 && Y1 < kGameBoardSize  ) 
        {
            Line[3][X1][Y1][Player - kSquareCross] = valueAdd(Line[3][X1][Y1][Player - kSquareCross]);
            if (GameWon && WinningLine == 0)   /* Save winning line */
            {
                xWinPos = X1;
                yWinPos = Y1;
                WinningLine = DownLeft;
            }
            for (L = 0; L < 5; L++)
            {
                globalUpdate(3,X1,Y1,X1 + L,Y1 - L,Opponent);
            }
        }
    }  /* for */


    if (Player == kSquareCross)
        gGameBoard[X][Y] = kSquareCross;   /* Place piece in board */
    else
        gGameBoard[X][Y] = kSquareNought;
    
    if (!GameWon)
        Player = Opponent;      /* The opponent is next to move */

  rcl.xRight = BITMAPSIZE + (rcl.xLeft   = X * BITMAPSIZE);
  rcl.yTop   = BITMAPSIZE + (rcl.yBottom = Y * BITMAPSIZE);
  WinInvalidateRect(hwndClient,&rcl,FALSE);
    
}  /* MakeMove */
/*--------------------------------------------------------------------------*/
void FindMove(int *xPos, int *yPos)
{
/* Finds a move X,Y for player, simply by
    picking the one with the highest value */
    int Opponent;
    int I, J;
    int Max, Valu;
    
    Opponent = opponentColor();
    Max = -10000;
    /* If no square has a high value then pick the one in the middle */
    *xPos = (kGameBoardSize + 1) / 2;
    *yPos = (kGameBoardSize + 1) / 2;
    
    
    if (gGameBoard[*xPos][*yPos] == kSquareEmpty )
        Max = 4;

        /* 
        ** The evaluation for a square is simply the value of the square
        ** for the player (attack points) plus the value for the opponent
        **(defense points). Attack is more important than defense, since
        ** it is better to get 5 in line yourself than to prevent the op-
        ** ponent from getting it. 
        */
    
    for (I = 0; I < kGameBoardSize; I++) 
    {   					/* For all empty squares */
        for (J = 0; J < kGameBoardSize; J++) 
        {
            if (gGameBoard[I][J] == kSquareEmpty) 
            {
                /* Calculate evaluation */
                Valu = Value[I][J][Player - kSquareCross] * (AttackFactor + 16) / 16 +
                    Value[I][J][Opponent - kSquareCross] + (int)randint(1,4);;
                
                if (Valu > Max) 
                {   /* Pick move with highest value */
                    *xPos = I;
                    *yPos = J;
                    Max = Valu;
                }
            }  /* if */
        }
    }
}  /* FindMove */

int main(void)
{

   static ULONG flFlags = FCF_TITLEBAR      | FCF_SYSMENU    |
                          FCF_DLGBORDER     | FCF_MINBUTTON  |
                          FCF_SHELLPOSITION | FCF_TASKLIST   |
                          FCF_MENU          | FCF_ICON;
   HAB  hab;
   HMQ  hmq;
   QMSG qmsg;
   RECTL rcl;
   static char  stdmessage[] = "This program is written by\n"
                               "- Jasper de Keijzer -.\n"
                               "The Netherlands (1996)\n";

   hab = WinInitialize (0);
   hmq = WinCreateMsgQueue(hab,0);

   WinRegisterClass( hab,
                    (PSZ)szClientClass,
                    (PFNWP)ClientWndProc,
                    CS_SIZEREDRAW,
                    (BOOL)0);

   hwndFrame = WinCreateStdWindow (HWND_DESKTOP,
                                   0,
                                   &flFlags, 
                                   szClientClass,
                                   "Gomoku",
                                   0, 
                                   (HMODULE)0L, 
                                   ID_RESOURCE, 
                                   &hwndClient);

   rcl.xLeft  = 0;
   rcl.xRight = kGameBoardSize * BITMAPSIZE;
   rcl.yBottom= 0;
   rcl.yTop   = kGameBoardSize * BITMAPSIZE;

   WinCalcFrameRect(hwndFrame,&rcl,FALSE);

   WinMessageBox( HWND_DESKTOP,
                  HWND_DESKTOP,
                  (PSZ)stdmessage,
                  (PSZ)"** GOMOKU **",
                  0,
                  MB_OK | MB_INFORMATION );



   WinSetWindowPos(hwndFrame,HWND_TOP,20,20,
                   (rcl.xRight - rcl.xLeft),
                   (rcl.yTop - rcl.yBottom),
                   SWP_MOVE | SWP_SHOW | SWP_SIZE | SWP_ZORDER);

   while (WinGetMsg (hab, &qmsg, 0L,0,0))
       WinDispatchMsg (hab,&qmsg);

   WinDestroyWindow (hwndFrame);
   WinDestroyMsgQueue ( hmq);
   WinTerminate (hab);
   return 0;

}
/*------------------------------------------------------------------------*/
/* ResetGame - cleanup the board and set gamewon to FALSE.                */
/*------------------------------------------------------------------------*/
static void ResetGame(BOOL FirstGame)
{
    int I, J;
    char D;
    int  C;
   
    WinningLine = 0;
    GameWon = FALSE;
    autoPlay= FALSE;   
    /*
    ** Clear gameboard
    */
    
    /* Resets global variables to start a new game */
    
    for (I = 0; I < kGameBoardSize; I++) 
    {
        for (J = 0; J < kGameBoardSize; J++) 
        {  
            /*
            ** Clear gameboard & Clear tables
            */
            gGameBoard[I][J] = 0;
            for (C = 0; C < 2; C++) 
            {
                Value[I][J][C] = 0;
                for (D = 0; D < NR_LINES; D++)
                    Line[D][I][J][C] = 0;
            }
        }  /* for */
    }
    Player = kSquareCross;                                       /* kSquareCross starts */
    /* Total number */
    TotalLines = (kGameBoardSize * (kGameBoardSize - 4) + (kGameBoardSize - 4) * (kGameBoardSize - 4)) * 4;   
    WinInvalidateRect(hwndClient,(RECTL *)0,TRUE);
}  /* ResetGame */
/*------------------------------------------------------------------------*/
static BOOL GameOver(void)
{
  /* A game is over if one of the players have
     won, or if there are no more empty lines */
  return (GameWon || TotalLines <= 0);
}  /* GameOver */
/*------------------------------------------------------------------------*/
static void ProgramMove(void)
{
    int X,Y;
  /* Find and perform programs move */
      FindMove(&X, &Y);
      MakeMove(X,Y);
      if (GameWon)
      {
         WinMessageBox( HWND_DESKTOP,
                        hwndClient,
                        "I won!",
                        (PSZ)"Ha Ha",
                        0,
                        MB_OK | MB_APPLMODAL | MB_INFORMATION );
      }
}  /* ProgramMove */
/*------------------------------------------------------------------------*/
static VOID DrawPiece(HPS hps,RECTL *rcl, SquareEnumType C)
{
   POINTL ptl;
   LONG   Clr;

   if (C == kSquareCross)
      Clr = CLR_GREEN;
   else
      Clr = CLR_RED;

   ptl.x = rcl->xLeft + 4;
   ptl.y = rcl->yTop  - 4;
   GpiMove (hps, &ptl);
   ptl.x = rcl->xRight  - 4;
   ptl.y = rcl->yBottom + 4;
   GpiSetColor(hps,Clr);
   GpiBox(hps, DRO_FILL, &ptl, 8, 8);
}
/*------------------------------------------------------------------------*/
MRESULT EXPENTRY ClientWndProc(HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2 )
{

    HPS          hps;
    RECTL        rcl;
    SHORT        x, y, ButtonNr;
    static SHORT CxClient, CyClient, cxChar, cyChar;
    FONTMETRICS  fm;
    static HBITMAP hbm;

   switch (msg)
   {
      case WM_CREATE:
         WinQueryWindowRect(hwnd,&rcl);
         CxClient = rcl.xRight;
         CyClient = rcl.yTop;
         ResetGame(TRUE);
         hps = WinGetPS(hwnd);
         hbm = GpiLoadBitmap(hps, (HMODULE)0,IB_BOARD, 0L, 0L);
         WinReleasePS(hps);
         return 0;
/*-----------------------------MENU----------------------------------------*/
       case WM_COMMAND:
	  switch(LOUSHORT(mp1))
	  {
             case IDM_NEW:
               ResetGame(TRUE);
               break;
             case IDM_QUIT:
               WinPostMsg(hwndFrame,WM_QUIT,0,0);
               break;
             case IDM_HELP:
               WinMessageBox( HWND_DESKTOP,
                              HWND_DESKTOP,
                              (PSZ)szHelp,
                              (PSZ)"** GOMOKU **",
                              0,
                              MB_OK | MB_APPLMODAL | MB_INFORMATION );


         }
         return (MRESULT)0;

      case WM_BUTTON1DOWN :
      case WM_BUTTON1DBLCLK:
         x = MOUSEMSG(&msg) ->x / BITMAPSIZE;
         y = MOUSEMSG(&msg) ->y / BITMAPSIZE;

         if (x < kGameBoardSize && y < kGameBoardSize)
         {
            /* Enter and make a move */
            if (gGameBoard[x][y] == kSquareEmpty && !GameWon)
            {
               MakeMove(x, y);
               if (GameWon)
                  WinMessageBox( HWND_DESKTOP,
                                 hwnd,
                                 "You won!",
                                 (PSZ)"Congratulations",
                                 0,
                                 MB_OK | MB_APPLMODAL | MB_INFORMATION );
               else
                  ProgramMove();
            }
            else
               DosBeep(880,40);
         }
         break;                                // do default processing

      case WM_PAINT:
         hps = WinBeginPaint (hwnd,(HPS)0,&rcl);
         WinFillRect(hps,&rcl,CLR_PALEGRAY);

	 WinQueryWindowRect(hwnd,&rcl);
         for ( x = 0; x < kGameBoardSize; x++ )
            for ( y = 0; y < kGameBoardSize; y++ )
            {
               rcl.xRight = BITMAPSIZE + (rcl.xLeft   = x * BITMAPSIZE);
               rcl.yTop   = BITMAPSIZE + (rcl.yBottom = y * BITMAPSIZE);
               WinDrawBitmap(hps,hbm, NULL, (PPOINTL) &rcl,
                             CLR_NEUTRAL, CLR_BACKGROUND, DBM_NORMAL );

               if (gGameBoard[x][y]!= kSquareEmpty)
               {
                  DrawPiece(hps,&rcl,gGameBoard[x][y]);
               }
            }
         WinEndPaint(hps);
         return 0;

   }
   return WinDefWindowProc (hwnd ,msg,mp1,mp2);
}
