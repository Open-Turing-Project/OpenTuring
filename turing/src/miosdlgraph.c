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

// Test to make certain we're not accidentally including <windows.h> which
// might allow for windows contamination of platform independent code.
#ifdef _INC_WINDOWS
xxx
#endif

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/
#define MIO_SDL_WINDOW_TITLE "Turing SDL Window"

/********************/
/* Global variables */
/********************/

static bool mioSDLGraphWinOpen = false;

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

extern void	MIOSDLGraph_NewWin (OOTint width,OOTint height)
{
	if(!mioSDLGraphWinOpen){
		SDL_Init( SDL_INIT_VIDEO );

		SDL_Surface* screen = SDL_SetVideoMode( width, height, 0, SDL_HWSURFACE | SDL_DOUBLEBUF );
		SDL_WM_SetCaption( MIO_SDL_WINDOW_TITLE, 0 );

		mioSDLGraphWinOpen = true;
	}
}