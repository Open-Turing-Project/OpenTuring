/************/
/* edtext.h */
/************/

/************************************************************************/
/* Symbols Used: 							*/
/*  JAVA - Defined if compiling "Ready for Java", not defined if	*/
/* 	   compiling Turing.						*/
/*  CONSOLE - Defined if compiling JavaSupervisor, not defined if 	*/
/*	      compiling the editor.					*/
/************************************************************************/

#ifndef _EDTEXT_H_
#define _EDTEXT_H_

/*******************/
/* System includes */
/*******************/
#include <windows.h>

/******************/
/* Other includes */
/******************/
#ifdef CONSOLE
    #include "jsdialogs.h"
#endif // #ifdef CONSOLE
#include "ederr.h"

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/
// Text type found in the textType buffer
#define NUM_TEXT_TYPES			13

#define TEXT_NORMAL			0
#define TEXT_IDENTIFIER			1
#define TEXT_PREDEF			2
#define TEXT_KEYWORD			3
// Any text in a double quoted string including the initial double quote
#define TEXT_DQSTRING			4
// The concluding double quote
#define TEXT_DQSTRING_CLOSE		5
// Any text in a single quoted string including the initial single quote
#define TEXT_SQSTRING			6
// The concluding single quote
#define TEXT_SQSTRING_CLOSE		7
// A single line comment (% in Turing, // in Java)
#define TEXT_LINE_COMMENT		8
// The second char in the start of a line comment (the second "/" in "//")
#define TEXT_LINE_COMMENT_OPEN		9
// A long style comment /* .. */
#define TEXT_LONG_COMMENT		10
// The second char in the start of a long comment (the "*" in "/*")
#define TEXT_LONG_COMMENT_OPEN		11
// The second char in the close of a long comment (the "/" in "*/")
#define TEXT_LONG_COMMENT_CLOSE		12
// The text type for characters not yet assignde a text type
#define TEXT_NO_CLASSIFICATION		13

#define NO_TOKEN			-1

// Results from caret and selection movement commands
#define NO_REDRAW			1
#define HIDE_SELECTION			2
#define CREATE_SELECTION		3
#define REDRAW				4
#define MOVE_CARET			5

// Results from EdText_Key  Values may be or'd together
#define KEY_ILLEGAL_CHAR		0x01
#define KEY_REDRAW_FROM_SELECTION	0x02
#define KEY_OUT_OF_MEMORY		0x04
#define KEY_UPDATE_LINE			0x08
#define KEY_LONGEST_LINE_CHANGED	0x10
#define KEY_MARK_REMOVED		0x20
#define KEY_ERROR_REMOVED		0x40
#define KEY_CANT_DELETE			0x80
#define KEY_NUM_LINES_CHANGED		0x100
#define KEY_FILE_DIRTY			0x200
#define KEY_SOME_LINES_NOT_COMMENTS	0x400

// Parameters for EdText_SetNewText
#define ERASE_UNDO			TRUE
#define DONT_ERASE_UNDO			FALSE

// For undo functions
#define UNDO_VALID			1
#define UNDO_MORE_KEYSTROKES		2

#define UNDO_ALL_TEXT			-99

#define UNDO				TRUE
#define REDO				FALSE

// Results from EdText_CompleteStructure
#define CANT_COMPLETE_JAVA		1
#define INDENT_TOO_DEEP			2
#define NO_IDENT_FOUND			3
#define STRUCTURE_COMPLETED		4
#define CANNOT_COMPLETE_STRUCTURE	5

/*********/
/* Types */
/*********/
typedef struct
{
    int		numSpacesPerTab;
#ifndef CONSOLE
    int		indentSize;
#endif // #ifndef CONSOLE
    
    // Debugging
    BOOL	verifyLineDB;
} EdTextOnly_Properties;

typedef struct
{
    EdTextOnly_Properties	myProperties;
} EdText_Properties;

#ifndef CONSOLE
    typedef struct ErrorLine
    {
        Error			*errorPtr;	// Pointer to EdErr error
        struct ErrorLine	*next;	// Next line to be highlighted
    } ErrorLine, *ErrorLinePtr;

    typedef struct MarkLine
    {
        int			line;	// Line of subprogram declaration
        char			*name;	// Name of the mark
	struct MarkLine		*next;	// Next subprogram in the chain
    } MarkLine, *MarkLinePtr;

    typedef struct BreakLine
    {
        int			line;	// Line of subprogram declaration
	struct BreakLine	*next;	// Next subprogram in the chain
    } BreakLine, *BreakLinePtr;

    typedef struct UndoBuffer
    {
    	// The undo buffer    
    	int			menuName;	// The operation to be undone
    	int			replFrom;	// Undo replaces from replFrom
    	int			replTo;		// Undo replaces to replTo
    	char			*text;		// Undo replaces with this text
    	int			textSize;	// The Undo replace text size
    	int			selFrom;	// The selection from to restore 
    	int			selTo;		// The selection to to restore
	struct UndoBuffer	*prev, *next;	// Links
    	BOOL			moreKeystrokes;	// Are we accumulating strokes
    } UndoBuffer, *UndoBufferPtr;
#endif // #ifndef CONSOLE

/************************************************************************/
/* Notes about the text data structure					*/
/*									*/
/* text - The text w/o tabs broken by '\n'.  After any operation, the	*/
/*	  last character in the buffer (text [textLen - 1]) is '\n'	*/
/* texttype - Is exactly the same length as text.			*/
/* numLines - The number of lines of text.  Note that this doesn't	*/
/*            include the "phantom" line that the cursor can go to	*/
/*	      at the end of the text.  					*/
/*		   i.e.							*/
/*			ABCEF						*/
/*			DEF						*/
/*			|						*/
/*            In this example, numLines = 2, even though the cursor can	*/
/*	      go to the third line.					*/
/* longestLine - The line numbers are all 0 based internally.  In the	*/
/*               above example, ABCDEF is line 0 and DEF is line 1.	*/
/* newLineAdded - The edit procedures will add a newline if the user's	*/
/*                change left the end of the text w/o a '\n'.  This can	*/
/*		  happen if user types a character when the cursor	*/
/*		  was on the "phantom" line.  It's when the newline is	*/
/*		  added that numLines changes.				*/
/* lineStarts - This array goes from 0 to numLines (so it actually has	*/
/*		numLines + 1 members.  It's value is the position of	*/
/*		the first character in each line.  For the last element	*/
/*		of lineStarts, this is what would be the first char	*/
/*		of the "phantom" line, is actually beyond the end of 	*/
/*		text and is equal to textLen.				*/
/* selFrom, selTo - This is the selection.  Note that the selection can	*/
/*		    be thought of as extending from before character	*/
/*		    at selFrom to before the character at selTo.  If	*/
/*		    if selFrom and selTo are the same, the selection is	*/
/*		    only a position and contains no text.  A selection	*/
/*		    from the beginning of line "x" is one where selFrom	*/
/*		    is the same as lineStarts [x]. A selection to the	*/
/*		    end of line y is one where selTo is the same as	*/
/*		    lineStarts [y + 1].  Note that selFrom and selTo	*/
/*		    go from 0 - textLen, where textLen indicates the	*/
/*		    selFrom or selTo are on the "phantom" line.  There	*/
/*		    is no guarantee that selFrom is less than selTo.	*/
/************************************************************************/
typedef struct
{
    char	*text;		// The buffer for the text
#ifndef CONSOLE	
	BYTE	*textType;	// The buffer for the colours
#endif // #ifndef CONSOLE
    BOOL	isJava;		// Is saved as .java or no suffix and in java
    BOOL	isTuring;	// Is saved as .t or no suffix and and in tur
    int		textBufferLen;	// Size of the text and colour buffers in bytes
    int		textLen;	// The number of characters in the text buffer
    int 	numLines;	// Number of lines of text in the text buffer
    int		longestLine;	// The line # of the longest line
    int		longestLineLen;	// The length of the longest line (not incl NL)
    int		secondLongestLineLen;	// The length of the 2nd longest line
#ifndef CONSOLE	
	ErrorLinePtr errorLine;	// Link to lines to be highlighted
	ErrorLinePtr currentError;	// Link to the last displayed error
	MarkLinePtr  markLine;	// Link to entries in mark menu
	BreakLinePtr breakLine;	// Link to lines that are breakpoints
#endif // #ifndef CONSOLE
    BOOL	newLineAdded;	// TRUE if newline added
#ifndef CONSOLE	
	BOOL	isDirty;	// TRUE if text modified since last save
	BOOL	isChangedSinceLastRun; // TRUE if text modified since last run
	int	lastSavedTextLen;	// The length of the text when
    					// last saved.
	UINT	lastSavedCRC;	// The CRC value of the text when last saved
#endif // #ifndef CONSOLE
    int		*lineStarts;	// The starting position of each line of text
    int		lineStartsBufferLen;	// The size of the line starts buffer in
    					// integers.
    int		selFrom;	// The starting position of the selection
    int		selTo;		// The ending position of the selection
    
    int		selWordFrom;	// The starting position of the selected word
    int		selWordTo;	// The ending position of the selected word
    
    int		virtualCol;	// Original location when arrowing up/down

    // Buffered information for fast lookup
    BOOL	locationOK;	// Whether the buffered location is still 
    				// accurate. Becomes false when the selection
    				// or the lineStarts change
    				
    int		lastLineFrom;	// The buffered line and col of the selection
    int		lastColFrom;
    int		lastLineTo;
    int		lastColTo;
        				  
#ifndef CONSOLE
	// Undo/redo buffer
	UndoBufferPtr	undoHead, undoTail;
	UndoBufferPtr	redoHead, redoTail;
#endif // #ifndef CONSOLE
} TextRecord, *TextPtr;

/**********************/
/* External variables */
/**********************/

/***********************/
/* External procedures */
/***********************/
#ifndef CONSOLE
    // Init, Finalize subprograms
    extern BOOL	EdText_Init (void);
    extern void	EdText_Finalize (void);
#endif // #ifndef CONSOLE
    // Properties subprograms
    extern void	EdText_PropertiesSet (EdText_Properties pmProperties);
#ifndef CONSOLE
    extern void	EdText_PropertiesImplementChanges (TextPtr pmTextPtr);
#endif // #ifndef CONSOLE

// Create, Dispose subprograms
#ifndef CONSOLE
extern void	EdText_ClassifyText (TextPtr pmTextPtr);
#endif
extern TextPtr	EdText_Create (void);
extern void	EdText_Dispose (TextPtr pmTextPtr);

// Other subprograms

//
// Misc
//
#ifndef CONSOLE
    extern void	EdText_AddError (TextPtr pmTextPtr, ErrorPtr pmError);
    extern UINT	EdText_CalculateCRC (TextPtr pmTextPtr);
    extern void	EdText_ClearAllBreakpoints (TextPtr pmTextPtr);
    extern void	EdText_ClearErrors (TextPtr pmTextPtr);
    extern int	EdText_CompleteStructure (TextPtr pmTextPtr, int *pmAction);
#endif // #ifndef CONSOLE

extern void	EdText_GetSelectionLinesCols (TextPtr pmTextPtr, 
					       int *myFromLine, int *myFromCol, 
					       int *myToLine, int *myToCol);
extern void	EdText_GetSelectionLocation (TextPtr pmTextPtr, int *pmLine,
    					      int *pmCol);
    					      
#ifndef CONSOLE					       
    extern int	EdText_GotoNextError (TextPtr pmTextPtr, 
				      const char **pmErrorMessage);
#endif // #ifndef CONSOLE

extern void	EdText_InitializeTextRecord (TextPtr pmTextPtr);

#ifndef CONSOLE
    extern BOOL	EdText_Indent (HWND pmWindowForStatus, TextPtr pmTextPtr);
#endif // #ifndef CONSOLE

extern BOOL	EdText_IsEmpty (TextPtr pmTextPtr);

#ifndef CONSOLE
    extern BOOL	EdText_IsIdentChar (char pmChar);
    extern int	EdText_Key (TextPtr pmTextPtr, char pmChar, 
			    char **pmDeletedChars);
    extern int	EdText_KeyDelete (TextPtr pmTextPtr, char **pmDeletedChars);
    extern int	EdText_KeyBackTab (TextPtr pmTextPtr, char **pmDeletedChars);

    //
    // Move caret
    //
    extern int	EdText_MoveColumnLeft (TextPtr pmTextPtr, BOOL pmSelect);
    extern int	EdText_MoveColumnRight (TextPtr pmTextPtr, BOOL pmSelect);
    extern int	EdText_MoveLineBegin (TextPtr pmTextPtr, BOOL pmSelect);    
    extern int	EdText_MoveLineDown (TextPtr pmTextPtr, BOOL *pmMoved, 
    				      BOOL pmSelect);
    extern int	EdText_MoveLineEnd (TextPtr pmTextPtr, BOOL pmSelect);    
    extern int	EdText_MoveLineUp (TextPtr pmTextPtr, BOOL pmSelect);
    extern int	EdText_MovePageDown (TextPtr pmTextPtr, int pmPageSize, 
    				      BOOL *pmMoved, BOOL pmSelect);
    extern int	EdText_MovePageUp (TextPtr pmTextPtr, int pmPageSize,
				    BOOL pmSelect);
    extern int	EdText_MoveProgramBottom (TextPtr pmTextPtr, BOOL pmSelect);    
    extern int	EdText_MoveProgramTop (TextPtr pmTextPtr, BOOL pmSelect);    
    extern int	EdText_MoveWordNext (TextPtr pmTextPtr, BOOL pmSelect);
    extern int	EdText_MoveWordPrev (TextPtr pmTextPtr, BOOL pmSelect);
#endif // #ifndef CONSOLE

//
// Move selection
//
extern int	EdText_SelectAll (TextPtr pmTextPtr);
extern int	EdText_SelectExtendSelection (TextPtr pmTextPtr, int pmLine, 
					      int pmCol);
extern int	EdText_SelectLine (TextPtr pmTextPtr);
#ifndef CONSOLE					  
    extern int	EdText_SelectLine1 (TextPtr pmTextPtr, int pmLine);
#endif
extern int	EdText_SelectWord (TextPtr pmTextPtr);

//
// Misc
// 
extern int	EdText_InsertText (TextPtr pmTextPtr, const char *pmBuffer, 
				   int pmBufferSize, BOOL pmClean, 
				   int *pmLinesInserted);
				   
#ifndef CONSOLE	
    extern int	EdText_SelectionComment (TextPtr pmTextPtr);
#endif // #ifndef CONSOLE

extern int	EdText_SelectionDelete (TextPtr pmTextPtr);

#ifndef CONSOLE	
    extern int	EdText_SelectionUncomment (TextPtr pmTextPtr);
    extern void	EdText_SetNewText (TextPtr pmTextPtr, const char *pmBuffer, 
				    int pmFileSize, BOOL pmEraseUndo);
#endif // #ifndef CONSOLE

extern BOOL	EdText_SetSelection (TextPtr pmTextPtr, int pmLine, int pmCol);

#ifndef CONSOLE	
    extern BOOL	EdText_SetSelection1 (TextPtr pmTextPtr, int pmSelFrom, 
				      int pmSelTo);
    extern void	EdText_ToggleBreakpoint (TextPtr pmTextPtr, int pmLine);

    //
    // Undo commands
    //
    extern void	EdText_Undo (TextPtr pmTextPtr, BOOL pmUndo);
//    extern void	EdText_UndoClear (TextPtr pmTextPtr);
    extern void	EdText_UndoFreeLastBuffer (TextPtr pmTextPtr);
    extern void	EdText_UndoSaveState (TextPtr pmTextPtr, int pmUndoMenuName);
    extern void	EdText_UndoSaveSpecified (TextPtr pmTextPtr, int pmUndoMenuName, 
					  int pmFromPos, int pmToPos);
    extern void	EdText_UndoSetMoreKeystrokes (TextPtr pmTextPtr, 
					      BOOL pmMoreKeystrokes);
    extern void	EdText_UndoSetReplFrom (TextPtr pmTextPtr, int pmReplFrom);
    extern void	EdText_UndoSetReplTo (TextPtr pmTextPtr, int pmReplTo);
#endif // #ifndef CONSOLE

#endif // #ifndef _EDTEXT_H_
