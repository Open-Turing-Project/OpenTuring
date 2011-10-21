/***************/
/* mdiomusic.c */
/***************/

/*******************/
/* System includes */
/*******************/
#include <windows.h>
#include <math.h>

/****************/
/* Self include */
/****************/
#include "mdiomusic.h"

/******************/
/* Other includes */
/******************/
#include "mio.h"

#include "edint.h"

#include "mioerr.h"

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/
#define PI			3.14159265
#define SAMPLE_RATE		11025
#define FREQ_BUFFER_SIZE	32768

/********************/
/* Global variables */
/********************/

/*********/
/* Types */
/*********/
typedef struct WAVEFile
{
    WAVEFORMATEX	wave;
    WAVEHDR		waveHdr;
    void		*buffer;
    BOOL		freeWhenFinished;
    int			soundID;
} WAVEFile;

/**********************/
/* External variables */
/**********************/

/********************/
/* Static constants */
/********************/

/********************/
/* Static variables */
/********************/
static struct WAVEFile	*stWAVEFileQueueHead = NULL;
static BOOL		stPlayingFile [] = {FALSE, FALSE, FALSE, FALSE, FALSE};
static int		stDeviceID [] = {0, 0, 0, 0, 0};
static BOOL		stPlayingNote = FALSE; 
static WORD		stPrevMidiTone = (WORD) -1;
static HMIDIOUT		stMidiOut;
static CRITICAL_SECTION	stMusicCallback;

// This is a counter for identifying individual sounds so we know what has
// been cancelled and what is still playing.  Start at 100 just so we know
// when we're getting real ID values back.
static int		stWAVEID = 100;
// This identifies the currently playing WAVE.  Used to determine if a sound
// is still playing, and if it needs to be cancelled.
static int		stPlayingWAVEID = 0;
// Is a WAVE File or frequency playing
static BOOL		stPlayingWAVE = FALSE;
// The WAVE currently being played
static HWAVEOUT		stWAVEOut;
// Is the WAVE Header for playing a frequency currently loaded
static BOOL		stFreqHeaderLoaded = FALSE;
// The WAVE Header and Format for playing a frequency
static WAVEHDR		stWAVEHeaderForFreq;
static WAVEFORMATEX	stWAVEFormatForFreq;
// The buffer for holding the sample data for a playing a frequency
static BYTE		stFreqBuffer [FREQ_BUFFER_SIZE];

/******************************/
/* Static callback procedures */
/******************************/

/*********************/
/* Static procedures */
/*********************/
static void	MyFreqClose (void);
static BOOL	MyHandleMCIResult (int error, MCIERROR myResult);
static BOOL	MyMidiOutMessage (int pmCommand, int pmChannel, int pmData1, 
				  int pmData2);
static BOOL	MyMusicFileStopAndClose (int pmKind, BOOL pmReallyClose);
static BOOL	MyPlayWAVE (WAVEFile *pmWAVEFile);
static BOOL	MyReadBytes (const char *pmPathName, HANDLE pmFile,  
			     void *pmBuffer, DWORD pmBytes);
static void	MyStopPlayWAVE (void);
static void CALLBACK	MyWAVEOutProcedure (HWAVEOUT pmWAVEOut, UINT pmMsg, 
					   DWORD pmInstance, DWORD pmParam1, 
					   DWORD pmParam2);


/***********************/
/* External procedures */
/***********************/
/************************************************************************/
/* MDIOMusic_Init							*/
/************************************************************************/
void	MDIOMusic_Init (void)
{
    stWAVEHeaderForFreq.lpData		= stFreqBuffer;
    stWAVEHeaderForFreq.dwBufferLength	= FREQ_BUFFER_SIZE;
    stWAVEHeaderForFreq.dwBytesRecorded	= 0;
    stWAVEHeaderForFreq.dwUser		= 0;
    stWAVEHeaderForFreq.dwFlags		= WHDR_BEGINLOOP | WHDR_ENDLOOP;
    stWAVEHeaderForFreq.dwLoops		= 10000;
    stWAVEHeaderForFreq.lpNext		= NULL;
    stWAVEHeaderForFreq.reserved	= 0;
    
    stWAVEFormatForFreq.wFormatTag	= WAVE_FORMAT_PCM;
    stWAVEFormatForFreq.nChannels	= 1;
    stWAVEFormatForFreq.nSamplesPerSec	= SAMPLE_RATE;
    stWAVEFormatForFreq.nAvgBytesPerSec	= SAMPLE_RATE;
    stWAVEFormatForFreq.nBlockAlign	= 1;
    stWAVEFormatForFreq.wBitsPerSample	= 8;
    stWAVEFormatForFreq.cbSize		= 0;

    InitializeCriticalSection (&stMusicCallback);
} // MDIOMusic_Init


/************************************************************************/
/* MDIOMusic_Finalize							*/
/************************************************************************/
void	MDIOMusic_Finalize (void)
{
    DeleteCriticalSection (&stMusicCallback);
    mciSendCommand (MCI_ALL_DEVICE_ID, MCI_CLOSE, MCI_WAIT, 0);
} // MDIOMusic_Finalize


/************************************************************************/
/* MDIOMusic_EventCheckMusicFile					*/
/*									*/
/* Returns false if the music is still playing!				*/
/************************************************************************/
BOOL	MDIOMusic_EventCheckMusicFile (int pmKind, int pmSoundID)
{
    if (pmKind == WAVE_AUDIO)
    {
	int myResult = (pmSoundID != stPlayingWAVEID);

	// If we've stopped playing (for one reason or another) then
	// free up the extra memory, etc.
	if (myResult)
	{
	}

	return myResult;
    }

    if (stPlayingFile [pmKind])
    {
	MCI_STATUS_PARMS	myMciStatusParms;
	MCIERROR		myResult;

	/* Check the device status. */
	myMciStatusParms.dwItem = MCI_STATUS_MODE;
	myResult = mciSendCommand (stDeviceID [pmKind], MCI_STATUS, 
	    MCI_STATUS_ITEM, (DWORD) (LPVOID) &myMciStatusParms);
	if (!MyHandleMCIResult (E_MUSIC_GET_STATUS_ERROR, myResult))
	{
	    if ((myMciStatusParms.dwReturn == MCI_MODE_PLAY) ||
	        (myMciStatusParms.dwReturn == MCI_MODE_SEEK))
	    {
		return FALSE;
	    }
	    stPlayingFile [pmKind] = FALSE;
	}

	MyMusicFileStopAndClose (pmKind, FALSE);
    }
    return TRUE;
} // MDIOMusic_EventCheckMusicFile


/************************************************************************/
/* MDIOMusic_FileCDPlay							*/
/*									*/
/* We play CD's using the MCI command set.  The format is		*/
/* "CD" or "CD:<track>".  At this point, pmTrack is either EOS or	*/
/* ":<track>".								*/
/************************************************************************/
BOOL	MDIOMusic_FileCDPlay (const char *pmTrackString, int *pmKind,
			      int *pmSoundID)
{
    int			pmTrack;
    MCI_OPEN_PARMS	myMciOpenParms;
    MCI_SET_PARMS	myMciSetParms;
    MCI_PLAY_PARMS	myMciPlayParms;
    MCIERROR		myResult;

    if (pmTrackString [0] == 0) pmTrack = -1;
    else
    {
	pmTrack = atoi (&pmTrackString [1]);
	if (pmTrack < 1) pmTrack = 1;
    }

    // If a CD is stPlaying, then stop it and close the channel.
    if (MyMusicFileStopAndClose (CD_AUDIO, TRUE)) return FALSE;

    // Initialize the CD player.
    myMciOpenParms.lpstrDeviceType = (LPCSTR) MCI_DEVTYPE_CD_AUDIO;
    myResult = mciSendCommand (0, MCI_OPEN, 
	MCI_OPEN_TYPE | MCI_OPEN_TYPE_ID, 
	(DWORD) (LPVOID) &myMciOpenParms);
    if (MyHandleMCIResult (E_MUSIC_CD_OPEN_ERROR, myResult)) return FALSE;

    stDeviceID [CD_AUDIO] = myMciOpenParms.wDeviceID;

    // Set the time format for the CD player
    myMciSetParms.dwTimeFormat = MCI_FORMAT_TMSF;
    myResult = mciSendCommand (stDeviceID [CD_AUDIO], MCI_SET, MCI_SET_TIME_FORMAT,
	(DWORD) (LPVOID) &myMciSetParms);
    if (MyHandleMCIResult (E_MUSIC_CD_OPEN_ERROR, myResult)) return FALSE;

    // Play the track
    myMciPlayParms.dwFrom = 0;
    myMciPlayParms.dwTo = 0;
    if (pmTrack != -1)
    {
	myMciPlayParms.dwFrom = MCI_MAKE_TMSF (pmTrack, 0, 0, 0);
	myMciPlayParms.dwTo = MCI_MAKE_TMSF (pmTrack + 1, 0, 0, 0);
	myResult = mciSendCommand (stDeviceID [CD_AUDIO], MCI_PLAY, 
	    MCI_FROM | MCI_TO, (DWORD) (LPVOID) &myMciPlayParms);
    }
    else
    {
	myResult = mciSendCommand (stDeviceID [CD_AUDIO], MCI_PLAY, 0,
	    (DWORD) (LPVOID) &myMciPlayParms);
    }
    if (MyHandleMCIResult (E_MUSIC_CD_PLAY_ERROR, myResult)) return FALSE;

    stPlayingFile [CD_AUDIO] = TRUE;
    *pmKind = CD_AUDIO;
    *pmSoundID = 0;
    return TRUE;
} // MDIOMusic_FileCDPlay


/************************************************************************/
/* MDIOMusic_FileMIDIPlay						*/
/************************************************************************/
BOOL	MDIOMusic_FileMIDIPlay (const char *pmFileName, int *pmKind,
				int *pmSoundID)
{
    MCI_OPEN_PARMS	myMciOpenParms;
    MCI_STATUS_PARMS	myMciStatusParms;
    MCI_SEEK_PARMS	myMciSeekParms;
    MCI_PLAY_PARMS	myMciPlayParms;
    MCIERROR		myResult;
    static char		stMyLastMIDIFileName [4096] = "\0";

    if ((stDeviceID [MIDI_AUDIO] == 0) ||
        (strcmp (pmFileName, stMyLastMIDIFileName) != 0))
    {
        /* If a MIDI file is stPlaying, then stop it and close the channel. */
        if (MyMusicFileStopAndClose (MIDI_AUDIO, TRUE)) return FALSE;

    	/* Initialize the MIDI sequencer. */
    	myMciOpenParms.lpstrDeviceType = (LPCSTR) MCI_DEVTYPE_SEQUENCER;
    	myMciOpenParms.lpstrElementName = pmFileName;
    	myResult = mciSendCommand (0, MCI_OPEN, 
	   MCI_OPEN_TYPE | MCI_OPEN_ELEMENT | MCI_OPEN_TYPE_ID, 
	   (DWORD) (LPVOID) &myMciOpenParms);
        if (MyHandleMCIResult (E_MUSIC_MIDI_OPEN_ERROR, myResult)) return FALSE;

        stDeviceID [MIDI_AUDIO] = myMciOpenParms.wDeviceID;

        /* Make certain that the output device is the MIDI mapper */
        myMciStatusParms.dwItem = MCI_SEQ_STATUS_PORT;
        myResult = mciSendCommand (stDeviceID [MIDI_AUDIO], MCI_STATUS, MCI_STATUS_ITEM,
	    (DWORD) (LPVOID) &myMciStatusParms);
        if (MyHandleMCIResult (E_MUSIC_MIDI_OPEN_ERROR, myResult)) return FALSE;
        if (LOWORD (myMciStatusParms.dwReturn) != (WORD) MIDI_MAPPER)
        {
	    SET_ERRMSG (E_MUSIC_CD_OPEN_ERROR, "MIDI Mapper not enabled");
	    return FALSE;
        }
        
        strcpy (stMyLastMIDIFileName, pmFileName);
    }
    else
    {
    	// Just rewind it instead
        myResult = mciSendCommand (stDeviceID [MIDI_AUDIO], MCI_SEEK, 
            MCI_SEEK_TO_START, (DWORD) (LPVOID) &myMciSeekParms);
        if (MyHandleMCIResult (E_MUSIC_MIDI_OPEN_ERROR, myResult)) return FALSE;
    }    	

    /* Play the file. */
    myResult = mciSendCommand (stDeviceID [MIDI_AUDIO], MCI_PLAY, 0,
	    (DWORD) (LPVOID) &myMciPlayParms);
    if (MyHandleMCIResult (E_MUSIC_MIDI_PLAY_ERROR, myResult)) return FALSE;
    stPlayingFile [MIDI_AUDIO] = TRUE;
    *pmKind = MIDI_AUDIO;
    *pmSoundID = 0;
    return TRUE;
} // MDIOMusic_FileMIDIPlay


/************************************************************************/
/* MDIOMusic_FileMP3Play						*/
/************************************************************************/
BOOL	MDIOMusic_FileMP3Play (const char *pmFileName, int *pmKind,
			       int *pmSoundID)
{
    MCI_OPEN_PARMS	myMciOpenParms;
    MCI_PLAY_PARMS	myMciPlayParms;
    MCIERROR		myResult;

    // If a MP3 file is stPlaying, then stop it and close the channel.
    if (MyMusicFileStopAndClose (MP3_AUDIO, TRUE)) return FALSE;

    // Initialize the MP3 player.
    myMciOpenParms.lpstrDeviceType = (LPCSTR) "MPEGVideo";
    myMciOpenParms.lpstrElementName = pmFileName;
    myResult = mciSendCommand (0, MCI_OPEN, 
	MCI_OPEN_TYPE | MCI_OPEN_ELEMENT, 
	(DWORD) (LPVOID) &myMciOpenParms);
    if (MyHandleMCIResult (E_MUSIC_WAVE_OPEN_ERROR, myResult)) return FALSE;

    stDeviceID [MP3_AUDIO] = myMciOpenParms.wDeviceID;

    // Play the file.
    myResult = mciSendCommand (stDeviceID [MP3_AUDIO], MCI_PLAY, 0,
	    (DWORD) (LPVOID) &myMciPlayParms);
    if (MyHandleMCIResult (E_MUSIC_WAVE_PLAY_ERROR, myResult)) return FALSE;
    stPlayingFile [MP3_AUDIO] = TRUE;
    *pmKind = MP3_AUDIO;
    *pmSoundID = 0;
    return TRUE;
} // MDIOMusic_FileMP3Play


/************************************************************************/
/* MDIOMusic_FileWAVEPlay						*/
/************************************************************************/
BOOL	MDIOMusic_FileWAVEPlay (const char *pmPathName, int *pmKind,
				int *pmSoundID)
{
    WAVEFile	*myInfo;
    BOOL	myResult = FALSE;

    myInfo = (WAVEFile *) MDIOMusic_PreLoadWAVE (pmPathName);
    if (myInfo != NULL)
    {
	myInfo -> freeWhenFinished = TRUE;

	*pmSoundID = stWAVEID++;
	stPlayingWAVEID = *pmSoundID;
	myInfo -> soundID = *pmSoundID;

	myResult = MyPlayWAVE (myInfo);
	if (myResult)
	{
	    stPlayingFile [WAVE_AUDIO] = TRUE;
	    *pmKind = WAVE_AUDIO;
	}

	LeaveCriticalSection (&stMusicCallback);
    }


    return myResult;
} // MDIOMusic_FileWAVEPlay


/************************************************************************/
/* MDIOMusic_FreePreLoad						*/
/************************************************************************/
void	MDIOMusic_FreePreLoad (void *pmMDInfo)
{
    WAVEFile	*myPtr = (WAVEFile *) pmMDInfo;
    free (myPtr -> buffer);
    free (myPtr);
} // MDIOMusic_FreePreLoad


/************************************************************************/
/* MDIOMusic_FreqPlay							*/
/*									*/
/* Returns TRUE if a frequency is now playing.				*/
/************************************************************************/
BOOL	MDIOMusic_FreqPlay (OOTint pmFrequency, int *pmSoundID)
{
    double		myAngle = 0;
    BOOL		myResult;
    int			cnt;
    
    // Fill the frequency sample buffer for the specified frequency
    for (cnt = 0 ; cnt < FREQ_BUFFER_SIZE ; cnt++)
    {
	stFreqBuffer [cnt] = (BYTE) (127 + 127 * sin (myAngle));	
	myAngle += 2 * PI * pmFrequency / SAMPLE_RATE;
	if (myAngle > 2 * PI) myAngle -= 2 * PI;
    }
  
    EnterCriticalSection (&stMusicCallback);

    myResult = MyPlayWAVE (NULL);
    if (myResult)
    {
	*pmSoundID = stWAVEID++;
	stPlayingWAVEID = *pmSoundID;
    }
	
    LeaveCriticalSection (&stMusicCallback);

    return myResult;
} // MDIOMusic_FreqPlay


/************************************************************************/
/* MDIOMusic_FreqStillPlaying						*/
/************************************************************************/
BOOL	MDIOMusic_FreqStillPlaying (int pmSoundID)
{
    return (stPlayingWAVEID == pmSoundID);
} // MDIOMusic_FreqStillPlaying


/************************************************************************/
/* MDIOMusic_FreqStop							*/
/************************************************************************/
void	MDIOMusic_FreqStop (void)
{
    if (stPlayingWAVE)
    {
	MyStopPlayWAVE ();
    }
} // MDIOMusic_FreqStop


/************************************************************************/
/* MDIOMusic_MusicFileStop						*/
/************************************************************************/
void	MDIOMusic_MusicFileStop (void)
{
    MyMusicFileStopAndClose (MIDI_AUDIO, TRUE);
    MyMusicFileStopAndClose (CD_AUDIO, TRUE);
    MyMusicFileStopAndClose (WAVE_AUDIO, TRUE);
    MyMusicFileStopAndClose (MP3_AUDIO, TRUE);
} // MDIOMusic_MusicFileStop


/************************************************************************/
/* MDIOMusic_NotePlay							*/
/************************************************************************/
BOOL	MDIOMusic_NotePlay (WORD pmMidiTone, WORD pmFrequency, WORD pmVolume)
{
    if (!stPlayingNote)
    {
	MMRESULT	myResult;

	myResult = midiOutOpen (&stMidiOut, MIDIMAPPER, 0, 0, 0);
	if (myResult != MMSYSERR_NOERROR)
	{
	    switch (myResult)
	    {
		case MIDIERR_NODEVICE:
		    SET_ERRMSG (E_MUSIC_NOTE_OPEN_ERROR, 
			"No MIDI port found");
		    break;
		case MMSYSERR_ALLOCATED:
		    SET_ERRMSG (E_MUSIC_NOTE_OPEN_ERROR, 
			"MIDI port already allocated");
		    break;
		default:
		    SET_ERRNO (E_MUSIC_NOTE_OPEN_ERROR);
	    }
	    return TRUE;
	}
	if (MyMidiOutMessage (0xC0, 0, 0, 0))
	{
	    MDIOMusic_NoteStop ();
	    return TRUE;
	}
	
	stPlayingNote = TRUE;
    }
    if (stPrevMidiTone != (WORD) -1)
    {
	if (MyMidiOutMessage (0x80, 0, stPrevMidiTone, 0))
	{
	    MDIOMusic_NoteStop ();
	    return TRUE;
	}
    }
    if (pmMidiTone != (WORD) -1)
    {
	if (MyMidiOutMessage (0x90, 0, pmMidiTone, 127))
	{
	    MDIOMusic_NoteStop ();
	    return TRUE;
	}
    }
    stPrevMidiTone = pmMidiTone;

    return FALSE;
} // MDIOMusic_NotePlay


/************************************************************************/
/* MDIOMusic_NoteStop							*/
/************************************************************************/
void	MDIOMusic_NoteStop (void)
{
    midiOutReset (stMidiOut);
    midiOutClose (stMidiOut);
    stPlayingNote = FALSE;
    stPrevMidiTone = -1;
    stMidiOut = NULL;
} // MDIOMusic_NoteStop


/************************************************************************/
/* MDIOMusic_PreLoadPlay						*/
/*									*/
/* Play a preloaded file (at this point, WAVE).  Returns TRUE if the	*/
/* file is starting to play.						*/
/************************************************************************/
BOOL	MDIOMusic_PreLoadPlay (void *pmMDInfo, int *pmKind, int *pmSoundID)
{
    BOOL    myResult;

    EnterCriticalSection (&stMusicCallback);

    myResult = MyPlayWAVE ((WAVEFile *) pmMDInfo);
    if (myResult)
    {
	stPlayingFile [WAVE_AUDIO] = TRUE;
	*pmKind = WAVE_AUDIO;
	*pmSoundID = stWAVEID++;
	stPlayingWAVEID = *pmSoundID;
    }

    LeaveCriticalSection (&stMusicCallback);

    return myResult;
} // MDIOMusic_PreLoadPlay


/************************************************************************/
/* MDIOMusic_PreLoadWAVE						*/
/************************************************************************/
void	*MDIOMusic_PreLoadWAVE (const char *pmPathName)
{
    HANDLE	myFile;
    char	myBuffer [10];
    DWORD 	myFileSize; // = m_buffer.GetNumSamples() * m_pcmWaveFormat.nBlockAlign + 36
    DWORD	myFormatSize; // = 16L
    DWORD	mySampleSize, myNumSamples, myBufferSize;
    DWORD	myNum;
    WAVEFile	*myWAVEFile;
            
    myFile = CreateFile (pmPathName, GENERIC_READ, FILE_SHARE_READ, NULL,
    			 OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (myFile == INVALID_HANDLE_VALUE)
    {
    	DWORD	myError = GetLastError ();
    	
    	if (myError == ERROR_FILE_NOT_FOUND)
    	{
	    SET_ERRMSG (E_FSYS_FILE_NOT_FOUND, "File '%s' not found", 
			pmPathName);
    	}
    	else if (myError == ERROR_PATH_NOT_FOUND)
    	{
    	    SET_ERRMSG (E_FSYS_PATH_NOT_FOUND, 
    	 	        "Path not found (couldn't find the directory "
    	 	        "that file '%s' is in)", pmPathName);
    	}
    	else
    	{
	    MIO_MoreInfo ("LoadMusic: Problem opening music file '%s' (%d)",
		          pmPathName, GetLastError ());
	    SET_ERRNO (E_MUSIC_IO_ERROR);
	}		          
    	return NULL;
    }

    //
    // First four bytes must be RIFF
    //
    ZeroMemory (myBuffer, 10);
    if (!MyReadBytes (pmPathName, myFile, myBuffer, 4))
    {
	MIO_MoreInfo ("LoadMusic: '%s': Read Failed (1)", pmPathName);
    	SET_ERRNO (E_MUSIC_BAD_MUSIC_FILE);
    	CloseHandle (myFile);
    	return NULL;
    }
    
    if (strncmp (myBuffer, "RIFF", 4) != 0) 
    {
	MIO_MoreInfo ("LoadMusic: '%s': Bad WAVE header (1)", pmPathName);
    	SET_ERRNO (E_MUSIC_BAD_MUSIC_FILE);
    	CloseHandle (myFile);
    	return NULL;
    }

    //
    // Get file size
    //
    if (!MyReadBytes (pmPathName, myFile, &myFileSize, sizeof (DWORD)))
    {
	MIO_MoreInfo ("LoadMusic: '%s': Read Failed (2)", pmPathName);
    	SET_ERRNO (E_MUSIC_BAD_MUSIC_FILE);
    	CloseHandle (myFile);
    	return NULL;
    }
    
    //
    // Next 8 bytes should read "WAVEfmt "
    //
    ZeroMemory (myBuffer, 10);
    if (!MyReadBytes (pmPathName, myFile, myBuffer, 8))
    {
	MIO_MoreInfo ("LoadMusic: '%s': Read Failed (3)", pmPathName);
    	SET_ERRNO (E_MUSIC_BAD_MUSIC_FILE);
    	CloseHandle (myFile);
    	return NULL;
    }
    
    if (strncmp (myBuffer, "WAVEfmt ", 8) != 0) 
    {
	MIO_MoreInfo ("LoadMusic: '%s': Bad WAVE header (2)", pmPathName);
    	SET_ERRNO (E_MUSIC_BAD_MUSIC_FILE);
    	CloseHandle (myFile);
    	return NULL;
    }

    // 
    // Read the format size
    //
    if (!MyReadBytes (pmPathName, myFile, &myFormatSize, sizeof (DWORD)))
    {
	MIO_MoreInfo ("LoadMusic: '%s': Read Failed (4)", pmPathName);
    	SET_ERRNO (E_MUSIC_BAD_MUSIC_FILE);
    	CloseHandle (myFile);
    	return NULL;
    }
    
    // Okay, allocate the space for the preloaded file.
    myWAVEFile = (WAVEFile *) malloc (sizeof (WAVEFile));
    myWAVEFile -> freeWhenFinished = FALSE;

    // 
    // Read the header
    //
    if (!MyReadBytes (pmPathName, myFile, &myWAVEFile -> wave.wFormatTag, 
    		     sizeof (myWAVEFile -> wave.wFormatTag)))
    {
	MIO_MoreInfo ("LoadMusic: '%s': Read Failed (5)", pmPathName);
    	SET_ERRNO (E_MUSIC_BAD_MUSIC_FILE);
    	CloseHandle (myFile);
    	free (myWAVEFile);
    	return NULL;
    }
    if (!MyReadBytes (pmPathName, myFile, &myWAVEFile -> wave.nChannels, 
    		     sizeof (myWAVEFile -> wave.nChannels)))
    {
	MIO_MoreInfo ("LoadMusic: '%s': Read Failed (6)", pmPathName);
    	SET_ERRNO (E_MUSIC_BAD_MUSIC_FILE);
    	CloseHandle (myFile);
    	free (myWAVEFile);
    	return NULL;
    }
    if (!MyReadBytes (pmPathName, myFile, &myWAVEFile -> wave.nSamplesPerSec, 
    		     sizeof (myWAVEFile -> wave.nSamplesPerSec)))
    {
	MIO_MoreInfo ("LoadMusic: '%s': Read Failed (7)", pmPathName);
    	SET_ERRNO (E_MUSIC_BAD_MUSIC_FILE);
    	CloseHandle (myFile);
    	free (myWAVEFile);
    	return NULL;
    }
    if (!MyReadBytes (pmPathName, myFile, &myWAVEFile -> wave.nAvgBytesPerSec, 
    		     sizeof (myWAVEFile -> wave.nAvgBytesPerSec)))
    {
	MIO_MoreInfo ("LoadMusic: '%s': Read Failed (8)", pmPathName);
    	SET_ERRNO (E_MUSIC_BAD_MUSIC_FILE);
    	CloseHandle (myFile);
    	free (myWAVEFile);
    	return NULL;
    }
    if (!MyReadBytes (pmPathName, myFile, &myWAVEFile -> wave.nBlockAlign, 
    		     sizeof (myWAVEFile -> wave.nBlockAlign)))
    {
	MIO_MoreInfo ("LoadMusic: '%s': Read Failed (9)", pmPathName);
    	SET_ERRNO (E_MUSIC_BAD_MUSIC_FILE);
    	CloseHandle (myFile);
    	free (myWAVEFile);
    	return NULL;
    }
    if (!MyReadBytes (pmPathName, myFile, &myWAVEFile -> wave.wBitsPerSample, 
    		     sizeof (myWAVEFile -> wave.wBitsPerSample)))
    {
	MIO_MoreInfo ("LoadMusic: '%s': Read Failed (10)", pmPathName);
    	SET_ERRNO (E_MUSIC_BAD_MUSIC_FILE);
    	CloseHandle (myFile);
    	free (myWAVEFile);
    	return NULL;
    }
    		     
    //
    // Next 4 bytes should read "data"
    //
    ZeroMemory (myBuffer, 10);
    if (!MyReadBytes (pmPathName, myFile, myBuffer, 4))
    {
	MIO_MoreInfo ("LoadMusic: '%s': Read Failed (11)", pmPathName);
    	SET_ERRNO (E_MUSIC_BAD_MUSIC_FILE);
    	CloseHandle (myFile);
    	free (myWAVEFile);
    	return NULL;
    }
    
    if (strncmp (myBuffer, "data", 4) != 0) 
    {
	MIO_MoreInfo ("LoadMusic: '%s': Bad WAVE header (3)", pmPathName);
    	SET_ERRNO (E_MUSIC_BAD_MUSIC_FILE);
    	CloseHandle (myFile);
    	free (myWAVEFile);
    	return NULL;
    }

    //
    // Read data
    // 
    if (!MyReadBytes (pmPathName, myFile, &myNum, sizeof (myNum)))
    {
	MIO_MoreInfo ("LoadMusic: '%s': Read Failed (12)", pmPathName);
    	SET_ERRNO (E_MUSIC_BAD_MUSIC_FILE);
    	CloseHandle (myFile);
    	free (myWAVEFile);
    	return NULL;
    }
    
    myNumSamples = myNum / myWAVEFile -> wave.nBlockAlign;
    mySampleSize = myWAVEFile -> wave.nBlockAlign;
    myBufferSize = myNumSamples * mySampleSize;
    myWAVEFile -> buffer = malloc (myBufferSize);
    if (!MyReadBytes (pmPathName, myFile, myWAVEFile -> buffer, 
    		     myBufferSize))
    {
	MIO_MoreInfo ("LoadMusic: '%s': Read Failed (13)", pmPathName);
    	SET_ERRNO (E_MUSIC_BAD_MUSIC_FILE);
    	CloseHandle (myFile);
    	free (myWAVEFile -> buffer);
    	free (myWAVEFile);
    	return NULL;
    }

    CloseHandle (myFile);

    myWAVEFile -> waveHdr.lpData		= myWAVEFile -> buffer;
    myWAVEFile -> waveHdr.dwBufferLength	= myBufferSize;
    myWAVEFile -> waveHdr.dwBytesRecorded	= 0;
    myWAVEFile -> waveHdr.dwUser		= 0;
    myWAVEFile -> waveHdr.dwFlags		= 0;
    myWAVEFile -> waveHdr.dwLoops		= 1;
    myWAVEFile -> waveHdr.lpNext		= NULL;
    myWAVEFile -> waveHdr.reserved		= 0;

    return myWAVEFile;    
} // MDIOMusic_PreLoadWAVE


/******************************/
/* Static callback procedures */
/******************************/


/*********************/
/* Static procedures */
/*********************/
/************************************************************************/
/* MyHandleMCIResult							*/
/************************************************************************/
static BOOL MyHandleMCIResult (int pmError, MCIERROR pmResult)
{
    char	myErrorMessage [1024];
    
    if (pmResult != 0)
    {
	if (mciGetErrorString (pmResult, (LPSTR) myErrorMessage, 1024))
	{
	    SET_ERRMSG (pmError, myErrorMessage);
	}
	else
	{
	    SET_ERRNO (pmError);
	}
	return TRUE;
    }
    return FALSE;
} // MyHandleMCIResult


/************************************************************************/
/* MyMidiOutMessage							*/
/************************************************************************/
static BOOL	MyMidiOutMessage (int pmCommand, int pmChannel, int pmData1, 
				  int pmData2)
{
    MMRESULT	myResult;
    DWORD	myMessage;

    myMessage =  pmCommand | pmChannel | (pmData1 << 8) | (pmData2 << 16);
    myResult = midiOutShortMsg (stMidiOut, myMessage);
    if (myResult != MMSYSERR_NOERROR)
    {
	switch (myResult)
	{
	    case MIDIERR_NOTREADY:
		SET_ERRMSG (E_MUSIC_NOTE_PLAY_ERROR, 
		    "MIDI port not ready");
		break;
	    default:
		SET_ERRNO (E_MUSIC_NOTE_PLAY_ERROR);
	}
	return TRUE;
    }
    return FALSE;
} // MyMidiOutMessage


/************************************************************************/
/* MyMusicFileStopAndClose						*/
/*									*/
/* Stops a device, if necessary, and then closes it for use by other	*/
/* processes.								*/
/************************************************************************/
static BOOL MyMusicFileStopAndClose (int pmKind, BOOL pmReallyClose)
{
    MCIERROR	myResult;

    if (pmKind == WAVE_AUDIO)
    {
	if (stPlayingWAVE)
	{
	    MyStopPlayWAVE ();
	}
    }

    // Stop the stPlaying, if applicable.
    if (stPlayingFile [pmKind])
    {
	myResult = mciSendCommand (stDeviceID [pmKind], MCI_STOP, MCI_WAIT, 
	    (DWORD) (LPVOID) NULL);
	if (MyHandleMCIResult (E_MUSIC_CLOSE_ERROR, myResult))
	{
	    return TRUE;
	}
	stPlayingFile [pmKind] = FALSE;
    }

    // We actually try and keep the MIDI audio around because it is often used
    // as background music in a loop.
    if ((pmKind != MIDI_AUDIO) || pmReallyClose)
    {
    	// Close the device, if open.
    	if (stDeviceID [pmKind] != 0)
    	{
	    myResult = mciSendCommand (stDeviceID [pmKind], MCI_CLOSE, MCI_WAIT,
	        (DWORD) (LPVOID) NULL);
	    if (MyHandleMCIResult (E_MUSIC_CLOSE_ERROR, myResult)) 
	    {
	        return TRUE;
	    }
	    stDeviceID [pmKind] = 0;
        }
    }
    return FALSE;
} // MyMusicFileStopAndClose


/************************************************************************/
/* MyPlayWAVE								*/
/*									*/
/* This plays either the WAVE buffer specified by the parameter, or if	*/
/* that is NULL, it plays the WAVE file specified by stWAVEFormat and	*/
/* stWAVEHeader.							*/
/************************************************************************/
static BOOL	MyPlayWAVE (WAVEFile *pmWAVEFile)
{
    WAVEFORMATEX	*myWAVEFormatPtr;
    WAVEHDR		*myWAVEHeaderPtr;
    // Note that WAVE files need a callback when completed.
    DWORD		myWAVEOutProc;
    DWORD		myWAVEData;
    DWORD		myWAVECallBack;
    MMRESULT		myResult;

    // If we're currently playing a frequency, stop it right now.
    if (stPlayingWAVE)
    {
	MyStopPlayWAVE ();
    }

    // If we were playing a frequency and we're about to again, skip
    // unloading the special frequency header.
    if (stFreqHeaderLoaded && (pmWAVEFile != NULL))
    {
	waveOutClose (stWAVEOut);
	waveOutUnprepareHeader (stWAVEOut, &stWAVEHeaderForFreq, 
				sizeof (WAVEHDR));
	stFreqHeaderLoaded = FALSE;
    }

    if (pmWAVEFile == NULL)
    {
	myWAVEFormatPtr = &stWAVEFormatForFreq;
	myWAVEHeaderPtr = &stWAVEHeaderForFreq;
	myWAVEOutProc = 0;
	myWAVEData = 0;
	myWAVECallBack = 0;
    }
    else
    {
	myWAVEFormatPtr = &pmWAVEFile -> wave;
	myWAVEHeaderPtr = &pmWAVEFile -> waveHdr;
	myWAVEOutProc = (DWORD) MyWAVEOutProcedure;
	myWAVEData = (DWORD) pmWAVEFile;
	myWAVECallBack = CALLBACK_FUNCTION;
    }

    // If the frequency
    if (!stFreqHeaderLoaded || (pmWAVEFile != NULL))
    {
	myResult = waveOutOpen (&stWAVEOut, WAVE_MAPPER, myWAVEFormatPtr, 
			        myWAVEOutProc, myWAVEData, myWAVECallBack);
	if (myResult != MMSYSERR_NOERROR)
	{
	    switch (myResult)
	    {
		case MMSYSERR_NODRIVER:
		    SET_ERRMSG (E_MUSIC_FREQ_OPEN_ERROR, 
			"No device driver for WAVE player found");
		    break;
		case MMSYSERR_ALLOCATED:
		    SET_ERRMSG (E_MUSIC_FREQ_OPEN_ERROR, 
			"WAVE player already allocated");
		    break;
		default:
		    SET_ERRNO (E_MUSIC_FREQ_OPEN_ERROR);
	    }
	    return FALSE;
	}

	myResult = waveOutPrepareHeader (stWAVEOut, myWAVEHeaderPtr, 
					 sizeof (WAVEHDR));
	if (myResult != MMSYSERR_NOERROR)
	{
	    switch (myResult)
	    {
		case MMSYSERR_NODRIVER:
		    SET_ERRMSG (E_MUSIC_FREQ_OPEN_ERROR, 
			"No device driver for WAVE player found");
		    break;
		case MMSYSERR_ALLOCATED:
		    SET_ERRMSG (E_MUSIC_FREQ_OPEN_ERROR, 
			"WAVE player already allocated");
		    break;
		default:
		    SET_ERRNO (E_MUSIC_FREQ_OPEN_ERROR);
	    }
	    return FALSE;
	}

	if (pmWAVEFile == NULL)
	{
	    stFreqHeaderLoaded = TRUE;
	}
    }

    // We've correctly set out the headers, now let's play the WAVE
    myResult = waveOutWrite (stWAVEOut, myWAVEHeaderPtr, sizeof (WAVEHDR));
    if (myResult != MMSYSERR_NOERROR)
    {
	switch (myResult)
	{
	    case MMSYSERR_NODRIVER:
		SET_ERRMSG (E_MUSIC_FREQ_PLAY_ERROR, 
		    "No device driver for WAVE player found");
		break;
	    default:
		SET_ERRNO (E_MUSIC_FREQ_PLAY_ERROR);
	}
	return FALSE;
    }

    stPlayingWAVE = TRUE;

    return TRUE;
} // MyPlayWAVE


/************************************************************************/
/* MyReadBytes								*/
/************************************************************************/
static BOOL	MyReadBytes (const char *pmPathName, HANDLE pmFile, 
			     void *pmBuffer, DWORD pmBytes)
{
    BOOL		mySuccess;
    DWORD		myBytesRead;

    mySuccess = ReadFile (pmFile, pmBuffer, pmBytes, &myBytesRead, NULL);    
    if (!mySuccess)
    {
	MIO_MoreInfo ("LoadMusic: Problem reading music file '%s' (%d)",
		      pmPathName, GetLastError ());
	SET_ERRNO (E_MUSIC_IO_ERROR);
    	CloseHandle (pmFile);
    	return FALSE;
    }
    
    if (myBytesRead != pmBytes)
    {
	MIO_MoreInfo ("LoadMusic: Read failure: Only read %d of %d bytes",
		      myBytesRead, pmBytes);
	SET_ERRNO (E_MUSIC_IO_ERROR);
    	CloseHandle (pmFile);
    	return FALSE;
    }
    
    return TRUE;
} // MyReadBytes


/************************************************************************/
/* MyStopPlayWAVE							*/
/************************************************************************/
static void	MyStopPlayWAVE (void)
{
    EnterCriticalSection (&stMusicCallback);

    waveOutReset (stWAVEOut);
    stPlayingWAVE = FALSE;
    stPlayingWAVEID = 0;

    LeaveCriticalSection (&stMusicCallback);
} // MyStopPlayWAVE


/************************************************************************/
/* MyWAVEOutProcedure							*/
/************************************************************************/
static void CALLBACK	MyWAVEOutProcedure (HWAVEOUT pmWAVEOut, UINT pmMsg, 
					    DWORD pmInstance, DWORD pmParam1, 
					    DWORD pmParam2)
{
    WAVEHDR	*myWAVEHeader;
    
    if (pmMsg == MM_WOM_DONE)
    {
	EnterCriticalSection (&stMusicCallback);

	myWAVEHeader = (WAVEHDR *) pmParam1;
	waveOutUnprepareHeader (pmWAVEOut, myWAVEHeader, sizeof(WAVEHDR));
	if (((WAVEFile *) pmInstance) -> freeWhenFinished)
	{
	    MDIOMusic_FreePreLoad (((WAVEFile *) pmInstance));
	}
	stPlayingWAVE = FALSE;
	stPlayingWAVEID = 0;
	stPlayingFile [WAVE_AUDIO] = FALSE;

	LeaveCriticalSection (&stMusicCallback);
    }
} // MyWAVEOutProcedure
