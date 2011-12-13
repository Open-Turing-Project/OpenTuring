/************/
/* edtext.c */
/************/

/*******************/
/* System includes */
/*******************/
#include <windows.h>
#include <crtdbg.h>

/****************/
/* Self include */
/****************/
#include "edtext.h"
#if !defined (CONSOLE) && !defined (TPROLOG)
    #include "edkeywords.in"
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)

/******************/
/* Other includes */
/******************/
#if defined (CONSOLE)
    #include "jsmain.h"
#elif defined (TPROLOG)
    #include "edfail.h"
    #include "..\..\turing\src\tprologdialogs.h"
#else
    #include "edfail.h"
    #include "edjava.h"
    #include "edtur.h"

    #include "eddialogs.h"
#endif // #ifndef CONSOLE

/**********/
/* Macros */
/**********/
#ifdef CONSOLE
    #define EdFail_Fatal	JS_ErrorMessage
#endif // #ifdef CONSOLE

/*************/
/* Constants */
/*************/
#define INITIAL_TEXT_BUFFER_SIZE	4000
#define INITIAL_LINE_STARTS_BUFFER_SIZE	1000
#define LINE_BUFFER_SLACK		300
#define LINE_STARTS_SLACK		25
#define TEXT_BUFFER_SIZE_INCREMENT	4000
#define LINE_STARTS_BUFFER_SIZE_INCREMENT	1000

#define DO_NOT_USE			-99

#if !defined (CONSOLE) && !defined (TPROLOG)
    #define KEYWORD_TABLE_SIZE		1009

    // return codes from MyClassifySection
    #define CLASSIFY_CHANGED_THIS_LINE	1
    #define CLASSIFY_CHANGE_MULTIPLE_LINES	2

    // Character on the keyboard that does a forward delete
    #define FORWARD_DELETE_CHAR		0x7F

    // Used by EdText_SelectionComment
    #define MAX_INDENT			999999

    // Size of the CRC Table array
    #define CRC_TABLE_SIZE			256
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)

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

// Used by Structure Completion
#define NAMED_STRUCTURE_KEYWORDS	 8
#define BEGIN_KEYWORD			17
#define END_KEYWORD			18

#define CMPLT_NO_IDENT_FOUND		81
#define CMPLT_KEYWORD_END		82
#define CMPLT_KEYWORD_FOUND		83
#define CMPLT_KEYWORD_NOT_FOUND		84
#define CMPLT_INDENT_TOO_DEEP		85
#define SPACES_250 "                                                  " \
		   "                                                  " \
		   "                                                  " \
		   "                                                  " \
		   "                                                  "

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
static BYTE	stJavaCharTags [] = 
    {CH_ERR, CH_ERR, CH_ERR, CH_ERR, CH_ERR, CH_ERR, CH_ERR, CH_ERR, //   0-  7
     CH_ERR, CH_ERR, CH_WSP, CH_ERR, CH_ERR, CH_ERR, CH_ERR, CH_ERR, //   8- 15
     CH_ERR, CH_ERR, CH_ERR, CH_ERR, CH_ERR, CH_ERR, CH_ERR, CH_ERR, //  16- 23
     CH_ERR, CH_ERR, CH_ERR, CH_ERR, CH_ERR, CH_ERR, CH_ERR, CH_ERR, //  24- 31
     // Differs from Java and Turing (Turing identifies %)
     CH_WSP, CH_NRM, CH_DQT, CH_NRM, CH_NRM, CH_NRM, CH_NRM, CH_SQT, //  32- 39
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
     // Differs from Java and Turing (Java identifies open/close brace)
     CH_IDS, CH_IDS, CH_IDS, CH_OBR, CH_NRM, CH_CBR, CH_NRM, CH_ERR, // 120-127

     CH_IDS, CH_ERR, CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, // 128-135
     CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_ERR, CH_IDS, CH_ERR, // 136-143
     CH_ERR, CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, // 144-151
     CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_ERR, CH_IDS, CH_IDS, // 152-159
     CH_ERR, CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, // 160-167
     CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, // 168-175
     CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, // 176-183
     CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, // 184-191
     CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, // 192-199
     CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, // 200-207
     CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, // 208-215
     CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, // 216-223
     CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, // 224-231
     CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, // 232-239
     CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, // 240-247
     CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS, CH_IDS};// 248-255

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

static BYTE	stAcceptableChar [] = 
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   //   0- 15
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   //  16- 31
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,   //  32- 47
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,   //  48- 63
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,   //  64- 79
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,   //  80- 95
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,   //  96-111
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,   // 112-127
     1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0,   // 128-143
     0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1,   // 144-159
     0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,   // 160-175
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,   // 176-191
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,   // 192-207
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,   // 208-223
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,   // 224-239
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};  // 240-255

// Keywords used in structure completion
static char		*stCompletionKeywords [] = 
    {"body", "proc", "procedure", "fcn", "function", "module", 
     "monitor", "class", "process",
     "if", "for", "loop", "record", "case", "begin", "handler", "union",
     "begin",
     "end"};

static char		*stCompletionModifiers [] =
{"body", "pervasive"};
/********************/
/* Static variables */
/********************/
// Properties of the Text class
static EdTextOnly_Properties	stProperties;
static BOOL			stPropertiesChanged;

#if !defined (CONSOLE) && !defined (TPROLOG)
    // Size of the keywords table
    static int	stNumJavaKeywords = sizeof (stJavaKeywords) / sizeof (char *);
    static int	stNumTuringKeywords = 
    				sizeof (stTuringKeywords) / sizeof (char *);
    static int	stNumJavaPredefs = sizeof (stJavaPredefs) / sizeof (char *);
    static int	stNumTuringPredefs = sizeof (stTuringPredefs) / sizeof (char *);
    static int	stJavaPredefsTableSize, stTuringPredefsTableSize;

    // Table used to calculate CRC's
    static UINT	stCRCTable [CRC_TABLE_SIZE];

    // Tables used by the Text class
    static char	**stJavaKeywordTable;
    static char	**stTuringKeywordTable;
    static char	**stJavaPredefTable;
    static char	**stTuringPredefTable;

    static int	stNumCompletionKeywords =
        			sizeof (stCompletionKeywords) / sizeof (char *);
	static int	stNumCompletionModifiers =
        			sizeof (stCompletionModifiers) / sizeof (char *);
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)
    
/******************************/
/* Static callback procedures */
/******************************/

/*********************/
/* Static procedures */
/*********************/
static void	MyAddNewLineIfNecessary (TextPtr pmTextPtr, int pmSelAdjust);
#if !defined (CONSOLE) && !defined (TPROLOG)
static void	MyAddTextState (TextPtr pmTextPtr, const char *pmString);
    static void	MyClassifyText (TextPtr pmTextPtr);
    static int	MyClassifySection (TextPtr pmTextPtr, int pmStartPos);
    static void	MyClearMarkLineBuffer (TextPtr pmTextPtr);
    static void	MyCompleteStructure (TextPtr pmTextPtr, int pmLineNum,
				     char *pmEndStruct, int pmIndent,
				     int *pmAction);
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)	
static int	MyConvertTabsAndNewLines (char **pmDestBuffer, int *pmDestSize, 
					  const char *pmSrcBuffer, 
					  int pmSrcSize, int *pmNumLines, 
					  int *pmNumChars, int pmStartCol);
#if !defined (CONSOLE) && !defined (TPROLOG)					  
    static void	MyCreateCRCTable ();					  
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)	
static BOOL	MyEnsureLineStartsSpace (TextPtr pmTextPtr, int pmLinesNeeded);
static BOOL	MyEnsureTextBufferSpace (TextPtr pmTextPtr, int pmSpaceNeeded);
static void	MyFindCharLocation (TextPtr pmTextPtr, int pmLoc, int *pmLine, 
				    int *pmCol);
static void	MyFindLocationPos (TextPtr pmTextPtr, int pmLine, int pmCol, 
				   int *pmLoc, BOOL *pmBeyondEnd);
#if !defined (CONSOLE) && !defined (TPROLOG)					  
    static void	MyFreeRedoBuffers (TextPtr pmTextPtr);
    static void	MyFreeTextBuffers (TextPtr pmTextPtr);
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)	
static void	MyFreeTextMemory (TextPtr pmTextPtr, BOOL pmEraseUndo);
#if !defined (CONSOLE) && !defined (TPROLOG)	
    static int	MyGetTuringStructure (TextPtr pmTextPtr, int pmLineNum,
				      char *pmEndStruct, int *pmIndent);
    static int	MyHash (char *pmString, int pmTableSize);
    static BOOL	MyIsJavaKeyword (char *pmId);
    static BOOL	MyIsJavaPredef (char *pmId);
    static BOOL	MyIsLineBlanks (TextPtr pmTextPtr, int pmLineNum);
    static BOOL	MyIsTuringKeyword (char *pmId);
    static BOOL	MyIsTuringPredef (char *pmId);
    static int	MyKeyBackspace (TextPtr pmTextPtr, char **pmDeletedChars);
    static int	MyKeyNormal (TextPtr pmTextPtr, char pmChar);
    static int	MyKeyReturn (TextPtr pmTextPtr);
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)	
static int	MyLineStartsAdjustForDelete (TextPtr pmTextPtr, int pmFromPos,
					     int pmToPos);
static int	MyLineStartsAdjustForInsert (TextPtr pmTextPtr, int pmFromPos,
					     int pmInsertSize, 
					     int pmInsertLines,
					     int pmSelAdjust);
#if !defined (CONSOLE) && !defined (TPROLOG)					  
    static void	MyMarkText (TextPtr pmTextPtr);
    static int	MyNotifyLinesDeleted (TextPtr pmTextPtr, int pmFromLine, 
				      int pmToLine, BOOL pmDelFromEOL);
    static int	MyNotifyLinesInserted (TextPtr pmTextPtr, int pmWhere, 
				       int pmNumLines, 
				       BOOL pmInsertAtBeginOfLine,
				       BOOL pmInsertAtEndOfLine,
				       BOOL pmInsertedTextEndsWithNL);
    static int	MyNotifyLinesModified (TextPtr pmTextPtr, int pmFromLine, 
				       int pmToLine);
    static void	MyNotifyMoveBreaks (TextPtr pmTextPtr, int pmWhere, 
				    int pmLinesMoved);
    static void	MyNotifyMoveErrors (TextPtr pmTextPtr, int pmWhere, 
				    int pmLinesMoved);
    static void	MyNotifyMoveMarks (TextPtr pmTextPtr, int pmWhere, 
				   int pmLinesMoved);
    static BOOL	MyNotifyRemoveBreaks (TextPtr pmTextPtr, int pmFromLine, 
				      int pmToLine);
    static BOOL	MyNotifyRemoveErrors (TextPtr pmTextPtr, int pmFromLine, 
				      int pmToLine);
    static BOOL	MyNotifyRemoveMarks (TextPtr pmTextPtr, int pmFromLine, 
				     int pmToLine);
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)
static int	MyRecalculateLongestLine (TextPtr pmTextPtr);
static int	MyReturnStatus (TextPtr pmTextPtr, BOOL pmWasSelection);
#if !defined (CONSOLE) && !defined (TPROLOG)
    static int	MySetDirty (TextPtr pmTextPtr);
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)
static void	MyVerifyLineDB (TextPtr pmTextPtr);


/***********************/
/* External procedures */
/***********************/

#if !defined (CONSOLE) && !defined (TPROLOG)
/************************************************************************/
/* EdText_Init								*/
/************************************************************************/
BOOL	EdText_Init (void)
{
    int	cnt, myHash;
    int myCollision = 0;    
    
    //
    // Initialize each of the submodules that need initialization 
    // 
    
    if (!EdJava_Init ())
    {
    	return FALSE;
    }

    if (!EdTur_Init ())
    {
    	return FALSE;
    }

    //
    // Initialize the EdText module
    //
        
    // Create the Java hash tables.
    stJavaPredefsTableSize = (int) (stNumJavaPredefs * 1.3);
    stTuringPredefsTableSize = (int) (stNumTuringPredefs * 1.3);

    stJavaKeywordTable = (char **) calloc (KEYWORD_TABLE_SIZE, sizeof (char *));
    if (stJavaKeywordTable == NULL)
    {
    	EdFail_Warn (IDS_OUTOFMEMORY, __FILE__, __LINE__, 0);
    	return FALSE;
    }
    
    for (cnt = 0 ; cnt < stNumJavaKeywords ; cnt++)
    {
    	myHash = MyHash (stJavaKeywords [cnt], KEYWORD_TABLE_SIZE);
    	while (stJavaKeywordTable [myHash] != NULL)
    	{
    	    myHash = (myHash + 1) % KEYWORD_TABLE_SIZE;
    	    myCollision++;
    	}
    	stJavaKeywordTable [myHash] = stJavaKeywords [cnt];
    }

    stJavaPredefTable = (char **) calloc ((int) (stNumJavaPredefs * 1.5), 
    						 sizeof (char *));
    if (stJavaPredefTable == NULL)
    {
    	EdFail_Warn (IDS_OUTOFMEMORY, __FILE__, __LINE__, 0);
    	return FALSE;
    }

    for (cnt = 0 ; cnt < stNumJavaPredefs ; cnt++)
    {
    	myHash = MyHash (stJavaPredefs [cnt], stJavaPredefsTableSize);
    	while (stJavaPredefTable [myHash] != NULL)
    	{
    	    myHash = (myHash + 1) % stJavaPredefsTableSize;
    	    myCollision++;
    	}
    	stJavaPredefTable [myHash] = stJavaPredefs [cnt];
    }

    // Create the Turing hash tables.
    stTuringKeywordTable = (char **) calloc (KEYWORD_TABLE_SIZE, 
    					     sizeof (char *));
    if (stTuringKeywordTable == NULL)
    {
    	EdFail_Warn (IDS_OUTOFMEMORY, __FILE__, __LINE__, 0);
    	return FALSE;
    }
    
    for (cnt = 0 ; cnt < stNumTuringKeywords ; cnt++)
    {
    	myHash = MyHash (stTuringKeywords [cnt], KEYWORD_TABLE_SIZE);
    	while (stTuringKeywordTable [myHash] != NULL)
    	{
    	    myHash = (myHash + 1) % KEYWORD_TABLE_SIZE;
    	    myCollision++;
    	}
    	stTuringKeywordTable [myHash] = stTuringKeywords [cnt];
    }

    stTuringPredefTable = (char **) calloc (stTuringPredefsTableSize, sizeof (char *));
    if (stTuringPredefTable == NULL)
    {
    	EdFail_Warn (IDS_OUTOFMEMORY, __FILE__, __LINE__, 0);
    	return FALSE;
    }

    for (cnt = 0 ; cnt < stNumTuringPredefs ; cnt++)
    {
    	myHash = MyHash (stTuringPredefs [cnt], stTuringPredefsTableSize);
    	while (stTuringPredefTable [myHash] != NULL)
    	{
    	    myHash = (myHash + 1) % stTuringPredefsTableSize;
    	    myCollision++;
    	}
    	stTuringPredefTable [myHash] = stTuringPredefs [cnt];
    }

    // Initialize the CRC Table
    MyCreateCRCTable ();
        
    return TRUE;
} // EdText_Init


/************************************************************************/
/* EdText_Finalize							*/
/************************************************************************/
void	EdText_Finalize (void)
{
    // Mostly here to eliminate memory leak messages
    EdJava_Finalize ();
    EdTur_Finalize ();
    
    if (stTuringPredefTable != NULL)
    {
    	free (stTuringPredefTable);
    }
    if (stTuringKeywordTable != NULL)
    {
    	free (stTuringKeywordTable);
    }
    if (stJavaPredefTable != NULL)
    {
    	free (stJavaPredefTable);
    }
    if (stJavaKeywordTable != NULL)
    {
    	free (stJavaKeywordTable);
    }
} // EdText_Finalize
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)


/************************************************************************/
/* EdText_PropertiesSet							*/
/************************************************************************/
void	EdText_PropertiesSet (EdText_Properties pmProperties)
{
#if !defined (CONSOLE) && !defined (TPROLOG)
    EdJava_PropertiesSet ();
    EdTur_PropertiesSet ();
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)
    	
    stProperties = pmProperties.myProperties;
    stPropertiesChanged = TRUE;
} // EdText_PropertiesSet

	
#if !defined (CONSOLE) && !defined (TPROLOG)
/************************************************************************/
/* EdText_PropertiesImplementChanges					*/
/************************************************************************/
void	EdText_PropertiesImplementChanges (TextPtr pmTextPtr)
{
    if (stPropertiesChanged)
    {
    	// Make changes for the module
    	EdJava_PropertiesImplementChanges ();
	EdTur_PropertiesImplementChanges ();
	
    	stPropertiesChanged = FALSE;
    }

    // ... 
} // EdText_PropertiesImplementChanges


/************************************************************************/
/* EdText_ClassifyText							*/
/************************************************************************/
void	EdText_ClassifyText (TextPtr pmTextPtr)
{
    // Process text.  Set the colours
    MyClassifyText (pmTextPtr);

    // Create the mark menu
    MyClearMarkLineBuffer (pmTextPtr);
    MyMarkText (pmTextPtr);
} // EdText_ClassifyText
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)

    
/************************************************************************/
/* EdText_Create							*/
/************************************************************************/
TextPtr	EdText_Create ()
{
    // Allocate the TextRecord
    TextPtr	myPtr = (TextPtr) malloc (sizeof (TextRecord));
    
    // Allocate the elements in the TextRecord
    myPtr -> text = (char *) malloc (INITIAL_TEXT_BUFFER_SIZE);
    // TW Check for null
#if !defined (CONSOLE) && !defined (TPROLOG)
    myPtr -> textType = (byte *) malloc (INITIAL_TEXT_BUFFER_SIZE);
    // TW Check for null
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)
    myPtr -> lineStarts = (int *) malloc (INITIAL_LINE_STARTS_BUFFER_SIZE * 
        sizeof (int));
    // TW Check for null

#if !defined (CONSOLE) && !defined (TPROLOG)
    myPtr -> errorLine = NULL;
    myPtr -> markLine = NULL;
    myPtr -> breakLine = NULL;
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)
    
    EdText_InitializeTextRecord (myPtr);    
    return myPtr;
} // EdText_Create


/************************************************************************/
/* EdText_Dispose							*/
/************************************************************************/
void	EdText_Dispose (TextPtr pmTextPtr)
{
    MyFreeTextMemory (pmTextPtr, ERASE_UNDO);
    
    free (pmTextPtr);
} // EdText_Dispose


#if !defined (CONSOLE) && !defined (TPROLOG)
/************************************************************************/
/* EdText_AddError							*/
/************************************************************************/
void	EdText_AddError (TextPtr pmTextPtr, ErrorPtr pmError)
{
    ErrorLinePtr	myNewError, myError, myPrevError;
    
    myNewError = (ErrorLinePtr) malloc (sizeof (ErrorLine));
    if (myNewError == NULL)
    {
    	// TW Warn
    	return;
    }
    
    myNewError -> errorPtr = pmError;
    
    // Add it to the where it fits
    myError = pmTextPtr -> errorLine;
    myPrevError = NULL;
    
    while ((myError != NULL) && 
        ((myError -> errorPtr -> line < pmError -> line) ||
         ((myError -> errorPtr -> line == pmError -> line) && 
          (myError -> errorPtr -> tokenPos < pmError -> tokenPos))))
    {
    	myPrevError = myError;
    	myError = myError -> next;
    }
    
    if (myPrevError == NULL)
    {
    	pmTextPtr -> errorLine = myNewError;
    }
    else
    {
    	myPrevError -> next = myNewError;
    }
    myNewError -> next = myError;
    
    pmTextPtr -> currentError = NULL;
} // EdText_AddError


/************************************************************************/
/* EdText_CalculateCRC							*/
/*									*/
/* This function calculates the CRC for the text.			*/
/************************************************************************/
extern UINT	EdText_CalculateCRC (TextPtr pmTextPtr)
{
    char	*myPtr = pmTextPtr -> text;
    char	*myEnd = myPtr + pmTextPtr -> textLen;
    UINT	myCRCValue = 0xffffffff;
    int		myIndex;
    
    while (myPtr < myEnd)
    {
    	myIndex = (*myPtr ^ myCRCValue) & 0xff;
    	myCRCValue >>= 8;
    	myCRCValue ^= stCRCTable [myIndex];
    	myPtr++;
    }

    return myCRCValue;    	
} // EdText_CalculateCRC


/************************************************************************/
/* EdText_ClearAllBreakpoints						*/
/************************************************************************/
void	EdText_ClearAllBreakpoints (TextPtr pmTextPtr)
{
    BreakLinePtr	myNextBreakLine = NULL;
    BreakLinePtr        myBreakLine = pmTextPtr -> breakLine;
    BOOL		myAdd = TRUE;
    
    while (myBreakLine != NULL)
    {
    	myNextBreakLine = myBreakLine -> next;
    	free (myBreakLine);
    	myBreakLine = myNextBreakLine;
    }
    pmTextPtr -> breakLine = NULL;
} // EdText_ClearAllBreakpoints


/************************************************************************/
/* EdText_ClearErrors							*/
/************************************************************************/
void	EdText_ClearErrors (TextPtr pmTextPtr)
{
    ErrorLinePtr	myErrorLine, myNextErrorLine;
    
    myErrorLine = pmTextPtr -> errorLine;
    while (myErrorLine != NULL)
    {
    	myNextErrorLine = myErrorLine -> next;
    	free (myErrorLine);
    	myErrorLine = myNextErrorLine;
    }
    
    pmTextPtr -> errorLine = NULL;
    pmTextPtr -> currentError = NULL;
} // EdText_ClearErrors


/************************************************************************/
/* EdText_CompleteStructure						*/
/*									*/
/* This does structure completion.  Starting from the bottom of the 	*/
/* selection, it goes through looking for unclosed structures.  It	*/
/* then adds a line after the selection with a closing structure.	*/
/*									*/
/* Note that it is not rigorous.  It can make mistakes because it only	*/
/* looks at the first token in each line to determine the structures.	*/
/************************************************************************/
int	EdText_CompleteStructure (TextPtr pmTextPtr, int *pmAction)
{
    int		myFromLine, myFromCol, myToLine, myToCol; 
    char	myEndStruct [256];
    int		myLineNum, myTokenType, myDepth, myIndent;
    
    EdText_GetSelectionLinesCols (pmTextPtr, &myFromLine, &myFromCol, 
				  &myToLine, &myToCol);

    // Starting on the line that ends the selection
    myLineNum = myToLine;
    myDepth = 0;
    while (myLineNum >= 0)
    {
        if (pmTextPtr -> isJava)
        {
	    return CANT_COMPLETE_JAVA;
	}
	else if (pmTextPtr -> isTuring)
	{
    	    myTokenType = MyGetTuringStructure (pmTextPtr, myLineNum, 
					        myEndStruct, &myIndent);
	}
	if (myTokenType == CMPLT_INDENT_TOO_DEEP)
	{
    	    // A structure was indented too deeply to do a struct completion
	    return INDENT_TOO_DEEP;
	}
    	if (myTokenType == CMPLT_NO_IDENT_FOUND)
    	{
	    // A proc, fcn, class, etc that needs an identifier didn't have one
	    return NO_IDENT_FOUND;
	}
	if (myTokenType == CMPLT_KEYWORD_END)
	{
    	    myDepth++;
    	}
    	else if (myTokenType == CMPLT_KEYWORD_FOUND)
    	{
    	    if (myDepth == 0)
    	    {
		MyCompleteStructure (pmTextPtr, myLineNum, myEndStruct, 
				     myIndent, pmAction);

		// Verify line database, if requested
		if (stProperties.verifyLineDB)
		{
		    MyVerifyLineDB (pmTextPtr);
		}
		
		return STRUCTURE_COMPLETED;
    	    } // if (depth == 0)
    	    else
    	    {
    		myDepth--;
    	    } // else
	} // if (useful token)    	    
    	
    	myLineNum--;
    } // while
    
    return CANNOT_COMPLETE_STRUCTURE;
} // EdText_CompleteStructure
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)	


/************************************************************************/
/* EdText_GetSelectionLinesCols						*/
/*									*/
/* Note that the from position is always before the to position		*/
/* regardless of what the selection was.				*/
/************************************************************************/
void	EdText_GetSelectionLinesCols (TextPtr pmTextPtr, 
    int *pmFromLine, int *pmFromCol, int *pmToLine, int *pmToCol)
{
    if (!pmTextPtr -> locationOK)
    {
        MyFindCharLocation (pmTextPtr, pmTextPtr -> selFrom, 
            &pmTextPtr -> lastLineFrom, &pmTextPtr -> lastColFrom);
        MyFindCharLocation (pmTextPtr, pmTextPtr -> selTo, 
            &pmTextPtr -> lastLineTo, &pmTextPtr -> lastColTo);
    	pmTextPtr -> locationOK = TRUE;
    }

    *pmFromLine = pmTextPtr -> lastLineFrom;
    *pmFromCol = pmTextPtr -> lastColFrom;
    *pmToLine = pmTextPtr -> lastLineTo;
    *pmToCol = pmTextPtr -> lastColTo;

    if ((*pmFromLine == *pmToLine) && (*pmFromCol > *pmToCol))
    {
    	int	tmp;

    	tmp = *pmFromCol;
    	*pmFromCol = *pmToCol;
    	*pmToCol = tmp;
    }
    else if (*pmFromLine > *pmToLine)
    {
        int	tmp;

        tmp = *pmFromLine;
        *pmFromLine = *pmToLine;
        *pmToLine = tmp;
    	        
        tmp = *pmFromCol;
        *pmFromCol = *pmToCol;
        *pmToCol = tmp;
    }
} // EdText_GetSelectionLinesCols


/************************************************************************/
/* EdText_GetSelectionLocation						*/
/************************************************************************/
void	EdText_GetSelectionLocation (TextPtr pmTextPtr, int *pmLine, 
    int *pmCol)
{
    	
    if (!pmTextPtr -> locationOK)
    {
#ifdef MINISTRY_VERSION
    	// Added here to eliminate the problem of selection extending beyond
    	// the end of the program.  This was turned on for the 4.0.3 release.
    	// Comment this out the ifdef/endif for a Ministry version until this
    	// bug is fixed.
    	if (pmTextPtr -> selFrom > pmTextPtr -> textLen)
    	{
    	    pmTextPtr -> selFrom = pmTextPtr -> textLen;
    	}
    	if (pmTextPtr -> selTo > pmTextPtr -> textLen)
    	{
    	    pmTextPtr -> selTo = pmTextPtr -> textLen;
    	}
    	// End of special code.
#endif
    	
        MyFindCharLocation (pmTextPtr, pmTextPtr -> selFrom, 
            &pmTextPtr -> lastLineFrom, &pmTextPtr -> lastColFrom);
        MyFindCharLocation (pmTextPtr, pmTextPtr -> selTo, 
            &pmTextPtr -> lastLineTo, &pmTextPtr -> lastColTo);
    	pmTextPtr -> locationOK = TRUE;
    }

    *pmLine = pmTextPtr -> lastLineTo;
    *pmCol = pmTextPtr -> lastColTo;
} // EdText_GetSelectionLocation


#if !defined (CONSOLE) && !defined (TPROLOG)	
/************************************************************************/
/* EdText_Indent							*/
/************************************************************************/
BOOL	EdText_Indent (HWND pmWindowForStatus, TextPtr pmTextPtr)
{
    char	*myNewBuffer;
    int		myNewBufferLen, myNewTextLen;
    int		myLine, myCol;
    int		myLineLen, myLineNumber, cnt;
    char	*myPtr, *myEndPtr;
    
    // Debug Messages    
    EdFail_AddMessage (NORMAL, "Indent (1) - Text: %x", pmTextPtr);
    MyAddTextState (pmTextPtr, NULL);	    	       

    // Indent the file    
    if (pmTextPtr -> isJava)
    {
    	EdJava_Indent (pmWindowForStatus, pmTextPtr -> text, 
            pmTextPtr -> textLen, &myNewBuffer, &myNewBufferLen, &myNewTextLen);
    }
    else if (pmTextPtr -> isTuring)
    {
    	EdTur_Indent (pmWindowForStatus, pmTextPtr -> text, 
            pmTextPtr -> textLen, &myNewBuffer, &myNewBufferLen, &myNewTextLen);
    }
    else
    {
    	return FALSE;
    }       

    // If the indent failed, then return
    if (myNewBuffer == NULL)
    {
    	return FALSE;
    }
    
    // Get the current selection
    EdText_GetSelectionLocation (pmTextPtr, &myLine, &myCol);
    
    // Free up any allocated buffers.
    MyFreeTextMemory (pmTextPtr, DONT_ERASE_UNDO);
    
    // Set the new text buffer    
    pmTextPtr -> textBufferLen = myNewBufferLen;
    pmTextPtr -> text = myNewBuffer;
    pmTextPtr -> textLen = myNewTextLen;
    
    // Get the number of lines
    myPtr = myNewBuffer;
    myEndPtr = myPtr + myNewTextLen;
    pmTextPtr -> numLines = 0;
    while (myPtr < myEndPtr)
    {
    	if (*myPtr++ == '\n')
    	{
    	    (pmTextPtr -> numLines)++;
    	}
    }

    // Create and fill the line starts    
    pmTextPtr -> lineStartsBufferLen = pmTextPtr -> numLines + 
        LINE_BUFFER_SLACK;
    pmTextPtr -> lineStarts = 
        (int *) malloc (pmTextPtr -> lineStartsBufferLen * sizeof (int));

    pmTextPtr -> longestLine = 0;
    pmTextPtr -> longestLineLen = 0;
    pmTextPtr -> newLineAdded = FALSE;

    pmTextPtr -> lineStarts [0] = 0;
    myLineNumber = 1;
    for (cnt = 0 ; cnt < pmTextPtr -> textLen ; cnt++)
    {
    	if (pmTextPtr -> text [cnt] == '\n')
    	{
    	    pmTextPtr -> lineStarts [myLineNumber] = cnt + 1;
    	    myLineLen = cnt - pmTextPtr -> lineStarts [myLineNumber - 1];
	    if (myLineLen > pmTextPtr -> longestLineLen)
	    {
	    	pmTextPtr -> secondLongestLineLen = pmTextPtr -> longestLineLen;
	    	pmTextPtr -> longestLineLen = myLineLen;
	    	pmTextPtr -> longestLine = myLineNumber - 1;
	    }
	    else if (myLineLen > pmTextPtr -> secondLongestLineLen)
	    {
	    	pmTextPtr -> secondLongestLineLen = myLineLen;
	    }
    	    myLineNumber++;
    	}
    }
    pmTextPtr -> lineStarts [myLineNumber] = pmTextPtr -> textLen;

    if ((pmTextPtr -> textLen == pmTextPtr -> lastSavedTextLen) &&
        (EdText_CalculateCRC (pmTextPtr) == pmTextPtr -> lastSavedCRC))
    {
    	pmTextPtr -> isDirty = FALSE;
    }
    else
    {
    	pmTextPtr -> isDirty = TRUE;
    }
    pmTextPtr -> isChangedSinceLastRun = TRUE;
    
    pmTextPtr -> virtualCol = DO_NOT_USE;

    pmTextPtr -> locationOK = FALSE;
        	        
    // Allocate a new textType buffer
    pmTextPtr -> textType = (BYTE *) malloc (pmTextPtr -> textBufferLen);
    // TW Check for null

    // Sey the new selection
    if (myLine > pmTextPtr -> numLines)
    {
    	myLine = pmTextPtr -> numLines;
    }
    EdText_SetSelection (pmTextPtr, myLine, 0);
    
    // Process text.  Set the colours and make the mark menu.
    MyClassifyText (pmTextPtr);
    MyMarkText (pmTextPtr);
    
    MyAddTextState (pmTextPtr, "Indent (2)");
    
    // Verify line database, if requested
    if (stProperties.verifyLineDB)
    {
    	MyVerifyLineDB (pmTextPtr);
    }
    
    return TRUE;
} // EdText_Indent
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)	


/************************************************************************/
/* EdText_InsertText							*/
/*									*/
/* Inserts the text located by pmBuffer into the file at the selection	*/
/* point.  If pmClean is FALSE, the buffer may contain CRLF instead of	*/
/* newlines, TABs and non-legal characters.				*/
/************************************************************************/
int	EdText_InsertText (TextPtr pmTextPtr, const char *pmBuffer, 
			   int pmBufferSize, BOOL pmClean, int *pmLinesInserted)
{
    const char	*myBufferUsed, *myBufPtr;
    char	*myBufferAllocated, *myPtr;
    int		myBufferSize, myBufferAllocatedSize;
    int		myNumNewLines;
    int		myLine, myCol;
    BOOL	myInsertAtBeginningOfLine, myInsertAtEndOfLine;
    BOOL	myInsertedTextEndsWithNL;
    int		myResultCode, myOriginalSelFrom;
    int		cnt;
        
#if !defined (CONSOLE) && !defined (TPROLOG)
    // Debug Messages    
    EdFail_AddMessage (NORMAL, "Insert Text (1) - Text: %x - size: %d"
    		       " - clean: %d",
	    	       pmTextPtr, pmBufferSize, pmClean);
    MyAddTextState (pmTextPtr, NULL);
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)

    // Do insertion of text
    if (pmClean)
    {
    	myBufferUsed = pmBuffer;
    	myBufferSize = pmBufferSize;
    
    	// Count the number of newlines
    	myBufPtr = myBufferUsed;
    	myNumNewLines = 0;
    	
    	for (cnt = 0 ; cnt < myBufferSize ; cnt++)
    	{
    	    if (*myBufPtr == '\n')
    	    {
    	    	myNumNewLines++;
    	    }
    	    myBufPtr++;
        }
    }
    else
    {
    	myBufferSize = max (pmBufferSize + 200, 2048);
    	myBufferAllocated = malloc (myBufferSize);
    	if (myBufferAllocated == NULL)
    	{
    	    // TW Error no memory
    	}

	// Clean the text
	myBufferAllocatedSize = myBufferSize;
        EdText_GetSelectionLocation (pmTextPtr, &myLine, &myCol);
    	MyConvertTabsAndNewLines (&myBufferAllocated, &myBufferAllocatedSize, 
    			          pmBuffer, pmBufferSize, 
    			          &myNumNewLines, &myBufferSize, myCol);
    	myBufferUsed = myBufferAllocated;
    }

    // Set flag
    myInsertedTextEndsWithNL = (myBufferUsed [myBufferSize - 1] == '\n');
    
#if !defined (CONSOLE) && !defined (TPROLOG)
    // Debug messages
    EdFail_AddMessage (NORMAL, "Insert Text (2) - numNewLines: %d", 
    		       myNumNewLines);
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)
        
    // Set number of lines inserted
    if (pmLinesInserted != NULL)
    {
    	*pmLinesInserted = myNumNewLines;
    	if (!myInsertedTextEndsWithNL)
    	{
    	    (*pmLinesInserted)++;
    	}
    }
        
    // Make certain there's enough space    
    if (!MyEnsureTextBufferSpace (pmTextPtr, myBufferSize))
    {
    	return KEY_OUT_OF_MEMORY;
    }
    if (!MyEnsureLineStartsSpace (pmTextPtr, myNumNewLines))
    {
    	return KEY_OUT_OF_MEMORY;
    }
    
    
    // Get line number of current selection
    EdText_GetSelectionLocation (pmTextPtr, &myLine, &myCol);
    
    if (myNumNewLines == 0)
    {
	myResultCode = KEY_UPDATE_LINE;   	
    }
    else
    {
    	myResultCode = KEY_REDRAW_FROM_SELECTION;
    }

    // Are we inserting at the beginning of the line  
    myPtr = pmTextPtr -> text + pmTextPtr -> selFrom;
    if ((pmTextPtr -> selFrom == 0) || (*(myPtr - 1) == '\n'))
    {
        myInsertAtBeginningOfLine = TRUE;
    }
    else
    {
        myInsertAtBeginningOfLine = FALSE;
    }

    // Are we inserting at the end of the line  
    myPtr = pmTextPtr -> text + pmTextPtr -> selFrom;
    if ((pmTextPtr -> selFrom == pmTextPtr -> textLen) || 
        (*myPtr == '\n'))
    {
        myInsertAtEndOfLine = TRUE;
    }
    else
    {
        myInsertAtEndOfLine = FALSE;
    }

    // Move from the selection to the end of the buffer over myBufferSize
    // places and insert the buffer into the file at the selection point.
    myPtr = pmTextPtr -> text + pmTextPtr -> selFrom;
    memmove (myPtr + myBufferSize, myPtr, 
        pmTextPtr -> textLen - pmTextPtr -> selFrom);
    memmove (myPtr, myBufferUsed, myBufferSize);

#if !defined (CONSOLE) && !defined (TPROLOG)	
    // Do the same for the textType block
    myPtr = pmTextPtr -> textType + pmTextPtr -> selFrom;
    memmove (myPtr + myBufferSize, myPtr, 
        pmTextPtr -> textLen - pmTextPtr -> selFrom);
    for (cnt = 0 ; cnt < myBufferSize ; cnt++)
    {
        *myPtr++ = TEXT_NO_CLASSIFICATION;
    }
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)	
    
    // Fix the line starts array
    myResultCode |= MyLineStartsAdjustForInsert (pmTextPtr, 
        pmTextPtr -> selFrom, myBufferSize, myNumNewLines, myBufferSize);
    
#if !defined (CONSOLE) && !defined (TPROLOG)	
    // Adjust all the errors and marks
    MyNotifyLinesInserted (pmTextPtr, myLine, myNumNewLines, 
        myInsertAtBeginningOfLine, myInsertAtEndOfLine, 
        myInsertedTextEndsWithNL);
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)	
        
    // Advance the selection
    myOriginalSelFrom = pmTextPtr -> selFrom;          
    pmTextPtr -> selFrom += myBufferSize;
    pmTextPtr -> selTo += myBufferSize;
    pmTextPtr -> locationOK = FALSE;
    
#if !defined (CONSOLE) && !defined (TPROLOG)	
    // Set dirty flag
    myResultCode |= MySetDirty (pmTextPtr);
    
    // Colour the new text (will always redraw screen
    if (MyClassifySection (pmTextPtr, myOriginalSelFrom) == 
        CLASSIFY_CHANGE_MULTIPLE_LINES)
    {
    	myResultCode |= KEY_REDRAW_FROM_SELECTION;
    }
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)	
    
    if (pmBuffer != myBufferUsed)
    {
    	free (myBufferAllocated);
    }
    
#if !defined (CONSOLE) && !defined (TPROLOG)	
    MyAddTextState (pmTextPtr, "Insert Text (3)");
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)	

    // Verify line database, if requested
    if (stProperties.verifyLineDB)
    {
    	MyVerifyLineDB (pmTextPtr);
    }
    
    return myResultCode;
} // EdText_InsertText


/************************************************************************/
/* EdText_InitializeTextRecord						*/
/************************************************************************/
void	EdText_InitializeTextRecord (TextPtr pmTextPtr)
{
    pmTextPtr -> textBufferLen = INITIAL_TEXT_BUFFER_SIZE;
    pmTextPtr -> textLen = 0;
    pmTextPtr -> numLines = 0;
    pmTextPtr -> longestLine = 0;
    pmTextPtr -> longestLineLen = 0;
    pmTextPtr -> secondLongestLineLen = 0;
    pmTextPtr -> newLineAdded = FALSE;
#if !defined (CONSOLE) && !defined (TPROLOG)    
    pmTextPtr -> isDirty = FALSE;
    pmTextPtr -> isChangedSinceLastRun = FALSE;
    pmTextPtr -> lastSavedTextLen = 0;
    pmTextPtr -> lastSavedCRC = 0xffffffff;
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)
    pmTextPtr -> lineStartsBufferLen = INITIAL_LINE_STARTS_BUFFER_SIZE;
    pmTextPtr -> selFrom = 0;
    pmTextPtr -> selTo = 0;
#if !defined (CONSOLE) && !defined (TPROLOG)    
    pmTextPtr -> selWordFrom = DO_NOT_USE;
    pmTextPtr -> selWordTo = DO_NOT_USE;
    pmTextPtr -> virtualCol = DO_NOT_USE;
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)

    pmTextPtr -> locationOK = FALSE;
    
#if !defined (CONSOLE) && !defined (TPROLOG)    
    // Free the mark, error and path name buffers.
    MyFreeTextBuffers (pmTextPtr);
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)
    
    // Make an empty buffer
    pmTextPtr -> text [0] = '\n';
    pmTextPtr -> lineStarts [0] = 0;
    pmTextPtr -> lineStarts [1] = 1;
    pmTextPtr -> textLen = 0;
    
#if !defined (CONSOLE) && !defined (TPROLOG)    
    // Make an empty undo
    pmTextPtr -> undoHead = NULL;
    pmTextPtr -> undoTail = NULL;
    pmTextPtr -> redoHead = NULL;
    pmTextPtr -> redoTail = NULL;
    
    pmTextPtr -> isJava = FALSE;
    pmTextPtr -> isTuring = FALSE;
/*
    pmTextPtr -> undoMenuName = 0;
    pmTextPtr -> undoStatus = 0;
    pmTextPtr -> undoReplFrom = 0;
    pmTextPtr -> undoReplTo = 0;
    pmTextPtr -> undoText = NULL;
    pmTextPtr -> undoTextSize = 0;
    pmTextPtr -> undoSelFrom = 0;
    pmTextPtr -> undoSelTo = 0;
*/
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)
} // EdText_InitializeTextRecord


/************************************************************************/
/* EdText_IsEmpty							*/
/************************************************************************/
BOOL	EdText_IsEmpty (TextPtr pmTextPtr)
{
    char	*myPtr = pmTextPtr -> text;
    int		cnt;
        
    for (cnt = 0 ; cnt < pmTextPtr -> textLen ; cnt++)
    {
    	if ((*myPtr != ' ') && (*myPtr != '\n'))
    	{
    	    return FALSE;
    	}
    	myPtr++;
    }
    
    return TRUE;
} // EdText_IsEmpty


#if !defined (CONSOLE) && !defined (TPROLOG)
/************************************************************************/
/* EdText_IsIdentChar							*/
/************************************************************************/
BOOL	EdText_IsIdentChar (char pmChar)
{
    // We arbitrarily use Java tags because the Turing and Java tables 
    // are identical with respect to CH_IDS and CH_IDC.
    return ((stJavaCharTags [pmChar] == CH_IDS) || 
    	    (stJavaCharTags [pmChar] == CH_IDC));
} // EdText_IsIdentChar


/************************************************************************/
/* EdText_Key								*/
/************************************************************************/
int	EdText_Key (TextPtr pmTextPtr, char pmChar, char **pmDeletedChars)
{
    int	myReturnValue;
    
    pmTextPtr -> virtualCol = DO_NOT_USE;

    // Debug Messages    
    EdFail_AddMessage (EXTRA_MESSAGE, "Keystroke - Text: %x - ch: %d", 
    		       pmTextPtr, pmChar);
    MyAddTextState (pmTextPtr, NULL);	    	       

    // Process the keystroke
    if (((' ' <= pmChar) && (pmChar <= '~')) || (pmChar == '\t'))
    {
        myReturnValue = MyKeyNormal (pmTextPtr, pmChar);
    }
    else if (pmChar == '\b')
    {
        myReturnValue = MyKeyBackspace (pmTextPtr, pmDeletedChars);
    }
    else if ((pmChar == '\n') || (pmChar == '\r'))
    {
        myReturnValue = MyKeyReturn (pmTextPtr);
    }
    else if (pmChar == FORWARD_DELETE_CHAR)
    {
    	myReturnValue = EdText_KeyDelete (pmTextPtr, pmDeletedChars);
    }
    else
    {
    	myReturnValue = KEY_ILLEGAL_CHAR;
    }
    
    // Debug Messages    
    MyAddTextState (pmTextPtr, "Keystroke (2)");
    
    // Verify line database, if requested
    if (stProperties.verifyLineDB)
    {
    	MyVerifyLineDB (pmTextPtr);
    }
    
    return myReturnValue;
} // EdText_Key


/************************************************************************/
/* EdText_KeyDelete							*/
/************************************************************************/
int	EdText_KeyDelete (TextPtr pmTextPtr, char **pmDeletedChars)
{
    char	*myPtr;
    
    // Debug Messages    
    EdFail_AddMessage (NORMAL, "Keystroke - Text: %x - Delete", pmTextPtr);
    MyAddTextState (pmTextPtr, NULL);	    	       

    // If there's a selection, delete it
    if (pmTextPtr -> selFrom != pmTextPtr -> selTo)
    {
    	return EdText_SelectionDelete (pmTextPtr);
    }

    myPtr = pmTextPtr -> text + pmTextPtr -> selFrom;
    if (pmTextPtr -> selFrom == pmTextPtr -> textLen)
    {
    	return KEY_CANT_DELETE;
    }
    
    pmTextPtr -> selFrom++;
    pmTextPtr -> selTo++;
    
    return MyKeyBackspace (pmTextPtr, pmDeletedChars);
} // EdText_KeyDelete


/************************************************************************/
/* EdText_KeyBackTab							*/
/************************************************************************/
int	EdText_KeyBackTab (TextPtr pmTextPtr, char **pmDeletedChars)
{
    char	*myPtr;
    int		myLine, myCol;
    int		myResult;
    
    // Debug Messages    
    EdFail_AddMessage (NORMAL, "Keystroke - Text: %x - Backtab", pmTextPtr);
    MyAddTextState (pmTextPtr, NULL);	    	       

    // If there's a selection, delete it
    if (pmTextPtr -> selFrom != pmTextPtr -> selTo)
    {
    	return EdText_SelectionDelete (pmTextPtr);
    }

    EdText_GetSelectionLocation (pmTextPtr, &myLine, &myCol);

    if (myCol == 0)
    {
    	return KEY_CANT_DELETE;
    }

    // Backspace to the previous tab position, if the char to be deleted is
    // a space
    
    myPtr = pmTextPtr -> text + pmTextPtr -> selFrom - 1;
    if (*myPtr != ' ')
    {
	return KEY_CANT_DELETE;
    }

    do
    {
    	myResult = MyKeyBackspace (pmTextPtr, pmDeletedChars);
   	myCol--;
    	myPtr = pmTextPtr -> text + pmTextPtr -> selFrom - 1;
    } while ((*myPtr == ' ') && ((myCol % stProperties.indentSize) != 0));
    
    return myResult;
} // EdText_KeyBackTab


/*****************************************/
/* Caret and Selection Movement Commands */
/*****************************************/

/************************************************************************/
/* EdText_MoveColumnLeft						*/
/*									*/
/* Return either NO_REDRAW (nothing happened), HIDE_SELECTION (there is	*/
/* now no selection and there was before), CREATE_SELECTION (there is	*/
/* now a selection and there wasn't before), REDRAW (selection has	*/
/* grown)								*/
/************************************************************************/
int	EdText_MoveColumnLeft (TextPtr pmTextPtr, BOOL pmSelect)
{
    BOOL	myWasSelection;

    // Debug Messages    
    EdFail_AddMessage (NORMAL, "Move Column Left - Text: %x - Select: %d",
	    	       pmTextPtr, pmSelect);
    MyAddTextState (pmTextPtr, NULL);	    	       

    // Do the cursor move
    myWasSelection = pmTextPtr -> selFrom != pmTextPtr -> selTo;

    if (!pmSelect && myWasSelection)
    {
        pmTextPtr -> selFrom = pmTextPtr -> selTo = 
            min (pmTextPtr -> selFrom, pmTextPtr -> selTo);
    }
    else
    {
        if (pmTextPtr -> selTo == 0)
            return NO_REDRAW;
        
        pmTextPtr -> selTo -= 1;
        if (!pmSelect) 
        {
            pmTextPtr -> selFrom = pmTextPtr -> selTo;
        }
    }
       
    pmTextPtr -> virtualCol = DO_NOT_USE;
    pmTextPtr -> locationOK = FALSE;
    
    return MyReturnStatus (pmTextPtr, myWasSelection);
} // EdText_MoveColumnLeft


/************************************************************************/
/* EdText_MoveColumnRight						*/
/************************************************************************/
int	EdText_MoveColumnRight (TextPtr pmTextPtr, BOOL pmSelect)
{
    BOOL	myWasSelection;
    
    // Debug Messages    
    EdFail_AddMessage (NORMAL, "Move Column Right - Text: %x - Select: %d",
	    	       pmTextPtr, pmSelect);
    MyAddTextState (pmTextPtr, NULL);	    	       

    // Do the cursor move
    myWasSelection = pmTextPtr -> selFrom != pmTextPtr -> selTo;

    if (!pmSelect && myWasSelection)
    {
        pmTextPtr -> selFrom = pmTextPtr -> selTo = 
            max (pmTextPtr -> selFrom, pmTextPtr -> selTo);  
    }
    else
    {
        if (pmTextPtr -> selTo == pmTextPtr -> textLen)
            return NO_REDRAW;
        
        pmTextPtr -> selTo++;
        if (!pmSelect) 
        {
            pmTextPtr -> selFrom = pmTextPtr -> selTo;
        }
    }
    
    pmTextPtr -> virtualCol = DO_NOT_USE;
    pmTextPtr -> locationOK = FALSE;
    
    return MyReturnStatus (pmTextPtr, myWasSelection);
} // EdText_MoveColumnRight


/************************************************************************/
/* EdText_MoveLineBegin							*/
/*									*/
/* If we're between the beginning of the line + 1 and the first 	*/
/* non-whitespace character inclusive, move the cursor to the		*/
/* beginning of the line.  If we're at the beginning of the line or	*/
/* after the first non-whitespace character, move to the first		*/
/* non-whitespace character.						*/
/************************************************************************/
int	EdText_MoveLineBegin (TextPtr pmTextPtr, BOOL pmSelect)
{
    BOOL	myWasSelection;
    int 	myLine, myCol, myBeginningOfLine, myFirstNonWhitespaceChar;
    
    // Debug Messages    
    EdFail_AddMessage (NORMAL, "Move Line Begin - Text: %x - Select: %d",
	    	       pmTextPtr, pmSelect);
    MyAddTextState (pmTextPtr, NULL);	    	       

    // Do the cursor move
    myWasSelection = (pmTextPtr -> selFrom != pmTextPtr -> selTo);
    
    MyFindCharLocation (pmTextPtr, pmTextPtr -> selTo, &myLine, &myCol);

    myBeginningOfLine = pmTextPtr -> lineStarts [myLine];
    myFirstNonWhitespaceChar = myBeginningOfLine;
    while ((pmTextPtr -> text [myFirstNonWhitespaceChar] == ' ') &&
	   (myFirstNonWhitespaceChar < pmTextPtr -> textLen))
    {	    
	myFirstNonWhitespaceChar++;
    }
    if ((pmTextPtr -> text [myFirstNonWhitespaceChar] == '\n') ||
        ((myBeginningOfLine < pmTextPtr -> selTo) &&
	 (pmTextPtr -> selTo <= myFirstNonWhitespaceChar)))
    {
	pmTextPtr -> selTo = myBeginningOfLine;
    }
    else
    {
	pmTextPtr -> selTo = myFirstNonWhitespaceChar;
    }

    if (!pmSelect) 
    {
        pmTextPtr -> selFrom = pmTextPtr -> selTo;
    }
    
    pmTextPtr -> virtualCol = DO_NOT_USE;
    pmTextPtr -> locationOK = FALSE;
    
    return MyReturnStatus (pmTextPtr, myWasSelection);
} // EdText_MoveLineBegin


/************************************************************************/
/* EdText_MoveLineDown							*/
/************************************************************************/
int	EdText_MoveLineDown (TextPtr pmTextPtr, BOOL *pmMoved, BOOL pmSelect)
{
    BOOL	myWasSelection;
    int 	myLine, myCol, myPos;
    BOOL	myBeyondEnd;
    
    // Debug Messages    
    EdFail_AddMessage (NORMAL, "Move Line Down - Text: %x - Select: %d",
	    	       pmTextPtr, pmSelect);
    MyAddTextState (pmTextPtr, NULL);	    	       

    // Do the cursor move
    myWasSelection = (pmTextPtr -> selFrom != pmTextPtr -> selTo);
    
    MyFindCharLocation (pmTextPtr, pmTextPtr -> selTo, &myLine, &myCol);
    if (myLine == pmTextPtr -> numLines) 
    {
    	*pmMoved = FALSE;
        if (!pmSelect && myWasSelection)
        {
            pmTextPtr -> selFrom = pmTextPtr -> selTo = 
                max (pmTextPtr -> selFrom, pmTextPtr -> selTo);  
	    return HIDE_SELECTION;                
        }
        return NO_REDRAW;
    }
    *pmMoved = TRUE;
    myLine++;
    if (pmTextPtr -> virtualCol != DO_NOT_USE)
        myCol = pmTextPtr -> virtualCol;
    MyFindLocationPos (pmTextPtr, myLine, myCol, &myPos, &myBeyondEnd);
    if (myBeyondEnd)
        pmTextPtr -> virtualCol = myCol;

    pmTextPtr -> selTo = myPos;  
    if (!pmSelect) 
    {
        pmTextPtr -> selFrom = pmTextPtr -> selTo;
    }

    pmTextPtr -> locationOK = FALSE;
    
    return MyReturnStatus (pmTextPtr, myWasSelection);
} // EdText_MoveLineDown


/************************************************************************/
/* EdText_MoveLineEnd							*/
/************************************************************************/
int	EdText_MoveLineEnd (TextPtr pmTextPtr, BOOL pmSelect)
{
    BOOL	myWasSelection;
    int 	myLine, myCol, myEndOfLine, myLastNonWhitespaceChar;;
    
    // Debug Messages    
    EdFail_AddMessage (NORMAL, "Move Line End - Text: %x - Select: %d",
	    	       pmTextPtr, pmSelect);
    MyAddTextState (pmTextPtr, NULL);	    	       

    // Do the cursor move
    myWasSelection = (pmTextPtr -> selFrom != pmTextPtr -> selTo);

    MyFindCharLocation (pmTextPtr, pmTextPtr -> selTo, &myLine, &myCol);
 
    if (myLine < pmTextPtr -> numLines)
    {
	myEndOfLine = pmTextPtr -> lineStarts [myLine + 1] - 1;
	myLastNonWhitespaceChar = myEndOfLine;
	while ((myLastNonWhitespaceChar > 0) &&
	       (pmTextPtr -> text [myLastNonWhitespaceChar - 1] == ' '))
	{
	    myLastNonWhitespaceChar--;
	}
	if ((myLastNonWhitespaceChar == 0) ||
	    (pmTextPtr -> text [myLastNonWhitespaceChar - 1] == '\n') ||
	    ((pmTextPtr -> selTo < myEndOfLine) &&
	    (myLastNonWhitespaceChar <= pmTextPtr -> selTo)))
	{
	    pmTextPtr -> selTo = myEndOfLine;
	}
	else
	{
	    pmTextPtr -> selTo = myLastNonWhitespaceChar;
	}
    }
    
    if (!pmSelect) 
    {
        pmTextPtr -> selFrom = pmTextPtr -> selTo;
    }
    
    pmTextPtr -> virtualCol = DO_NOT_USE;
    pmTextPtr -> locationOK = FALSE;
            
    return MyReturnStatus (pmTextPtr, myWasSelection);
} // EdText_MoveLineEnd


/************************************************************************/
/* EdText_MoveLineUp							*/
/*									*/
/* Move the cursor up a line.  Returns TRUE if there was a selection	*/
/* that must be removed.						*/
/************************************************************************/
int	EdText_MoveLineUp (TextPtr pmTextPtr, BOOL pmSelect)
{
    BOOL	myWasSelection;
    int 	myLine, myCol, myPos;
    BOOL	myBeyondEnd;
    
    // Debug Messages    
    EdFail_AddMessage (NORMAL, "Move Line Up - Text: %x - Select: %d",
	    	       pmTextPtr, pmSelect);
    MyAddTextState (pmTextPtr, NULL);	    	       

    // Do the cursor move
    myWasSelection = (pmTextPtr -> selFrom != pmTextPtr -> selTo);
    
    MyFindCharLocation (pmTextPtr, pmTextPtr -> selTo, &myLine, &myCol);
    if (myLine == 0) 
    {
        if (!pmSelect && myWasSelection)
        {
            pmTextPtr -> selFrom = pmTextPtr -> selTo = 
                max (pmTextPtr -> selFrom, pmTextPtr -> selTo);  
	    return HIDE_SELECTION;                
        }
        return NO_REDRAW;
    }
    myLine--;
    if (pmTextPtr -> virtualCol != DO_NOT_USE)
        myCol = pmTextPtr -> virtualCol;
    MyFindLocationPos (pmTextPtr, myLine, myCol, &myPos, &myBeyondEnd);
    if (myBeyondEnd)
        pmTextPtr -> virtualCol = myCol;

    pmTextPtr -> selTo = myPos;  
    if (!pmSelect) 
    {
        pmTextPtr -> selFrom = pmTextPtr -> selTo;
    }
    
    pmTextPtr -> locationOK = FALSE;

    return MyReturnStatus (pmTextPtr, myWasSelection);
} // EdText_MoveLineUp


/************************************************************************/
/* EdText_MovePageDown							*/
/************************************************************************/
int	EdText_MovePageDown (TextPtr pmTextPtr, int pmPageSize, 
			      BOOL *pmMoved, BOOL pmSelect)
{
    BOOL	myWasSelection;
    int 	myLine, myCol, myPos;
    BOOL	myBeyondEnd;
    
    // Debug Messages    
    EdFail_AddMessage (NORMAL, "Move Page Down - Text: %x - Select: %d",
	    	       pmTextPtr, pmSelect);
    MyAddTextState (pmTextPtr, NULL);	    	       

    // Do the cursor move
    myWasSelection = (pmTextPtr -> selFrom != pmTextPtr -> selTo);
    
    MyFindCharLocation (pmTextPtr, pmTextPtr -> selTo, &myLine, &myCol);
    if (myLine == pmTextPtr -> numLines) 
    {
    	*pmMoved = FALSE;
        return NO_REDRAW;
    }
    *pmMoved = TRUE;
    myLine = min (pmTextPtr -> numLines, myLine + pmPageSize);
    if (pmTextPtr -> virtualCol != DO_NOT_USE)
        myCol = pmTextPtr -> virtualCol;
    MyFindLocationPos (pmTextPtr, myLine, myCol, &myPos, &myBeyondEnd);
    if (myBeyondEnd)
        pmTextPtr -> virtualCol = myCol;

    pmTextPtr -> selTo = myPos;  
    if (!pmSelect) 
    {
        pmTextPtr -> selFrom = pmTextPtr -> selTo;
    }
    
    pmTextPtr -> locationOK = FALSE;

    return MyReturnStatus (pmTextPtr, myWasSelection);
} // EdText_MovePageDown


/************************************************************************/
/* EdText_MovePageUp							*/
/************************************************************************/
int	EdText_MovePageUp (TextPtr pmTextPtr, int pmPageSize, BOOL pmSelect)
{
    BOOL	myWasSelection;
    int 	myLine, myCol, myPos;
    BOOL	myBeyondEnd;
    
    // Debug Messages    
    EdFail_AddMessage (NORMAL, "Move Page Up - Text: %x - Select: %d",
	    	       pmTextPtr, pmSelect);
    MyAddTextState (pmTextPtr, NULL);	    	       

    // Do the cursor move
    myWasSelection = (pmTextPtr -> selFrom != pmTextPtr -> selTo);
    
    MyFindCharLocation (pmTextPtr, pmTextPtr -> selTo, &myLine, &myCol);
    myLine = max (0, myLine - pmPageSize);
    if (pmTextPtr -> virtualCol != DO_NOT_USE)
        myCol = pmTextPtr -> virtualCol;
    MyFindLocationPos (pmTextPtr, myLine, myCol, &myPos, &myBeyondEnd);
    if (myBeyondEnd)
        pmTextPtr -> virtualCol = myCol;
        
    pmTextPtr -> selTo = myPos;  
    if (!pmSelect) 
    {
        pmTextPtr -> selFrom = pmTextPtr -> selTo;
    }
    
    pmTextPtr -> locationOK = FALSE;

    return MyReturnStatus (pmTextPtr, myWasSelection);
} // EdText_MovePageUp


/************************************************************************/
/* EdText_MoveProgramBottom						*/
/************************************************************************/
int	EdText_MoveProgramBottom (TextPtr pmTextPtr, BOOL pmSelect)
{
    BOOL	myWasSelection;
    
    // Debug Messages    
    EdFail_AddMessage (NORMAL, "Move Program Bottom - Text: %x - Select: %d",
	    	       pmTextPtr, pmSelect);
    MyAddTextState (pmTextPtr, NULL);	    	       

    // Do the cursor move
    myWasSelection = (pmTextPtr -> selFrom != pmTextPtr -> selTo);

    pmTextPtr -> selTo = pmTextPtr -> textLen;  
    if (!pmSelect) 
    {
        pmTextPtr -> selFrom = pmTextPtr -> selTo;
    }

    pmTextPtr -> virtualCol = DO_NOT_USE;
    pmTextPtr -> locationOK = FALSE;

    return MyReturnStatus (pmTextPtr, myWasSelection);
} // EdText_MoveProgramBottom


/************************************************************************/
/* EdText_MoveProgramTop						*/
/************************************************************************/
int	EdText_MoveProgramTop (TextPtr pmTextPtr, BOOL pmSelect)
{
    BOOL	myWasSelection;
    
    // Debug Messages    
    EdFail_AddMessage (NORMAL, "Move Program Top - Text: %x - Select: %d",
	    	       pmTextPtr, pmSelect);
    MyAddTextState (pmTextPtr, NULL);	    	       

    // Do the cursor move
    myWasSelection = (pmTextPtr -> selFrom != pmTextPtr -> selTo);
    
    pmTextPtr -> selTo = 0;  
    if (!pmSelect) 
    {
        pmTextPtr -> selFrom = pmTextPtr -> selTo;
    }

    pmTextPtr -> virtualCol = DO_NOT_USE;
    pmTextPtr -> locationOK = FALSE;

    return MyReturnStatus (pmTextPtr, myWasSelection);
} // EdText_MoveProgramTop


/************************************************************************/
/* EdText_MoveWordNext							*/
/*									*/
/* The algorithm is: 							*/
/*     (1a) if an NL, then forward over NL - DONE			*/
/*     (1b) if at alphanum, then forward until ws/non-alphanum	 	*/
/*     (1c) if at non-alphanum, then forward until ws/alphanum	 	*/
/*     (2) skip over spaces/tabs					*/
/*									*/
/* Note in this function, we arbitrarily use Java tags because the	*/
/* Turing and Java tables are identical with respect to CH_IDS and 	*/
/* CH_IDC.								*/
/************************************************************************/
int	EdText_MoveWordNext (TextPtr pmTextPtr, BOOL pmSelect)
{
    BOOL		myWasSelection;
    unsigned char	*myPtr;
    char		*myEnd;
    
    // Debug Messages    
    EdFail_AddMessage (NORMAL, "Move Word Next - Text: %x - Select: %d",
	    	       pmTextPtr, pmSelect);
    MyAddTextState (pmTextPtr, NULL);	    	       

    // Do the cursor move
    myWasSelection = (pmTextPtr -> selFrom != pmTextPtr -> selTo);

    myPtr = &pmTextPtr -> text [pmTextPtr -> selTo];
    myEnd = pmTextPtr -> text + pmTextPtr -> textLen;  
    
    // (1a) if an NL, then forward over NL - DONE
    if (*myPtr == '\n')
    {
    	myPtr++;
    }
    else 
    {
    	// (1b) if at alphanum, then forward until ws/non-alphanum
    	if ((stJavaCharTags [*myPtr] == CH_IDS) || 
    	    (stJavaCharTags [*myPtr] == CH_IDC))
    	{
    	    while ((myPtr < myEnd) && 
	   	   ((stJavaCharTags [*myPtr] == CH_IDS) || 
	    	    (stJavaCharTags [*myPtr] == CH_IDC)))
    	    {
            	myPtr++;
            }
    	}
    	// (1c) if at non-alphanum, then forward until ws/alphanum
    	else if ((*myPtr != ' ') && (*myPtr != '\t'))
    	{
    	    while ((myPtr < myEnd) && 
    	           ((stJavaCharTags [*myPtr] != CH_IDS) && 
    	            (stJavaCharTags [*myPtr] != CH_IDC) &&
    	            (*myPtr != ' ') && (*myPtr != '\t') && (*myPtr != '\n')))
    	    {
            	myPtr++;
            }
        }
        
        // (2) skip over spaces/tabs
    	while ((myPtr < myEnd) && 
    	       ((*myPtr == ' ') || (*myPtr == '\t')))
    	{
            myPtr++;
    	}
    }

    pmTextPtr -> selTo = myPtr - pmTextPtr -> text;
    if (!pmSelect) 
    {
        pmTextPtr -> selFrom = pmTextPtr -> selTo;
    }

    pmTextPtr -> virtualCol = DO_NOT_USE;
    pmTextPtr -> locationOK = FALSE;
    
    return MyReturnStatus (pmTextPtr, myWasSelection);
} // EdText_MoveWordNext


/************************************************************************/
/* EdText_MoveWordPrev							*/
/************************************************************************/
int	EdText_MoveWordPrev (TextPtr pmTextPtr, BOOL pmSelect)
{
    BOOL		myWasSelection;
    unsigned char	*myPtr;
    
    // Debug Messages    
    EdFail_AddMessage (NORMAL, "Move Word Prev - Text: %x - Select: %d",
	    	       pmTextPtr, pmSelect);
    MyAddTextState (pmTextPtr, NULL);	    	       

    // Do the cursor move
    myWasSelection = (pmTextPtr -> selFrom != pmTextPtr -> selTo);

    myPtr = &pmTextPtr -> text [pmTextPtr -> selTo] - 1;
    
    // (1a) if an NL, then forward over NL - DONE
    if (*myPtr == '\n')
    {
    	myPtr--;
    }
    else 
    {
        // (1b) skip over spaces/tabs
    	while ((myPtr >= pmTextPtr -> text) && 
    	       ((*myPtr == ' ') || (*myPtr == '\t')))
    	{
            myPtr--;
    	}
        
    	// (2a) if at alphanum, then forward until ws/non-alphanum
    	if ((stJavaCharTags [*myPtr] == CH_IDS) || 
    	    (stJavaCharTags [*myPtr] == CH_IDC))
    	{
    	    while ((myPtr >= pmTextPtr -> text) && 
	   	   ((stJavaCharTags [*myPtr] == CH_IDS) || 
	    	    (stJavaCharTags [*myPtr] == CH_IDC)))
    	    {
            	myPtr--;
            }
    	}
    	// (2b) if at non-alphanum, then forward until ws/alphanum
    	else if ((*myPtr != ' ') && (*myPtr != '\t'))
    	{
    	    while ((myPtr >= pmTextPtr -> text) && 
    	           ((stJavaCharTags [*myPtr] != CH_IDS) && 
    	            (stJavaCharTags [*myPtr] != CH_IDC) &&
    	            (*myPtr != ' ') && (*myPtr != '\t') && (*myPtr != '\n')))
    	    {
            	myPtr--;
            }
        }
    }

    myPtr++;

    pmTextPtr -> selTo = myPtr - pmTextPtr -> text;
    if (!pmSelect)   
    {
        pmTextPtr -> selFrom = pmTextPtr -> selTo;
    }
    
    pmTextPtr -> virtualCol = DO_NOT_USE;
    pmTextPtr -> locationOK = FALSE;
    
    return MyReturnStatus (pmTextPtr, myWasSelection);
} // EdText_MoveWordPrev
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)					  


/************************************************************************/
/* EdText_SelectAll							*/
/************************************************************************/
int	EdText_SelectAll (TextPtr pmTextPtr)
{
    BOOL	myWasSelection;
    
#if !defined (CONSOLE) && !defined (TPROLOG)
    // Debug Messages    
    EdFail_AddMessage (NORMAL, "Select All - Text: %x", pmTextPtr);
    MyAddTextState (pmTextPtr, NULL);	    	       
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)

    // Do the cursor move
    if (pmTextPtr -> numLines == 0)
        return NO_REDRAW;
        
    myWasSelection = pmTextPtr -> selFrom != pmTextPtr -> selTo;

    pmTextPtr -> selFrom = 0;
    pmTextPtr -> selTo = pmTextPtr -> textLen;  
    pmTextPtr -> virtualCol = DO_NOT_USE;
    pmTextPtr -> locationOK = FALSE;
    
#if !defined (CONSOLE) && !defined (TPROLOG)
    // Once you've selected all, you can't undo more keystrokes
    EdText_UndoSetMoreKeystrokes (pmTextPtr, FALSE);
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)					  
    
    return MyReturnStatus (pmTextPtr, myWasSelection);
} // EdText_SelectAll


/************************************************************************/
/* EdText_SelectExtendSelection						*/
/************************************************************************/
int	EdText_SelectExtendSelection (TextPtr pmTextPtr, int pmLine, int pmCol)
{
    BOOL	myWasSelection, myDummy;
    int		myLoc;
    
#if !defined (CONSOLE) && !defined (TPROLOG)
    // Debug Messages    
    EdFail_AddMessage (MOUSE_MESSAGE, 
    		       "Extend Selection - Text: %x - L: %d - C: %d", 
    		       pmTextPtr, pmLine, pmCol);
    MyAddTextState (pmTextPtr, NULL);	    	       
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)

    // Do the cursor move
    myWasSelection = (pmTextPtr -> selFrom != pmTextPtr -> selTo);

    MyFindLocationPos (pmTextPtr, pmLine, pmCol, &myLoc, &myDummy);
    pmTextPtr -> selTo = myLoc;  
    pmTextPtr -> virtualCol = DO_NOT_USE;
    pmTextPtr -> locationOK = FALSE;
    
    // If we are extending a selection which was based on a double clicked 
    // word, the new selection always includes the word.
    if (pmTextPtr -> selWordFrom != DO_NOT_USE)
    {
        if (pmTextPtr -> selFrom <= pmTextPtr -> selTo)
        {
    	    if (pmTextPtr -> selWordFrom < pmTextPtr -> selFrom)
    	    {
    	        pmTextPtr -> selFrom = pmTextPtr -> selWordFrom;
    	    }
    	    if (pmTextPtr -> selWordTo > pmTextPtr -> selTo)
    	    {
    	        pmTextPtr -> selTo = pmTextPtr -> selWordTo;
    	    }
        }
        else
        {
    	    if (pmTextPtr -> selWordFrom < pmTextPtr -> selTo)
    	    {
    	        pmTextPtr -> selTo = pmTextPtr -> selWordFrom;
    	    }
    	    if (pmTextPtr -> selWordTo > pmTextPtr -> selFrom)
    	    {
    	        pmTextPtr -> selFrom = pmTextPtr -> selWordTo;
    	    }
        }
    }

    return MyReturnStatus (pmTextPtr, myWasSelection);
} // EdText_SelectExtendSelection


/************************************************************************/
/* EdText_SelectLine							*/
/************************************************************************/
int	EdText_SelectLine (TextPtr pmTextPtr)
{
    BOOL	myWasSelection;
    int		myLine, myCol;
    int 	myStartLinePos, myEndLinePos;
    
#if !defined (CONSOLE) && !defined (TPROLOG)
    // Debug Messages    
    EdFail_AddMessage (NORMAL, "Select Line - Text: %x", pmTextPtr);
    MyAddTextState (pmTextPtr, NULL);	    	       
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)

    // Do the cursor move
    myWasSelection = (pmTextPtr -> selFrom != pmTextPtr -> selTo);

    MyFindCharLocation (pmTextPtr, pmTextPtr -> selTo, &myLine, &myCol);
    
    // If we're at the end of the file, don't bother trying to select
    // the line
    if (myLine == pmTextPtr -> numLines)
    {
    	pmTextPtr -> selFrom = pmTextPtr -> selTo = pmTextPtr -> textLen;
    }
    else
    {
    	myStartLinePos = pmTextPtr -> lineStarts [myLine];
    	myEndLinePos = pmTextPtr -> lineStarts [myLine + 1];

    	pmTextPtr -> virtualCol = DO_NOT_USE;
    	pmTextPtr -> locationOK = FALSE;

    	if (pmTextPtr -> selFrom <= pmTextPtr -> selTo)
    	{
    	    if (myStartLinePos < pmTextPtr -> selFrom)
    	    {
    	    	pmTextPtr -> selFrom = myStartLinePos;
    	    }
    	    if (myEndLinePos > pmTextPtr -> selTo)
    	    {
    	    	pmTextPtr -> selTo = myEndLinePos;
    	    }
    	}
    	else
    	{
    	    if (myStartLinePos < pmTextPtr -> selTo)
    	    {
    	    	pmTextPtr -> selTo = myStartLinePos;
    	    }
    	    if (myEndLinePos > pmTextPtr -> selFrom)
    	    {
    	    	pmTextPtr -> selFrom = myEndLinePos;
	    }    	    	
    	}
    
    	pmTextPtr -> selWordFrom = myStartLinePos;
    	pmTextPtr -> selWordTo = myEndLinePos;
    }
    
    return MyReturnStatus (pmTextPtr, myWasSelection);
} // EdText_SelectLine


#if !defined (CONSOLE) && !defined (TPROLOG)					  
/************************************************************************/
/* EdText_SelectLine1							*/
/************************************************************************/
int	EdText_SelectLine1 (TextPtr pmTextPtr, int pmLine)
{
    BOOL	myWasSelection;
    
    // Debug Messages    
    EdFail_AddMessage (NORMAL, "Select Line 1 - Text: %x - L: %d", pmTextPtr, 
    		       pmLine);
    MyAddTextState (pmTextPtr, NULL);	    	       

    // Do the cursor move
    myWasSelection = (pmTextPtr -> selFrom != pmTextPtr -> selTo);

    if (pmLine >= pmTextPtr -> numLines)
    {
	pmTextPtr -> selFrom = pmTextPtr -> textLen;
	pmTextPtr -> selTo = pmTextPtr -> textLen;
    }
    else
    {
        pmLine = max (0, min (pmLine, pmTextPtr -> numLines - 1));
        pmTextPtr -> selFrom = pmTextPtr -> lineStarts [pmLine];
	pmLine++;
	if (pmLine >= pmTextPtr -> numLines)
	{
	    pmTextPtr -> selTo = pmTextPtr -> textLen;
	}
	else
	{
            pmLine = min (pmLine, pmTextPtr -> numLines - 1);
	    pmTextPtr -> selTo = pmTextPtr -> lineStarts [pmLine];
	}
    }

    pmTextPtr -> virtualCol = DO_NOT_USE;
    pmTextPtr -> locationOK = FALSE;
    
    pmTextPtr -> selWordFrom = DO_NOT_USE;
    pmTextPtr -> selWordTo = DO_NOT_USE;
        
    return MyReturnStatus (pmTextPtr, myWasSelection);
} // EdText_SelectLine1
#endif


/************************************************************************/
/* EdText_SelectWord							*/
/*									*/
/* Select the entire word that selTo is in.				*/
/************************************************************************/
int	EdText_SelectWord (TextPtr pmTextPtr)
{
    BOOL		myWasSelection;
    unsigned char	*myPtr;
    int 		myStartWordPos, myEndWordPos;
    
#if !defined (CONSOLE) && !defined (TPROLOG)
    // Debug Messages    
    EdFail_AddMessage (NORMAL, "Select Word - Text: %x", pmTextPtr);
    MyAddTextState (pmTextPtr, NULL);	    	       
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)

    // Do the cursor move
    myWasSelection = (pmTextPtr -> selFrom != pmTextPtr -> selTo);

    myPtr = &pmTextPtr -> text [pmTextPtr -> selTo - 1];
    // We arbitrarily use Java tags because the Turing and Java tables 
    // are identical with respect to CH_IDS and CH_IDC.
    while ((myPtr >= pmTextPtr -> text) && 
    	   ((stJavaCharTags [*myPtr] == CH_IDS) || 
    	    (stJavaCharTags [*myPtr] == CH_IDC)))
    {
    	myPtr--;
    }
    myStartWordPos = myPtr - pmTextPtr -> text + 1;
    
    myPtr = &pmTextPtr -> text [pmTextPtr -> selTo];
    // We arbitrarily use Java tags because the Turing and Java tables 
    // are identical with respect to CH_IDS and CH_IDC.
    while ((stJavaCharTags [*myPtr] == CH_IDS) || 
           (stJavaCharTags [*myPtr] == CH_IDC))
    {
    	myPtr++;
    }
    myEndWordPos = myPtr - pmTextPtr -> text;
    
    pmTextPtr -> virtualCol = DO_NOT_USE;
    pmTextPtr -> locationOK = FALSE;

    if (pmTextPtr -> selFrom <= pmTextPtr -> selTo)
    {
    	if (myStartWordPos < pmTextPtr -> selFrom)
    	{
    	    pmTextPtr -> selFrom = myStartWordPos;
    	}
    	if (myEndWordPos > pmTextPtr -> selTo)
    	{
    	    pmTextPtr -> selTo = myEndWordPos;
    	}
    }
    else
    {
    	if (myStartWordPos < pmTextPtr -> selTo)
    	{
    	    pmTextPtr -> selTo = myStartWordPos;
    	}
    	if (myEndWordPos > pmTextPtr -> selFrom)
    	{
    	    pmTextPtr -> selFrom = myEndWordPos;
    	}
    }
    
    pmTextPtr -> selWordFrom = myStartWordPos;
    pmTextPtr -> selWordTo = myEndWordPos;
    
    return MyReturnStatus (pmTextPtr, myWasSelection);
} // EdText_SelectWord


#if !defined (CONSOLE) && !defined (TPROLOG)					  
/************************************************************************/
/* EdText_SelectionComment						*/
/************************************************************************/
int	EdText_SelectionComment (TextPtr pmTextPtr)
{
    int		myCommentSize;
    int		myResultCode = KEY_UPDATE_LINE;
    int		myFromLine, myFromCol, myToLine, myToCol;
    char	*myPtr, *myStartPtr, *myFromPtr, *myToPtr;
    int		myIndent;
    int		myLine;
    int		cnt;
    
    if (pmTextPtr -> isJava)
    {
    	myCommentSize = 3;
    }
    else if (pmTextPtr -> isTuring)
    {
    	myCommentSize = 2;
    }
    else
    {
    	// TW Don't know how to comment this type of file
    	return 0;
    }
    
    EdFail_AddMessage (NORMAL, "Comment Selection (1) - Text: %x", pmTextPtr);
    MyAddTextState (pmTextPtr, NULL);	    	       

    EdText_GetSelectionLinesCols (pmTextPtr, &myFromLine, &myFromCol, 
				  &myToLine, &myToCol);

    // Debugging information
    EdFail_AddMessage (NORMAL, "Comment Selection (2) - From line: %d - "
        "From col: %d - To line: %d - To col: %d", myFromLine, myFromCol, 
        myToLine, myToCol);

    // Calculate the selection for lines for undo purposes
    if (myToCol > 0)
    {
    	myToLine++;
    }
    
    if ((myToLine - myFromLine) > 1)
    {
    	myResultCode = KEY_REDRAW_FROM_SELECTION;
    }

    // Get the indent level
    myIndent = MAX_INDENT;
    for (myLine = myFromLine ; myLine < myToLine ; myLine++)
    {
    	myPtr = myStartPtr = pmTextPtr -> text + 
    	    pmTextPtr -> lineStarts [myLine];
    	while (*myPtr == ' ')
	{
    	    myPtr++;
	}
	if (*myPtr != '\n')
	{
	    myIndent = min (myIndent, myPtr - myStartPtr);
	}
    }
    if (myIndent == MAX_INDENT)
    {
    	myIndent = 0;
    }
    
    // Insert the text
    for (myLine = myFromLine ; myLine < myToLine ; myLine++)
    {
    	int	myLen, myPos, mySpaces;
    	
    	myLen = pmTextPtr -> lineStarts [myLine + 1] - 
    	    pmTextPtr -> lineStarts [myLine] - 1;
	
	// Calculate if we need to add extra spaces to the line
	if (myLen < myIndent)
	{
	    myPos = myLen;
	    mySpaces = myIndent - myLen;
	}    	    
	else
	{
	    myPos = myIndent;
	    mySpaces = 0;
	}
	
    	// Make certain there's enough space    
    	if (!MyEnsureTextBufferSpace (pmTextPtr, myCommentSize + mySpaces))
    	{
    	    return KEY_OUT_OF_MEMORY;
    	}
    	
	// Move from the selection to the end of the buffer one plus the auto
    	// indent size over to fit the newline and the extra spaces. Then
    	// place the newline and the spaces in the buffer
    	myPtr = pmTextPtr -> text + pmTextPtr -> lineStarts [myLine] + myPos;
    	memmove (myPtr + myCommentSize + mySpaces, myPtr, 
            pmTextPtr -> textLen - pmTextPtr -> lineStarts [myLine] - myPos);
	for (cnt = 0 ; cnt < mySpaces ; cnt++)
        {
    	     *myPtr++ = ' ';
        }
	if (pmTextPtr -> isJava)
	{
            *myPtr++ = '/';
            *myPtr++ = '/';
        }
        else
        {
	    *myPtr++ = '%';
	}
	*myPtr++ = ' ';
	
    	// Do the same for the textType block
    	myPtr = pmTextPtr -> textType + pmTextPtr -> lineStarts [myLine] + 
    	    myPos;
    	memmove (myPtr + myCommentSize + mySpaces, myPtr, 
            pmTextPtr -> textLen - pmTextPtr -> lineStarts [myLine] - myPos);

    	// Fix the line starts array
    	myResultCode |= MyLineStartsAdjustForInsert (pmTextPtr, 
            pmTextPtr -> lineStarts [myLine], myCommentSize + mySpaces, 0, 0);
    
    	// Wipe out mark or error if on this line
    	myResultCode |= MyNotifyLinesModified (pmTextPtr, myLine, myLine);
    }
    
    // Set the comment selection's text type
    myFromPtr = pmTextPtr -> textType + pmTextPtr -> lineStarts [myFromLine];
    myToPtr = pmTextPtr -> textType + pmTextPtr -> lineStarts [myToLine];
    myPtr = myFromPtr;
    while (myPtr < myToPtr)
    {
    	*myPtr++ = TEXT_NO_CLASSIFICATION;
    }
    
    // Advance the selection
    pmTextPtr -> selFrom = pmTextPtr -> selTo = 
        pmTextPtr -> lineStarts [myToLine];
    pmTextPtr -> locationOK = FALSE;
    
    // Set dirty flag
    myResultCode |= MySetDirty (pmTextPtr);
   
    // Colour the new text
    if (MyClassifySection (pmTextPtr, pmTextPtr -> lineStarts [myFromLine]) == 
        CLASSIFY_CHANGE_MULTIPLE_LINES)
    {
    	myResultCode |= KEY_REDRAW_FROM_SELECTION;
    }
    
    // Verify line database, if requested
    if (stProperties.verifyLineDB)
    {
    	MyVerifyLineDB (pmTextPtr);
    }
    
    return myResultCode;
} // EdText_SelectionComment
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)	


/************************************************************************/
/* EdText_SelectionDelete						*/
/************************************************************************/
int	EdText_SelectionDelete (TextPtr pmTextPtr)
{
    int		myResultCode = KEY_UPDATE_LINE;
    int		myFromLine, myFromCol, myToLine, myToCol;
    char	*myPtr;
    int		myBeginSel, myEndSel, myNumSpacesToDelete, myNumLinesToDelete;
    BOOL	mySelStartAtEndOfLine;

#if !defined (CONSOLE) && !defined (TPROLOG)	
    // Debug Messages    
    EdFail_AddMessage (NORMAL, "Delete Selection (1) - Text: %x", pmTextPtr);
    MyAddTextState (pmTextPtr, NULL);	    	       
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)	
    
    // Calculate deletion information
    EdText_GetSelectionLinesCols (pmTextPtr, &myFromLine, &myFromCol, 
        &myToLine, &myToCol);

#if !defined (CONSOLE) && !defined (TPROLOG)	
    // Debugging information
    EdFail_AddMessage (NORMAL, "Delete Selection (2) - From line: %d - "
        "From col: %d - To line: %d - To col: %d", myFromLine, myFromCol, 
        myToLine, myToCol);
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)	

    myNumSpacesToDelete = abs (pmTextPtr -> selTo - pmTextPtr -> selFrom);
    myNumLinesToDelete = myToLine - myFromLine;
    myBeginSel = min (pmTextPtr -> selFrom, pmTextPtr -> selTo);
    myEndSel = max (pmTextPtr -> selFrom, pmTextPtr -> selTo);
    
    // Move from the selection to the end of the buffer one plus the auto
    // indent size over to fit the newline and the extra spaces. Then
    // place the newline and the spaces in the buffer
    myPtr = pmTextPtr -> text + myBeginSel;
    memmove (myPtr, myPtr + myNumSpacesToDelete, 
        pmTextPtr -> textLen - myEndSel);
        
#if !defined (CONSOLE) && !defined (TPROLOG)
    // Do the same for the textType block
    myPtr = pmTextPtr -> textType + myBeginSel;
    memmove (myPtr, myPtr + myNumSpacesToDelete, 
        pmTextPtr -> textLen - myEndSel);
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)	

    // Advance the selection
    pmTextPtr -> selTo = pmTextPtr -> selFrom = myBeginSel;
    pmTextPtr -> locationOK = FALSE;
    
    // Adjust the line starts array
    myResultCode |= MyLineStartsAdjustForDelete (pmTextPtr, myBeginSel, 
        myEndSel);
    
    // Is the selection from at the end of a line
    if (*((pmTextPtr -> text + pmTextPtr -> selFrom) + 1) == '\n')
    {
        mySelStartAtEndOfLine = TRUE;
    }
    else
    {
        mySelStartAtEndOfLine = FALSE;
    }

#if !defined (CONSOLE) && !defined (TPROLOG)	
    // Handle all the changing errors and marks
    MyNotifyLinesDeleted (pmTextPtr, myFromLine, myToLine, 
        mySelStartAtEndOfLine);   
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)	
        
#if !defined (CONSOLE) && !defined (TPROLOG)	
    // Set dirty flag
    myResultCode |= MySetDirty (pmTextPtr);
    
    // Colour the new text (will always redraw screen
    (void) MyClassifySection (pmTextPtr, pmTextPtr -> selFrom);
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)	
    
#if !defined (CONSOLE) && !defined (TPROLOG)	
    MyAddTextState (pmTextPtr, "Delete Selection (3)");
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)	

    // Verify line database, if requested
    if (stProperties.verifyLineDB)
    {
    	MyVerifyLineDB (pmTextPtr);
    }

    return myResultCode;
} // EdText_SelectionDelete


#if !defined (CONSOLE) && !defined (TPROLOG)	
/************************************************************************/
/* EdText_SelectionUncomment						*/
/************************************************************************/
int	EdText_SelectionUncomment (TextPtr pmTextPtr)
{
    int		myCommentSize;
    int		myResultCode = KEY_UPDATE_LINE;
    int		myFromLine, myFromCol, myToLine, myToCol;
    char	*myPtr, *myNextPtr, *myFromPtr, *myToPtr;
    int		myCharsToDelete, myPos;
    int		myLine;
    
    if (pmTextPtr -> isJava)
    {
    	myCommentSize = 3;
    }
    else if (pmTextPtr -> isTuring)
    {
    	myCommentSize = 2;
    }
    else
    {
    	// TW Don't know how to comment this type of file
    	return 0;
    }
    
    // Debug Messages    
    EdFail_AddMessage (NORMAL, "Uncomment Selection (1) - Text: %x", pmTextPtr);
    MyAddTextState (pmTextPtr, NULL);	    	       
    
    EdText_GetSelectionLinesCols (pmTextPtr, &myFromLine, &myFromCol, 
				  &myToLine, &myToCol);

    // Debugging information
    EdFail_AddMessage (NORMAL, "Uncomment Selection (2) - From line: %d "
        "- From col: %d - To line: %d - To col: %d", myFromLine, myFromCol, 
        myToLine, myToCol);

    // Calculate the selection for lines for undo purposes
    if (myToCol > 0)
    {
    	myToLine++;
    }
    
    if ((myToLine - myFromLine) > 1)
    {
    	myResultCode = KEY_REDRAW_FROM_SELECTION;
    }

    //
    // Ensure every line inthe selection is a comment
    //
    for (myLine = myFromLine ; myLine < myToLine ; myLine++)
    {
    	myPtr = pmTextPtr -> text + pmTextPtr -> lineStarts [myLine];
    	while (*myPtr == ' ')
	{
    	    myPtr++;
	}
	if (((pmTextPtr -> isJava) && 
	     ((*myPtr != '/') || (*(myPtr + 1) != '/'))) ||
	    ((pmTextPtr -> isTuring) && (*myPtr != '%')))
	{
	    pmTextPtr -> selFrom = pmTextPtr -> lineStarts [myLine];
	    pmTextPtr -> selTo = pmTextPtr -> lineStarts [myLine + 1];
	    pmTextPtr -> locationOK = FALSE;
    	    return KEY_SOME_LINES_NOT_COMMENTS;
	}
    }
    
    //
    // Remove the comments
    //
    for (myLine = myFromLine ; myLine < myToLine ; myLine++)
    {
    	myPtr = pmTextPtr -> text + pmTextPtr -> lineStarts [myLine];
    	
    	// Skip the whitespace
    	while (*myPtr == ' ')
	{
    	    myPtr++;
	}
	
	// Were at the comment.  Delete the comment symbol, a trailing space
	// and if there's no text to the right, delete from the left
	myNextPtr = myPtr;
	myNextPtr += myCommentSize - 1;
	
	myCharsToDelete = myCommentSize - 1;
	
	if (*myNextPtr == ' ')
	{
	    myCharsToDelete++;
	}
	
	while (*myNextPtr == ' ')
	{
	    myNextPtr++;
	}
	if (*myNextPtr == '\n')
	{
	    myCharsToDelete = myNextPtr - myPtr;
	    myPtr--;
	    while ((myPtr >= pmTextPtr -> text) && (*myPtr == ' '))
	    {
	    	myCharsToDelete++;
	    	myPtr--;
	    }
	    myPtr++;
	}
	
	// Calculate the position
	myPos = myPtr - pmTextPtr -> text;
	
        // Delete the comment plus extra spaces
        memmove (myPtr, myPtr + myCharsToDelete, pmTextPtr -> textLen - myPos);

    	// Do the same for the textType block
    	myPtr = pmTextPtr -> textType + myPos;
    	memmove (myPtr, myPtr + myCharsToDelete, pmTextPtr -> textLen - myPos);

    	// Fix the line starts array
    	myResultCode |= MyLineStartsAdjustForDelete (pmTextPtr, 
            myPos, myPos + myCharsToDelete);
    
    	// Wipe out mark or error if on this line
    	myResultCode |= MyNotifyLinesModified (pmTextPtr, myLine, myLine);
    }
    
    // Set the comment selection's text type
    myFromPtr = pmTextPtr -> textType + pmTextPtr -> lineStarts [myFromLine];
    myToPtr = pmTextPtr -> textType + pmTextPtr -> lineStarts [myToLine];
    myPtr = myFromPtr;
    while (myPtr < myToPtr)
    {
    	*myPtr++ = TEXT_NO_CLASSIFICATION;
    }
    
    // Advance the selection
    pmTextPtr -> selFrom = pmTextPtr -> selTo = 
        pmTextPtr -> lineStarts [myToLine];
    pmTextPtr -> locationOK = FALSE;
    
    // Set dirty flag
    myResultCode |= MySetDirty (pmTextPtr);
   
    // Colour the new text
    if (MyClassifySection (pmTextPtr, pmTextPtr -> lineStarts [myFromLine]) == 
        CLASSIFY_CHANGE_MULTIPLE_LINES)
    {
    	myResultCode |= KEY_REDRAW_FROM_SELECTION;
    }
    
    // Debug Messages    
    MyAddTextState (pmTextPtr, "Uncomment Selection (3)");
    
    // Verify line database, if requested
    if (stProperties.verifyLineDB)
    {
    	MyVerifyLineDB (pmTextPtr);
    }
    
    return myResultCode;
} // EdText_SelectionUncomment


/************************************************************************/
/* EdText_SetNewText							*/
/************************************************************************/
void	EdText_SetNewText (TextPtr pmTextPtr, const char *pmBuffer, 
			   int pmFileSize, BOOL pmEraseUndo)
{
    char 	*mySrc;
    int		myLineLen, myNumChars;
    int		cnt;
    
    // Debug Messages    
    EdFail_AddMessage (NORMAL, "Set New Text (1) - Text: %x - size: %d", 
    		       pmTextPtr, pmFileSize);
    		       
    // Free up any allocated buffers.
    MyFreeTextMemory (pmTextPtr, pmEraseUndo);
    
    // Set the new text buffer    
    pmTextPtr -> textBufferLen = max ((int) (pmFileSize * 1.1), 
        INITIAL_TEXT_BUFFER_SIZE);
    pmTextPtr -> text = (char *) malloc (pmTextPtr -> textBufferLen);
    // TW Check for null
        
    pmTextPtr -> textLen = 0;
    pmTextPtr -> longestLine = 0;
    pmTextPtr -> longestLineLen = 0;
    pmTextPtr -> newLineAdded = FALSE;
    pmTextPtr -> isDirty = FALSE;
    pmTextPtr -> isChangedSinceLastRun = FALSE;
    pmTextPtr -> numLines = 0;
    pmTextPtr -> selFrom = 0;
    pmTextPtr -> selTo = 0;
    pmTextPtr -> virtualCol = DO_NOT_USE;

    pmTextPtr -> locationOK = FALSE;
    
    // Convert the buffer. \r and \r\n converted to \n
    MyConvertTabsAndNewLines (&(pmTextPtr -> text), 
    			      &(pmTextPtr -> textBufferLen),
    			      pmBuffer, pmFileSize, &(pmTextPtr -> numLines),
    			      &myNumChars, 0);
    			      
    // Handle the case where the file doesn't end with a newline    			      
    if ((myNumChars > 0) && (pmTextPtr -> text [myNumChars - 1] != '\n'))
    {
        pmTextPtr -> text [myNumChars] = '\n';
        myNumChars++;
        pmTextPtr -> numLines++;
        // TW Doesn't end in a newline warning
    }
    	        
    pmTextPtr -> textLen = myNumChars;
    pmTextPtr -> lineStartsBufferLen = pmTextPtr -> numLines + 
        LINE_BUFFER_SLACK;
    pmTextPtr -> lineStarts = 
        (int *) malloc (pmTextPtr -> lineStartsBufferLen * sizeof (int));

    pmTextPtr -> lastSavedTextLen = pmTextPtr -> textLen;
    pmTextPtr -> lastSavedCRC = EdText_CalculateCRC (pmTextPtr);

    mySrc = pmTextPtr -> text;
    pmTextPtr -> lineStarts [0] = 0;
    for (cnt = 1 ; cnt <= pmTextPtr -> numLines ; cnt++)
    {
    	while (*mySrc != '\n') 
    	{
    	    mySrc++;
    	}
	mySrc++;
	pmTextPtr -> lineStarts [cnt] = mySrc - pmTextPtr -> text;
	myLineLen = pmTextPtr -> lineStarts [cnt] -
	    pmTextPtr -> lineStarts [cnt - 1] - 1;
	if (myLineLen > pmTextPtr -> longestLineLen)
	{
	    pmTextPtr -> longestLine = cnt - 1;
	    pmTextPtr -> secondLongestLineLen = pmTextPtr -> longestLineLen;
	    pmTextPtr -> longestLineLen = myLineLen;
	}
	else if (myLineLen > pmTextPtr -> secondLongestLineLen)
	{
	    pmTextPtr -> secondLongestLineLen = myLineLen;
	}
    }

    // Allocate a new textType buffer
    pmTextPtr -> textType = (BYTE *) malloc (pmTextPtr -> textBufferLen);
    // TW Check for null
    
    MyAddNewLineIfNecessary (pmTextPtr, 0);

    // Process text.  Set the colours and make the mark menu.
    MyClassifyText (pmTextPtr);
    MyMarkText (pmTextPtr);
    
    // Debug Messages    
    MyAddTextState (pmTextPtr, "Set New Text (2)");
} // EdText_SetNewText
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)	


/************************************************************************/
/* EdText_SetSelection							*/
/************************************************************************/
BOOL	EdText_SetSelection (TextPtr pmTextPtr, int pmLine, int pmCol)
{
    BOOL	myWasSelection, myDummy;
    int		myLoc;
    
#if !defined (CONSOLE) && !defined (TPROLOG)	
    // Debug Messages    
    EdFail_AddMessage (NORMAL, "Set Selection (1) - Text: %x - L: %d - C: %d", 
    		       pmTextPtr, pmLine, pmCol);
    MyAddTextState (pmTextPtr, NULL);	    	       
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)	

    // Do selection
    myWasSelection = (pmTextPtr -> selFrom != pmTextPtr -> selTo);

    MyFindLocationPos (pmTextPtr, pmLine, pmCol, &myLoc, &myDummy);
    pmTextPtr -> selFrom = pmTextPtr -> selTo = myLoc;  
    pmTextPtr -> virtualCol = DO_NOT_USE;
    pmTextPtr -> locationOK = FALSE;
    
    pmTextPtr -> selWordFrom = DO_NOT_USE;
    pmTextPtr -> selWordTo = DO_NOT_USE;
    
#if !defined (CONSOLE) && !defined (TPROLOG)	
    // Debug Messages    
    MyAddTextState (pmTextPtr, "Set Selection (2)");
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)	
    
    return myWasSelection;
} // EdText_SetSelection


#if !defined (CONSOLE) && !defined (TPROLOG)	
/************************************************************************/
/* EdText_SetSelection1							*/
/************************************************************************/
BOOL	EdText_SetSelection1 (TextPtr pmTextPtr, int pmSelFrom, int pmSelTo)
{
    BOOL	myWasSelection;
    
    // Debug Messages    
    EdFail_AddMessage (NORMAL, "Set Selection1 (1) - Text: %x - From: %d"
    		       " - To: %d", pmTextPtr, pmSelFrom, pmSelTo);
    MyAddTextState (pmTextPtr, NULL);	    	       

    // Do selection
    myWasSelection = (pmTextPtr -> selFrom != pmTextPtr -> selTo);

    pmTextPtr -> selFrom = pmSelFrom;
    pmTextPtr -> selTo = pmSelTo;  
    pmTextPtr -> virtualCol = DO_NOT_USE;
    pmTextPtr -> locationOK = FALSE;
    
    pmTextPtr -> selWordFrom = DO_NOT_USE;
    pmTextPtr -> selWordTo = DO_NOT_USE;
    
    return myWasSelection;
} // EdText_SetSelection1


/************************************************************************/
/* EdText_ToggleBreakpoint						*/
/*									*/
/* Set a breakpoint if it doesn't exist on the line, otherwise remove	*/
/* a breakpoint if it does exist on the line.				*/
/************************************************************************/
void	EdText_ToggleBreakpoint (TextPtr pmTextPtr, int pmLine)
{
    BreakLinePtr	myPrevBreakLine = NULL;
    BreakLinePtr        myBreakLine = pmTextPtr -> breakLine;
    BOOL		myAdd = TRUE;
    
    while (myBreakLine != NULL)
    {
    	if (myBreakLine -> line == pmLine)
    	{
    	    myAdd = FALSE;
    	}	
    	if (myBreakLine -> line >= pmLine)
    	{
    	    break;
    	}
    	myPrevBreakLine = myBreakLine;
    	myBreakLine = myBreakLine -> next;
    }
    
    if (myAdd)
    {
    	myBreakLine = (BreakLinePtr) malloc (sizeof (BreakLine));
    	myBreakLine -> line = pmLine;
    	
    	if (myPrevBreakLine == NULL)
    	{
    	    myBreakLine -> next = pmTextPtr -> breakLine;
    	    pmTextPtr -> breakLine = myBreakLine;
    	}
    	else
    	{
    	    myBreakLine -> next = myPrevBreakLine -> next;
    	    myPrevBreakLine -> next = myBreakLine;
    	}
    }
    else
    {
    	if (myPrevBreakLine == NULL)
    	{
    	    pmTextPtr -> breakLine = myBreakLine -> next;
    	}
    	else
    	{
    	    myPrevBreakLine -> next = myBreakLine -> next;
    	}
    	free (myBreakLine);
    }
} // EdText_ToggleBreakpoint


/************************************************************************/
/* EdText_Undo								*/
/*									*/
/* Undo according to the last operation on the undo stack and place it	*/
/* on the redo stack or the reverse.					*/
/************************************************************************/
void	EdText_Undo (TextPtr pmTextPtr, BOOL pmUndo)
{
    UndoBufferPtr	myUndoPtr;
    int			myUndoReplFrom;	// Undo replaces from undoReplFrom
    int			myUndoReplTo;	// Undo replaces to undoReplTo
    char		*myUndoText;	// Undo replaces with this text
    int			myUndoTextSize;	// The Undo replace text size
    int			myUndoSelFrom;	// The selection from to restore 
    int			myUndoSelTo;	// The selection to to restore

    // Debug Messages    
    EdFail_AddMessage (NORMAL, "Undo (1) - Text: %x - Undo: %d", 
    		       pmTextPtr, pmUndo);
    MyAddTextState (pmTextPtr, NULL);	    	       

    // Do the undo!
    if (pmUndo)
    {
	myUndoPtr = pmTextPtr -> undoTail;
	pmTextPtr -> undoTail = myUndoPtr -> prev;
	myUndoPtr -> prev = pmTextPtr -> redoTail;
	if (pmTextPtr -> undoTail != NULL)
	{
	    pmTextPtr -> undoTail -> next = NULL;
	}
	else
	{
	    pmTextPtr -> undoHead = NULL;
	}
    }
    else
    {
	myUndoPtr = pmTextPtr -> redoTail;
	pmTextPtr -> redoTail = myUndoPtr -> prev;
	myUndoPtr -> prev = pmTextPtr -> undoTail;
	if (pmTextPtr -> redoTail != NULL)
	{
	    pmTextPtr -> redoTail -> next = NULL;
	}
	else
	{
	    pmTextPtr -> redoHead = NULL;
	}
    }

    // Save the original state
    myUndoReplFrom = myUndoPtr -> replFrom;
    myUndoReplTo = myUndoPtr -> replTo;
    myUndoText = myUndoPtr -> text;
    myUndoTextSize = myUndoPtr -> textSize;
    myUndoSelFrom = myUndoPtr -> selFrom;
    myUndoSelTo = myUndoPtr -> selTo;
    
    myUndoPtr -> selFrom = pmTextPtr -> selFrom;
    myUndoPtr -> selTo = pmTextPtr -> selTo;
	
    myUndoPtr -> textSize = myUndoReplTo - myUndoReplFrom;
    	
    // Debugging information
    EdFail_AddMessage (NORMAL, "Undo (2) - Undo size: %d - Repl from: %d"
    		       " - Repl to: %d", myUndoTextSize, myUndoReplFrom, 
    		       myUndoReplTo);

    if (myUndoPtr -> textSize > 0)
    {
    	myUndoPtr -> text = malloc (myUndoPtr -> textSize);
    	if (myUndoPtr -> text == NULL)
    	{
	    // TW Dialog box indicating not enough mem
	    // TW Free the entire redo buffer
	    free (myUndoPtr);
	    return;
    	}
    }
    else
    {
    	myUndoPtr -> text = NULL;
    }

    // Set the new selection
    pmTextPtr -> selFrom = myUndoPtr -> replFrom;
    pmTextPtr -> selTo = myUndoPtr -> replTo;

    // TW Adjust marks and errors to take into account deletion and insertion
	
    // Text has been added since the undo start that must now be deleted
    if (myUndoPtr -> replTo > myUndoPtr -> replFrom)
    {
        if (myUndoPtr -> text != NULL)
        {
	    memcpy (myUndoPtr -> text, 
	            &pmTextPtr -> text [myUndoReplFrom],
	            myUndoPtr -> textSize);
	}
	EdText_SelectionDelete (pmTextPtr);
    }
    if (myUndoTextSize > 0)
    {
	EdText_InsertText (pmTextPtr, myUndoText, myUndoTextSize, TRUE, 
	    	NULL);
    }
    myUndoPtr -> replTo = pmTextPtr -> selTo;
	
    // Set the selection back to what it was
    pmTextPtr -> selFrom = myUndoSelFrom;
    pmTextPtr -> selTo = myUndoSelTo;
    pmTextPtr -> locationOK = FALSE;
    free (myUndoText);
    
    if (myUndoPtr -> menuName >= IDS_REDO_BASE)
    {
    	myUndoPtr -> menuName -= (IDS_REDO_BASE - IDS_UNDO_BASE);
    }
    else
    {
    	myUndoPtr -> menuName += (IDS_REDO_BASE - IDS_UNDO_BASE);
    }
    
    // Now add the new Undo Buffer to the appropriate list
    if (pmUndo)
    {
	if (pmTextPtr -> redoTail == NULL)
	{
	    pmTextPtr -> redoHead = pmTextPtr -> redoTail = myUndoPtr;
	}
	else
	{
	    pmTextPtr -> redoTail -> next = myUndoPtr;
	    myUndoPtr -> prev = pmTextPtr -> redoTail;
	    myUndoPtr -> next = NULL;
	    pmTextPtr -> redoTail = myUndoPtr;
	}
    }
    else
    {
	if (pmTextPtr -> undoTail == NULL)
	{
	    pmTextPtr -> undoHead = pmTextPtr -> undoTail = myUndoPtr;
	}
	else
	{
	    pmTextPtr -> undoTail -> next = myUndoPtr;
	    myUndoPtr -> prev = pmTextPtr -> undoTail;
	    myUndoPtr -> next = NULL;
	    pmTextPtr -> undoTail = myUndoPtr;
	}
    }

    // Debug Messages    
    MyAddTextState (pmTextPtr, "Undo (3)");
    
    // Verify line database, if requested
    if (stProperties.verifyLineDB)
    {
    	MyVerifyLineDB (pmTextPtr);
    }
} // EdText_Undo

/************************************************************************/
/* EdText_UndoClear							*/
/************************************************************************/
/* TW
void	EdText_UndoClear (TextPtr pmTextPtr)
{
    UndoBufferPtr   myUndoPtr;

    myUndoPtr = pmTextPtr -> undoTail;
    pmTextPtr -> undoTail = myUndoPtr -> prev;
    if (pmTextPtr -> undoTail != NULL)
    {
	pmTextPtr -> undoTail -> next = NULL;
    }
    else
    {
	pmTextPtr -> undoHead = NULL;
    }

    // Free the current undo buffer
    if (myUndoPtr -> text != NULL)
    {
	free (myUndoPtr -> text);
    }
    free (myUndoPtr);
} // EdText_UndoClear
*/

/************************************************************************/
/* EdText_UndoFreeLastBuffer						*/
/************************************************************************/
void	EdText_UndoFreeLastBuffer (TextPtr pmTextPtr)
{
    UndoBufferPtr	myUndoPtr;

    myUndoPtr = pmTextPtr -> undoHead;
    
    if (myUndoPtr != NULL)
    {
	pmTextPtr -> undoHead = myUndoPtr -> next;
	if (pmTextPtr -> undoHead == NULL)
	{
	    pmTextPtr -> undoTail = NULL;
	}

	if (myUndoPtr -> text != NULL)
	{
	    free (myUndoPtr -> text);
	}
	free (myUndoPtr);
    }
} // EdText_UndoFreeLastBuffer


/************************************************************************/
/* EdText_UndoSaveState							*/
/************************************************************************/
void	EdText_UndoSaveState (TextPtr pmTextPtr, int pmUndoMenuName)
{
    UndoBufferPtr	myUndoPtr;
    int			myCopyFrom;

    // No more redoing possible    
    MyFreeRedoBuffers (pmTextPtr);
    
    // Allocate a new undo buffer
    myUndoPtr = (UndoBufferPtr) malloc (sizeof (UndoBuffer));

    myUndoPtr -> menuName = pmUndoMenuName;
    
    myUndoPtr -> selFrom = pmTextPtr -> selFrom;
    myUndoPtr -> selTo = pmTextPtr -> selTo;
    myUndoPtr -> replFrom = min (pmTextPtr -> selFrom, pmTextPtr -> selTo);
    myUndoPtr -> replTo = myUndoPtr -> replFrom;
    myUndoPtr -> moreKeystrokes = FALSE;
    myUndoPtr -> next = NULL;
    myUndoPtr -> prev = pmTextPtr -> undoTail;

    myUndoPtr -> textSize = abs (pmTextPtr -> selTo - pmTextPtr -> selFrom);
    if (myUndoPtr -> textSize == 0)
    {
    	myUndoPtr -> text = NULL;
    }
    else
    {
    	myUndoPtr -> text = malloc (myUndoPtr -> textSize);
	if (myUndoPtr -> text == NULL)
	{
	    // TW Not enough memory dialog
	    // Free up the entire undo list
	    free (myUndoPtr);
	    return;
	}
    }

    myCopyFrom = min (pmTextPtr -> selFrom, pmTextPtr -> selTo);
    memcpy (myUndoPtr -> text, &pmTextPtr -> text [myCopyFrom],
        myUndoPtr -> textSize);
    
    // Add the undo buffer to the end of the undo buffer
    if (pmTextPtr -> undoTail == NULL)
    {
	pmTextPtr -> undoHead = pmTextPtr -> undoTail = myUndoPtr;
    }
    else
    {
	pmTextPtr -> undoTail -> next = myUndoPtr;
	pmTextPtr -> undoTail = myUndoPtr;
    }
} // EdText_UndoSaveState


/************************************************************************/
/* EdText_UndoSaveSpecified						*/
/*									*/
/* Indicate that when the operation is undone, it must restore all the	*/
/* text currently located at pmFromPos - pmToPos			*/
/*									*/
/* The caller is responsible for undoReplFrom and undoReplTo		*/
/************************************************************************/
void	EdText_UndoSaveSpecified (TextPtr pmTextPtr, int pmUndoMenuName, 
				  int pmFromPos, int pmToPos)
{
    UndoBufferPtr	myUndoPtr;

    // No more redoing possible    
    MyFreeRedoBuffers (pmTextPtr);
    
    // Allocate a new undo buffer
    myUndoPtr = (UndoBufferPtr) malloc (sizeof (UndoBuffer));

    myUndoPtr -> menuName = pmUndoMenuName;
    
    if (pmFromPos == UNDO_ALL_TEXT)
    {
    	pmFromPos = 0;
    	pmToPos = pmTextPtr -> textLen;
    } 
    
    myUndoPtr -> selFrom = pmTextPtr -> selFrom;
    myUndoPtr -> selTo = pmTextPtr -> selTo;
    myUndoPtr -> moreKeystrokes = FALSE;
    myUndoPtr -> next = NULL;
    myUndoPtr -> prev = pmTextPtr -> undoTail;

    myUndoPtr -> textSize = pmToPos - pmFromPos;
    if (myUndoPtr -> textSize == 0)
    {
    	myUndoPtr -> text = NULL;
    }
    else
    {
    	myUndoPtr -> text = malloc (myUndoPtr -> textSize);
	if (myUndoPtr -> text == NULL)
	{
	    // TW Not enough memory dialog
	    // Free up the entire undo list
	    free (myUndoPtr);
	    return;
	}
    }

    memcpy (myUndoPtr -> text, &pmTextPtr -> text [pmFromPos], 
	    myUndoPtr -> textSize);
    
    // Add the undo buffer to the end of the undo buffer
    if (pmTextPtr -> undoTail == NULL)
    {
	pmTextPtr -> undoHead = pmTextPtr -> undoTail = myUndoPtr;
    }
    else
    {
	pmTextPtr -> undoTail -> next = myUndoPtr;
	myUndoPtr -> prev = pmTextPtr -> undoTail;
	myUndoPtr -> next = NULL;
	pmTextPtr -> undoTail = myUndoPtr;
    }
} // EdText_UndoSaveSpecified


/************************************************************************/
/* EdText_UndoSetMoreKeystrokes						*/
/************************************************************************/
void	EdText_UndoSetMoreKeystrokes (TextPtr pmTextPtr, 
				      BOOL pmMoreKeystrokes)
{
    UndoBufferPtr   myUndoPtr = pmTextPtr -> undoTail;

    if (pmMoreKeystrokes && (myUndoPtr == NULL))
    {
	// TW Assertion failure
    }

    if (myUndoPtr != NULL)
    {
	myUndoPtr -> moreKeystrokes = pmMoreKeystrokes;
    }
} // EdText_UndoSetMoreKeystrokes


/************************************************************************/
/* EdText_UndoSetReplFrom						*/
/************************************************************************/
void	EdText_UndoSetReplFrom (TextPtr pmTextPtr, int pmReplFrom)
{
    if (pmTextPtr -> undoTail != NULL)
    {
	pmTextPtr -> undoTail -> replFrom = pmReplFrom;
    }
} // EdText_UndoSetReplFrom


/************************************************************************/
/* EdText_UndoSetReplTo							*/
/************************************************************************/
void	EdText_UndoSetReplTo (TextPtr pmTextPtr, int pmReplTo)
{
    if (pmTextPtr -> undoTail != NULL)
    {
	pmTextPtr -> undoTail -> replTo = pmReplTo;
    }
} // EdText_UndoSetReplTo


#endif // #if !defined (CONSOLE) && !defined (TPROLOG)	


/*********************/
/* Static procedures */
/*********************/
/************************************************************************/
/* MyAddNewLineIfNecessary						*/
/*									*/
/* Adds a newline to the end of the file if the file doesn't already	*/
/* end in a newline.  If a newline has been previously added and is no	*/
/* longer needed, then remove it.					*/
/************************************************************************/
static void	MyAddNewLineIfNecessary (TextPtr pmTextPtr, int pmSelAdjust)
{
    int	myLen = pmTextPtr -> textLen;
    
    if (pmTextPtr -> newLineAdded && (myLen > 1) &&
        (pmTextPtr -> text [myLen - 2] == '\n') &&
        (pmTextPtr -> text [myLen - 1] == '\n'))
    {
	if ((pmTextPtr -> selFrom + pmSelAdjust) != myLen)
	{
    	    pmTextPtr -> textLen--;
    	    pmTextPtr -> numLines--;
	}
    	
    	// Set newLineAdded flag
        pmTextPtr -> newLineAdded = FALSE;
        return;
    }
        
    if ((pmTextPtr -> textLen > 0) && 
        (pmTextPtr -> text [myLen - 1] != '\n'))
    {
    	// We cheat here.  We know that the added newline will be well within
    	// the slack space, so we don't bother checking for buffer or line
    	// starts limits.
    	// Add a newline
    	pmTextPtr -> text [myLen] = '\n';
#if !defined (CONSOLE) && !defined (TPROLOG)	
    	pmTextPtr -> textType [myLen] = TEXT_NO_CLASSIFICATION;
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)	
    	pmTextPtr -> textLen++;
    	pmTextPtr -> numLines++;
    	pmTextPtr -> lineStarts [pmTextPtr -> numLines] = pmTextPtr -> textLen;
	    	
    	// Set newLineAdded flag
        pmTextPtr -> newLineAdded = TRUE;
    }
} // MyAddNewLineIfNecessary


#if !defined (CONSOLE) && !defined (TPROLOG)
/************************************************************************/
/* MyAddTextState							*/
/************************************************************************/
static void	MyAddTextState (TextPtr pmTextPtr, const char *pmString)
{
    int	myNL = pmTextPtr -> numLines;
    
    if (pmString != NULL)
    {
    	if (pmTextPtr -> locationOK)
    	{
    	    EdFail_AddMessage (EXTRA_MESSAGE, 
    	        "%s - sz:%d:%d - slFr:%d:%d:%d - slTo:%d:%d:%d - nlA:%d",
    	        pmString, pmTextPtr -> numLines, pmTextPtr -> textLen, 
    	        pmTextPtr -> lastLineFrom, pmTextPtr -> lastColFrom, 
    	        pmTextPtr -> selFrom,
    	        pmTextPtr -> lastLineTo, pmTextPtr -> lastColTo, 
    	        pmTextPtr -> selTo, pmTextPtr -> newLineAdded);
    	}
    	else
    	{
    	    EdFail_AddMessage (EXTRA_MESSAGE, 
    	    	"%s - sz:%d:%d - slFr:%d - slTo:%d - nlA:%d",
    	        pmString, pmTextPtr -> numLines, pmTextPtr -> textLen, 
    	        pmTextPtr -> selFrom, pmTextPtr -> selTo, 
    	        pmTextPtr -> newLineAdded);
    	}
    }
    else
    {
    	if (pmTextPtr -> locationOK)
    	{
    	    EdFail_AddMessage (EXTRA_MESSAGE,
    	    	"Text - sz:%d:%d - slFr:%d:%d:%d - slTo:%d:%d:%d - nlA:%d",
    	        pmTextPtr -> numLines, pmTextPtr -> textLen, 
    	        pmTextPtr -> lastLineFrom, pmTextPtr -> lastColFrom, 
    	        pmTextPtr -> selFrom,
    	        pmTextPtr -> lastLineTo, pmTextPtr -> lastColTo, 
    	        pmTextPtr -> selTo, pmTextPtr -> newLineAdded);
    	}
    	else
    	{
    	    EdFail_AddMessage (EXTRA_MESSAGE, 
    	    	"Text - sz:%d:%d - slFr:%d - slTo:%d - nlA:%d",
    	        pmTextPtr -> numLines, pmTextPtr -> textLen, 
    	        pmTextPtr -> selFrom, pmTextPtr -> selTo, 
    	        pmTextPtr -> newLineAdded);
    	}
    }
    
    if (myNL >= 3)
    {
    	EdFail_AddMessage (EXTRA_MESSAGE, 
    	    "Line Starts: %d:%d %d:%d %d:%d %d:%d",
    	    myNL - 3, pmTextPtr -> lineStarts [myNL - 3],
    	    myNL - 2, pmTextPtr -> lineStarts [myNL - 2],
    	    myNL - 1, pmTextPtr -> lineStarts [myNL - 1],
    	    myNL, pmTextPtr -> lineStarts [myNL]);
    }
    else if (myNL == 2)
    {
    	EdFail_AddMessage (EXTRA_MESSAGE, "Line Starts: %d:%d %d:%d %d:%d",
    	    myNL - 2, pmTextPtr -> lineStarts [myNL - 2],
    	    myNL - 1, pmTextPtr -> lineStarts [myNL - 1],
    	    myNL, pmTextPtr -> lineStarts [myNL]);
    }
    else if (myNL == 1)
    {
    	EdFail_AddMessage (EXTRA_MESSAGE, "Line Starts: %d:%d %d:%d",
    	    myNL - 1, pmTextPtr -> lineStarts [myNL - 1],
    	    myNL, pmTextPtr -> lineStarts [myNL]);
    }
    else
    {
    	EdFail_AddMessage (EXTRA_MESSAGE, "Line Starts: %d:%d",
    	    myNL, pmTextPtr -> lineStarts [myNL]);
    }
} // MyAddTextState


/************************************************************************/
/* MyClassifyText							*/
/*									*/
/* This function sets the appropriate colours for the text.		*/
/************************************************************************/
static void	MyClassifyText (TextPtr pmTextPtr)
{
    BYTE		*myCharTags;
    unsigned char	*myPtr = pmTextPtr -> text;
    BYTE		*myTextType = pmTextPtr -> textType;
    char		*myEnd = myPtr + pmTextPtr -> textLen;
    char		*myStart;
    int			myLen;
    BYTE		myIDTextType;
    char		myIdentifier [100];
    int			cnt;

    if (pmTextPtr -> isJava)
    {
    	myCharTags = stJavaCharTags;
    }
    else if (pmTextPtr -> isTuring)
    {
    	myCharTags = stTuringCharTags;
    }
    else
    {
    	return;
    }
    
    while (myPtr < myEnd)
    {
    	switch (myCharTags [*myPtr])
    	{
    	    case CH_NRM: // normal char
    	    case CH_OPA: // open parenthesis
    	    case CH_CPA: // close parenthesis
    	    case CH_OBR: // open brace
    	    case CH_CBR: // close brace
    	    case CH_WSP: // white space
    	    case CH_IDC: // id continue
    	        *myTextType++ = TEXT_NORMAL;
    	        myPtr++;
    	        break;
    	    case CH_DQT: // "
    	        *myTextType++ = TEXT_DQSTRING;
    	        myPtr++;
    	        while ((*myPtr != '\n') && (*myPtr != '"'))
    	        {
    	            if (*myPtr == '\\')
    	            {
    	            	*myTextType++ = TEXT_DQSTRING;
    	            	myPtr++;
    	            }
    	            *myTextType++ = TEXT_DQSTRING;
    	            myPtr++;
    	        }
    	        if (*myPtr == '"')
    	        {
    	            *myTextType++ = TEXT_DQSTRING_CLOSE;
    	            myPtr++;
    	        }
    	        break;
    	    case CH_SQT: // '
    	        *myTextType++ = TEXT_SQSTRING;
    	        myPtr++;
    	        while ((*myPtr != '\n') && (*myPtr != '\''))
    	        {
    	            if (*myPtr == '\\')
    	            {
    	            	*myTextType++ = TEXT_SQSTRING;
    	            	myPtr++;
    	            }
    	            *myTextType++ = TEXT_SQSTRING;
    	            myPtr++;
    	        }
    	        if (*myPtr == '\'')
    	        {
    	            *myTextType++ = TEXT_SQSTRING_CLOSE;
    	            myPtr++;
    	        }
    	        break;
    	    case CH_PCT: // %
    	        do
    	        {
     	            *myTextType++ = TEXT_LINE_COMMENT;
     	            myPtr++;
     	        } while (*myPtr != '\n');
    	        break;
    	    case CH_IDS: // id start
    	        myStart = myPtr;
    	        while ((myCharTags [*myPtr] == CH_IDS) || 
    	               (myCharTags [*myPtr] == CH_IDC) ||
    	               (*myPtr == '.') || 
    	               ((myPtr > pmTextPtr -> text) && (*(myPtr - 1) == '.') && 
    	                (*myPtr == '*')))
    	        {
    	            myPtr++;
    	        }
    	        myLen = myPtr - myStart;
    	        
   	        // Check for the situation "this.x"
    	        if (pmTextPtr -> isJava && (myLen >= 5) &&
    	            (strncmp (myStart, "this.", 5) == 0))
		{
		    for (cnt = 0 ; cnt < 4 ; cnt++)
    	            {
    	            	*myTextType++ = TEXT_KEYWORD;
    	            }
    	            *myTextType++ = TEXT_NORMAL;
    	            for (cnt = 0 ; cnt < myLen - 5 ; cnt++)
    	            {
    	            	*myTextType++ = TEXT_IDENTIFIER;
    	            }
    	            break;
		}    	             
   	        // Check for the situation "super.x"
     	        if (pmTextPtr -> isJava && (myLen >= 6) &&
    	            (strncmp (myStart, "super.", 6) == 0))
		{
		    for (cnt = 0 ; cnt < 5 ; cnt++)
    	            {
    	            	*myTextType++ = TEXT_KEYWORD;
    	            }
    	            *myTextType++ = TEXT_NORMAL;
    	            for (cnt = 0 ; cnt < myLen - 6 ; cnt++)
    	            {
    	            	*myTextType++ = TEXT_IDENTIFIER;
    	            }
    	            break;
		}    	             

    	        // If the identifier is too long, it's not a keyword or predef
    	        if (myLen > 50)
    	        {
    	            myIDTextType = TEXT_IDENTIFIER;
    	        }
    	        else
    	        {
    	            strncpy (myIdentifier, myStart, myLen);
    	            myIdentifier [myLen] = 0;
    	            if ((pmTextPtr -> isJava && 
    	                 MyIsJavaKeyword (myIdentifier)) ||
    	                (pmTextPtr -> isTuring && 
    	                 MyIsTuringKeyword (myIdentifier)))
    	            {
    	            	myIDTextType = TEXT_KEYWORD;
    	            }
    	            else if ((pmTextPtr -> isJava && 
    	                      MyIsJavaPredef (myIdentifier)) ||
    	                     (pmTextPtr -> isTuring && 
    	                      MyIsTuringPredef (myIdentifier)))
    	            {
    	            	myIDTextType = TEXT_PREDEF;
    	            }
    	            else
    	            {
    	            	myIDTextType = TEXT_IDENTIFIER;
    	            }
    	        }
    	        for (cnt = 0 ; cnt < myLen ; cnt++)
    	        {
    	            *myTextType++ = myIDTextType;
    	        }
    	        break;
    	    case CH_SLS: // /
		// Check for "//"
    	        if ((pmTextPtr -> isJava) && (*(myPtr + 1) == '/'))
    	        {
    	            *myTextType++ = TEXT_LINE_COMMENT;
	    	    // The second "/" in "//"
    	            *myTextType++ = TEXT_LINE_COMMENT_OPEN;
    	            myPtr += 2;
    	            while (*myPtr != '\n')
    	            {
     	            	*myTextType++ = TEXT_LINE_COMMENT;
     	            	myPtr++;
     	            }
    	            break;
    	        }
		// Check for "/*"
     	        else if (*(myPtr + 1) == '*')
    	        {
    	            *myTextType++ = TEXT_LONG_COMMENT;
	    	    // The "*" in "/*"
    	            *myTextType++ = TEXT_LONG_COMMENT_OPEN;
    	            myPtr += 2;
    	            while (myPtr < myEnd) 
    	            {
    	            	if ((*myPtr == '*') && (*(myPtr + 1) == '/'))
    	            	{
	    	            *myTextType++ = TEXT_LONG_COMMENT;
	    	            // The "/" in "*/"
	    	            *myTextType++ = TEXT_LONG_COMMENT_CLOSE;
	    	            myPtr += 2;
	    	            break;
	    	        }
	    	        *myTextType++ = TEXT_LONG_COMMENT;
	    	        myPtr++;
	    	    } // while
    	        }
   	        else
    	        {
     	            *myTextType++ = TEXT_NORMAL;
     	            myPtr++;
     	        }
    	        break;
    	    case CH_ERR: // error
	    default:
    	        EdFail_Fatal (IDS_UNEXPECTED_CHAR, __FILE__, __LINE__, 0,
    	            *myPtr);
    	        break;
    	} // switch
    } // while
} // MyClassifyText


/************************************************************************/
/* MyClassifySection							*/
/*									*/
/* This function classifies a section of text into its type.  It starts	*/
/* the pmStartPos, although, if may backtrack a few characters if	*/
/* necessary.  It classifies from the pmStartPos onward until the new	*/
/* classification of the text matches the old classification, in which 	*/
/* case it's no longer need continue.					*/
/************************************************************************/
static int	MyClassifySection (TextPtr pmTextPtr, int pmStartPos)
{
    BYTE		*myCharTags;
    unsigned char	*myPtr = pmTextPtr -> text + pmStartPos;
    BYTE		*myTextType = pmTextPtr -> textType + pmStartPos;
    char		*myEnd = pmTextPtr -> text + pmTextPtr -> textLen;
    char		*myStart;
    BYTE		myTag;
    BYTE		myIDTextType;
    BOOL		myStartingInMiddle = FALSE;
    int			myLen;
    char		myIdentifier [100];
    int			myResultCode = CLASSIFY_CHANGED_THIS_LINE;
    int			cnt;

    if (pmTextPtr -> isJava)
    {
    	myCharTags = stJavaCharTags;
    }
    else if (pmTextPtr -> isTuring)
    {
    	myCharTags = stTuringCharTags;
    }
    else
    {
    	return 0;
    }
    
    //
    // First, back one space if new char is a / (might be second char of a //
    // or */) or a * (might be second char of a /*). If it's an CH_IDS
    // (identifier start), CH_IDC (identifier continuer) or a CH_PRD (a
    // period), back up until it's not any of the three. (This allows the
    // reclassification of keywords, identifiers, etc.
    //
    if (pmStartPos > 0) 
    {
    	// Check to see if we've just added the "/" in a "*/" AND the "*"
    	// is not part of a "/*"
    	if ((*(myPtr - 1) == '*') && (*myPtr == '/') &&
    	    (*(myTextType - 1) != TEXT_LONG_COMMENT_OPEN))
    	{
    	    myPtr--;
    	    myTextType--;
    	    *myTextType = TEXT_NO_CLASSIFICATION;
    	}
	// Check to see if we've just added the "/" in a "//" AND the 
	// second "/" is not part of a "*/" or a "//"
    	else if ((pmTextPtr -> isJava) &&
    		 ((*(myPtr - 1) == '/') && (*myPtr == '/') &&
    	          (*(myTextType - 1) != TEXT_LONG_COMMENT_CLOSE) &&
    	          (*(myTextType - 1) != TEXT_LINE_COMMENT_OPEN)))
    	{
    	    myPtr--;
    	    myTextType--;
    	    *myTextType = TEXT_NO_CLASSIFICATION;
    	}
	// Check to see if we've just added the "*" in a "/*" AND the "/"
	// is not part of a "*/" or a "//"
    	else if ((*(myPtr - 1) == '/') && (*myPtr == '*') &&
    	         (*(myTextType - 1) != TEXT_LONG_COMMENT_CLOSE) &&
    	         (*(myTextType - 1) != TEXT_LINE_COMMENT_OPEN))
    	{
    	    myPtr--;
    	    myTextType--;
    	    *myTextType = TEXT_NO_CLASSIFICATION;
    	}
    	// Check to see if we've just eliminate the "*" in "/*".  We determine
    	// this by checking to see if the character before the startPos is
    	// a comment but the one before that is not.
    	else if ((*(myTextType - 1) == TEXT_LONG_COMMENT) && 
	    	 (pmStartPos > 1) && 
	    	 ((*(myTextType - 2) != TEXT_LONG_COMMENT) &&
	    	  (*(myTextType - 2) != TEXT_LONG_COMMENT_OPEN)))
    	{
    	    myPtr--;
    	    myTextType--;
    	    *myTextType = TEXT_NO_CLASSIFICATION;
    	}
    	// Check to see if we've just eliminate the second "/" in "//".  We 
    	// determine this by checking to see if the character before 
    	// the startPos is a comment but the one before it is not.
    	else if ((pmTextPtr -> isJava) && 
    		 ((*(myTextType - 1) == TEXT_LINE_COMMENT) && 
	    	  (pmStartPos > 1) && 
	    	  ((*(myTextType - 2) != TEXT_LINE_COMMENT) &&
	    	   (*(myTextType - 2) != TEXT_LINE_COMMENT_OPEN))))
    	{
    	    myPtr--;
    	    myTextType--;
    	    *myTextType = TEXT_NO_CLASSIFICATION;
    	}
    	else
    	{
    	    myPtr--;
    	    myTextType--;
    	    
            while ((myTextType >= pmTextPtr -> textType) &&
            	   ((*myTextType == TEXT_KEYWORD) || 
    	            (*myTextType == TEXT_PREDEF) ||
    	            (*myTextType == TEXT_IDENTIFIER)))
	    {
	    	myPtr--;
    	        *myTextType = TEXT_NO_CLASSIFICATION;
    	        myTextType--;
	    }
	    
	    myPtr++;
    	    myTextType++;
	}
       
        // We need to set the initial tag to take into account we may already be
        // in a double quote string, a single quote string, a short comment or
    	// a long comment.  We also need a marker whether we are faking the
    	// tag to enter into the middle of a longer item.
    	if ((myTextType > pmTextPtr -> textType) &&
    	    (*(myTextType - 1) == TEXT_DQSTRING))
    	{
	    // If the previous character was a \, move back one space
	    if (*(myPtr - 1) == '\\')
	    {
		myPtr--;
		myTextType--;
    	        *myTextType = TEXT_NO_CLASSIFICATION;
	    }
    	    myTag = CH_DQT;
    	    myStartingInMiddle = TRUE;
        }
    	else if ((myTextType > pmTextPtr -> textType) &&
    		 (*(myTextType - 1) == TEXT_SQSTRING))
        {
	    // If the previous character was a \, move back one space
	    if (*(myPtr - 1) == '\\')
	    {
		myPtr--;
		myTextType--;
    	        *myTextType = TEXT_NO_CLASSIFICATION;
	    }
    	    myTag = CH_SQT;
    	    myStartingInMiddle = TRUE;
    	}
    	// If the previous character is a line comment, then in Java we want
    	// to make certain that it's not the first "/" in "//".
    	else if ((pmTextPtr -> isJava) &&
    		 ((myTextType > pmTextPtr -> textType) &&
    		  ((*(myTextType - 1) == TEXT_LINE_COMMENT) || 
    		   (*(myTextType - 1) == TEXT_LINE_COMMENT_OPEN))))
    	{
    	    myTag = CH_SLS;
    	    myStartingInMiddle = TRUE;
    	}
    	else if ((pmTextPtr -> isTuring) && 
    		 ((myTextType > pmTextPtr -> textType) &&
    		  (*(myTextType - 1) == TEXT_LINE_COMMENT)))
    	{
    	    myTag = CH_PCT;
    	    myStartingInMiddle = TRUE;
    	}
    	else if ((myTextType > pmTextPtr -> textType) &&
    		 ((*(myTextType - 1) == TEXT_LONG_COMMENT) || 
    		  (*(myTextType - 1) == TEXT_LONG_COMMENT_OPEN)))
    	{
    	    myTag = CH_SLS;
    	    myStartingInMiddle = TRUE;
    	}
    	else
    	{
    	    myTag = myCharTags [*myPtr];
    	}
    }
    else
    {
    	myTag = myCharTags [*myPtr];
    }

    //
    // The main loop
    //
    while (myPtr < myEnd)
    {
    	switch (myTag)
    	{
    	    case CH_NRM: // normal char
    	    case CH_OPA: // open parenthesis
    	    case CH_CPA: // close parenthesis
    	    case CH_OBR: // open brace
    	    case CH_CBR: // close brace
    	    case CH_WSP: // white space
    	    case CH_IDC: // id continue
    	        // Check to see if we're back on track, in which case
    	        // we don't need to reclassify any more code
    	        if (*myTextType == TEXT_NORMAL)
    	        {
    	            return myResultCode;
    	        }
    	        
    	        // Check to see if we're relcassifying multiple lines of code.
    	        if (*myPtr == '\n')
    	        {
    	            myResultCode = CLASSIFY_CHANGE_MULTIPLE_LINES;
    	        }
    	        
    	        *myTextType++ = TEXT_NORMAL;
    	        myPtr++;
    	        break;
    	    case CH_DQT: // "
    	        if (!myStartingInMiddle)
    	        {
    	            *myTextType++ = TEXT_DQSTRING;
    	            myPtr++;
    	        }
    	        while ((myPtr < myEnd) && (*myPtr != '\n') && (*myPtr != '"'))
    	        {
    	            if (*myPtr == '\\')
    	            {
    	            	*myTextType++ = TEXT_DQSTRING;
    	            	myPtr++;
    	            }
    	            // Check to see if we're back on track, in which case
    	            // we don't need to reclassify any more code
    	            if (*myTextType == TEXT_DQSTRING)
    	            {
    	                return myResultCode;
    	            }
    	            *myTextType++ = TEXT_DQSTRING;
    	            myPtr++;
    	        }
    	        if (*myPtr == '"')
    	        {
    	            // Check to see if we're back on track, in which case
    	            // we don't need to reclassify any more code
    	            if (*myTextType == TEXT_DQSTRING_CLOSE)
    	            {
    	                return myResultCode;
    	            }
    	            *myTextType++ = TEXT_DQSTRING_CLOSE;
    	            myPtr++;
    	        }
    	        break;
    	    case CH_SQT: // '
    	        if (!myStartingInMiddle)
    	        {
    	            *myTextType++ = TEXT_SQSTRING;
    	            myPtr++;
    	        }
    	        while ((myPtr < myEnd) && (*myPtr != '\n') && (*myPtr != '\''))
    	        {
    	            if (*myPtr == '\\')
    	            {
    	            	*myTextType++ = TEXT_SQSTRING;
    	            	myPtr++;
    	            }
    	            // Check to see if we're back on track, in which case
    	            // we don't need to reclassify any more code
    	            if (*myTextType == TEXT_SQSTRING)
    	            {
    	                return myResultCode;
    	            }
    	            *myTextType++ = TEXT_SQSTRING;
    	            myPtr++;
    	        }
    	        if (*myPtr == '\'')
    	        {
    	            // Check to see if we're back on track, in which case
    	            // we don't need to reclassify any more code
    	            if (*myTextType == TEXT_SQSTRING_CLOSE)
    	            {
    	                return myResultCode;
    	            }
    	            *myTextType++ = TEXT_SQSTRING_CLOSE;
    	            myPtr++;
    	        }
    	        break;
    	    case CH_PCT: // % (Turing only)
    	        while (*myPtr != '\n')
    	        {
    	            // Check to see if we're back on track, in which case
    	            // we don't need to reclassify any more code
    	            if (*myTextType == TEXT_LINE_COMMENT)
    	            {
    	                return myResultCode;
    	            }
     	            *myTextType++ = TEXT_LINE_COMMENT;
     	            myPtr++;
     	        } 
    	        break;
    	    case CH_IDS: // id start
    	        myStart = myPtr;
    	        while ((myCharTags [*myPtr] == CH_IDS) || 
    	            (myCharTags [*myPtr] == CH_IDC) ||
   	            (*myPtr == '.') || 
    	            ((myPtr > pmTextPtr -> text) && (*(myPtr - 1) == '.') && 
    	             (*myPtr == '*')))
    	        {
    	            myPtr++;
    	        }
    	        myLen = myPtr - myStart;
    	        
   	        // Check for the situation "this.x"
    	        if (pmTextPtr -> isJava && (myLen >= 5) &&
    	            (strncmp (myStart, "this.", 5) == 0))
		{
		    for (cnt = 0 ; cnt < 4 ; cnt++)
    	            {
    	            	*myTextType++ = TEXT_KEYWORD;
    	            }
    	            *myTextType++ = TEXT_NORMAL;
    	            for (cnt = 0 ; cnt < myLen - 5 ; cnt++)
    	            {
    			// Check to see if we're back on track, in which case
    			// we don't need to reclassify any more code
    			if (*myTextType == TEXT_IDENTIFIER)
    			{
    			    return myResultCode;
    			}
    	            	*myTextType++ = TEXT_IDENTIFIER;
    	            }
    	            break;
		}    	             
   	        // Check for the situation "super.x"
     	        if (pmTextPtr -> isJava && (myLen >= 6) &&
    	            (strncmp (myStart, "super.", 6) == 0))
		{
		    for (cnt = 0 ; cnt < 5 ; cnt++)
    	            {
    	            	*myTextType++ = TEXT_KEYWORD;
    	            }
    	            *myTextType++ = TEXT_NORMAL;
    	            for (cnt = 0 ; cnt < myLen - 6 ; cnt++)
    	            {
    			// Check to see if we're back on track, in which case
    			// we don't need to reclassify any more code
    			if (*myTextType == TEXT_IDENTIFIER)
    			{
    			    return myResultCode;
    			}
    	            	*myTextType++ = TEXT_IDENTIFIER;
    	            }
    	            break;
		}    	             

    	        // If the identifier is too long, it's not a keyword or predef
    	        if (myLen > 50)
    	        {
    	            myIDTextType = TEXT_IDENTIFIER;
    	        }
    	        else
    	        {
    	            strncpy (myIdentifier, myStart, myLen);
    	            myIdentifier [myLen] = 0;
    	            if ((pmTextPtr -> isJava && 
    	                 MyIsJavaKeyword (myIdentifier)) ||
    	                (pmTextPtr -> isTuring && 
    	                 MyIsTuringKeyword (myIdentifier)))
    	            {
    	            	myIDTextType = TEXT_KEYWORD;
    	            }
    	            else if ((pmTextPtr -> isJava && 
    	                      MyIsJavaPredef (myIdentifier)) ||
    	                     (pmTextPtr -> isTuring && 
    	                      MyIsTuringPredef (myIdentifier)))
    	            {
    	            	myIDTextType = TEXT_PREDEF;
    	            }
    	            else
    	            {
    	            	myIDTextType = TEXT_IDENTIFIER;
    	            }
    	        }
    	        for (cnt = 0 ; cnt < myLen ; cnt++)
    	        {
    	            // Check to see if we're back on track, in which case
    	            // we don't need to reclassify any more code
    	            if (*myTextType == myIDTextType)
    	            {
    	                return myResultCode;
    	            }
    	            *myTextType++ = myIDTextType;
    	        }
    	        break;
    	    case CH_SLS: // /
		if (!myStartingInMiddle)
		{
    	            if (*(myPtr + 1) == '*')
    	            {
    	            	*myTextType++ = TEXT_LONG_COMMENT;
    	            	// The "*" in "/*"
    	            	*myTextType++ = TEXT_LONG_COMMENT_OPEN; 
    	            	myPtr += 2;
    	            }
    	            else if ((pmTextPtr -> isJava) && (*(myPtr + 1) == '/'))
    	            {
    	            	*myTextType++ = TEXT_LINE_COMMENT;
    	            	// The "*" in "/*"
    	            	*myTextType++ = TEXT_LINE_COMMENT_OPEN; 
    	            	myPtr += 2;
    	            }
    	            else
    	            {
     	            	*myTextType++ = TEXT_NORMAL;
     	            	myPtr++;
     	            	break;
     	            }
     	        }
		if ((*(myTextType - 1) == TEXT_LINE_COMMENT) ||
		    (*(myTextType - 1) == TEXT_LINE_COMMENT_OPEN))
		{
		    // We're in a line comment
    	            while (*myPtr != '\n')
    	            {
    	                // Check to see if we're back on track, in which case
    	                // we don't need to reclassify any more code
    	                if (*myTextType == TEXT_LINE_COMMENT)
    	                {
    	                    return myResultCode;
    	                }
     	            	*myTextType++ = TEXT_LINE_COMMENT;
     	            	myPtr++;
     	            }
		}		    
		else
		{
		    // We're in a long comment
    	            while (myPtr < myEnd) 
    	            {
    	            	if ((*myPtr == '*') && (*(myPtr + 1) == '/'))
    	            	{
	    	            *myTextType++ = TEXT_LONG_COMMENT;
	    	            // The "/" in "*/"
	    	            *myTextType++ = TEXT_LONG_COMMENT_CLOSE;
	    	            myPtr += 2;
	    	            break;
	    	        }
	    	        // Check to see if we're reclassifying 
	    	        // multiple lines of code.
	    	        if (*myPtr == '\n')
	    	        {
	    	            myResultCode = CLASSIFY_CHANGE_MULTIPLE_LINES;
	    	        }
	    	        
    	                // Check to see if we're back on track, in which case
    	                // we don't need to reclassify any more code
    	                if (*myTextType == TEXT_LONG_COMMENT)
    	                {
    	                    return myResultCode;
    	                }
	    	        *myTextType++ = TEXT_LONG_COMMENT;
	    	        myPtr++;
	    	    } // while
    	        }
    	        break;
    	    case CH_ERR: // error
	    default:
    	        EdFail_Fatal (IDS_UNEXPECTED_CHAR, __FILE__, __LINE__, 0,
    	            *myPtr);
    	        break;
    	} // switch
    	
    	myTag = myCharTags [*myPtr];
    	myStartingInMiddle = FALSE;
    } // while
    
    return myResultCode;
} // MyClassifySection


/************************************************************************/
/* MyClearMarkLineBuffer						*/
/************************************************************************/
static void	MyClearMarkLineBuffer (TextPtr pmTextPtr)
{
    MarkLinePtr		myMarkLine, myNextMarkLine;
    
    // Eliminate the MarkLine buffer
    myMarkLine = pmTextPtr -> markLine;
    while (myMarkLine != NULL)
    {
    	myNextMarkLine = myMarkLine -> next;
    	free (myMarkLine -> name);
    	free (myMarkLine);
    	myMarkLine = myNextMarkLine;
    }
    pmTextPtr -> markLine = NULL;
} // MyClearMarkLineBuffer


/************************************************************************/
/* MyCompleteStructure							*/
/*									*/
/* pmLineNum contains the structure that we want to match.		*/
/************************************************************************/
static void	MyCompleteStructure (TextPtr pmTextPtr, int pmLineNum,
				     char *pmEndStruct, int pmIndent,
				     int *pmAction)
{
    int		myFromLine, myFromCol, myToLine, myToCol, myDummy, myPos; 
    int		myDeleteLineFrom, myDeleteLineTo;
    BOOL	myInsertIndent, myStartOnNextLine;

    EdText_GetSelectionLinesCols (pmTextPtr, &myFromLine, &myFromCol, 
				  &myToLine, &myToCol);

    // First determine if we are on the line that has the structure that
    // we want to match and there's no selected area
    if ((pmLineNum == myToLine) && 
	(pmTextPtr -> selTo == pmTextPtr -> selFrom))
    {
	// Set the undo state
	EdText_UndoSaveState (pmTextPtr, IDS_UNDO_COMPLETION);
	
	// We're on the same line as the structure to be matched
	// First move the cursor to the beginning of the next line
	EdText_SetSelection (pmTextPtr, pmLineNum + 1, 0);
	myPos = pmTextPtr -> selFrom;
	EdText_UndoSetReplFrom (pmTextPtr, myPos);
	// Then insert the end structure
	*pmAction = EdText_InsertText (pmTextPtr, "\n", 1, TRUE, &myDummy);
	*pmAction |= EdText_InsertText (pmTextPtr, pmEndStruct,
					strlen (pmEndStruct), TRUE, &myDummy);
	// Then move the cursor back to the same position
	EdText_SetSelection (pmTextPtr, pmLineNum + 1, 0);
	// Then insert the proper indent
	*pmAction |= EdText_InsertText (pmTextPtr, SPACES_250,
					pmIndent + 4, TRUE, &myDummy);
	EdText_UndoSetReplTo (pmTextPtr, 
	    myPos + 1 + strlen (pmEndStruct) + pmIndent + 4);
	return;
    } // if (on same line as structure)

    // Determine if we are on the line following the the structure that
    // we want to match and there's no selected area
    if ((pmLineNum == myToLine - 1) && 
	(pmTextPtr -> selTo == pmTextPtr -> selFrom))
    {
	// Determine if we're actually on the phantom line
	if (pmTextPtr -> numLines == myToLine)
	{
	    // Set the undo state
	    EdText_UndoSaveSpecified (pmTextPtr, IDS_UNDO_COMPLETION,
		pmTextPtr -> lineStarts [myToLine],
		pmTextPtr -> lineStarts [myToLine]);
	    
	    myPos = pmTextPtr -> selFrom;
	    // Insert the proper indent
	    EdText_UndoSetReplFrom (pmTextPtr, myPos);
	    *pmAction = EdText_InsertText (pmTextPtr, SPACES_250,
		pmIndent + 4, TRUE,
		&myDummy);
	    // Insert the new line
	    *pmAction |= EdText_InsertText (pmTextPtr, "\n", 1,
		TRUE, &myDummy);
	    // Then insert the end structure
	    *pmAction |= EdText_InsertText (pmTextPtr, pmEndStruct,
		strlen (pmEndStruct), 
		TRUE, &myDummy);
	    // Then move the cursor back to the same position
	    EdText_SetSelection (pmTextPtr, myToLine, pmIndent + 4);
	    EdText_UndoSetReplTo (pmTextPtr, 
		myPos + strlen (pmEndStruct) + pmIndent + 5);
	    
	    return;
	} // if (on phantom line)

	// Determine if this line is blank
	if (MyIsLineBlanks (pmTextPtr, myToLine))
	{
	    // If this line is all blanks, replace it with the
	    // indented line
	    
	    // We're going to erase this line and replace it with
	    // and indented line and the end structure.
	    
	    // Set the undo state
	    EdText_UndoSaveSpecified (pmTextPtr, IDS_UNDO_COMPLETION,
		pmTextPtr -> lineStarts [myToLine],
		pmTextPtr -> lineStarts [myToLine + 1]);
	    
	    // First, delete all the blanks on the line, if there re any
	    pmTextPtr -> selFrom = pmTextPtr -> lineStarts [myToLine];
	    pmTextPtr -> selTo = pmTextPtr -> lineStarts [myToLine + 1];
	    if (pmTextPtr -> selFrom != pmTextPtr -> selTo)
	    {
		*pmAction = EdText_SelectionDelete (pmTextPtr);
	    }
	    // Set the undo to replace from current location
	    myPos = pmTextPtr -> selFrom;
	    EdText_UndoSetReplFrom (pmTextPtr, myPos);
	    // Insert the proper indent
	    *pmAction = EdText_InsertText (pmTextPtr, SPACES_250, pmIndent + 4,
					   TRUE, &myDummy);
	    // Insert the new line
	    *pmAction |= EdText_InsertText (pmTextPtr, "\n", 1, TRUE, &myDummy);
	    // Then insert the end structure
	    *pmAction |= EdText_InsertText (pmTextPtr, pmEndStruct,
					    strlen (pmEndStruct), TRUE, 
					    &myDummy);
	    // Then move the cursor back to the same position
	    EdText_UndoSetReplTo (pmTextPtr, pmTextPtr -> selFrom);
	    EdText_SetSelection (pmTextPtr, myToLine, pmIndent + 4);
	
	    return;
	} // if (line is all spaces)
    } // if (next line from structure)
    
    // There's a bunch of text between the structure and the 
    // current line.  Just add the end structure and an
    // indentation if appropriate/
    
    // There are four possible situations.  
    //    (a) This line is phantom
    //    (b) Text on this line, next line is phantom
    //    (c) Text on this line, next line is blank
    //    (d) Text on this line, text on next
    //    (e) This line is blank, next line is phantom
    //    (f) This line is blank, next line is blank
    //    (g) This line is blank, text on next
    
    // (a) This line is phantom
    if (pmTextPtr -> numLines == myToLine)
    {
	myStartOnNextLine = FALSE;
	myDeleteLineFrom = -1;
	myDeleteLineTo = -1;
	myInsertIndent = TRUE;
    }
    // (b) Text on this line, next line is phantom
    // (c) Text on this line, text on next
    // (d) Text on this line, next line is blank
    else if (!MyIsLineBlanks (pmTextPtr, myToLine))
    {
	myStartOnNextLine = TRUE;
	
	// (b) Text on this line, next line is phantom
	if (myToLine + 1 == pmTextPtr -> numLines)
	{
	    myDeleteLineFrom = -1;
	    myDeleteLineTo = -1;
	    myInsertIndent = TRUE;
	}
	// (c) Text on this line, next line is blank
	else if (MyIsLineBlanks (pmTextPtr, myToLine + 1))
	{
	    if (myToLine + 2 == pmTextPtr -> numLines)
	    {
		myDeleteLineFrom = 1;
		myDeleteLineTo = 2;
		myInsertIndent = TRUE;
	    }
	    else if (MyIsLineBlanks (pmTextPtr, myToLine + 2))
	    {
		myDeleteLineFrom = 1;
		myDeleteLineTo = 3;
		myInsertIndent = TRUE;
	    }
	    else
	    {
		myDeleteLineFrom = 1;
		myDeleteLineTo = 2;
		myInsertIndent = FALSE;
	    }
	}
	// (d) Text on this line, text on next
	else
	{
	    myDeleteLineFrom = -1;
	    myDeleteLineTo = -1;
	    myInsertIndent = FALSE;
	}
    }
    // (e) This line is blank, next line is phantom
    // (f) This line is blank, text on next
    // (g) This line is blank, next line is blank
    else
    {
	myStartOnNextLine = FALSE;
	// (e) This line is blank, next line is phantom
	if (myToLine + 1 == pmTextPtr -> numLines)
	{
	    myDeleteLineFrom = 0;
	    myDeleteLineTo = 1;
	    myInsertIndent = TRUE;
	}
	// (f) This line is blank, next line is blank
	else if (MyIsLineBlanks (pmTextPtr, myToLine + 1))
	{
	    myDeleteLineFrom = 0;
	    myDeleteLineTo = 2;
	    myInsertIndent = TRUE;
	}
	// (g) This line is blank, text on next
	else
	{
	    myDeleteLineFrom = 0;
	    myDeleteLineTo = 1;
	    myInsertIndent = FALSE;
	}
    }
 
    // Delete the appropriate blank lines, set selection to location of 
    // where the text is to be inserted
    if (myDeleteLineFrom == -1)
    {
	// We don't use EdText_UndoSaveState because we are not replacing
	// the selection.
	EdText_UndoSaveSpecified (pmTextPtr, IDS_UNDO_COMPLETION,
	    pmTextPtr -> lineStarts [myToLine],
	    pmTextPtr -> lineStarts [myToLine]);

	// Move to the end of file and insert text
	if (myStartOnNextLine)
	{
	    EdText_SetSelection (pmTextPtr, myToLine + 1, 0);
	}
	else
	{
	    EdText_SetSelection (pmTextPtr, myToLine, 0);
	}
    }
    else
    {
	// Set the undo state
	EdText_UndoSaveSpecified (pmTextPtr, IDS_UNDO_COMPLETION,
	    pmTextPtr -> lineStarts [myToLine + myDeleteLineFrom],
	    pmTextPtr -> lineStarts [myToLine + myDeleteLineTo]);

	// First, delete the blank lines
	pmTextPtr -> selFrom = pmTextPtr -> lineStarts [myToLine + myDeleteLineFrom];
	pmTextPtr -> selTo = pmTextPtr -> lineStarts [myToLine + myDeleteLineTo];
	*pmAction = EdText_SelectionDelete (pmTextPtr);
    }

    // Insert the text
    EdText_UndoSetReplFrom (pmTextPtr, pmTextPtr -> selFrom);
    *pmAction |= EdText_InsertText (pmTextPtr, pmEndStruct, 
				    strlen (pmEndStruct), TRUE, &myDummy);
    if (myInsertIndent)
    {
	*pmAction |= EdText_InsertText (pmTextPtr, SPACES_250,
					pmIndent, TRUE, &myDummy);
	*pmAction |= EdText_InsertText (pmTextPtr, "\n", 1, TRUE, &myDummy);
	EdText_UndoSetReplTo (pmTextPtr, pmTextPtr -> selFrom);
	// Cursor is now at beginning of next line. 
	// Move to end of indent
	pmTextPtr -> selFrom = pmTextPtr -> selTo = pmTextPtr -> selFrom - 1;
    }
    else
    {
	EdText_UndoSetReplTo (pmTextPtr, pmTextPtr -> selFrom);
	// Cursor is now at beginning of next line. 
	// Move to start of text on line
	*pmAction |= EdText_MoveLineBegin (pmTextPtr, FALSE);
    }
} // MyCompleteStructure
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)	


/************************************************************************/
/* MyConvertTabsAndNewLines						*/
/************************************************************************/
static int	MyConvertTabsAndNewLines (char **pmDestBuffer, int *pmDestSize, 
					  const char *pmSrcBuffer, 
					  int pmSrcSize, int *pmNumLines, 
					  int *pmNumChars, int pmStartCol)
{
    unsigned const char	*mySrc, *myEnd;
    char		*myDest;
    int			myCol;
    int			cnt;
    
    mySrc = pmSrcBuffer;
    myDest = *pmDestBuffer;
    myEnd = (char *) ((int) mySrc + pmSrcSize);
    myCol = pmStartCol;
    
    *pmNumLines = 0;
    
    while (mySrc < myEnd)
    {
    	// Convert \r or \r\n to \n
    	if (*mySrc == '\r')
    	{
    	    *myDest++ = '\n';
    	    mySrc++;
            (*pmNumLines)++;

    	    if ((mySrc < myEnd) && (*mySrc == '\n'))
    	    {
    	        mySrc++;
    	    }
    	    
            myCol = 0;
    	}
    	// Convert \n to \n
    	else if (*mySrc == '\n')
    	{
    	    *myDest++ = *mySrc++;
            (*pmNumLines)++;
            myCol = 0;
    	}
    	// Convert \t to spaces
    	else if (*mySrc == '\t')
    	{
    	    int mySpaces = stProperties.numSpacesPerTab - 
    	        (myCol % stProperties.numSpacesPerTab);
    	    for (cnt = 0 ; cnt < mySpaces ; cnt++)
    	    {
    	        *myDest++ = ' ';
    	        myCol++;
    	    }
    	    mySrc++;
    	}
    	// Convert 0x0B (used as non-para line break) into \n
    	else if (*mySrc == 0x0B)
    	{
	    mySrc++;
    	    *myDest++ = '\n';
            (*pmNumLines)++;
    	    myCol = 0;
    	}
    	// Convert 0x96 (m-dash) into minus sign
    	else if (*mySrc == 0x96)
    	{
	    mySrc++;
    	    *myDest++ = '-';
    	    myCol++;
    	}
    	// Convert 0x91 and 0x92 (left and right single quote) into flat quotes
    	else if ((*mySrc == 0x91) || (*mySrc == 0x92))
    	{
	    mySrc++;
    	    *myDest++ = '\'';
    	    myCol++;
    	}
    	// Convert 0x93 and 0x94 (left and right double quote) into flat quotes
    	else if ((*mySrc == 0x93) || (*mySrc == 0x94))
    	{
	    mySrc++;
    	    *myDest++ = '"';
    	    myCol++;
    	}
    	// Convert ASCII to same
    	else if (stAcceptableChar [*mySrc])
    	{
    	    *myDest++ = *mySrc++;
    	    myCol++;
    	}
    	// Skip the rest
    	else
    	{
    	    mySrc++;
    	    // TW Non-printable warning 
    	}
    	
    	// If we're near the end of the current buffer, extend it some more.
    	if (myDest - *pmDestBuffer > *pmDestSize - 16)
    	{
    	    char *origBuffer = *pmDestBuffer;
    	    
	    *pmDestSize = (int) (*pmDestSize * 1.1);
	    *pmDestBuffer = (char *) realloc (*pmDestBuffer, *pmDestSize);
	    if (*pmDestBuffer  == NULL)
	    {
	    	free (origBuffer);
	    	return -1;
	    	// TW run out mem error message
	    }
	    myDest = myDest - origBuffer + *pmDestBuffer;
	}
    }

    *pmNumChars = myDest - *pmDestBuffer;  
    
    return 0;
} // MyConvertTabsAndNewLines


#if !defined (CONSOLE) && !defined (TPROLOG)	
/************************************************************************/
/* MyCreateCRCTable							*/
/************************************************************************/
static void	MyCreateCRCTable (void)
{
    int		myIndex, myEntry, myCarry, cnt;
    
    for (myIndex = 0 ; myIndex < CRC_TABLE_SIZE ; myIndex++)
    {
    	myEntry = myIndex;
    	
    	for (cnt = 0 ; cnt < 8 ; cnt++)
    	{
    	    myCarry = myEntry & 1;
    	    myEntry >>= 1;
    	    if (myCarry)
    	    {
    	        myEntry ^= 0xedb88320;
    	    }
    	}
    	stCRCTable [myIndex] = myEntry;
    }    
} // MyCreateCRCTable
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)	


/************************************************************************/
/* MyEnsureLineStartsSpace						*/
/************************************************************************/
static BOOL	MyEnsureLineStartsSpace (TextPtr pmTextPtr, int pmLinesNeeded)
{
    int	mySizeNeeded, mySizeToAllocate;
    
    // Calculate the buffer size needed
    mySizeNeeded = pmTextPtr -> numLines + pmLinesNeeded + LINE_STARTS_SLACK;
    
    if (pmTextPtr -> lineStartsBufferLen < mySizeNeeded)
    {
	int	*myPtr;
	
    	// Calculate the buffer size's new size
	mySizeToAllocate = mySizeNeeded + LINE_STARTS_BUFFER_SIZE_INCREMENT;

	// Allocate the new buffer
	myPtr = (int *) realloc (pmTextPtr -> lineStarts, 
	    mySizeToAllocate * sizeof (int));
	        
	if (myPtr == NULL)
	{
	    return FALSE;
	}
	
	pmTextPtr -> lineStartsBufferLen = mySizeToAllocate;
	pmTextPtr -> lineStarts = myPtr;
    }
    
    return TRUE;
} // MyEnsureLineStartsSpace


/************************************************************************/
/* MyEnsureTextBufferSpace						*/
/************************************************************************/
static BOOL	MyEnsureTextBufferSpace (TextPtr pmTextPtr, int pmSpaceNeeded)
{
    int	mySizeNeeded, mySizeToAllocate;
    
    // Calculate the buffer size needed
    mySizeNeeded = pmTextPtr -> textLen + pmSpaceNeeded + LINE_BUFFER_SLACK;
    
    if (pmTextPtr -> textBufferLen < mySizeNeeded)
    {
	char	*myPtr;
	    
    	// Calculate the buffer size's new size
	mySizeToAllocate = mySizeNeeded + TEXT_BUFFER_SIZE_INCREMENT;

	// Allocate the new buffer
	myPtr = realloc (pmTextPtr -> text, mySizeToAllocate);
	        
	if (myPtr == NULL)
	{
	    return FALSE;
	}
	pmTextPtr -> text = myPtr;
	
#if !defined (CONSOLE) && !defined (TPROLOG)
	// Allocate the new text type buffer
	myPtr = realloc (pmTextPtr -> textType, mySizeToAllocate);
	        
	if (myPtr == NULL)
	{
	    return FALSE;
	}
	pmTextPtr -> textType = myPtr;
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)

    	pmTextPtr -> textBufferLen = mySizeToAllocate;
    }
    
    return TRUE;
} // MyEnsureTextBufferSpace


/************************************************************************/
/* MyFindCharLocation							*/
/*									*/
/* Given an index into the array, this function returns the line and	*/
/* column of this position. 						*/
/************************************************************************/
static void	MyFindCharLocation (TextPtr pmTextPtr, int pmLoc, int *pmLine, 
    int *pmCol)
{
    int myStart = 0;
    int myEnd = pmTextPtr -> numLines;
    int myLine = (myStart + myEnd) / 2;
    int	*myLineStarts = pmTextPtr -> lineStarts;
    int	myCounter = 0;
    
    if (pmLoc == myLineStarts [myEnd])
    {
    	*pmLine = myEnd;
    	*pmCol = 0;
    	return;
    }
    
    while ((pmLoc < myLineStarts [myLine]) ||
        (myLineStarts [myLine + 1] <= pmLoc))
    {
    	if (pmLoc < myLineStarts [myLine])
    	    myEnd = myLine - 1;
    	else
    	    myStart = myLine + 1;
    	    
    	myLine = (myStart + myEnd) / 2;
    	myCounter++;
    	if (myCounter > 40)
    	{
    	    char	*myText;
    	    
    	    // We've got a real problem here...  We're in an infinite loop!
    	    
    	    // If the program is less than 20 lines long, place it in the log
    	    if (pmTextPtr -> numLines <= 20)
    	    {
    	    	myText = pmTextPtr -> text;
    	    	myText [pmTextPtr -> textLen] = 0;
    	    }
    	    else
    	    {
    	    	myText = "";
    	    }
    	    
            /*EdFail_Fatal (IDS_LOCATER_LOOPING_INFINITELY, __FILE__, 
            	__LINE__, 0, pmTextPtr -> numLines, myStart, myEnd, myLine, 
            	pmLoc, myLineStarts [myStart], myLineStarts [myEnd], 
                myLineStarts [myLine], myText);*/
								break;
    	}
    }
    
    *pmLine = myLine;
    *pmCol = pmLoc - myLineStarts [myLine];
} // MyFindCharLocation


/************************************************************************/
/* MyFindLocationPos							*/
/*									*/
/* Given a line, col position, this function returns the index into the	*/
/* array for the position (suitable from selFrom/To).  It also returns	*/
/* a boolean set to true if the column position was actually beyond the	*/
/* end of the line.							*/
/************************************************************************/
static void	MyFindLocationPos (TextPtr pmTextPtr, int pmLine, int pmCol, 
    				   int *pmLoc, BOOL *pmBeyondEnd)
{
    if (pmLine >= pmTextPtr -> numLines)
    {
    	*pmLoc = pmTextPtr -> textLen;
    	*pmBeyondEnd = TRUE;
    }
    else
    {
        pmLine = max (0, min (pmLine, pmTextPtr -> numLines - 1));
        *pmLoc = pmTextPtr -> lineStarts [pmLine] + pmCol;
        if (*pmLoc >= pmTextPtr -> lineStarts [pmLine + 1])
        {
    	    *pmLoc = pmTextPtr -> lineStarts [pmLine + 1] - 1;
    	    *pmBeyondEnd = TRUE;
    	}
    	else
    	{
            *pmBeyondEnd = FALSE;    
        }
    }
} // MyFindLocationPos


#if !defined (CONSOLE) && !defined (TPROLOG)	
/************************************************************************/
/* MyFreeRedoBuffers							*/
/************************************************************************/
static void	MyFreeRedoBuffers (TextPtr pmTextPtr)
{
    UndoBufferPtr	myRedoPtr;

    myRedoPtr = pmTextPtr -> redoHead;
    
    if (myRedoPtr != NULL)
    {
	pmTextPtr -> redoHead = myRedoPtr -> next;
	if (pmTextPtr -> redoHead == NULL)
	{
	    pmTextPtr -> redoTail = NULL;
	}

	if (myRedoPtr -> text != NULL)
	{
	    free (myRedoPtr -> text);
	}
	free (myRedoPtr);
    }
} // MyFreeRedoBuffers


/************************************************************************/
/* MyFreeTextBuffers							*/
/************************************************************************/
static void	MyFreeTextBuffers (TextPtr pmTextPtr)
{
    // Eliminate the error line buffer
    EdText_ClearErrors (pmTextPtr);
    
    // Eliminate the MarkLine buffer
    MyClearMarkLineBuffer (pmTextPtr);
} // MyFreeTextBuffers
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)	


/************************************************************************/
/* MyFreeTextMemory							*/
/************************************************************************/
static void	MyFreeTextMemory (TextPtr pmTextPtr, BOOL pmEraseUndo)
{
    if (pmTextPtr -> text != NULL)
    {
    	free (pmTextPtr -> text);
    	pmTextPtr -> text = NULL;
    }

#if !defined (CONSOLE) && !defined (TPROLOG)	
    if (pmTextPtr -> textType != NULL)
    {
    	free (pmTextPtr -> textType);
    	pmTextPtr -> textType = NULL;
    }
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)	

    if (pmTextPtr -> lineStarts != NULL)
    {
    	free (pmTextPtr -> lineStarts);
    	pmTextPtr -> lineStarts = NULL;
    }

#if !defined (CONSOLE) && !defined (TPROLOG)	
    if (pmEraseUndo)
    {
    	while (pmTextPtr -> undoHead != NULL)
    	{
	    EdText_UndoFreeLastBuffer (pmTextPtr);
    	}
    	MyFreeRedoBuffers (pmTextPtr);
    }

    MyFreeTextBuffers (pmTextPtr);
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)	
} // MyFreeTextMemory


#if !defined (CONSOLE) && !defined (TPROLOG)	
/************************************************************************/
/* MyGetTuringStructure							*/
/*									*/
/* Returns a number representing the structure on the line.		*/
/************************************************************************/
static int	MyGetTuringStructure (TextPtr pmTextPtr, int pmLineNum, 
				      char *pmEndStruct, int *pmIndent)
{
    int		myPos = pmTextPtr -> lineStarts [pmLineNum];
    BYTE	*myChar = pmTextPtr -> text + myPos;
    BYTE	*myTextType = pmTextPtr -> textType + myPos;
    char	myToken [256]; // Max length of a keyword
    int		myLen, myEndLen, myIndent;
    int		cnt;
    
    // If at end of line, return nothing found.
    if (pmLineNum == pmTextPtr -> numLines)
    {
    	return CMPLT_KEYWORD_NOT_FOUND;
    }

    // Get the first non-whitespace character    
    while (*myChar == ' ')
    {
    	myChar++;
    	myTextType++;
    }
    // Reached end of line? return
    if (*myChar == 0)
    {
    	return CMPLT_KEYWORD_NOT_FOUND;
    }
    
    // First non-whitespace char not a keyword? return
    if (*myTextType != TEXT_KEYWORD)
    {
    	return CMPLT_KEYWORD_NOT_FOUND;
    }
    
    // Get the keyword
    myLen = 0;
    while (*myTextType == TEXT_KEYWORD)
    {
    	myLen++;
    	myTextType++;
    }
    
    strncpy (myToken, (char*)myChar, myLen);
    myToken [myLen] = 0;
    myIndent = myChar - pmTextPtr -> text - myPos;
    *pmIndent = myIndent;
    if (myIndent > 240)
    {
	return CMPLT_INDENT_TOO_DEEP;
    }

	// Match the keyword with modifiers
    for (cnt = 0 ; cnt < stNumCompletionModifiers ; cnt++)
    {
		// structures may have a modifier like "body" or pervasive"
		if (strcmp (myToken, stCompletionModifiers[cnt]) == 0) {
			// TODO this code duplicates most of the code before it. Abstraction?
			// Forward past the "body" keyword
			myChar += myLen;

			// eat whitespace
			while (*myChar == ' ')
			{
	    		myChar++;
	    		myTextType++;
			}

			// Not a keyword? return
			if (*myTextType != TEXT_KEYWORD)
			{
    			return CMPLT_KEYWORD_NOT_FOUND;
			}
    
			// Get the keyword
			myLen = 0;
			while (*myTextType == TEXT_KEYWORD)
			{
    			myLen++;
    			myTextType++;
			}

			strncpy (myToken, (char*)myChar, myLen);
			myToken [myLen] = 0;

			break; // only one modifier
		}
	}

    // Match the keyword with structures
    for (cnt = 0 ; cnt < stNumCompletionKeywords ; cnt++)
    {
    	if (strcmp (myToken, stCompletionKeywords [cnt]) == 0)
    	{
	    // Put enough spaces to cover the indent of the keyword
	    (void) strcpy (pmEndStruct, SPACES_250);
	    pmEndStruct [myIndent] = 0;

    	    // Create the appropriate end structure
    	    if (cnt < NAMED_STRUCTURE_KEYWORDS)
    	    {
	    	(void) strcat (pmEndStruct, "end ");
	    	myEndLen = strlen (pmEndStruct);
	    	
	    	// Add the identifier.
	    	myChar += myLen;
	    	while (*myChar == ' ' || *myChar == '*')
	    	{
	    	    myChar++;
	    	    myTextType++;
	    	}
	    	
	    	if ((*myChar == 0) || (*myTextType != TEXT_IDENTIFIER))
	    	{
	    	    return NO_IDENT_FOUND;
	    	}
	    	myLen = 0;
	    	while ((*myTextType == TEXT_IDENTIFIER) && (myLen < 200))
	    	{
	    	    myLen++;
	    	    myTextType++;
	    	}
		if (myIndent + myLen> 240)
		{
		    return CMPLT_INDENT_TOO_DEEP;
		}
	    	strncpy (&pmEndStruct [myEndLen], myChar, myLen);
	    	pmEndStruct [myEndLen + myLen] = 0; 
	    	(void) strcat (pmEndStruct, "\n");
	    }
	    else if (cnt < BEGIN_KEYWORD)
	    {
		if (myIndent + strlen (stCompletionKeywords [cnt])> 240)
		{
		    return CMPLT_INDENT_TOO_DEEP;
		}
	    	(void) strcat (pmEndStruct, "end ");
	    	(void) strcat (pmEndStruct, stCompletionKeywords [cnt]);
	    	(void) strcat (pmEndStruct, "\n");
	    }
	    else if (cnt == BEGIN_KEYWORD)
	    {
	    	(void) strcat (pmEndStruct, "end\n");
	    }
	    else if (cnt == END_KEYWORD)
	    {
	    	return CMPLT_KEYWORD_END;
	    }
	
    	    return CMPLT_KEYWORD_FOUND;
    	}
    }
    
    return CMPLT_KEYWORD_NOT_FOUND;
} // MyGetTuringStructure


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
    	myValue = myValue * 59 + (unsigned char) pmString [cnt];
    }
    return (myValue * 37 + pmString [0]) % pmTableSize;
} // MyHash


/************************************************************************/
/* MyIsLineBlanks							*/
/*									*/
/* Note: Returns true if past end of program.				*/
/************************************************************************/
static BOOL	MyIsLineBlanks (TextPtr pmTextPtr, int pmLineNum)
{
    char    *myPtr;

    if (pmLineNum == pmTextPtr -> numLines)
    {
	return TRUE;
    }

    myPtr = pmTextPtr -> text + pmTextPtr -> lineStarts [pmLineNum];
    while (*myPtr == ' ')
    {
	myPtr++;
    }
    return (*myPtr == '\n');
} // MyIsLineBlanks


/************************************************************************/
/* MyIsJavaKeyword							*/
/************************************************************************/
static BOOL	MyIsJavaKeyword (char *pmId)
{
    int	myHash = MyHash (pmId, KEYWORD_TABLE_SIZE);
    
    while (stJavaKeywordTable [myHash] != NULL)
    {
    	if (strcmp (pmId, stJavaKeywordTable [myHash]) == 0)
    	    return TRUE;
    	myHash = (myHash + 1) % KEYWORD_TABLE_SIZE;
    }
    return FALSE;
} // MyIsJavaKeyword


/************************************************************************/
/* MyIsJavaPredef							*/
/************************************************************************/
static BOOL	MyIsJavaPredef (char *pmId)
{
    int	myHash = MyHash (pmId, stJavaPredefsTableSize);

    while (stJavaPredefTable [myHash] != NULL)
    {
    	if (strcmp (pmId, stJavaPredefTable [myHash]) == 0)
    	    return TRUE;
    	myHash = (myHash + 1) % stJavaPredefsTableSize;
    }
    return FALSE;
} // MyIsJavaPredef


/************************************************************************/
/* MyIsTuringKeyword							*/
/************************************************************************/
static BOOL	MyIsTuringKeyword (char *pmId)
{
    int	myHash = MyHash (pmId, KEYWORD_TABLE_SIZE);
    
    while (stTuringKeywordTable [myHash] != NULL)
    {
    	if (strcmp (pmId, stTuringKeywordTable [myHash]) == 0)
    	    return TRUE;
    	myHash = (myHash + 1) % KEYWORD_TABLE_SIZE;
    }
    return FALSE;
} // MyIsTuringKeyword


/************************************************************************/
/* MyIsTuringPredef							*/
/************************************************************************/
static BOOL	MyIsTuringPredef (char *pmId)
{
    int	myHash = MyHash (pmId, stTuringPredefsTableSize);

    while (stTuringPredefTable [myHash] != NULL)
    {
    	if (strcmp (pmId, stTuringPredefTable [myHash]) == 0)
    	    return TRUE;
    	myHash = (myHash + 1) % stTuringPredefsTableSize;
    }
    return FALSE;
} // MyIsTuringPredef


/************************************************************************/
/* MyKeyBackspace							*/
/************************************************************************/
static int	MyKeyBackspace (TextPtr pmTextPtr, char **pmDeletedChars)
{
    int		myResultCode = KEY_UPDATE_LINE;
    char	*myPtr;
    int		myLine, myCol, myNumSpacesToDelete;

    // If there's a selection, delete it
    if (pmTextPtr -> selFrom != pmTextPtr -> selTo)
    {
    	return EdText_SelectionDelete (pmTextPtr);
    }
    
    if (pmTextPtr -> selFrom == 0)
    {
    	return KEY_CANT_DELETE;
    }
    
    // Modify following line starts by 1
    EdText_GetSelectionLocation (pmTextPtr, &myLine, &myCol);
    
    myNumSpacesToDelete = 0;
    if (myCol == 0)
    {
    	myPtr = pmTextPtr -> text + pmTextPtr -> selFrom - 2;
    	while ((myPtr >= pmTextPtr -> text) && (*myPtr == ' '))
    	{
    	    myPtr--;
	    myNumSpacesToDelete++;
    	}
    	if ((myPtr >= pmTextPtr -> text) && (*myPtr != '\n') &&
    	    (myNumSpacesToDelete > 0))
    	{
    	    myNumSpacesToDelete--;
    	}
    }

    // Move the characters to be deleted to the buffer.
    myPtr = pmTextPtr -> text + pmTextPtr -> selFrom;
    *pmDeletedChars = malloc (1 + myNumSpacesToDelete + 1);
    memcpy (*pmDeletedChars, myPtr - 1 - myNumSpacesToDelete, 
        1 + myNumSpacesToDelete);
    (*pmDeletedChars) [1 + myNumSpacesToDelete] = 0;
    
    // Move from the selection to the end of the buffer one plus the auto
    // indent size over to fit the newline and the extra spaces. Then
    // place the newline and the spaces in the buffer
    memmove (myPtr - 1 - myNumSpacesToDelete, myPtr, 
        pmTextPtr -> textLen - pmTextPtr -> selFrom);

    // Do the same for the textType block
    myPtr = pmTextPtr -> textType + pmTextPtr -> selFrom;
    memmove (myPtr - 1 - myNumSpacesToDelete, myPtr, 
        pmTextPtr -> textLen - pmTextPtr -> selFrom);
    
    // Adjust the line starts array
    myResultCode |= MyLineStartsAdjustForDelete (pmTextPtr, 
        pmTextPtr -> selFrom - 1 - myNumSpacesToDelete, 
        pmTextPtr -> selFrom);

    // Wipe out mark or error if we split the line in the middle
    if (myCol != 0)
    {
        myResultCode |= MyNotifyLinesModified (pmTextPtr, myLine, myLine);
    }
    
    // Move all the marks and errors down a line
    if (myCol == 0)
    {
        MyNotifyLinesDeleted (pmTextPtr, myLine, myLine + 1, TRUE);
    }
        
    // Advance the selection
    pmTextPtr -> selFrom -= 1 + myNumSpacesToDelete;
    pmTextPtr -> selTo -= 1 + myNumSpacesToDelete;
    pmTextPtr -> locationOK = FALSE;
    
    // Set dirty flag
    myResultCode |= MySetDirty (pmTextPtr);
    	   
    // Colour the new text (will always redraw screen
    (void) MyClassifySection (pmTextPtr, pmTextPtr -> selFrom);
    
    return myResultCode;
} // MyKeyBackspace


/************************************************************************/
/* MyKeyNormal								*/
/************************************************************************/
static int	MyKeyNormal (TextPtr pmTextPtr, char pmChar)
{
    int		myResultCode = KEY_UPDATE_LINE;
    int		myNumChars;
    char	*myPtr;
    int		myLine, myCol;
    int		myOriginalSelFrom;
    int		cnt;

    EdText_GetSelectionLocation (pmTextPtr, &myLine, &myCol);

    // If we're entering a tab, we convert it into a number of spaces
    if (pmChar == '\t')
    {
    	myNumChars = stProperties.indentSize - 
    	    (myCol % stProperties.indentSize);
        pmChar = ' ';
    }
    else
    {
    	myNumChars = 1;
    }

    // Make certain there's enough space    
    if (!MyEnsureTextBufferSpace (pmTextPtr, myNumChars))
    {
    	return KEY_OUT_OF_MEMORY;
    }
    if (!MyEnsureLineStartsSpace (pmTextPtr, myNumChars))
    {
    	return KEY_OUT_OF_MEMORY;
    }
    
    // If there's a selection, delete it
    if (pmTextPtr -> selFrom != pmTextPtr -> selTo)
    {
    	myResultCode = EdText_SelectionDelete (pmTextPtr);
    }
    
    // Move from the selection to the end of the buffer one over to fit
    // the character and place the character in the buffer
    myPtr = pmTextPtr -> text + pmTextPtr -> selFrom;
    memmove (myPtr + myNumChars, myPtr, 
        pmTextPtr -> textLen - pmTextPtr -> selFrom);
    for (cnt = 0 ; cnt < myNumChars ; cnt++)
    {
        *myPtr++ = pmChar;
    }
    
    // Do the same for the textType block
    myPtr = pmTextPtr -> textType + pmTextPtr -> selFrom;
    memmove (myPtr + myNumChars, myPtr, 
        pmTextPtr -> textLen - pmTextPtr -> selFrom);
    for (cnt = 0 ; cnt < myNumChars ; cnt++)
    {
        *myPtr++ = TEXT_NO_CLASSIFICATION;
    }
    
    // Fix the line starts array
    myResultCode |= MyLineStartsAdjustForInsert (pmTextPtr, 
        pmTextPtr -> selFrom, myNumChars, 0, myNumChars);
    
    // Wipe out mark or error if on this line
    myResultCode |= MyNotifyLinesModified (pmTextPtr, myLine, myLine);
    
    // Advance the selection
    myOriginalSelFrom = pmTextPtr -> selFrom;          
    pmTextPtr -> selFrom += myNumChars;
    pmTextPtr -> selTo += myNumChars;
    pmTextPtr -> locationOK = FALSE;
    
    // Set dirty flag
    myResultCode |= MySetDirty (pmTextPtr);
    
    // Colour the new text
    if (MyClassifySection (pmTextPtr, myOriginalSelFrom) == 
        CLASSIFY_CHANGE_MULTIPLE_LINES)
    {
    	myResultCode |= KEY_REDRAW_FROM_SELECTION;
    }
    
    return myResultCode;
} // MyKeyNormal


/************************************************************************/
/* MyKeyReturn								*/
/************************************************************************/
static int	MyKeyReturn (TextPtr pmTextPtr)
{
    int		myResultCode = KEY_REDRAW_FROM_SELECTION;
    int		myInitialSpaces, myAutoIndentSize, mySearchLineForIndent;
    int		mySpacesUsed;
    char	*myPtr, *myStartPtr;
    int		myLine, myCol;
    BOOL	myAtStartOfLine, myAtEndOfLine;
    int		myOriginalSelFrom;
    int		cnt;

    // If there's a selection, delete it
    if (pmTextPtr -> selFrom != pmTextPtr -> selTo)
    {
    	myResultCode |= EdText_SelectionDelete (pmTextPtr);
    }
    
    // Modify following line starts by 1
    EdText_GetSelectionLocation (pmTextPtr, &myLine, &myCol);
    
    if (myCol == 0)
    {
        myAtStartOfLine = TRUE;
    }
    else
    {
        myAtStartOfLine = FALSE;
    }
  
    //
    // Figure out the auto indent size.
    //
    
    // Get the number of spaces after the selection point
    myInitialSpaces = 0;
    myPtr = pmTextPtr -> text + pmTextPtr -> selFrom;
    if (pmTextPtr -> selFrom != pmTextPtr -> textLen)
    {
	while (*myPtr++ == ' ')
	{
    	    myInitialSpaces++;
	}
    }
    
    // If there are non-ws chars on the current line before the selection,
    // use the current line to determine the indentation
    myPtr = pmTextPtr -> text + pmTextPtr -> selFrom - 1;
    myStartPtr = pmTextPtr -> text + pmTextPtr -> lineStarts [myLine];
    mySearchLineForIndent = myLine - 1;    	    
    while (myPtr >= myStartPtr)
    {
    	if (*myPtr != ' ')
    	{
    	    mySearchLineForIndent++;
    	    break;
    	}
    	myPtr--;
    }

    // Keep going back through lines to find an indented line
    myAutoIndentSize = 0;
    while (mySearchLineForIndent >= 0)
    {
    	myStartPtr = pmTextPtr -> text + 
    	    pmTextPtr -> lineStarts [mySearchLineForIndent];
    	myPtr = myStartPtr;
    	while (*myPtr == ' ')
    	{
    	    myPtr++;
    	}
    	if (*myPtr != '\n')
    	{
	    myAutoIndentSize = myPtr - myStartPtr;
	    break;
	}
	mySearchLineForIndent--;
    }   
    
    if (myInitialSpaces >= 4)
    {
	mySpacesUsed = min (myAutoIndentSize, myInitialSpaces);
    	myAutoIndentSize = max (0, myAutoIndentSize - myInitialSpaces);
    }
    else
    {
	mySpacesUsed = 0;
    }

    //
    // Now do the rest
    //
        	
    // Make certain there's enough space    
    if (!MyEnsureTextBufferSpace (pmTextPtr, 1 + myAutoIndentSize))
    {
    	return KEY_OUT_OF_MEMORY;
    }
    if (!MyEnsureLineStartsSpace (pmTextPtr, 1))
    {
    	return KEY_OUT_OF_MEMORY;
    }
    
    myPtr = pmTextPtr -> text + pmTextPtr -> selFrom;

    if ((pmTextPtr -> selFrom == pmTextPtr -> textLen) || 
        (*(myPtr + 1) == '\n'))
    {
        myAtEndOfLine = TRUE;
    }
    else
    {
        myAtEndOfLine = FALSE;
    }

    // Move from the selection to the end of the buffer one plus the auto
    // indent size over to fit the newline and the extra spaces. Then
    // place the newline and the spaces in the buffer
    memmove (myPtr + 1 + myAutoIndentSize, myPtr, 
        pmTextPtr -> textLen - pmTextPtr -> selFrom);
    *myPtr++ = '\n';
    for (cnt = 0 ; cnt < myAutoIndentSize ; cnt++)
    {
    	*myPtr++ = ' ';
    }

    // Do the same for the textType block
    myPtr = pmTextPtr -> textType + pmTextPtr -> selFrom;
    memmove (myPtr + 1 + myAutoIndentSize, myPtr, 
        pmTextPtr -> textLen - pmTextPtr -> selFrom);
    for (cnt = 0 ; cnt < 1 + myAutoIndentSize ; cnt++)
    {
        *myPtr++ = TEXT_NO_CLASSIFICATION;
    }
    
    // Fix the line starts array
    myResultCode |= MyLineStartsAdjustForInsert (pmTextPtr, 
        pmTextPtr -> selFrom, 1 + myAutoIndentSize, 1, 
        1 + myAutoIndentSize + mySpacesUsed);
    
    // Adjust error and marks
    myResultCode |= MyNotifyLinesInserted (pmTextPtr, myLine, 1, 
        myAtStartOfLine, myAtEndOfLine, TRUE);
    
     // Advance the selection
    myOriginalSelFrom = pmTextPtr -> selFrom;          
    pmTextPtr -> selFrom += 1 + myAutoIndentSize + mySpacesUsed;
    pmTextPtr -> selTo += 1 + myAutoIndentSize + mySpacesUsed;
    pmTextPtr -> locationOK = FALSE;
    
   // Set dirty flag
    myResultCode |= MySetDirty (pmTextPtr);
   
    // Colour the new text (will always redraw screen
    (void) MyClassifySection (pmTextPtr, myOriginalSelFrom);
    
    return myResultCode;
} // MyKeyReturn
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)	


/************************************************************************/
/* MyLineStartsAdjustForDelete						*/
/************************************************************************/
static int	MyLineStartsAdjustForDelete (TextPtr pmTextPtr, int pmFromPos,
					     int pmToPos)
{
    int		myFromLine, myFromCol, myToLine, myToCol;
    int		myCharsToDelete, myLinesToDelete;
    int		myResultCode;
    int		myLineLen;
    int		cnt;
    
    //
    // Adjust the lineStarts
    //    
    MyFindCharLocation (pmTextPtr, pmFromPos, &myFromLine, &myFromCol);
    MyFindCharLocation (pmTextPtr, pmToPos, &myToLine, &myToCol);

    myCharsToDelete = pmToPos - pmFromPos;
    myLinesToDelete = myToLine - myFromLine;
    
    if (myFromLine == myToLine)
    {    
        for (cnt = myFromLine + 1 ; cnt <= pmTextPtr -> numLines ; cnt++)
        {
            pmTextPtr -> lineStarts [cnt] -= myCharsToDelete;
        }
        myResultCode = 0;
    }
    else
    {
        for (cnt = myFromLine + myLinesToDelete + 1 ; 
             cnt <= pmTextPtr -> numLines + 1; 
             cnt++)
        {
            pmTextPtr -> lineStarts [cnt - myLinesToDelete] = 
                pmTextPtr -> lineStarts [cnt] - 
                myCharsToDelete;
        }
        
        myResultCode = KEY_REDRAW_FROM_SELECTION | KEY_NUM_LINES_CHANGED;
    }

    //
    // Adjust textLen and numLines
    //
    pmTextPtr -> textLen -= myCharsToDelete;
    pmTextPtr -> numLines -= myLinesToDelete;
    
    // Add a blank line at the end of the text, if necessary
    MyAddNewLineIfNecessary (pmTextPtr, 0);
    
    //
    // Adjust longestLine, longestLineLen and secondLongestLineLen
    //
    if (myLinesToDelete == 0)
    {
    	if (myFromLine == pmTextPtr -> longestLine)
    	{
    	    pmTextPtr -> longestLineLen -= myCharsToDelete;
    	    if (pmTextPtr -> longestLineLen < pmTextPtr -> secondLongestLineLen)
    	    {
	    	myResultCode |= MyRecalculateLongestLine (pmTextPtr);
	    }
    	    myResultCode |= KEY_LONGEST_LINE_CHANGED;
	}
    }
    else if ((myFromLine <= pmTextPtr -> longestLine) &&
             (pmTextPtr -> longestLine <= myToLine))
    {
    	myResultCode |= MyRecalculateLongestLine (pmTextPtr);
    }
    else if (myFromLine <= pmTextPtr -> longestLine)
    {
    	pmTextPtr -> longestLine -= myLinesToDelete;
    }

    //
    // Check to see if we've created a new longest line
    //
    
    // Calculate the length of the new line
    if (myFromLine == pmTextPtr -> numLines)
    {
	myLineLen = 0;
    }
    else
    {
	myLineLen = pmTextPtr -> lineStarts [myFromLine + 1] -
    	    pmTextPtr -> lineStarts [myFromLine] - 1;
    }
    	
    if (myLineLen > pmTextPtr -> longestLineLen)
    {
    	// If this was not already the longest line, make current longest
    	// line into the second longest line
    	if (pmTextPtr -> longestLine != myFromLine)
    	{
	    pmTextPtr -> longestLine = myFromLine;
    	    pmTextPtr -> secondLongestLineLen = pmTextPtr -> longestLineLen;
	}    		
    	pmTextPtr -> longestLineLen = myLineLen;
    	myResultCode |= KEY_LONGEST_LINE_CHANGED;
    }
    else if (myLineLen > pmTextPtr -> secondLongestLineLen)
    {
    	pmTextPtr -> secondLongestLineLen = myLineLen;
    }

    return myResultCode;    
} // MyLineStartsAdjustForDelete

					     
/************************************************************************/
/* MyLineStartsAdjustForInsert						*/
/*									*/
/* This must be called after the text has been inserted.		*/
/************************************************************************/
static int	MyLineStartsAdjustForInsert (TextPtr pmTextPtr, int pmFromPos,
					     int pmInsertSize, 
					     int pmInsertLines, int pmSelAdjust)
{
    int		myFromLine, myFromCol;
    int		myCalcLine;
    char	*myPtr;
    int		myResultCode;
    int		cnt;

    //
    // Adjust the lineStarts
    //    
    MyFindCharLocation (pmTextPtr, pmFromPos, &myFromLine, &myFromCol);

    if (pmInsertLines == 0)
    {    
        for (cnt = myFromLine + 1 ; cnt <= pmTextPtr -> numLines ; cnt++)
        {
            pmTextPtr -> lineStarts [cnt] += pmInsertSize;
        }
        myResultCode = 0;
    }
    else
    {
    	for (cnt = pmTextPtr -> numLines ; cnt > myFromLine ; cnt--)
    	{	
            pmTextPtr -> lineStarts [cnt + pmInsertLines] = 
                pmTextPtr -> lineStarts [cnt] + pmInsertSize;
        }
        
        myCalcLine = myFromLine + 1;
        myPtr = pmTextPtr -> text + pmFromPos;
        while (myCalcLine <= myFromLine + pmInsertLines)
        {
    	    if (*myPtr == '\n')
    	    {
	        pmTextPtr -> lineStarts [myCalcLine] = 
	            myPtr - pmTextPtr -> text + 1;
	        myCalcLine++;
	    }
	    myPtr++;
        }

        myResultCode = KEY_REDRAW_FROM_SELECTION | KEY_NUM_LINES_CHANGED;
    }

    //
    // Adjust textLen and numLines
    //
    pmTextPtr -> textLen += pmInsertSize;
    pmTextPtr -> numLines += pmInsertLines;

    // Add a blank line at the end of the text, if necessary
    MyAddNewLineIfNecessary (pmTextPtr, pmSelAdjust);
    
    //
    // Adjust longestLine, longestLineLen and secondLongestLineLen
    //
    if (pmInsertLines == 0)
    {
        if (myFromLine == pmTextPtr -> longestLine)
        {
    	    pmTextPtr -> longestLineLen += pmInsertSize;
    	    myResultCode |= KEY_LONGEST_LINE_CHANGED;
    	}
    	else
    	{
    	    int myLineLen = pmTextPtr -> lineStarts [myFromLine + 1] -
    	        pmTextPtr -> lineStarts [myFromLine] - 1;
    	    
    	    if (myLineLen > pmTextPtr -> longestLineLen)
    	    {
    	    	pmTextPtr -> longestLine = myFromLine;
    	    	pmTextPtr -> secondLongestLineLen = pmTextPtr -> longestLineLen;
    	    	pmTextPtr -> longestLineLen = myLineLen;
    	    	myResultCode |= KEY_LONGEST_LINE_CHANGED;
    	    }
    	    else if (myLineLen > pmTextPtr -> secondLongestLineLen)
    	    {
    	    	pmTextPtr -> secondLongestLineLen = myLineLen;
    	    }
    	}
    }
    else if (pmInsertLines == 1)
    {
    	int	myLineLen;
    	
        myPtr = pmTextPtr -> text + pmFromPos;

    	// Check for longest line
    	if (myFromLine == pmTextPtr -> longestLine)
    	{
    	    myResultCode |= MyRecalculateLongestLine (pmTextPtr);
    	}
        else 
        {
            if (myFromLine < pmTextPtr -> longestLine)
            {
    	    	pmTextPtr -> longestLine++;
            } 
        
            // There are two lines we need to check for the being long, the
            // one the insertion point is on and the one before it.
            // Example:
            //                AB|CD  Inserts 12345\n67890
            //
            //                AC12345
            //                67890|CD
            //
            // Let's see either newly inserted line is the longest 
            // or second longest line
    	    myLineLen = pmTextPtr -> lineStarts [myFromLine + 1] -
    	        pmTextPtr -> lineStarts [myFromLine] - 1;
    	    
    	    if (myLineLen > pmTextPtr -> longestLineLen)
    	    {
    	    	pmTextPtr -> longestLine = myFromLine;
    	    	pmTextPtr -> secondLongestLineLen = pmTextPtr -> longestLineLen;
    	    	pmTextPtr -> longestLineLen = myLineLen;
    	    	myResultCode |= KEY_LONGEST_LINE_CHANGED;
    	    }
    	    else if (myLineLen > pmTextPtr -> secondLongestLineLen)
    	    {
    	    	pmTextPtr -> secondLongestLineLen = myLineLen;
    	    }

	    // If we added a single line to the last line, don't check for
	    // the line after, since it doesn't exist.
	    if (myFromLine < pmTextPtr -> numLines - 1)
	    {
    		myLineLen = pmTextPtr -> lineStarts [myFromLine + 2] -
    		    pmTextPtr -> lineStarts [myFromLine + 1] - 1;
    	    
    		if (myLineLen > pmTextPtr -> longestLineLen)
    		{
    	    	    pmTextPtr -> longestLine = myFromLine + 1;
    	    	    pmTextPtr -> secondLongestLineLen = pmTextPtr -> longestLineLen;
    	    	    pmTextPtr -> longestLineLen = myLineLen;
    	    	    myResultCode |= KEY_LONGEST_LINE_CHANGED;
    		}
    		else if (myLineLen > pmTextPtr -> secondLongestLineLen)
    		{
    	    	    pmTextPtr -> secondLongestLineLen = myLineLen;
    		}
	    } // if (myFromLine < pmTextPtr -> numLines - 1)
        }
    }
    else	   
    {
    	myResultCode |= MyRecalculateLongestLine (pmTextPtr);
    }

    return myResultCode;    
} // MyLineStartsAdjustForInsert

					    
#if !defined (CONSOLE) && !defined (TPROLOG)
/************************************************************************/
/* MyMarkText								*/
/*									*/
/* This creates the items for the mark menu.				*/
/************************************************************************/
static void	MyMarkText (TextPtr pmTextPtr)
{
    if (pmTextPtr -> isJava)
    {
        pmTextPtr -> markLine = EdJava_Mark (pmTextPtr -> text, 
    					     pmTextPtr -> textLen);
    }
    else if (pmTextPtr -> isTuring)
    {    					     
        pmTextPtr -> markLine = EdTur_Mark (pmTextPtr -> text, 
    					    pmTextPtr -> textLen);
    }
} // MyMarkText


/************************************************************************/
/* MyNotifyLinesDeleted							*/
/*									*/
/* Modifies the Error and Mark structures to take into account the 	*/
/* deletion of lines.							*/
/*									*/
/* Cases:   pmFromLine = 2  pmToLine = 3				*/
/* 1	ABC    ABC    ABC    ABC    ABC    ABC    ABC    ABC    ABC	*/
/* 2    [DEF   [DEF   [DEF   D[EF   D[EF   D[EF   DEF[   DEF[   DEF[	*/
/* 3    ]GHI   GH]I   GHI]   ]GHI   GH]I   GHI]   ]GHI   GH]I   GHI]	*/
/* 4	JKL    JKL    JKL    JKL    JKL    JKL    JKL    JKL    JKL	*/
/*									*/
/* 	 V      V      V      V      V      V      V      V      V	*/
/*									*/
/* 1	ABC    ABC    ABC    ABC    ABC    ABC    ABC    ABC    ABC	*/
/* 2    GHI    I             DGHI   DI     D      DEFGHI DEFI   DEF	*/
/* 3	JKL    JKL    JKL    JKL    JKL    JKL    JKL    JKL    JKL	*/
/************************************************************************/
static int	MyNotifyLinesDeleted (TextPtr pmTextPtr, int pmFromLine, 
				      int pmToLine, BOOL pmDelFromEOL)
{
    int		myResultCode;
    
    if (pmDelFromEOL)
    {
    	myResultCode = MyNotifyLinesModified (pmTextPtr, pmFromLine + 1, 
    					      pmToLine);
    }
    else
    {    
    	myResultCode = MyNotifyLinesModified (pmTextPtr, pmFromLine, pmToLine);
    }
    
    // Move the break points, errors and marks coming after the deleted section
    MyNotifyMoveBreaks (pmTextPtr, pmToLine + 1, pmFromLine - pmToLine);
    MyNotifyMoveErrors (pmTextPtr, pmToLine + 1, pmFromLine - pmToLine);
    MyNotifyMoveMarks (pmTextPtr, pmToLine + 1, pmFromLine - pmToLine);

    return myResultCode;
} // MyNotifyLinesDeleted


/************************************************************************/
/* MyNotifyLinesInserted						*/
/*									*/
/* Modifies the Error and Mark structures to take into account the 	*/
/* insertion of text.							*/
/*									*******/
/* Cases:   pmWhere = 2							      */
/* 1  ABC   ABC   ABC   ABC   ABC   ABC   ABC   ABC   ABC   ABC   ABC   ABC   */
/* 2  |DEF  |DEF  |DEF  |DEF  DE|F  DE|F  DE|F  DE|F  DEF|  DEF|  DEF|  DEF|  */
/* 3  GHI   GHI   GHI   GHI   GHI   GHI   GHI   GHI   GHI   GHI   GHI   GHI   */
/*									      */
/* I  X     X\n   \nX   \nX\n X     X\n   \nX   \nX\n X     X\n   \nX   \nX\n */ 
/*									      */
/*     V     V     V     V     V     V     V     V     V     V     V     V    */
/*									      */
/* 1  ABC   ABC   ABC   ABC   ABC   ABC   ABC   ABC   ABC   ABC   ABC   ABC   */
/* 2  XDEF  X                 DEXC  DEX   DE    DE    DEFX  DEFX  DEF   DEF   */
/* 3  GHI   DEF   XDEF  X     GHI   F     XF    X     GHI         X     X     */
/* 4        GHI   GHI   DEF         GHI   GHI   F           GHI   GHI         */
/* 5                    GHI			GHI			GHI   */
/*									      */
/*          -A-         -A-                           -B-   -B-   -B-   -B-   */
/*									      */
/* Case A: pmInsertAtBeginOfLine && pmInsertedTextEndsWithNL		      */
/* Case B: pmInsertAtEndOfLine						      */
/******************************************************************************/
static int	MyNotifyLinesInserted (TextPtr pmTextPtr, int pmWhere, 
				       int pmNumLines, 
				       BOOL pmInsertAtBeginOfLine,
				       BOOL pmInsertAtEndOfLine,
				       BOOL pmInsertedTextEndsWithNL)
{
    int		myResultCode = 0;
    
    if (!(pmInsertAtEndOfLine || 
          (pmInsertAtBeginOfLine && pmInsertedTextEndsWithNL)))
    {
    	myResultCode = MyNotifyLinesModified (pmTextPtr, pmWhere, pmWhere); 
    }
    
    // Move the errors and marks coming after the deleted section
    if (pmInsertAtBeginOfLine)
    {
    	MyNotifyMoveBreaks (pmTextPtr, pmWhere, pmNumLines);
    	MyNotifyMoveErrors (pmTextPtr, pmWhere, pmNumLines);
    	MyNotifyMoveMarks (pmTextPtr, pmWhere, pmNumLines);
    }
    else
    {
    	MyNotifyMoveBreaks (pmTextPtr, pmWhere + 1, pmNumLines);
    	MyNotifyMoveErrors (pmTextPtr, pmWhere + 1, pmNumLines);
    	MyNotifyMoveMarks (pmTextPtr, pmWhere + 1, pmNumLines);
    }

    return myResultCode;
} // MyNotifyLinesInserted


/************************************************************************/
/* MyNotifyLinesModified						*/
/************************************************************************/
static int	MyNotifyLinesModified (TextPtr pmTextPtr, int pmFromLine, 
				       int pmToLine)
{
    BOOL	myBreakRemoved, myErrorRemoved, myMarkRemoved;
    int		myNumLines;
    
    myNumLines = pmFromLine - pmToLine;
    
    // Delete the errors and marks
    myBreakRemoved = MyNotifyRemoveBreaks (pmTextPtr, pmFromLine, pmToLine);
    myErrorRemoved = MyNotifyRemoveErrors (pmTextPtr, pmFromLine, pmToLine);
    myMarkRemoved = MyNotifyRemoveMarks (pmTextPtr, pmFromLine, pmToLine);
    
    // Note: So far, we don't need myBreakRemoved.  It is valid, however, if
    // someone wants to use it in future.
    
    // Return a value
    if (myMarkRemoved && myErrorRemoved)
    {
    	return KEY_MARK_REMOVED | KEY_ERROR_REMOVED;
    } 
    else if (myMarkRemoved)
    {
    	return KEY_MARK_REMOVED;
    }
    else if (myErrorRemoved)
    {
    	return KEY_ERROR_REMOVED;
    }
    else
    {
    	return 0;
    }
} // MyNotifyLinesModified


/************************************************************************/
/* MyNotifyMoveBreaks							*/
/*									*/
/* Move all the break points that fall on pmWhere and beyond by 	*/
/* pmLinesMoved.							*/
/************************************************************************/
static void	MyNotifyMoveBreaks (TextPtr pmTextPtr, int pmWhere, 
				    int pmLinesMoved)
{	
    BreakLinePtr	myBreak;

    myBreak = pmTextPtr -> breakLine;
        	    
    while (myBreak != NULL)
    {
    	if (myBreak -> line >= pmWhere)
    	{
    	    myBreak -> line += pmLinesMoved;
    	}
    	myBreak = myBreak -> next;
    }
} // MyNotifyMoveBreaks


/************************************************************************/
/* MyNotifyMoveErrors							*/
/*									*/
/* Move all the errors that fall on pmWhere and beyond by pmLinesMoved.	*/
/************************************************************************/
static void	MyNotifyMoveErrors (TextPtr pmTextPtr, int pmWhere, 
				    int pmLinesMoved)
{	
    ErrorLinePtr	myError;
    
    myError = pmTextPtr -> errorLine;
        	    
    while (myError != NULL)
    {
    	if (myError -> errorPtr -> line >= pmWhere)
    	{
    	    myError -> errorPtr -> line += pmLinesMoved;
    	}
    	myError = myError -> next;
    }
} // MyNotifyMoveErrors


/************************************************************************/
/* MyNotifyMoveMarks							*/
/*									*/
/* Move all the marks that fall on pmWhere and beyond by pmLinesMoved.	*/
/************************************************************************/
static void	MyNotifyMoveMarks (TextPtr pmTextPtr, int pmWhere, 
				   int pmLinesMoved)
{	
    MarkLinePtr		myMark;

    myMark = pmTextPtr -> markLine;
        	    
    while (myMark != NULL)
    {
    	if (myMark -> line >= pmWhere)
    	{
    	    myMark -> line += pmLinesMoved;
    	}
    	myMark = myMark -> next;
    }
} // MyNotifyMoveMarks


/************************************************************************/
/* MyNotifyRemoveBreaks							*/
/*									*/
/* Remove all the break points that fall between pmFromLine and 	*/
/* pmToLine.								*/
/*									*/
/* Returns TRUE if a mark was deleted.					*/
/************************************************************************/
static BOOL	MyNotifyRemoveBreaks (TextPtr pmTextPtr, int pmFromLine, 
				      int pmToLine)
{	
    BreakLinePtr	myBreak, myPrevBreak, myBreakToDelete;
    BOOL		myBreakRemoved;

    myBreak = pmTextPtr -> breakLine;
    myPrevBreak = NULL;
    myBreakRemoved = FALSE;
        	    
    while ((myBreak != NULL) && (myBreak -> line <= pmToLine))
    {
    	if ((pmFromLine <= myBreak -> line) && (myBreak -> line <= pmToLine))
    	{
    	    if (myPrevBreak == NULL)
    	    {
    	    	pmTextPtr -> breakLine = myBreak -> next;
    	    }
    	    else
    	    {
		myPrevBreak -> next = myBreak -> next;
	    }
	    myBreakToDelete = myBreak;
    	    myBreak = myBreak -> next;
    	    // TW Delete the breakpoint from the Run list.
    	    free (myBreakToDelete);
	    myBreakRemoved = TRUE;
    	}
    	else
    	{
    	    myPrevBreak = myBreak;
    	    myBreak = myBreak -> next;
    	}
    }
    
    return myBreakRemoved;
} // MyNotifyRemoveBreaks


/************************************************************************/
/* MyNotifyRemoveErrors							*/
/*									*/
/* Remove all the errors that fall between pmFromLine and pmToLine.	*/
/*									*/
/* Returns TRUE if an error was deleted.				*/
/************************************************************************/
static BOOL	MyNotifyRemoveErrors (TextPtr pmTextPtr, int pmFromLine, 
				      int pmToLine)
{	
    ErrorLinePtr	myError, myPrevError, myErrorToDelete;
    BOOL		myErrorRemoved;
    
    myError = pmTextPtr -> errorLine;
    myPrevError = NULL;
    myErrorRemoved = FALSE;
        	    
    while ((myError != NULL) && (myError -> errorPtr -> line <= pmToLine))
    {
    	if ((pmFromLine <= myError -> errorPtr -> line) && 
    	    (myError -> errorPtr -> line <= pmToLine))
    	{
    	    if (myPrevError == NULL)
    	    {
    	    	pmTextPtr -> errorLine = myError -> next;
    	    	if (pmTextPtr -> currentError == myError)
    	    	{
    	    	    pmTextPtr -> currentError = NULL;
    	    	}
    	    }
    	    else
    	    {
		myPrevError -> next = myError -> next;
    	    	if (pmTextPtr -> currentError == myError)
    	    	{
    	    	    pmTextPtr -> currentError = myPrevError;
    	    	}
	    }
	    myErrorToDelete = myError;
    	    myError = myError -> next;
    	    EdErr_DeleteError (myErrorToDelete -> errorPtr);
    	    free (myErrorToDelete);
	    myErrorRemoved = TRUE;
    	}
    	else
    	{
    	    myPrevError = myError;
    	    myError = myError -> next;
    	}
    }
    
    return myErrorRemoved;
} // MyNotifyRemoveErrors


/************************************************************************/
/* MyNotifyRemoveMarks							*/
/*									*/
/* Remove all the marks that fall between pmFromLine and pmToLine.	*/
/*									*/
/* Returns TRUE if a mark was deleted.					*/
/************************************************************************/
static BOOL	MyNotifyRemoveMarks (TextPtr pmTextPtr, int pmFromLine, 
				     int pmToLine)
{	
    MarkLinePtr		myMark, myPrevMark, myMarkToDelete;
    BOOL		myMarkRemoved;

    myMark = pmTextPtr -> markLine;
    myPrevMark = NULL;
    myMarkRemoved = FALSE;
        	    
    while ((myMark != NULL) && (myMark -> line <= pmToLine))
    {
    	if ((pmFromLine <= myMark -> line) && (myMark -> line <= pmToLine))
    	{
    	    if (myPrevMark == NULL)
    	    {
    	    	pmTextPtr -> markLine = myMark -> next;
    	    }
    	    else
    	    {
		myPrevMark -> next = myMark -> next;
	    }
	    myMarkToDelete = myMark;
    	    myMark = myMark -> next;
    	    free (myMarkToDelete -> name);
    	    free (myMarkToDelete);
	    myMarkRemoved = TRUE;
    	}
    	else
    	{
    	    myPrevMark = myMark;
    	    myMark = myMark -> next;
    	}
    }
    
    return myMarkRemoved;
} // MyNotifyRemoveMarks
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)


/************************************************************************/
/* MyRecalculateLongestLine						*/
/************************************************************************/
static int	MyRecalculateLongestLine (TextPtr pmTextPtr)
{
    int	myPrevLongestLineLen, myLineLen;
    int	cnt;
        
    myPrevLongestLineLen = pmTextPtr -> longestLineLen;
    
    pmTextPtr -> longestLine = 0;
    pmTextPtr -> longestLineLen = 0;
    pmTextPtr -> secondLongestLineLen = 0;
    
    for (cnt = 1 ; cnt <= pmTextPtr -> numLines ; cnt++)
    {
	myLineLen = pmTextPtr -> lineStarts [cnt] -
	    pmTextPtr -> lineStarts [cnt - 1] - 1;
	if (myLineLen > pmTextPtr -> longestLineLen)
	{
	    pmTextPtr -> longestLine = cnt - 1;
	    pmTextPtr -> secondLongestLineLen = pmTextPtr -> longestLineLen;
	    pmTextPtr -> longestLineLen = myLineLen;
	}
	else if (myLineLen > pmTextPtr -> secondLongestLineLen)
	{
	    pmTextPtr -> secondLongestLineLen = myLineLen;
	}
    }
    
    if (myPrevLongestLineLen == pmTextPtr -> longestLineLen)
    {
    	return 0;
    }
    else
    {
    	return KEY_LONGEST_LINE_CHANGED;
    }
} // MyRecalculateLongestLine


/************************************************************************/
/* MyReturnStatus							*/
/*									*/
/* This returns an integer suitable for returning from any of the 	*/
/* selection  functions.  If there was a selection and now there isn't,	*/
/* return HIDE, if there was no selection and now there is, return 	*/
/* CREATE, and if there was and still is, or wasn't and still isn't,	*/
/* return REDRAW.							*/
/************************************************************************/
static int	MyReturnStatus (TextPtr pmTextPtr, BOOL pmWasSelection)
{
    if (pmTextPtr -> selFrom == pmTextPtr -> selTo)
    {
    	if (pmWasSelection)
    	{
    	    return HIDE_SELECTION;
    	}
    	else
    	{
    	    return MOVE_CARET;
    	}
    }
    else if (!pmWasSelection && (pmTextPtr -> selFrom != pmTextPtr -> selTo))
    {
    	return CREATE_SELECTION;
    }
    return REDRAW;    
} // MyReturnStatus


#if !defined (CONSOLE) && !defined (TPROLOG)
/************************************************************************/
/* MySetDirty								*/
/************************************************************************/
static int	MySetDirty (TextPtr pmTextPtr)
{
    BOOL	myWasDirty = pmTextPtr -> textLen;
    
    if ((pmTextPtr -> textLen == pmTextPtr -> lastSavedTextLen) &&
        (EdText_CalculateCRC (pmTextPtr) == pmTextPtr -> lastSavedCRC))
    {
    	pmTextPtr -> isDirty = FALSE;
    }
    else
    {
    	pmTextPtr -> isDirty = TRUE;
    }
    pmTextPtr -> isChangedSinceLastRun = TRUE;
    
    if (pmTextPtr -> isDirty == myWasDirty)
    {
    	return 0;
    }
    else
    {
    	return KEY_FILE_DIRTY;
    }
} // MySetDirty
#endif // #if !defined (CONSOLE) && !defined (TPROLOG)


/************************************************************************/
/* MyVerifyLineDB							*/
/*									*/
/* Used for debugging, this makes certain that the Line DB is the	*/
/* format expected.						 	*/
/************************************************************************/
static void	MyVerifyLineDB (TextPtr pmTextPtr)
{
    int		myLongestLine, myLongestLineLen, mySecondLongestLine, myLen;
    int		cnt;
    char	*myText;
    
    // Test first lineStart
    if (pmTextPtr -> lineStarts [0] != 0)
    {
    	EdFail_Fatal (IDS_LINEDBVERIFY1_FAILED, __FILE__, __LINE__, 0, 1, 0, 0);
    }   
    
    // Test rest of line starts
    for (cnt = 1 ; cnt <= pmTextPtr -> numLines ; cnt++)
    {
    	if (pmTextPtr -> text [pmTextPtr -> lineStarts [cnt] - 1] != '\n')
    	{
    	    if (pmTextPtr -> numLines <= 20)
    	    {
    	    	myText = pmTextPtr -> text;
    	    	myText [pmTextPtr -> textLen] = 0;
    	    }
    	    else
    	    {
    	        myText = "";
    	    }
    	
    	    EdFail_Fatal (IDS_LINEDBVERIFY_FAILED, __FILE__, __LINE__, 0, 2, 
    	        pmTextPtr -> numLines, pmTextPtr -> textLen,
    	        cnt, 0, myText);
	}
    }
    
    // Test number of line starts
    if (pmTextPtr -> lineStarts [pmTextPtr -> numLines] != pmTextPtr -> textLen)
    {
    	if (pmTextPtr -> numLines <= 20)
    	{
    	    myText = pmTextPtr -> text;
    	    myText [pmTextPtr -> textLen] = 0;
    	}
    	else
    	{
    	    myText = "";
    	}
    	
    	EdFail_Fatal (IDS_LINEDBVERIFY_FAILED, __FILE__, __LINE__, 0, 3, 
    	    pmTextPtr -> numLines, pmTextPtr -> textLen,
    	    pmTextPtr -> lineStarts [pmTextPtr -> numLines], 
    	    0, myText);
    }

    // Test longest lines    
    myLongestLine = 0;
    myLongestLineLen = 0;
    mySecondLongestLine = 0;
    for (cnt = 1 ; cnt <= pmTextPtr -> numLines ; cnt++)
    {
    	myLen = pmTextPtr -> lineStarts [cnt] - 
    	    pmTextPtr -> lineStarts [cnt - 1] - 1;
	if (myLen > myLongestLineLen)
	{
	    mySecondLongestLine = myLongestLineLen;
	    myLongestLineLen = myLen;
	    myLongestLine = cnt - 1;
	}
	else if (myLen > mySecondLongestLine)
	{
	    mySecondLongestLine = myLen;
	}
    }	
    
    // Handle the fact that there may be more than one line with the same
    // length
    if (pmTextPtr -> longestLine == pmTextPtr -> numLines)
    {
    	myLen = 0;
    }
    else
    {
    	myLen = pmTextPtr -> lineStarts [pmTextPtr -> longestLine + 1] - 
    	    pmTextPtr -> lineStarts [pmTextPtr -> longestLine] - 1;
    }
    	
    if ((myLongestLine != pmTextPtr -> longestLine) && 
        (myLongestLineLen != myLen))
    {
    	if (pmTextPtr -> numLines <= 20)
    	{
    	    myText = pmTextPtr -> text;
    	    myText [pmTextPtr -> textLen] = 0;
    	}
    	else
    	{
    	    myText = "";
    	}
    	
    	EdFail_Fatal (IDS_LINEDBVERIFY_FAILED, __FILE__, __LINE__, 0, 4, 
    	    pmTextPtr -> numLines, pmTextPtr -> textLen,
    	    myLongestLine, pmTextPtr -> longestLine, myText);
    }
    if (myLongestLineLen != pmTextPtr -> longestLineLen)
    {
    	if (pmTextPtr -> numLines <= 20)
    	{
    	    myText = pmTextPtr -> text;
    	    myText [pmTextPtr -> textLen] = 0;
    	}
    	else
    	{
    	    myText = "";
    	}
    	
    	EdFail_Fatal (IDS_LINEDBVERIFY_FAILED, __FILE__, __LINE__, 0, 5, 
    	    pmTextPtr -> numLines, pmTextPtr -> textLen,
    	    myLongestLineLen, pmTextPtr -> longestLineLen, myText);
    }
    if (mySecondLongestLine > pmTextPtr -> secondLongestLineLen)
    {
    	if (pmTextPtr -> numLines <= 20)
    	{
    	    myText = pmTextPtr -> text;
    	    myText [pmTextPtr -> textLen] = 0;
    	}
    	else
    	{
    	    myText = "";
    	}
    	
    	EdFail_Fatal (IDS_LINEDBVERIFY_FAILED, __FILE__, __LINE__, 0, 6, 
    	    pmTextPtr -> numLines, pmTextPtr -> textLen,
    	    mySecondLongestLine, pmTextPtr -> secondLongestLineLen, myText);
    }
} // MyVerifyLineDB
