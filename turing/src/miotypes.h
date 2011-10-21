/**************/
/* miotypes.h */
/**************/

#ifndef _MIOTYPES_H_
#define _MIOTYPES_H_

/*******************/
/* System includes */
/*******************/

/******************/
/* Other includes */
/******************/
#include "language.h"
#include "levent.h"

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/
#define NULL		((void *)0)
#define TRUE		1
#define FALSE		0
#define EOF		(-1)

#define STRLEN		256

// Used in MIOWinInfo
#define INPUT_BUFFER_SIZE	     1024

/*********/
/* Types */
/*********/
typedef int		BOOL;
typedef unsigned char	UCHAR;
typedef unsigned char	BYTE;
typedef unsigned short	WORD;
typedef unsigned short	USHORT;
typedef unsigned long	DWORD;
typedef unsigned int	UINT;

typedef UINT		WPARAM;
typedef long		LPARAM;

typedef void		*COLOR;
typedef void		*DEVCONTEXT;
typedef void		*DIR;
typedef void		*FILESPEC;
typedef void		*FONT;
typedef void		*INSTANCE;
typedef void		*MENU;
typedef void		*MYBITMAP;
typedef void		*PALETTE;
typedef void		*PIC;
typedef void		*PICBUFFER;
typedef void		*SPRITELIST;
typedef void		*REGION;
typedef void		*TEXT;
typedef void		*TW_OOT_EVENT;
typedef void		*WIND;

typedef struct 
{
    int left; 
    int top; 
    int right; 
    int bottom;
} MYRECT;

typedef struct MYRECTLIST
{
    MYRECT		rect;
    struct MYRECTLIST	*next;
} MYRECTLIST;

typedef struct
{
    int	x, y;
    int button;
    int buttonDirection;
} MOUSEPOINT;

typedef struct
{
    // Type of window (one of TEXT_MODE or GRAPHICS_MODE)
    int		windowKind;
    
    // The Turing window ID
    int		turingWindowID;
    void	*turingMIOFilePtr;

    // The status of the caret in the window
    BOOL	caretShowing;

    // The outer text/graphics window  (window including button bar)
    WIND	outerWindow;

    // The inner text/graphics window  (w/o button bar, with scroll bars)
    WIND	innerWindow;

    // The button bar window
    WIND	buttonBarWindow;

    // The order that the run windows should be displayed.  Run windows with a
    // zOrder of 0 are placed in whatever order the user puts them in.  Run
    // windows with a zOrder that is positive are always placed above Run 
    // windows with a lower zOrder.
    int		zOrder;

    // The name of the program being executed. This is used to construct a
    // window title if the window is not otherwise named.  Otherwise it is
    // set to NULL.
    char	title [256];
    
    MYRECT	oldWindowRect;

    // The font size in pixels
    int		fontCharWidth, fontCharHeight;
    
    // The font's ascent (used in MIODraw_Text)
    int		fontCharAscent, fontCharDescent, fontCharIntLeading;

    FONT	font;

    COLOR	selectionColour;

    int		visibility;

    // You may close this window without halting program execution.
    BOOL	okayToClose;

    // This window is in the process of closing.  In doing so, it may
    // terminate execution of the program which in turn closes all the
    // run windows.  This prevents the window being closed twice.
    BOOL	isClosing;

    //
    // Initial attributes
    //
    
    // The initial position specified (used when resizing the
    // window without specifying its new location
    int		initXPos, initYPos;
    int		initRows, initColumns;
    int		initXSize, initYSize;
    
    //
    // Input related fields
    //
    
    // The unprocessed keypresses
    UCHAR	kbdBuffer [INPUT_BUFFER_SIZE];
    int		kbdBufferHead, kbdBufferTail;

    UCHAR	lineBuffer [INPUT_BUFFER_SIZE];
    int		lineBufferHead, lineBufferTail;
    int		lineBufferLastNewLine;

    // The keys currently held down
    BYTE	pressedKeys [256];

    // This variable indicates if the cursor has moved because of output or
    // a cursor move since we started receiving input.
    BOOL	outputSinceLastInput;
    
    // This variable indicates if the cursor has moved because of output or
    // a cursor move since we received our last input char.
    BOOL	outputSinceLastInputChar;
    
    // These two variables indicate where the start of the last continuous
    // string of input occurred. For Text windows, only startInputRow is used
    // and it specifies the text location.  These are used so that if it is
    // necessary to delete all the input a user has typed, you can either
    // delete from the start of the last input, or, if there has been program
    // output since the start of the last input (which can occur if there's 
    // threads), then only delete from the text of the last output.  That way,
    // you never actually have any output deleted.
    int		startInputRow, startInputCol;
    
    // Is the program waiting input at the moment
    BOOL	acceptingInput;

    // Program has reached an EOF in user input.
    BOOL	eofAlreadyEntered;

    // Is input being redirected from a file
    BOOL	inputFromFile;
    
    // When there is no more line input send an empty string
    BOOL	eofOnDeck;

    // Is the waiting for input message displayed in the window title bar
    BOOL	waitingForInputMessageDisplayed;

    // The ungot char.  The char placed back in the line buffer queue
    int		ungetChar;
    
    // Is input echoed to the screen
    BOOL	echo;

    // Is cursor visible
    BOOL	cursor;

    // Controls window title
    int		waitingForMouseClick;

    //
    // Graphics Mode Variables
    //
    
    // The actual width/height of the drawing surface.
    long	width, height;
    // The actual width/height of the child graphics window (includes margins)
    long	graphicsRunWindowWidth, graphicsRunWindowHeight;
    // The minimum allowable run window width and height
    long	minWindowWidth, minWindowHeight;
    // The initial and maximum run window width and height
    long	maxWindowWidth, maxWindowHeight;
    // The initial run window position
    long	initWindowTop, initWindowLeft;
    // The displacement of the top/left corner due to scrolling in window
    long	leftX, topY;
    // The number of colours in the palette
    long	numColours;
    // The number of slots in the palette.  (i.e. use only x of y pal. entries)
    long	numPaletteSlots;
    // The transparent colour
    int		transparentColour;

    // Draw in xor
    BOOL	xor;

    // Use the MS-DOS character set when drawing
    BOOL	msdosCharSet;

    // Draw to screen window and offscreen window, or just offscreen window?
    BOOL	displayOnScreen;

    // The pixel graphics
    DEVCONTEXT	deviceContext;		// was hdc
    DEVCONTEXT	offscreenDeviceContext; // was hdcMem
    MYBITMAP	offscreenBitMap;	// was hBitmap

    REGION	clippingRegion;

    // The unprocessesed mouse presses
    MOUSEPOINT	mouseBuffer [INPUT_BUFFER_SIZE];
    int		mouseBufferHead, mouseBufferTail;

    // The current row and column (can exceed maxRow at the end of the line)
    int		row, col;
    // The actual row and column that the cursor appears on
    int		actualRow, actualCol;
    // The maximum number of rows and columns in the window
    int		maxRow, maxCol;
    // Flag to indicate if echoing a newline should clear to end of line
    BOOL	clearToEndOfLine;
    
    int		textColour, textBGColour;
    
    // Cached GDI Information
    int		cachedPenColour, cachedPenWidth, cachedPenStyle;
    int		cachedBrushColour;

    PALETTE	palette;
    BOOL	usingDefaultPalette;
    
    //    
    // Text Mode Variables
    //
    TEXT	text;
    
    // The rectange in the inner window containing all the text
    MYRECT	textRect;
    
    int		accumulateDelta; // For the wheel mouse

    int		topLine; 	// Top visible line (0-based)
    int		leftCol;	// The left most visible char (0-based)

    // These two variables indicate where the start of the last input occurred
    // and the end of the last output.  These are used so that if it is
    // necessary to delete all the input a user has typed, you can either
    // delete from the start of the last input, or, if there has been program
    // output since the start of the last input (which can occur if there's 
    // threads), then only delete to the text of the last output.  That way,
    // you never actually have any output deleted.
//    int		textStartPosOfLastInput;
//    int		textEndPosOfLastOutput;

    int		windowWidth;	// Window width in characters
    int		windowHeight;	// Window height in characters

    // Used for mouse selection
    int		lastMouseLine, lastMouseCol;
    int		horizontalTicks, verticalTicks;
    BOOL	selectingWithMouse;
    int		numClicks, lastClickTime;
    BOOL	clickOnSameChar;

    // Used for sprites
    BOOL	hasSprites;
    SPRITELIST	spriteQueueHead;
    // Contains all the non-sprite drawing.  The sprites are then added
    // to this context to create the actual window seen by user.
    DEVCONTEXT	spriteDeviceContext;
    MYBITMAP	spriteBitMap;		// was hBitmap
    MYRECTLIST	*dirtyRectHead;		// Stored in Turing coordinates
} MIOWindowInfo, *MIOWinInfoPtr;
    

/**********************/
/* External variables */
/**********************/

/***********************/
/* External procedures */
/***********************/

#endif // #ifndef _MIOTYPES_H_
