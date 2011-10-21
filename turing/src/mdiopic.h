/*************/
/* mdiopic.h */
/*************/

#ifndef _MDIOPIC_H_
#define _MDIOPIC_H_

/*******************/
/* System includes */
/*******************/

/******************/
/* Other includes */
/******************/
#include "miotypes.h"

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/
// Used for MDIOPic_Flip
#define VERTICAL_FLIP		1
#define HORIZONTAL_FLIP		2
#define BOTH_FLIP		3

// These must match predefs/Pic.tu
#define MODE_COPY		0
#define MODE_XOR		1
#define MODE_MERGE		2
#define MODE_UNDER_MERGE	3
#define MODE_XOR_MERGE		4

#define UNKNOWN_FORMAT		0
#define BMP			1
#define GIF			2
#define JPG			3
#define PNG			4
#define PCX			5
#define PCT			6

#define WIPE_BAR_WIDTH		2

// Used for MDIOPic_DrawTransition 
#define MOVE_RIGHT		1
#define MOVE_LEFT		0
#define MOVE_DOWN		1
#define MOVE_UP			0

#define WIPE		TRUE
#define GROW		FALSE

#define FROM_UPPER_LEFT	    1
#define FROM_UPPER_RIGHT    2
#define FROM_LOWER_LEFT	    3
#define FROM_LOWER_RIGHT    4

#define MODE_WIPE	    1
#define MODE_SLIDE	    2
#define MODE_GROW	    3

/*********/
/* Types */
/*********/

/***********************/
/* External procedures */
/***********************/

/***********************/
/* External procedures */
/***********************/
extern void	MDIOPic_Init (void);
extern BOOL	MDIOPic_CreatePicByBlend (PIC pmPic1, PIC pmPic2, 
					  int pmWidth, int pmHeight, 
					  int pmPercent, PIC *pmResultPic);
extern BOOL	MDIOPic_CreatePicByBlur (PIC pmPic, int pmWidth, int pmHeight, 
					 int pmIterations, PIC *pmResultPic);
extern BOOL	MDIOPic_CreatePicByDraw (PIC pmPic, int pmX, int pmY, 
					 int pmWidth, int pmHeight, 
					 int pmTransparentColour, int pmMode,
					 PIC *pmResultPic);
extern void	MDIOPic_DecrementUse (PIC pmPic);
extern int	MDIOPic_DefaultFormat (void);
extern void	MDIOPic_Draw (PIC pmPic, PIC pmBackPic, int pmX, int pmY, 
			      int pmXSize, int pmYSize, 
			      int pmTransparentColour, int pmMode);
extern void	MDIOPic_DrawPic (PICBUFFER pmPic, int pmX, int pmY, int pmWidth, 
			         int pmHeight, int pmTransparentColour, 
				 int pmMode);
extern void	MDIOPic_DrawTransitionBlend (PIC pmPic, PIC pmBackPic, 
				int pmX, int pmY, int pmWidth, int pmHeight, 
				int pmPercent);
extern void	MDIOPic_DrawTransitionLeftRight (PIC pmPic, PIC pmBackPic, 
				int pmX, int pmY, int pmWidth, int pmHeight,
				int pmLastPos, int pmCurrentPos, BOOL pmUseBar,
				BOOL pmSlideRight, int pmMode);
extern void	MDIOPic_DrawTransitionTopBottom (PIC pmPic, PIC pmBackPic, 
				int pmX, int pmY, int pmWidth, int pmHeight,
				int pmLastPos, int pmCurrentPos, BOOL pmUseBar,
				BOOL pmSlideRight, int pmMode);
extern void	MDIOPic_DrawTransitionCentreToEdge (
				PIC pmPic, PIC pmBackPic, int pmX, int pmY, 
				int pmWidth, int pmHeight,
				int pmCurrentPos, BOOL pmUseBar, 
				int pmMode);
extern void	MDIOPic_DrawTransitionCornerToCorner (
				PIC pmPic, PIC pmBackPic, int pmX, int pmY, 
				int pmWidth, int pmHeight,
				int pmCurrentPos, BOOL pmUseBar, 
				BOOL pmMode, int pmCorner);
extern void	MDIOPic_DrawTransitionEdgeToCentre (
				PIC pmPic, PIC pmBackPic, int pmX, int pmY, 
				int pmWidth, int pmHeight,
				int pmCurrentPos, BOOL pmUseBar, 
				int pmMode, int pmCorner);
extern void	MDIOPic_FreePic (PIC pmPic);
extern int	MDIOPic_GetExtraInfo (void);
extern BOOL	MDIOPic_GetImage (OOTint pmX1, OOTint pmY1, OOTint pmX2, 
				  OOTint pmY2, PIC *pmMDPicInfo);
extern void	*MDIOPic_GetMaskFromMDInfo (PIC pmPic);
extern int	MDIOPic_GetMemSize (PIC pmPic);
extern void	MDIOPic_IncrementUse (PIC pmPic);
extern BOOL	MDIOPic_IsSupportedFormat (int pmFormat);
extern BOOL	MDIOPic_LoadBMP (const char *pmFullPathName, int *pmWidth,
				 int *pmHeight, PIC *pmMDPicInfo);
extern BOOL	MDIOPic_Rotate (PICBUFFER pmSrcPic, PICBUFFER pmDestPic, 
			        int pmSrcWidth, int pmSrcHeight, 
			        int *pmDestWidth, int *pmDestHeight,
			        int pmAngle, int pmXAxis, int pmYAxis, 
			        int pmBGColorNum);
extern BOOL	MDIOPic_SaveImage (const char *pmFullPathName, 
				   PIC pmMDPicInfo);
extern BOOL	MDIOPic_Scale (PICBUFFER pmSrcPic, PICBUFFER pmDestPic, 
			       int pmSrcWidth, int pmSrcHeight, 
			       int pmDestWidth, int pmDestHeight);
extern int	MDIOPic_SizePic (OOTint pmX1, OOTint pmY1, OOTint pmX2, 
				 OOTint pmY2);
extern void	MDIOPic_SpriteDraw (MIOWinInfoPtr pmInfo, PIC pmPic, int pmX, 
				    int pmY, int pmXSize, int pmYSize, 
				    int pmTransparentColour);
extern int	MDIOPic_TakePic (OOTint pmX1, OOTint pmY1, OOTint pmX2, 
				 OOTint pmY2, PICBUFFER pmPic);

#endif // #ifndef _MDIOPIC_H_
