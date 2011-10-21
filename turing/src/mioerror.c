/************************************************************************/
/* mioerror.c								*/
/************************************************************************/

/*******************/
/* System includes */
/*******************/
#include <string.h>
#include <stdarg.h>

/****************/
/* Self include */
/****************/
#include "mioerror.h"

/******************/
/* Other includes */
/******************/
#include "mio.h"
#include "mioerr.h"
#include "mioerrstr.in"
#include "mioerrmsg.in"

#include "language.h"

#include "mdio.h"

#include "edint.h"

#include "miodialogs.h"

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/

/********************/
/* Global variables */
/********************/

/*********/
/* Types */
/*********/
struct ErrorCheck
{
    int	firstError, lastError;
    int sizeOfMessages;
    int sizeOfStrings;
} ErrorCheck;

/**********************/
/* External variables */
/**********************/

/********************/
/* Static constants */
/********************/

/********************/
/* Static variables */
/********************/
static struct ErrorCheck stErrorTypes [] = 
{
    {E_Z_FIRST_NO_ERROR, E_Z_LAST_NO_ERROR, sizeof (noneErrMsg), sizeof (noneErrStr)},
    {E_Z_FIRST_GENERAL_ERROR, E_Z_LAST_GENERAL_ERROR, sizeof (generalErrMsg), sizeof (generalErrStr)},
    {E_Z_FIRST_FSYS_ERROR, E_Z_LAST_FSYS_ERROR, sizeof (fileSysErrMsg), sizeof (fileSysErrStr)},
    {E_Z_FIRST_STREAM_ERROR, E_Z_LAST_STREAM_ERROR, sizeof (streamErrMsg), sizeof (streamErrStr)},
    {E_Z_FIRST_FILE_ERROR, E_Z_LAST_FILE_ERROR, sizeof (fileErrMsg), sizeof (fileErrStr)},
    {E_Z_FIRST_DIR_ERROR, E_Z_LAST_DIR_ERROR, sizeof (dirErrMsg), sizeof (dirErrStr)},
    {E_Z_FIRST_PIC_ERROR, E_Z_LAST_PIC_ERROR, sizeof (picErrMsg), sizeof (picErrStr)},
    {E_Z_FIRST_PEN_ERROR, E_Z_LAST_PEN_ERROR, sizeof (penErrMsg), sizeof (penErrStr)},
    {E_Z_FIRST_BRUSH_ERROR, E_Z_LAST_BRUSH_ERROR, sizeof (brushErrMsg), sizeof (brushErrStr)},
    {E_Z_FIRST_SPRITE_ERROR, E_Z_LAST_SPRITE_ERROR, sizeof (spriteErrMsg), sizeof (spriteErrStr)},
    {E_Z_FIRST_FONT_ERROR, E_Z_LAST_FONT_ERROR, sizeof (fontErrMsg), sizeof (fontErrStr)},
    {E_Z_FIRST_DRAW_ERROR, E_Z_LAST_DRAW_ERROR, sizeof (drawErrMsg), sizeof (drawErrStr)},
    {E_Z_FIRST_TIME_ERROR, E_Z_LAST_TIME_ERROR, sizeof (timeErrMsg), sizeof (timeErrStr)},
    {E_Z_FIRST_MOUSE_ERROR, E_Z_LAST_MOUSE_ERROR, sizeof (mouseErrMsg), sizeof (mouseErrStr)},
    {E_Z_FIRST_RGB_ERROR, E_Z_LAST_RGB_ERROR, sizeof (rgbErrMsg), sizeof (rgbErrStr)},
    {E_Z_FIRST_MUSIC_ERROR, E_Z_LAST_MUSIC_ERROR, sizeof (musicErrMsg), sizeof (musicErrStr)},
    {E_Z_FIRST_CONFIG_ERROR, E_Z_LAST_CONFIG_ERROR, sizeof (configErrMsg), sizeof (configErrStr)},
    {E_Z_FIRST_VIEW_ERROR, E_Z_LAST_VIEW_ERROR, sizeof (viewErrMsg), sizeof (viewErrStr)},
    {E_Z_FIRST_WINDOW_ERROR, E_Z_LAST_WINDOW_ERROR, sizeof (windowErrMsg), sizeof (windowErrStr)},
    {E_Z_FIRST_PRINTER_ERROR, E_Z_LAST_PRINTER_ERROR, sizeof (printerErrMsg), sizeof (printerErrStr)},
    {E_Z_FIRST_TEXT_ERROR, E_Z_LAST_TEXT_ERROR, sizeof (textErrMsg), sizeof (textErrStr)},
    {E_Z_FIRST_GUI_ERROR, E_Z_LAST_GUI_ERROR, sizeof (guiErrMsg), sizeof (guiErrStr)},
    {E_Z_FIRST_LEX_ERROR, E_Z_LAST_LEX_ERROR, sizeof (lexerErrMsg), sizeof (lexerErrStr)},
    {E_Z_FIRST_NET_ERROR, E_Z_LAST_NET_ERROR, sizeof (netErrMsg), sizeof (netErrStr)},
    {E_Z_FIRST_JOY_ERROR, E_Z_LAST_JOY_ERROR, sizeof (joystickErrMsg), sizeof (joystickErrStr)},
    {E_Z_FIRST_PC_ERROR, E_Z_LAST_PC_ERROR, sizeof (pcErrMsg), sizeof (pcErrStr)},
    {E_Z_FIRST_SYS_ERROR, E_Z_LAST_SYS_ERROR, sizeof (sysErrMsg), sizeof (sysErrStr)},
    {E_Z_FIRST_INI_FILE_ERROR, E_Z_LAST_INI_FILE_ERROR, sizeof (iniFileErrMsg), sizeof (iniFileErrStr)},
    {E_Z_FIRST_STR_ERROR, E_Z_LAST_STR_ERROR, sizeof (strErrMsg), sizeof (strErrStr)},
    {E_Z_FIRST_CRYPTO_ERROR, E_Z_LAST_CRYPTO_ERROR, sizeof (cryptoErrMsg), sizeof (cryptoErrStr)},
};
static int	stNumErrorTypes; 
static int	stNumErrors [100];
static BOOL	stEchoLibErrorsToStderr = FALSE;

/******************************/
/* Static callback procedures */
/******************************/

/*********************/
/* Static procedures */
/*********************/
static void	MyAbortErrMsg (int pmErrNo, char *pmErrMsg);
static void	MySetErrMsg (int pmErrNo, char *pmErrMsg);


/***********************/
/* External procedures */
/***********************/
/************************************************************************/
/* MIOError_Init							*/
/************************************************************************/
void	MIOError_Init (void)
{
    int myNumErrorMsgs, myNumErrorStrs;
    int	cnt;
    
    // Calculate the number of error types
    stNumErrorTypes = sizeof (stErrorTypes) / sizeof (ErrorCheck);

    // Make certain all error numbers, error messages and error names are
    // the same size.
    for (cnt = 0 ; cnt < stNumErrorTypes ; cnt++)
    {
        stNumErrors [cnt] = stErrorTypes [cnt].lastError - 
        		    stErrorTypes [cnt].firstError - 1;
	myNumErrorMsgs = stErrorTypes [cnt].sizeOfMessages / sizeof (char *);
	myNumErrorStrs = stErrorTypes [cnt].sizeOfStrings / sizeof (char *);
        if (stNumErrors [cnt] != myNumErrorMsgs)
    	{
    	    EdInt_HaltEnvironment (IDS_MIO_MISMATCHEDERRORMSGS, __FILE__, 
    	    			   __LINE__, 0, cnt, stNumErrors [cnt],
				   myNumErrorMsgs);
    	    return;
    	}
        if (stNumErrors [cnt] != myNumErrorStrs)
    	{
    	    EdInt_HaltEnvironment (IDS_MIO_MISMATCHEDERRORSTRS, __FILE__, 
    	    			   __LINE__, 0, cnt, stNumErrors [cnt],
				   myNumErrorStrs);
    	    return;
	}
    }
} // MIOError_Init


/************************************************************************/
/* MIOError_Abort							*/
/************************************************************************/
void	MIOError_Abort (OOTint pmErrNo, OOTstring pmMessage)
{
    if (pmErrNo <= 0)
    {
    	pmErrNo = E_USER_PROGRAM_ACTIVATED_ERROR;
    }

    if ((pmMessage == NULL) || (pmMessage [0] == 0))
    {
    	MIOError_AbortWithErrNo (pmErrNo);
    }
    else
    {
    	MyAbortErrMsg (pmErrNo, pmMessage);
    }
} // MIOError_Abort


/************************************************************************/
/* MIOError_EchoLibErrorsToStderr					*/
/************************************************************************/
void	MIOError_EchoLibErrorsToStderr (BOOL pmEchoLibErrorsToStderr)
{
    stEchoLibErrorsToStderr = pmEchoLibErrorsToStderr;
} // MIOError_EchoLibErrorsToStderr


/************************************************************************/
/* MIOError_Halt							*/
/************************************************************************/
void	MIOError_Halt (OOTstring pmMessage)
{
    if ((pmMessage == NULL) || (pmMessage [0] == 0))
    {
    	MIOError_AbortWithErrMsg (E_USER_PROGRAM_ACTIVATED_ERROR, 
    	    			  "Program execution terminated");
    }
    else
    {
    	MIOError_AbortWithErrMsg (E_USER_PROGRAM_ACTIVATED_ERROR, pmMessage);
    }
} // MIOError_Halt


/************************************************************************/
/* MIOError_Last							*/
/************************************************************************/
OOTint	MIOError_Last (void)
{
    OOTnat	myErrNo, myPrevErrNo;
    OOTaddr	myErrMessagePtr, myPrevErrMessagePtr;
    char	myMessage [1024];

    Language_Execute_GetErrno (&myErrNo, &myPrevErrNo, &myErrMessagePtr, 
	&myPrevErrMessagePtr);
    strcpy (myMessage, (char *) myPrevErrMessagePtr);
    Language_Execute_SetErrno (myPrevErrNo, myMessage);

    return myPrevErrNo;
} // MIOError_Last


/************************************************************************/
/* MIOError_LastMsg							*/
/************************************************************************/
void	MIOError_LastMsg (OOTstring pmMessage)
{
    OOTnat	myErrNo, myPrevErrNo;
    OOTaddr	myErrMessagePtr, myPrevErrMessagePtr;

    Language_Execute_GetErrno (&myErrNo, &myPrevErrNo, &myErrMessagePtr, 
	&myPrevErrMessagePtr);
    if (myPrevErrMessagePtr [0] == 0)
    {
	MIOError_Msg (pmMessage, myPrevErrNo);
	Language_Execute_SetErrno (myPrevErrNo, 0);
    }
    else
    {
	strcpy (pmMessage, (char *) myPrevErrMessagePtr);
	Language_Execute_SetErrno (myPrevErrNo, pmMessage);
    }
} // MIOError_LastMsg


/************************************************************************/
/* MIOError_LastStr							*/
/************************************************************************/
void	MIOError_LastStr (OOTstring pmString)
{
    MIOError_Str (pmString, MIOError_Last ());
} // MIOError_LastStr


/************************************************************************/
/* MIOError_Msg								*/
/************************************************************************/
void	MIOError_Msg (OOTstring pmMessage, OOTint pmErrNo)
{
    char	**myBaseMsgArray;
    int		myModule = pmErrNo / 100;
    int		myOffset = pmErrNo % 100;

    if (pmErrNo == 0)
    {
	pmMessage [0] = 0;
	return;
    }

    if ((pmErrNo <= 0) || (myModule >= stNumErrorTypes)|| 
    	(myOffset >= stNumErrors [myModule]))
    {
    	MDIO_sprintf (pmMessage, "Unknown error #%d", pmErrNo);
	return;
    }

    myBaseMsgArray = errorMsgBase [myModule];

    strcpy (pmMessage, myBaseMsgArray [myOffset]);
} // MIOError_Msg


/************************************************************************/
/* MIOError_Set								*/
/************************************************************************/
void	MIOError_Set (OOTint pmErrNo, OOTstring pmMessage)
{
    if (pmErrNo <= 0)
    {
    	pmErrNo = E_USER_PROGRAM_ACTIVATED_ERROR;
    }

    if ((pmMessage == NULL) || (pmMessage [0] == 0))
    {
    	MIOError_SetErrNo (pmErrNo);
    }
    else
    {
    	MIOError_SetErrMsg (pmErrNo, pmMessage);
    }
} // MIOError_Set


/************************************************************************/
/* MIOError_Str								*/
/************************************************************************/
void	MIOError_Str (OOTstring pmMessage, OOTint pmErrNo)
{
    char	**myBaseStrArray;
    int		myModule = pmErrNo / 100;
    int		myOffset = pmErrNo % 100;

    if ((pmErrNo <= 0) || (myModule >= stNumErrorTypes)|| 
    	(myOffset >= stNumErrors [myModule]))
    {
	*pmMessage = 0;
	return;
    }

    myBaseStrArray = errorStrBase [myModule];

    strcpy (pmMessage, myBaseStrArray [myOffset]);
} // MIOError_Str


/***************************************/
/* External procedures for MIO library */
/***************************************/
/************************************************************************/
/* MIOError_AbortWithErrNo						*/
/*									*/
/* Used internally AND as Error.Abort					*/
/************************************************************************/
void	MIOError_AbortWithErrNo (int pmErrNo)
{
    char	myErrorMessage [1024];

    MIOError_Msg (myErrorMessage, pmErrNo);
    
    MyAbortErrMsg (pmErrNo, myErrorMessage);
} // MIOError_AbortWithErrNo


/************************************************************************/
/* MIOError_AbortWithErrMsg						*/
/************************************************************************/
void	MIOError_AbortWithErrMsg (int pmErrNo, char *pmErrMsg, ...)
{
    va_list 	myArgList;
    char	myErrorMessage [1024];
    
    va_start (myArgList, pmErrMsg);
    MDIO_vsprintf (myErrorMessage, pmErrMsg, myArgList);
    va_end (myArgList);

    MyAbortErrMsg (pmErrNo, myErrorMessage);
} // MIOError_AbortWithErrMsg


/************************************************************************/
/* MIOError_SetErrNo							*/
/************************************************************************/
void	MIOError_SetErrNo (int pmErrNo)
{
    char	myErrorMessage [1024];

    MIOError_Msg (myErrorMessage, pmErrNo);
    
    Language_Execute_SetErrno (pmErrNo, myErrorMessage);

    MySetErrMsg (pmErrNo, myErrorMessage);
} // MIOError_SetErrNo


/************************************************************************/
/* MIOError_SetErrMsg							*/
/************************************************************************/
void	MIOError_SetErrMsg (int pmErrNo, char *pmErrMsg, ...)
{
    va_list 	myArgList;
    char	myErrorMessage [1024];
    
    va_start (myArgList, pmErrMsg);
    MDIO_vsprintf (myErrorMessage, pmErrMsg, myArgList);
    va_end (myArgList);

    MySetErrMsg (pmErrNo, myErrorMessage);
} // MIOError_SetErrMsg


/******************************/
/* Static callback procedures */
/******************************/


/*********************/
/* Static procedures */
/*********************/
/************************************************************************/
/* MyAbortErrMsg							*/
/************************************************************************/
static void	MyAbortErrMsg (int pmErrNo, char *pmErrMsg)
{
    SRCPOS	mySrcPos;
    char	myFilePath [4096];

    Language_Execute_SetErrno (pmErrNo, pmErrMsg);
    
    Language_Execute_RunSrcPosition (&mySrcPos);
    EdInt_NotifyDebuggerAboutError (pmErrNo, pmErrMsg, &mySrcPos);
    
    // Provide extra information
    FileManager_FileName (mySrcPos.fileNo, myFilePath);
    MIO_ErrorInfo ("Line %d of %s: Error #%d - \"%s\"",
    	mySrcPos.lineNo, myFilePath, pmErrNo, pmErrMsg);

    Language_Execute_ExecutionError_Abort (pmErrNo, pmErrMsg);
} // MyAbortErrMsg


/************************************************************************/
/* MySetErrMsg								*/
/************************************************************************/
static void	MySetErrMsg (int pmErrNo, char *pmErrMsg)
{
    SRCPOS	mySrcPos;
    char	myFilePath [4096];

    Language_Execute_SetErrno (pmErrNo, pmErrMsg);
    
    Language_Execute_RunSrcPosition (&mySrcPos);
    EdInt_NotifyDebuggerAboutError (pmErrNo, pmErrMsg, &mySrcPos);
    
    // Provide extra information
    FileManager_FileName (mySrcPos.fileNo, myFilePath);
    MIO_ErrorInfo ("Line %d of %s: Error #%d - \"%s\"",
    	mySrcPos.lineNo, myFilePath, pmErrNo, pmErrMsg);
} // MySetErrMsg

