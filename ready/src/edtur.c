/***********/
/* edtur.c */
/***********/

/*******************/
/* System includes */
/*******************/
#include <windows.h>
#include <htmlhelp.h>

/****************/
/* Self include */
/****************/
#include "edtur.h"

/******************/
/* Other includes */
/******************/
#include "edfail.h"
#include "edfile.h"
#include "edglob.h"
#include "edgui.h"
#include "edprog.h"
#include "edtext.h"
#include "edwin.h"

#include "eddialogs.h"

/**********/
/* Macros */
/**********/
#define ISIDENT(x) ((('a' <= x) && (x <= 'z')) || \
		    (('A' <= x) && (x <= 'Z')) || \
		    (('0' <= x) && (x <= '9')) || \
		    (x == '.') || (x == '_'))
			
/*************/
/* Constants */
/*************/
#define INITIAL_TEXT_BUFFER_SIZE	4000

#define SUBPROG_DECL_TABLE_SIZE		199

// NRM = Normal, DQT = Double Quote, SQT = Single Quote, PCT = Percent
// SLS = Slash, WSP = White Space (space, newline)
// IDS = Start of a Turing Identifier, IDC = Continuation of a Turing Identifier
// ERR = A character that should never appear in the editor
#define CH_NRM				0
#define CH_DQT				1
#define CH_SQT				2
#define CH_PCT				3
#define CH_SLS				5
#define CH_OPA				7
#define CH_CPA				8
#define CH_OBR				9
#define CH_CBR				10
#define CH_WSP				11
#define CH_IDS				12
#define CH_IDC				13
#define CH_ERR				14

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
static BYTE	stTuringCharTags [] = 
    {CH_ERR, CH_ERR, CH_ERR, CH_ERR, CH_ERR, CH_ERR, CH_ERR, CH_ERR, //   0-  7
     CH_ERR, CH_ERR, CH_WSP, CH_ERR, CH_ERR, CH_ERR, CH_ERR, CH_ERR, //   8- 15
     CH_ERR, CH_ERR, CH_ERR, CH_ERR, CH_ERR, CH_ERR, CH_ERR, CH_ERR, //  16- 23
     CH_ERR, CH_ERR, CH_ERR, CH_ERR, CH_ERR, CH_ERR, CH_ERR, CH_ERR, //  24- 31
     CH_WSP, CH_NRM, CH_DQT, CH_NRM, CH_NRM, CH_PCT, CH_NRM, CH_SQT, //  32- 39
     CH_OPA, CH_CPA, CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_SLS, //  40- 47
     CH_IDC, CH_IDC, CH_IDC, CH_IDC, CH_IDC, CH_IDC, CH_IDC, CH_IDC, //  48- 55
     CH_IDC, CH_IDC, CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_NRM, //  56- 63
     CH_NRM, CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, //  64- 71
     CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, //  72- 79
     CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, //  80- 87
     CH_IDS, CH_IDS, CH_IDS, CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_IDS, //  88- 95
     CH_NRM, CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, //  96-103
     CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, // 104-111
     CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, // 112-119
     // Differs from Java and Turing (Java identifes open/close brace)
     CH_IDS, CH_IDS, CH_IDS, CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_ERR, // 120-127

     CH_NRM, CH_ERR, CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_NRM, // 128-135
     CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_ERR, CH_NRM, CH_ERR, // 136-143
     CH_ERR, CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_NRM, // 144-151
     CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_ERR, CH_NRM, CH_NRM, // 152-159
     CH_ERR, CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_NRM, // 160-167
     CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_NRM, // 168-175
     CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_NRM, // 176-183
     CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_NRM, // 184-191
     CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_NRM, // 192-199
     CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_NRM, // 200-207
     CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_NRM, // 208-215
     CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_NRM, // 216-223
     CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_NRM, // 224-231
     CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_NRM, // 232-239
     CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_NRM, // 240-247
     CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_NRM};// 248-255

/********************/
/* Static variables */
/********************/
// Properties of the Indent class
static BOOL	stPropertiesChanged;

static char	*stTuringSubprogDecls[] = 
    {"class", "fcn", "function", "module", "monitor",
     "proc", "procedure", "process"};
static int	stNumTuringSubprogDecls = 
        			sizeof (stTuringSubprogDecls) / sizeof (char *);
// Tables used by the Text class
static char	**stTuringSubprogDeclTable;

// The output buffer
static char	*stResultBuffer;

// Pointers to the input and output buffer
static char	*stBufPtr, *stEndBufPtr;
static char	*stResultBufPtr, *stResultEndBufPtr;

// The size of the output buffer
static int	stResultBufferSize;

// The window for status messages
static HWND	stStatusWindow;

// Flag to indicate if indent failed
static BOOL	stIndentSuccess;

// Lines of help read in from Keyword Help document
static char	*stKeyword [1000], *stKeywordLine [1000];
static int	stKeywordCount = 0;

/******************************/
/* Static callback procedures */
/******************************/

/*********************/
/* Static procedures */
/*********************/
static BOOL	MyGetSelectedIdentifier (TextPtr pmTextPtr, char *pmIdentifier, 
					 int pmIdentLen);
static void	MyGetType1ProgramName (char *pmText, int pmTextLen, 
				       char *pmProgramName);
static void	MyGetType2ProgramName (char *pmText, int pmTextLen, 
				       char *pmProgramName);
static int	MyHash (char *pmString, int pmTableSize);
static void	MyIndentBuffer (void);
static void	MyInitializeIndent (void);
static BOOL	MyIsTuringSubprogramDeclaration (char *pmId);


/***********************/
/* External procedures */
/***********************/
extern void Paragrapher_SetKeepCommentColumn (int pmTrueFalse);

/************************************************************************/
/* EdTur_Init								*/
/************************************************************************/
BOOL	EdTur_Init (void)
{
    FilePath	myKeywordLookupFile;
    char	*myKeywordLookupBuffer;
    char	*myLine, *myPtr;
    int		myFileSize;
    int		myHash;
    int		cnt;
    
    stTuringSubprogDeclTable = (char **) calloc (SUBPROG_DECL_TABLE_SIZE, 
        sizeof (char *));
    if (stTuringSubprogDeclTable == NULL)
    {
    	EdFail_Warn (IDS_OUTOFMEMORY, __FILE__, __LINE__, 0);
    	return FALSE;
    }

    for (cnt = 0 ; cnt < stNumTuringSubprogDecls ; cnt++)
    {
    	myHash = MyHash (stTuringSubprogDecls [cnt], SUBPROG_DECL_TABLE_SIZE);
    	while (stTuringSubprogDeclTable [myHash] != NULL)
    	{
    	    myHash = (myHash + 1) % SUBPROG_DECL_TABLE_SIZE;
    	}
    	stTuringSubprogDeclTable [myHash] = stTuringSubprogDecls [cnt];
    }
    
    //
    // Load the Turing help files
    //
    EdFile_ConstructPath (myKeywordLookupFile, IDS_HELP_DIR,
			  KEYWORD_LOOKUP);
    myKeywordLookupBuffer = EdFile_ReadFileIntoBuffer (NULL, 
			        myKeywordLookupFile, &myFileSize, 
				OK_IF_NOT_FOUND);
    if (myKeywordLookupBuffer == NULL)
    {
    	return TRUE;
    }

    // The files are stored in order from most recent to least    
    EdFile_ReadLineFromBuffer (myKeywordLookupBuffer, myFileSize, READ_FIRST);

    // Read a line from the file
    myLine = EdFile_ReadLineFromBuffer (myKeywordLookupBuffer, 0, READ_NEXT);
    while (myLine != NULL)
    {
	// Put the keyword and the line into the two arrays
	myPtr = strtok (myLine, " ");
	stKeyword [stKeywordCount] = EdGlob_MallocString (myPtr);
	myPtr = strtok (NULL, "\n");
	if (myPtr == NULL)
	{
	    stKeywordLine [stKeywordCount++] = NULL;
	}
	else
	{
	    stKeywordLine [stKeywordCount++] = EdGlob_MallocString (myPtr);
	}

	myLine = EdFile_ReadLineFromBuffer (myKeywordLookupBuffer, 0, 
					    READ_NEXT);
    }

    free (myKeywordLookupBuffer);

    return TRUE;
} // EdTur_Init


/************************************************************************/
/* EdTur_Finalize							*/
/************************************************************************/
void	EdTur_Finalize (void)
{
    int		cnt;
    
    if (stTuringSubprogDeclTable != NULL)
    {
    	free (stTuringSubprogDeclTable);
    }
    
    // Free up the online help
    for (cnt = 0 ; cnt < stKeywordCount ; cnt++)
    {
    	if (stKeyword [cnt] != NULL) free (stKeyword [cnt]);
    	if (stKeywordLine [cnt] != NULL) free (stKeywordLine [cnt]);
    }
} // EdTur_Finalize


/************************************************************************/
/* EdTur_PropertiesSet							*/
/************************************************************************/
void	EdTur_PropertiesSet (void)
{
    stPropertiesChanged = TRUE;
} // EdFile_PropertiesSet

	
/************************************************************************/
/* EdTur_PropertiesImplementChanges					*/
/************************************************************************/
void	EdTur_PropertiesImplementChanges (void)
{
    stPropertiesChanged = FALSE;
} // EdTur_PropertiesImplementChanges


/************************************************************************/
/* EdTur_GetProgramName							*/
/*									*/
/* This looks for a program name by checking the beginning of the file	*/
/* and looking for the line:						*/
/*		% The "program name" program[. other stuff]		*/
/*		% The programname program[. other stuff]		*/
/*		% "program name" program[. other stuff]			*/
/*		% programname program[. other stuff]			*/
/*									*/
/* It returns "program name" in pmProgramName if it finds it.  It will	*/
/* only look at the beginning of the program.				*/
/************************************************************************/
void	EdTur_GetProgramName (char *pmText, int pmTextLen, char *pmProgramName)
{
    char	*myStart = pmText;

    *pmProgramName = 0;
    
    // Find first non-whitespace char
    while ((pmText - myStart < pmTextLen) &&
    	   ((*pmText == ' ') || (*pmText == '\t') || (*pmText == '\n')))
    {
    	pmText++;
    }
    
    pmTextLen -= pmText - myStart;

    // If there's not enough text left in the file to fit a program
    // name, don't bother trying to find it.
    if (pmTextLen <= 5)
    {
    	return;
    }

    // We're either looking for "
    //      (1) % The "programname" program.
    //      (2) unit module programname
    if (*pmText == '%')
    {
	MyGetType1ProgramName (pmText, pmTextLen, pmProgramName);
    }
    else if (strncmp (pmText, "unit", 4) == 0)
    {
	MyGetType2ProgramName (pmText, pmTextLen, pmProgramName);
    }
} // EdTur_GetProgramName

			    		      
// These routines are from Turing paragrapher (paraoot) and library (tlib.obj)
extern void Paragrapher_Para (char **pmTextHandle, int *pmBufferSize);
extern void TL (void);

/************************************************************************/
/* EdTur_Indent								*/
/************************************************************************/
void	EdTur_Indent (HWND pmWindowForStatus, char *pmText, int pmTextLen, 
		      char **pmNewBuffer, int *pmNewBufferLen, 
		      int *pmNewTextLen)
{
    char	*myText;
    static BOOL	myStLibraryNotInitialized = TRUE;
    static BOOL	myStParagrapherNotInitialized = TRUE;

    // Have the paragrapher keep comments in the appropriate column
    if (myStParagrapherNotInitialized)
    {
    	myStParagrapherNotInitialized = FALSE;
	Paragrapher_SetKeepCommentColumn (TRUE);
    }
    

    //
    // Initialize the library if (1) this is not Turing (Turing has already
    // initialized the library, and (2) the library hasn't already been 
    // initialized
    //
    if ((!gProgram.isTuring) && myStLibraryNotInitialized)
    {
    	myStLibraryNotInitialized = FALSE;
    	TL ();
    }
    
    //
    // The Turing paragrapher expects the original buffer and the original
    // buffer length.  It returns a pointer to the paragraphed text (the
    // original is still there, and the size of the buffer allocated.
    //
    myText = pmText;
    myText [pmTextLen] = 0;

    Paragrapher_Para (&myText, &pmTextLen);
    
    if (myText == NULL)
    {
    	EdGUI_Message1 (pmWindowForStatus, 0, IDS_INDENT_FAILED_TITLE,
    	    IDS_INDENT_OUT_OF_MEM);

    	*pmNewBuffer = NULL;
    	*pmNewBufferLen = 0;
    	*pmNewTextLen = 0;
    	return;
    }
    *pmNewBuffer = myText;
    *pmNewBufferLen = pmTextLen;
    *pmNewTextLen = strchr (myText, 0) - myText;
} // EdTur_Indent

		      
/************************************************************************/
/* EdTur_KeywordDisplayPage						*/
/************************************************************************/
void	EdTur_KeywordDisplayPage (HWND pmEditWindow, TextPtr pmTextPtr)
{
    BOOL	myGotKeyword, myFound = FALSE;
    char	myKeyword [4096], myMessage [4096];
    HH_AKLINK	myAKLink;
    FilePath	myHelpPath;
    int		myPos;

    myGotKeyword = MyGetSelectedIdentifier (pmTextPtr, myKeyword, 
    					    sizeof (myKeyword));
    		       
    if (!myGotKeyword)
    {
    	EdWin_ShowStatus (pmEditWindow, "Selection is not an identifier");
    	return;
    } 
    
    for (myPos = 0 ; myPos < stKeywordCount ; myPos++)
    {
	if (strcmp (stKeyword [myPos], myKeyword) == 0) 
	{
	    myFound = TRUE;
	    break;
	} // if
    } // for
    
    if (!myFound)
    {
    	EdGUI_Message1 (pmEditWindow, 0, IDS_CANT_FIND_KEYWORD_TITLE,
    	    IDS_CANT_FIND_KEYWORD, myKeyword);
    	return;
    }
    
    EdWin_ShowStatus (pmEditWindow, "Loading help for '%s'", myKeyword);
    
    EdFile_ConstructPath (myHelpPath, IDS_HELP_DIR, IDS_TURING_HELP_FILE);
    
    wsprintf (myMessage, "Unable to find keyword '%s' in Help\n"
    			 "Press F10 for general Turing help", myKeyword);

    myAKLink.cbStruct = sizeof (HH_AKLINK);
    myAKLink.fReserved = 0;
    myAKLink.pszKeywords = myKeyword;
    myAKLink.pszUrl = NULL;
    myAKLink.pszMsgText = myMessage;
    myAKLink.pszMsgTitle = "Keyword Not Found";
    myAKLink.pszWindow = NULL;
    myAKLink.fIndexOnFail = FALSE;
    
    HtmlHelp (gProgram.globalWindow, myHelpPath, HH_DISPLAY_INDEX,
              (DWORD) myKeyword);
    HtmlHelp (gProgram.globalWindow, myHelpPath, HH_ALINK_LOOKUP, 
    	      (DWORD) &myAKLink);
} // EdTur_KeywordDisplayPage

   		       
/************************************************************************/
/* EdTur_KeywordLookup							*/
/************************************************************************/
void	EdTur_KeywordLookup (HWND pmEditWindow, TextPtr pmTextPtr)
{
    BOOL	myGotKeyword;
    char	myKeyword [1024];
    int		myPos;
    
    myGotKeyword = MyGetSelectedIdentifier (pmTextPtr, myKeyword, 
    					    sizeof (myKeyword));
    		       
    if (!myGotKeyword)
    {
    	EdWin_ShowStatus (pmEditWindow, "Selection is not an identifier");
    	return;
    }    		       

    // Now we try and find the identifier.  We do sequential (ick) lookup
    myPos = 0;
    while (myPos < stKeywordCount)
    {
	if (strcmp (stKeyword [myPos], myKeyword) == 0) 
	{
	    if (stKeywordLine [myPos]) 
	    {
	    	if (strcmp (stKeywordLine [myPos], "[]") == 0)
	    	{
	    	    EdWin_ShowStatus (pmEditWindow, 
	    	    		      "Please consult reference manual (F9)");
	    	}
	    	else if (strcmp (stKeywordLine [myPos], "<>") == 0)
	    	{
	    	    EdWin_ShowStatus (pmEditWindow,
	    	    		      "Please consult reference manual (F9) "
	    	    		      "for predefined module information");
		}	    	    		     
	    	else 
	    	{
	    	    EdWin_ShowStatus (pmEditWindow, stKeywordLine [myPos]);
	    	}
	    }
	    else
	    {
	    	EdWin_ShowStatus (pmEditWindow, 
				  "'%s' not found in keyword list", myKeyword);
	    }
	    
	    return;
	} // if

	myPos++;
    } // while
    
    EdWin_ShowStatus (pmEditWindow, 
    		      "Unable to find '%s' in online keyword lookup",
    		      myKeyword);
} // EdTur_KeywordLookup


/************************************************************************/
/* EdTur_Mark								*/
/************************************************************************/
MarkLinePtr	EdTur_Mark (char *pmText, int pmTextLen)
{
    unsigned char	*myPtr = pmText;
    char		*myEnd = myPtr + pmTextLen - 1;
    char		*myStart;
    int			myLen;
    int			myLine = 1;
    char		myIdentifier [100];
    int			myParenDepth = 0;
    int			myBraceDepth = 0;
    BOOL		myMarkNextID = FALSE;
    MarkLinePtr		myMarkLineHead = NULL;
    MarkLinePtr		myLastMarkLine = NULL;
            
    while (myPtr < myEnd)
    {
    	switch (stTuringCharTags [*myPtr])
    	{
	    case CH_WSP:
	    	if (*myPtr == '\n')
	    	{
	    	    myLine++;
	    	}
	        myPtr++;
	        break;
	    case CH_OPA:
	        myParenDepth++;
    		myMarkNextID = FALSE;
    	        myPtr++;
    	        break;
	    case CH_CPA:
	        myParenDepth = max (0, myParenDepth - 1);
    		myMarkNextID = FALSE;
    	        myPtr++;
    	        break;
	    case CH_OBR:
	        myBraceDepth++;
    		myMarkNextID = FALSE;
    	        myPtr++;
    	        break;
	    case CH_CBR:
	        myBraceDepth = max (0, myBraceDepth - 1);
    		myMarkNextID = FALSE;
    	        myPtr++;
    	        break;
    	    case CH_NRM: // normal char
    	    case CH_IDC: // id continue
    		myMarkNextID = FALSE;
    	        myPtr++;
    	        break;
    	    case CH_DQT: // "
    		myMarkNextID = FALSE;
    	        myPtr++;
    	        while ((*myPtr != '\n') && (*myPtr != '"'))
    	        {
    	            if (*myPtr == '\\')
    	            {
    	            	myPtr++;
    	            }
    	            myPtr++;
    	        }
    	        if (*myPtr == '"')
    	        {
    	            myPtr++;
    	        }
    	        break;
    	    case CH_SQT: // '
    		myMarkNextID = FALSE;
    	        myPtr++;
    	        while ((*myPtr != '\n') && (*myPtr != '\''))
    	        {
    	            if (*myPtr == '\\')
    	            {
    	            	myPtr++;
    	            }
    	            myPtr++;
    	        }
    	        if (*myPtr == '\'')
    	        {
    	            myPtr++;
    	        }
    	        break;
    	    case CH_PCT: // %
    	        do
    	        {
     	            myPtr++;
     	        } while (*myPtr != '\n');
    	        break;
    	    case CH_IDS: // id start
    	        myStart = myPtr;
    	        while ((stTuringCharTags [*myPtr] == CH_IDS) || 
    	            (stTuringCharTags [*myPtr] == CH_IDC))
    	        {
    	            myPtr++;
    	        }
    	        myLen = myPtr - myStart;
    	        
    	        // If the identifier is too long, it's not a keyword or predef
    	        if (myLen < 50)
    	        {
    	            strncpy (myIdentifier, myStart, myLen);
    	            myIdentifier [myLen] = 0;
    	            
    	            // If we reached an "end", set the parenthesis depth to
    	            // zero.  This kludge helps stop one missed close paren
    	            // from eliminating all following marks.
    	            if (strcmp (myIdentifier, "end") == 0)
    	            {
    	            	myParenDepth = 0;
    	            }	
    	            // If the identifier is the start of a subprogram 
    	            // declaration, make the following identifier in the
    	            // mark list.
    	            else if (MyIsTuringSubprogramDeclaration (myIdentifier))
    	            {
    	            	myMarkNextID = TRUE;
    	            }
    	            // If the preceding identifier was the start of a subprogram
    	            // declaration, and we aren't in parentheses (= we are not
    	            // a parameter), then add the identifier to the mark list.
    	            else if (myMarkNextID && (myParenDepth == 0))
    	            {
    	            	MarkLinePtr	myMarkLine;
			
			myMarkLine = malloc (sizeof (MarkLine));
			myMarkLine -> line = myLine;
			myMarkLine -> name = malloc (strlen (myIdentifier) + 1);
			// TW Check for NULL
			strcpy (myMarkLine -> name, myIdentifier);
			myMarkLine -> next = NULL;
			if (myLastMarkLine == NULL)
			    myMarkLineHead = myMarkLine;
			else
			    myLastMarkLine -> next = myMarkLine;
			myLastMarkLine = myMarkLine;
    	            	myMarkNextID = FALSE;
    	            }
    	        }
    	        break;
    	    case CH_SLS: // /
    	        if (*(myPtr + 1) == '*')
    	        {
    	            myPtr += 2;
    	            while (myPtr < myEnd) 
    	            {
    	            	if ((*myPtr == '*') && (*(myPtr + 1) == '/'))
    	            	{
	    	            myPtr += 2;
	    	            break;
	    	        }
	    	        myPtr++;
	    	    } // while
     	        }
    	        else
    	        {
     	            myPtr++;
    	        }
    	        break;
    	    case CH_ERR: // error
    	        EdFail_Fatal (IDS_UNEXPECTED_CHAR, __FILE__, __LINE__, 0,
    	            *myPtr);
    	        break;
    	} // switch
    } // while
    
    return myMarkLineHead;
} // EdTur_Mark


/******************************/
/* Static callback procedures */
/******************************/


/*********************/
/* Static procedures */
/*********************/
/************************************************************************/
/* MyGetSelectedIdentifier						*/
/************************************************************************/
static BOOL	MyGetSelectedIdentifier (TextPtr pmTextPtr, char *pmIdentifier, 
					 int pmIdentLen)
{					 
    int		myFrom, myTo, myPos;
            
    // Get the selection, if possible
    myFrom = min (pmTextPtr -> selFrom, pmTextPtr -> selTo);
    myTo = max (pmTextPtr -> selFrom, pmTextPtr -> selTo);
    
    // The algorithm is:
    //    if at a point, then if in middle of identifier, then expand to fill
    //        identifier.  if not in the middle of an identifier, go backward
    //        through spaces and open brackets until you find an identifier.
    //        if no identifier, then go forward over spaces until you find
    //        and identifier.
    //    if selection is text, then it must be all identifier (leading and
    //        trailing non-identifier excepted).  Expand in either (if 
    //        necessary) direction to fit the entire identifier.
    //    
    if (myFrom == myTo)
    {
    	// Are we in the middle of an identifier?
    	if (ISIDENT (pmTextPtr -> text [myFrom - 1]) &&
    	    ISIDENT (pmTextPtr -> text [myTo]))
        {    	    
    	    // The selection is a point, expand it in both directions
    	    while (myFrom > 0)
    	    {
    	        if (!ISIDENT (pmTextPtr -> text [myFrom - 1]))
    	        {
    	    	    break;
    	        }
    	    	myFrom--;
    	    }
    	    while (myTo < pmTextPtr -> textLen)
    	    {
    	    	if (!ISIDENT (pmTextPtr -> text [myTo]))
    	    	{
    	    	    break;
    	    	}
    	    	myTo++;
    	    }
    	}
    	else
    	{
    	    myPos = myFrom;
    	    while ((myPos > 0) && 
    	    	   ((pmTextPtr -> text [myPos - 1] == ' ') ||
    	    	    (pmTextPtr -> text [myPos - 1] == '(')))
	    {
	    	myPos--;
	    }
	    if ((myPos > 0) && ISIDENT (pmTextPtr -> text [myPos - 1]))
	    {
	    	myFrom = myTo = myPos;
	    	 
	    	while ((myFrom > 0) && ISIDENT (pmTextPtr -> text [myFrom - 1]))
	    	{
	    	    myFrom--;
	    	}
	    }
	    else
	    {
	    	// There wasn't any suitable identifier before the caret,
	    	// so let's look after it.
    	        while ((myTo < pmTextPtr -> textLen) && 
    	               (pmTextPtr -> text [myTo] == ' '))
	    	{
	    	    myTo++;
	    	}
	    	if ((myTo < pmTextPtr -> textLen) && 
	    	    ISIDENT (pmTextPtr -> text [myTo]))
	    	{
	    	    myFrom = myTo;
	    	 
	    	    while ((myTo < pmTextPtr -> textLen) && 
	    	    	   ISIDENT (pmTextPtr -> text [myTo]))
	    	    {
	    	    	myTo++;
	    	    }
	    	}
	    	else
	    	{
	    	    return FALSE;
	    	}
	    }
    	}
    }
    else
    {
    	// Trim the whitespace at the beginning and the end
    	while (myFrom < myTo)
    	{
    	    if (ISIDENT (pmTextPtr -> text [myFrom]))
    	    {
    	    	break;
    	    }
    	    myFrom++;
    	}
    	while (myTo > myFrom)
    	{
    	    if (ISIDENT (pmTextPtr -> text [myTo - 1]))
    	    {
    	    	break;
    	    }
    	    myTo--;
    	}
    	
    	// Hopefully, we know have a keyword that is all idents chars
    	// Now we check to make certain that this is so.
    	if (myFrom == myTo)
    	{
    	    return FALSE;
    	}
    	
    	myPos = myFrom;
    	while (myPos < myTo)
    	{
    	    if (!ISIDENT (pmTextPtr -> text [myPos]))
    	    {
    	    	return FALSE;
    	    }
    	    myPos++;
    	}
    	
    	// The selection is a some chars, expand it in both directions
    	// if the beginning/end of the selection is a char.
    	if (ISIDENT (pmTextPtr -> text [myFrom]))
    	{
    	    while (myFrom > 0)
    	    {
    	    	if (!ISIDENT (pmTextPtr -> text [myFrom - 1]))
    	    	{
    	    	    break;
    	    	}
    	    	myFrom--;
    	    }
    	}
    	if (ISIDENT (pmTextPtr -> text [myTo - 1]))
    	{
    	    while (myTo < pmTextPtr -> textLen)
    	    {
    	    	if (!ISIDENT (pmTextPtr -> text [myTo]))
    	    	{
    	    	    break;
    	    	}
    	    	myTo++;
    	    }
    	}
    }
    
    // Copy the keyword into the buffer
    myPos = myFrom;
    while ((myPos - myFrom < pmIdentLen - 3) && (myPos < myTo))
    {
    	pmIdentifier [myPos - myFrom] = pmTextPtr -> text [myPos];
    	myPos++;
    }
    pmIdentifier [myPos - myFrom] = 0;
    
    return TRUE;
} // MyGetSelectedIdentifier


/************************************************************************/
/* MyGetType1ProgramName						*/
/*									*/
/* This gets called if the first non-whitespace char is a '%'.  pmText	*/
/* points to the '%', pmTextLen is the length from the '%' onward.	*/
/* It looks for a program name by checking the beginning of the file	*/
/* and looking for the line:						*/
/*		% The "program name" program[. other stuff]		*/
/*		% The programname program[. other stuff]		*/
/*		% "program name" program[. other stuff]			*/
/*		% programname program[. other stuff]			*/
/*									*/
/* It returns "program name" in pmProgramName if it finds it.  It will	*/
/* only look at the beginning of the program.				*/
/************************************************************************/
static void	MyGetType1ProgramName (char *pmText, int pmTextLen, 
				       char *pmProgramName)
{
    char	*myStart = pmText;
    
    pmText++;
    	
    // Skip in-line whitespace (we know that we'll have a newline before 
    // running out of text.
    while ((*pmText == ' ') || (*pmText == '\t'))
    {
    	pmText++;
    }

    // If next word is "the ", then skip
    if (pmText - myStart < pmTextLen - 6)
    {
    	if (_strnicmp (pmText, "the ", 4) == 0)
    	{
    	    pmText += 4;
    	}
    	
    	// Skip in-line whitespace
    	while ((*pmText == ' ') || (*pmText == '\t'))
    	{
    	    pmText++;
    	}
    } // if
    
    // The next character is either a double quote or a alphanumeric
    if (*pmText == '"')
    {
    	char *myQuoteStart = pmText + 1;
    	
    	pmText++;
    	while ((*pmText != '"') && (*pmText != '\n') && 
    	       (pmText - myQuoteStart < 50))
	{
	    pmProgramName [pmText - myQuoteStart] = *pmText;
	    pmText++;
	}
	if (*pmText == '"')
	{
	    pmProgramName [pmText - myQuoteStart] = 0;
	    pmText++;
	}
	else
	{
	    *pmProgramName = 0;
	    return;
	}
    }
    else if (isalnum (*pmText))
    {
    	char *myQuoteStart = pmText;
    	
    	pmText++;
    	while ((*pmText != ' ') && (*pmText != '\n') && 
    	       (pmText - myQuoteStart < 50))
	{
	    pmProgramName [pmText - myQuoteStart] = *pmText;
	    pmText++;
	}
	if (*pmText == ' ')
	{
	    pmProgramName [pmText - myQuoteStart] = 0;
	    pmText++;
	}
	else
	{
	    *pmProgramName = 0;
	    return;
	}
    }
    else
    {
    	return;
    }
    
    // Skip in-line whitespace
    while ((*pmText == ' ') || (*pmText == '\t'))
    {
    	pmText++;
    }
    
    // The next character is either a double quote or a alphanumeric
    if (pmText - myStart >= pmTextLen - 9)
    {
        *pmProgramName = 0;
    	return;
    }
    
    if (_strnicmp (pmText, "program", 7) != 0)
    {
        *pmProgramName = 0;
    	return;
    }
    pmText += 7;
    
    // Skip in-line whitespace
    while ((*pmText == ' ') || (*pmText == '\t'))
    {
    	pmText++;
    }
    
    // We now expect either a period or a newline
    if ((*pmText != '\n') && (*pmText != '.'))
    {
        *pmProgramName = 0;
    }
} // MyGetType1ProgramName


/************************************************************************/
/* MyGetType2ProgramName						*/
/*									*/
/* This gets called if the first non-whitespace char is a '%'.  pmText	*/
/* points to the '%', pmTextLen is the length from the '%' onward.	*/
/* It looks for a program name by checking the beginning of the file	*/
/* and looking for the line(s):						*/
/*		unit module programname					*/
/*									*/
/* It returns "program name.tu" in pmProgramName if it finds it.  It 	*/
/* will only look at the beginning of the program.			*/
/************************************************************************/
static void	MyGetType2ProgramName (char *pmText, int pmTextLen, 
				       char *pmProgramName)
{
    char	*myStart = pmText;
    BOOL	myFound = FALSE;

    pmText += 4;

    // Skip whitespace and comment lines
    while ((pmText - myStart < pmTextLen) &&
    	   ((*pmText == ' ') || (*pmText == '\t') || (*pmText == '\n') ||
	    (*pmText == '%') || (*pmText == '/')))
    {
	myFound = TRUE;
	if (*pmText == '%')
	{
	    // Skip rest of line
	    while (*pmText != '\n')
	    {
		pmText++;
	    }
	}
	else if (*pmText == '/')
	{
	    if (*(pmText + 1) != '*')
	    {
		break;
	    }
	    pmText += 2;
	    // Skip to end of '/* */' comment
	    while (TRUE)
	    {
		while ((pmText - myStart < pmTextLen) && (*pmText != '*'))
		{
		    pmText++;
		}
		if (pmText - myStart >= pmTextLen)
		{
		    return;
		}
		pmText++;
		if (*pmText == '/')
		{
		    // Found closing '*/'
		    break;
		}
	    }
	}
	pmText++;
    }
    if (!myFound) 
    {
	// There must be a comment or whitespace between unit and module
	return;
    }

    // We must now find the word 'class' or 'module'
    if (pmText + 7 - myStart > pmTextLen)
    {
	return;
    }

    if (strncmp (pmText, "class", 5) == 0)
    {
	pmText += 5;
    }
    else if (strncmp (pmText, "module", 6) == 0)
    {
	pmText += 6;
    }
    else
    {
	return;
    }

    // Skip whitespace and comment lines
    myFound = FALSE;
    while ((pmText - myStart < pmTextLen) &&
    	   ((*pmText == ' ') || (*pmText == '\t') || (*pmText == '\n') ||
	    (*pmText == '%') || (*pmText == '/')))
    {
	myFound = TRUE;
	if (*pmText == '%')
	{
	    // Skip rest of line
	    while (*pmText != '\n')
	    {
		pmText++;
	    }
	}
	else if (*pmText == '/')
	{
	    if (*(pmText + 1) != '*')
	    {
		break;
	    }
	    pmText += 2;
	    // Skip to end of '/* */' comment
	    while (TRUE)
	    {
		while ((pmText - myStart < pmTextLen) && (*pmText != '*'))
		{
		    pmText++;
		}
		if (pmText - myStart >= pmTextLen)
		{
		    return;
		}
		pmText++;
		if (*pmText == '/')
		{
		    // Found closing '*/'
		    break;
		}
	    }
	}
	pmText++;
    }
    if (!myFound) 
    {
	// There must be a comment or whitespace between module and name
	return;
    }

    // We should now be at the module/class name
    if (stTuringCharTags [*pmText] != CH_IDS)
    {
	return;
    }
    myStart = pmText;
    while ((stTuringCharTags [*pmText] == CH_IDS) ||
	   (stTuringCharTags [*pmText] == CH_IDC))
    {
	pmText++;
    }
    strncpy (pmProgramName, myStart, pmText - myStart);
    pmProgramName [pmText - myStart] = 0;
    strcat (pmProgramName, ".tu");
} // MyGetType2ProgramName


/************************************************************************/
/* MyHash								*/
/************************************************************************/
static int	MyHash (char *pmString, int pmTableSize)
{
    int	myStart = strlen (pmString);
    int	myEnd = max (0, myStart - 4);
    int	myValue = 0;
    int	cnt;
    
    for (cnt = myStart ; cnt >= myEnd ; cnt--)
    {
    	myValue = myValue * 59 + pmString [cnt];
    }
    return (myValue * 37 + pmString [0]) % pmTableSize;
} // MyHash


/************************************************************************/
/* MyIndentBuffer							*/
/************************************************************************/
static void	MyIndentBuffer (void)
{
#ifdef JUNK	
    while (TRUE)
    {
    	// Skip white space
    	while (*myBufPtr == ' ')
    	{
    	    myBufPtr++;
    	}
#endif    	
} // MyIndentBuffer


/************************************************************************/
/* MyInitializeIndent							*/
/************************************************************************/
static void	MyInitializeIndent (void)
{
} // MyInitializeIndent


/************************************************************************/
/* MyIsTuringSubprogramDeclaration					*/
/************************************************************************/
static BOOL	MyIsTuringSubprogramDeclaration (char *pmId)
{
    int	myHash = MyHash (pmId, SUBPROG_DECL_TABLE_SIZE);
    
    while (stTuringSubprogDeclTable [myHash] != NULL)
    {
    	if (strcmp (pmId, stTuringSubprogDeclTable [myHash]) == 0)
    	    return TRUE;
    	myHash = (myHash + 1) % SUBPROG_DECL_TABLE_SIZE;
    }
    return FALSE;
} // MyIsTuringSubprogramDeclaration
