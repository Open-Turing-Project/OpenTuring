/***************/
/* mdiomusic.h */
/***************/

#ifndef _MDIOMUSIC_H_
#define _MDIOMUSIC_H_

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
#define ANY_AUDIO	0
#define CD_AUDIO	1
#define MIDI_AUDIO	2
#define WAVE_AUDIO	3
#define MP3_AUDIO	4

/*********/
/* Types */
/*********/
typedef struct tagTONE_RECORD 
{
    WORD midiTone;
    WORD freq;
    WORD duration;
    WORD volume;
} TONE_RECORD;

/**********************/
/* External variables */
/**********************/

/***********************/
/* External procedures */
/***********************/
extern void	MDIOMusic_Init (void);

extern BOOL	MDIOMusic_EventCheckMusicFile (int pmKind);
extern BOOL	MDIOMusic_FileCDPlay (const char *pmTrackString, int *pmKind);
extern BOOL	MDIOMusic_FileMIDIPlay (const char *pmFileName, int *pmKind);
extern BOOL	MDIOMusic_FileMP3Play (const char *pmFileName, int *pmKind);
extern BOOL	MDIOMusic_FileWAVEPlay (const char *pmFileName, int *pmKind);
extern void	MDIOMusic_Finalize (void);
extern BOOL	MDIOMusic_FreqPlay (OOTint pmFrequency);
extern void	MDIOMusic_FreqStop (void);
extern void	MDIOMusic_MusicFileStop (void);
extern BOOL	MDIOMusic_NotePlay (WORD pmMidiTone, WORD pmFrequency, 
				    WORD pmVolume);
extern void	MDIOMusic_NoteStop (void);
#endif // #ifndef _MDIOMUSIC_H_
