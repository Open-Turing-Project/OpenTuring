/************/
/* miopic.h */
/************/

#ifndef _MIOPIC_H_
#define _MIOPIC_H_

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

/*********/
/* Types */
/*********/

/**********************/
/* External variables */
/**********************/

/*********************************************/
/* External procedures for Turing predefines */
/*********************************************/
extern OOTint	MIOPic_Blend (OOTint pmPicID1, OOTint pmPicID2, int pmPercent,
			      SRCPOS *pmSrcPos);
extern OOTint	MIOPic_Blur (OOTint pmPicID, int pmIterations, 
			     SRCPOS *pmSrcPos);
extern void	MIOPic_Draw (OOTint pmPicID, OOTint pmX, OOTint pmY, 
			     OOTint pmMode);
extern void	MIOPic_DrawPic (OOTint pmX, OOTint pmY, PICBUFFER pmBuffer, 
				OOTint pmMode);
extern BOOL	MIOPic_DrawFrames (struct EventDescriptor *pmEvent,
				   void *pmPicIDs, OOTint pmX, OOTint pmY,
				   OOTint pmMode, OOTint pmNumFrames, 
				   OOTint pmArraySize, 
				   OOTint pmDelayBetweenFrames,
				   OOTboolean pmEraseAfter);
extern BOOL	MIOPic_DrawSpecial (struct EventDescriptor *pmEvent,
				    OOTint pmPicID, OOTint pmX, OOTint pmY, 
				    OOTint pmMode, OOTint pmTransition,
				    OOTint pmDuration);
extern BOOL	MIOPic_EventCheckDrawFrames (struct EventDescriptor *pmEvent);
extern BOOL	MIOPic_EventCheckDrawSpecial (struct EventDescriptor *pmEvent);
extern OOTint	MIOPic_FileNew (const OOTstring pmPathName, SRCPOS *pmSrcPos);
extern void	MIOPic_FileNewFrames (OOTint *pmPicIDs, OOTint pmArraySize,
				      const OOTstring pmPathName, 
				      OOTint *pmDelayTime, SRCPOS *pmSrcPos);
extern void	MIOPic_FileSave (OOTint pmPicID, const OOTstring pmPathName);
extern OOTint	MIOPic_Flip (OOTint pmSrcPicID, BOOL pmVertical, 
			     SRCPOS *pmSrcPos);
extern int	MIOPic_Frames (const OOTstring pmPathName);
extern void	MIOPic_Free (OOTint pmPicID);
extern void	MIOPic_GetDimensions (OOTint pmPicID, OOTint *pmWidth,
				      OOTint *pmHeight);
extern void	MIOPic_GetInfoForSprite (OOTint pmPicID, OOTint *pmWidth, 
					 OOTint *pmHeight, 
					 int *pmTransparentColour,
					 PIC *pmMDPicInfo);
extern OOTint	MIOPic_New (OOTint pmX1, OOTint pmY1, OOTint pmX2, OOTint pmY2, 
			    SRCPOS *pmSrcPos);
extern OOTint	MIOPic_Rotate (OOTint pmSrcPicID, OOTint pmAngle, 
			       OOTint pmXAxis, OOTint pmYAxis, 
			       SRCPOS *pmSrcPos);
extern OOTint	MIOPic_Scale (OOTint pmSrcPicID, OOTint newWidth, 
			      OOTint newHeight, SRCPOS *pmSrcPos);
extern void	MIOPic_SetTransparentColour (OOTint pmPicID, 
					     OOTint pmColourNum);
extern int	MIOPic_SizePic (OOTint pmX1, OOTint pmY1, OOTint pmX2, 
				OOTint pmY2);
extern void	MIOPic_TakePic (OOTint pmX1, OOTint pmY1, OOTint pmX2, 
				OOTint pmY2, PICBUFFER pmBuffer);
				   

/***************************************/
/* External procedures for MIO library */
/***************************************/
extern void	MIOPic_Init (void);
extern void	MIOPic_IncremementSpriteUse (pmPicID);
extern void	MIOPic_DecremementSpriteUse (pmPicID);

#endif // #ifndef _MIOPIC_H_
