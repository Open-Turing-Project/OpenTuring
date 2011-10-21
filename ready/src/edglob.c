/************/
/* edglob.c */
/************/

/*******************/
/* System includes */
/*******************/
#include <windows.h>
#include <crtdbg.h>

/****************/
/* Self include */
/****************/
#include "edglob.h"

/******************/
/* Other includes */
/******************/

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/
#define PAD		1024

#define CH_BAD		1
#define CH_WS		2
#define CH_OK		3

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
BYTE	stCharClass [] = 
    {CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, //   0-  7
     CH_BAD, CH_WS,  CH_WS,  CH_BAD, CH_BAD, CH_WS,  CH_BAD, CH_BAD, //   8- 15
     CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, //  16- 23
     CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, //  24- 31
     CH_WS,  CH_OK,  CH_OK,  CH_OK,  CH_OK,  CH_OK,  CH_OK,  CH_OK,  //  32- 39
     CH_OK,  CH_OK,  CH_OK,  CH_OK,  CH_OK,  CH_OK,  CH_OK,  CH_OK,  //  40- 47
     CH_OK,  CH_OK,  CH_OK,  CH_OK,  CH_OK,  CH_OK,  CH_OK,  CH_OK,  //  48- 55
     CH_OK,  CH_OK,  CH_OK,  CH_OK,  CH_OK,  CH_OK,  CH_OK,  CH_OK,  //  56- 63
     CH_OK,  CH_OK,  CH_OK,  CH_OK,  CH_OK,  CH_OK,  CH_OK,  CH_OK,  //  64- 71
     CH_OK,  CH_OK,  CH_OK,  CH_OK,  CH_OK,  CH_OK,  CH_OK,  CH_OK,  //  72- 79
     CH_OK,  CH_OK,  CH_OK,  CH_OK,  CH_OK,  CH_OK,  CH_OK,  CH_OK,  //  80- 87
     CH_OK,  CH_OK,  CH_OK,  CH_OK,  CH_OK,  CH_OK,  CH_OK,  CH_OK,  //  88- 95
     CH_OK,  CH_OK,  CH_OK,  CH_OK,  CH_OK,  CH_OK,  CH_OK,  CH_OK,  //  96-103
     CH_OK,  CH_OK,  CH_OK,  CH_OK,  CH_OK,  CH_OK,  CH_OK,  CH_OK,  // 104-111
     CH_OK,  CH_OK,  CH_OK,  CH_OK,  CH_OK,  CH_OK,  CH_OK,  CH_OK,  // 112-119
     CH_OK,  CH_OK,  CH_OK,  CH_OK,  CH_OK,  CH_OK,  CH_OK,  CH_BAD, // 120-127

     CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, // 128-135
     CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, // 136-143
     CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, // 144-151
     CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, // 152-159
     CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, // 160-167
     CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, // 168-175
     CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, // 176-183
     CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, // 184-191
     CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, // 192-199
     CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, // 200-207
     CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, // 208-215
     CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, // 216-223
     CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, // 224-231
     CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, // 232-239
     CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, // 240-247
     CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD, CH_BAD};// 248-255

/********************/
/* Static variables */
/********************/

/******************************/
/* Static callback procedures */
/******************************/

/*********************/
/* Static procedures */
/*********************/


/***********************/
/* External procedures */
/***********************/
/******************************************************************/
/* EdGlob_AddString						  */
/******************************************************************/
char	*EdGlob_AddString (char *pmBuffer, int *pmSize, const char *pmAddString)
{
    int		myLen = strlen (pmBuffer);
    int		myAddLen = strlen (pmAddString);
    char	*myNewBuffer = NULL;
    
    if (pmBuffer == NULL)
    {
    	*pmSize = myAddLen + PAD;
    	myNewBuffer = malloc (*pmSize);
    	if (pmBuffer != NULL)
    	{
    	    strcpy (pmBuffer, pmAddString);
    	}
    	return myNewBuffer;
    }
    else if (myLen + myAddLen >= *pmSize - 3)
    {
    	*pmSize = myLen + myAddLen + PAD;
    	myNewBuffer = realloc (pmBuffer, *pmSize);
    	if (myNewBuffer != NULL)
    	{
    	    strcat (myNewBuffer, pmAddString);
    	}
    	return myNewBuffer;
    }
    else
    {
    	*pmSize += myAddLen;
    	strcat (pmBuffer, pmAddString);
	return pmBuffer;
    }    
} // EdGlob_AddString


/******************************************************************/
/* EdGlob_ConvertStringToNum					  */
/*								  */
/* Convert a string to a number.  It ignores leading and trailing */
/* whitespace.  If the string is non-numeric, it returns FALSE,   */
/* otherwise it sets pmValue and returns TRUE			  */
/******************************************************************/
BOOL	EdGlob_ConvertStringToNum (char *pmString, int *pmValue)
{
    char	*myPtr;
    int		myTotal = 0;
    
    // Skip leading spaces
    myPtr = pmString;
    while ((*myPtr == ' ') || (*myPtr == '\t'))
    {
    	myPtr++;
    }

    // Make certain there is a number    
    if (*myPtr == 0)
    {
    	return FALSE;
    }
 
    // Convert string to a number   
    while (('0' <= *myPtr) && (*myPtr <= '9'))
    {
    	myTotal = myTotal * 10 + (*myPtr - '0');
    	myPtr++;
    }

    // Skip trailing spaces    
    while ((*myPtr == ' ') || (*myPtr == '\t'))
    {
    	myPtr++;
    }
    
    // Make certain to non-numerics
    if (*myPtr != 0)
    {
    	return FALSE;
    }
    
    *pmValue = myTotal;
    return TRUE;
} // EdGlob_ConvertStringToNum


/*****************************/
/* EdGlob_ConvertToLowerCase */
/*****************************/
void	EdGlob_ConvertToLowerCase (char *pmString)
{
    UINT    cnt;
    
    for (cnt = 0 ; cnt < strlen (pmString) ; cnt++)
    {
    	if (('A' <= pmString [cnt]) && (pmString [cnt] <= 'Z'))
    	{
    	    pmString [cnt] += ('a' - 'A');
    	}
    } // for
} // EdGlob_ConvertToLowerCase


/*****************************/
/* EdGlob_ConvertToUpperCase */
/*****************************/
void	EdGlob_ConvertToUpperCase (char *pmString)
{
    UINT    cnt;
    
    for (cnt = 0 ; cnt < strlen (pmString) ; cnt++)
    {
    	if (('a' <= pmString [cnt]) && (pmString [cnt] <= 'z'))
    	{
    	    pmString [cnt] -= ('a' - 'A');
    	}
    } // for
} // EdGlob_ConvertToUpperCase

/************************************************************************/
/* EdGlob_CreateStringLink						*/
/************************************************************************/
StringLink	EdGlob_CreateStringLink (const char *pmString)
{
    StringLink	myLink;
    
    myLink = calloc (sizeof (struct StringLink), 1);
    if (myLink != NULL)
    {
    	myLink -> string = malloc (strlen (pmString) + 1);
    	if (myLink -> string == NULL)
    	{
    	    return NULL;
    	}
    	strcpy (myLink -> string, pmString);
    }
    
    return myLink;
} // EdGlob_CreateStringLink


/************************************************************************/
/* EdGlob_EndsWith							*/
/************************************************************************/
BOOL	EdGlob_EndsWith (const char *pmString, const char *pmSearchString)
{
    UINT	myLen, mySearchLen;
    
    if (pmString == NULL)
    {
	return FALSE;
    }

    myLen = strlen (pmString);
    mySearchLen = strlen (pmSearchString);
    
    if (mySearchLen > myLen)
    {
    	return FALSE;
    }
    
    return (strcmp (&pmString [myLen - mySearchLen], pmSearchString) == 0);
} // EdGlob_EndsWith


/************************************************************************/
/* EdGlob_FreeStringLinkList						*/
/*									*/
/* Free up a linked list headed by pmLink. Also sets pmLink to NULL.	*/
/************************************************************************/
void	EdGlob_FreeStringLinkList (StringLink *pmLink)
{
    StringLink	myLink, myNextLink;
    
    myLink = *pmLink;
    while (myLink != NULL)
    {
    	myNextLink = myLink -> next;
    	free (myLink -> string);
    	free (myLink);
    	myLink = myNextLink;
    }
    
    *pmLink = NULL;
} // EdGlob_FreeStringLinkList


/***********************/
/* EdGlob_IsWhitespace */
/***********************/
BOOL	EdGlob_IsWhitespace (char pmChar)
{
    return (stCharClass [pmChar] == CH_WS);
} // EdGlob_IsWhitespace

			   
/************************************************************************/
/* EdGlob_MallocString							*/
/************************************************************************/
char	*EdGlob_MallocString (const char *pmString)
{
    char	*myString;
    
    myString = malloc (strlen (pmString) + 1);
    if (myString != NULL)
    {
    	strcpy (myString, pmString);
    }
    return myString;
} // EdGlob_MallocString


/************************************************************************/
/* EdGlob_Trim								*/
/************************************************************************/
void	EdGlob_Trim (char *pmString)
{
    char	*myPtr = pmString;
    char	*myStart = pmString;
    char	*myEnd = pmString + strlen (pmString) - 1;

    while (EdGlob_IsWhitespace (*myPtr))
    {
	myPtr++;
    }
    while (EdGlob_IsWhitespace (*myEnd))
    {
	myEnd--;
    }
    myEnd++;
    if (myPtr != myStart)
    {
	while (myPtr < myEnd)
	{
	    *myStart++ = *myPtr++;
	}
	*myStart = 0;
    }
    else
    {
	*myEnd = 0;
    }
} // EdGlob_Trim


/******************************/
/* Static callback procedures */
/******************************/


/*********************/
/* Static procedures */
/*********************/
