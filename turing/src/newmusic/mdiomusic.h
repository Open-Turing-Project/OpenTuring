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

extern BOOL	MDIOMusic_EventCheckMusicFile (int pmKind, int pmSoundID);
extern BOOL	MDIOMusic_FileCDPlay (const char *pmTrackString, int *pmKind,
				      int *pmSoundID);
extern BOOL	MDIOMusic_FileMIDIPlay (const char *pmFileName, int *pmKind,
					int *pmSoundID);
extern BOOL	MDIOMusic_FileMP3Play (const char *pmFileName, int *pmKind,
				       int *pmSoundID);
extern BOOL	MDIOMusic_FileWAVEPlay (const char *pmFileName, int *pmKind,
					int *pmSoundID);
extern void	MDIOMusic_Finalize (void);
extern void	MDIOMusic_FreePreLoad (void *pmMDInfo);
extern BOOL	MDIOMusic_FreqPlay (OOTint pmFrequency, int *pmSoundID);
extern BOOL	MDIOMusic_FreqStillPlaying (int pmSoundID);
extern void	MDIOMusic_FreqStop (void);
extern void	MDIOMusic_MusicFileStop (void);
extern BOOL	MDIOMusic_NotePlay (WORD pmMidiTone, WORD pmFrequency, 
				    WORD pmVolume);
extern void	MDIOMusic_NoteStop (void);
extern BOOL	MDIOMusic_PreLoadPlay (void *pmMDInfo, int *pmKind, 
				       int *pmSoundID);
extern void	*MDIOMusic_PreLoadWAVE (const char *pmPathName);
#endif // #ifndef _MDIOMUSIC_H_
