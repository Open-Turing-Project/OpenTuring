/*************/
/* miofont.c */
/*************/

/*******************/
/* System includes */
/*******************/
#include <malloc.h>
#include <string.h>

/****************/
/* Self include */
/****************/
#include "miofont.h"

/******************/
/* Other includes */
/******************/
#include "mdiofont.h"

#include "mio.h"
#include "mdio.h"

#include "mioerr.h"

// Test to make certain we're not accidentally including <windows.h> which
// might allow for windows contamination of platform independent code.
#ifdef _INC_WINDOWS
xxx
#endif

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/
#define BOLD_KEYWORD		"bold"
#define ITALIC_KEYWORD		"italic"
#define UNDERLINE_KEYWORD	"underline"
#define NAA_KEYWORD		"naa"
#define NO_ANTI_ALIAS_KEYWORD	"noantialias"

/********************/
/* Global variables */
/********************/

/*********/
/* Types */
/*********/
typedef struct FontInfo
{
    int			height, ascent, descent, internalLeading;
    FONT		mdFontInfo;
} FontInfo;

// Used for comparing two fonts
typedef struct FontCompareInfo
{
    char	typeFace [1024];
    int		height, width;
    int		bold, italic, underline;
    BOOL	noAntiAlias;
} FontCompareInfo;

// Used for holding the font enumeration
typedef struct FontSize
{
    int			size;
    struct FontSize	*nextSize;
} FontSize;

typedef struct FontName
{
    char 		*fontName;
    FontSize		*sizes [2][2][2];
    struct FontName	*nextFont;
} FontName;

/**********************/
/* External variables */
/**********************/

/********************/
/* Static constants */
/********************/

/********************/
/* Static variables */
/********************/
static BOOL	stFontsEnumerated;
static FontName	*stFontListHead;
static FontName	*stFontPtr;
static FontSize	*stFontSizePtr;

/******************************/
/* Static callback procedures */
/******************************/

/*********************/
/* Static procedures */
/*********************/
static void	MyAddFontName (const char *pmFontName, FontName *pmBefore, 
			       FontName **pmHead, FontName *pmAfter,
			       int pmHeight, BOOL pmIsBold, BOOL pmIsItalic,
			       BOOL pmIsUnderline);
static void	MyAddFontSize (int pmHeight, FontSize *pmBefore, 
			       FontSize **pmHead, FontSize *pmAfter);
static void	MyFreeFont (FontInfo *pmFontInfo);
static FontName	*MyGetFont (const char *pmFontName);
static void	MyGetFontAttrib (const char *pmStyle, int *pmBold, 
				 int *pmItalic, int *pmUnderline,
				 BOOL *pmNoAntiAlias);


/*********************************************/
/* External procedures for Turing predefines */
/*********************************************/
/************************************************************************/
/* MIOFont_Free								*/
/************************************************************************/
void	MIOFont_Free (OOTint pmFontID)
{
    FontInfo *myFontInfo;

    if (MIO_IDGetCount (pmFontID, FONT_ID) > 1)
    {
	// The font has been 'new'ed several times,  Decrement the use count
	// by one.
	MIO_IDDecrement (pmFontID);
    }
    else
    {
	// The font has been free'd once for each 'new'.  Now get rid of it.
	myFontInfo = MIO_IDGet (pmFontID, FONT_ID);
	MyFreeFont (myFontInfo);
	MIO_IDRemove (pmFontID, FONT_ID);
    }
} // MIOFont_Free


/************************************************************************/
/* MIOFont_GetName							*/
/************************************************************************/
void	MIOFont_GetName (char *pmFontName)
{
    if (stFontPtr == NULL)
    {
    	pmFontName [0] = 0;
    }
    else
    {
    	strncpy (pmFontName, stFontPtr -> fontName, 255);
    	stFontPtr = stFontPtr -> nextFont;
    }
} // MIOFont_GetName


/************************************************************************/
/* MIOFont_GetSize							*/
/************************************************************************/
OOTint	MIOFont_GetSize (void)
{
    OOTint	myResult;
    
    if (stFontSizePtr == NULL)
    {
    	myResult = 0;
    }
    else
    {
    	myResult = stFontSizePtr -> size;
    	stFontSizePtr = stFontSizePtr -> nextSize;
    }
    
    return myResult;
} // MIOFont_GetSize


/************************************************************************/
/* MIOFont_GetStyle							*/
/************************************************************************/
void	MIOFont_GetStyle (char *pmFontName, OOTboolean *pmBold, 
			  OOTboolean *pmItalic, OOTboolean *pmUnderline)
{
    FontName	*myFontPtr = MyGetFont (pmFontName);
    
    *pmBold = 0;
    *pmItalic = 0;
    *pmUnderline = 0;
    
    if (myFontPtr == NULL)
    {
    	return;
    }
    	    
    if ((myFontPtr -> sizes [1][0][0] != NULL) || 
        (myFontPtr -> sizes [1][0][1] != NULL) ||
        (myFontPtr -> sizes [1][1][0] != NULL) ||
        (myFontPtr -> sizes [1][1][1] != NULL))
    {
    	*pmBold = 1;
    }    	        
    if ((myFontPtr -> sizes [0][1][0] != NULL) || 
        (myFontPtr -> sizes [0][1][1] != NULL) ||
        (myFontPtr -> sizes [1][1][0] != NULL) ||
        (myFontPtr -> sizes [1][1][1] != NULL))
    {
    	*pmItalic = 1;
    }    	        
    if ((myFontPtr -> sizes [0][0][1] != NULL) || 
        (myFontPtr -> sizes [0][1][1] != NULL) ||
        (myFontPtr -> sizes [1][0][1] != NULL) ||
        (myFontPtr -> sizes [1][1][1] != NULL))
    {
    	*pmUnderline = 1;
    }
} // MIOFont_GetStyle


/************************************************************************/
/* MIOFont_Name								*/
/************************************************************************/
void	MIOFont_Name (OOTint pmFontID, OOTstring pmFontName)
{
    FontInfo *myFontInfo;

    myFontInfo = MIO_IDGet (pmFontID, FONT_ID);

    MDIOFont_GetName (myFontInfo -> mdFontInfo, pmFontName);    
} // MIOFont_Name


/************************************************************************/
/* MIOFont_New								*/
/************************************************************************/
OOTint	MIOFont_New (OOTstring pmFontStr, SRCPOS *pmSrcPos)
{
    char	myTypeFace [256], mySizeStr [256], myStyleStr [256];
    char	*mySrc, *myDest, *myPtr, *myStartOfSize;
    int		myHeight, myWidth, myBold, myItalic, myUnderline;
    BOOL	myNoAntiAlias;
    BOOL	myNegativeHeight = FALSE, myNegativeWidth = FALSE;
    FontInfo	*myFontInfo;
    FontCompareInfo	*myFontCompareInfo;
    int		myFontID;
       
    // Copy the type face to typeFace
    if (strchr (pmFontStr, ':') == NULL)
    {
    	char	myMessage [1024];
    	
    	MDIO_sprintf (myMessage, "No type size in font selection string '%s'", 
    		      pmFontStr);
    	ABORT_WITH_ERRMSG (E_FONT_BAD_FONT_SELECT_STR, myMessage);
	return 0;
    } 

    // Seperate out the font name
    mySrc = pmFontStr;
    myDest = myTypeFace;    
    while (*mySrc != ':') 
    {
    	*myDest++ = *mySrc++;
    }
    *myDest = 0;
    mySrc++;

    //
    // Get the width and height
    //
    myHeight = 0;
    myWidth = 0;
    myStartOfSize = mySrc;
    myDest = mySizeStr;
    if (*mySrc == '-')
    {
    	myNegativeHeight = TRUE;
    	mySrc++;
    }
    while (('0' <= *mySrc) && (*mySrc <= '9'))
    {
    	*myDest++ = *mySrc++;
    }
    if ((*mySrc != ':') && (*mySrc != 'x') && (*mySrc != 'X') && (*mySrc != 0))
    {
    	char	myMessage [1024];
    	
    	MDIO_sprintf (myMessage, "Bad font size '%s' in font selection string", 
    		      myStartOfSize);
    	ABORT_WITH_ERRMSG (E_FONT_BAD_FONT_SELECT_STR, myMessage);
	return 0;
    } 
    *myDest = 0;
    myPtr = mySizeStr;
    while (*myPtr != 0) 
    {
    	myHeight = myHeight * 10 + (*myPtr++ - '0');
    }
    if (myNegativeHeight)
    {
    	myHeight = -myHeight;
    }
    if ((*mySrc == 'x') || (*mySrc == 'X'))
    {
	mySrc++;
	myDest = mySizeStr;
    	if (*mySrc == '-')
    	{
    	    myNegativeWidth = TRUE;
    	    mySrc++;
    	}
	while (('0' <= *mySrc) && (*mySrc <= '9'))
	{
	    *myDest++ = *mySrc++;
	}
	if ((*mySrc != ':') && (*mySrc != 0))
	{
    	    char	myMessage [1024];
    	
    	    MDIO_sprintf (myMessage, "Bad font size '%s' in font selection string", 
    		          myStartOfSize);
    	    ABORT_WITH_ERRMSG (E_FONT_BAD_FONT_SELECT_STR, myMessage);
	    return 0;
	}
	*myDest = 0;
    	myPtr = mySizeStr;
    	while (*myPtr != 0) 
    	{
    	    myWidth = myWidth * 10 + (*myPtr++ - '0');
    	}
    	if (myNegativeWidth)
    	{
    	    myWidth = -myWidth;
    	}
    }

    //
    // Get the font style
    //
    if (*mySrc == ':')
    {
	mySrc++;
	myDest = myStyleStr;
	while (*mySrc != 0)
	{
	    *myDest++ = *mySrc++;
	}
	*myDest = 0;
    }
    else myStyleStr [0] = 0;
    
    MyGetFontAttrib (myStyleStr, &myBold, &myItalic, &myUnderline, 
	&myNoAntiAlias);
  
    // Allocate
    myFontCompareInfo = 
	(FontCompareInfo *) calloc (sizeof (FontCompareInfo), 1);
    strcpy (myFontCompareInfo -> typeFace, myTypeFace);
    myFontCompareInfo -> height = myHeight;
    myFontCompareInfo -> width = myWidth;
    myFontCompareInfo -> bold = myBold;
    myFontCompareInfo -> italic = myItalic;
    myFontCompareInfo -> underline = myUnderline;
    myFontCompareInfo -> noAntiAlias = myNoAntiAlias;

    myFontID = MIO_IDCompare (FONT_ID, myFontCompareInfo, 
			      sizeof (FontCompareInfo));

    if (myFontID != 0)
    {
	// A font with all these characteristics already exists.  Incremenent
	// that font's count instead of creating a new font.
	MIO_IDIncrement (myFontID);
	free (myFontCompareInfo);
	return myFontID;
    }

    myFontInfo = (FontInfo *) calloc (sizeof (FontInfo), 1);
    
    myFontInfo -> mdFontInfo = MDIOFont_CreateFont (myTypeFace, myHeight, 
    						    myWidth, myBold, myItalic,
    			 			    myUnderline, myNoAntiAlias);

    MDIOFont_GetSizes (myFontInfo -> mdFontInfo, &myFontInfo -> height,
    		       &myFontInfo -> ascent, &myFontInfo -> descent, 
    		       &myFontInfo -> internalLeading);
    
    myFontID = MIO_IDAdd (FONT_ID, myFontInfo, pmSrcPos, pmFontStr, 
			  myFontCompareInfo);

    // Couldn't allocate an identifier.  Return the default font ID.
    if (myFontID == 0)
    {
	MyFreeFont (myFontInfo);
	return DEFAULT_FONT_ID;
    }
        	    
    return myFontID;
} // MIOFont_New


/************************************************************************/
/* MIOFont_Sizes							*/
/************************************************************************/
void	MIOFont_Sizes (OOTint pmFontID, OOTint *pmHeight, OOTint *pmAscent, 
		       OOTint *pmDescent, OOTint *pmInternalLeading)
{
    FontInfo *myFontInfo;

    if (pmFontID == DEFAULT_FONT_ID)
    {
	*pmHeight = MIO_selectedRunWindowInfo -> fontCharHeight;
	*pmAscent = MIO_selectedRunWindowInfo -> fontCharAscent;
	*pmDescent = MIO_selectedRunWindowInfo -> fontCharDescent;
	*pmInternalLeading = MIO_selectedRunWindowInfo -> fontCharIntLeading;
    }
    else
    {
	myFontInfo = MIO_IDGet (pmFontID, FONT_ID);
	
	*pmHeight = myFontInfo -> height;
	*pmAscent = myFontInfo -> ascent;
	*pmDescent = myFontInfo -> descent;
	*pmInternalLeading = myFontInfo -> internalLeading;
    }
} // MIOFont_Sizes


/************************************************************************/
/* MIOFont_StartName							*/
/************************************************************************/
void	MIOFont_StartName (void)
{
    if (!stFontsEnumerated)
    {
    	//
    	// Enumerate all the fonts and place them in a data structure for 
    	// later use.  MDIOFont_EnumerateAllFonts calls MIOFont_something 
    	// or other to place data in a independent data structure.
    	//
    	MDIOFont_EnumerateAllFonts ();
    	stFontsEnumerated = TRUE;
    }
    
    stFontPtr = stFontListHead;
} // MIOFont_StartName


/************************************************************************/
/* MIOFont_StartSize							*/
/************************************************************************/
void	MIOFont_StartSize (const OOTstring pmFontName, 
			   const OOTstring pmFontStyle)
{
    FontName	*myFontPtr = MyGetFont (pmFontName);
    int		myBold, myItalic, myUnderline;
    BOOL	myNoAntiAlias;

    stFontSizePtr = NULL;
    
    if (myFontPtr == NULL)
    {
    	return;
    }
    
    MyGetFontAttrib (pmFontStyle, &myBold, &myItalic, &myUnderline, 
	&myNoAntiAlias);

    stFontSizePtr = myFontPtr -> sizes [myBold] [myItalic] [myUnderline];
} // MIOFont_StartSize


/************************************************************************/
/* MIOFont_Width							*/
/************************************************************************/
OOTint	MIOFont_Width (OOTint pmFontID, OOTstring pmString)
{
    FONT	myFont;

    if (pmFontID == DEFAULT_FONT_ID)
    {
    	myFont = MIO_selectedRunWindowInfo -> font;
    }
    else
    {
    	myFont = MIOFont_GetFont (pmFontID);
    }

    return MDIOFont_GetWidth (myFont, pmString);    
} // MIOFont_Width


/***************************************/
/* External procedures for MIO library */
/***************************************/
/************************************************************************/
/* MIOFont_Init								*/
/************************************************************************/
void	MIOFont_Init_Run (void)
{
    stFontsEnumerated = FALSE;
    stFontListHead = NULL;
    stFontPtr = NULL;
} // MIOFont_Init


/************************************************************************/
/* MIOFont_Finalize							*/
/************************************************************************/
void	MIOFont_Finalize_Run (void)
{
    //
    // Free all the allocated memory.
    //
    FontName	*myFontPtr, *myNextFontPtr;
    FontSize	*myFontSizePtr, *myNextFontSizePtr;
    int		myBold, myItalic, myUnderline;
        
    myFontPtr = stFontListHead;
    while (myFontPtr != NULL)
    {
    	for (myBold = 0 ; myBold < 2 ; myBold++)
    	{
    	    for (myItalic = 0 ; myItalic < 2 ; myItalic++)
    	    {
    	        for (myUnderline = 0 ; myUnderline < 2 ; myUnderline++)
    	        {
    	            myFontSizePtr = 
    	            	myFontPtr -> sizes [myBold] [myItalic] [myUnderline];
		    while (myFontSizePtr != NULL)
		    {
		    	myNextFontSizePtr = myFontSizePtr -> nextSize;
		    	free (myFontSizePtr);
		    	myFontSizePtr = myNextFontSizePtr;
		    } // while
		} // for underline
	    } // for italic
	} // for bold
	myNextFontPtr = myFontPtr -> nextFont;
	free (myFontPtr);
	myFontPtr = myNextFontPtr;
    } // while
    
    stFontListHead = NULL;
} // MIOFont_Finalize


/************************************************************************/
/* MIOFont_AddFont							*/
/************************************************************************/
void	MIOFont_AddFont (const char *pmFontName, int pmHeight, BOOL pmIsBold, 
		         BOOL pmIsItalic, BOOL pmIsUnderline)
{
    // First, find the insertion point in the list for the font name
    FontName	*myFontPtr = stFontListHead;
    FontName	*myPrevFontPtr = NULL;
    
    while (myFontPtr != NULL)
    {
    	if (strcmp (pmFontName, myFontPtr -> fontName) <= 0)
    	{
    	    //
    	    // Create the FontName entry, if necessary
    	    //
	    MyAddFontName (pmFontName, myPrevFontPtr, &stFontListHead,
	    		   myFontPtr, pmHeight, pmIsBold, pmIsItalic,
	    		   pmIsUnderline);
	    return;
	}
	myPrevFontPtr = myFontPtr;
	myFontPtr = myFontPtr -> nextFont;		    		
    } // while
    
    MyAddFontName (pmFontName, myPrevFontPtr, &stFontListHead,
    		   NULL, pmHeight, pmIsBold, pmIsItalic, pmIsUnderline);
} // MIOFont_AddFont


/************************************************************************/
/* MIOFont_GetFont							*/
/************************************************************************/
FONT	MIOFont_GetFont (OOTint pmFontID)
{
    return ((FontInfo *) MIO_IDGet (pmFontID, FONT_ID)) -> mdFontInfo;
} // MIOFont_GetFont


/************************************************************************/
/* MIOFont_GetFontAscent						*/
/************************************************************************/
int	MIOFont_GetFontAscent (OOTint pmFontID)
{
    return ((FontInfo *) MIO_IDGet (pmFontID, FONT_ID)) -> ascent;
} // MIOFont_GetFontAscent


/******************************/
/* Static callback procedures */
/******************************/


/*********************/
/* Static procedures */
/*********************/
/************************************************************************/
/* MyAddFontName							*/
/*									*/
/* Create an entry in the font name list and add the font size		*/
/************************************************************************/
static void	MyAddFontName (const char *pmFontName, FontName *pmBefore, 
			       FontName **pmHead, FontName *pmAfter,
			       int pmHeight, BOOL pmIsBold, BOOL pmIsItalic,
			       BOOL pmIsUnderline)
{
    FontName	*myNewFont;
    FontSize	*myFontSizePtr, *myPrevFontSizePtr, **myFontSizeHead;
    int		myBoldIndex, myItalicIndex, myUnderlineIndex;

    if ((pmAfter == NULL) || (strcmp (pmFontName, pmAfter -> fontName) < 0))
    {
    	    	
    	myNewFont = (FontName *) calloc (sizeof (FontName), 1);
    	myNewFont -> fontName = MIO_MallocString (pmFontName);
    	myNewFont -> nextFont = pmAfter;
    	    	
    	if (pmBefore == NULL)
    	{
    	   *pmHead = myNewFont;
    	}
    	else
    	{
    	    pmBefore -> nextFont = myNewFont;
    	}
    }
    else
    {
    	myNewFont = pmAfter;
    }
    
    // Add the size to the appropriate list
    if (pmIsBold) myBoldIndex = 1;
    else myBoldIndex = 0;
    if (pmIsItalic) myItalicIndex = 1;
    else myItalicIndex = 0;
    if (pmIsUnderline) myUnderlineIndex = 1;
    else myUnderlineIndex = 0;

    myFontSizeHead = 
    	&myNewFont -> sizes [myBoldIndex] [myItalicIndex] [myUnderlineIndex];
    myFontSizePtr = *myFontSizeHead;	    	    			       	   
    myPrevFontSizePtr = NULL;
	    	    	    			       	   
    while (myFontSizePtr != NULL)
    {
    	if (pmHeight <= myFontSizePtr -> size)
    	{
    	    // Create the FontSize entry, if necessary
    	    if (pmHeight < myFontSizePtr -> size)
    	    {
	    	MyAddFontSize (pmHeight, myPrevFontSizePtr, 
	    		       myFontSizeHead, myFontSizePtr);
    	    }
    	    return;
    	}
    	myPrevFontSizePtr = myFontSizePtr;
    	myFontSizePtr = myFontSizePtr -> nextSize;
    } // while
    
    // Add the font size to the end of the list
    MyAddFontSize (pmHeight, myPrevFontSizePtr, myFontSizeHead, NULL);
} // MyAddFontName


/************************************************************************/
/* MyAddFontSize							*/
/************************************************************************/
static void	MyAddFontSize (int pmHeight, FontSize *pmBefore, 
			       FontSize **pmHead, FontSize *pmAfter)
{
    FontSize	*myNewFontSize;
    	    	
    myNewFontSize = (FontSize *) calloc (sizeof (FontSize), 1);
    myNewFontSize -> size = pmHeight;
    myNewFontSize -> nextSize = pmAfter;

    if (pmBefore == NULL)
    {
	*pmHead = myNewFontSize;
    }
    else
    {
	pmBefore -> nextSize = myNewFontSize;
    }
} // MyAddFontSize


/************************************************************************/
/* MyFreeFont								*/
/************************************************************************/
static void	MyFreeFont (FontInfo *pmFontInfo)
{
    MDIOFont_FreeFont (pmFontInfo -> mdFontInfo);
    free (pmFontInfo);
} // MyFreeFont


/************************************************************************/
/* MyGetFont								*/
/************************************************************************/
static FontName	*MyGetFont (const char *pmFontName)
{
    FontName	*myFontPtr = stFontListHead;
    
    while (myFontPtr != NULL)
    {
    	if (strcmp (pmFontName, myFontPtr -> fontName) == 0)
    	{
	    return myFontPtr;
	} // if font found
	
	myFontPtr = myFontPtr -> nextFont;
    } // while

    SET_ERRMSG (E_FONT_NAME_NOT_FOUND, "No font named '%s' found", 
    		pmFontName);
    
    return NULL;
} // MyGetFont


/************************************************************************/
/* MyGetFontAttrib							*/
/************************************************************************/
static void	MyGetFontAttrib (const char *pmStyle, int *pmBold, 
				 int *pmItalic, int *pmUnderline,
				 BOOL *pmNoAntiAlias)
{
    const char	*myPtr = pmStyle;
    char	myMessage [1024];

    *pmBold = 0;
    *pmItalic = 0;
    *pmUnderline = 0;
    *pmNoAntiAlias = FALSE;

    while (*myPtr != 0)
    {
    	if ((*myPtr == ' ') || (*myPtr == ','))
    	{
    	    myPtr++;
    	}
    	else if (_strnicmp (myPtr, BOLD_KEYWORD, strlen (BOLD_KEYWORD)) == 0)
    	{
    	    *pmBold = 1;
    	    myPtr += strlen (BOLD_KEYWORD);
    	    if ((*myPtr != ' ') && (*myPtr != ',') && (*myPtr != 0))
    	    {
    	    	myPtr -= strlen (BOLD_KEYWORD);
    	    	MDIO_sprintf (myMessage, "Unknown font style '%s'", myPtr);
	    	ABORT_WITH_ERRMSG (E_FONT_UNKNOWN_FONT_STYLE, myMessage);
    	    }
    	}
    	else if (_strnicmp (myPtr, ITALIC_KEYWORD, strlen (ITALIC_KEYWORD)) == 0)
    	{
    	    *pmItalic = 1;
    	    myPtr += strlen (ITALIC_KEYWORD);
    	    if ((*myPtr != ' ') && (*myPtr != ',') && (*myPtr != 0))
    	    {
    	    	myPtr -= strlen (ITALIC_KEYWORD);
    	    	MDIO_sprintf (myMessage, "Unknown font style '%s'", myPtr);
	    	ABORT_WITH_ERRMSG (E_FONT_UNKNOWN_FONT_STYLE, myMessage);
    	    }
    	}
    	else if (_strnicmp (myPtr, UNDERLINE_KEYWORD, 
    					strlen (UNDERLINE_KEYWORD)) == 0)
    	{
    	    *pmUnderline = 1;
    	    myPtr += strlen (UNDERLINE_KEYWORD);
    	    if ((*myPtr != ' ') && (*myPtr != ',') && (*myPtr != 0))
    	    {
    	    	myPtr -= strlen (UNDERLINE_KEYWORD);
    	    	MDIO_sprintf (myMessage, "Unknown font style '%s'", myPtr);
	    	ABORT_WITH_ERRMSG (E_FONT_UNKNOWN_FONT_STYLE, myMessage);
    	    }
    	}
    	else if (_strnicmp (myPtr, NAA_KEYWORD, strlen (NAA_KEYWORD)) == 0)
    	{
    	    *pmNoAntiAlias = TRUE;
    	    myPtr += strlen (NAA_KEYWORD);
    	    if ((*myPtr != ' ') && (*myPtr != ',') && (*myPtr != 0))
    	    {
    	    	myPtr -= strlen (NAA_KEYWORD);
    	    	MDIO_sprintf (myMessage, "Unknown font style '%s'", myPtr);
	    	ABORT_WITH_ERRMSG (E_FONT_UNKNOWN_FONT_STYLE, myMessage);
    	    }
    	}
    	else if (_strnicmp (myPtr, NO_ANTI_ALIAS_KEYWORD, strlen (NO_ANTI_ALIAS_KEYWORD)) == 0)
    	{
    	    *pmNoAntiAlias = TRUE;
    	    myPtr += strlen (NO_ANTI_ALIAS_KEYWORD);
    	    if ((*myPtr != ' ') && (*myPtr != ',') && (*myPtr != 0))
    	    {
    	    	myPtr -= strlen (NAA_KEYWORD);
    	    	MDIO_sprintf (myMessage, "Unknown font style '%s'", myPtr);
	    	ABORT_WITH_ERRMSG (E_FONT_UNKNOWN_FONT_STYLE, myMessage);
    	    }
    	}
    	else
    	{
    	    MDIO_sprintf (myMessage, "Unknown font style '%s'", myPtr);
	    ABORT_WITH_ERRMSG (E_FONT_UNKNOWN_FONT_STYLE, myMessage);
	}
    }
} // MyGetFontAttrib