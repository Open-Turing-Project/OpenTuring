/**************/
/* miolexer.c */
/**************/

/*******************/
/* System includes */
/*******************/
#include <malloc.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

/****************/
/* Self include */
/****************/
#include "miolexer.h"

/******************/
/* Other includes */
/******************/
#include "mio.h"
#include "mdio.h"
#include "miotypes.h"
#include "miofile.h"
#include "mioerr.h"

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/
#define EOS			  0

#define MAX_NUM_TABLES		 10
/*
#define MAX_NUM_STREAMS		256
*/
#define MAX_NUM_COMMENTS	  5
#define MAX_COMMENT_SIZE	  5
/*
#define MAX_NUM_WHITESPACE	 20
*/
#define MAX_NUM_TOKENS		500
#define MAX_TOKEN_SIZE		 30

#define MAX_NUM_NEOS		  5
#define MAX_NEOS_SIZE		  5

#define MAX_NUM_BEGINCHAR	100
#define MAX_NUM_SPECCHAR	100

#define MAX_NUM_EXPCHAR		 10

#define MAX_NUM_ESCAPE		  2

#define MAX_NUM_PREFIX		 10
#define MAX_PREFIX_SIZE		  5
/*
#define MAX_NUM_SUFFIX		 10
#define MAX_SUFFIX_SIZE		  5
*/
#define NONE		       (-1)

#define NUM_HASH_SLOTS		737

// Flag bits - THESE MUST MATCH THOSE IN Lexer class 

// This is reversed in meaning here
#define	FLAG_EVAL		0x1	
#define FLAG_NONEWLINE		0x2
#define FLAG_NOSPACE		0x4
#define FLAG_NOCOMMENT		0x8

#define FLAG_OOTESCAPE		0x100
#define FLAG_CESCAPE		0x200

#define NL	'\n'
#define HT	'\t'
#define CR	'\r'

#define tIntLit		0
#define tRealLit	1
#define tStringLit	2
#define tCharLit	3
#define tComment	4
#define tIdentifier	5
#define tNewLine	6
#define tWhitespace	7
#define tEOF		8
#define tOther		9

#define tTokenBase	100


/********************/
/* Global variables */
/********************/

/*********/
/* Types */
/*********/
typedef struct LexTable 
{
    char	tableName[STRLEN];

    char	commentStart[MAX_NUM_COMMENTS][MAX_COMMENT_SIZE+1];
    char	commentEnd[MAX_NUM_COMMENTS][MAX_COMMENT_SIZE+1];
    UINT	numComments;

    char	startStringChar;
    char	endStringChar;

    char	charDelimitter;

    char	notEOS[MAX_NUM_NEOS][MAX_NEOS_SIZE+1];
    UINT	numNotEOS;

    char	beginIDchars[MAX_NUM_BEGINCHAR+1];
    char	specialIDchars[MAX_NUM_SPECCHAR+1];

    char	realExpChars[MAX_NUM_EXPCHAR+1];

    char	escapeChars[MAX_NUM_ESCAPE+1];

    char	integerPrefix[MAX_NUM_PREFIX][MAX_PREFIX_SIZE+1];
    BYTE	preBaseConvert[MAX_NUM_PREFIX];
    UINT	numIntegerPrefix;

    char	baseConvertChar;

    char        token[MAX_NUM_TOKENS][MAX_TOKEN_SIZE+1];
    UINT	numTokens;

    short       hashTable[NUM_HASH_SLOTS];
} LexTable;

typedef struct LexRecord
{
    char	fileName[STRLEN];
    UINT	lineNo;

    void	*fp;
    LexTable	*table;
    UINT	flags;

    /* Scanner state for this stream */

    char	line[1024];
    int		pos;

    BOOL	seenEOF;
    BOOL	inString;
    int		commentNum;
} LexRecord;

/**********************/
/* External variables */
/**********************/

/********************/
/* Static constants */
/********************/

/********************/
/* Static variables */
/********************/
static LexTable	*stLexTable [MAX_NUM_TABLES];
static int	stNumTables = 0;

/******************************/
/* Static callback procedures */
/******************************/

/*********************/
/* Static procedures */
/*********************/
static LexRecord	*MyCreateLexRecord (void);
static LexTable		*MyInitializeTable (const char *pmTableFile);
static void		MyReadTable (void *pmFilePointer, LexTable *pmLexTable);
static BOOL		MyIsThrowAwayToken (LexRecord *pmLexRecord, 
					    TOKEN_RECORD *pmTokRecord);

/**************************************/
/* Static procedures in miolexread.in */
/**************************************/
static BOOL	MyGetSection (char *pmSection);
static void	MyReadChar (LexTable *pmLexTable);
static void	MyReadComments (LexTable *pmLexTable);
static void	MyReadIdentifier (LexTable *pmLexTable);
static void	MyReadInteger (LexTable *pmLexTable);
static void	MyReadReal (LexTable *pmLexTable);
static void	MyReadString (LexTable *pmLexTable);
static void	MyReadTokens (LexTable *pmLexTable);
static void	MyTableError (const char *pmStr);
static char	*MyTget (char *pmBuf, int pmSize);

/**************************************/
/* Static procedures in miolexscan.in */
/**************************************/
static UINT	MyHash (const char *pmStr);
static void	MyNextToken (LexRecord *pmLexRecord, TOKEN_RECORD *pmTokRecord);
static void	MyGetComment(LexRecord *pmLexRecord, TOKEN_RECORD *pmTokRecord);

/*********************************************/
/* External procedures for Turing predefines */
/*********************************************/
/************************************************************************/
/* MIOLexer_Begin							*/
/************************************************************************/
UINT	MIOLexer_Begin (const char *pmSrcFile, const char *pmTableFile, 
		        OOTint pmFlags, SRCPOS *pmSrcPos)
{
    LexTable	*myLexTable;
    LexRecord	*myLexRecord;
    char	mySrcPath [1024];
    int		myLexID;

    myLexTable = MyInitializeTable (pmTableFile);

    if (!myLexTable)
    {
	return 0;
    }

    if (!MIOFile_ConvertPath (pmSrcFile, NULL, mySrcPath, NO_TRAILING_SLASH))
    {
	return 0;
    }

    myLexRecord = MyCreateLexRecord ();
    if (myLexRecord == NULL)
    {
	return 0;
    }

    myLexRecord -> fp = MDIO_DiskFile_Open (mySrcPath, "r");
    if (myLexRecord -> fp == NULL) 
    {
	free (myLexRecord);
	return 0;
    }
    
    myLexRecord -> table = myLexTable;
    myLexRecord -> flags = pmFlags ^ FLAG_EVAL;
    
    strcpy (myLexRecord -> fileName, mySrcPath);
    myLexRecord -> lineNo = 0;
    
    myLexID = MIO_IDAdd (LEXER_ID, myLexRecord, pmSrcPos, mySrcPath, NULL);

    // Couldn't allocate an identifier.  Return an illegal ID.
    if (myLexID == 0)
    {
	free (myLexRecord);
	return 0;
    }
        	    
    return myLexID;
} // MIOLexer_Begin


/************************************************************************/
/* MIOLexer_End								*/
/************************************************************************/
void	MIOLexer_End (OOTint pmLexID)
{
    LexRecord	*myLexRecord;

    myLexRecord = MIO_IDGet (pmLexID, LEXER_ID);
    if (myLexRecord == NULL)
    {
    	return;
    }
    MDIO_DiskFile_Close (myLexRecord -> fp);
    free (myLexRecord);
    MIO_IDRemove (pmLexID, LEXER_ID);
} // MIOLexer_End


/************************************************************************/
/* MIOLexer_Scan							*/
/************************************************************************/
void	MIOLexer_Scan (OOTint pmLexID, TOKEN_RECORD *pmTokRecord)
{
    LexRecord	*myLexRecord;

    myLexRecord = MIO_IDGet (pmLexID, LEXER_ID);
    if (myLexRecord == NULL)
    {
    	return;
    }

    do 
    {
        MyNextToken (myLexRecord, pmTokRecord);
    } while (MyIsThrowAwayToken(myLexRecord, pmTokRecord));
} // MIOLexer_Scan


/***************************************/
/* External procedures for MIO library */
/***************************************/
/************************************************************************/
/* MIOLexer_Init							*/
/************************************************************************/
void	MIOLexer_Init (void)
{
    int	cnt;

    for (cnt = 0; cnt < MAX_NUM_TABLES; cnt++)
    {
	stLexTable [cnt] = NULL;
    }
} // MIOLexer_Init


/************************************************************************/
/* MIOLexer_Finalize_Run						*/
/************************************************************************/
void	MIOLexer_Finalize_Run (void)
{
    int	cnt;

    for (cnt = 0; cnt < stNumTables; cnt++)
    {
	free (stLexTable [cnt]);
	stLexTable [cnt] = NULL;
    }

    stNumTables = 0;
} // MIOLexer_Finalize_Run


/******************************/
/* Static callback procedures */
/******************************/


/*********************/
/* Static procedures */
/*********************/
/************************************************************************/
/* MyCreateLexRecord							*/
/************************************************************************/
static LexRecord	*MyCreateLexRecord (void)
{
    LexRecord	*myLexRecord;

    myLexRecord = (LexRecord *) malloc (sizeof (LexRecord));
    if (myLexRecord == NULL)
    {
	SET_ERRNO (E_LEX_CANT_ALLOC_MEM_FOR_TABLE);
	return NULL;
    }

    myLexRecord -> pos        = NONE;
    myLexRecord -> seenEOF    = FALSE;
    myLexRecord -> inString   = FALSE;
    myLexRecord -> commentNum = NONE;

    return myLexRecord;
} // MyCreateLexRecord


/************************************************************************/
/* MyCreateLexRecord							*/
/************************************************************************/
static LexTable		*MyInitializeTable (const char *pmTableFile)
{
    LexTable	*myLexTable;
    char	myTablePath [1024];
    void	*myFilePointer;
    int		cnt;

    if (!MIOFile_ConvertPath (pmTableFile, NULL, myTablePath, 
			      NO_TRAILING_SLASH))
    {
	return NULL;
    }

    // If this table file has already been open, then keep using it.
    for (cnt = 0; cnt < stNumTables; cnt++) 
    {
	if (strcmp (myTablePath, stLexTable [cnt] -> tableName) == 0)
	    return stLexTable [cnt];
    }

    myFilePointer = MDIO_DiskFile_Open (myTablePath, "r");
    if (myFilePointer == NULL) 
    {
	return NULL;
    }

    myLexTable = (LexTable *) malloc (sizeof (LexTable));

    if (myLexTable == NULL)
    {
	ABORT_WITH_ERRNO (E_INSUFFICIENT_MEMORY);
    }

    if (stNumTables == MAX_NUM_TABLES)
    {
	ABORT_WITH_ERRNO (E_LEX_TABLE_ERROR);
    }

    stLexTable [stNumTables] = myLexTable;
    stNumTables++;

    /* Initialize some fields */
    strcpy (myLexTable -> tableName, myTablePath);

    myLexTable -> numComments      = 0;
    myLexTable -> numTokens        = 0;
    myLexTable -> numIntegerPrefix = 0;
    myLexTable -> numNotEOS        = 0;

    myLexTable -> startStringChar = EOS;
    myLexTable -> endStringChar   = EOS;
    myLexTable -> baseConvertChar = EOS;
    myLexTable -> charDelimitter  = EOS;

    myLexTable -> beginIDchars [0]   = EOS;
    myLexTable -> specialIDchars [0] = EOS;
    myLexTable -> realExpChars [0]   = EOS;
    myLexTable -> escapeChars [0]    = EOS;

    for (cnt = 0; cnt < NUM_HASH_SLOTS; cnt++)
    {
	myLexTable -> hashTable [cnt] = NONE;
    }

    MyReadTable (myFilePointer, myLexTable);
    MDIO_DiskFile_Close (myFilePointer);

    return myLexTable;
} // MyInitializeTable


/************************************************************************/
/* IsThrowAwayToken							*/
/************************************************************************/
static BOOL	MyIsThrowAwayToken (LexRecord *pmLexRecord, 
				    TOKEN_RECORD *pmTokRecord)
{
    switch (pmTokRecord -> tokNum)
    {
	case tComment:
	    return (pmLexRecord -> flags & FLAG_NOCOMMENT);

	case tNewLine:
	    return (pmLexRecord -> flags & FLAG_NONEWLINE);

	case tWhitespace:
	    return (pmLexRecord -> flags & FLAG_NOSPACE);
    }
    return FALSE;
} // IsThrowAwayToken


#include "miolexscan.in"
#include "miolexread.in"

