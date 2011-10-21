/**************/
/* miomusic.c */
/**************/

/*******************/
/* System includes */
/*******************/
#include <ctype.h>
#include <string.h>

/****************/
/* Self include */
/****************/
#include "miomusic.h"

/******************/
/* Other includes */
/******************/
#include "mdiomusic.h"

#include "mio.h"

#include "mioerr.h"

#include "miofile.h"
#include "miotime.h"

#include "edint.h"

// Test to make certain we're not accidentally including <windows.h> which
// might allow for windows contamination of platform independent code.
#ifdef _INC_WINDOWS
xxx
#endif

/**********/
/* Macros */
/**********/
#define FREQ_FACTOR		1193180

#define DEFAULT_VOLUME			10  // Set to something reasonable
#define DEFAULT_OCTAVE			4
#define DEFAULT_NOTE_DIV		4
#define DEFAULT_WHOLE_NOTE_DURATION	1000

/*************/
/* Constants */
/*************/
#define STRLEN	    256

/********************/
/* Global variables */
/********************/

/*********/
/* Types */
/*********/

/**********************/
/* External variables */
/**********************/

/********************/
/* Static constants */
/********************/
// offsets:                  C  D  E  F  G  A   B
static int  stToneBase[] = { 1, 3, 5, 6, 8, 10, 12 };
static int  stToneDivisor[] = {
	19327,		// C b		
	18242,		// C		
	17218,		// C #	( D b ) 
	16252,		// D		
	15340,		// D #  ( E b ) 
	14479,		// E	( F b ) 
	13666,		// F	( E # ) 
	12899,		// F #  ( G b ) 
	12175,		// G		
	11492,		// G #	( A b ) 
	10847,		// A		
	10238,		// A #  ( B b ) 
	9664,		// B    ( C b ) 
	9121		// B #		
};

/********************/
/* Static variables */
/********************/
static BOOL		stAllowSound;
static TONE_RECORD	stToneList [STRLEN]; // Max num tones is length of OOTstring
static int		stNumTones;
static int		stCurrentTone;
static int		stWholeNoteDuration = DEFAULT_WHOLE_NOTE_DURATION;
static int		stOctave;
static int		stNoteDiv;
static int		stVolume;

/******************************/
/* Static callback procedures */
/******************************/

/*********************/
/* Static procedures */
/*********************/
static void	MyParsePlayString (const OOTstring pmPlayStr, 
				   TONE_RECORD pmToneList [],
				   int *pmNumTones);


/*********************************************/
/* External procedures for Turing predefines */
/*********************************************/
/************************************************************************/
/* MIOMusic_Play							*/
/************************************************************************/
BOOL	MIOMusic_Play (EventDescriptor *pmEvent, OOTstring pmPlayStr)
{
    char        myStrippedPlayStr [STRLEN];
    int		i = 0;
    int		j = 0;

    if (!stAllowSound)
    {
        SET_ERRNO(E_MUSIC_DISABLED);
        return FALSE;
    }

    //
    // Strip out any white space
    //
    while (pmPlayStr [i]) 
    {
	if (!isspace (pmPlayStr[i])) 
	{
	    myStrippedPlayStr [j] = pmPlayStr [i];
	    j++;
	}
	i++;
    }
    myStrippedPlayStr [j] = 0;

    MyParsePlayString (myStrippedPlayStr, stToneList, &stNumTones);

    if (stNumTones > 0) 
    {
        stCurrentTone = 0;
	if (MDIOMusic_NotePlay (stToneList [stCurrentTone].midiTone, 
				stToneList [stCurrentTone].freq, 
				stToneList [stCurrentTone].volume))
	{
	    stNumTones = 0;
	    return FALSE;
	}

        pmEvent -> mode = EventMode_PlayNoteDone;
        pmEvent -> count = MIOTime_GetTicks () + 
			   stToneList [stCurrentTone].duration;
	return TRUE;
    }

    SET_ERRNO(E_MUSIC_NO_NOTES);
    return FALSE;
} // MIOMusic_Play


/************************************************************************/
/* MIOMusic_PlayDone							*/
/************************************************************************/
OOTint	MIOMusic_PlayDone (void)
{
    return (stNumTones == 0);
} // MIOMusic_PlayDone


/************************************************************************/
/* MIOMusic_PlayFile							*/
/************************************************************************/
BOOL	MIOMusic_PlayFile (EventDescriptor *pmEvent, OOTstring pmPath)
{
    char	myFilePath[256];
    int		myKind;

    if (!stAllowSound)
    {
        SET_ERRNO(E_MUSIC_DISABLED);
        return FALSE;
    }

    /* File name is "CD" or "CD:<track>" */
    if ((strlen (pmPath) >= 2) &&
        (((pmPath [0] == 'c') || (pmPath [0] == 'C')) &&
	 ((pmPath [1] == 'd') || (pmPath [1] == 'D')) &&
	 ((pmPath [2] == 0) || (pmPath [2] == ':'))))
    {
	if (MDIOMusic_FileCDPlay (&pmPath [2], &myKind)) 
	{
	    pmEvent -> mode = EventMode_PlayFileDone;
	    pmEvent -> count = myKind;
	    return TRUE;
	}
    }

    /* File name is <pmPath>.wav */
    if (_stricmp (".wav", &pmPath [strlen (pmPath) - 4]) == 0)
    {
	if (!MIOFile_ConvertPath (pmPath, NULL, myFilePath, NO_TRAILING_SLASH))
	{
	    return FALSE;
	}
	if (MDIOMusic_FileWAVEPlay (myFilePath, &myKind))
	{
	    pmEvent -> mode = EventMode_PlayFileDone;
	    pmEvent -> count = myKind;
	    return TRUE;
	}
    }

    /* File name is <pmPath>.mp3 */
    if (_stricmp (".mp3", &pmPath [strlen (pmPath) - 4]) == 0)
    {
	if (!MIOFile_ConvertPath (pmPath, NULL, myFilePath, NO_TRAILING_SLASH))
	{
	    return FALSE;
	}
	if (MDIOMusic_FileMP3Play (myFilePath, &myKind))
	{
	    pmEvent -> mode = EventMode_PlayFileDone;
	    pmEvent -> count = myKind;
	    return TRUE;
	}
    }

    /* File name is <pmPath>.mid */
    if ((_stricmp (".mid", &pmPath [strlen (pmPath) - 4]) == 0) ||
        (_stricmp (".midi", &pmPath [strlen (pmPath) - 5]) == 0))
    {
	if (!MIOFile_ConvertPath (pmPath, NULL, myFilePath, NO_TRAILING_SLASH))
	{
	    return FALSE;
	}
	if (MDIOMusic_FileMIDIPlay (myFilePath, &myKind))
	{
	    pmEvent -> mode = EventMode_PlayFileDone;
	    pmEvent -> count = myKind;
	    return TRUE;
	}
    }

    return FALSE;
} // MIOMusic_PlayFile


/************************************************************************/
/* MIOMusic_PlayFileDone						*/
/************************************************************************/
OOTint	MIOMusic_PlayFileDone (void)
{
    return (!MDIOMusic_EventCheckMusicFile (ANY_AUDIO));
} // MIOMusic_PlayFileDone


/************************************************************************/
/* MIOMusic_PlayFileStop						*/
/************************************************************************/
void	MIOMusic_PlayFileStop (void)
{
    MDIOMusic_MusicFileStop ();
} // MIOMusic_PlayFileStop


/************************************************************************/
/* MIOMusic_PreLoad							*/
/************************************************************************/
void	MIOMusic_PreLoad (char *pmPathName)
{
    // TW
} // MIOMusic_PreLoad


/************************************************************************/
/* MIOMusic_SetTempo							*/
/************************************************************************/
void	MIOMusic_SetTempo (OOTint pmWholeNoteDuration)
{
    stWholeNoteDuration = pmWholeNoteDuration;
} // MIOMusic_SetTempo


/************************************************************************/
/* MIOMusic_Sound							*/
/************************************************************************/
BOOL	MIOMusic_Sound (EventDescriptor *pmEvent, OOTint pmFrequency, 
			OOTint pmDuration)
{
    if (!stAllowSound)
    {
        SET_ERRNO(E_MUSIC_DISABLED);
        return FALSE;
    }

    if (pmFrequency < 0 || pmFrequency > 20000) 
    {
	SET_ERRNO(E_MUSIC_FREQUENCY_OUT_OF_RANGE);
	return FALSE;
    }

    if (pmDuration < 0 || pmDuration > 60000) 
    {
	SET_ERRNO(E_MUSIC_DURATION_OUT_OF_RANGE);
	return FALSE;
    }

    pmEvent -> mode  = EventMode_PlayFreqDone;
    pmEvent -> count = MIOTime_GetTicks () + pmDuration;

    if (MDIOMusic_FreqPlay (pmFrequency))
    {
	return FALSE;
    }

    return TRUE;
} // MIOMusic_Sound


/************************************************************************/
/* MIOMusic_SoundOff							*/
/************************************************************************/
void	MIOMusic_SoundOff (void)
{
    MDIOMusic_NoteStop ();
    MDIOMusic_FreqStop ();
    if (stNumTones) 
    {
	stNumTones = 0;
    }
} // MIOMusic_SoundOff


/***************************************/
/* External procedures for MIO library */
/***************************************/
/************************************************************************/
/* MIOMusic_Init							*/
/************************************************************************/
void	MIOMusic_Init ()
{
    MDIOMusic_Init ();
} // MIOMusic_Init


/************************************************************************/
/* MIOMusic_Finalize							*/
/************************************************************************/
void	MIOMusic_Finalize (void)
{
    MDIOMusic_Finalize ();
} // MIOMusic_Finalize


/************************************************************************/
/* MIOMusic_Init_Run							*/
/************************************************************************/
void	MIOMusic_Init_Run (BOOL pmAllowSound)
{
    stAllowSound = pmAllowSound;
    stNumTones = 0;
    stCurrentTone = 0;
    stWholeNoteDuration = DEFAULT_WHOLE_NOTE_DURATION;
    stOctave = DEFAULT_OCTAVE;
    stNoteDiv = DEFAULT_NOTE_DIV;
    stVolume = DEFAULT_VOLUME;
} // MIOMusic_Init_Run


/************************************************************************/
/* MIOMusic_Finalize_Run						*/
/************************************************************************/
void	MIOMusic_Finalize_Run (void)
{
    MIOMusic_SoundOff ();
    MIOMusic_PlayFileStop ();
} // MIOMusic_Finalize_Run


/************************************************************************/
/* MIOMusic_EventCheckFreq						*/
/************************************************************************/
BOOL	MIOMusic_EventCheckFreq (EventDescriptor *pmEvent)
{
    if (pmEvent -> count > MIOTime_GetTicks ())
    {
	return FALSE;
    }

    return TRUE;
} // MIOMusic_EventCheckFreq
 

/************************************************************************/
/* MIOMusic_EventCheckNote						*/
/************************************************************************/
BOOL	MIOMusic_EventCheckNote (EventDescriptor *pmEvent)
{
    if (pmEvent -> count > MIOTime_GetTicks ())
    {
	return FALSE;
    }
    stCurrentTone++;
    if (stCurrentTone < stNumTones) 
    {
	pmEvent -> count = MIOTime_GetTicks () + 
		           stToneList [stCurrentTone].duration;
	if (stToneList [stCurrentTone].freq == 0)
	{
	    MDIOMusic_NoteStop ();
	}
	else
	{
	    MDIOMusic_NotePlay (stToneList [stCurrentTone].midiTone, 
				stToneList [stCurrentTone].freq, 
				stToneList [stCurrentTone].volume);
	}
	return FALSE;
    }
    return TRUE;
} // MIOMusic_EventCheckNote
 

/************************************************************************/
/* MIOMusic_EventCheckMusicFile						*/
/************************************************************************/
BOOL	MIOMusic_EventCheckMusicFile (EventDescriptor *pmEvent)
{
    return MDIOMusic_EventCheckMusicFile (pmEvent -> count);
} // MIOMusic_EventCheckMusicFile
 

/******************************/
/* Static callback procedures */
/******************************/


/*********************/
/* Static procedures */
/*********************/
static void	MyParsePlayString (const OOTstring pmPlayStr,
				   TONE_RECORD pmToneList [],
				   int *pmNumTones)
{
    #define NUM_NOTE_TYPES	6

    char    	*myPtr = pmPlayStr;
    char    	myChar;
    int	    	myNoteInd, myNote;
    int		cnt;
    static char stMyNoteCode [NUM_NOTE_TYPES] = 
			    { '1', '2', '4', '8', '6', '3' };

    *pmNumTones = 0;

    while (*myPtr != 0) 
    {
	myChar = tolower (*myPtr);

	if (isdigit (myChar)) 
	{
            cnt = 0;

	    while ((cnt < NUM_NOTE_TYPES) && (myChar != stMyNoteCode [cnt]))
	    {
		cnt++;
	    }

	    if (cnt == NUM_NOTE_TYPES) 
	    {
		SET_ERRNO(E_MUSIC_BAD_PLAY_CHAR);
	    }
	    else 
	    {
		stNoteDiv = (1 << cnt);
	    }
	}
	else if (('a' <= myChar) && (myChar <= 'g'))
	{
	    myNoteInd = 
		((( myChar < 'c' ) ? (myChar - 'a' + 5) : (myChar - 'c')));
	    myNote = stToneBase [myNoteInd];

	    // Check for flat or sharp
	    if (*(myPtr + 1) == '-') 
	    {
		myNote--;
		myPtr++;
	    }
	    else if (*(myPtr + 1) == '+') 
	    {
		myNote++;
		myPtr++;
	    }

	    // Fill in a tone record

	    pmToneList [*pmNumTones].midiTone = (myNote - 1) + stOctave * 12;
	    pmToneList [*pmNumTones].freq =
		FREQ_FACTOR / (stToneDivisor [myNote] / ( 1 << stOctave ));
	    pmToneList [*pmNumTones].duration = stWholeNoteDuration / stNoteDiv;
	    pmToneList [*pmNumTones].volume = stVolume;
	    (*pmNumTones)++;
	}
	else if (myChar == 'p') 
	{
	    // Fill in a tone record
	    pmToneList [*pmNumTones].midiTone = -1;
	    pmToneList [*pmNumTones].freq = 0;
	    pmToneList [*pmNumTones].duration = stWholeNoteDuration / stNoteDiv;
	    pmToneList [*pmNumTones].volume = 0;
	    (*pmNumTones)++;
	}
	else if (myChar == '<') 
	{
	    // Down one octave
	    if (stOctave > 1)
	    {
	        stOctave--;
	    }
	}
	else if (myChar == '>') 
	{
	    // Up one octave
	    if (stOctave < 8)
	    {
	        stOctave++;
	    }
	}
	else 
	{
	    SET_ERRNO (E_MUSIC_BAD_PLAY_CHAR);
	}
	myPtr++;
    }
    #undef NUM_NOTE_TYPES
} // MyParsePlayString
