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
static int stSDLLightingOn;

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
	stSDLLightingOn = FALSE;
}

extern void	MIOSDLGraph_NewWin (OOTint width,OOTint height,OOTint winMode)
{
	int mode;
	GLfloat LightAmbient[]= { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat LightDiffuse[]= { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat LightPosition[]= { 0.0f, 0.0f, 2.0f, 1.0f }; 

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

		/* Enable multisampling for a nice antialiased effect */
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 2);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 5);

		// better lines		
		glEnable(GL_LINE_SMOOTH);
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		glLineWidth(1.5f);

		glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
		glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
		glClearDepth(1.0f);									// Depth Buffer Setup
		glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
		glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations

		//lighting
		glShadeModel(GL_SMOOTH);
		glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);

		glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);             // Setup The Diffuse Light
		glLightfv(GL_LIGHT1, GL_POSITION,LightPosition);            // Position The Light
		glEnable(GL_LIGHT1);

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
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
	}
}
//light
extern void	MIOSDLGraph_SetLight (OOTint lighting)
{
	if(stSDLWinOpen){
		if(lighting) {
			glEnable(GL_LIGHTING);
			stSDLLightingOn = TRUE;
		} else {
			glDisable(GL_LIGHTING);
			stSDLLightingOn = FALSE;
		}
	}
}

// matrix stack
extern void	MIOSDLGraph_ClearMatrix ()
{
	if(stSDLWinOpen){
		glLoadIdentity();
	}
}

extern void	MIOSDLGraph_PushMatrix ()
{
	if(stSDLWinOpen){
		glPushMatrix();
	}
}

extern void	MIOSDLGraph_PopMatrix ()
{
	if(stSDLWinOpen){
		glPopMatrix();
	}
}
// matrix manipulation
extern void	MIOSDLGraph_Translate (OOTreal x,OOTreal y,OOTreal z)
{
	if(stSDLWinOpen){
		glTranslatef((GLfloat)x,(GLfloat)y,(GLfloat)z);
	}
}
extern void	MIOSDLGraph_Rotate (OOTreal angle,OOTreal x,OOTreal y,OOTreal z)
{
	if(stSDLWinOpen){
		glRotatef((GLfloat)angle,(GLfloat)x,(GLfloat)y,(GLfloat)z);
	}
}
extern void	MIOSDLGraph_Scale (OOTreal x,OOTreal y,OOTreal z)
{
	if(stSDLWinOpen){
		glScalef((GLfloat)x,(GLfloat)y,(GLfloat)z);
	}
}
// drawing
extern void	MIOSDLGraph_Line (
								OOTreal x1,OOTreal y1,OOTreal z1, 
								OOTreal x2,OOTreal y2,OOTreal z2,
								OOTint r, OOTint g, OOTint b)
{
	if(stSDLWinOpen){
		if(stSDLLightingOn) { // because winding is weird. Hafta light both sides
			float color[] = { r/255.0f, g/255.0f, b/255.0f, 1.0f };
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
		}

		//glColor3f(r/255.0f,g/255.0f,b/255.0f);
		glColor3ub(CAP8(r), CAP8(g), CAP8(b));

		glBegin(GL_LINES);
		glVertex3f((GLfloat)x1, (GLfloat)y1, (GLfloat)z1); 
		glVertex3f((GLfloat)x2, (GLfloat)y2, (GLfloat)z2);
		glEnd();

		if(stSDLLightingOn) { // because winding is weird. Hafta light both sides

		}
	}
}

extern void	MIOSDLGraph_FillTriangle (
								OOTreal x1,OOTreal y1,OOTreal z1, 
								OOTreal x2,OOTreal y2,OOTreal z2,
								OOTreal x3,OOTreal y3,OOTreal z3,
								OOTint r, OOTint g, OOTint b)
{
	if(stSDLWinOpen){
		if(stSDLLightingOn) { // because winding is weird. Hafta light both sides
			float color[] = { r/255.0f, g/255.0f, b/255.0f, 1.0f };
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
		}

		glColor3ub(CAP8(r), CAP8(g), CAP8(b));
		
		glBegin(GL_TRIANGLES);
		glVertex3f((GLfloat)x1, (GLfloat)y1, (GLfloat)z1);
		glVertex3f((GLfloat)x2, (GLfloat)y2, (GLfloat)z2);
		glVertex3f((GLfloat)x3, (GLfloat)y3, (GLfloat)z3);
		glEnd();
	}
}

extern void	MIOSDLGraph_Triangle (
								OOTreal x1,OOTreal y1,OOTreal z1, 
								OOTreal x2,OOTreal y2,OOTreal z2,
								OOTreal x3,OOTreal y3,OOTreal z3,
								OOTint r, OOTint g, OOTint b)
{
	if(stSDLWinOpen){		
		if(stSDLLightingOn) { // because winding is weird. Hafta light both sides
			float color[] = { r/255.0f, g/255.0f, b/255.0f, 1.0f };
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
		}
		glColor3ub(CAP8(r), CAP8(g), CAP8(b));
		
		glBegin(GL_LINE_LOOP); // draw rect with quads
		glVertex3f((GLfloat)x1, (GLfloat)y1, (GLfloat)z1);
		glVertex3f((GLfloat)x2, (GLfloat)y2, (GLfloat)z2);
		glVertex3f((GLfloat)x3, (GLfloat)y3, (GLfloat)z3);
		glEnd();
	}
}