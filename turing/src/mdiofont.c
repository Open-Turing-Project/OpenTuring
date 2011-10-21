/**************/
/* mdiofont.c */
/**************/

/*******************/
/* System includes */
/*******************/
#include <windows.h>
#include <windowsx.h>

/****************/
/* Self include */
/****************/
#include "mdiofont.h"

/******************/
/* Other includes */
/******************/
#include "miofont.h"

#include "mio.h"

#include "mdio.h"

#include "mioerr.h"

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
typedef struct FontMap
{
    char *oldName;
    char *newName;
} FontMap;

/**********************/
/* External variables */
/**********************/

/********************/
/* Static constants */
/********************/
static FontMap stFontMapping [] = 
{{"Serif", "Times New Roman"}, {"Sans Serif", "Ariel"}, {"SansSerif", "Ariel"},
 {"Sans", "Ariel"}, {"Mono", "Courier New"}, {"Monospace", "Courier New"},
 {"Monospaced", "Courier New"}, {"Times Roman", "Times New Roman"},
 {"TimesRoman", "Times New Roman"}, {"Times", "Times New Roman"},
 {"Helvetica", "Ariel"}, {"Courier", "Courier New"}, {NULL, NULL}};
 
/********************/
/* Static variables */
/********************/
    	

/******************************/
/* Static callback procedures */
/******************************/
static int CALLBACK	MyFontEnumerator (ENUMLOGFONTEX *pmEnumLogFont,
					  NEWTEXTMETRICEX *pmFontAttrib,
					  DWORD pmFontType, LPARAM pmLParam);
static int CALLBACK	MyFontFamEnumerator (ENUMLOGFONTEX *pmEnumLogFont,
					     NEWTEXTMETRICEX *pmFontAttrib,
					     DWORD pmFontType, LPARAM pmLParam);

/*********************/
/* Static procedures */
/*********************/


/***********************/
/* External procedures */
/***********************/
/************************************************************************/
/* MDIOFont_CreateFont							*/
/************************************************************************/
FONT	MDIOFont_CreateFont (const char *pmTypeFace, int pmHeight, int pmWidth, 
			     int pmBold, int pmItalic, int pmUnderline,
			     BOOL pmNoAntiAlias)
{
    LOGFONT	myLogFont;
    HDC		myDisplayContext;
    FontMap	*myFontMap;
    BOOL	myFontMapped;
    HFONT	myFont;
    char	myFontName [1024];
    
    ZeroMemory (&myLogFont, sizeof (LOGFONT));
    myDisplayContext = MIO_selectedRunWindowInfo -> deviceContext;
    if (pmHeight > 0)
    {
    	myLogFont.lfHeight = -MulDiv (pmHeight, 
    	    GetDeviceCaps (myDisplayContext, LOGPIXELSY), 72);
    }
    else
    {    	    
    	myLogFont.lfHeight = pmHeight;
    }
    if (pmWidth > 0)
    {
    	myLogFont.lfWidth = -MulDiv (pmWidth, 
    	    GetDeviceCaps (myDisplayContext, LOGPIXELSX), 72);
    }
    else
    {    	    
    	myLogFont.lfWidth = pmWidth;
    }
    
    if (pmBold)
    {
        myLogFont.lfWeight = 700;
    }
    else
    {
        myLogFont.lfWeight = 400;
    }
    myLogFont.lfItalic = pmItalic;
    myLogFont.lfUnderline = pmUnderline;
    myLogFont.lfEscapement = myLogFont.lfOrientation = 0;
    if (pmNoAntiAlias)
    {
	myLogFont.lfQuality = NONANTIALIASED_QUALITY;
    }

    //
    // Map the type faces as follows:
    //
    // Serif -> Times New Roman
    // Sans Serif -> Ariel
    // SansSerif -> Ariel
    // Sans -> Ariel
    // Mono -> Courier New
    // Monospace -> Courier New
    // Monospaced -> Courier New
    // Times Roman -> Times New Roman
    // TimesRoman -> Times New Roman
    // Times -> Times New Roman
    // Helvetica -> Ariel
    // Courier -> Courier New
    // 
    myFontMap = stFontMapping;
    myFontMapped = FALSE;
    while (myFontMap -> oldName != NULL)
    {
    	if (_stricmp (pmTypeFace, myFontMap -> oldName) == 0)
    	{
	    myFontMapped = TRUE;
	    strcpy (myLogFont.lfFaceName, myFontMap -> newName);
	    break;
	}
	myFontMap++;
    }
    if (!myFontMapped)
    {
    	strcpy (myLogFont.lfFaceName, pmTypeFace);
    }
    
    myFont = CreateFontIndirect (&myLogFont);

    // Get the name of the font we actually selected
    SelectFont (myDisplayContext, myFont);
    GetTextFace (myDisplayContext, 1024, myFontName);

    // If the font name we asked for and the font name we got don't match
    // then warn the user.
    if (_stricmp (myLogFont.lfFaceName, myFontName) != 0)
    {
	MIOError_SetErrMsg (E_FONT_DIDNT_GET_REQUESTED_FONT, 
    		      "Unable to create font '%s'. Font '%s' used instead",
    		      myLogFont.lfFaceName, myFontName);
    }
        	
    return myFont;
} // MDIOFont_CreateFont

			      
/************************************************************************/
/* MDIOFont_EnumerateAllFonts						*/
/************************************************************************/
void	MDIOFont_EnumerateAllFonts (void)
{
    LOGFONT	myLogFont;
    HDC		myDeviceContext;
    
    ZeroMemory (&myLogFont, sizeof (LOGFONT));
    
    myLogFont.lfCharSet = DEFAULT_CHARSET;
    
    myDeviceContext = MIO_selectedRunWindowInfo -> deviceContext;
    if (myDeviceContext == NULL)
    {
	myDeviceContext = GetDC (MIO_selectedRunWindow);
    }

    EnumFontFamiliesEx (myDeviceContext, &myLogFont, 
    			(FONTENUMPROC) MyFontFamEnumerator, 
    			(LPARAM) myDeviceContext, 0);
} // MDIOFont_EnumerateAllFonts


/************************************************************************/
/* MDIOFont_FreeFont							*/
/************************************************************************/
void	MDIOFont_FreeFont (FONT pmMDFontInfo)
{
    DeleteObject (pmMDFontInfo);
} // MDIOFont_FreeFont


/************************************************************************/
/* MDIOFont_GetName							*/
/************************************************************************/
void	MDIOFont_GetName (FONT pmFont, char *pmFontName)
{
    SelectFont (MIO_selectedRunWindowInfo -> deviceContext, pmFont);
    GetTextFace (MIO_selectedRunWindowInfo -> deviceContext, 255, pmFontName);
} // MDIOFont_GetName


/************************************************************************/
/* MDIOFont_GetSizes							*/
/************************************************************************/
void	MDIOFont_GetSizes (FONT pmFont, int *pmHeight, int *pmAscent,
			   int *pmDescent, int *pmIntLeading)
{
    TEXTMETRIC	myTextMetrics;

    SelectFont (MIO_selectedRunWindowInfo -> deviceContext, (HFONT) pmFont);
    GetTextMetrics (MIO_selectedRunWindowInfo -> deviceContext, &myTextMetrics);

    *pmHeight = myTextMetrics.tmHeight;
    *pmAscent = myTextMetrics.tmAscent;
    *pmDescent = myTextMetrics.tmDescent;
    *pmIntLeading = myTextMetrics.tmInternalLeading;
} // MDIOFont_GetSizes


/************************************************************************/
/* MDIOFont_GetWidth							*/
/************************************************************************/
int	MDIOFont_GetWidth (FONT pmFont, char *pmString)
{
    SIZE	mySize;
    
    SelectFont (MIO_selectedRunWindowInfo -> deviceContext, (HFONT) pmFont);
    
    GetTextExtentPoint (MIO_selectedRunWindowInfo -> deviceContext, pmString, 
    		        strlen (pmString), &mySize);

    return mySize.cx;
} // MDIOFont_GetWidth


/******************************/
/* Static callback procedures */
/******************************/
/************************************************************************/
/* MyFontEnumerator							*/
/************************************************************************/
static int CALLBACK	MyFontEnumerator (ENUMLOGFONTEX *pmEnumLogFont,
					  NEWTEXTMETRICEX *pmFontAttrib,
					  DWORD pmFontType, LPARAM pmLParam)
{
    int	myHeight = MulDiv (pmEnumLogFont -> elfLogFont.lfHeight, 72, 
    	    	       	   GetDeviceCaps ((HDC) pmLParam, LOGPIXELSY));

    // MS Windows does not seem to include any fixed height fonts along with
    // their truetype fonts.  It's either truetype or fixed heights.
    if (pmFontType == TRUETYPE_FONTTYPE)
    {
    	myHeight = -1;
    }
    
    MIOFont_AddFont (pmEnumLogFont -> elfLogFont.lfFaceName, 
    		     myHeight,
    		     (pmEnumLogFont -> elfLogFont.lfWeight >= 600),
    		     pmEnumLogFont -> elfLogFont.lfItalic,
    		     pmEnumLogFont -> elfLogFont.lfUnderline);
    		 
    return 1;
} // MyFontEnumerator
					   

/************************************************************************/
/* MyFontFamEnumerator							*/
/************************************************************************/
static int CALLBACK	MyFontFamEnumerator (ENUMLOGFONTEX *pmEnumLogFont,
					     NEWTEXTMETRICEX *pmFontAttrib,
					     DWORD pmFontType, LPARAM pmLParam)
{
    LOGFONT	myLogFont;

    ZeroMemory (&myLogFont, sizeof (LOGFONT));
    strcpy (myLogFont.lfFaceName, pmEnumLogFont -> elfLogFont.lfFaceName);
    myLogFont.lfCharSet = DEFAULT_CHARSET;
    
    EnumFontFamiliesEx ((HDC) pmLParam, &myLogFont, 
    			(FONTENUMPROC) MyFontEnumerator, pmLParam, 0);
    
    return 1;
} // MyFontFamEnumerator
					   

/*********************/
/* Static procedures */
/*********************/


