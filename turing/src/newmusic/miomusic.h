/**************/
/* miomusic.h */
/**************/

#ifndef _MIOMUSIC_H_
#define _MIOMUSIC_H_

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
extern BOOL	MIOMusic_Play (EventDescriptor *pmEvent, OOTstring pmPlayStr);
extern OOTint	MIOMusic_PlayDone (void);
extern BOOL	MIOMusic_PlayFile (EventDescriptor *pmEvent, OOTstring pmPath);
extern OOTint	MIOMusic_PlayFileDone (void);
extern void	MIOMusic_PlayFileStop (void);
extern void	MIOMusic_PreLoad (OOTstring pmPath);
extern void	MIOMusic_SetTempo (OOTint pmWholeNoteDuration);
extern BOOL	MIOMusic_Sound (EventDescriptor *pmEvent, OOTint pmFrequency, 
				OOTint pmDuration);
extern void	MIOMusic_SoundOff (void);

/***************************************/
/* External procedures for MIO library */
/***************************************/
extern void	MIOMusic_Init (void);
extern void	MIOMusic_Finalize (void);
extern void	MIOMusic_Init_Run (BOOL pmAllowSound);
extern void	MIOMusic_Finalize_Run (void);

extern BOOL	MIOMusic_EventCheckNote (EventDescriptor *pmEvent);
extern BOOL	MIOMusic_EventCheckFreq (EventDescriptor *pmEvent);
extern BOOL	MIOMusic_EventCheckMusicFile (EventDescriptor *pmEvent);

#endif // #ifndef _MIOMUSIC_H_
