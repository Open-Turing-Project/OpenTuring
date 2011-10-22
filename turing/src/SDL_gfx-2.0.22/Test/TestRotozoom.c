/* 

TestRotozoom 

Test program for rotozoom routines

Copyright (C) A. Schiffler, July 2001-2009, GPL

*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "SDL.h"

#ifdef WIN32
#include <windows.h>
#include "SDL_gfxPrimitives.h"
#include "SDL_rotozoom.h"
#else
#include "SDL/SDL_rotozoom.h"
#endif

/* Custom rotation setup */
double custom_angle=0.0;
double custom_fx=1.0;
double custom_fy=1.0;
int custom_smooth=0;

/* Delay between frames */
int delay;

/* Curren message */
char *messageText;

void HandleEvent()
{
	SDL_Event event; 

	/* Check for events */
	while ( SDL_PollEvent(&event) ) {
		switch (event.type) {
			 case SDL_KEYDOWN:
			 case SDL_QUIT:
				 exit(0);
				 break;
		}
	}
}

void ClearScreen(SDL_Surface *screen)
{
	int i;
	/* Set the screen to black */
	if ( SDL_LockSurface(screen) == 0 ) {
		Uint8 *pixels;
		pixels = (Uint8 *)screen->pixels;
		for ( i=0; i<screen->h; ++i ) {
			memset(pixels, 0,
				screen->w*screen->format->BytesPerPixel);
			pixels += screen->pitch;
		}
		SDL_UnlockSurface(screen);
	}
}

void RotatePicture (SDL_Surface *screen, SDL_Surface *picture, int rotate, int flip, int smooth) 
{
	SDL_Surface *rotozoom_picture;
	SDL_Rect dest;
	int framecount, framemax, frameinc;
	double angle, zoomf, zoomfx, zoomfy;

	printf("%s\n", messageText);

	/* Rotate and display the picture */
	framemax=4*360; 
	frameinc=1;
	for (framecount=-360; framecount<framemax; framecount += frameinc) {
		if ((framecount % 360)==0) frameinc++;
		HandleEvent();
		ClearScreen(screen);
		zoomf=(float)(framecount+2*360)/(float)framemax;
		zoomf=1.5*zoomf*zoomf;
		/* Are we in flipping mode? */
		if (flip) {
			/* Flip X factor */
			if (flip & 1) {
				zoomfx=-zoomf;
			} else {
				zoomfx=zoomf;
			}
			/* Flip Y factor */
			if (flip & 2) {
				zoomfy=-zoomf;
			} else {
				zoomfy=zoomf;
			}
			angle=framecount*rotate;
			if (((framecount % 120)==0) || (delay>0)) {
				printf ("  Frame: %i   Rotate: angle=%.2f  Zoom: x=%.2f y=%.2f\n",framecount,angle,zoomfx,zoomfy);
			}
			if ((rotozoom_picture=rotozoomSurfaceXY (picture, angle, zoomfx, zoomfy, smooth))!=NULL) {
				dest.x = (screen->w - rotozoom_picture->w)/2;;
				dest.y = (screen->h - rotozoom_picture->h)/2;
				dest.w = rotozoom_picture->w;
				dest.h = rotozoom_picture->h;
				if ( SDL_BlitSurface(rotozoom_picture, NULL, screen, &dest) < 0 ) {
					fprintf(stderr, "Blit failed: %s\n", SDL_GetError());
					break;
				}
				SDL_FreeSurface(rotozoom_picture);
		 }
		} else {
			angle=framecount*rotate;
			if ((framecount % 120)==0) {
				printf ("  Frame: %i   Rotate: angle=%.2f  Zoom: f=%.2f \n",framecount,angle,zoomf);
			}
			if ((rotozoom_picture=rotozoomSurface (picture, angle, zoomf, smooth))!=NULL) {
				dest.x = (screen->w - rotozoom_picture->w)/2;;
				dest.y = (screen->h - rotozoom_picture->h)/2;
				dest.w = rotozoom_picture->w;
				dest.h = rotozoom_picture->h;
				if ( SDL_BlitSurface(rotozoom_picture, NULL, screen, &dest) < 0 ) {
					fprintf(stderr, "Blit failed: %s\n", SDL_GetError());
					break;
				}
				SDL_FreeSurface(rotozoom_picture);
		 }
		}

		stringRGBA(screen, 8, 8, messageText, 255, 255, 255, 255);

		/* Display by flipping screens */
		SDL_Flip(screen);

		/* Maybe delay */
		if (delay>0) {
			SDL_Delay(delay);
		}
	}

	if (rotate) {
		/* Final display with angle=0 */
		HandleEvent();
		ClearScreen(screen);
		if (flip) {
			if ((rotozoom_picture=rotozoomSurfaceXY (picture, 0.01, zoomfx, zoomfy, smooth))!=NULL) {
				dest.x = (screen->w - rotozoom_picture->w)/2;;
				dest.y = (screen->h - rotozoom_picture->h)/2;
				dest.w = rotozoom_picture->w;
				dest.h = rotozoom_picture->h;
				if ( SDL_BlitSurface(rotozoom_picture, NULL, screen, &dest) < 0 ) {
					fprintf(stderr, "Blit failed: %s\n", SDL_GetError());
					return;
				}
				SDL_FreeSurface(rotozoom_picture);
			}		
		} else {
			if ((rotozoom_picture=rotozoomSurface (picture, 0.01, zoomf, smooth))!=NULL) {
				dest.x = (screen->w - rotozoom_picture->w)/2;;
				dest.y = (screen->h - rotozoom_picture->h)/2;
				dest.w = rotozoom_picture->w;
				dest.h = rotozoom_picture->h;
				if ( SDL_BlitSurface(rotozoom_picture, NULL, screen, &dest) < 0 ) {
					fprintf(stderr, "Blit failed: %s\n", SDL_GetError());
					return;
				}
				SDL_FreeSurface(rotozoom_picture);
			}		
		}

		stringRGBA(screen, 8, 8, messageText, 255, 255, 255, 255);

		/* Display by flipping screens */
		SDL_Flip(screen);

		/* Maybe delay */
		if (delay>0) {
			SDL_Delay(delay);
		}
	}

	/* Pause for a sec */
	SDL_Delay(1000);
}

void ZoomPicture (SDL_Surface *screen, SDL_Surface *picture, int smooth) 
{
	SDL_Surface *rotozoom_picture;
	SDL_Rect dest;
	int framecount, framemax, frameinc;
	double zoomxf,zoomyf;

	printf("%s\n", messageText);

	/* Zoom and display the picture */
	framemax=4*360; frameinc=1;
	for (framecount=360; framecount<framemax; framecount += frameinc) {
		if ((framecount % 360)==0) frameinc++;
		HandleEvent();
		ClearScreen(screen);
		zoomxf=(float)framecount/(float)framemax;
		zoomxf=1.5*zoomxf*zoomxf;
		zoomyf=0.5+fabs(1.0*sin((double)framecount/80.0));
		if ((framecount % 120)==0) {
			printf ("  Frame: %i   Zoom: x=%.2f y=%.2f\n",framecount,zoomxf,zoomyf);
		}
		if ((rotozoom_picture=zoomSurface (picture, zoomxf, zoomyf, smooth))!=NULL) {
			dest.x = (screen->w - rotozoom_picture->w)/2;;
			dest.y = (screen->h - rotozoom_picture->h)/2;
			dest.w = rotozoom_picture->w;
			dest.h = rotozoom_picture->h;
			if ( SDL_BlitSurface(rotozoom_picture, NULL, screen, &dest) < 0 ) {
				fprintf(stderr, "Blit failed: %s\n", SDL_GetError());
				break;
			}
			SDL_FreeSurface(rotozoom_picture);
		}

		stringRGBA(screen, 8, 8, messageText, 255, 255, 255, 255);

		/* Display by flipping screens */
		SDL_Flip(screen);

		/* Maybe delay */
		if (delay>0) {
			SDL_Delay(delay);
		}
	}

	/* Pause for a sec */
	SDL_Delay(1000);
}

void RotatePicture90Degrees (SDL_Surface *screen, SDL_Surface *picture) 
{
	SDL_Surface *rotozoom_picture;
	SDL_Rect dest;
	int framecount, framemax, frameinc;
	int numClockwiseTurns;

	printf("%s\n", messageText);

	/* Rotate and display the picture */
	framemax = 21;
	frameinc = 1;
	numClockwiseTurns = -4;
	for (framecount=0; framecount<framemax; framecount += frameinc) {
		HandleEvent();
		ClearScreen(screen);
		printf ("  Frame: %i   Rotate90: %i clockwise turns\n",framecount,numClockwiseTurns+4);
		if ((rotozoom_picture=rotateSurface90Degrees(picture, numClockwiseTurns))!=NULL) {
			dest.x = (screen->w - rotozoom_picture->w)/2;;
			dest.y = (screen->h - rotozoom_picture->h)/2;
			dest.w = rotozoom_picture->w;
			dest.h = rotozoom_picture->h;
			if (SDL_BlitSurface(rotozoom_picture, NULL, screen, &dest) < 0 ) {
				fprintf(stderr, "Blit failed: %s\n", SDL_GetError());
				break;
			}
			SDL_FreeSurface(rotozoom_picture);
		}

		stringRGBA(screen, 8, 8, messageText, 255, 255, 255, 255);

		/* Display by flipping screens */
		SDL_Flip(screen);

		/* Always delay */
		SDL_Delay(333);
		if (delay>0) {
			SDL_Delay(delay);
		}

		numClockwiseTurns++;
	}

	/* Pause for a sec */
	SDL_Delay(1000);
}

#define ROTATE_OFF	0
#define ROTATE_ON	1

#define FLIP_OFF	0
#define FLIP_X		1
#define FLIP_Y		2
#define FLIP_XY		3

void CustomTest(SDL_Surface *screen, SDL_Surface *picture, double a, double x, double y, int smooth){
	SDL_Surface *rotozoom_picture;
	SDL_Rect dest;

	printf("%s\n", messageText);
	printf ("  Frame: C   Rotate: angle=%.2f  Zoom: fx=%.2f fy=%.2f \n",a,x,y);

	HandleEvent();
	ClearScreen(screen);
	if ((rotozoom_picture=rotozoomSurfaceXY (picture, a, x, y, smooth))!=NULL) {
		dest.x = (screen->w - rotozoom_picture->w)/2;;
		dest.y = (screen->h - rotozoom_picture->h)/2;
		dest.w = rotozoom_picture->w;
		dest.h = rotozoom_picture->h;
		if ( SDL_BlitSurface(rotozoom_picture, NULL, screen, &dest) < 0 ) {
			fprintf(stderr, "Blit failed: %s\n", SDL_GetError());
			return;
		}
		SDL_FreeSurface(rotozoom_picture);
	}

	/* Display by flipping screens */
	SDL_Flip(screen);

	/* Maybe delay */
	if (delay>0) {
		SDL_Delay(delay);
	}

	SDL_Delay(1000);		
}


void AccuracyTest(SDL_Surface *screen)
{
	SDL_Surface* testx1;
	SDL_Surface* testx2;
	SDL_Rect r;
	SDL_Rect target;
	SDL_Surface* ref;
	int size, halfsize, doublesize;

	printf("%s\n", messageText);
	for (size = 10; size < 200; size += 2)
	{		
		HandleEvent();
		ClearScreen(screen);

		stringRGBA(screen, 8, 8, messageText, 255, 255, 255, 255);

		halfsize = size / 2;
		doublesize = size * 2;
		printf ("  zoom from %i to %i\n", size, doublesize);

		// Set up test surfaces
		testx1 = SDL_CreateRGBSurface(SDL_SWSURFACE, size, size, 24, 0, 0, 0, 0);

		r.x = 0;
		r.y = 0;
		r.w = halfsize;
		r.h = halfsize;
		SDL_FillRect(testx1, &r, 0x339933);

		r.x = halfsize;
		r.y = halfsize;
		SDL_FillRect(testx1, &r, 0x993399);

		r.x = 0;
		r.y = halfsize;
		SDL_FillRect(testx1, &r, 0x999933);

		r.x = halfsize;
		r.y = 0;
		SDL_FillRect(testx1, &r, 0x333399);

		testx2 = zoomSurface(testx1, 2.0, 2.0, 0);

		ref = SDL_CreateRGBSurface(SDL_SWSURFACE, size, size, 24, 0, 0, 0, 0);
		r.w = size;
		r.h = size;
		r.x = 0;
		r.y = 0;
		SDL_FillRect(ref, &r, 0xFFFFFF);

		/* Validation display */

		target.x = 0;
		target.y = 20;
		target.w = 0;
		target.h = 0;
		SDL_BlitSurface(testx1, 0, screen, &target);

		target.x = size;
		target.y = 20;
		SDL_BlitSurface(ref, 0, screen, &target);

		target.x = 0;
		target.y = 20 + size;
		SDL_BlitSurface(ref, 0, screen, &target);

		target.x = doublesize + 20;
		target.y = 20;
		SDL_BlitSurface(testx2, 0, screen, &target);

		target.x = doublesize + doublesize + 20;
		target.y = 20;
		SDL_BlitSurface(ref, 0, screen, &target);

		target.x = doublesize + 20;
		target.y = 20 + doublesize;
		SDL_BlitSurface(ref, 0, screen, &target);

		SDL_FreeSurface(testx1);
		SDL_FreeSurface(testx2);
		SDL_FreeSurface(ref);

		/* Display by flipping screens */
		SDL_Flip(screen);

		/* Always delay */
		SDL_Delay(250);		

		/* Maybe add extra delay */
		if (delay>0) {
			SDL_Delay(delay);
		}
	}

	SDL_Delay(1000);		
}


void Draw (SDL_Surface *screen, int start)
{
	SDL_Surface *picture, *picture_again;
	char *bmpfile;

	/* --------- 8 bit test -------- */

	if (start<=6) {

		/* Message */
		printf("Loading 8bit image\n");

		/* Load the image into a surface */
		bmpfile = "sample8.bmp";
		printf("Loading picture: %s\n", bmpfile);
		picture = SDL_LoadBMP(bmpfile);
		if ( picture == NULL ) {
			fprintf(stderr, "Couldn't load %s: %s\n", bmpfile, SDL_GetError());
			return;
		}

		sprintf(messageText, "1.  rotozoom: Rotating and zooming");
		RotatePicture(screen,picture,ROTATE_ON,FLIP_OFF,SMOOTHING_OFF);

		sprintf(messageText, "2.  rotozoom: Just zooming (angle=0)");
		RotatePicture(screen,picture,ROTATE_OFF,FLIP_OFF,SMOOTHING_OFF);

		sprintf(messageText, "3.  zoom: Just zooming");
		ZoomPicture(screen,picture,SMOOTHING_OFF);

		sprintf(messageText, "4.  rotozoom: Rotating and zooming, interpolation on but unused");
		RotatePicture(screen,picture,ROTATE_ON,FLIP_OFF,SMOOTHING_ON);

		sprintf(messageText, "5.  rotozoom: Just zooming (angle=0), interpolation on but unused");
		RotatePicture(screen,picture,ROTATE_OFF,FLIP_OFF,SMOOTHING_ON);

		sprintf(messageText, "6.  zoom: Just zooming, interpolation on but unused");
		ZoomPicture(screen,picture,SMOOTHING_ON);

		/* Free the picture */
		SDL_FreeSurface(picture);
	}

	/* -------- 24 bit test --------- */

	if (start<=12) {

		/* Message */
		printf("Loading 24bit image\n");

		/* Load the image into a surface */
		bmpfile = "sample24.bmp";
		printf("Loading picture: %s\n", bmpfile);
		picture = SDL_LoadBMP(bmpfile);
		if ( picture == NULL ) {
			fprintf(stderr, "Couldn't load %s: %s\n", bmpfile, SDL_GetError());
			return;
		}

		sprintf(messageText, "7.  rotozoom: Rotating and zooming, no interpolation");
		RotatePicture(screen,picture,ROTATE_ON,FLIP_OFF,SMOOTHING_OFF);

		sprintf(messageText, "8.  rotozoom: Just zooming (angle=0), no interpolation");
		RotatePicture(screen,picture,ROTATE_ON,FLIP_OFF,SMOOTHING_OFF);

		sprintf(messageText, "9.  zoom: Just zooming, no interpolation");
		ZoomPicture(screen,picture,SMOOTHING_OFF);

		sprintf(messageText, "10. rotozoom: Rotating and zooming, with interpolation");
		RotatePicture(screen,picture,ROTATE_ON,FLIP_OFF,SMOOTHING_ON);

		sprintf(messageText, "11. rotozoom: Just zooming (angle=0), with interpolation");
		RotatePicture(screen,picture,ROTATE_OFF,FLIP_OFF,SMOOTHING_ON);

		sprintf(messageText, "12. zoom: Just zooming, with interpolation");
		ZoomPicture(screen,picture,SMOOTHING_ON);

		/* Free the picture */
		SDL_FreeSurface(picture);
	}

	/* -------- 32 bit test --------- */

	if (start<=16) {

		/* Message */
		printf("Loading 24bit image\n");

		/* Load the image into a surface */
		bmpfile = "sample24.bmp";
		printf("Loading picture: %s\n", bmpfile);
		picture = SDL_LoadBMP(bmpfile);
		if ( picture == NULL ) {
			fprintf(stderr, "Couldn't load %s: %s\n", bmpfile, SDL_GetError());
			return;
		}

		/* New source surface is 32bit with defined RGBA ordering */
		/* Much faster to do this once rather than the routine on the fly */
		fprintf(stderr,"Converting 24bit image into 32bit RGBA surface ...\n");
		picture_again = SDL_CreateRGBSurface(SDL_SWSURFACE, picture->w, picture->h, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
		SDL_BlitSurface(picture,NULL,picture_again,NULL);

		/* Message */
		sprintf(messageText, "13. Rotating and zooming, with interpolation");
		RotatePicture(screen,picture_again,ROTATE_ON,FLIP_OFF,SMOOTHING_ON);

		/* Message */
		sprintf(messageText, "14. Just zooming (angle=0), with interpolation");
		RotatePicture(screen,picture_again,ROTATE_OFF,FLIP_OFF,SMOOTHING_ON);

		SDL_FreeSurface(picture_again);

		/* New source surface is 32bit with defined ABGR ordering */
		/* Much faster to do this once rather than the routine on the fly */
		fprintf(stderr,"Converting 24bit image into 32bit ABGR surface ...\n");
		picture_again = SDL_CreateRGBSurface(SDL_SWSURFACE, picture->w, picture->h, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
		SDL_BlitSurface(picture,NULL,picture_again,NULL);

		/* Message */
		sprintf(messageText, "15. Rotating and zooming, with interpolation");
		RotatePicture(screen,picture_again,ROTATE_ON,FLIP_OFF,SMOOTHING_ON);

		/* Message */
		sprintf(messageText, "16. Just zooming (angle=0), with interpolation");
		RotatePicture(screen,picture_again,ROTATE_OFF,FLIP_OFF,SMOOTHING_ON);

		SDL_FreeSurface(picture_again);

		/* Free the picture */
		SDL_FreeSurface(picture);
	}

	/* -------- 32 bit flip test --------- */

	if (start<=22) {

		/* Message */
		printf("Loading 24bit image\n");

		/* Load the image into a surface */
		bmpfile = "sample24.bmp";
		printf("Loading picture: %s\n", bmpfile);
		picture = SDL_LoadBMP(bmpfile);
		if ( picture == NULL ) {
			fprintf(stderr, "Couldn't load %s: %s\n", bmpfile, SDL_GetError());
			return;
		}

		/* Excercise flipping functions on 32bit RGBA */
		printf("Converting 24bit image into 32bit RGBA surface ...\n");
		picture_again = SDL_CreateRGBSurface(SDL_SWSURFACE, picture->w, picture->h, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
		SDL_BlitSurface(picture,NULL,picture_again,NULL);

		/* Message */
		sprintf(messageText, "17. Rotating with x-flip, no interpolation");
		RotatePicture(screen,picture_again,ROTATE_ON,FLIP_X,SMOOTHING_OFF);

		/* Message */
		sprintf(messageText, "18. Rotating with y-flip, no interpolation");
		RotatePicture(screen,picture_again,ROTATE_ON,FLIP_Y,SMOOTHING_OFF);

		/* Message */
		sprintf(messageText, "19. Rotating with xy-flip, no interpolation");
		RotatePicture(screen,picture_again,ROTATE_ON,FLIP_XY,SMOOTHING_OFF);

		/* Message */
		sprintf(messageText, "20. Rotating with x-flip, with interpolation");
		RotatePicture(screen,picture_again,ROTATE_ON,FLIP_X,SMOOTHING_ON);

		/* Message */
		sprintf(messageText, "21. Rotating with y-flip, with interpolation");
		RotatePicture(screen,picture_again,ROTATE_ON,FLIP_Y,SMOOTHING_ON);

		/* Message */
		sprintf(messageText, "22. Rotating with xy-flip, with interpolation");
		RotatePicture(screen,picture_again,ROTATE_ON,FLIP_XY,SMOOTHING_ON);

		/* Free the pictures */
		SDL_FreeSurface(picture);
		SDL_FreeSurface(picture_again);
	}

	if (start<=24) {

		/* Message */
		printf("Loading 24bit image\n");

		/* Load the image into a surface */
		bmpfile = "sample24.bmp";
		printf("Loading picture: %s\n", bmpfile);
		picture = SDL_LoadBMP(bmpfile);
		if ( picture == NULL ) {
			fprintf(stderr, "Couldn't load %s: %s\n", bmpfile, SDL_GetError());
			return;
		}

		/* Excercise flipping functions on 32bit RGBA */
		fprintf(stderr,"Converting 24bit image into 32bit RGBA surface ...\n");
		picture_again = SDL_CreateRGBSurface(SDL_SWSURFACE, picture->w, picture->h, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
		SDL_BlitSurface(picture,NULL,picture_again,NULL);

		sprintf(messageText, "23. CustomTest, values from commandline (32bit)");
		CustomTest(screen, picture_again, custom_angle, custom_fx, custom_fy, custom_smooth);

		SDL_FreeSurface(picture_again);

		/* Free the picture */
		SDL_FreeSurface(picture);

		/* Message */
		printf("Loading 8bit image\n");

		/* Load the image into a surface */
		bmpfile = "sample8.bmp";
		printf("Loading picture: %s\n", bmpfile);
		picture = SDL_LoadBMP(bmpfile);
		if ( picture == NULL ) {
			fprintf(stderr, "Couldn't load %s: %s\n", bmpfile, SDL_GetError());
			return;
		}

		sprintf(messageText, "24. CustomTest, values from commandline (8bit)");
		CustomTest(screen, picture, custom_angle, custom_fx, custom_fy, custom_smooth);

		/* Free the picture */
		SDL_FreeSurface(picture);
	}

	if (start<=25) {

		/* Message */
		printf("Loading 24bit image\n");

		/* Load the image into a surface */
		bmpfile = "sample24.bmp";
		printf("Loading picture: %s\n", bmpfile);
		picture = SDL_LoadBMP(bmpfile);
		if ( picture == NULL ) {
			fprintf(stderr, "Couldn't load %s: %s\n", bmpfile, SDL_GetError());
			return;
		}

		/* New source surface is 32bit with defined RGBA ordering */
		printf("Converting 24bit image into 32bit RGBA surface ...\n");
		picture_again = SDL_CreateRGBSurface(SDL_SWSURFACE, picture->w, picture->h, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
		SDL_BlitSurface(picture,NULL,picture_again,NULL);

		/* Excercise rotate90 function on 32bit RGBA */
		sprintf(messageText, "25.  rotate90: Rotate 90 degrees clockwise (32bit)");
		RotatePicture90Degrees(screen, picture_again);

		/* Free the pictures */
		SDL_FreeSurface(picture);
		SDL_FreeSurface(picture_again);
	}

	if (start<=26) {
		/* Run accuracy test */
		sprintf(messageText, "26.  accuracy: zoom by factor of 2");
		AccuracyTest(screen);
	}

	return;
}

int main ( int argc, char *argv[] )
{
	SDL_Surface *screen;
	int w, h;
	int desired_bpp;
	Uint32 video_flags;
	int start;

	/* Title */
	fprintf(stderr,"SDL_rotozoom test\n");
	messageText = (char *)malloc(128);

	/* Set default options and check command-line */
	w = 640;
	h = 480;
	desired_bpp = 0;
	video_flags = 0;
	start = 1;
	delay = 0;
	while ( argc > 1 ) {
		if ( strcmp(argv[1], "-start") == 0 ) {
			if ( argv[2] && ((start = atoi(argv[2])) > 0) ) {
				argv += 2;
				argc -= 2;
			} else {
				fprintf(stderr,
					"The -start option requires an argument\n");
				exit(1);
			}
		} else
			if ( strcmp(argv[1], "-delay") == 0 ) {
				if ( argv[2] && ((delay = atoi(argv[2])) > 0) ) {
					argv += 2;
					argc -= 2;
				} else {
					fprintf(stderr,
						"The -delay option requires an argument\n");
					exit(1);
				}
			} else
				if ( strcmp(argv[1], "-width") == 0 ) {
					if ( argv[2] && ((w = atoi(argv[2])) > 0) ) {
						argv += 2;
						argc -= 2;
					} else {
						fprintf(stderr,
							"The -width option requires an argument\n");
						exit(1);
					}
				} else
					if ( strcmp(argv[1], "-height") == 0 ) {
						if ( argv[2] && ((h = atoi(argv[2])) > 0) ) {
							argv += 2;
							argc -= 2;
						} else {
							fprintf(stderr,
								"The -height option requires an argument\n");
							exit(1);
						}
					} else
						if ( strcmp(argv[1], "-bpp") == 0 ) {
							if ( argv[2] ) {
								desired_bpp = atoi(argv[2]);
								argv += 2;
								argc -= 2;
							} else {
								fprintf(stderr,
									"The -bpp option requires an argument\n");
								exit(1);
							}
						} else
							if ( strcmp(argv[1], "-warp") == 0 ) {
								video_flags |= SDL_HWPALETTE;
								argv += 1;
								argc -= 1;
							} else
								if ( strcmp(argv[1], "-hw") == 0 ) {
									video_flags |= SDL_HWSURFACE;
									argv += 1;
									argc -= 1;
								} else
									if ( strcmp(argv[1], "-fullscreen") == 0 ) {
										video_flags |= SDL_FULLSCREEN;
										argv += 1;
										argc -= 1;
									} else 
										if ( strcmp(argv[1], "-custom") == 0 ) {
											if (( argv[2] ) && ( argv[3] ) && ( argv[4] ) && (argv[5] )) {
												custom_angle = atof(argv[2]);
												custom_fx = atof(argv[3]);
												custom_fy = atof(argv[4]);
												custom_smooth = atoi(argv[5]);
												argv += 5;
												argc -= 5;
											} else {
												fprintf(stderr,
													"The -custom option requires 4 arguments\n");
												exit(1);
											}
										} else
											if (( strcmp(argv[1], "-help") == 0 ) || (strcmp(argv[1], "--help") == 0)) {
												printf ("Usage:\n");
												printf (" -start #	  Set starting test number\n");
												printf ("             1=8bit, 7=24bit, 13=32bit, 19=32bit flip, 23=custom, 25=rotate90\n");
												printf ("             26=zoom accuracy\n");
												printf (" -delay #        Set delay between frames in ms (default: 0=off)\n");
												printf ("                 (if >0, enables verbose frame logging\n");
												printf (" -width #	  Screen width (Default: %i)\n",w);
												printf (" -height #	  Screen height (Default: %i)\n",h);
												printf (" -bpp #	  Screen bpp\n");
												printf (" -warp		  Enable hardware palette\n");
												printf (" -hw		  Enable hardware surface\n");
												printf (" -fullscreen	  Enable fullscreen mode\n");
												printf (" -custom # # #	# Custom: angle scalex scaley smooth\n");
												printf ("                  scalex/scaley<0, enables flip on axis\n");
												printf ("                  smooth=0/1\n");
												exit(0);
											} else
												break;
	}

	/* Force double buffering */
	video_flags |= SDL_DOUBLEBUF;

	/* Initialize SDL */
	if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
		fprintf(stderr,
			"Couldn't initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}
	atexit(SDL_Quit);			/* Clean up on exit */

	/* Initialize the display */
	screen = SDL_SetVideoMode(w, h, desired_bpp, video_flags);
	if ( screen == NULL ) {
		fprintf(stderr, "Couldn't set %dx%dx%d video mode: %s\n",
			w, h, desired_bpp, SDL_GetError());
		exit(1);
	}

	/* Show some info */
	printf("Set %dx%dx%d mode\n",
		screen->w, screen->h, screen->format->BitsPerPixel);
	printf("Video surface located in %s memory.\n",
		(screen->flags&SDL_HWSURFACE) ? "video" : "system");

	/* Check for double buffering */
	if ( screen->flags & SDL_DOUBLEBUF ) {
		printf("Double-buffering enabled - good!\n");
	}

	/* Set the window manager title bar */
	SDL_WM_SetCaption("SDL_rotozoom test", "rotozoom");

	/* Do all the drawing work */
	Draw(screen, start);	
	free(messageText);

	return(0);
}
