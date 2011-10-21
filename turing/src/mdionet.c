/*************/
/* mdionet.c */
/*************/

/*******************/
/* System includes */
/*******************/
#include <winsock2.h>
#include <wininet.h>

/****************/
/* Self include */
/****************/
#include "mdionet.h"

/******************/
/* Other includes */
/******************/
#include "mionet.h"

#include "mio.h"

#include "mioerr.h"

#include "edint.h"

#include "mdio.h"

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/
#ifndef SD_SEND
#define SD_SEND		0x01
#endif

/********************/
/* Global variables */
/********************/

/*********/
/* Types */
/*********/
typedef int (_stdcall *WSAStartupProc) (WORD wVersionRequested,
					LPWSADATA lpWSAData);
typedef int (_stdcall *WSACleanupProc) (void);
typedef int (_stdcall *WSAGetLastErrorProc) (void);
typedef SOCKET (_stdcall *acceptProc) (SOCKET s, struct sockaddr FAR *addr,
				       int FAR *addrlen);
typedef int (_stdcall *bindProc) (SOCKET s,                          
				  const struct sockaddr FAR *name,   
				  int namelen);
typedef int (_stdcall *closesocketProc) (SOCKET s);
typedef int (_stdcall *connectProc) (SOCKET s, const struct sockaddr FAR *name,
				     int namelen);
typedef struct hostent FAR * (_stdcall *gethostbyaddrProc) (
						    const char FAR *name,
						    int len, int type);
typedef struct hostent FAR * (_stdcall *gethostbynameProc) (
						    const char FAR *name);
typedef int (_stdcall *gethostnameProc) (const char FAR *name, int namelen);
typedef u_short (_stdcall *htonsProc) (u_short hostshort);
typedef unsigned long (_stdcall *inet_addrProc) (const char FAR *cp);
typedef char FAR * (_stdcall *inet_ntoaProc) (struct in_addr in);
typedef int (_stdcall *ioctlsocketProc) (SOCKET s, long cmd, u_long FAR *argp);
typedef int (_stdcall *listenProc) (SOCKET s, int backlog);
typedef int (_stdcall *recvProc) (SOCKET s, char FAR *buf, int len, int flags);
typedef int (_stdcall *selectProc) (int nfds, fd_set FAR *readfds,               
				      fd_set FAR *writefds,
				      fd_set FAR *exceptfds,             
				      const struct timeval FAR *timeout);
typedef int (_stdcall *sendProc) (SOCKET s, const char FAR *buf, int len, 
				  int flags);
typedef int (_stdcall *shutdownProc) (SOCKET s, int how);
typedef SOCKET (_stdcall *socketProc) (int af, int type, int protocol);

/**********************/
/* External variables */
/**********************/

/********************/
/* Static constants */
/********************/

/********************/
/* Static variables */
/********************/
static WSADATA		stWSAData;

// Routines from ws2_32.dll
static WSACleanupProc	stWSACleanupProc;
static WSAGetLastErrorProc	stWSAGetLastErrorProc;
static acceptProc	stAcceptProc;
static bindProc		stBindProc;
static closesocketProc	stCloseSocketProc;
static connectProc	stConnectProc;
static gethostbyaddrProc	stGetHostByAddrProc;
static gethostbynameProc	stGetHostByNameProc;
static gethostnameProc	stGetHostNameProc;
static htonsProc	stHtoNSProc;
static inet_addrProc	stInetAddrProc;
static inet_ntoaProc	stInetNtoAProc;
static ioctlsocketProc	stIoctlSocketProc;
static listenProc	stListenProc;
static recvProc		stRecvProc;
static selectProc	stSelectProc;
static sendProc		stSendProc;
static shutdownProc	stShutDownProc;
static socketProc	stSocketProc;

/******************************/
/* Static callback procedures */
/******************************/

/*********************/
/* Static procedures */
/*********************/
static char	*MyGetCookies (const char *pmURL);
static void	MyProcessWin32Error (int pmWin32Error);
static void	MyProcessWSAError (int *pmStatus);

/***********************/
/* External procedures */
/***********************/
/************************************************************************/
/* MDIONet_Init								*/
/************************************************************************/
BOOL	MDIONet_Init (void)
{
    HMODULE		myWinSockDLL;
    WSAStartupProc	myWSAStartupProc;

    // To avoid auto loading the ws2_32.dll, we make all the calls
    // by loading up the function's address
    myWinSockDLL = LoadLibrary ("ws2_32.dll");
    if (myWinSockDLL == NULL)
    {
	// The DLL doesn't exist on this platform.
	return FALSE;
    }

    myWSAStartupProc = (WSAStartupProc) GetProcAddress (myWinSockDLL, 
							"WSAStartup");
    if (myWSAStartupProc == NULL)
    {
	// The routine was not found in the DLL.
	return FALSE;
    }
    stWSACleanupProc = (WSACleanupProc) GetProcAddress (myWinSockDLL,
							"WSACleanup");
    if (stWSACleanupProc == NULL)
    {
	// The routine was not found in the DLL.
	return FALSE;
    }

    stWSAGetLastErrorProc = (WSAGetLastErrorProc) GetProcAddress (myWinSockDLL,
							"WSAGetLastError");
    if (stWSAGetLastErrorProc == NULL)
    {
	// The routine was not found in the DLL.
	return FALSE;
    }
    stAcceptProc = (acceptProc) GetProcAddress (myWinSockDLL, "accept");
    if (stAcceptProc == NULL)
    {
	// The routine was not found in the DLL.
	return FALSE;
    }
    stBindProc = (bindProc) GetProcAddress (myWinSockDLL, "bind");
    if (stBindProc == NULL)
    {
	// The routine was not found in the DLL.
	return FALSE;
    }
    stCloseSocketProc = (closesocketProc) GetProcAddress (myWinSockDLL,
							  "closesocket");
    if (stCloseSocketProc == NULL)
    {
	// The routine was not found in the DLL.
	return FALSE;
    }
    stConnectProc = (connectProc) GetProcAddress (myWinSockDLL, "connect");
    if (stConnectProc == NULL)
    {
	// The routine was not found in the DLL.
	return FALSE;
    }
    stGetHostByAddrProc = (gethostbyaddrProc) GetProcAddress (myWinSockDLL, 
							      "gethostbyaddr");
    if (stGetHostByAddrProc == NULL)
    {
	// The routine was not found in the DLL.
	return FALSE;
    }
    stGetHostByNameProc = (gethostbynameProc) GetProcAddress (myWinSockDLL, 
							      "gethostbyname");
    if (stGetHostByNameProc == NULL)
    {
	// The routine was not found in the DLL.
	return FALSE;
    }
    stGetHostNameProc = (gethostnameProc) GetProcAddress (myWinSockDLL, 
							  "gethostname");
    if (stGetHostNameProc == NULL)
    {
	// The routine was not found in the DLL.
	return FALSE;
    }
    stHtoNSProc = (htonsProc) GetProcAddress (myWinSockDLL, "htons");
    if (stHtoNSProc == NULL)
    {
	// The routine was not found in the DLL.
	return FALSE;
    }
    stInetAddrProc = (inet_addrProc) GetProcAddress (myWinSockDLL, "inet_addr");
    if (stInetAddrProc == NULL)
    {
	// The routine was not found in the DLL.
	return FALSE;
    }
    stInetNtoAProc = (inet_ntoaProc) GetProcAddress (myWinSockDLL, "inet_ntoa");
    if (stInetNtoAProc == NULL)
    {
	// The routine was not found in the DLL.
	return FALSE;
    }
    stIoctlSocketProc = (ioctlsocketProc) GetProcAddress (myWinSockDLL, "ioctlsocket");
    if (stIoctlSocketProc == NULL)
    {
	// The routine was not found in the DLL.
	return FALSE;
    }
    stListenProc = (listenProc) GetProcAddress (myWinSockDLL, "listen");
    if (stListenProc == NULL)
    {
	// The routine was not found in the DLL.
	return FALSE;
    }
    stRecvProc = (recvProc) GetProcAddress (myWinSockDLL, "recv");
    if (stRecvProc == NULL)
    {
	// The routine was not found in the DLL.
	return FALSE;
    }
    stSelectProc = (selectProc) GetProcAddress (myWinSockDLL, "select");
    if (stSelectProc == NULL)
    {
	// The routine was not found in the DLL.
	return FALSE;
    }
    stSendProc = (sendProc) GetProcAddress (myWinSockDLL, "send");
    if (stSendProc == NULL)
    {
	// The routine was not found in the DLL.
	return FALSE;
    }
    stShutDownProc = (shutdownProc) GetProcAddress (myWinSockDLL, "shutdown");
    if (stShutDownProc == NULL)
    {
	// The routine was not found in the DLL.
	return FALSE;
    }
    stSocketProc = (socketProc) GetProcAddress (myWinSockDLL, "socket");
    if (stSocketProc == NULL)
    {
	// The routine was not found in the DLL.
	return FALSE;
    }
    
    // Start up WinSock
    (myWSAStartupProc) (MAKEWORD(2, 0), &stWSAData);

    return TRUE;
} // MDIONet_Init


/************************************************************************/
/* MDIONet_Finalize							*/
/************************************************************************/
void	MDIONet_Finalize (void)
{
    (stWSACleanupProc) ();
} // MDIONet_Finalize


/************************************************************************/
/* MDIO_AllocateSockAddr						*/
/************************************************************************/
SOCK_ADDR	MDIO_AllocateSockAddr (void)
{
    struct sockaddr_in	*mySockAddr;

    mySockAddr = (struct sockaddr_in *) malloc (sizeof (struct sockaddr_in));
    if (mySockAddr != NULL)
    {
	memset (mySockAddr, 0, sizeof (struct sockaddr_in));
    }

    return mySockAddr;
} // MDIO_AllocateSockAddr


/************************************************************************/
/* MDIONet_CheckStatus							*/
/*									*/
/*  This routine checks readability of the socket.  The routine returns	*/
/* true if a recv could be done without blocking.  This occurs when	*/
/* there is data waiting, or when the connection has been closed or	*/
/* when the connection has been reset (WSAECONNRESET).  If the select	*/
/* returns 0, then no reading can be done, but it also means that the	*/
/* connection has not been closed.					*/
/************************************************************************/
BOOL	MDIONet_CheckStatus (SOCK pmSocket, int *pmStatus)
{
    // Check for EOF condition
    // TW : Changing the call to recv to a call to select (non-blocking)
    struct timeval	myTimeout;
    fd_set		mySockets;
    int			myResult;

    FD_ZERO (&mySockets);
    FD_SET ((SOCKET) pmSocket, &mySockets);
    myTimeout.tv_sec = 0;
    myTimeout.tv_usec = 0;

    myResult = (stSelectProc) (0, &mySockets, (fd_set *) NULL, 
		               (fd_set *) NULL, &myTimeout);
    if (myResult == SOCKET_ERROR) 
    {
	// Select failed
	MyProcessWSAError (pmStatus);
	return FALSE;
    }

    return (myResult != 0);
} // MDIONet_CheckStatus


/************************************************************************/
/* MDIONet_CloseSocket							*/
/************************************************************************/
void	MDIONet_CloseSocket (SOCK pmSocket)
{
    (stCloseSocketProc) ((SOCKET) pmSocket);
} // MDIONet_CreateSocket


/************************************************************************/
/* MDIONet_Bind								*/
/************************************************************************/
BOOL	MDIONet_Bind (SOCK pmSocket, int pmPort)
{
    struct sockaddr_in	mySockAddr;
    int			myResult;

    mySockAddr.sin_family = AF_INET;
    mySockAddr.sin_port = (stHtoNSProc) ((USHORT) pmPort);
    mySockAddr.sin_addr.s_addr = INADDR_ANY;
  
    myResult = (stBindProc) ((SOCKET) pmSocket, (struct sockaddr *) &mySockAddr,
        		     sizeof (struct sockaddr));

    if (myResult == SOCKET_ERROR) 
    {
	// Bind failed
	MyProcessWSAError (NULL);
	return FALSE;
    }

    return TRUE;
} // MDIONet_Bind


/************************************************************************/
/* MDIONet_CharsWaiting							*/
/************************************************************************/
BOOL	MDIONet_CharsWaiting (SOCK pmSocket, unsigned long *pmNum, 
			      int *pmStatus)
{
    if ((stIoctlSocketProc) ((SOCKET) pmSocket, FIONREAD, pmNum) == 
							    SOCKET_ERROR) 
    {
	// Check failed
	MyProcessWSAError (pmStatus);
	return FALSE;
    }

    return TRUE;
} // MDIONet_CharsWaiting


/************************************************************************/
/* MDIONet_Connect							*/
/************************************************************************/
BOOL	MDIONet_Connect (SOCK pmSocket, SOCK_ADDR pmSockAddr, int pmPort, 
			 NET_ADDR pmAddr)
{
    int			myResult;

    ((struct sockaddr_in *) pmSockAddr) -> sin_family = AF_INET;
    ((struct sockaddr_in *) pmSockAddr) -> sin_port = 
				    (stHtoNSProc) ((USHORT) pmPort);
    ((struct sockaddr_in *) pmSockAddr) -> sin_addr.s_addr = 
						    (unsigned long) pmAddr;
  
    myResult = (stConnectProc) ((SOCKET) pmSocket, 
				(struct sockaddr *) pmSockAddr, 
				sizeof (struct sockaddr));

    if (myResult == SOCKET_ERROR) 
    {
	// Connect failed
//	MIOCommon_VAOkMsgBox ("connect failed: %ld", 
//			      (stWSAGetLastErrorProc) ());
	MyProcessWSAError (NULL);
	return FALSE;
    }

    return TRUE;
} // MDIONet_Connect


/************************************************************************/
/* MDIONet_CreateSocket							*/
/************************************************************************/
SOCK	MDIONet_CreateSocket (void)
{
    SOCKET	mySocket;

    mySocket = (stSocketProc) (AF_INET, SOCK_STREAM, 0);

    if (mySocket == INVALID_SOCKET)
    {
	MyProcessWSAError (NULL);
	return NULL;
    }

    return (SOCK) mySocket;
} // MDIONet_CreateSocket


/************************************************************************/
/* MDIONet_GetCookie							*/
/************************************************************************/
void	MDIONet_GetCookie (OOTstring pmCookieValue,
			   const OOTstring pmURL,
			   const OOTstring pmCookieName)
{
    char    *myData;
    char    *myPtr, *myEqualsSign, *myEndOfCookie;
    char    *myNamePtr, *myEndNamePtr, *myValPtr, *myEndValPtr;

    myData = MyGetCookies (pmURL);
    if (myData == NULL)
    {
	pmCookieValue [0] = 0;
	return;
    }

    myEndOfCookie = myData - 1;

    while (TRUE)
    {
	// We reached the end last iteration
	if (myEndOfCookie == NULL)
	{
	    MIOError_SetErrNo (E_NET_NO_COOKIE_FOR_THIS_NAME);
	    pmCookieValue [0] = 0;
	    break;
	}
	else
	{
	    myPtr = myEndOfCookie + 1;
	}
	myEndOfCookie = strchr (myPtr, ';');
	if (myEndOfCookie != NULL)
	{
	    *myEndOfCookie = 0;
	}
	myEqualsSign = strchr (myPtr, '=');
	if (myEqualsSign == NULL)
	{
	    // No name=value pair, skip to the next one
	    continue;
	}

	// We foudn an equals sign, trim the name
	myNamePtr = myPtr;
	while (*myNamePtr == ' ')
	{
	    myNamePtr++;
	}
	myEndNamePtr = myEqualsSign;
	do
	{
	    *myEndNamePtr = 0;
	    myEndNamePtr--;
	} while (*myEndNamePtr == ' ');

	// Let's see if there's a match
	if (strcmp (pmCookieName, myNamePtr) != 0)
	{
	    // No match, skip to next one
	    continue;
	}

	// There's a match - Trim the value
	myValPtr = myEqualsSign + 1;
	while (*myValPtr == ' ')
	{
	    myValPtr++;
	}
	myEndValPtr = myValPtr + strlen (myValPtr) - 1;
	while (*myEndValPtr == ' ')
	{
	    *myEndValPtr = 0;
	    myEndValPtr--;
	}
	if (strlen (myValPtr) > 255)
	{
	    // TW Set cookie too long
	    strncpy (pmCookieValue, myValPtr, 255);
	    pmCookieValue [255] = 0;
	}
	else
	{
	    strcpy (pmCookieValue, myValPtr);
	}
	break;
    } // for (;;)

    free (myData);
} // MDIONet_GetCookie


/************************************************************************/
/* MDIONet_GetCookies							*/
/************************************************************************/
//extern void	MDIONet_GetCookies (const OOTString pmURL,
//				    OOTstring pmCookieName []
//				    OOTstring pmCookieValue [])
// {
// } // MDIONet_GetCookies


/************************************************************************/
/* MDIONet_GetHostAddressByName						*/
/************************************************************************/
void	MDIONet_GetHostAddressByName (OOTstring pmNetAddress,
				      const OOTstring pmNetName)
{
    struct hostent	*myHostEntry;
    struct in_addr	myAddress;

    pmNetAddress [0] = 0;

    myHostEntry = (stGetHostByNameProc) (pmNetName);

    if (myHostEntry) 
    {
	memcpy (&myAddress, myHostEntry -> h_addr_list[0], 
		myHostEntry -> h_length);
        strcpy (pmNetAddress, (stInetNtoAProc) (myAddress));
    }
    else 
    {
	MyProcessWSAError (NULL);
    }
} // MDIONet_GetHostAddressByName


/************************************************************************/
/* MDIONet_GetHostName							*/
/************************************************************************/
void	MDIONet_GetHostName (OOTstring pmNetName, int pmMaxLen)
{
    (stGetHostNameProc) (pmNetName, pmMaxLen);
} // MDIONet_GetHostName


/************************************************************************/
/* MDIONet_GetHostNameByAddress						*/
/************************************************************************/
void	MDIONet_GetHostNameByAddress (OOTstring pmNetName,
				      const OOTstring pmNetAddress)
{
    struct hostent	*myHostEntry;
    NET_ADDR		myAddr;

    pmNetName [0] = 0;

    myAddr = (NET_ADDR) (stInetAddrProc) (pmNetAddress);

    if (myAddr == (NET_ADDR) INADDR_NONE) 
    {
	SET_ERRNO (E_NET_BAD_ADDRESS);
	return;
    }

    myHostEntry = (stGetHostByAddrProc) ((char *) &myAddr, 4, AF_INET);

    if (myHostEntry) 
    {
        strcpy (pmNetName, myHostEntry -> h_name);
    }
    else 
    {
	MyProcessWSAError (NULL);
    }
} // MDIONet_GetHostNameByAddress


/************************************************************************/
/* MDIONet_GetNetAddress						*/
/************************************************************************/
void	MDIONet_GetNetAddress (SOCK_ADDR pmSockAddr, char *pmNetAddress)
{
    strcpy (pmNetAddress, 
	    (stInetNtoAProc) (((struct sockaddr_in *) pmSockAddr) -> sin_addr));
} // MDIONet_GetNetAddress


/************************************************************************/
/* MDIONet_GetNetworkAddrByString					*/
/************************************************************************/
NET_ADDR	MDIONet_GetNetworkAddrByString (const OOTstring pmAddress)
{
    NET_ADDR	myAddr;

    // Is the address in "a.b.c.d" format or a machine name
    if (isdigit (pmAddress [0])) 
    {
	myAddr = (NET_ADDR) (stInetAddrProc) (pmAddress);

	if (myAddr == (NET_ADDR) INADDR_NONE) 
	{
	    // Bad address 
	    SET_ERRNO (E_NET_BAD_ADDRESS);
	    return NULL;
	}
    }
    else 
    {
	struct hostent *myHostEntry = (stGetHostByNameProc) (pmAddress);
	struct in_addr myInAddr;

	if (myHostEntry == NULL) 
	{
	    // Couldn't resolve name
	    SET_ERRNO (E_NET_NAME_RESOLUTION_FAILED);
	    return NULL;
	}

	memcpy (&myInAddr, myHostEntry -> h_addr_list[0], 
	        myHostEntry -> h_length);
	myAddr = (NET_ADDR) myInAddr.s_addr;
    }

    return myAddr;
} // MDIONet_GetNetworkAddrByString


/************************************************************************/
/* MDIONet_GetCookie							*/
/************************************************************************/
int	MDIONet_GetNumCookies (const OOTstring pmURL)
{
    char    *myData;
    char    *myPtr, *myEqualsSign, *myEndOfCookie;
    int	    myNumCookies = 0;

    myData = MyGetCookies (pmURL);

    myEndOfCookie = myData - 1;

    while (TRUE)
    {
	// We reached the end last iteration
	if (myEndOfCookie == NULL)
	{
	    break;
	}

	myPtr = myEndOfCookie + 1;
	myEndOfCookie = strchr (myPtr, ';');
	if (myEndOfCookie != NULL)
	{
	    *myEndOfCookie = 0;
	}
	myEqualsSign = strchr (myPtr, '=');
	if (myEqualsSign == NULL)
	{
	    // No name=value pair, skip to the next one
	    continue;
	}

	myNumCookies++;
    } // for (;;)

    free (myData);

    return (myNumCookies);
} // MDIONet_GetNumCookies


/************************************************************************/
/* MDIONet_HasConnected							*/
/************************************************************************/
BOOL	MDIONet_HasConnected (SOCK *pmSocket, SOCK pmOrigSocket,
			      SOCK_ADDR pmSockAddr)
{
    int		myLen = sizeof (struct sockaddr_in);
    SOCKET	mySocket;

    mySocket = (stAcceptProc) ((SOCKET) pmOrigSocket, 
			       (struct sockaddr *) pmSockAddr, &myLen);

    if (mySocket != INVALID_SOCKET)
    {
	* (SOCKET *) pmSocket = mySocket;
	return TRUE;
    }

    return FALSE;
} // MDIONet_HasConnected


/************************************************************************/
/* MDIONet_Listen							*/
/************************************************************************/
BOOL	MDIONet_Listen (SOCK pmSocket, int *pmStatus)
{
    int		myResult;

    myResult = (stListenProc) ((SOCKET) pmSocket, SOMAXCONN);

    if (myResult == SOCKET_ERROR) 
    {
	// Listen failed
	MyProcessWSAError (pmStatus);
	return FALSE;
    }

    return TRUE;
} // MDIONet_Listen


/************************************************************************/
/* MDIONet_Receive							*/
/************************************************************************/
BOOL	MDIONet_Receive (SOCK pmSocket, char *pmBuffer, int pmBytesToRead, 
			 int *pmBytesRead, BOOL *pmWouldBlock, int *pmStatus)
{
    int	myBytesRead;

    if (pmWouldBlock != NULL)
    {
	*pmWouldBlock = FALSE;
    }

    myBytesRead = (stRecvProc) ((SOCKET) pmSocket, pmBuffer, pmBytesToRead, 0);

    if (myBytesRead == SOCKET_ERROR)
    {
	// Receive failed
	if ((stWSAGetLastErrorProc) () == WSAEWOULDBLOCK)
	{
	    if (pmWouldBlock != NULL)
	    {
		*pmWouldBlock = FALSE;
	    }
	}

	MyProcessWSAError (pmStatus);
	*pmBytesRead = 0;
	return FALSE;
    }

    *pmBytesRead = myBytesRead;
    return TRUE;
} // MDIONet_Receive


/************************************************************************/
/* MDIONet_Send								*/
/************************************************************************/
BOOL	MDIONet_Send (SOCK pmSocket, const char *pmMessage, int *pmStatus)
{
    int		myResult;

    myResult = (stSendProc) ((SOCKET) pmSocket, pmMessage, 
			     strlen (pmMessage), 0);

    if (myResult == SOCKET_ERROR) 
    {
	// Send failed
	MyProcessWSAError (pmStatus);
	return FALSE;
    }

    return TRUE;
} // MDIONet_Send


/************************************************************************/
/* MDIONet_SendBytes							*/
/************************************************************************/
BOOL	MDIONet_SendBytes (SOCK pmSocket, const char *pmBuffer, int pmLen,
			   int *pmStatus)
{
    int		myResult;

    myResult = (stSendProc) ((SOCKET) pmSocket, pmBuffer, pmLen, 0);

    if (myResult == SOCKET_ERROR) 
    {
	// Send failed
	MyProcessWSAError (pmStatus);
	return FALSE;
    }

    return TRUE;
} // MDIONet_SendBytes


/************************************************************************/
/* MDIONet_SetCookie							*/
/************************************************************************/
void	MDIONet_SetCookie (const char *pmURL,
			   const OOTstring pmCookieName,
			   const OOTstring pmCookieValue,
			   const char *pmExpiryDate)
{
    char    myString [1024];
    int	    myErr;


    if (pmExpiryDate [0] == 0)
    {
	wsprintf (myString, "%s=%s", pmCookieName, pmCookieValue);
    }
    else
    {
	wsprintf (myString, "%s=%s; Expires=%s", pmCookieName, pmCookieValue, 
		  pmExpiryDate);
    }
    InternetSetCookie (pmURL, NULL, myString);
    myErr = GetLastError ();
    MyProcessWin32Error (myErr);
} // MDIONet_SetCookie


/************************************************************************/
/* MDIONet_SetNonBlocking						*/
/************************************************************************/
BOOL	MDIONet_SetNonBlocking (SOCK pmSocket)
{
    int	myVal = 1;

    // Set to non-blocking mode
    if ((stIoctlSocketProc) ((SOCKET) pmSocket, FIONBIO, &myVal) != 0) 
    {
	MyProcessWSAError(NULL);
	return FALSE;
    }

    return TRUE;
} // MDIONet_SetNonBlocking


/************************************************************************/
/* MDIONet_ShutDownSocket						*/
/************************************************************************/
void	MDIONet_ShutDownSocket (SOCK pmSocket)
{
    (stShutDownProc) ((SOCKET) pmSocket, SD_SEND);
} // MDIONet_ShutDownSocket


/******************************/
/* Static callback procedures */
/******************************/


/*********************/
/* Static procedures */
/*********************/
/************************************************************************/
/* MyGetCookies								*/
/************************************************************************/
static char	*MyGetCookies (const char *pmURL)
{
    BOOL    myRes;
    int	    myErr;
    int	    mySize = 0;
    char    *myData;

    myRes = InternetGetCookie (pmURL, NULL, NULL, &mySize);
    myErr = GetLastError ();
    if (myErr != 0)
    {
	MyProcessWin32Error (myErr);
    }
    if (mySize == 0)
    {
	// Successful - must mean no cookies
	MIOError_SetErrNo (E_NET_NO_COOKIE_FOR_URL);
	return NULL;
    }

    // Allocate the buffer sufficient for the cookie
    myData = malloc (mySize + 1000);
    mySize += 1000;
    myRes = InternetGetCookie (pmURL, NULL, myData, &mySize);
    MyProcessWin32Error (GetLastError ());
    if (myRes)
    {
	return myData;
    }
    else
    {
	free (myData);
	return NULL;
    }
} // MyGetCookies


/************************************************************************/
/* MyProcessWin32Error							*/
/************************************************************************/
static void	MyProcessWin32Error (int pmWin32Error)
{
    switch (pmWin32Error)
    {
	case 0:
	    // Leave previous error untouched.
	    break;
	case ERROR_NO_MORE_ITEMS:
	    MIOError_SetErrNo (E_NET_NO_COOKIE_FOR_URL);
	    break;
    	case 12005:
	    MIOError_SetErrNo (E_NET_BAD_COOKIE_URL);
    	    break;
	case 12006:
	    MIOError_SetErrNo (E_NET_BAD_COOKIE_URL);
    	    break;
	default: 
	    MIOError_SetErrMsg (E_NET_UNKNOWN_ERROR,
    		      "Net Error. Win32 Error #%d", pmWin32Error);
    }
} // MyProcessWin32Error


/************************************************************************/
/* MyProcessWSAError							*/
/************************************************************************/
static void	MyProcessWSAError (int *pmStatus)
{
    switch ((stWSAGetLastErrorProc) ())
    {
    	case WSAECONNABORTED:
    	    MIOError_SetErrNo (E_NET_CONNECTION_ABORTED);
	    if (pmStatus != NULL) 
	    {
		*pmStatus = NET_STATUS_ABORTED_LOCAL;
	    }
    	    break;
	case WSAECONNRESET:    	    
    	    MIOError_SetErrNo (E_NET_CONNECTION_ABORTED_REMOTE);
	    if (pmStatus != NULL) 
	    {
		*pmStatus = NET_STATUS_ABORTED_REMOTE;
	    }
    	    break;
	default: 
	    MIOError_SetErrMsg (E_NET_UNKNOWN_ERROR,
    		      "Net Error. Windows Socket Library Error #%d",
		      (stWSAGetLastErrorProc) ());
    }
} // MyProcessWSAError




