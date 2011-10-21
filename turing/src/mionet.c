/************/
/* mionet.c */
/************/

/*******************/
/* System includes */
/*******************/
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

/****************/
/* Self include */
/****************/
#include "mionet.h"

/******************/
/* Other includes */
/******************/
#include "mdionet.h"

#include "mio.h"

#include "mioerr.h"

#include "edint.h"

#include "mdio.h"

#include "miofile.h"
#include "miotime.h"

// Test to make certain we're not accidentally including <windows.h> which
// might allow for windows contamination of platform independent code.
#ifdef _INC_WINDOWS
xxx
#endif

/**********/
/* Macros */
/**********/

#define SUCC(p)  ((p+1 == BUFFER_SIZE) ? 0 : p+1)
#define PRED(p)  ((p == 0) ? BUFFER_SIZE-1 : p-1)

/*************/
/* Constants */
/*************/
#define BUFFER_SIZE		128*1024
#define NET_STREAM_CLOSED	(NetRecord *) 1

#define NA_BYTES		0
#define NA_LINE			1
#define NA_TOKEN		2


/********************/
/* Global variables */
/********************/

/*********/
/* Types */
/*********/
typedef struct 
{
    SOCK	socket;	    // Windows = SOCKET
    SOCK	s_original;
    SOCK_ADDR	sockAddr;   // Windows = struct sockaddr_in *
    BOOL	connected;
    BOOL	atEOF;
    BOOL	seenEOF;
    BOOL	binary;
    BOOL	lastCharWasCR;
    int		status;	    // One of NET_STATUS_...
    SRCPOS	srcPos;
    BYTE	*buf;
    int		head;
    int		tail;
} NetRecord;


/**********************/
/* External variables */
/**********************/

/********************/
/* Static constants */
/********************/

/********************/
/* Static variables */
/********************/
// The following is set to TRUE if the machine dependent parts
// were successfully initialized.
static BOOL		stMDNetInitialized = FALSE;
static NetRecord    	*stStreams [40];

// Used for MIONet_RegisterOpen
static NetRecord	*stLastStreamOpened = NULL;
static char		stLastOpenDescription [1024];

/******************************/
/* Static callback procedures */
/******************************/

/*********************/
/* Static procedures */
/*********************/
static int	MyBytesAvailable (NetRecord *pmNetRecord);
static void	MyConvertToForwardSlash (char *pmURL);
static BOOL	MyDoConnect (NetRecord *pmNetRecord, char *pmOpenStr,
			     int *pmPort, char *pmNetAddr);
static BOOL	MyDoOpenForAccept (NetRecord *pmNetRecord, char *pmOpenStr,
				   int *pmPort);
static NetRecord	*MyGetNetRecord (int pmNetID);
static void	MyGetDefaultCookieURL (const char *pmOrigURL, char *pmNewURL);
static BOOL	MyGetURLAddressAndDocument (char *pmNetAddr, char *pmDoc,
					    char *pmHTTP);
static int	MyRead (unsigned char *pmBuffer, int pmBytesToRead, 
			NetRecord *pmNetRecord);
static void	MyReadAhead (NetRecord *pmNetRecord);
static void	MySendURLGet (NetRecord *pmNetRecord, char *pmDoc);


/*********************************************/
/* External procedures for Turing predefines */
/*********************************************/
/************************************************************************/
/* MIONet_Available							*/
/************************************************************************/
OOTint	MIONet_Available (OOTint pmNetID, OOTint pmType)
{
    NetRecord	*myNetRecord = MyGetNetRecord (pmNetID);
    int		myPos;

    if (!stMDNetInitialized)
    {
	ABORT_WITH_ERRNO (E_NET_INIT_FAILED);
	// Never reaches here
    }

    MyReadAhead (myNetRecord);

    myPos = myNetRecord -> tail;

    switch (pmType)
    {
	case NA_BYTES:
	    return MyBytesAvailable (myNetRecord);

	case NA_TOKEN:
	    while ((myPos != myNetRecord -> head) && 
		   isspace (myNetRecord -> buf [myPos]))
            myPos = SUCC (myPos);

	    if (myPos == myNetRecord -> head)
		return 0;

	    while (myPos != myNetRecord->head) 
	    {
		if (!isspace(myNetRecord -> buf[myPos]))
		{
		    return 1;
		}
		myPos = SUCC (myPos);
	    }
	    return 0;

	case NA_LINE:
	    while (myPos != myNetRecord -> head) 
	    {
		if (myNetRecord -> buf[myPos] == '\n')
		{
		    return 1;
		}
		myPos = SUCC(myPos);
	    }
	    return 0;

	default:
	    return 0;
    } // switch
} // MIONet_Available


/************************************************************************/
/* MIONet_ConnectionStatus						*/
/************************************************************************/
int	MIONet_ConnectionStatus (OOTint pmNetID)
{
    NetRecord	*myNetRecord = MyGetNetRecord (pmNetID);

    if (!stMDNetInitialized)
    {
	ABORT_WITH_ERRNO (E_NET_INIT_FAILED);
	// Never reaches here
    }

    return myNetRecord -> status;
} // MIONet_ConnectionStatus


/************************************************************************/
/* MIONet_DeleteCookie							*/
/************************************************************************/
void	MIONet_DeleteCookie (const OOTstring pmURL,
			     const OOTstring pmCookieName)
{
    char	myURL [512];

    MyGetDefaultCookieURL (pmURL, myURL);

    MDIONet_SetCookie (myURL, pmCookieName, "", "Sat, 01-Jan-2000 00:00:00 GMT");
} // MIONet_DeleteCookie


/************************************************************************/
/* MIONet_GetAddress							*/
/************************************************************************/
void	MIONet_GetAddress (OOTint pmNetID, OOTstring pmNetAddress)
{
    NetRecord	*myNetRecord = MyGetNetRecord (pmNetID);

    if (!stMDNetInitialized)
    {
	ABORT_WITH_ERRNO (E_NET_INIT_FAILED);
	// Never reaches here
    }

    if (myNetRecord == NULL)
    {
	pmNetAddress [0] = 0;
    }
    else
    {
	MDIONet_GetNetAddress (myNetRecord -> sockAddr, pmNetAddress);
    }
} // MIONet_GetAddress


/************************************************************************/
/* MIONet_GetCookie							*/
/************************************************************************/
void	MIONet_GetCookie (OOTstring pmCookieValue, const OOTstring pmURL,
			  const OOTstring pmCookieName)
{
    char    myURL [512];

    MyGetDefaultCookieURL (pmURL, myURL);

    MDIONet_GetCookie (pmCookieValue, myURL, pmCookieName);
} // MIONet_GetCookie


/************************************************************************/
/* MIONet_GetCookies							*/
/************************************************************************/
void	MIONet_GetCookies (const OOTstring pmURL,
			   OOTstring pmCookieName [],
			   OOTint pmCookieNameArraySize,
			   OOTstring pmCookieValue [],
			   OOTint pmCookieValueArraySize)
{
    int	myNumCookies = MDIONet_GetNumCookies (pmURL);

    if (myNumCookies > pmCookieNameArraySize)
    {
	SET_ERRMSG (E_NET_ARRAY_TOO_SMALL, "Cookie name array of %d is too small "
	    "(%d cookies to be read)", pmCookieNameArraySize, myNumCookies);
	return;
    }
    if (myNumCookies > pmCookieValueArraySize)
    {
	SET_ERRMSG (E_NET_ARRAY_TOO_SMALL, "Cookie value array of %d is too small "
	    "(%d cookies to be read)", pmCookieNameArraySize, myNumCookies);
	return;
    }

    MIONet_GetCookies (pmURL, pmCookieName, pmCookieNameArraySize,
		       pmCookieValue, pmCookieValueArraySize);
} // MIONet_GetCookies

/************************************************************************/
/* MIONet_GetNumCookies							*/
/************************************************************************/
int	MIONet_GetNumCookies (const OOTstring pmURL)
{
    return MDIONet_GetNumCookies (pmURL);
} // MIONet_GetNumCookies


/************************************************************************/
/* MIONet_GetHostAddressByName						*/
/************************************************************************/
void	MIONet_GetHostAddressByName (OOTstring pmNetAddress,
				     const OOTstring pmNetName)
{
    if (!stMDNetInitialized)
    {
	ABORT_WITH_ERRNO (E_NET_INIT_FAILED);
	// Never reaches here
    }

    MDIONet_GetHostAddressByName (pmNetAddress, pmNetName);
} // MIONet_GetHostAddressByName


/************************************************************************/
/* MIONet_GetHostNameByAddress						*/
/************************************************************************/
void	MIONet_GetHostNameByAddress (OOTstring pmNetName,
				     const OOTstring pmNetAddress)
{
    if (!stMDNetInitialized)
    {
	ABORT_WITH_ERRNO (E_NET_INIT_FAILED);
	// Never reaches here
    }

    MDIONet_GetHostNameByAddress (pmNetName, pmNetAddress);
} // MIONet_GetHostNameByAddress


/************************************************************************/
/* MIONet_GetLocalAddress						*/
/************************************************************************/
void	MIONet_GetLocalAddress (OOTstring pmNetAddress)
{
    char    myNetName [256];

    if (!stMDNetInitialized)
    {
	ABORT_WITH_ERRNO (E_NET_INIT_FAILED);
	// Never reaches here
    }

    MIONet_GetLocalName (myNetName);
    MIONet_GetHostAddressByName (pmNetAddress, myNetName);
} // MIONet_GetLocalAddress


/************************************************************************/
/* MIONet_GetLocalName							*/
/************************************************************************/
void	MIONet_GetLocalName (OOTstring pmNetName)
{
    if (!stMDNetInitialized)
    {
	ABORT_WITH_ERRNO (E_NET_INIT_FAILED);
	// Never reaches here
    }

    pmNetName [0] = 0;

    MDIONet_GetHostName (pmNetName, 255);
} // MIONet_GetLocalName


/************************************************************************/
/* MIONet_RegisterClose							*/
/************************************************************************/
void	MIONet_RegisterClose (OOTint pmNetID)
{
    if (!stMDNetInitialized)
    {
	ABORT_WITH_ERRNO (E_NET_INIT_FAILED);
	// Never reaches here
    }

    // Tell the debugger it is no longer in use.
    // TW: Actually the debugger already deallocated the stream when
    // it closed the stream in mio.c
//    EdInt_NotifyDebuggerObjectDeallocated (pmNetID);

    stStreams [pmNetID] = NET_STREAM_CLOSED;
} // MIONet_RegisterClose


/************************************************************************/
/* MIONet_RegisterOpen							*/
/*									*/
/* We take advantage of the fact that we know that a MIONet_Open	*/
/* is always immediately followed by a MIONet_RegisterOpen		*/
/************************************************************************/
void	MIONet_RegisterOpen (OOTint pmNetID, SRCPOS *pmSrcPos)
{
    if (!stMDNetInitialized)
    {
	ABORT_WITH_ERRNO (E_NET_INIT_FAILED);
	// Never reaches here
    }

    EdInt_NotifyDebuggerObjectAllocated (NET_STREAM, pmNetID, pmSrcPos,
					 stLastOpenDescription);

    stStreams [pmNetID] = stLastStreamOpened;
    stLastStreamOpened = NULL;
} // MIONet_RegisterOpen


/************************************************************************/
/* MIONet_SetCookie							*/
/************************************************************************/
void	MIONet_SetCookie (const OOTstring pmURL, const OOTstring pmCookieName,
			  const OOTstring pmCookieValue, OOTint pmExpiryDate)
{
    char	myURL [512], myExpiryDate [256];

    MyGetDefaultCookieURL (pmURL, myURL);

    if (pmExpiryDate == 0)
    {
	pmExpiryDate = MIOTime_Sec () + (365 * 24 * 60 * 60);
    }

    MIOTime_SecStr (myExpiryDate, pmExpiryDate, "%a, %d-%b-%Y %H:%M:%S GMT");

    MDIONet_SetCookie (myURL, pmCookieName, pmCookieValue, myExpiryDate);
} // MIONet_SetCookie


/************************************************************************/
/* MIONet_WaitForConnect						*/
/************************************************************************/
BOOL	MIONet_WaitForConnect (OOTint pmNetID, EventDescriptor *pmEvent)
{
    NetRecord	*myNetRecord = MyGetNetRecord (pmNetID);

    if (!stMDNetInitialized)
    {
	ABORT_WITH_ERRNO (E_NET_INIT_FAILED);
	// Never reaches here
    }

    if (myNetRecord == NULL)
    {
	return FALSE;
    }

    pmEvent -> mode = EventMode_NetAccept;
    return TRUE;
} // MIONet_WaitForConnect


/***************************************/
/* External procedures for MIO library */
/***************************************/
/************************************************************************/
/* MIONet_Init								*/
/************************************************************************/
void	MIONet_Init (void)
{
    stMDNetInitialized = MDIONet_Init ();
} // MIONet_Init


/************************************************************************/
/* MIONet_Finalize							*/
/************************************************************************/
void	MIONet_Finalize (void)
{
    if (stMDNetInitialized)
    {
	MDIONet_Finalize ();
    }
} // MIONet_Finalize


/************************************************************************/
/* MIONet_Close								*/
/************************************************************************/
BOOL	MIONet_Close (void *pmFilePtr)
{
    NetRecord	*myNetRecord = (NetRecord *) pmFilePtr;

    MDIONet_ShutDownSocket (myNetRecord -> socket);
    MDIONet_CloseSocket (myNetRecord -> socket);
    if (myNetRecord -> s_original != NULL)
    {
	MDIONet_CloseSocket (myNetRecord -> s_original);
    }

    if (myNetRecord -> sockAddr != NULL)
    {
	free (myNetRecord -> sockAddr);
    }
    free (myNetRecord -> buf);
    free (myNetRecord);

    return 0;
} // MIONet_Close


/************************************************************************/
/* MIONet_Getc								*/
/*									*/
/* This routine converts LF's, CR's and CRLF's into NL's.		*/
/************************************************************************/
int	MIONet_Getc (void *pmFilePtr)
{
    NetRecord		*myNetRecord = (NetRecord *) pmFilePtr;
    unsigned char	myChar;

    // It is possible that MIONet_Getc gets called without the appropriate
    // HasEvent having been called and passed because 'eof' doesn't call
    // HasEvent first.  So, we risk a busy wait...
    if (MyBytesAvailable (myNetRecord) == 0)
    {
	// Do Nothing - the MyBytesAvailable reads the buffer
	ABORT_WITH_ERRNO (E_NET_DONT_USE_EOF);
    }

    if (MyRead (&myChar, 1, myNetRecord) != 1) 
    {
	return EOF;
    }

    if (myNetRecord -> binary)
    {
	return myChar;
    }

    // If the last character we read was a CR and this is a LF, skip over
    // it.  This should be alright, as the HasEvent also takes the 
    // newline conversion into account
    if (myNetRecord -> lastCharWasCR && (myChar == '\n'))
    {
	if (MyRead (&myChar, 1, myNetRecord) != 1) 
	{
	    return EOF;
	}
    }

    // If the char just read is not a CR, return the char
    if (myChar != '\r')
    {
	myNetRecord -> lastCharWasCR = FALSE;
	return myChar;
    }
    
    myNetRecord -> lastCharWasCR = TRUE;

    // We've read a CR.  See if the next character is a NL and 
    // clear it off the buffer if it is.
    if ((!myNetRecord -> seenEOF) && 
	(myNetRecord -> tail != myNetRecord -> head) &&
	(* (char *) (myNetRecord -> buf + myNetRecord -> tail) == '\n'))
    {
	// The next char *is* a LF.  So read the LF.
	MyRead (&myChar, 1, myNetRecord);
    }
    
    return '\n';
} // MIONet_Getc


/************************************************************************/
/* MIONet_HasEvent							*/
/************************************************************************/
BOOL	MIONet_HasEvent (void *pmFilePtr, EventDescriptor *pmEvent)
{
    NetRecord	*myNetRecord = (NetRecord *) pmFilePtr;
    int		myPos, myAvail;

    if (pmEvent -> mode == EventMode_NetAccept)
    {
	return MDIONet_HasConnected (&myNetRecord -> socket, 
				     myNetRecord -> s_original, 
				     myNetRecord -> sockAddr);
    }

//    if (!nr->connected)		connected is never set to true!
//	return FALSE;

    MyReadAhead (myNetRecord);

    myPos = myNetRecord -> tail;

    switch (pmEvent -> mode)
    {
	case EventMode_InputToken:
	    while ((myPos != myNetRecord -> head) && 
		   isspace (myNetRecord -> buf [myPos]))
	    {
		myPos = SUCC (myPos);
	    }
	    if (myPos == myNetRecord -> head)
	    {
		return FALSE;
	    }
	    while (myPos != myNetRecord -> head) 
	    {
		if (!isspace (myNetRecord -> buf [myPos]))
		{
		    return TRUE;
		}
		myPos = SUCC (myPos);
	    }
	    return FALSE;

	case EventMode_InputLine:
	    while (myPos != myNetRecord -> head) 
	    {
		if (myNetRecord -> buf [myPos] == '\n')
		{
		    return TRUE;
		}
		myPos = SUCC (myPos);
	    }
	    return FALSE;

	case EventMode_InputCount:
	    myAvail = MyBytesAvailable (myNetRecord);
	    return ((myNetRecord -> atEOF) || (myAvail >= pmEvent -> count));
    } // switch

    return FALSE; // Stops bitching
} // MIONet_HasEvent


/************************************************************************/
/* MIONet_Open								*/
/************************************************************************/
void	*MIONet_Open (char *pmOpenString)
{
    NetRecord	*myNetRecord;
    char	myNetAddr[256];
    char	myDoc [256];
    int		myPort;

    if (!stMDNetInitialized)
    {
	ABORT_WITH_ERRNO (E_NET_INIT_FAILED);
	// Never reaches here
    }

    // Allocate the NetRecord
    myNetRecord = (NetRecord *) malloc (sizeof (NetRecord));
    if (myNetRecord == NULL) 
    {
	SET_ERRNO (E_INSUFFICIENT_MEMORY);
	return NULL;
    }

    // Initialize the entire record to zeroes
    memset (myNetRecord, 0, sizeof (NetRecord));

    // Allocate the socket
    myNetRecord -> socket = MDIONet_CreateSocket ();
    if (myNetRecord -> socket == NULL) 
    {
	free (myNetRecord);
	return NULL;
    }

    // Allocate the socket address
    myNetRecord -> sockAddr = MDIO_AllocateSockAddr ();
    if (myNetRecord -> sockAddr == NULL)
    {
	free (myNetRecord);
	return NULL;
    }

    // Allocate the buffer
    myNetRecord -> buf = (BYTE *) malloc(BUFFER_SIZE);
    if (myNetRecord -> buf == NULL) 
    {
	MDIONet_CloseSocket (myNetRecord -> socket);
	free (myNetRecord);
	SET_ERRNO (E_INSUFFICIENT_MEMORY);
	return NULL;
    }
    memset (myNetRecord -> buf, 128, BUFFER_SIZE);

    switch (pmOpenString[0])
    {
	case 'C':
	    // Connect to arbitrary port
	    if (MyDoConnect (myNetRecord, pmOpenString + 2, &myPort, 
			     myNetAddr)) 
	    {
		MDIO_sprintf (stLastOpenDescription,
		    "Connect to port %d on %s", &myPort, myNetAddr);
		stLastStreamOpened = myNetRecord;
		return myNetRecord;
	    }
	    break;
	
	case 'A':
	    // Open for accepting a connection on a port
	    if (MyDoOpenForAccept (myNetRecord, pmOpenString + 2, &myPort)) 
	    {
		MDIO_sprintf (stLastOpenDescription,
		    "Accepted connection on port %d", &myPort);
		stLastStreamOpened = myNetRecord;
		return myNetRecord;
	    }
	    break;

	case 'U':
	    // Open a URL for read
	    {
		if (MyGetURLAddressAndDocument (myNetAddr, myDoc, 
					        pmOpenString + 2)) 
		{
		    if (MyDoConnect (myNetRecord, myNetAddr, &myPort, NULL)) 
		    {
			MySendURLGet (myNetRecord, myDoc);
			if (myPort == 80)
			{
			    MDIO_sprintf (stLastOpenDescription,
				"Connect to URL %s", myNetAddr);
			}
			else
			{
			    MDIO_sprintf (stLastOpenDescription,
				"Connect to URL %s (port %d)", myNetAddr, 
				myPort);
			}
			stLastStreamOpened = myNetRecord;
			return myNetRecord;
		    }
		}
	    }
	    break;
    }

    MDIONet_CloseSocket (myNetRecord -> socket);
    if (myNetRecord -> sockAddr != NULL)
    {
	free (myNetRecord -> sockAddr);
    }
    free (myNetRecord -> buf);
    free (myNetRecord);
    stLastStreamOpened = NULL;
    return NULL;
} // MIONet_Open


/************************************************************************/
/* MIONet_Putc								*/
/************************************************************************/
int	MIONet_Putc (char pmChar, void *pmFilePtr)
{
    NetRecord	*myNetRecord = (NetRecord *) pmFilePtr;
    
    MDIONet_SendBytes (myNetRecord -> socket, &pmChar, 1,
		       &myNetRecord -> status);

    return 0;
} // MIONet_Putc


/************************************************************************/
/* MIONet_Puts								*/
/************************************************************************/
int	MIONet_Puts (const char *pmString, void *pmFilePtr)
{
    NetRecord	*myNetRecord = (NetRecord *) pmFilePtr;

    MDIONet_Send (myNetRecord -> socket, pmString, &myNetRecord -> status);

    return 0;
} // MIONet_Puts


/************************************************************************/
/* MIONet_Read								*/
/************************************************************************/
int	MIONet_Read (void *pmBuffer, int pmSize, void *pmFilePtr)
{
    return MyRead (pmBuffer, pmSize, (NetRecord *) pmFilePtr);
} // MIONet_Read


/************************************************************************/
/* MIONet_Ungetc							*/
/************************************************************************/
void	MIONet_Ungetc (OOTint pmChar, void *pmFilePtr)
{
    NetRecord	*myNetRecord = (NetRecord *) pmFilePtr;

    myNetRecord -> tail = PRED (myNetRecord -> tail);
    myNetRecord -> buf [myNetRecord -> tail] = (char) pmChar;
    myNetRecord -> seenEOF = FALSE;
} // MIONet_Ungetc


/************************************************************************/
/* MIONet_Write								*/
/************************************************************************/
int	MIONet_Write (void *pmBuffer, int pmSize, void *pmFilePtr)
{
    NetRecord	*myNetRecord = (NetRecord *) pmFilePtr;

    MDIONet_SendBytes (myNetRecord -> socket, pmBuffer, pmSize,
		       &myNetRecord -> status);

    return pmSize;
} // MIONet_Write


/******************************/
/* Static callback procedures */
/******************************/


/*********************/
/* Static procedures */
/*********************/
/************************************************************************/
/* MyBytesAvailable							*/
/************************************************************************/
static int	MyBytesAvailable (NetRecord *pmNetRecord)
{
    int	myTempTail, myLastCharWasCR, myBytesAvailable;

    // If we're at the EOF, then return 1 so we'll read it.
    if (pmNetRecord -> seenEOF || pmNetRecord -> atEOF)
    {
	return 1;
    }

    // If we're in binary mode, simply return the number of chars in the
    // buffer.
    if (pmNetRecord -> binary)
    {
	if (pmNetRecord -> tail <= pmNetRecord -> head)
	{
	    return pmNetRecord -> head - pmNetRecord -> tail;
	}
	else
	{
	    return BUFFER_SIZE - pmNetRecord -> tail + pmNetRecord -> head;
	}
    }

    // If we're in text mode, we need to convert CR/LF to LF for purposes
    // of number of characters available.  As well, if the last character
    // read was a CR (as evidenced by lastCharWasCR being true) and the
    // first character is a LF, then we skip that character.
    myTempTail = pmNetRecord -> tail;
    myLastCharWasCR = pmNetRecord -> lastCharWasCR;
    myBytesAvailable = 0;

    while (myTempTail != pmNetRecord -> head)
    {
	if (!myLastCharWasCR || (pmNetRecord -> buf [myTempTail] != '\n'))
	{
	    myBytesAvailable++;
	}
	if (pmNetRecord -> buf [myTempTail] == '\r')
	{
	    myLastCharWasCR = TRUE;
	}
	else
	{
	    myLastCharWasCR = FALSE;
	}

	myTempTail = (myTempTail + 1) % BUFFER_SIZE;
    }

    return myBytesAvailable;
} // MyBytesAvailable


/************************************************************************/
/* MyConvertToForwardSlash						*/
/************************************************************************/
static void	MyConvertToForwardSlash (char *pmURL)
{
    char    *myPtr = pmURL;

    while (*myPtr != 0)
    {
	if (*myPtr == '\\')
	{
	    *myPtr = '/';
	}
	myPtr++;
    }
} // MyConvertToForwardSlash


/************************************************************************/
/* MyDoConnect								*/
/************************************************************************/
static BOOL	MyDoConnect (NetRecord *pmNetRecord, char *pmOpenStr,
			     int *pmPort, char *pmNetAddr)
{
    char	*myPtr = strchr (pmOpenStr, ':');
    int		myPort;
    NET_ADDR	myAddr;

    if (myPtr == NULL) 
    {
	// Missing address
	SET_ERRNO (E_NET_MISSING_ADDRESS);
	return FALSE;
    }

    *myPtr = 0;
    myPtr++;

    myPort = atol (pmOpenStr);

    if (myPort == 0) 
    {
	SET_ERRNO (E_NET_INVALID_PORT);
	return FALSE;
    }

    // Check for the ":b" at the end signalling the stream is binary
    // (i.e. no line end conversions to be done.)
    if (strchr (myPtr, ':') != NULL)
    {
	char	*myBinaryPtr;

	myBinaryPtr = strchr (myPtr, ':');
	*myBinaryPtr++ = 0;
	if (strcmp (myBinaryPtr, "b") == 0)
	{
	    pmNetRecord -> binary = TRUE;
	}
	else
	{
	    SET_ERRMSG (E_NET_BAD_ADDRESS, "No colons allowed in network address");
	    return FALSE;
	}
    }

    myAddr = MDIONet_GetNetworkAddrByString (myPtr);
    if (myAddr == NULL)
    {
	return FALSE;
    }

    // Set debugging information
    *pmPort = myPort;
    if (pmNetAddr != NULL)
    {
	strcpy (pmNetAddr, myPtr);
    }

    return MDIONet_Connect (pmNetRecord -> socket, pmNetRecord -> sockAddr, 
			    myPort, myAddr);
} // MyDoConnect


/************************************************************************/
/* MyDoOpenForAccept							*/
/************************************************************************/
static BOOL	MyDoOpenForAccept (NetRecord *pmNetRecord, char *pmOpenStr,
				   int *pmPort)
{
    int		myPort;

    myPort = atol (pmOpenStr);

    if (myPort == 0) 
    {
	SET_ERRNO (E_NET_INVALID_PORT);
	return FALSE;
    }

    if (!MDIONet_Bind (pmNetRecord -> socket, myPort))
    {
	return FALSE;
    }

    pmNetRecord -> s_original = pmNetRecord -> socket;
    pmNetRecord -> socket = NULL;

    if (!MDIONet_Listen (pmNetRecord -> s_original,
		         &pmNetRecord -> status))
    {
	return FALSE;
    }

    if (!MDIONet_SetNonBlocking (pmNetRecord -> s_original))
    {
	return FALSE;
    }

    *pmPort = myPort;

    return TRUE;
} // MyDoOpenForAccept



/************************************************************************/
/* MyGetDefaultCookieURL						*/
/************************************************************************/
static void MyGetDefaultCookieURL (const char *pmOrigURL, char *pmNewURL)
{ 
    char    *myPtr;

    if (pmOrigURL [0] == 0)
    {
	strcpy (pmNewURL, "file:///");
	myPtr = &pmNewURL[8];
	MIOFile_GetExecutionDirectory (myPtr);
	
	if (pmNewURL [strlen (pmNewURL) - 1] != '\\')
	{
	    strcat (pmNewURL, "\\");
	}
	while (strchr (myPtr, '/') != NULL)
	{
	    *strchr (myPtr, '/') = '\\';
	}
    }
    else
    {
	strcpy (pmNewURL, pmOrigURL);
    }
} // MyGetDefaultCookieURL


/************************************************************************/
/* MyGetNetRecord							*/
/************************************************************************/
static NetRecord	*MyGetNetRecord (int pmNetID)
{
    char    myMessage [1024];

    if ((pmNetID < 1) || (pmNetID > 20))
    {
	MDIO_sprintf (myMessage, "%d is not a legal net stream id", pmNetID);
	ABORT_WITH_ERRMSG (E_NET_NOT_AN_ID, myMessage);
    }
    if (stStreams [pmNetID] == NULL)
    {
	MDIO_sprintf (myMessage, "Net stream d is not been opened", pmNetID);
	ABORT_WITH_ERRMSG (E_NET_NEVER_OPENED, myMessage);
    }
    if (stStreams [pmNetID] == NET_STREAM_CLOSED)
    {
	MDIO_sprintf (myMessage, "Net stream id of %d is closed", pmNetID);
	ABORT_WITH_ERRMSG (E_NET_CLOSED, myMessage);
    }
    return stStreams [pmNetID];
} // MyGetNetRecord


/************************************************************************/
/* MyDoOpenForAccept							*/
/************************************************************************/
static BOOL	MyGetURLAddressAndDocument (char *pmNetAddr, char *pmDoc,
					    char *pmHTTP)
{
    int		myLen = strlen (pmHTTP);
    int		myStart = 0;
    char	*myPtr;


    MyConvertToForwardSlash (pmHTTP);

    if ((myLen > 7) && (_strnicmp (pmHTTP, "http://", 7) == 0))
    {
	myStart = 7;
    }

    myPtr = strchr (pmHTTP + myStart, '/');

    if (myPtr == NULL) 
    {
	// If there's no slash in the URL, then doc is set to "/index.html"
	strcpy (pmDoc, "/index.html");
    }
    else if (myPtr [1] == 0) 
    {
	// If the URL ends in a '/', then doc is set to "/index.html"
	strcpy (pmDoc, "/index.html");
	*myPtr = 0;
    }
    else 
    {
	// doc is set to the document name in the URL
        strcpy (pmDoc, myPtr);
        *myPtr = 0;
    }

    // Find the port number in the URL, if there is one
    myPtr = strchr (pmHTTP + myStart, ':');

    if (myPtr) 
    {
	*myPtr = 0;
	MDIO_sprintf("%s:%s", myPtr + 1, pmHTTP + myStart);
    }
    else 
    {
	MDIO_sprintf (pmNetAddr, "80:%s", pmHTTP + myStart);
    }

    return TRUE;
} // MyGetURLAddressAndDocument


/************************************************************************/
/* MyRead								*/
/************************************************************************/
static int	MyRead (unsigned char *pmBuffer, int pmBytesToRead, 
			NetRecord *pmNetRecord)
{
    int myBytesTillWrap = BUFFER_SIZE - pmNetRecord -> tail;
    int	myAvail = MyBytesAvailable (pmNetRecord);
    int myRemainingBytes;

    if (pmNetRecord -> seenEOF)
    {
	ABORT_WITH_ERRNO (E_NET_READ_PAST_EOF);
	return 0;
    }

    // A hasevent call should have been made to ensure bytes were
    // available. Verify.
    if (myAvail < pmBytesToRead) 
    {
	if ((myAvail == 0) && (pmNetRecord -> atEOF))
	{
	    pmNetRecord -> seenEOF = TRUE;
	    return 0;
	}
	ABORT_WITH_ERRNO (E_NET_UNKNOWN_ERROR);
	return 0;
    }

    //
    // Read data from the buffer
    //
    if (pmBytesToRead <= myBytesTillWrap) 
    {
        memcpy (pmBuffer, pmNetRecord -> buf + pmNetRecord -> tail, 
		pmBytesToRead);
        pmNetRecord -> tail += pmBytesToRead;

        if (pmNetRecord -> tail == BUFFER_SIZE)
	{
            pmNetRecord -> tail = 0;
	}
    }
    else 
    {
        myRemainingBytes = pmBytesToRead - myBytesTillWrap;

        memcpy (pmBuffer, pmNetRecord -> buf + pmNetRecord -> tail, 
		myBytesTillWrap);
        memcpy (pmBuffer + myBytesTillWrap, pmNetRecord -> buf, 
		myRemainingBytes);
        pmNetRecord -> tail = myRemainingBytes;
    }

    return pmBytesToRead;
} // MyRead


/************************************************************************/
/* MyReadAhead								*/
/************************************************************************/
static void	MyReadAhead (NetRecord *pmNetRecord)
{
    unsigned long	myNum;
    int			myBufFree;
    int			myBytesRead;


    myBufFree = (pmNetRecord -> tail <= pmNetRecord -> head) ? 
	    (BUFFER_SIZE - pmNetRecord -> head + pmNetRecord -> tail - 1) :
	    (pmNetRecord -> tail - pmNetRecord -> head - 1);
    if (myBufFree == 0)
    {
	return;		// No space
    }

    // See if there are any chars waiting to be read
    if (!MDIONet_CharsWaiting (pmNetRecord -> socket, &myNum,
			       &pmNetRecord -> status))
    {
	return;
    }

    if (myNum > 0) 
    {
        // Characters are available, read them!
        int 	myBytesToRead = ((int) myNum <= myBufFree) ? myNum : myBufFree;
        int	myBytesTillWrap = BUFFER_SIZE - pmNetRecord -> head;
	
        if (myBytesToRead <= myBytesTillWrap)
	{
	    if (!MDIONet_Receive (pmNetRecord -> socket, 
				  pmNetRecord -> buf + pmNetRecord -> head,
				  myBytesToRead, &myBytesRead, NULL,
				  &pmNetRecord -> status))
	    {
		return;
	    }
	    
            pmNetRecord -> head += myBytesRead;
        }
        else 
	{
            int 	myRemainingBytes = myBytesToRead - myBytesTillWrap;
	    
	    if (!MDIONet_Receive (pmNetRecord -> socket, 
				  pmNetRecord -> buf + pmNetRecord -> head,
				  myBytesTillWrap, &myBytesRead, NULL,
				  &pmNetRecord -> status))
	    {
		return;
	    }
	    
	    if (myBytesRead == myBytesTillWrap) 
	    {
		// Still more bytes remaining
		pmNetRecord -> head = 0;
		if (!MDIONet_Receive (pmNetRecord -> socket, 
				      pmNetRecord -> buf + pmNetRecord -> head,
				      myRemainingBytes, &myBytesRead, NULL,
				      &pmNetRecord -> status))
		{
		    return;
		}
		pmNetRecord -> head = myBytesRead;
	    }
	    else 
	    {
		pmNetRecord -> head += myBytesRead;
	    }
        }
	
        if (pmNetRecord -> head == BUFFER_SIZE)
	{
            pmNetRecord -> head = 0;
	}
    }
    else 
    {
	if (MDIONet_CheckStatus (pmNetRecord -> socket,
				 &pmNetRecord -> status))
	{
	    //
	    // At this point, a positive result from MDIONet_CheckStatus means
	    // one of three things:
	    //    (1) There is data to be read
	    //    (2) The connection was closed and when we read we'll get 
	    //        zero bytes
	    //    (3) The connection was forcibly closed and when we read
	    //        we'll get a WSAECONNRESET error.
	    //
	    char    myChar;
	    BOOL    myWouldBlock;

	    if (!MDIONet_Receive (pmNetRecord -> socket, &myChar, 1, 
			          &myBytesRead, &myWouldBlock,
				  &pmNetRecord -> status))
	    {
		if (!myWouldBlock)
		{
		    pmNetRecord -> atEOF = TRUE;
		}
		return;
	    }

	    if (myBytesRead == 0) 
	    {
		pmNetRecord -> atEOF = TRUE;
	    }
	    else if (myBytesRead == 1) 
	    {
		// OOPS a byte must have arrived, process it

		pmNetRecord -> buf [pmNetRecord -> head] = myChar;
		pmNetRecord -> head++;

		if (pmNetRecord -> head == BUFFER_SIZE)
		{
		    pmNetRecord -> head = 0;
		}
	    }
	}
    }
} // MyReadAhead


/************************************************************************/
/* MySendURLGet								*/
/************************************************************************/
static void	MySendURLGet (NetRecord *pmNetRecord, char *pmDoc)
{
    char    myMessage [512];

    MDIO_sprintf (myMessage, "GET %s\r\n", pmDoc);

    MDIONet_Send (pmNetRecord -> socket, myMessage, &pmNetRecord -> status);
} // MySendURLGet
