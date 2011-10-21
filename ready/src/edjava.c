/************/
/* edjava.c */
/************/

/*******************/
/* System includes */
/*******************/
#include <windows.h>
#include <crtdbg.h>

/****************/
/* Self include */
/****************/
#include "edjava.h"

/******************/
/* Other includes */
/******************/
#include "edfail.h"
#include "edgui.h"
#include "edprog.h"
#include "edwin.h"

#include "eddialogs.h"

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/
#define ECHO_NEWLINE			TRUE
#define DONT_ECHO_NEWLINE		FALSE

#define DONT_ECHO_COMMENTS		0
#define ECHO_COMMENTS			1
#define ECHO_COMMENTS_KEEP_LOCATION	2

#define INITIAL_TEXT_BUFFER_SIZE	4000
#define MAX_FILE_SIZE			10000000;
#define BUFFER_ADJUSTMENT		10000;
 	
#define TOKEN_TABLE_SIZE		250

#define INIT_MAX_TOKEN_SIZE		4096

#define EOS				0
#define EOF_MARKER			0

// Parameters for MyParseBuffer
#define GET_CLASS			1
#define MARK_METHODS			2
#define GET_PROGRAM_TYPE		3

#define SHORT_APPLET			"Applet"
#define LONG_APPLET			"java.applet.Applet"
#define SHORT_JAPPLET			"JApplet"
#define LONG_JAPPLET			"javax.swing.JApplet"
#define SHORT_STRING			"String"
#define LONG_STRING			"java.lang.String"

/********************/
/* Global variables */
/********************/

/*********/
/* Types */
/*********/
// All the possible input tokens
typedef enum 
{
    TOK_NONE, TOK_NEWLINE, TOK_SEMICOLON, TOK_COLON, TOK_PERIOD,
    TOK_COMMA, TOK_OPEN_BRACE, TOK_CLOSE_BRACE, TOK_OPEN_PAREN,
    TOK_CLOSE_PAREN, TOK_OPEN_CLOSE_BRACKET, TOK_CASE, TOK_FOR, TOK_WHILE, TOK_DO, TOK_IF,
    TOK_ELSE, TOK_SWITCH, TOK_PRE_AND_SUF_OPERATOR, TOK_PREFIX_OPERATOR,
    TOK_OPERATOR, TOK_IDENTIFIER, TOK_NUMBER, TOK_STRING_LITERAL,
    TOK_ONE_LINE_COMMENT, TOK_LONG_COMMENT, TOK_EOF
} Token;

// The following are the different types of (reasons for) indents
// 	BLOCK_INDENT is for if, else, while, do, for
// 	DO_INDENT doesn't actually indent. It specifies when a DO statement 
// 		has been called and is waiting for a while statement.
// 	IF_INDENT doesn't actually indent. It specifies when a IF statement 
// 		has been called and may be waiting for an else statement.
// 	ONE_BLOCK_INDENT is used to remove a single BLOCK_INDENT.
//		It never actually appears in the indent stack.
// 		
typedef enum 
{
    BRACE_INDENT, PAREN_INDENT, BRACKET_INDENT, IN_STATEMENT_INDENT,
    SWITCH_INDENT, BLOCK_INDENT, DO_INDENT, IF_INDENT, ONE_BLOCK_INDENT,
    INITIALIZER_INDENT
} Indent;

typedef struct IndentStack
{
    Indent		indent;
    struct IndentStack	*next;
} IndentStack, *IndentStackPtr;

typedef struct
{
    char	*token;
    int		tokenValue;
} PredefToken;

typedef struct
{
    char	*className;
    char	*package;
} ClassPackage;

/**********************/
/* External variables */
/**********************/

/********************/
/* Static constants */
/********************/
static PredefToken	stPredefTokens [] = 
    {{"\n", TOK_NEWLINE},
     {";", TOK_SEMICOLON},
     {":", TOK_COLON},
     {".", TOK_PERIOD},
     {",", TOK_COMMA},
     {"{", TOK_OPEN_BRACE},
     {"}", TOK_CLOSE_BRACE},
     {"(", TOK_OPEN_PAREN},
     {"[", TOK_OPEN_PAREN},
     {")", TOK_CLOSE_PAREN},
     {"]", TOK_CLOSE_PAREN},
     {"case", TOK_CASE},
     {"default", TOK_CASE},
     {"for", TOK_FOR},
     {"while", TOK_WHILE},
     {"do", TOK_DO},
     {"if", TOK_IF},
     {"else", TOK_ELSE},
     {"switch", TOK_SWITCH},
     {"++", TOK_PRE_AND_SUF_OPERATOR},
     {"--", TOK_PRE_AND_SUF_OPERATOR},
     {"[]", TOK_OPEN_CLOSE_BRACKET},
     {"-", TOK_PREFIX_OPERATOR},
     {"+", TOK_PREFIX_OPERATOR},
     {"!", TOK_PREFIX_OPERATOR},
     {"~", TOK_PREFIX_OPERATOR},
     {"*", TOK_OPERATOR},
     {"/", TOK_OPERATOR},
     {"%", TOK_OPERATOR},
     {"<<", TOK_OPERATOR},
     {">>", TOK_OPERATOR},
     {">>>", TOK_OPERATOR},
     {"<", TOK_OPERATOR},
     {">", TOK_OPERATOR},
     {">=", TOK_OPERATOR},
     {"<=", TOK_OPERATOR},
     {"==", TOK_OPERATOR},
     {"!=", TOK_OPERATOR},
     {"&", TOK_OPERATOR},
     {"^", TOK_OPERATOR},
     {"|", TOK_OPERATOR},
     {"&&", TOK_OPERATOR},
     {"||", TOK_OPERATOR},
     {"?", TOK_OPERATOR},
     {"=", TOK_OPERATOR},
     {"+=", TOK_OPERATOR},
     {"-=", TOK_OPERATOR},
     {"*=", TOK_OPERATOR},
     {"/=", TOK_OPERATOR},
     {"%=", TOK_OPERATOR},
     {">>=", TOK_OPERATOR},
     {"<<=", TOK_OPERATOR},
     {">>>=", TOK_OPERATOR},
     {"&=", TOK_OPERATOR},
     {"^=", TOK_OPERATOR},
     {"|=", TOK_OPERATOR}};

static char	*stClassNameInJavaLang [] = 
    {"Boolean", "Byte", "Character", "Class", "Comparable", "Compiler", 
     "Double", "Float", "Integer", "Long", "Math", "Number", "Object",
     "Package", "Process", "Short", "String", "System", "Thread", "Void", 
     "Exception", "Error"};
     
static int stNumClassNamesInJavaLang = 
    sizeof (stClassNameInJavaLang) / sizeof (char *);

static ClassPackage	stClassPackageName [] = {
    // java class library
    {"Applet", "java.applet"},
    {"ArrayList", "java.util"},
    {"Button", "java.awt"},
    {"Calendar", "java.util"},
    {"Canvas", "java.awt"},
    {"Checkbox", "java.awt"},
    {"Color", "java.awt"},
    {"Component", "java.awt"},
    {"Date", "java.util.Date"},
    {"Dialog", "java.awt.Dialog"},
    {"Dimension", "java.awt.Dimension"},
    {"Event", "java.awt.Event"},
    {"Font", "java.awt.Font"},
    {"Frame", "java.awt.Frame"},
    {"Graphics", "java.awt.Graphics"},
    {"Image", "java.awt.Image"},
    {"Iterator", "java.util.Iterator"},
    {"Label", "java.awt.Label"},
    {"List", "java.awt.List or java.util.List"},
    {"Locale", "java.util.Locale"},
    {"Map", "java.util.Map"},
    {"Menu", "java.awt.Menu"},
    {"Panel", "java.awt.Panel"},
    {"Point", "java.awt.Point"},
    {"Random", "java.util.Random"},
    {"Set", "java.util.Set"},
    {"Stack", "java.util.Stack"},
    {"Toolkit", "java.awt.Toolkit"},
    {"TreeMap", "java.util.TreeMap"},
    {"TreeSet", "java.util.TreeSet"},
    {"Vector", "java.util.Vector"}};
    
static int stNumClassPackages = 
    sizeof (stClassPackageName) / sizeof (ClassPackage);

/********************/
/* Static variables */
/********************/
// Properties of the Indent class
static BOOL		stPropertiesChanged;

// The indentation stack. Each element of the stack in an Indents
static IndentStackPtr	stIndentStackHead;

// Set to TRUE of output has appeared on the current line.
static BOOL		stOutputOnLine;

// Set to TRUE if the last character written to the output stream was a space.
// Used to prevent two spaces appearing together.
static BOOL		stSpaceWritten;

// Set to TRUE when parsing file and newlines can be ignored
static BOOL		stIgnoreNewLines;

// The output buffer
static char		*stResultBuffer;

// The size of the output buffer
static int		stResultBufferSize;

// Pointers to the input and output buffer
static char		*stBufPtr, *stEndBufPtr;
static char		*stResultBufPtr, *stResultEndBufPtr;

// Tokens being read by the program	
static Token		stPrevToken, stCurrentToken, stNextToken;
static char		*stPrevTokenValue;
static char		*stCurrentTokenValue;
static char		*stNextTokenValue;
static int		stTokenValueMaxSize;
static Token		stPrevNonWhitespaceToken;
static char		*stPrevNonWhitespaceTokenValue;
	
// The position of the beginning of the prev, current and stNextToken.
// These positions are zero-based (0 = first column position).
static int		stPrevTokenColumn;
static int 		stCurrentTokenColumn;
static int 		stNextTokenColumn;
	
// The value of stBufPtr before reading the prev, current and stNextToken.
static char		*stPrevTokenBufPtr;
static char		*stCurrentTokenBufPtr;
static char 		*stNextTokenBufPtr;
	
// Characters being read by the program
static char 		stPrevChar, stCurrentChar, stNextChar;
	
// The position of the beginning of the prev, current and next char.
// These positions are zero-based (0 = first column position).
static int 		stPrevColumn, stCurrentColumn, stNextColumn;
	
// The position of the first non-whitespace character on the line.
// These position is zero-based (0 = first column position).
static int 		stStartOfNonWhitespaceColumn;
	
// The current column position in the output
static int		stOutputColumn;

// The line number of the source	
static int 		stSourceLineNumber;

// A hash table containing recognized tokens
static PredefToken	**stTokenTable;
static int		stNumPredefTokens = 
    sizeof (stPredefTokens) / sizeof (PredefToken);

// The window for status messages
static HWND		stStatusWindow;

// Flag to indicate if indent failed
static BOOL		stIndentSuccess;

/******************************/
/* Static callback procedures */
/******************************/

/*********************/
/* Static procedures */
/*********************/
static void	MyAddIndent (Indent pmIndentType);
static void	MyAddSpaceAfterIdentifierIfApplicable (void);
static int	MyGetBraceIndentLevel (void);
static int	MyGetIndentLevel (void);
static int	MyHash (char *pmString, int pmTableSize);
static void	MyIndentBuffer (void);
static void	MyInitializeReader (char *pmText, int pmTextLen);
static int	MyIsPredefToken (char *pmId);
static void	MyOutputChar (char ch);
static void	MyOutputIndentation (void);
static void	MyOutputNewLine (void);
static void	MyOutputSpace (void);
static void	MyOutputString (const char *pmString);
static void	MyParseBuffer (int pmTask, char *pmPackageName, 
			       char *pmClassName, MarkLinePtr *pmMarkLine,
			       BOOL *pmIsApplication, BOOL *pmIsApplet);
static void	MyPushChar (void);
static void	MyPushToken (void);
static char	MyReadChar (void);
static void	MyReadFullIdent (char *pmIdent);
static void	MyReadNumber (char ch, char *pmNumber);
static int	MyReadToken (void);
static void	MyRemoveIndent (Indent pmIndentType);
static void	MyResizeTokenValueBuffers (char **pmNewTokenValuePtr);
static void 	MySkipNewLine (int pmEchoComments);
static BOOL	MyTokenIsIdentifier (int token);
static void	MyWriteChar (char pmChar);
static void	MyWriteSpace (void);
static void	MyWriteString (const char *pmString);


/***********************/
/* External procedures */
/***********************/
/************************************************************************/
/* EdJava_Init								*/
/************************************************************************/
BOOL	EdJava_Init (void)
{
    int	cnt, myHash;
    int myCollision = 0;    
    
    // Create the hash tables.
    stTokenTable = (PredefToken **) calloc (TOKEN_TABLE_SIZE, 
    					    sizeof (PredefToken *));
    if (stTokenTable == NULL)
    {
    	EdFail_Warn (IDS_OUTOFMEMORY, __FILE__, __LINE__, 0);
    	return FALSE;
    }
    
    for (cnt = 0 ; cnt < stNumPredefTokens ; cnt++)
    {
    	myHash = MyHash (stPredefTokens [cnt].token, TOKEN_TABLE_SIZE);
    	while (stTokenTable [myHash] != NULL)
    	{
    	    myHash = (myHash + 1) % TOKEN_TABLE_SIZE;
    	    myCollision++;
    	}
    	stTokenTable [myHash] = &stPredefTokens [cnt];
    }

    // Allocate space for the token values
    stPrevTokenValue = (char *) malloc (INIT_MAX_TOKEN_SIZE);
    stCurrentTokenValue = (char *) malloc (INIT_MAX_TOKEN_SIZE);
    stNextTokenValue  = (char *) malloc (INIT_MAX_TOKEN_SIZE);
    stTokenValueMaxSize = INIT_MAX_TOKEN_SIZE;
    stPrevNonWhitespaceTokenValue = (char *) malloc (INIT_MAX_TOKEN_SIZE);
    
    if ((stPrevTokenValue == NULL) || (stCurrentTokenValue == NULL) ||
        (stNextTokenValue == NULL) || (stPrevNonWhitespaceTokenValue == NULL)) 
    {
    	EdFail_Warn (IDS_OUTOFMEMORY, __FILE__, __LINE__, 0);
    	return FALSE;
    }
        
    return TRUE;
} // EdJava_Init


/************************************************************************/
/* EdJava_Finalize							*/
/************************************************************************/
void	EdJava_Finalize (void)
{
    free (stTokenTable);
    free (stPrevTokenValue);
    free (stCurrentTokenValue);
    free (stNextTokenValue);
    free (stPrevNonWhitespaceTokenValue);
} // EdJava_Finalize


/************************************************************************/
/* EdJava_PropertiesSet							*/
/************************************************************************/
void	EdJava_PropertiesSet (void)
{
    stPropertiesChanged = TRUE;
} // EdFile_PropertiesSet

	
/************************************************************************/
/* EdJava_PropertiesImplementChanges					*/
/************************************************************************/
void	EdJava_PropertiesImplementChanges (void)
{
    stPropertiesChanged = FALSE;
} // EdJava_PropertiesImplementChanges


/************************************************************************/
/* EdJava_GetPackageAndClass						*/
/************************************************************************/
void	EdJava_GetPackageAndClass (char *pmText, int pmTextLen,
		    		   char *pmPackage, char *pmClass)
{
    stResultBufferSize = 0;
    stResultBuffer = NULL;

    MyInitializeReader (pmText, pmTextLen);

    pmPackage [0] = 0;
    pmClass [0] = 0;
        
    MyParseBuffer (GET_CLASS, pmPackage, pmClass, NULL, NULL, NULL);
} // EdJava_GetPackageAndClass

		    			   
/************************************************************************/
/* EdJava_GetProgramType						*/
/************************************************************************/
void	EdJava_GetProgramType (char *pmText, int pmTextLen,
		    	       BOOL *pmIsApplication, BOOL *pmIsApplet)
{
    stResultBufferSize = 0;
    stResultBuffer = NULL;

    MyInitializeReader (pmText, pmTextLen);

    *pmIsApplication = FALSE;
    *pmIsApplet = FALSE;
        
    MyParseBuffer (GET_PROGRAM_TYPE, NULL, NULL, NULL, pmIsApplication,
    		   pmIsApplet);
} // EdJava_GetProgramType

		    			   
/************************************************************************/
/* EdJava_Indent							*/
/************************************************************************/
void	EdJava_Indent (HWND pmWindowForStatus, char *pmText, int pmTextLen,
		       char **pmNewBuffer, int *pmNewBufferLen,
		       int *pmNewTextLen)
{
    stStatusWindow = pmWindowForStatus;
    
    stResultBufferSize = max (INITIAL_TEXT_BUFFER_SIZE, 
    	(int) (pmTextLen * 1.2));
    stResultBuffer = malloc (stResultBufferSize);
    if (stResultBuffer == NULL)
    {
    	EdGUI_Message1 (pmWindowForStatus, 0, IDS_INDENT_FAILED_TITLE,
    	    IDS_INDENT_OUT_OF_MEM);
    	*pmNewBuffer = NULL;
    	return;
    }

    MyInitializeReader (pmText, pmTextLen);
    stIgnoreNewLines = FALSE;
    
    MyIndentBuffer ();
    
    if (stIndentSuccess)
    {
    	*pmNewBuffer = stResultBuffer;
    	*pmNewBufferLen = stResultBufferSize;
    	*pmNewTextLen = stResultBufPtr - stResultBuffer;
    }
    else
    {
    	*pmNewBuffer = NULL;
    	*pmNewBufferLen = 0;
    	*pmNewTextLen = 0;
    }
} // EdJava_Indent


/************************************************************************/
/* EdJava_IsIdentFollow							*/
/************************************************************************/
BOOL	EdJava_IsIdentFollow (char ch)
{
    return (EdJava_IsIdentStart (ch) ||
            (('0' <= ch) && (ch <= '9')));
} // EdJava_IsIdentFollow


/************************************************************************/
/* EdJava_IsIdentStart							*/
/************************************************************************/
BOOL	EdJava_IsIdentStart (char ch)
{
    return ((('a' <= ch) && (ch <= 'z')) ||
            (('A' <= ch) && (ch <= 'Z')) ||
            (ch == '_') || (ch == '$') ||
            (((char) 0xc0 <= ch) && (ch <= (char) 0xd6)) ||
            (((char) 0xd8 <= ch) && (ch <= (char) 0xf6)) ||
            (((char) 0xf8 <= ch) && (ch <= (char) 0xff)));
} // EdJava_IsIdentStart


/************************************************************************/
/* EdJava_IsInJavaLang							*/
/************************************************************************/
BOOL	EdJava_IsInJavaLang (const char *pmClassName)
{
    int	cnt;
    for (cnt = 0 ; cnt < stNumClassNamesInJavaLang ; cnt++)
    {
        if (strcmp (pmClassName, stClassNameInJavaLang [cnt]) == 0)
        {
            return TRUE;
        }
    }
    return FALSE;
} // EdJava_IsInJavaLang


/************************************************************************/
/* EdJava_IsInOtherJavaPackage						*/
/************************************************************************/
BOOL	EdJava_IsInOtherJavaPackage (const char *pmClassName, char *pmPackage)
{
    int	cnt;
    for (cnt = 0 ; cnt < stNumClassPackages ; cnt++)
    {
        if (strcmp (pmClassName, stClassPackageName [cnt].className) == 0)
        {
            strcpy (pmPackage, stClassPackageName [cnt].package);
            return TRUE;
        }
    }
    return FALSE;
} // EdJava_IsInOtherJavaPackage


/************************************************************************/
/* EdJava_IsUpperCase							*/
/************************************************************************/
BOOL	EdJava_IsUpperCase (char ch)
{
    return ((('A' <= ch) && (ch <= 'Z')) ||
            (((char) 0xc0 <= ch) && (ch <= (char) 0xd6)) ||
            (((char) 0xd8 <= ch) && (ch <= (char) 0xef)));
} // EdJava_IsUpperCase


/************************************************************************/
/* EdJava_Mark								*/
/************************************************************************/
MarkLinePtr	EdJava_Mark (char *pmText, int pmTextLen)
{
    MarkLinePtr	myMarkLine;
    
    stResultBufferSize = 0;
    stResultBuffer = NULL;

    MyInitializeReader (pmText, pmTextLen);

    myMarkLine = NULL;
    
    MyParseBuffer (MARK_METHODS, NULL, NULL, &myMarkLine, NULL, NULL);
    
    return myMarkLine;
} // EdJava_Mark


/******************************/
/* Static callback procedures */
/******************************/


/*********************/
/* Static procedures */
/*********************/
/************************************************************************/
/* MyAddIndent								*/
/************************************************************************/
static void	MyAddIndent (Indent pmIndentType)
{
    IndentStackPtr	myPtr;
    
    if (pmIndentType == BRACE_INDENT)
    {
        // A BRACE indent should remove any open parentheses or
        // brackets and the in-statement indentation.
        
        // Remove any PAREN_INDENT, BRACKET_INDENT, IN_STATEMENT indents
        // at the top of the indent stack.
        while ((stIndentStackHead != NULL) &&
            ((stIndentStackHead -> indent == PAREN_INDENT) ||
             (stIndentStackHead -> indent == BRACKET_INDENT) ||
             (stIndentStackHead -> indent == IN_STATEMENT_INDENT)))
        {
            IndentStackPtr	myToDelete = stIndentStackHead;
            stIndentStackHead = stIndentStackHead -> next;
            free (myToDelete);
        }
    }
    else if (pmIndentType == IN_STATEMENT_INDENT)
    {
        // Only add an IN_STATEMENT indent if it is not already
        // somewhere in the stack.
        BOOL		myFound = FALSE;
        IndentStackPtr	myPtr = stIndentStackHead;
        
        while (myPtr != NULL)
        {
            if (myPtr -> indent == pmIndentType)
            {
            	myFound = TRUE;
            	break;
            }
            myPtr = myPtr -> next;
        }
        if (myFound)
        {
            return;
        }    
    }    
   
    myPtr = (IndentStackPtr) malloc (sizeof (IndentStack));
    myPtr -> indent = pmIndentType;
    myPtr -> next = stIndentStackHead;
    stIndentStackHead = myPtr;
} // MyAddIndent


/************************************************************************/
/* MyAddSpaceAfterIdentifierIfApplicable				*/
/************************************************************************/
static void	MyAddSpaceAfterIdentifierIfApplicable (void)
{
    if ((stNextToken == TOK_OPEN_BRACE) || 
        (stNextToken == TOK_OPEN_PAREN) ||
        (stNextToken == TOK_PREFIX_OPERATOR) ||
        (stNextToken == TOK_OPERATOR) ||
        (MyTokenIsIdentifier (stNextToken)) ||
        (stNextToken == TOK_NUMBER) ||
        (stNextToken == TOK_STRING_LITERAL))
    {
        MyOutputSpace ();
    }    
} // MyAddSpaceAfterIdentifierIfApplicable


/************************************************************************/
/* MyGetBraceIndentLevel						*/
/************************************************************************/
static int	MyGetBraceIndentLevel (void)
{
    int 		myNumBraceIndents = 0;
    IndentStackPtr	myPtr = stIndentStackHead;
        
    while (myPtr != NULL)
    {
    	if (myPtr -> indent == BRACE_INDENT)
    	{
            myNumBraceIndents++;
        }
        myPtr = myPtr -> next;
    }    
    return myNumBraceIndents;
} // MyGetBraceIndentLevel


/************************************************************************/
/* MyGetIndentLevel							*/
/************************************************************************/
static int	MyGetIndentLevel (void)
{
    IndentStackPtr	myPtr = stIndentStackHead;
    int 		myIndent = 0;
    int			myInitializerIndent = 0;
    BOOL		myFoundOnce = FALSE;
    
    if (stIndentStackHead == NULL)
    {
        return 0;
    }

    while (myPtr != NULL)
    {
    	if ((myPtr -> indent != DO_INDENT) && (myPtr -> indent != IF_INDENT))
    	{
    	    if (!myFoundOnce && (myPtr -> indent == DO_INDENT) && 
    	        (myPtr -> indent == IF_INDENT))
    	    {
    	    	myFoundOnce = TRUE;
    	    }
    	    else if (myPtr -> indent == INITIALIZER_INDENT)
    	    {
    	    	myInitializerIndent++;
    	    }
    	    else
    	    {
    	    	myIndent++;
    	    }
    	}
        myPtr = myPtr -> next;
    }

    if (myInitializerIndent > 0)
    {
    	myIndent += (myInitializerIndent - 1);
    }

    return myIndent;
} // MyGetIndentLevel


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
    return ((myValue * 37 + pmString [0]) & 0x7fffffff) % pmTableSize;
} // MyHash


/************************************************************************/
/* MyIndentBuffer							*/
/************************************************************************/
static void	MyIndentBuffer (void)
{
    int 	myStatementParenLevel = 0;
    int 	myInitializerBraceLevel = 0;
    BOOL	myNextOpenBraceActivatesSwitch = FALSE;
    BOOL	myInForStatement = FALSE;
	
    // Read initial character and token
    MyReadChar ();
    MyReadToken ();
	
    // Skip any initial newlines
    while (MyReadToken () == TOK_NEWLINE)
    {
        ;
    }
            
    // This following loop exits at the end of the program.  It loops
    // every line.
    while (stIndentSuccess && (stCurrentToken != TOK_EOF))
    {
        switch (stCurrentToken)
        {
            case TOK_NEWLINE:
                MyOutputNewLine ();
                break;
            case TOK_SEMICOLON:
                if (myInForStatement && (myStatementParenLevel > 1))
                {
                    // Semicolon inside for parenthesis
                    // for (<expr> ; <expr> ; <expr>) or for (;;)
                    if ((stPrevToken != TOK_OPEN_PAREN) &&
                        (stPrevToken != TOK_SEMICOLON))
                    {
                        MyOutputSpace ();
                    }
                    MyOutputString (stCurrentTokenValue);
                    if ((stNextToken != TOK_CLOSE_PAREN) &&
                        (stNextToken != TOK_SEMICOLON))
                    {
                        MyOutputSpace ();
                    }
                }
                else    
                {
                    MyOutputString (stCurrentTokenValue);
                    // We definitely shouldn't have semicolons in our
                    // statement paren unless it's a FOR statement, in
                    // which case it was dealt with above.
                    myStatementParenLevel = 0;
                    MyRemoveIndent (IN_STATEMENT_INDENT);                    
                    MySkipNewLine (ECHO_COMMENTS_KEEP_LOCATION);
                    MyOutputNewLine ();
                    
                    // We also should not be in the middle of an initializer.
                    // This gets us out of the initializer so that if we
                    // missed a bracket, we're not completely messed up.
                    myInitializerBraceLevel = 0;
                    
                    if (stNextToken == TOK_WHILE)
                    {
                        MyRemoveIndent (ONE_BLOCK_INDENT);
                    }
                    else    
                    {
                        if (stNextToken == TOK_ELSE)
                        {
                            // Remove all blocks to the IF_INDENT (which is
                            // removed as well)
                            MyRemoveIndent (IF_INDENT);
                        }
                        else
                        {
                            // The following will only do something if this close
                            // brace closes an if, else, for, while or do block.
                            MyRemoveIndent (BLOCK_INDENT);
                        }        
                    }    
                }    
                
                // If we read a semicolon, something's wrong. Reset this
                myNextOpenBraceActivatesSwitch = FALSE;
                break;
            case TOK_COLON:
                MyOutputString (stCurrentTokenValue);
                MyRemoveIndent (IN_STATEMENT_INDENT);
                MySkipNewLine (ECHO_COMMENTS_KEEP_LOCATION);
                MyOutputNewLine ();
                break;
            case TOK_PERIOD:
                MyOutputString (stCurrentTokenValue);
                MyAddIndent (IN_STATEMENT_INDENT);
                if ((strcmp (stNextTokenValue, "*") != 0) &&
                    (!MyTokenIsIdentifier (stNextToken)))
                {
                    MyOutputSpace ();
                }    
                break;
            case TOK_COMMA:
                MyOutputString (stCurrentTokenValue);
                MyAddIndent (IN_STATEMENT_INDENT);
                MyOutputSpace ();
                break;
            case TOK_OPEN_BRACE:
                if ((myInitializerBraceLevel > 0) ||
                    ((stPrevNonWhitespaceToken == TOK_OPERATOR) && 
                     (strcmp (stPrevNonWhitespaceTokenValue, "=") == 0)))
                {
                    myInitializerBraceLevel++;
                    MyAddIndent (INITIALIZER_INDENT);
                    MyOutputString (stCurrentTokenValue);
                    if ((stNextToken != TOK_OPEN_BRACE) &&
                        (stNextToken != TOK_CLOSE_BRACE) &&
                        (!MyTokenIsIdentifier (stNextToken)) &&
                        (stNextToken != TOK_NUMBER) &&
                        (stNextToken != TOK_STRING_LITERAL))
                    {
                        MyOutputSpace ();
                    }    
                    break;
                }
                
                MyRemoveIndent (IN_STATEMENT_INDENT);
                if (gProperties.editOpenBraceOnSameLine)
                {
                    MyOutputString (stCurrentTokenValue);
                    MySkipNewLine (ECHO_COMMENTS);
                }
                else
                {
                    if (stOutputOnLine)
                    {
                        MyOutputNewLine ();
                    }    
                    MyOutputString (stCurrentTokenValue);
                    MySkipNewLine (ECHO_COMMENTS);
                    MyOutputNewLine ();
                }
                MyAddIndent (BRACE_INDENT);
                if (myNextOpenBraceActivatesSwitch)
                {
                    MyAddIndent (SWITCH_INDENT);
                    myNextOpenBraceActivatesSwitch = FALSE;
                }    
                
                // If we reach an open brace, we're definitely out of the FOR
                // statement.
                myInForStatement = FALSE;
                break;
            case TOK_CLOSE_BRACE:
                if (myInitializerBraceLevel > 0)
                {
                    myInitializerBraceLevel--;
                    MyRemoveIndent (INITIALIZER_INDENT);
                    MyOutputString (stCurrentTokenValue);
                    if ((stNextToken != TOK_SEMICOLON) &&
                        (stNextToken != TOK_COMMA) &&
                        (stNextToken != TOK_CLOSE_BRACE))
                    {
                        MyOutputSpace ();
                    }    
                    break;
                }
                if (stOutputOnLine)
                {
                    MyOutputNewLine ();
                }    
                MyRemoveIndent (IN_STATEMENT_INDENT);
                MyRemoveIndent (BRACE_INDENT);
                MyOutputString (stCurrentTokenValue);
                MySkipNewLine (ECHO_COMMENTS);
                MyOutputNewLine ();                
                
                if (stNextToken == TOK_WHILE)
                {
                    MyRemoveIndent (ONE_BLOCK_INDENT);
                }
                else    
                {
                    if (stNextToken == TOK_ELSE)
                    {
                        // Remove all blocks to the IF_INDENT (which is
                        // removed as well)
                        MyRemoveIndent (IF_INDENT);
                    }
                    else
                    {
                        // The following will only do something if this close
                        // brace closes an if, else, for, while or do block.
                        MyRemoveIndent (BLOCK_INDENT);
                    }        
                }    
                
                // This places blank lines between methods as needed.
                if (MyGetBraceIndentLevel () == 1)
                {
                    // For the printer, put at least two blank lines 
                    // between methods unless closing brace of class
                    // is next.
                    MySkipNewLine (DONT_ECHO_COMMENTS);
                    MySkipNewLine (DONT_ECHO_COMMENTS);
                    if (stNextToken != TOK_CLOSE_BRACE)
                    {
                        MyOutputNewLine ();
                        MyOutputNewLine ();
                    }
                }    
                break;
            case TOK_OPEN_PAREN:
                if (myStatementParenLevel > 0)
                {
                    myStatementParenLevel++;
                }
                MyOutputString (stCurrentTokenValue);
                if ((stNextToken != TOK_OPEN_PAREN) &&
                    (stNextToken != TOK_CLOSE_PAREN) &&
                    (stNextToken != TOK_PRE_AND_SUF_OPERATOR) &&
                    (stNextToken != TOK_PREFIX_OPERATOR) &&
                    (!MyTokenIsIdentifier (stNextToken)) &&
                    (stNextToken != TOK_NUMBER) &&
                    (stNextToken != TOK_STRING_LITERAL) &&
                    ((stNextToken != TOK_SEMICOLON) || (myStatementParenLevel != 2)))
                {    
                    MyOutputSpace ();
                }    
                MyAddIndent (PAREN_INDENT);
                break;
            case TOK_CLOSE_PAREN:
                MyOutputString (stCurrentTokenValue);
                MyRemoveIndent (PAREN_INDENT);
                if (myStatementParenLevel > 0)
                {
                    // Used for if (...), for (...), while (...)
                    myStatementParenLevel--;
                    if (myStatementParenLevel <= 1)
                    {
                        myStatementParenLevel = 0;
                        MyRemoveIndent (IN_STATEMENT_INDENT);
                        MySkipNewLine (ECHO_COMMENTS_KEEP_LOCATION);
                        MyOutputNewLine ();
                        if (stNextToken != TOK_OPEN_BRACE)
                        {
                            MyAddIndent (BLOCK_INDENT);
                        }
                        break;
                    }
                }        
                
                if ((stNextToken != TOK_SEMICOLON) &&
                    (stNextToken != TOK_COLON) &&
                    (stNextToken != TOK_PERIOD) &&
                    (stNextToken != TOK_COMMA) &&
                    (stNextToken != TOK_CLOSE_PAREN) &&
                    (stNextToken != TOK_PRE_AND_SUF_OPERATOR))
                {
                    MyOutputSpace ();
                }    
                break;
	    case TOK_OPEN_CLOSE_BRACKET:
                MyOutputString (stCurrentTokenValue);
                
                if ((stNextToken != TOK_SEMICOLON) &&
                    (stNextToken != TOK_COLON) &&
                    (stNextToken != TOK_PERIOD) &&
                    (stNextToken != TOK_COMMA) &&
                    (stNextToken != TOK_CLOSE_PAREN) &&
                    (stNextToken != TOK_PRE_AND_SUF_OPERATOR))
                {
                    MyOutputSpace ();
                }    
                break;
            case TOK_PRE_AND_SUF_OPERATOR:
                if ((stPrevToken == TOK_OPERATOR) ||
                    (stPrevToken == TOK_PREFIX_OPERATOR) ||
                    (stPrevToken == TOK_PRE_AND_SUF_OPERATOR))
                {
                    MyOutputSpace ();
                }    
                MyOutputString (stCurrentTokenValue);
                if ((stNextToken == TOK_OPERATOR) ||
                    (stNextToken == TOK_PREFIX_OPERATOR) ||
                    (stNextToken == TOK_PRE_AND_SUF_OPERATOR))
                {
                    MyOutputSpace ();
                }
                MyAddIndent (IN_STATEMENT_INDENT);
                break;
            case TOK_PREFIX_OPERATOR:
                MyOutputString (stCurrentTokenValue);
                // If the operator is '+' or '-', this could be a binary
                // operator, instead.  We guess that it is a binary operator
                // if the previous expression was not an operator
                if (((strcmp (stCurrentTokenValue, "+") == 0) || 
                     (strcmp (stCurrentTokenValue, "-") == 0)) &&
                    ((stPrevToken != TOK_OPERATOR) &&
                     (stPrevToken != TOK_OPEN_PAREN) &&
                     (stPrevToken != TOK_COMMA) &&
                     ((stPrevToken != TOK_IDENTIFIER) ||
		      (strcmp (stPrevTokenValue, "return") != 0))))
                {
                    MyOutputSpace ();
                }    
                MyAddIndent (IN_STATEMENT_INDENT);
                break;
            case TOK_OPERATOR:
                MyOutputString (stCurrentTokenValue);
                // A special case for the "*" in package names.
                // e.g. "import java.awt.*"
                if ((strcmp (stCurrentTokenValue, "*") != 0) ||
                    (stNextToken != TOK_SEMICOLON))
                {
                    MyOutputSpace ();
                }            
                MyAddIndent (IN_STATEMENT_INDENT);
                break;
            case TOK_SWITCH:
                myStatementParenLevel = 1;
                myNextOpenBraceActivatesSwitch = TRUE;
                MyOutputString (stCurrentTokenValue);
                MyAddIndent (IN_STATEMENT_INDENT);
                MyAddSpaceAfterIdentifierIfApplicable ();
                break;
            case TOK_CASE:
                MyRemoveIndent (SWITCH_INDENT);
                MyOutputString (stCurrentTokenValue);
                MyAddIndent (SWITCH_INDENT);
                MyAddIndent (IN_STATEMENT_INDENT);
                MyAddSpaceAfterIdentifierIfApplicable ();
                break;
            case TOK_FOR:
                myStatementParenLevel = 1;
                myInForStatement = TRUE;
                MyOutputString (stCurrentTokenValue);
                MyAddIndent (IN_STATEMENT_INDENT);
                MyAddSpaceAfterIdentifierIfApplicable ();
                break;
            case TOK_DO:
                MyOutputString (stCurrentTokenValue);
                MyRemoveIndent (IN_STATEMENT_INDENT);
                // DO_INDENT is a place holder to help check if a "while"
                // statement is the end of a do statement.
                MyAddIndent (DO_INDENT);
                MySkipNewLine (ECHO_COMMENTS_KEEP_LOCATION);
                MyOutputNewLine ();
                if (stNextToken != TOK_OPEN_BRACE)
                {
                    MyAddIndent (BLOCK_INDENT);
                }
                break;
            case TOK_WHILE:
                // Check to see if the top level of indentation is a DO_INDENT
                // If it is, then this while is closing a "do" loop and not
                // opening another loop.
                if ((stIndentStackHead != NULL) &&
                    (stIndentStackHead -> indent != DO_INDENT))
                {
                    myStatementParenLevel = 1;
                }        
                else
                {
                    MyRemoveIndent (DO_INDENT);
                }    
                MyOutputString (stCurrentTokenValue);
                MyAddIndent (IN_STATEMENT_INDENT);
                MyAddSpaceAfterIdentifierIfApplicable ();
                break;
            case TOK_IF:
                myStatementParenLevel = 1;
                MyOutputString (stCurrentTokenValue);
                MyAddIndent (IF_INDENT);
                MyAddIndent (IN_STATEMENT_INDENT);
                MyAddSpaceAfterIdentifierIfApplicable ();
                break;
            case TOK_ELSE:
                MyOutputString (stCurrentTokenValue);
                MyRemoveIndent (IN_STATEMENT_INDENT);
                
                // Added to handle "else if (...)" constructs
                if (stNextToken == TOK_IF)
                {
                    MyOutputSpace ();
                }
                else    
                {
                    MySkipNewLine (ECHO_COMMENTS_KEEP_LOCATION);
                    MyOutputNewLine ();
                    if (stNextToken != TOK_OPEN_BRACE)
                    {
                        MyAddIndent (BLOCK_INDENT);
                    }
                }
                break;
            case TOK_IDENTIFIER:
            case TOK_STRING_LITERAL:
                MyOutputString (stCurrentTokenValue);
                MyAddIndent (IN_STATEMENT_INDENT);
                MyAddSpaceAfterIdentifierIfApplicable ();
                break;
            case TOK_NUMBER:
                MyOutputString (stCurrentTokenValue);
                MyAddIndent (IN_STATEMENT_INDENT);
                if ((stNextToken == TOK_PERIOD) ||
                    (stNextToken == TOK_OPEN_BRACE) || 
                    (stNextToken == TOK_OPEN_PAREN) ||
                    (stNextToken == TOK_PREFIX_OPERATOR) ||
                    (stNextToken == TOK_OPERATOR) ||
                    (MyTokenIsIdentifier (stNextToken)) ||
                    (stNextToken == TOK_NUMBER) ||
                    (stNextToken == TOK_STRING_LITERAL))
                {
                    MyOutputSpace ();
                }    
                break;
            case TOK_ONE_LINE_COMMENT:
                if (stOutputColumn == -1)
                {
                    stOutputColumn = 0;
                }
                if ((stOutputColumn != 0) || (stCurrentTokenColumn != 0))
                {
                    do
                    {
                        MyWriteSpace ();
                    } while (stOutputColumn < stCurrentTokenColumn);
                }    
                MyOutputString (stCurrentTokenValue);
                break;
            case TOK_LONG_COMMENT:
                if (stOutputOnLine)
                {
                    MyOutputNewLine ();
                }
                
                {
                    char *myIndex = stCurrentTokenValue;
                    char *myNewIndex;
                
                    myNewIndex = strchr (myIndex, '\n');
                    while (myNewIndex != NULL)
                    {
                    	char	myTempChar;
                    
                    	myTempChar = *myNewIndex;
                    	*myNewIndex = 0;
                    	MyOutputString (myIndex);
                    	*myNewIndex = myTempChar;
                    
                    	MyOutputNewLine ();
                    	myIndex = myNewIndex + 1;
                    	myNewIndex = strchr (myIndex, '\n');
                    }    
                    MyOutputString (myIndex);
                }
                break;
        } // switch (stCurrentToken)
            
        MyReadToken ();
    }    
} // MyIndentBuffer


/************************************************************************/
/* MyInitializeReader							*/
/************************************************************************/
static void	MyInitializeReader (char *pmText, int pmTextLen)
{
    // Initialization
    stIndentStackHead = NULL;
    stOutputOnLine = FALSE;
    stSpaceWritten = FALSE;

    stIgnoreNewLines = TRUE;
 
    stPrevToken = TOK_NONE;
    stCurrentToken = TOK_NONE;
    stNextToken = TOK_NONE;
    stPrevNonWhitespaceToken = TOK_NONE;
    
    stPrevTokenValue [0] = EOS;
    stCurrentTokenValue [0] = EOS;
    stNextTokenValue [0] = EOS;
    stPrevNonWhitespaceTokenValue [0] = EOS;
        
    stPrevTokenColumn = 0;
    stCurrentTokenColumn = 0;
    stNextTokenColumn = 0;
    
    stPrevTokenBufPtr = NULL;
    stCurrentTokenBufPtr = NULL;
    stNextTokenBufPtr = NULL;
    
    stPrevChar = EOF_MARKER;
    stCurrentChar = EOF_MARKER;
    stNextChar = EOF_MARKER;
    
    stPrevColumn = -1;
    stCurrentColumn = -1;
    stNextColumn = -1;

    stStartOfNonWhitespaceColumn = -1;
	
    stOutputColumn = 0;
    
    stBufPtr = pmText;
    stEndBufPtr = stBufPtr + pmTextLen;
    stResultBufPtr = stResultBuffer;
    stResultEndBufPtr = stResultBufPtr + stResultBufferSize - 50;
    stSourceLineNumber = 1;
    
    stIndentSuccess = TRUE;

    stStatusWindow = NULL;
} // MyInitializeReader


/************************************************************************/
/* MyIsPredefToken							*/
/************************************************************************/
static int	MyIsPredefToken (char *pmId)
{
    int	myHash = MyHash (pmId, TOKEN_TABLE_SIZE);

    while (stTokenTable [myHash] != NULL)
    {
    	if (strcmp (pmId, stTokenTable [myHash] -> token) == 0)
    	{
    	    return stTokenTable [myHash] -> tokenValue;
	}
    	myHash = (myHash + 1) % TOKEN_TABLE_SIZE;
    }
    return TOK_NONE;
} // MyIsPredefToken


/************************************************************************/
/* MyOutputChar								*/
/************************************************************************/
static void	MyOutputChar (char ch)
{
	
    if (ch == '\n')
    {
        stOutputOnLine = FALSE;
    }
    else
    {
        if (!stOutputOnLine)
        {
            MyOutputIndentation ();
            stOutputOnLine = TRUE;
        }
    }        
    
    if (ch == ' ')
    {
    	EdFail_Fatal (IDS_OUTPUTCHAR_OF_SPACE, __FILE__, __LINE__, 0);
    }
    
    stSpaceWritten = FALSE;    

    MyWriteChar (ch);
} // MyOutputChar


/************************************************************************/
/* MyOutputIndentation							*/
/************************************************************************/
static void	MyOutputIndentation (void)
{
    int myIndentationLevel;
    int	cnt;
    int mySpaces;
    
    myIndentationLevel = MyGetIndentLevel ();
    
    if (myIndentationLevel == 0) return;

    // Calculate size of indentation
    mySpaces = myIndentationLevel * gProperties.editIndentSize;
        
    // Then output rest of spaces
    for (cnt = 0 ; cnt < mySpaces ; cnt++)
    {
        MyWriteChar (' ');
    }            
} // MyOutputIndentation


/************************************************************************/
/* MyOutputSpace							*/
/************************************************************************/
static void	MyOutputSpace (void)
{
    if (!stSpaceWritten)
    {
        MyWriteChar (' ');
        stSpaceWritten = TRUE;
    }    
} // MyOutputSpace


/************************************************************************/
/* MyOutputString							*/
/************************************************************************/
static void	MyOutputString (const char *pmString)
{
    if (!stOutputOnLine)
    {
        MyOutputIndentation ();
        stOutputOnLine = TRUE;
    }    
    
    stSpaceWritten = FALSE;
    
    MyWriteString (pmString);
} // MyOutputString


/************************************************************************/
/* MyOutputNewLine							*/
/************************************************************************/
static void	MyOutputNewLine (void)
{
    MyOutputChar ('\n');
    stOutputOnLine = FALSE;
} // MyOutputNewLine


/************************************************************************/
/* MyParseBuffer							*/
/*									*/
/* This is much like the indenter, except that it doesn't write		*/
/* anything out.							*/
/************************************************************************/
static void	MyParseBuffer (int pmTask, char *pmPackageName, 
			       char *pmClassName, MarkLinePtr *pmMarkLine,
			       BOOL *pmIsApplication, BOOL *pmIsApplet)
{
    int 	myStatementParenLevel = 0;
    int 	myInitializerBraceLevel = 0;
    BOOL	myNextOpenBraceActivatesSwitch = FALSE;
    BOOL	myInStatement = FALSE;
    BOOL	myInForStatement = FALSE;
    MarkLinePtr	myMarkLine, myLastMarkLine = NULL;
    	
    // Read initial character and token
    MyReadChar ();
    MyReadToken ();
	
    // Skip any initial newlines
    while (MyReadToken () == TOK_NEWLINE)
    {
        ;
    }
            
    // This following loop exits at the end of the program.  It loops
    // every line.
    while (stIndentSuccess && (stCurrentToken != TOK_EOF))
    {
        switch (stCurrentToken)
        {
            case TOK_NEWLINE:
                break;
            case TOK_SEMICOLON:
                if (myInForStatement && (myStatementParenLevel > 1))
                {
                }
                else    
                {
                    // We definitely shouldn't have semicolons in our
                    // statement paren unless it's a FOR statement, in
                    // which case it was dealt with above.
                    myStatementParenLevel = 0;
                    
                    myInStatement = FALSE;
                    
                    // We also should not be in the middle of an initializer.
                    // This gets us out of the initializer so that if we
                    // missed a bracket, we're not completely messed up.
                    myInitializerBraceLevel = 0;
                    
                    if (stNextToken == TOK_WHILE)
                    {
                        MyRemoveIndent (ONE_BLOCK_INDENT);
                    }
                    else    
                    {
                        if (stNextToken == TOK_ELSE)
                        {
                            // Remove all blocks to the IF_INDENT (which is
                            // removed as well)
                            MyRemoveIndent (IF_INDENT);
                        }
                        else
                        {
                            // The following will only do something if this close
                            // brace closes an if, else, for, while or do block.
                            MyRemoveIndent (BLOCK_INDENT);
                        }        
                    }    
                }    
                
                // If we read a semicolon, something's wrong. Reset this
                myNextOpenBraceActivatesSwitch = FALSE;
                break;
            case TOK_COLON:
                myInStatement = FALSE;
                break;
            case TOK_PERIOD:
            case TOK_COMMA:
            case TOK_PRE_AND_SUF_OPERATOR:
            case TOK_PREFIX_OPERATOR:
            case TOK_STRING_LITERAL:
            case TOK_NUMBER:
            case TOK_OPERATOR:
            case TOK_OPEN_CLOSE_BRACKET:
                myInStatement = TRUE;
                break;
            case TOK_ONE_LINE_COMMENT:
            case TOK_LONG_COMMENT:
                break;
            case TOK_IDENTIFIER:
                // Check if we're reading the package or class statement
                if ((pmTask == GET_CLASS) && (myInitializerBraceLevel == 0) &&
                    (myStatementParenLevel == 0) && (stIndentStackHead == NULL))
                {
                    if (strcmp (stCurrentTokenValue, "package") == 0)
                    {
                    	if (stNextToken == TOK_IDENTIFIER)
                    	{
                    	    MyReadToken (); // Current token = start of pkg name
                    	    MyReadFullIdent (pmPackageName);
                    	}
                    }
                    else if (strcmp (stCurrentTokenValue, "class") == 0)
                    {
                    	if (stNextToken == TOK_IDENTIFIER)
                    	{
                    	    MyReadToken (); // Current token = class name
                    	    strcpy (pmClassName, stCurrentTokenValue);
                    	    return;
                    	}
                    }
                }
                // Check if we're reading the class statement for extends Applet
                else if ((pmTask == GET_PROGRAM_TYPE) && 
                    	 (myInitializerBraceLevel == 0) &&
                    	 (myStatementParenLevel == 0) && 
                    	 (stIndentStackHead == NULL) &&
                    	 (strcmp (stCurrentTokenValue, "class") == 0))
                {
                    // Check for class name
                    if (stNextToken == TOK_IDENTIFIER)
                    {
                    	MyReadToken (); // Current token = class name
                    	
                    	// Check for extends
                    	if ((stNextToken == TOK_IDENTIFIER) && 
			    (strcmp (stNextTokenValue, "extends") == 0))
                	{
                    	    MyReadToken (); // Current token = "extends"
                    	    
                    	    if (stNextToken == TOK_IDENTIFIER)
                    	    {
                    	    	char	myIdent [4096];
                    	    	
                    	    	MyReadToken (); // Curren token = parent class
                    	    	MyReadFullIdent (myIdent);
                    	    	
				if ((strcmp (myIdent, SHORT_APPLET) == 0) ||
				    (strcmp (myIdent, LONG_APPLET) == 0) ||
				    (strcmp (myIdent, SHORT_JAPPLET) == 0) ||
				    (strcmp (myIdent, LONG_JAPPLET) == 0))
                	    	{
                	    	    *pmIsApplet = TRUE;
                	    	} // if Applet, JApplet or java.applet.Applet
                	    } // if identifier
                	} // if extends
		    } // if class name
                } 
                // Check to see if we're at the start of a declaration
                else if (((pmTask == MARK_METHODS) || 
                	  (pmTask == GET_PROGRAM_TYPE)) &&
                	 (myInitializerBraceLevel == 0) &&
                         (myStatementParenLevel == 0) && 
                         (stIndentStackHead != NULL) &&
                         (stIndentStackHead -> indent == BRACE_INDENT) &&
                         (stIndentStackHead -> next == NULL) && !myInStatement)
		{
		    char	myMarkText [200];
		    BOOL	myFailFlag, myAfterParen;
		    UINT	myLenLeft = 150;
		    int		mySourceLine;
		    int		myComponentNumber; // Used to track String[] arg
		    BOOL	mySquareBracketsFound;
		    		    		    
		    //
		    // We read identifiers, adding them to the line, if we
		    // read an open square, then make certain the next is a
		    // close square, when we read an open paren, then we
		    // go to next section
		    //
		    strcpy (myMarkText, stCurrentTokenValue);
		    myFailFlag = FALSE;
		    mySourceLine = stSourceLineNumber - 1;
		    
		    while (TRUE)
		    {
		    	if (stNextToken == TOK_IDENTIFIER)
		    	{
		    	    MyReadToken (); // Curren token = next identifier
		    	    if (strlen (stCurrentTokenValue) > myLenLeft)
		    	    {
		                myFailFlag = TRUE;
		                break;
		            }
		            else
		            {
		            	strcat (myMarkText, " ");
		            	strcat (myMarkText, stCurrentTokenValue);
		            	myLenLeft -= strlen (stCurrentTokenValue) + 1;
		            }
		        }
		        else if (stNextToken == TOK_OPEN_CLOSE_BRACKET)
		        {
		            MyReadToken (); // Current token = "[]"
		            strcat (myMarkText, " []");
		            myLenLeft -= 3;
		        }
		    	else if (stNextTokenValue [0] == '[')
		    	{
		    	    MyReadToken (); // Current token = "["
		    	    if (stNextTokenValue [0] == ']')
		    	    {
		    	    	MyReadToken (); // Current token = "]"
		            	strcat (myMarkText, " []");
		            	myLenLeft -= 3;
		    	    }
		    	    else
		    	    {
		                MyAddIndent (PAREN_INDENT); // For "["
		                myFailFlag = TRUE;
		                break;
		            }
		        }
		        else
		        {
		            break;
		        }
		    } // while
		    if (myFailFlag || (stNextTokenValue [0] != '('))
		    {
		    	myInStatement = TRUE;
		    	break;
		    }
		    else
		    {
		        strcat (myMarkText, " (");
		        myLenLeft -= 2;
		    }

		    //
		    // If we're a getting the program type, check for main
		    //		    
                    if ((pmTask == GET_PROGRAM_TYPE) &&
                        ((strcmp (myMarkText, "public static void main (") != 0) &&
                         (strcmp (myMarkText, "static public void main (") != 0)))
                    {
                    	break;
                    }
                    
		    //
		    // We've now read "<id> <id> <id> (".  Now read until ")"
		    //
		    MyReadToken (); // Current Token = "("
		    myAfterParen = TRUE;
		    myComponentNumber = 0;
		    mySquareBracketsFound = FALSE;
		    
		    MyReadToken (); // Current Token = After Paren
		    
		    while (TRUE)
		    {
		    	if (stCurrentToken == TOK_IDENTIFIER)
		    	{
		    	    char	myIdent [4096];
		    	    
		    	    MyReadFullIdent (myIdent);
		    	
		    	    if (strlen (myIdent) > myLenLeft)
		    	    {
		                myFailFlag = TRUE;
		                break;
		            }
		            else
		            {
		            	if ((pmTask == GET_PROGRAM_TYPE) && 
		            	    (myComponentNumber == 0) &&
		            	    (strcmp (myIdent, SHORT_STRING) != 0) &&
				    (strcmp (myIdent, LONG_STRING) != 0))
				{
		                    myFailFlag = TRUE;
		                    break;
				}
				    		            	    
		            	if (!myAfterParen)
		            	{
		            	    strcat (myMarkText, " ");
		            	    myLenLeft--;
		            	}
		            	else
		            	{
		            	    myAfterParen = FALSE;
		            	}
		            	
		            	strcat (myMarkText, myIdent);
		            	myLenLeft -= strlen (myIdent);
				myComponentNumber++;
		            }
		        }
		        else if (stCurrentToken == TOK_OPEN_CLOSE_BRACKET)
		        {
		            strcat (myMarkText, " []");
		            myLenLeft -= 3;
			    mySquareBracketsFound = TRUE;
			    myComponentNumber++;
		        }
		    	else if (stCurrentTokenValue [0] == '[')
		    	{
		    	    if (stNextTokenValue [0] == ']')
		    	    {
		    	    	MyReadToken (); // Current token = "]"
		            	strcat (myMarkText, " []");
		            	myLenLeft -= 3;
		    	    }
		    	    else
		    	    {
		                MyAddIndent (PAREN_INDENT); // For "["
		                myFailFlag = TRUE;
		                break;
		            }
		            
		            // If we've already found one square bracket, a
		            // second is not expected in the main declaration
		            if ((pmTask == GET_PROGRAM_TYPE) && 
		                mySquareBracketsFound)
		            {
		                MyAddIndent (PAREN_INDENT); // For "["
		                myFailFlag = TRUE;
		                break;
			    }
			    		            	
			    mySquareBracketsFound = TRUE;
			    myComponentNumber++;
		        }
			else if (stCurrentToken == TOK_COMMA)
			{
			    // More parameters, but also definitely not main ()
			    if (pmTask == GET_PROGRAM_TYPE) 
			    {
		                myFailFlag = TRUE;
		                break;
			    }
			    strcat (myMarkText, ",");
	            	    myLenLeft--;
			}
		        else
		        {
		            break;
		        }
		        
		        MyReadToken ();
		    } // while
		    if (myFailFlag || (stCurrentTokenValue [0] != ')'))
		    {
		        MyAddIndent (PAREN_INDENT); // For "("
		        myInStatement = TRUE;
		    	break;
		    }
		    else
		    {
		        strcat (myMarkText, ")");
		    }

		    //
		    // Now we've read "<id> <id> <id> ( <id> <id> )" 
		    // Now we expect "throws exception"
		    //
		    if ((stNextToken == TOK_IDENTIFIER) && 
			(strcmp (stNextTokenValue, "throws") == 0))
		    {
			// Read id, id, id
		    	MyReadToken ();  // Current token = "throws"
			if (stNextToken == TOK_IDENTIFIER)
			{
			    while (TRUE)
			    {
		    		MyReadToken (); // Current token = excpn name
		    		MyReadFullIdent (NULL);
				if (stNextToken == TOK_COMMA)
				{
		    		    MyReadToken (); // Current token = ","
				    if (stNextToken != TOK_IDENTIFIER)
				    {
				    	// throws <id>, non-id is illegal
				    	myFailFlag = TRUE;
				    	break;
				    }
				}
				else
				{
				    break;
				}
			    } // while
			} // if
			else
			{
		    	    // throws non-id is illegal
			    myFailFlag = TRUE;
			}
		    } // if "throws"

		    //
		    // Skip over the possible comments searching for 
		    // possible open brace
		    //
		    while ((stNextToken == TOK_ONE_LINE_COMMENT) ||
			   (stNextToken == TOK_LONG_COMMENT))
		    {
			MyReadToken ();
		    }

		    //
		    // Finally, we check for an open brace, indicating 
		    // this is a method declaration
		    //
		    if (myFailFlag || (stNextToken != TOK_OPEN_BRACE))
		    {
		        myInStatement = TRUE;
		        break;
		    }
		    
		    //
		    // Check if String[] args or String args[]
		    //
		    if (pmTask == GET_PROGRAM_TYPE)
		    {
		    	if ((myComponentNumber == 3) && mySquareBracketsFound)
		    	{
		    	    *pmIsApplication = TRUE;
		    	}
		    	break;
		    }
		    
		    // Add it to the marks
		    myMarkLine = malloc (sizeof (MarkLine));
		    // TW Check for NULL
		    myMarkLine -> line = mySourceLine;
		    myMarkLine -> name = malloc (strlen (myMarkText) + 1);
		    // TW Check for NULL
		    strcpy (myMarkLine -> name, myMarkText);
		    myMarkLine -> next = NULL;
		    if (myLastMarkLine == NULL)
			*pmMarkLine = myMarkLine;
		    else
			myLastMarkLine -> next = myMarkLine;
		    myLastMarkLine = myMarkLine;
		}                         
		else
		{
		    myInStatement = TRUE;
		}
                break;
            case TOK_OPEN_BRACE:
                if ((myInitializerBraceLevel > 0) ||
                    ((stPrevNonWhitespaceToken == TOK_OPERATOR) && 
                     (strcmp (stPrevNonWhitespaceTokenValue, "=") == 0)))
                {
                    myInitializerBraceLevel++;
                    MyAddIndent (INITIALIZER_INDENT);
                    break;
                }
                
                myInStatement = FALSE;
                
                MyAddIndent (BRACE_INDENT);
                if (myNextOpenBraceActivatesSwitch)
                {
                    MyAddIndent (SWITCH_INDENT);
                    myNextOpenBraceActivatesSwitch = FALSE;
                }    
                
                // If we reach an open brace, we're definitely out of the FOR
                // statement.
                myInForStatement = FALSE;
                break;
            case TOK_CLOSE_BRACE:
                if (myInitializerBraceLevel > 0)
                {
                    myInitializerBraceLevel--;
                    MyRemoveIndent (INITIALIZER_INDENT);
                    break;
                }
                myInStatement = FALSE;
                
                MyRemoveIndent (BRACE_INDENT);
                
                if (stNextToken == TOK_WHILE)
                {
                    MyRemoveIndent (ONE_BLOCK_INDENT);
                }
                else    
                {
                    if (stNextToken == TOK_ELSE)
                    {
                        // Remove all blocks to the IF_INDENT (which is
                        // removed as well)
                        MyRemoveIndent (IF_INDENT);
                    }
                    else
                    {
                        // The following will only do something if this close
                        // brace closes an if, else, for, while or do block.
                        MyRemoveIndent (BLOCK_INDENT);
                    }        
                }    
                break;
            case TOK_OPEN_PAREN:
                if (myStatementParenLevel > 0)
                {
                    myStatementParenLevel++;
                }
                MyAddIndent (PAREN_INDENT);
                break;
            case TOK_CLOSE_PAREN:
                MyRemoveIndent (PAREN_INDENT);
                if (myStatementParenLevel > 0)
                {
                    // Used for if (...), for (...), while (...)
                    myStatementParenLevel--;
                    if (myStatementParenLevel <= 1)
                    {
                        myStatementParenLevel = 0;
                        myInStatement = FALSE;
                        if (stNextToken != TOK_OPEN_BRACE)
                        {
                            MyAddIndent (BLOCK_INDENT);
                        }
                        break;
                    }
                }        
                break;
            case TOK_SWITCH:
                myStatementParenLevel = 1;
                myInStatement = TRUE;
                myNextOpenBraceActivatesSwitch = TRUE;
                break;
            case TOK_CASE:
                MyRemoveIndent (SWITCH_INDENT);
                MyAddIndent (SWITCH_INDENT);
                myInStatement = TRUE;
                break;
            case TOK_FOR:
                myStatementParenLevel = 1;
                myInForStatement = TRUE;
                myInStatement = TRUE;
                break;
            case TOK_DO:
                // DO_INDENT is a place holder to help check if a "while"
                // statement is the end of a do statement.
                MyAddIndent (DO_INDENT);
                if (stNextToken != TOK_OPEN_BRACE)
                {
                    MyAddIndent (BLOCK_INDENT);
                }
                myInStatement = FALSE;
                break;
            case TOK_WHILE:
                // Check to see if the top level of indentation is a DO_INDENT
                // If it is, then this while is closing a "do" loop and not
                // opening another loop.
                if ((stIndentStackHead != NULL) &&
                    (stIndentStackHead -> indent != DO_INDENT))
                {
                    myStatementParenLevel = 1;
                }        
                else
                {
                    MyRemoveIndent (DO_INDENT);
                }    
                myInStatement = TRUE;
                break;
            case TOK_IF:
                myStatementParenLevel = 1;
                MyAddIndent (IF_INDENT);
                myInStatement = TRUE;
                break;
            case TOK_ELSE:
                // Added to handle "else if (...)" constructs
                myInStatement = FALSE;
                if ((stNextToken != TOK_IF) && (stNextToken != TOK_OPEN_BRACE))
                {
                    MyAddIndent (BLOCK_INDENT);
                }
                break;
        } // switch (stCurrentToken)
            
        MyReadToken ();
    }    
} // MyParseBuffer


/************************************************************************/
/* MyPushChar								*/
/************************************************************************/
static void	MyPushChar (void)
{
    stBufPtr--;
    
    // Reverse the character and positions from prev to current to next.
    stNextChar = stCurrentChar;
    stCurrentChar = stPrevChar;
    stPrevChar = EOS;
    
    if (stNextChar == '\n')
    {
	stSourceLineNumber--;
    }
    stNextColumn = stCurrentColumn;
    stCurrentColumn = stPrevColumn;
    stPrevColumn = 0;
} // MyPushChar


/************************************************************************/
/* MyReadChar								*/
/************************************************************************/
static char	MyReadChar (void)
{
    // Advance the character and positions from next to current to prev.
    stPrevChar = stCurrentChar;
    stCurrentChar = stNextChar;
    
    stPrevColumn = stCurrentColumn;
    stCurrentColumn = stNextColumn;
    
    if (stBufPtr >= stEndBufPtr)
    {
        stNextChar = EOF_MARKER;
        stBufPtr++;
    }
    else
    {
        stNextChar = *stBufPtr++;
    }    
        
    if (stCurrentChar == '\n')
    {
        stNextColumn = 0;
        stStartOfNonWhitespaceColumn = -1;
            
        // Update source line number
        stSourceLineNumber++;
        if (stSourceLineNumber % 10 == 0)
        {
	    if (stStatusWindow != NULL)
	    {
		EdWin_ShowStatus (stStatusWindow, "Indented: %d lines", 
		    stSourceLineNumber);
	    }
        }
        return (stCurrentChar);
    }    
        
    stNextColumn = stCurrentColumn + 1;

    if ((stCurrentChar != EOF_MARKER) && (stCurrentChar != ' ') && 
        (stCurrentChar != '\n') && (stStartOfNonWhitespaceColumn == -1))
    {
        stStartOfNonWhitespaceColumn = stCurrentColumn;
    }    

    return (stCurrentChar);
} // MyReadChar


/************************************************************************/
/* MyReadFullIdent							*/
/*									*/
/* If stCurrentToken is an identifier, construct the whole identifier	*/
/* including periods.  This means reading several tokens.		*/
/************************************************************************/
static void	MyReadFullIdent (char *pmIdent)
{
    if (pmIdent != NULL)
    {
    	strcpy (pmIdent, stCurrentTokenValue);
    }
    
    while (TRUE)
    {
    	if (stNextToken != TOK_PERIOD)
    	{
    	    break;
    	}
    	
    	MyReadToken (); // "." is now current
    	if (pmIdent != NULL)
    	{
    	    strcat (pmIdent, ".");
    	}

    	if (stNextToken != TOK_IDENTIFIER)
    	{
    	    break;
    	}
    	
    	MyReadToken (); // ID is now current
    	if (pmIdent != NULL)
    	{
    	    strcat (pmIdent, stCurrentTokenValue);
    	}
    }
} // MyReadFullIdent


/************************************************************************/
/* MyReadNumber								*/
/*									*/
/* Reads in an integer or floating point literal			*/
/*									*/
/* Formats supported							*/
/*  (1) Hexadecimal  0X[hexadecimal digits]{L}				*/
/*  (2) Integer - Long  [digits]L					*/
/*  (3) Float - [digits]F						*/
/*  (4) Double - [digits]D						*/
/*  (5) Real - {digits}.{digits}E{+,-}{digits}{F,D}			*/
/*  (6) Real - {digits}.{digits}{F,D}					*/
/*  (7) Real - {digits}.{digits}					*/
/*  (8) Real - [digits]E{+,-}{digits}{F,D}				*/
/*  (9) Real - [digits]E{+,-}{digits}					*/
/* (10) Real - [digits]							*/
/************************************************************************/
static void	MyReadNumber (char ch, char *pmNumber)
{
    char	*myPtr;
    
    myPtr = pmNumber;
            
    if ((ch == '0') && (tolower (stNextChar) == 'x'))
    {
        // (1) Hexadecimal  0{x,X}[hexadecimal digits]{l,L}
        
        *myPtr++ = ch;			// Add the '0'
        *myPtr++ = MyReadChar ();	// Read and add the 'x'

        // Add any hex digits following it.
        ch = MyReadChar ();
        while (isdigit (ch) || (('a' <= tolower (ch)) && (tolower (ch) <= 'f')))
        {
	    *myPtr++ = ch;            
            ch = MyReadChar ();
        }
        
        // Add the 'L' if it's there
        if (tolower (ch) == 'l')
        {
	    *myPtr++ = ch;            
	    *myPtr = EOS;
            return;
        }    
            
	*myPtr = EOS;
        MyPushChar ();
    
        return;
    }    
        
    // Add any decimal digits in the number.
    while (isdigit (ch))
    {
	*myPtr++ = ch;            
        ch = MyReadChar ();
    }
        
    // Add the 'L' if it's there
    if (tolower (ch) == 'l')
    {
        // (2) Integer - Long  [digits]{l,L}
        
        // If there's an 'L' at the end, it was an integer,
        // and it's now completed.
	*myPtr++ = ch;            
	*myPtr = EOS;
        return;
    }    
        
    if ((tolower (ch) == 'f') || (tolower (ch) == 'd'))
    {
        // (3) Float - [digits]F
        // (4) Double - [digits]D
        
        // If there's an 'F' or 'D' at the end, it was a real,
        // and it's now completed.
	*myPtr++ = ch;            
	*myPtr = EOS;
        return;
    }
    
    if (ch == '.')
    {
        // (5) Real - {digits}.{digits}E{+,-}[digits]{F,D}
        // (6) Real - {digits}.[digits]{F,D}
        // (7) Real - {digits}.[digits]}
	*myPtr++ = ch;            
        ch = MyReadChar ();
                    
        // Add any decimal digits after decimal place in the number.
        while (isdigit (ch))
        {
	    *myPtr++ = ch;            
            ch = MyReadChar ();
        }
        
        if (tolower (ch) == 'e')
        {
            // (5) Real - {digits}.{digits}E{+,-}[digits]{F,D}
	    *myPtr++ = ch;            
            ch = MyReadChar ();
            if ((ch == '+') || (ch == '-'))
            {
	    	*myPtr++ = ch;            
            	ch = MyReadChar ();
            }
            while (isdigit (ch))
            {
	    	*myPtr++ = ch;            
            	ch = MyReadChar ();
            }
            if ((tolower (ch) == 'f') || (tolower (ch) == 'd'))
            {
	    	*myPtr++ = ch;            
            }
            else
            {
                MyPushChar ();
            }
	    *myPtr = EOS;
            return;
        }    
        else if ((tolower (ch) == 'f') || (tolower (ch) == 'd'))
        {
            // (6) Real - {digits}.[digits]{F,D}
	    *myPtr++ = ch;            
        }
        else
        {
            // (7) Real - {digits}.[digits]
            MyPushChar ();
        }        
	*myPtr = EOS;
        return;
    }    
    else if (tolower (ch) == 'e')
    {
        // (8) Real - [digits]E{+,-}{digits}{F,D}
        // (9) Real - [digits]E{+,-}{digits}
	*myPtr++ = ch;            
        ch = MyReadChar ();
        if ((ch == '+') || (ch == '-'))
        {
	    *myPtr++ = ch;            
            ch = MyReadChar ();
        }
        while (isdigit (ch))
        {
	    *myPtr++ = ch;            
            ch = MyReadChar ();
        }
        if ((tolower (ch) == 'f') || (tolower (ch) == 'd'))
        {
            // (8) Real - [digits]E{+,-}{digits}{F,D}
	    *myPtr++ = ch;            
        }
        else
        {
            // (9) Real - [digits]E{+,-}{digits}
            MyPushChar ();
        }
	*myPtr = EOS;
        return;
    }    
    else
    {
        // (10) Integer - [digits]
        MyPushChar ();
	*myPtr = EOS;
        return;
    }        
} // MyReadNumber


/************************************************************************/
/* MyReadToken								*/
/************************************************************************/
static int	MyReadToken (void)
{
    char ch;
    
    stPrevToken = stCurrentToken;
    strcpy (stPrevTokenValue, stCurrentTokenValue);
    stPrevTokenColumn = stCurrentTokenColumn;
    stPrevTokenBufPtr = stCurrentTokenBufPtr;
    
    if ((stCurrentToken != TOK_NEWLINE) && 
    	(stCurrentToken != TOK_ONE_LINE_COMMENT) &&
        (stCurrentToken != TOK_LONG_COMMENT))
    {
        stPrevNonWhitespaceToken = stCurrentToken;
        strcpy (stPrevNonWhitespaceTokenValue, stCurrentTokenValue);
    }

    stCurrentToken = stNextToken;
    strcpy (stCurrentTokenValue, stNextTokenValue);
    stCurrentTokenColumn = stNextTokenColumn;
    stCurrentTokenBufPtr = stNextTokenBufPtr;
    
    // Skip spaces, formfeeds and tabs
    do
    {
        ch = MyReadChar ();
    }    
    while ((ch == ' ') || (ch == '\f') || (ch == '\t') || 
           (stIgnoreNewLines && (ch == '\n')));

    // We are now ready to read the token
    // Record the current position in the line and buffer pointer.
    if (stStartOfNonWhitespaceColumn == -1)
    {
        stNextTokenColumn = 0;
    }
    else
    {
        // The stCurrentColumn points to the beginning
        // of the first character in the token, adjusted for the
        // beginning of the non whitespace character.  Thus the first
        // token should always be at position 0.
        stNextTokenColumn = stCurrentColumn - stStartOfNonWhitespaceColumn;
    }
    stNextTokenBufPtr = stBufPtr;
    
    // Read the token
    // Check for end-of-file
    if (ch == EOF_MARKER)
    {
        stNextToken = TOK_EOF;
        stNextTokenValue [0] = EOS;
    }
    // Check for identifier 
    // TW Change a-zA-Z to accept high ASCII
    else if (isalpha (ch) || (ch == '$') || (ch == '_'))
    {
        //
        // Read the identifier
        //    
        char	*myIdentifier = stNextTokenValue;
	int	myIdentifierSize = 10; // Allow a little extra room

	// Calculate identifier size
        do
        {
            *myIdentifier++ = ch;
            myIdentifierSize++;
            if (myIdentifierSize > stTokenValueMaxSize)
            {
            	MyResizeTokenValueBuffers (&myIdentifier);
            }
            ch = MyReadChar ();
        } while (isalpha (ch) || isdigit (ch) || (ch == '$') || (ch == '_'));
        
        *myIdentifier = EOS;
        
        MyPushChar ();
    
        // Check to see if we recognize it.
        stNextToken = MyIsPredefToken (stNextTokenValue);
        
        // If not recognized, then it's a regular token
        if (stNextToken == TOK_NONE)
        {
            stNextToken = TOK_IDENTIFIER;
        }    
    }
    // Check for number
    else if (isdigit (ch) || ((ch == '.') && isdigit (stNextChar)))
    {
        //
        // Read the number
        //
        stNextToken = TOK_NUMBER;
        MyReadNumber (ch, stNextTokenValue);
    }
    // Check for string literal
    else if ((ch == '"') || (ch == '\''))
    {
        //
        // Read the string literal
        //
        char	myLiteralEnd = ch;
        char	*myLiteral = stNextTokenValue;
	int	myLiteralSize = 10; // Allow a little extra room
        
        stNextToken = TOK_STRING_LITERAL;
        
        *myLiteral++ = ch;
        ch = MyReadChar ();
        while ((ch != myLiteralEnd) && (ch != '\n'))
        {
            // Used for skipping \" and \'
            if (ch == '\\')
            {
            	*myLiteral++ = ch;
	        myLiteralSize++;
	        ch = MyReadChar ();
            }    
            
            *myLiteral++ = ch;
            myLiteralSize++;
            if (myLiteralSize > stTokenValueMaxSize)
            {
            	MyResizeTokenValueBuffers (&myLiteral);
            }

	    ch = MyReadChar ();
        }
        if (ch == myLiteralEnd)
        {            
            *myLiteral++ = ch;
        }    
        else
        {
            MyPushChar ();
        }
        *myLiteral = EOS;
    }    
    // Check for one line comment
    else if ((ch == '/') && (stNextChar == '/'))
    {
        //
        // Read the one line comment
        //
        char	*myComment = stNextTokenValue;
        int	myCommentSize = 10; // Allow a little extra room
        int	commentColumn = 0;

        stNextToken = TOK_ONE_LINE_COMMENT;
        while (ch != '\n')
        {
            commentColumn++;
            *myComment++ = ch;
            myCommentSize++;
            if (myCommentSize > stTokenValueMaxSize)
            {
            	MyResizeTokenValueBuffers (&myComment);
            }
            ch = MyReadChar ();
        }
        MyPushChar ();
        *myComment = EOS;
    }    
    // Check for long comment
    else if ((ch == '/') && (stNextChar == '*'))
    {
        //
        // Read the long comment
        //
        char	*myComment = stNextTokenValue;
        int	myCommentSize = 10; // Allow a little extra room
        int 	myCommentColumn = 2;
        int 	myStartOfCommentColumn = stStartOfNonWhitespaceColumn;
                
        *myComment++ = ch;		  // Add the '/'
        *myComment++ = MyReadChar ();     // Read and add the '*'

        ch = MyReadChar ();
        while ((ch != EOF_MARKER) && ((ch != '*') || (stNextChar != '/')))
        {
            // Convert the tabs in the comments into spaces
            if (ch == ' ')
            {
                int myCommentColumnPosition = stNextColumn - stNextTokenColumn -
                    myStartOfCommentColumn;
                while (myCommentColumn < myCommentColumnPosition)
                {
                    myCommentColumn++;
                    *myComment++ = ' ';
	            myCommentSize++;
	            if (myCommentSize > stTokenValueMaxSize)
	            {
	            	MyResizeTokenValueBuffers (&myComment);
	            }
                }
            }
            else if (ch == '\n')
            {
                myCommentColumn = 0;
                *myComment++ = ch;
	        myCommentSize++;
            }
            else    
            {    
                myCommentColumn++;
                *myComment++ = ch;
	        myCommentSize++;
            }    
            if (myCommentSize > stTokenValueMaxSize)
            {
            	MyResizeTokenValueBuffers (&myComment);
            }
            ch = MyReadChar ();
        }
        
        *myComment++ = ch;               // Add the '*'
        *myComment++ = MyReadChar ();    // Read and add the '/'
        
	*myComment = EOS;        
	
        if (strchr (stNextTokenValue, '\n') == NULL)
        {
            stNextToken = TOK_ONE_LINE_COMMENT;
        }
        else
        {
            stNextToken = TOK_LONG_COMMENT;
        }        
    }    
    // Failed all others, read miscellaneous non-identifier
    else
    {
        //
        // Read the operator
        //
        char *myOperator = stNextTokenValue;

	*myOperator++ = ch;        
        ch = MyReadChar ();
	*myOperator++ = ch;
	*myOperator = EOS;
	
        while ((ch != EOS) && (MyIsPredefToken (stNextTokenValue) != TOK_NONE))
        {
            ch = MyReadChar ();
	    *myOperator++ = ch;
	    *myOperator = EOS;
        }
	*(myOperator - 1) = EOS;
        MyPushChar ();
        
        stNextToken = MyIsPredefToken (stNextTokenValue);
        
        // If not recognized, then it's a regular token
        if (stNextToken == TOK_NONE)
        {
            stNextToken = TOK_IDENTIFIER;
        }    
    }
    
    return (stCurrentToken);
} // MyReadToken


/************************************************************************/
/* MyRemoveIndent 							*/
/************************************************************************/
static void	MyRemoveIndent (Indent pmIndentType)
{
    // Handle BRACE_INDENT, PAREN_INDENT, BRACKET_INDENT, 
    //     IN_STATEMENT_INDENT
    if (stIndentStackHead == NULL)
    {
        return;
    }
        
    if (pmIndentType == BRACE_INDENT)
    {
        // Removing a BRACE indent should remove any open parentheses
        // or brackets, the in-statement indentation, the switch
        // indentation and the do-waiting-for while if applicable.
        
        // Remove any PAREN_INDENT, BRACKET_INDENT, IN_STATEMENT and
        // SWITCH_INDENT indents at the top of the indent stack.
        while ((stIndentStackHead != NULL) &&
            ((stIndentStackHead -> indent == PAREN_INDENT) ||
             (stIndentStackHead -> indent == BRACKET_INDENT) ||
             (stIndentStackHead -> indent == SWITCH_INDENT) ||
             (stIndentStackHead -> indent == DO_INDENT) ||
             (stIndentStackHead -> indent == IN_STATEMENT_INDENT)))
        {
            IndentStackPtr	myToDelete = stIndentStackHead;
            stIndentStackHead = stIndentStackHead -> next;
            free (myToDelete);
        }
    }
    if ((pmIndentType == BLOCK_INDENT) || (pmIndentType == ONE_BLOCK_INDENT))
    {
        // Removing a BLOCK indent should remove any open 
        // parentheses or brackets and the in-statement indentation.
        
        // Remove any PAREN_INDENT, BRACKET_INDENT, IN_STATEMENT indents
        // at the top of the indent stack.
        while ((stIndentStackHead != NULL) &&
            ((stIndentStackHead -> indent == PAREN_INDENT) ||
             (stIndentStackHead -> indent == BRACKET_INDENT) ||
             (stIndentStackHead -> indent == IN_STATEMENT_INDENT)))
        {
            IndentStackPtr	myToDelete = stIndentStackHead;
            stIndentStackHead = stIndentStackHead -> next;
            free (myToDelete);
        }
        
        if (pmIndentType == BLOCK_INDENT)
        {
            // It should then remove any number of BLOCK_INDENT and IF_INDENT
            // indents
            while ((stIndentStackHead != NULL) &&
                ((stIndentStackHead -> indent == BLOCK_INDENT) ||
                 (stIndentStackHead -> indent == IF_INDENT)))
            {
            	IndentStackPtr	myToDelete = stIndentStackHead;
            	stIndentStackHead = stIndentStackHead -> next;
            	free (myToDelete);
            }
        }
        else
        {
            // This will remove one block indent at the bottom.
            pmIndentType = BLOCK_INDENT;
        }    
    }
    else if (pmIndentType == PAREN_INDENT)
    {
        // We've reached a closing bracket. 
        // Get rid of any open parentheses.

        // Remove any BRACKET_INDENT indents at the top of the indent stack.
        while ((stIndentStackHead != NULL) &&
            (stIndentStackHead -> indent == BRACKET_INDENT))
        {
            IndentStackPtr	myToDelete = stIndentStackHead;
            stIndentStackHead = stIndentStackHead -> next;
            free (myToDelete);
        }
    }
    else if (pmIndentType == BRACKET_INDENT)
    {
        // We've reached a closing bracket. 
        // Get rid of any open parentheses.

        // Remove any PAREN_INDENT indents at the top of the indent stack.
        while ((stIndentStackHead != NULL) &&
            (stIndentStackHead -> indent == PAREN_INDENT))
        {
            IndentStackPtr	myToDelete = stIndentStackHead;
            stIndentStackHead = stIndentStackHead -> next;
            free (myToDelete);
        }
    }
    else if ((pmIndentType == IN_STATEMENT_INDENT) ||
        (pmIndentType == SWITCH_INDENT))
    {
        // We've reached the end of a statement or a switch statement. 
        // Get rid of any open parentheses or brackets.

        // Remove any PAREN_INDENT or BRACKET_INDENT indents
        // at the top of the indent stack.
            while ((stIndentStackHead != NULL) &&
                ((stIndentStackHead -> indent == PAREN_INDENT) ||
                 (stIndentStackHead -> indent == BRACKET_INDENT)))
        {
            IndentStackPtr	myToDelete = stIndentStackHead;
            stIndentStackHead = stIndentStackHead -> next;
            free (myToDelete);
        }
    }                
    else if (pmIndentType == IF_INDENT)
    {
        // If we've reached and else, get rid of any open parentheses,
        // brackets or in-statements

        // Remove any PAREN_INDENT or BRACKET_INDENT indents
        // at the top of the indent stack.
        while ((stIndentStackHead != NULL) &&
            ((stIndentStackHead -> indent == PAREN_INDENT) ||
             (stIndentStackHead -> indent == IN_STATEMENT_INDENT) ||
             (stIndentStackHead -> indent == BLOCK_INDENT) ||
             (stIndentStackHead -> indent == BRACKET_INDENT)))
        {
            IndentStackPtr	myToDelete = stIndentStackHead;
            stIndentStackHead = stIndentStackHead -> next;
            free (myToDelete);
        }
    }                
    
    if ((stIndentStackHead != NULL) && 
        (stIndentStackHead -> indent == pmIndentType))
    {
        IndentStackPtr	myToDelete = stIndentStackHead;
        stIndentStackHead = stIndentStackHead -> next;
        free (myToDelete);
    }    
} // MyRemoveIndent


/************************************************************************/
/* MyResizeTokenValueBuffers						*/
/*									*/
/* Resize the token value buffers to fit a new token.			*/
/************************************************************************/
static void	MyResizeTokenValueBuffers (char **pmNewTokenValuePtr)
{
    int	myOffset;
    
    if (stTokenValueMaxSize > 50000) 
    {
    	stTokenValueMaxSize = stTokenValueMaxSize + 50000;
    }
    else 
    {
    	stTokenValueMaxSize = stTokenValueMaxSize * 2;
    }
    
    myOffset = *pmNewTokenValuePtr - stNextTokenValue;
    
    stPrevTokenValue = realloc (stPrevTokenValue, stTokenValueMaxSize);
    if (stPrevTokenValue == NULL)
    {
    	// TW Crash
    	return;
    }
    
    stCurrentTokenValue = realloc (stCurrentTokenValue, stTokenValueMaxSize);
    if (stCurrentTokenValue == NULL)
    {
    	// TW Crash
    	return;
    }

    stNextTokenValue = realloc (stNextTokenValue, stTokenValueMaxSize);
    if (stNextTokenValue == NULL)
    {
    	// TW Crash
    	return;
    }
    
    stPrevNonWhitespaceTokenValue = realloc (stPrevNonWhitespaceTokenValue, 
    					     stTokenValueMaxSize);
    if (stPrevNonWhitespaceTokenValue == NULL)
    {
    	// TW Crash
    	return;
    }
    
    *pmNewTokenValuePtr = stNextTokenValue + myOffset;
} // MyResizeTokenValueBuffers


/************************************************************************/
/* MyTokenIsIdentifier							*/
/*									*/
/* Skip over whitespace to the end of line and read end of line		*/
/************************************************************************/
static BOOL	MyTokenIsIdentifier (int pmToken)
{
    if ((pmToken == TOK_IDENTIFIER) || (pmToken == TOK_CASE) ||
        (pmToken == TOK_FOR) || (pmToken == TOK_WHILE) ||
        (pmToken == TOK_DO) || (pmToken == TOK_IF) ||
        (pmToken == TOK_ELSE) || (pmToken == TOK_SWITCH))
    {
        return TRUE;
    }
    
    return FALSE;
} // MyTokenIsIdentifier


/************************************************************************/
/* MySkipNewLine							*/
/************************************************************************/
static void 	MySkipNewLine (int pmEchoComments)
{
    if ((pmEchoComments == ECHO_COMMENTS) ||
        (pmEchoComments == ECHO_COMMENTS_KEEP_LOCATION))
    {
        if (stNextToken == TOK_ONE_LINE_COMMENT)
        {
            MyReadToken ();
            if (pmEchoComments == ECHO_COMMENTS)
            {
                MyOutputSpace ();
            }
            else
            {
                if (stOutputColumn == -1)
                {
                    stOutputColumn = 0;
                }    
                do
                {
                    MyWriteSpace ();
                } while (stOutputColumn < stCurrentTokenColumn);
            }        
            MyOutputString (stCurrentTokenValue);
        }
    }    
            
    if (stNextToken == TOK_NEWLINE)
    {
        MyReadToken ();
    }        
} // MySkipNewLine


/************************************************************************/
/* MyWriteChar								*/
/************************************************************************/
static void	MyWriteChar (char pmChar)
{
    // There's no space to write the character
    if (!stIndentSuccess)
    {
    	return;
    }
    
    if (stResultBufPtr + 1 >= stResultEndBufPtr)
    {
        // Not enough space in buffer, increase size of array
        char	*myPtr;
        
        stResultBufferSize += BUFFER_ADJUSTMENT;
        myPtr = realloc (stResultBuffer, stResultBufferSize);
        if (myPtr == NULL)
        {
    	    EdGUI_Message1 (stStatusWindow, 0, IDS_INDENT_FAILED_TITLE,
    	    	IDS_INDENT_OUT_OF_MEM);
	    stIndentSuccess = FALSE;
	    free (stResultBuffer);
	    stResultBufPtr = NULL;
	    return;
        }
        stResultBufPtr = stResultBufPtr - stResultBuffer + myPtr;
        stResultEndBufPtr = stResultEndBufPtr - stResultBuffer + myPtr;
        stResultBuffer = myPtr;
    }
    
    // Get rid of trailing whitespace if we're outputting a newline
    if (pmChar == '\n')
    {
        char	*tmpBufPtr = stResultBufPtr - 1;
        
        while ((tmpBufPtr > stResultBuffer) && 
               ((*tmpBufPtr == ' ') || (*tmpBufPtr == '\t')))
        {
            tmpBufPtr--;
        }
        stResultBufPtr = tmpBufPtr + 1;
        stOutputColumn = -1;
    }
    
    // Set stOutputColumn
    if (stOutputColumn != -1)
    {
        stOutputColumn++;
    }
    else
    {
        if ((pmChar != '\n') && (pmChar != ' ') && (pmChar != '\t'))
        {
            stOutputColumn = 1;
        }
    }        
    
    // Add the character to the result buffer
    *stResultBufPtr++ = pmChar;
} // MyWriteChar


/************************************************************************/
/* MyWriteSpace								*/
/************************************************************************/
static void	MyWriteSpace (void)
{
    MyWriteChar (' ');
} // MyWriteSpace


/************************************************************************/
/* MyWriteString							*/
/************************************************************************/
static void	MyWriteString (const char *pmString)
{
    UINT	cnt;
    
    // There's no space to write the character
    if (!stIndentSuccess)
    {
    	return;
    }
    
    if (stResultBufPtr + strlen (pmString) >= stResultEndBufPtr)
    {
        // Not enough space in buffer, increase size of array
        char	*myPtr;
        
        stResultBufferSize += BUFFER_ADJUSTMENT;
        myPtr = realloc (stResultBuffer, stResultBufferSize);
        if (myPtr == NULL)
        {
    	    EdGUI_Message1 (stStatusWindow, 0, IDS_INDENT_FAILED_TITLE,
    	    	IDS_INDENT_OUT_OF_MEM);
	    stIndentSuccess = FALSE;
	    free (stResultBuffer);
	    stResultBufPtr = NULL;
	    return;
        }
        stResultBufPtr = stResultBufPtr - stResultBuffer + myPtr;
        stResultEndBufPtr = stResultEndBufPtr - stResultBuffer + myPtr;
        stResultBuffer = myPtr;
    }
    
    // Set stOutputColumn
    if (stOutputColumn != -1)
    {
        stOutputColumn += strlen (pmString);
    }
    else
    {
        stOutputColumn = strlen (pmString);
    }        
    
    // Add the characters to the result buffer
    for (cnt = 0 ; cnt < strlen (pmString) ; cnt++)
    {
        *stResultBufPtr++ = pmString [cnt];
    }    
} // MyWriteString


