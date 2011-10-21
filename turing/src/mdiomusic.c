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

/**********************/
/* External variables */
/**********************/

/********************/
/* Static constants */
/********************/

/********************/
/* Static variables */
/********************/
static BOOL	stPlaying [] = {FALSE, FALSE, FALSE, FALSE, FALSE};
static int	stDeviceID [] = {0, 0, 0, 0, 0};
static BOOL	stPlayingNote = FALSE, stPlayingFreq = FALSE;
static BOOL	stFreqOpen = FALSE;
static WORD	stPrevMidiTone = (WORD) -1;
static BYTE	stFreqBuffer [FREQ_BUFFER_SIZE];
static HMIDIOUT	stMidiOut;
static HWAVEOUT	stWaveOut;
static WAVEHDR	stWaveHeader;

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


/***********************/
/* External procedures */
/***********************/
/************************************************************************/
/* MDIOMusic_Init							*/
/************************************************************************/
void	MDIOMusic_Init (void)
{
} // MDIOMusic_Init


/************************************************************************/
/* MDIOMusic_EventCheckMusicFile					*/
/************************************************************************/
BOOL	MDIOMusic_EventCheckMusicFile (int pmKind)
{
    if (stPlaying [pmKind])
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
	    stPlaying [pmKind] = FALSE;
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
BOOL	MDIOMusic_FileCDPlay (const char *pmTrackString, int *pmKind)
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

    stPlaying [CD_AUDIO] = TRUE;
    *pmKind = CD_AUDIO;
    return TRUE;
} // MDIOMusic_FileCDPlay


/************************************************************************/
/* MDIOMusic_FileMIDIPlay						*/
/************************************************************************/
BOOL	MDIOMusic_FileMIDIPlay (const char *pmFileName, int *pmKind)
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
    stPlaying [MIDI_AUDIO] = TRUE;
    *pmKind = MIDI_AUDIO;
    return TRUE;
} // MDIOMusic_FileMIDIPlay


/************************************************************************/
/* MDIOMusic_FileMP3Play						*/
/************************************************************************/
BOOL	MDIOMusic_FileMP3Play (const char *pmFileName, int *pmKind)
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
    stPlaying [MP3_AUDIO] = TRUE;
    *pmKind = MP3_AUDIO;
    return TRUE;
} // MDIOMusic_FileMP3Play


/************************************************************************/
/* MDIOMusic_FileWAVEPlay						*/
/************************************************************************/
BOOL	MDIOMusic_FileWAVEPlay (const char *pmFileName, int *pmKind)
{
    MCI_OPEN_PARMS	myMciOpenParms;
    MCI_PLAY_PARMS	myMciPlayParms;
    MCIERROR		myResult;

    // If a WAVE frequency is stPlaying, then return unsuccessfully.
    if (stPlayingFreq)
    {
	SET_ERRMSG (E_MUSIC_WAVE_OPEN_ERROR,
	    "Can't play a WAVE file and a frequency at the same time.");
	return FALSE;
    }

    // If a WAVE frequency player has been initialized, then uninitialize it.
    MyFreqClose ();

    // If a WAVE file is stPlaying, then stop it and close the channel.
    if (MyMusicFileStopAndClose (WAVE_AUDIO, TRUE)) return FALSE;

    // Initialize the WAVE player.
    myMciOpenParms.lpstrDeviceType = (LPCSTR) MCI_DEVTYPE_WAVEFORM_AUDIO;
    myMciOpenParms.lpstrElementName = pmFileName;
    myResult = mciSendCommand (0, MCI_OPEN, 
	MCI_OPEN_TYPE | MCI_OPEN_ELEMENT | MCI_OPEN_TYPE_ID, 
	(DWORD) (LPVOID) &myMciOpenParms);
    if (MyHandleMCIResult (E_MUSIC_WAVE_OPEN_ERROR, myResult)) return FALSE;

    stDeviceID [WAVE_AUDIO] = myMciOpenParms.wDeviceID;

    // Play the file.
    myResult = mciSendCommand (stDeviceID [WAVE_AUDIO], MCI_PLAY, 0,
	    (DWORD) (LPVOID) &myMciPlayParms);
    if (MyHandleMCIResult (E_MUSIC_WAVE_PLAY_ERROR, myResult)) return FALSE;
    stPlaying [WAVE_AUDIO] = TRUE;
    *pmKind = WAVE_AUDIO;
    return TRUE;
} // MDIOMusic_FileWAVEPlay


/************************************************************************/
/* MDIOMusic_Finalize							*/
/************************************************************************/
void	MDIOMusic_Finalize (void)
{
    mciSendCommand (MCI_ALL_DEVICE_ID, MCI_CLOSE, MCI_WAIT, 0);
} // MDIOMusic_Finalize


/************************************************************************/
/* MDIOMusic_FreqPlay							*/
/************************************************************************/
BOOL	MDIOMusic_FreqPlay (OOTint pmFrequency)
{
    WAVEFORMATEX	myWaveFormat;
    MMRESULT		myResult;
    double		myAngle = 0;
    int			cnt;
    
    if (stPlayingFreq)
    {
	MDIOMusic_FreqStop ();
    }

    if (!stFreqOpen)
    {
	myWaveFormat.wFormatTag		= WAVE_FORMAT_PCM;
	myWaveFormat.nChannels		= 1;
	myWaveFormat.nSamplesPerSec	= SAMPLE_RATE;
	myWaveFormat.nAvgBytesPerSec	= SAMPLE_RATE;
	myWaveFormat.nBlockAlign	= 1;
	myWaveFormat.wBitsPerSample	= 8;
	myWaveFormat.cbSize		= 0;

	myResult = waveOutOpen (&stWaveOut, WAVE_MAPPER, &myWaveFormat, 
			        0, 0, 0);
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
	    return TRUE;
	}

	stWaveHeader.lpData		= stFreqBuffer;
	stWaveHeader.dwBufferLength	= FREQ_BUFFER_SIZE;
	stWaveHeader.dwBytesRecorded	= 0;
	stWaveHeader.dwUser		= 0;
	stWaveHeader.dwFlags		= WHDR_BEGINLOOP | WHDR_ENDLOOP;
	stWaveHeader.dwLoops		= 10000;
	stWaveHeader.lpNext		= NULL;
	stWaveHeader.reserved		= 0;

	myResult = waveOutPrepareHeader (stWaveOut, &stWaveHeader, 
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
	    return TRUE;
	}

	stFreqOpen = TRUE;
    }

    for (cnt = 0 ; cnt < FREQ_BUFFER_SIZE ; cnt++)
    {
	stFreqBuffer [cnt] = (BYTE) (127 + 127 * sin (myAngle));	
	myAngle += 2 * PI * pmFrequency / SAMPLE_RATE;
	if (myAngle > 2 * PI) myAngle -= 2 * PI;
    }
  
    myResult = waveOutWrite (stWaveOut, &stWaveHeader, sizeof (WAVEHDR));
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
	return TRUE;
    }

    stPlayingFreq = TRUE;

    return FALSE;
} // MDIOMusic_FreqPlay


/************************************************************************/
/* MDIOMusic_FreqStop							*/
/************************************************************************/
void	MDIOMusic_FreqStop (void)
{
    if (stPlayingFreq)
    {
	waveOutReset (stWaveOut);
	stPlayingFreq = FALSE;
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


/******************************/
/* Static callback procedures */
/******************************/


/*********************/
/* Static procedures */
/*********************/
/************************************************************************/
/* MyFreqClose								*/
/************************************************************************/
static void	MyFreqClose (void)
{
    MDIOMusic_FreqStop ();
    if (stFreqOpen)
    {
	waveOutClose (stWaveOut);
	waveOutUnprepareHeader (stWaveOut, &stWaveHeader, sizeof (WAVEHDR));
	stFreqOpen = FALSE;
    }
} // MyFreqClose


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

    // Stop the stPlaying, if applicable.
    if (stPlaying [pmKind])
    {
	myResult = mciSendCommand (stDeviceID [pmKind], MCI_STOP, MCI_WAIT, 
	    (DWORD) (LPVOID) NULL);
	if (MyHandleMCIResult (E_MUSIC_CLOSE_ERROR, myResult))
	{
	    return TRUE;
	}
	stPlaying [pmKind] = FALSE;
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
