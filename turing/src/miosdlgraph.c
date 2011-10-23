/*******************/
/* System includes */
/*******************/
#include "SDL.h"
#include "SDL_opengl.h"

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
// disabled because SDL is platform independent but it includes windows.h on windows
//xxx
#endif

/**********/
/* Macros */
/**********/

#define CAP8(x) ((GLubyte)(((x) < 255 && (x) >= 0) ? (x) : 255))

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

		mode = ((winMode == 2) ? SDL_FULLSCREEN : 0); // winMode should = 2 for fullscreen
		SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
		stSDLScreen = SDL_SetVideoMode( width, height, 0, SDL_HWSURFACE | SDL_OPENGL | mode );
		SDL_WM_SetCaption( MIO_SDL_WINDOW_TITLE, 0 );

		// set up openGL for 2D rendering ----------

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // clear to white
		//glClearDepth(1.0f);
 
		glViewport(0, 0, width, height); // set coordinate system (0,0) in top left
 
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity(); // clear projection matix
 
		glOrtho(0, width, height, 0, 1, -1); // set up 2d perspective

		glDisable(GL_DEPTH_TEST); // disable depth
 
		glMatrixMode(GL_MODELVIEW);
 
		glEnable(GL_TEXTURE_2D);
 
		glLoadIdentity();

		glTranslatef(0.375f, 0.375f, 0.0f); // displacement trick for more exact pixelization

		MIOSDLGraph_Cls();

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
		//SDL_Flip(stSDLScreen);
		//SDL_UpdateRect(stSDLScreen, 0, 0, 0, 0);
		SDL_GL_SwapBuffers();
	}
}

extern void	MIOSDLGraph_Cls ()
{
	if(stSDLWinOpen){
		glClear(GL_COLOR_BUFFER_BIT);
	}
}

extern void	MIOSDLGraph_Line (OOTint x1,OOTint y1,OOTint x2,OOTint y2,
								OOTint r, OOTint g, OOTint b, OOTint a)
{
	if(stSDLWinOpen){
		glColor3f(r/255.0f,g/255.0f,b/255.0f);
		//glColor4ub(CAP8(r), CAP8(g), CAP8(b),CAP8(a));

		glBegin(GL_LINES);
		glVertex2f((GLfloat)x1, (GLfloat)y1); 
		glVertex2f((GLfloat)x2, (GLfloat)y2);
		glEnd();
	}
}

extern void	MIOSDLGraph_FillRect (OOTint x1,OOTint y1,OOTint x2,OOTint y2,
								OOTint r, OOTint g, OOTint b, OOTint a)
{
	if(stSDLWinOpen){
		glColor4ub(CAP8(r), CAP8(g), CAP8(b),CAP8(a));
		
		glBegin(GL_QUADS); // draw rect with quads
		glVertex2f((GLfloat)x1, (GLfloat)y1);
		glVertex2f((GLfloat)x2, (GLfloat)y1);
		glVertex2f((GLfloat)x2, (GLfloat)y2);
		glVertex2f((GLfloat)x1, (GLfloat)y2);
		glEnd();
	}
}