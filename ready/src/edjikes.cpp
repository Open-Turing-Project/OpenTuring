/*************/
/* edjikes.c */
/*************/

/*******************/
/* System includes */
/*******************/
#include <windows.h>

/****************/
/* Self include */
/****************/
#include "edjikes.h"

/******************/
/* Other includes */
/******************/

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
typedef struct
{
    char *jikesMsg;
    char *readyMsg;
} ErrorMsg;

typedef struct
{
    char *className;
    char *packageName;
} ClassPackage;

typedef struct
{
    char *badClassName;
    char *goodClassName;
} ClassNameCorrection;

/**********************/
/* External variables */
/**********************/
extern "C" 
{
	int	EdRun_GetFileStat (const char *name, struct stat *stat_struct);
	int 	EdRun_GetOpenFile (const char *pmPathName, const char **pmBuffer, unsigned int *pmSize);
	void	EdRun_ProcessJikesErrorMsg (const char *pmPathName, 
			    int pmLeftLineNo, int pmLeftCol, int pmRightLineNo,
			    int pmRightCol, int pmSeverity, 
			    const char *pmMessage);
	void 	EdRun_StatusReading (const char *pmPathName);
	void 	EdRun_StatusFinishedReading (void);
	void	EdJikes_Compile (int pmNumArgs, char *pmArguments[]);
}

/********************/
/* Static constants */
/********************/
static ErrorMsg	stErrorMsg [] = {
    {"\";\" inserted to complete ExpressionStatement",
     "Missing \";\" at end of statement"},
    {"The use of \"assert\" as an identifier is deprecated, as it is now a "
     "keyword. Use -source 1.4 if you intended  to make use of assertions.",
     "Assertions are not enabled. To use the \"assert\" statement, go to "
     "Preferences -> Java and set the control to allow \"assert\" statements"},
    {"assert expected instead of this token",
     "\"assert\" statements are currently disabled"},
    {"String constant not properly terminated.",
     "Missing double quote at end of String"},
    {"Character constant not properly terminated.",
     "Missing single quote after character"},
    {"Character constant must be only one character.",
     "Character constant must be only one character. (Note: Strings use double quotes)"},
    };
    
static int stNumErrorMsgs = 
    sizeof (stErrorMsg) / sizeof (ErrorMsg);

static ClassPackage stClassPackage [] = {
    // hsa classes
    {"Console", "hsa.Console"},
    {"Stdin", "hsa.Stdin"},
    {"Stdout", "hsa.Stdout"},
    {"PaintBug", "hsa.PaintBug"},
    {"Animator", "hsa.Animator"},
    {"Moveable", "hsa.Moveable"},
    {"Bounceable", "hsa.Bounceable"},
    {"Paintable", "hsa.Paintable"},
    {"VisibleHanoi", "hsa.VisibleHanoi"},
    {"VisibleSortUtil", "hsa.VisibleSortUtil"},
    {"TreeUtil", "hsa.TreeUtil"},
    // java class library
    {"ActionEvent", "java.awt.event.ActionEvent"},
    {"ActionListener", "java.awt.event.ActionListener"},
    {"AdjustmentEvent", "java.awt.event.AdjustmentEvent"},
    {"AdjustmentListener", "java.awt.event.AdjustmentListener"},
    {"Applet", "java.applet.Applet"},
    {"ArrayList", "java.util.ArrayList"},
    {"BorderLayout", "java.awt.BorderLayout"},
    {"BufferedReader", "java.io.BufferedReader"},
    {"Button", "java.awt.Button"},
    {"Calendar", "java.util.Calendar"},
    {"Canvas", "java.awt.Canvas"},
    {"Checkbox", "java.awt.Checkbox"},
    {"CheckboxGroup", "java.awt.CheckboxGroup"},
    {"CheckboxMenuItem", "java.awt.CheckboxMenuItem"},
    {"Color", "java.awt.Color"},
    {"Component", "java.awt.Component"},
    {"DataInputStream", "java.io.DataInputStream"},
    {"DataOutputStream", "java.io.DataOutputStream"},
    {"Date", "java.util.Date"},
    {"DateFormat", "java.text.DateFormat"},
    {"DecimalFormat", "java.text.DecimalFormat"},
    {"Dialog", "java.awt.Dialog"},
    {"Dimension", "java.awt.Dimension"},
    {"Event", "java.awt.Event"},
    {"FileDialog", "java.io.FileDialog"},
    {"FileInputStream", "java.io.FileInputStream"},
    {"FileOutputStream", "java.io.FileOutputStream"},
    {"FileReader", "java.io.FileReader"},
    {"FileWriter", "java.io.FileWriter"},
    {"FlowLayout", "java.awt.FlowLayout"},
    {"Font", "java.awt.Font"},
    {"FontMetrics", "java.awt.FontMetrics"},
    {"Frame", "java.awt.Frame"},
    {"Graphics", "java.awt.Graphics"},
    {"GridLayout", "java.awt.GridLayout"},
    {"HashMap", "java.util.HashMap"},
    {"HashSet", "java.util.HashSet"},
    {"Hashtable", "java.util.Hashtable"},
    {"Image", "java.awt.Image"},
    {"ImageObserver", "java.awt.ImageObserver"},
    {"InputStreamReader", "java.io.InputStreamReader"},
    {"IOException", "java.io.IOException"},
    {"Iterator", "java.util.Iterator"},
    {"KeyEvent", "java.awt.event.KeyEvent"},
    {"KeyListener", "java.awt.event.KeyListener"},
    {"Label", "java.awt.Label"},
    {"LinkedList", "java.util.LinkedList"},
    {"List", "java.awt.List or java.util.List"},
    {"ListIterator", "java.util.ListIterator"},
    {"Locale", "java.util.Locale"},
    {"Map", "java.util.Map"},
    {"MediaTracker", "java.awt.MediaTracker"},
    {"Menu", "java.awt.Menu"},
    {"MenuBar", "java.awt.MenuBar"},
    {"MenuItem", "java.awt.MenuItem"},
    {"MouseEvent", "java.awt.event.MouseEvent"},
    {"MouseListener", "java.awt.event.MouseListener"},
    {"MouseMotionListener", "java.awt.event.MouseMotionListener"},
    {"NumberFormat", "java.text.NumberFormat"},
    {"Panel", "java.awt.Panel"},
    {"Point", "java.awt.Point"},
    {"PrintWriter", "java.io.PrintWriter"},
    {"Random", "java.util.Random"},
    {"Scrollbar", "java.awt.Scrollbar"},
    {"Set", "java.util.Set"},
    {"Stack", "java.util.Stack"},
    {"StringTokenizer", "java.util.StringTokenizer"},
    {"TextArea", "java.awt.TextArea"},
    {"TextField", "java.awt.TextField"},
    {"Toolkit", "java.awt.Toolkit"},
    {"TreeMap", "java.util.TreeMap"},
    {"TreeSet", "java.util.TreeSet"},
    {"Vector", "java.util.Vector"},
    {"WindowEvent", "java.awt.event.WindowEvent"},
    {"WindowListener", "java.awt.event.WindowListener"}};
    
static int stNumClassPackages = 
    sizeof (stClassPackage) / sizeof (ClassPackage);
    
static ClassNameCorrection stClassNameCorrection [] = {
    {"CheckBox", "Checkbox (lower case 'b')"},
    {"CheckBoxGroup", "CheckboxGroup (lower case 'b')"},
    {"CheckBoxMenuItem", "CheckboxMenuItem (lower case 'b')"},
    {"Colour", "Color (no 'u')"},
    {"HashTable", "Hashtable (lower case 't')"},
    {"ScrollBar", "Scrollbar (lower case 'b')"},
    {"ToolKit", "Toolkit (lower case 'k')"}};

static int stNumClassNameCorrections = 
    sizeof (stClassNameCorrection) / sizeof (ClassNameCorrection);
    
static ClassNameCorrection stImportCorrection [] = {
    {"java.awt.CheckBox", "java.awt.Checkbox (lower case 'b')"},
    {"java.awt.CheckBoxGroup", "java.awt.CheckboxGroup (lower case 'b')"},
    {"java.awt.CheckBoxMenuItem", "java.awt.CheckboxMenuItem (lower case 'b')"},
    {"java.awt.Colour", "java.awt.Color (no 'u')"},
    {"java.awt.HashTable", "java.awt.Hashtable (lower case 't')"},
    {"java.awt.ScrollBar", "java.awt.Scrollbar (lower case 'b')"},
    {"java.awt.ToolKit", "java.awt.Toolkit (lower case 'k')"}};

static int stNumImportCorrections = 
    sizeof (stImportCorrection) / sizeof (ClassNameCorrection);

/******************************/
/* Static callback procedures */
/******************************/

/*********************/
/* Static procedures */
/*********************/

/***********************/
/* External procedures */
/***********************/
void	EdJikes_Compile (int pmNumArgs, char *pmArguments[])
{
    char 	**myFiles;
    JikesAPI 	*myCompiler = new ReadyJikesAPI ();

    myFiles = myCompiler -> parseOptions (pmNumArgs, pmArguments);
    
    myCompiler -> compile (myFiles);

    delete myCompiler;
}


ReadyJikesAPI::ReadyJikesAPI () : JikesAPI ()
{
} // ReadyJikesAPI Constructor


int ReadyJikesAPI::stat(const char *fileName, struct stat *status)
{
    return (EdRun_GetFileStat (fileName, status));
} // ReadyJikesAPI::stat


void ReadyJikesAPI::reportError (JikesError *error)
{
    int			myIsWarning = FALSE;
    const wchar_t	*myWideErrorMsg;
    int			myErrorMsgSize;
    char		myErrorMsg [4000], *myPtr;
    int			cnt;
    BOOL		myFound = FALSE;
    
    if (error -> getSeverity () == JikesError::JIKES_WARNING)
    {
    	myIsWarning = TRUE;
    }
    
    // This line is necessary because Lexical errors will not have 
    // line. column positions placed until this call.
    error -> getErrorReport();
    myWideErrorMsg = error -> getErrorMessage ();
    myErrorMsgSize = wcslen (myWideErrorMsg);
    wcstombs (myErrorMsg, myWideErrorMsg, myErrorMsgSize + 1);

    // Translate some error messages
    for (cnt = 0 ; cnt < stNumErrorMsgs ; cnt++)
    {
    	if (strcmp (myErrorMsg, stErrorMsg [cnt].jikesMsg) == 0)
    	{
    	    strcpy (myErrorMsg, stErrorMsg [cnt].readyMsg);
    	    break;
    	}
    }
    
    // Special translations - Type <classname> was not found
    if (strncmp (myErrorMsg, "Type ", 5) == 0)
    {
    	myPtr = &myErrorMsg [5];
    	while ((*myPtr != ' ') && (*myPtr != 0))
    	{
    	    myPtr++;
    	}
    	if (strcmp (myPtr, " was not found.") == 0)
    	{
    	    char	myClassName [4000];
    	    int		myLen = myPtr - myErrorMsg - 5;
    	    
    	    strncpy (myClassName, &myErrorMsg [5], myLen);
    	    myClassName [myLen] = 0;
    	    for (cnt = 0 ; cnt < stNumClassPackages ; cnt++)
    	    {
    	    	if (strcmp (myClassName, stClassPackage [cnt].className) == 0)
    	    	{
		    wsprintf (myErrorMsg, "%s class not found. Did you forget"
		        " to import %s using\r\n        import %s", myClassName,
		        myClassName, stClassPackage [cnt].packageName);
		    myFound = TRUE;
		    break;
		}
	    } // for
	    
	    if (!myFound)
	    {
    	        for (cnt = 0 ; cnt < stNumClassNameCorrections ; cnt++)
    	    	{
    	    	    if (strcmp (myClassName, 
    	    			stClassNameCorrection [cnt].badClassName) == 0)
    	    	    {
		        wsprintf (myErrorMsg, "%s class not found. Did you "
		            "mean %s?", myClassName, 
		            stClassNameCorrection [cnt].goodClassName);
		        myFound = TRUE;
		        break;
		    } // if
	    	} // for
	    } // if
	} // if
    } // if
    
    // Special translations - No field named "<classname>" was found in type "x"
    if (strncmp (myErrorMsg, "No field named \"", 16) == 0)
    {
    	
    	myPtr = &myErrorMsg [16];
    	while ((*myPtr != '\"') && (*myPtr != 0))
    	{
    	    myPtr++;
    	}
    	if (strncmp (myPtr, "\" was found in type \"", 21) == 0)
    	{
    	    char	myClassName [4000];
    	    int		myLen = myPtr - myErrorMsg - 16;
    	    
    	    strncpy (myClassName, &myErrorMsg [16], myLen);
    	    myClassName [myLen] = 0;
    	    for (cnt = 0 ; cnt < stNumClassPackages ; cnt++)
    	    {
    	    	if (strcmp (myClassName, stClassPackage [cnt].className) == 0)
    	    	{
		    wsprintf (myErrorMsg, "%s class not found. Did you forget"
		        " to import %s using:\r\n        import %s", 
			myClassName, myClassName, 
			stClassPackage [cnt].packageName);
		    myFound = TRUE;
		    break;
		}
	    } // for
	    
	    if (!myFound)
	    {
    	        for (cnt = 0 ; cnt < stNumClassNameCorrections ; cnt++)
    	    	{
    	    	    if (strcmp (myClassName, 
    	    			stClassNameCorrection [cnt].badClassName) == 0)
    	    	    {
		        wsprintf (myErrorMsg, "%s class not found. Did you "
		            "mean %s?", myClassName, 
		            stClassNameCorrection [cnt].goodClassName);
		        myFound = TRUE;
		        break;
		    } // if
	    	} // for
	    } // if
	} // if
    } // if
    
    // Special translations - The import "<packagename>" is not valid, since it
    //                        does not name a type in a package.
    if (strncmp (myErrorMsg, "The import \"", 12) == 0)
    {
    	BOOL	myFound = FALSE;
    	
    	myPtr = &myErrorMsg [12];
    	while ((*myPtr != '"') && (*myPtr != 0))
    	{
    	    myPtr++;
    	}
    	if (strcmp (myPtr, "\" is not valid, since it does not name a type in a package.") == 0)
    	{
    	    char	myClassName [4000];
    	    int		myLen = myPtr - myErrorMsg - 12;
    	    
    	    // Convert the / to .
    	    myPtr = &myErrorMsg [12];
    	    while ((*myPtr != '"') && (*myPtr != 0))
    	    {
    	    	if (*myPtr == '/')
    	    	   *myPtr = '.';
    	        myPtr++;
    	    }
    	    
    	    strncpy (myClassName, &myErrorMsg [12], myLen);
    	    myClassName [myLen] = 0;
    	    for (cnt = 0 ; cnt < stNumImportCorrections ; cnt++)
    	    {
    	    	if (strcmp (myClassName, 
    	    		    stImportCorrection [cnt].badClassName) == 0)
    	    	{
		    wsprintf (myErrorMsg, "%s class not found. Did you "
		            "mean %s?", myClassName, 
		            stImportCorrection [cnt].goodClassName);
		    myFound = TRUE;
		    break;
		}
	    } // for
	} // if
    } // if    	    		

    // Special translation - The type of the right sub-expression, "x", is
    //			     not assignable to the variable, of type "y"
    if (strncmp (myErrorMsg, "The type of the right sub-expression, \"", 39) == 0)
    {
	char	*myStart;

    	myPtr = &myErrorMsg [39];
	myStart = myPtr;
    	while ((*myPtr != '\"') && (*myPtr != 0))
    	{
    	    myPtr++;
    	}
    	if (strncmp (myPtr, "\", is not assignable to the variable, of type \"", 22) == 0)
    	{
    	    char	myExprType [4000], myVariableType [4000];
    	    int		myLen = myPtr - myStart;
    	    
    	    strncpy (myExprType, myStart, myLen);
    	    myExprType [myLen] = 0;
    	    myPtr += 47;
    	    myStart = myPtr;
    	    while ((*myPtr != '\"') && (*myPtr != 0))
    	    {
    	    	myPtr++;
    	    }
    	    
    	    myLen = myPtr - myStart;
    	    strncpy (myVariableType, myStart, myLen);
    	    myVariableType [myLen] = 0;

    	    wsprintf (myErrorMsg, 
    	    	      "You cannot assign a%s \"%s\" expression to a%s \"%s\" variable", 
    	    	      (strcmp (myExprType, "int") == 0) ? "n" : "",
    	    	      myExprType, 
    	    	      (strcmp (myVariableType, "int") == 0) ? "n" : "",
    	    	      myVariableType);
	} // if
    } // if    
       	
    // Process the error message
    EdRun_ProcessJikesErrorMsg (error -> getFileName (), 
    	error -> getLeftLineNo (), error -> getLeftColumnNo (),
    	error -> getRightLineNo (), error -> getRightColumnNo (),
    	myIsWarning, myErrorMsg);
} // ReadyJikesAPI::reportError


/**
 * By default return an object that reads from the file system.
 */
class ReadyFileReader: public JikesAPI::FileReader
{
public:
    ReadyFileReader (const char *fileName);
    virtual  ~ReadyFileReader();
    
    virtual const char* getBuffer() { return buffer; }
    virtual size_t getBufferSize() { return size; }

private:
    const char* buffer;
    size_t size;
    HANDLE srcfile;
    HANDLE mapfile;
};


ReadyJikesAPI::FileReader *ReadyJikesAPI::read(const char *fileName)
{
    FileReader  *result  =  new ReadyFileReader (fileName);

    // NB even if a file is empty (0 bytes)
    // This will return a pointer to 0 length array
    // and should not be NULL.
    if (result && (result->getBuffer() == NULL))  
    {
        delete result;
        result  = NULL;
    }
    return result;
}


// Open a windows file and map the file onto processor memory.
ReadyFileReader::ReadyFileReader (const char *fileName)
{
    size = 0;
    buffer = NULL;
    srcfile = INVALID_HANDLE_VALUE;
    mapfile = INVALID_HANDLE_VALUE;
    
    EdRun_StatusReading (fileName);
    if (EdRun_GetOpenFile (fileName, &buffer, &size))
    {
    	return;
    }

    srcfile = CreateFile (fileName, GENERIC_READ, FILE_SHARE_READ, NULL,
                          OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
    if (srcfile != INVALID_HANDLE_VALUE)
    {
        mapfile = CreateFileMapping (srcfile, NULL, PAGE_READONLY, 0, 0, NULL);
        if (mapfile != INVALID_HANDLE_VALUE)
        {
            buffer = (char *) MapViewOfFile (mapfile, FILE_MAP_READ, 0, 0, 0);
            size = (size_t) GetFileSize (srcfile, NULL);
        }
    }
} // ReadyFileReader constructor


// When the ReadObject is destroyed close all the associated files.
// and unmap the memory.
ReadyFileReader::~ReadyFileReader ()
{
    if (srcfile != INVALID_HANDLE_VALUE)
    {
        if (mapfile != INVALID_HANDLE_VALUE)
        {
            if (buffer)
            {
                UnmapViewOfFile((void *) buffer);
            }
            CloseHandle(mapfile);
        }
        CloseHandle(srcfile);
    }
    
    EdRun_StatusFinishedReading ();
} // ReadyFileReader destructor