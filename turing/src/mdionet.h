/*************/
/* mdionet.h */
/*************/

#ifndef _MDIONET_H_
#define _MDIONET_H_

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
typedef void	*SOCK;
typedef void	*SOCK_ADDR;
typedef char	*NET_ADDR;

/**********************/
/* External variables */
/**********************/

/***********************/
/* External procedures */
/***********************/
extern BOOL	MDIONet_Init (void);
extern void	MDIONet_Finalize (void);
extern SOCK_ADDR MDIO_AllocateSockAddr (void);
extern BOOL	MDIONet_Bind (SOCK pmSocket, int pmPort);
extern BOOL	MDIONet_CharsWaiting (SOCK pmSocket, unsigned long *pmNum, 
				      int *pmStatus);
extern BOOL	MDIONet_CheckStatus (SOCK pmSocket, int *pmStatus);
extern void	MDIONet_CloseSocket (SOCK pmSocket);
extern BOOL	MDIONet_Connect (SOCK pmSocket, SOCK_ADDR pmSockAddr, 
				 int pmPort, NET_ADDR pmAddr);
extern SOCK	MDIONet_CreateSocket (void);
extern void	MDIONet_GetCookie (OOTstring pmCookieValue,
				   const OOTstring pmURL,
				   const OOTstring pmCookieName);
extern void	MDIONet_GetCookies (const OOTstring pmURL,
				    OOTstring pmCookieName [],
				    OOTint pmCookieNameArraySize,
				    OOTstring pmCookieValue [],
				    OOTint pmCookieValueArraySize);
extern void	MDIONet_GetHostAddressByName (OOTstring pmNetAddress,
					      const OOTstring pmNetName);
extern void	MDIONet_GetHostName (OOTstring pmNetName, int pmMaxLen);
extern void	MDIONet_GetHostNameByAddress (OOTstring pmNetName,
					      const OOTstring pmNetAddress);
extern void	MDIONet_GetNetAddress (void *pmSockAddr, char *pmNetAddress);
extern NET_ADDR	MDIONet_GetNetworkAddrByString (const OOTstring pmAddress);
extern int	MDIONet_GetNumCookies (const OOTstring pmURL);
extern BOOL	MDIONet_HasConnected (SOCK *pmSocket, SOCK pmOrigSocket,
				      SOCK_ADDR pmSockAddr);
extern BOOL	MDIONet_Listen (SOCK pmSocket, int *pmStatus);
extern BOOL	MDIONet_Receive (SOCK pmSocket, char *pmBuffer, 
				 int pmBytesToRead, int *pmBytesRead,
				 BOOL *pmWouldBlock, int *pmStatus);
extern BOOL	MDIONet_Send (SOCK pmSocket, const char *pmMessage, 
			      int *pmStatus);
extern BOOL	MDIONet_SendBytes (SOCK pmSocket, const char *pmBuffer, 
				   int pmLen, int *pmStatus);
extern void	MDIONet_SetCookie (const char *pmURL,
				  const OOTstring pmCookieName,
				  const OOTstring pmCookieValue,
				  const char *pmExpiryDate);
extern BOOL	MDIONet_SetNonBlocking (SOCK pmSocket);
extern void	MDIONet_ShutDownSocket (SOCK pmSocket);
#endif // #ifndef _MDIONET_H_
