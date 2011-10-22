/*******************/
/* System includes */
/*******************/
#include "SDL.h"
#include "SDL_gfxPrimitives.h" 

/****************/
/* Self include */
/****************/
#include "miosdlgraph.h"

/******************/
/* Other includes */
/******************/
#include "mio.h"
#include "mdio.h"

#include "mioerr.h"

// Test to make certain we're not accidentally including <windows.h> which
// might allow for windows contamination of platform independent code.
#ifdef _INC_WINDOWS
xxx
#endif

/**********/
/* Macros */
/**********/

#define CAP8(x) ((Uint8)(((x) < 255 && (x) >= 0) ? (x) : 255))

/*************/
/* Constants */
/*************/
#define MIO_SDL_WINDOW_TITLE "Turing SDL Window"

/********************/
/* Global variables */
/********************/

static int stSDLWinOpen;
static SDL_Surface* stSDLScreen;

/*********/
/* Types */
/*********/

/**********************/
/* External variables */
/**********************/

/********************/
/* Static constants */
/********************/

/********************/
/* Static variables */
/********************/

/******************************/
/* Static callback procedures */
/******************************/

/*********************/
/* Static procedures */
/*********************/


/***********************/
/* External procedures */
/***********************/

extern void	MIOSDLGraph_InitRun () {
	stSDLWinOpen = FALSE;
}

extern void	MIOSDLGraph_NewWin (OOTint width,OOTint height,OOTint winMode)
{
	int mode;
	if(!stSDLWinOpen){
		// Initializes the video subsystem
		if (SDL_Init(SDL_INIT_VIDEO) < 0) {
			char	myMessage [1024];    	
    		MDIO_sprintf (myMessage, "Unable to init SDL: %s", 
    				  SDL_GetError());
    		ABORT_WITH_ERRMSG (E_FONT_BAD_FONT_SELECT_STR, myMessage);
		}
		mode = ((winMode == 2) ? SDL_FULLSCREEN : 0);
		stSDLScreen = SDL_SetVideoMode( width, height, 0, SDL_HWSURFACE | SDL_DOUBLEBUF | mode );
		SDL_WM_SetCaption( MIO_SDL_WINDOW_TITLE, 0 );

		stSDLWinOpen = TRUE;
	}
}

extern void	MIOSDLGraph_CloseWin ()
{
	if(stSDLWinOpen){
		SDL_Quit();
		stSDLWinOpen = FALSE;
	}
}

extern void	MIOSDLGraph_Update ()
{
	if(stSDLWinOpen){
		SDL_Flip(stSDLScreen);
	}
}

extern void	MIOSDLGraph_Line (OOTint x1,OOTint y1,OOTint x2,OOTint y2,
								OOTint r, OOTint g, OOTint b, OOTint a)
{
	if(stSDLWinOpen){
		lineRGBA(stSDLScreen,
               (Sint16)x1, (Sint16)y1,
               (Sint16)x2, (Sint16)y2,
               CAP8(r), CAP8(g), CAP8(b), CAP8(a));
	}
}