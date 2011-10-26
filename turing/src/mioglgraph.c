/*******************/
/* System includes */
/*******************/
#include "SDL.h"
#include "SDL_opengl.h"

/****************/
/* Self include */
/****************/
#include "mioglgraph.h"

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

static int stGLWinOpen;
static int stSDLLightingOn;

static int myWidth,myHeight;

// windows specific
static HDC glDeviceContext;
static HGLRC glRenderContext;

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
void MySetupOpenGL() {
		GLfloat LightAmbient[]= { 0.5f, 0.5f, 0.5f, 1.0f };
		GLfloat LightDiffuse[]= { 1.0f, 1.0f, 1.0f, 1.0f };
		GLfloat LightPosition[]= { 0.0f, 0.0f, 2.0f, 1.0f }; 
		
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
}

/***********************/
/* External procedures */
/***********************/

extern void	MIOGLGraph_InitRun () {
	stGLWinOpen = FALSE;
	stSDLLightingOn = FALSE;
}
extern void	MIOGLGraph_NewContext (OOTint width,OOTint height)
{
	MIOWinInfoPtr	myInfo = MIO_selectedRunWindowInfo;
	HBITMAP emptyBitmap;
	if(!stGLWinOpen){
		// create an in memory device context to render to
		glDeviceContext = CreateCompatibleDC (myInfo -> deviceContext);
		emptyBitmap = CreateCompatibleBitmap(glDeviceContext,width,height);
		SelectObject(glDeviceContext,emptyBitmap);

		//setup for GL
		glRenderContext = wglCreateContext( glDeviceContext );
		wglMakeCurrent( glDeviceContext, glRenderContext );

		myWidth = width;
		myHeight = height;

		//ogl stuff
		MySetupOpenGL();

		stGLWinOpen = TRUE;
	}
}

extern void	MIOGLGraph_FreeContext()
{
	if(stGLWinOpen){
		//wglMakeCurrent( NULL, NULL );
		wglDeleteContext( glRenderContext );
		DeleteDC (glDeviceContext);
	}
}

extern void	MIOGLGraph_CopyToWin (OOTint x,OOTint y) 
{
	MIOWinInfoPtr	myInfo = MIO_selectedRunWindowInfo;
	if (myInfo -> displayOnScreen)
	{
		BitBlt (myInfo -> deviceContext, x, y+myHeight, 
				myWidth, myHeight, glDeviceContext, 0, 0,
			SRCCOPY);
	}
	// Blit the picture onto the backing store
	BitBlt (myInfo -> offscreenDeviceContext, x, y+myHeight, 
				myWidth, myHeight, glDeviceContext, 0, 0,
			SRCCOPY);
}

extern void	MIOGLGraph_NewWin (OOTint width,OOTint height,OOTint winMode)
{
	int mode;

	if(!stGLWinOpen){
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

		//ogl stuff
		MySetupOpenGL();

		MIOGLGraph_Cls();

		stGLWinOpen = TRUE;
	}
}

extern void	MIOGLGraph_CloseWin ()
{
	if(stGLWinOpen){
		SDL_Quit();
		stGLWinOpen = FALSE;
	}
}

extern void	MIOGLGraph_Update ()
{
	if(stGLWinOpen){
		//SDL_Flip(stSDLScreen);
		//SDL_UpdateRect(stSDLScreen, 0, 0, 0, 0);
		SDL_GL_SwapBuffers();
	}
}

extern void	MIOGLGraph_Cls ()
{
	if(stGLWinOpen){
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
	}
}
//light
extern void	MIOGLGraph_SetLight (OOTint lighting)
{
	if(stGLWinOpen){
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
extern void	MIOGLGraph_ClearMatrix ()
{
	if(stGLWinOpen){
		glLoadIdentity();
	}
}

extern void	MIOGLGraph_PushMatrix ()
{
	if(stGLWinOpen){
		glPushMatrix();
	}
}

extern void	MIOGLGraph_PopMatrix ()
{
	if(stGLWinOpen){
		glPopMatrix();
	}
}
// matrix manipulation
extern void	MIOGLGraph_Translate (OOTreal x,OOTreal y,OOTreal z)
{
	if(stGLWinOpen){
		glTranslatef((GLfloat)x,(GLfloat)y,(GLfloat)z);
	}
}
extern void	MIOGLGraph_Rotate (OOTreal angle,OOTreal x,OOTreal y,OOTreal z)
{
	if(stGLWinOpen){
		glRotatef((GLfloat)angle,(GLfloat)x,(GLfloat)y,(GLfloat)z);
	}
}
extern void	MIOGLGraph_Scale (OOTreal x,OOTreal y,OOTreal z)
{
	if(stGLWinOpen){
		glScalef((GLfloat)x,(GLfloat)y,(GLfloat)z);
	}
}
// drawing
extern void	MIOGLGraph_Line (
								OOTreal x1,OOTreal y1,OOTreal z1, 
								OOTreal x2,OOTreal y2,OOTreal z2,
								OOTint r, OOTint g, OOTint b)
{
	if(stGLWinOpen){
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

extern void	MIOGLGraph_FillTriangle (
								OOTreal x1,OOTreal y1,OOTreal z1, 
								OOTreal x2,OOTreal y2,OOTreal z2,
								OOTreal x3,OOTreal y3,OOTreal z3,
								OOTint r, OOTint g, OOTint b)
{
	if(stGLWinOpen){
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

extern void	MIOGLGraph_Triangle (
								OOTreal x1,OOTreal y1,OOTreal z1, 
								OOTreal x2,OOTreal y2,OOTreal z2,
								OOTreal x3,OOTreal y3,OOTreal z3,
								OOTint r, OOTint g, OOTint b)
{
	if(stGLWinOpen){		
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