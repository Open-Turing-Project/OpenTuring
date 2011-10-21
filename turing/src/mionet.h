/************/
/* mionet.h */
/************/

#ifndef _MIONET_H_
#define _MIONET_H_

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
#define NET_STATUS_NORMAL	    0
#define NET_STATUS_CLOSED_REMOTE    1
#define NET_STATUS_ABORTED_LOCAL    2
#define NET_STATUS_ABORTED_REMOTE   3

/*********/
/* Types */
/*********/

/**********************/
/* External variables */
/**********************/

/*********************************************/
/* External procedures for Turing predefines */
/*********************************************/
extern OOTint	MIONet_Available (OOTint pmNetID, OOTint pmType);
extern int	MIONet_ConnectionStatus (OOTint pmNetID);
extern void	MIONet_GetAddress (OOTint pmNetID, OOTstring pmNetAddress);
extern void	MIONet_DeleteCookie (const OOTstring pmURL,
				     const OOTstring pmCookieName);
extern void	MIONet_GetCookie (OOTstring pmCookieValue,
				  const OOTstring pmURL,
				  const OOTstring pmCookieName);
extern void	MIONet_GetCookies (const OOTstring pmURL,
				   OOTstring pmCookieName [],
				   OOTint pmCookieNameArraySize,
				   OOTstring pmCookieValue [],
				   OOTint pmCookieValueArraySize);
extern int	MIONet_GetNumCookies (const OOTstring pmURL);
extern void	MIONet_GetHostAddressByName (OOTstring pmNetAddress,
					     const OOTstring pmNetName);
extern void	MIONet_GetHostNameByAddress (OOTstring pmNetName,
					     const OOTstring pmNetAddress);
extern void	MIONet_GetLocalAddress (OOTstring pmNetAddress);
extern void	MIONet_GetLocalName (OOTstring pmNetName);
extern void	MIONet_RegisterClose (OOTint pmNetID);
extern void	MIONet_RegisterOpen (OOTint pmNetID, SRCPOS *pmSrcPos);
extern void	MIONet_SetCookie (const OOTstring pmURL,
				  const OOTstring pmCookieName,
				  const OOTstring pmCookieValue,
				  OOTint pmExpiryDate);
extern BOOL	MIONet_WaitForConnect (OOTint pmNetID, 
				       EventDescriptor *pmEvent);

/***************************************/
/* External procedures for MIO library */
/***************************************/
extern void	MIONet_Init (void);
extern void	MIONet_Finalize (void);
extern BOOL	MIONet_Close (void *pmFilePtr);
extern int	MIONet_Getc (void *pmFilePtr);
extern BOOL	MIONet_HasEvent (void *pmFilePtr, EventDescriptor *pmEvent);
extern void	*MIONet_Open (char *pmOpenString);
extern int	MIONet_Putc (char pmChar, void *pmFilePtr);
extern int	MIONet_Puts (const char *pmString, void *pmFilePtr);
extern int	MIONet_Read (void *pmBuffer, int pmSize, void *pmFilePtr);
extern void	MIONet_Ungetc (OOTint pmChar, void *pmFilePtr);
extern int	MIONet_Write (void *pmBuffer, int pmSize, void *pmFilePtr);

#endif // #ifndef _MIONET_H_
