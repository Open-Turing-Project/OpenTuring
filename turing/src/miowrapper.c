/************************************************************************/
/* miowrapper.c								*/
/*									*/
/* Each routine defined in the predefined modules has an entry in one	*/
/* of the lookup?.c files. Entries in these files consist of two	*/
/* parts:								*/
/*									*/
/* { "internal_routine_name",   pointer_to_wrapper_routine }		*/
/*									*/
/* The following routines are wrappers. Wrappers are interfaces between	*/
/* the OOT executor, and an implementation of a predefined (external)	*/
/* routine.  Wrappers pull pointers and values off of the OOT executor 	*/
/* stack, and pass them on to the implementation routines. In the case	*/
/* of Turing functions, the wrapper stores the return value at the 	*/
/* location pointed to by the top element on the executor stack (this	*/
/* is the OOT executor's scalar result convention), conceptually all	*/
/* functions are procedures with an initial var parameter used to 	*/
/* return the result of the function.					*/
/************************************************************************/

/*******************/
/* System includes */
/*******************/
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

/****************/
/* Self include */
/****************/

/******************/
/* Other includes */
/******************/
#include "language.h"
#include "levent.h"

#include "mioerr.h"

// The actual MIO routines
#include "mioconfig.h"
#include "miocrypto.h"
#include "miodir.h"
#include "miodlg.h"
#include "miodraw.h"
#include "mioerror.h"
#include "miofile.h"
#include "miofont.h"
#include "miogui.h"
#include "miojoy.h"
#include "miolexer.h"
#include "miomouse.h"
#include "miomusic.h"
#include "mionet.h"
#include "miopic.h"
#include "miopc.h"
#include "miorgb.h"
#include "miosprite.h"
#include "miosys.h"
#include "miotext.h"
#include "miotime.h"
#include "mioview.h"
#include "miowindow.h"

/**********/
/* Macros */
/**********/
#define RESULT_OOT_BOOL(sp, val)  (*((OOTboolean *) *sp) = val)
#define RESULT_OOT_INT(sp, val)  (*((OOTint *) *sp) = val)
#define RESULT_OOT_ADDR(sp, val)  (*((OOTaddr *) *sp) = (OOTaddr) val)

/*************/
/* Constants */
/*************/
#define INT_SIZE	1
#define ADDR_SIZE	1
#define REAL4_SIZE	1
#define REAL8_SIZE	2

// The following are to be transferred to other .h's
#define DEFAULT_WIN	 0

#define TURINGSTDIN	-2

/********************/
/* Global variables */
/********************/

/*********/
/* Types */
/*********/

/**********************/
/* External variables */
/**********************/

/****************************/
/* External procedures used */
/****************************/
extern void	Language_Execute_EventQueueInsert (int pmStream, 
					struct EventDescriptor *pmEvent);

/********************/
/* Static constants */
/********************/

/********************/
/* Static variables */
/********************/

/******************************/
/* Static callback procedures */
/******************************/

/*********************/
/* Static procedures */
/*********************/
static void	MyExecutorScan (OOTaddr *sp, const char *param, ...);


/***********************/
/* External procedures */
/***********************/
/************************************************************************/
/* Config module							*/
/************************************************************************/
void MIO_config_display (OOTaddr *sp)
{
    OOTint code;

    MyExecutorScan (sp, "rI", &code);
    RESULT_OOT_INT(sp, MIOConfig_Display (code));
} // MIO_config_display

void MIO_config_lang (OOTaddr *sp)
{
    OOTint code;

    MyExecutorScan (sp, "rI", &code);
    RESULT_OOT_INT(sp, MIOConfig_Lang (code));
} // MIO_config_lang

void MIO_config_machine (OOTaddr *sp)
{
    OOTint code;

    MyExecutorScan (sp, "rI", &code);
    RESULT_OOT_INT(sp, MIOConfig_Machine (code));
} // MIO_config_machine


/************************************************************************/
/* Crypto module							*/
/************************************************************************/
void MIO_crypto_des3decrypt (OOTaddr *sp)
{
    char	*charArray;
    int		charArraySize;
    int		numChars;

    MyExecutorScan (sp, "RII", &charArray, &charArraySize, &numChars);

    if (numChars > charArraySize)
    {
	// numChars larger then array size
	SET_ERRNO (E_CRYPTO_NUM_CHARS_TOO_LARGE);
	return;
    }
    if (numChars % 8 != 0)
    {
	// Can only encrupt/decrypt 8 byte chunks
	SET_ERRNO (E_CRYPTO_NUM_CHARS_TOO_LARGE);
	return;
    }

    MIOCrypto_DES3Decrypt (charArray, charArraySize, numChars);
} // MIO_crypto_des3decrypt

void MIO_crypto_des3encrypt (OOTaddr *sp)
{
    char	*charArray;
    int		charArraySize;
    int		numChars;

    MyExecutorScan (sp, "RII", &charArray, &charArraySize, &numChars);

    if (numChars > charArraySize)
    {
	// numChars larger then array size
	SET_ERRNO (E_CRYPTO_NUM_CHARS_TOO_LARGE);
	return;
    }
    if (numChars % 8 != 0)
    {
	// Can only encrupt/decrypt 8 byte chunks
	SET_ERRNO (E_CRYPTO_NUM_CHARS_TOO_LARGE);
	return;
    }

    MIOCrypto_DES3Encrypt (charArray, charArraySize, numChars);
} // MIO_crypto_des3encrypt

void MIO_crypto_des3set2key (OOTaddr *sp)
{
    OOTnat key1A, key1B, key2A, key2B;

    MyExecutorScan (sp, "IIII", &key1A, &key1B, &key2A, &key2B);

    MIOCrypto_DES3Set2Key (key1A, key1B, key2A, key2B);
} // MIO_crypto_des3set2key

void MIO_crypto_des3set3key (OOTaddr *sp)
{
    OOTnat key1A, key1B, key2A, key2B, key3A, key3B;

    MyExecutorScan (sp, "IIIIII", &key1A, &key1B, &key2A, &key2B, &key3A, &key3B);

    MIOCrypto_DES3Set3Key (key1A, key1B, key2A, key2B, key3A, key3B);
} // MIO_crypto_des3set3key

void MIO_crypto_desdecrypt (OOTaddr *sp)
{
    char	*charArray;
    int		charArraySize;
    int		numChars;

    MyExecutorScan (sp, "RII", &charArray, &charArraySize, &numChars);

    if (numChars > charArraySize)
    {
	// numChars larger then array size
	SET_ERRNO (E_CRYPTO_NUM_CHARS_TOO_LARGE);
	return;
    }
    if (numChars % 8 != 0)
    {
	// Can only encrupt/decrypt 8 byte chunks
	SET_ERRNO (E_CRYPTO_NUM_CHARS_TOO_LARGE);
	return;
    }

    MIOCrypto_DESDecrypt (charArray, charArraySize, numChars);
} // MIO_crypto_desdecrypt

void MIO_crypto_desencrypt (OOTaddr *sp)
{
    char	*charArray;
    int		charArraySize;
    int		numChars;

    MyExecutorScan (sp, "RII", &charArray, &charArraySize, &numChars);

    if (numChars > charArraySize)
    {
	// numChars larger then array size
	SET_ERRNO (E_CRYPTO_NUM_CHARS_TOO_LARGE);
	return;
    }
    if (numChars % 8 != 0)
    {
	// Can only encrupt/decrypt 8 byte chunks
	SET_ERRNO (E_CRYPTO_NUM_CHARS_TOO_LARGE);
	return;
    }

    MIOCrypto_DESEncrypt (charArray, charArraySize, numChars);
} // MIO_crypto_desencrypt

void MIO_crypto_dessetkey (OOTaddr *sp)
{
    OOTnat keyA, keyB;

    MyExecutorScan (sp, "II", &keyA, &keyB);

    MIOCrypto_DESSetKey (keyA, keyB);
} // MIO_crypto_dessetkey


/************************************************************************/
/* Dir module								*/
/************************************************************************/
void MIO_dir_open (OOTaddr *sp)
{
    OOTstring   dirPath;
    SRCPOS	srcPos;

    MyExecutorScan (sp, "rRp", &dirPath);
    
    Language_Execute_RunSrcPosition (&srcPos);

    RESULT_OOT_INT(sp, MIODir_Open (dirPath, &srcPos));
} // MIO_dir_open

void MIO_dir_close (OOTaddr *sp)
{
    OOTint dirId;

    MyExecutorScan (sp, "I", &dirId);
    
    MIODir_Close (dirId);
} // MIO_dir_close

void MIO_dir_get (OOTaddr *sp)
{
    OOTstring  fileName;
    OOTint     dirId;

    MyExecutorScan (sp, "RI", &fileName, &dirId);
    
    MIODir_Get (dirId, fileName);
} // MIO_dir_get

void MIO_dir_getlong (OOTaddr *sp)
{
    OOTint     dirId;
    OOTstring  fileName;
    OOTint    *size, *attributes, *fileTime;

    MyExecutorScan (sp, "IRRRR", &dirId, &fileName, &size, &attributes, &fileTime);
    
    MIODir_GetLong (dirId, fileName, size, attributes, fileTime);
} // MIO_dir_getlong

void MIO_dir_create (OOTaddr *sp)
{
    OOTstring dirPath;

    MyExecutorScan (sp, "R", &dirPath);
    
    MIODir_Create (dirPath);
} // MIO_dir_create

void MIO_dir_delete (OOTaddr *sp)
{
    OOTstring dirPath;

    MyExecutorScan (sp, "R", &dirPath);
    
    MIODir_Delete (dirPath);
} // MIO_dir_delete

void MIO_dir_change (OOTaddr *sp)
{
    OOTstring dirPath;

    MyExecutorScan (sp, "R", &dirPath);
    
    MIODir_Change (dirPath);
} // MIO_dir_change

void MIO_dir_current (OOTaddr *sp)
{
    OOTstring dirPath;

    MyExecutorScan (sp, "R", &dirPath);
    
    MIODir_Current (dirPath);
} // MIO_dir_current

void MIO_dir_exists (OOTaddr *sp)
{
    OOTstring dirPath;

    MyExecutorScan (sp, "rR", &dirPath);

    RESULT_OOT_INT(sp, (OOTint) MIODir_Exists (dirPath));
} // MIO_dir_exists


/************************************************************************/
/* Dialog module							*/
/************************************************************************/
void MIO_dlg_alert (OOTaddr *sp)
{
    OOTstring title, msg;

    MyExecutorScan (sp, "RR", &title, &msg);

    MIODlg_Alert (title, msg);
} // MIO_dlg_alert

/************************************************************************/
/* Draw module								*/
/************************************************************************/
void MIO_draw_arc (OOTaddr *sp)
{
    OOTint x, y, xr, yr, iAngle, fAngle, clrNum;

    MyExecutorScan (sp, "IIIIIII", &x, &y, &xr, &yr, &iAngle, &fAngle, &clrNum);

    MIODraw_Arc (x, y, xr, yr, iAngle, fAngle, NO_STYLE, NORMAL_WIDTH, 
    		 clrNum, NOFILL);
} // MIO_draw_arc

void MIO_draw_fillarc (OOTaddr *sp)
{
    OOTint x, y, xr, yr, iAngle, fAngle, clrNum;

    MyExecutorScan (sp, "IIIIIII", &x, &y, &xr, &yr, &iAngle, &fAngle, &clrNum);

    MIODraw_Arc (x, y, xr, yr, iAngle, fAngle, NO_STYLE, NORMAL_WIDTH, 
    		 clrNum, FILL);
} // MIO_draw_fillarc

void MIO_draw_box (OOTaddr *sp)
{
    OOTint x1, y1, x2, y2, clrNum;

    MyExecutorScan (sp, "IIIII", &x1, &y1, &x2, &y2, &clrNum);

    MIODraw_Box (x1, y1, x2, y2, NO_STYLE, NORMAL_WIDTH, clrNum, NOFILL);
} // MIO_draw_box

void MIO_draw_fillbox (OOTaddr *sp)
{
    OOTint x1, y1, x2, y2, clrNum;

    MyExecutorScan (sp, "IIIII", &x1, &y1, &x2, &y2, &clrNum);

    MIODraw_Box (x1, y1, x2, y2, NO_STYLE, NORMAL_WIDTH, clrNum, FILL);
} // MIO_draw_fillbox

void MIO_draw_oval (OOTaddr *sp)
{
    OOTint x, y, xr, yr, clrNum;

    MyExecutorScan (sp, "IIIII", &x, &y, &xr, &yr, &clrNum);

    MIODraw_Oval (x, y, xr, yr, NO_STYLE, NORMAL_WIDTH, clrNum, NOFILL);
} // MIO_draw_oval

void MIO_draw_filloval (OOTaddr *sp)
{
    OOTint x, y, xr, yr, clrNum;

    MyExecutorScan (sp, "IIIII", &x, &y, &xr, &yr, &clrNum);

    MIODraw_Oval (x, y, xr, yr, NO_STYLE, NORMAL_WIDTH, clrNum, FILL);
} // MIO_draw_filloval

void MIO_draw_polygon (OOTaddr *sp)
{
    OOTint *x, *y, n, clrNum;

    MyExecutorScan (sp, "AAII", &x, &y, &n, &clrNum);

    MIODraw_Polygon (x, y, n, NO_STYLE, NORMAL_WIDTH, clrNum, NOFILL);
} // MIO_draw_polygon

void MIO_draw_fillpolygon (OOTaddr *sp)
{
    OOTint *x, *y, n, clrNum;

    MyExecutorScan (sp, "AAII", &x, &y, &n, &clrNum);

    MIODraw_Polygon (x, y, n, NO_STYLE, NORMAL_WIDTH, clrNum, FILL);
} // MIO_draw_fillpolygon

void MIO_draw_star (OOTaddr *sp)
{
    OOTint x1, y1, x2, y2, clrNum;

    MyExecutorScan (sp, "IIIII", &x1, &y1, &x2, &y2, &clrNum);

    MIODraw_Star (x1, y1, x2, y2, NO_STYLE, NORMAL_WIDTH, clrNum, NOFILL);
} // MIO_draw_star

void MIO_draw_fillstar (OOTaddr *sp)
{
    OOTint x1, y1, x2, y2, clrNum;

    MyExecutorScan (sp, "IIIII", &x1, &y1, &x2, &y2, &clrNum);

    MIODraw_Star (x1, y1, x2, y2, NO_STYLE, NORMAL_WIDTH, clrNum, FILL);
} // MIO_draw_fillstar

void MIO_draw_mapleleaf (OOTaddr *sp)
{
    OOTint x1, y1, x2, y2, clrNum;

    MyExecutorScan (sp, "IIIII", &x1, &y1, &x2, &y2, &clrNum);

    MIODraw_MapleLeaf (x1, y1, x2, y2, NO_STYLE, NORMAL_WIDTH, clrNum, NOFILL);
} // MIO_draw_mapleleaf

void MIO_draw_fillmapleleaf (OOTaddr *sp)
{
    OOTint x1, y1, x2, y2, clrNum;

    MyExecutorScan (sp, "IIIII", &x1, &y1, &x2, &y2, &clrNum);

    MIODraw_MapleLeaf (x1, y1, x2, y2, NO_STYLE, NORMAL_WIDTH, clrNum, FILL);
} // MIO_draw_fillmapleleaf

void MIO_draw_dot (OOTaddr *sp)
{
    OOTint x, y, clrNum;

    MyExecutorScan (sp, "III", &x, &y, &clrNum);

    MIODraw_Dot (x, y, clrNum);
} // MIO_draw_dot

void MIO_draw_fill (OOTaddr *sp)
{
    OOTint x, y, fillClrNum, borderClrNum;

    MyExecutorScan (sp, "IIII", &x, &y, &fillClrNum, &borderClrNum);

    MIODraw_Fill (x, y, fillClrNum, borderClrNum);
} // MIO_draw_fill

void MIO_draw_line (OOTaddr *sp)
{
    OOTint x1, y1, x2, y2, clrNum;

    MyExecutorScan (sp, "IIIII", &x1, &y1, &x2, &y2, &clrNum);

    MIODraw_Line (x1, y1, x2, y2, NO_STYLE, NORMAL_WIDTH, clrNum);
} // MIO_draw_line

void MIO_draw_line_dashed (OOTaddr *sp)
{
    OOTint x1, y1, x2, y2, style, clrNum;

    MyExecutorScan (sp, "IIIIII", &x1, &y1, &x2, &y2, &style, &clrNum);

    MIODraw_Line (x1, y1, x2, y2, style, NORMAL_WIDTH, clrNum);
} // MIO_draw_line_ex

void MIO_draw_line_thick (OOTaddr *sp)
{
    OOTint x1, y1, x2, y2, width, clrNum;

    MyExecutorScan (sp, "IIIIII", &x1, &y1, &x2, &y2, &width, &clrNum);

    MIODraw_Line (x1, y1, x2, y2, NO_STYLE, width, clrNum);
} // MIO_draw_line_ex

void MIO_draw_cls (OOTaddr *sp)
{
    MIODraw_Cls ();
} // MIO_draw_cls

void MIO_draw_text (OOTaddr *sp)
{
    OOTstring str;
    OOTint x, y, fontId, colourNum;

    MyExecutorScan (sp, "RIIII", &str, &x, &y, &fontId, &colourNum);
    MIODraw_Text (str, x, y, fontId, colourNum);
} // MIO_font_draw


/************************************************************************/
/* Error module								*/
/************************************************************************/
void MIO_error_halt (OOTaddr *sp)
{
    OOTstring  str;

    MyExecutorScan (sp, "R", &str);
    
    MIOError_Halt (str);
} // MIO_error_halt

void MIO_error_last (OOTaddr *sp)
{
    RESULT_OOT_INT(sp, MIOError_Last());
} // MIO_error_last

void MIO_error_laststr (OOTaddr *sp)
{
    OOTstring  str;

    MyExecutorScan (sp, "R", &str);

    MIOError_LastStr (str);
} // MIO_error_laststr

void MIO_error_str (OOTaddr *sp)
{
    OOTstring  str;
    OOTint     errNo;

    MyExecutorScan (sp, "RI", &str, &errNo);
    
    MIOError_Str (str, errNo);
} // MIO_error_str

void MIO_error_lastmsg (OOTaddr *sp)
{
    OOTstring  str;

    MyExecutorScan (sp, "R", &str);

    MIOError_LastMsg (str);
} // MIO_error_lastmsg

void MIO_error_msg (OOTaddr *sp)
{
    OOTstring  str;
    OOTint     errNo;

    MyExecutorScan (sp, "RI", &str, &errNo);
    
    MIOError_Msg (str, errNo);
} // MIO_error_msg

void MIO_error_set (OOTaddr *sp)
{
    OOTint     errNo;

    MyExecutorScan (sp, "I", &errNo);
    
    MIOError_Set (errNo, NULL);
} // MIO_error_set

void MIO_error_setmsg (OOTaddr *sp)
{
    OOTint     errNo;
    OOTstring  str;

    MyExecutorScan (sp, "IR", &errNo, &str);
    
    MIOError_Set (errNo, str);
} // MIO_error_setmsg

void MIO_error_abort (OOTaddr *sp)
{
    OOTint     errNo;

    MyExecutorScan (sp, "I", &errNo);
    
    MIOError_Abort (errNo, NULL);
} // MIO_error_abort

void MIO_error_abortmsg (OOTaddr *sp)
{
    OOTint     errNo;
    OOTstring  str;

    MyExecutorScan (sp, "IR", &errNo, &str);
    
    MIOError_Abort (errNo, str);
} // MIO_error_abortmsg

/************************************************************************/
/* Event module								*/
/************************************************************************/
/*
void MIO_event_select (OOTaddr *sp)
{
    OOTstring eventStr;

    MyExecutorScan (sp, "R", &eventStr);

    EVENT_Set(DEFAULT_WIN, eventStr);
} // MIO_event_select
*/

/************************************************************************/
/* File module								*/
/************************************************************************/
void MIO_file_status (OOTaddr *sp)
{
    OOTstring path;
    OOTint *size, *attributes, *fileTime;

    MyExecutorScan (sp, "RRRR", &path, &size, &attributes, &fileTime);

    MIOFile_Status (path, size, attributes, fileTime);
} // MIO_file_status

void MIO_file_copy (OOTaddr *sp)
{
    OOTstring src, dest;

    MyExecutorScan (sp, "RR", &src, &dest);

    MIOFile_Copy(src, dest);
} // MIO_file_copy

void MIO_file_delete (OOTaddr *sp)
{
    OOTstring file;

    MyExecutorScan (sp, "R", &file);

    MIOFile_Delete(file);
} // MIO_file_delete

void MIO_file_diskfree (OOTaddr *sp)
{
    OOTstring path;

    MyExecutorScan (sp, "rR", &path);

    RESULT_OOT_INT(sp, MIOFile_DiskFree(path));
} // MIO_file_diskfree

void MIO_file_exists (OOTaddr *sp)
{
    OOTstring file;

    MyExecutorScan (sp, "rR", &file);

    RESULT_OOT_INT(sp, (OOTint) MIOFile_Exists(file));
} // MIO_file_exists

void MIO_file_fullpath (OOTaddr *sp)
{
    OOTstring newPath, oldPath;

    MyExecutorScan (sp, "RR", &newPath, &oldPath);

    MIOFile_FullPath (newPath, oldPath);
} // MIO_file_fullpath

void MIO_file_parent (OOTaddr *sp)
{
    OOTstring newPath, oldPath;

    MyExecutorScan (sp, "RR", &newPath, &oldPath);

    MIOFile_Parent (newPath, oldPath);
} // MIO_file_parent

void MIO_file_rename (OOTaddr *sp)
{
    OOTstring old, new;

    MyExecutorScan (sp, "RR", &old, &new);

    MIOFile_Rename(old, new);
} // MIO_file_rename

void MIO_file_createpath (OOTaddr *sp)
{
    OOTstring pathName, path, name;

    MyExecutorScan (sp, "RRR", &pathName, &path, &name);

    MIOFile_SplitPath(pathName, path, name);
} // MIO_file_createpath

void MIO_file_splitpath (OOTaddr *sp)
{
    OOTstring pathName, path, name;

    MyExecutorScan (sp, "RRR", &pathName, &path, &name);

    MIOFile_SplitPath(pathName, path, name);
} // MIO_file_splitpath


/************************************************************************/
/* Font module								*/
/************************************************************************/
void MIO_font_new (OOTaddr *sp)
{
    OOTstring	str;
    SRCPOS	srcPos;

    MyExecutorScan (sp, "rR", &str);

    Language_Execute_RunSrcPosition (&srcPos);

    RESULT_OOT_INT(sp, MIOFont_New (str, &srcPos));
} // MIO_font_new

void MIO_font_free (OOTaddr *sp)
{
    OOTint fontId;

    MyExecutorScan (sp, "I", &fontId);
    MIOFont_Free (fontId);
} // MIO_font_free

void MIO_font_width (OOTaddr *sp)
{
    OOTstring str;
    OOTint fontId;

    MyExecutorScan (sp, "rRI", &str, &fontId);
    RESULT_OOT_INT(sp, MIOFont_Width (fontId, str));
} // MIO_font_width

void MIO_font_sizes (OOTaddr *sp)
{
    OOTint fontId;
    OOTint *height, *ascent, *descent, *il;

    MyExecutorScan (sp, "IRRRR", &fontId, &height, &ascent, &descent, &il);
    MIOFont_Sizes (fontId, height, ascent, descent, il);
} // MIO_font_sizes

void MIO_font_name (OOTaddr *sp)
{
    OOTstring name;
    OOTint fontId;

    MyExecutorScan (sp, "RI", &name, &fontId);
    MIOFont_Name (fontId, name);
} // MIO_font_name

void MIO_font_startname (OOTaddr *sp)
{
    MIOFont_StartName ();
} // MIO_font_startname

void MIO_font_getname (OOTaddr *sp)
{
    OOTstring name;

    MyExecutorScan (sp, "R", &name);

    MIOFont_GetName (name);
} // MIO_font_getname

void MIO_font_getstyle (OOTaddr *sp)
{
    OOTstring name;
    OOTboolean *bold, *italic, *underline;
    
    MyExecutorScan (sp, "RRRR", &name, &bold, &italic, &underline);

    MIOFont_GetStyle (name, bold, italic, underline);
} // MIO_font_getstyle

void MIO_font_startsize (OOTaddr *sp)
{
    OOTstring name, style;

    MyExecutorScan (sp, "RR", &name, &style);

    MIOFont_StartSize (name, style);
} // MIO_font_startsize

void MIO_font_getsize (OOTaddr *sp)
{
    RESULT_OOT_INT(sp, MIOFont_GetSize ());
} // MIO_font_getsize


/************************************************************************/
/* GUI module								*/
/************************************************************************/
void MIO_gui_alert (OOTaddr *sp)
{
    OOTstring title, button;
    void *stringArray, *arrayDescriptor;

    MyExecutorScan (sp, "RRRR", &title, &stringArray, &arrayDescriptor,
		    &button);
    MIOGUI_Alert (title, stringArray, arrayDescriptor, button);
} // MIO_gui_alert

void MIO_gui_choose (OOTaddr *sp)
{
    OOTstring title, button1, button2, button3;
    void *stringArray, *arrayDescriptor;
    int defaultButton;

    MyExecutorScan (sp, "RRRRRRI", &title, &stringArray, &arrayDescriptor,
		    &button1, &button2, &button3, &defaultButton);
    MIOGUI_Choose (title, stringArray, arrayDescriptor, button1, button2,
		   button3, defaultButton);
} // MIO_gui_choose



/*
void MIO_gui_fileviewer (OOTaddr *sp)
{
    struct EventDescriptor event;

    OOTstring viewFile, banner;
    OOTint    fontId;

    MyExecutorScan (sp, "RRI", &viewFile, &banner, &fontId);

    if (viewFile == NULL) {
	// Detect the NULL write over when rerun
	return;
    }

    if (GUI_FileViewer(DEFAULT_WIN, viewFile, banner, fontId, &event))
    {
        *((OOTint *) sp) = 0;
        Language_Execute_EventQueueInsert(TURINGSTDIN, &event);
    }
} // MIO_gui_fileviewer

void MIO_gui_opendlg (OOTaddr *sp)
{
    OOTstring banner, openFile;

    MyExecutorScan (sp, "rRR", &banner, &openFile);

    RESULT_OOT_INT(sp, GUI_OpenDlg(DEFAULT_WIN, banner, openFile));
} // MIO_gui_opendlg

void MIO_gui_savedlg (OOTaddr *sp)
{
    OOTstring banner, saveFile;

    MyExecutorScan (sp, "rRR", &banner, &saveFile);

    RESULT_OOT_INT(sp, GUI_SaveDlg(DEFAULT_WIN, banner, saveFile));
} // MIO_gui_savedlg

void MIO_gui_alert (OOTaddr *sp)
{
    OOTstring banner, message;
    OOTint    placement;

    MyExecutorScan (sp, "RRI", &banner, &message, &placement);

    GUI_Alert(DEFAULT_WIN, banner, message, placement);
} // MIO_gui_alert

void MIO_gui_request (OOTaddr *sp)
{
    OOTstring banner, message, button1, button2, button3;
    OOTint    placement;

    MyExecutorScan (sp, "rRRRRRI", &banner, &message,
			&button1, &button2, &button3, &placement);

    RESULT_OOT_INT(sp, GUI_Request(DEFAULT_WIN, banner, message,
			button1, button2, button3, placement));
} // MIO_gui_request

void MIO_gui_input (OOTaddr *sp)
{
    OOTstring banner, message, button1, button2, inputStr;
    OOTint    placement;

    MyExecutorScan (sp, "rRRRRRI", &banner, &message,
			&button1, &button2, &inputStr, &placement);

    RESULT_OOT_INT(sp, GUI_Input(DEFAULT_WIN, banner, message,
			button1, button2, inputStr, placement));
} // MIO_gui_input
*/
/************************************************************************/
/* Input module								*/
/************************************************************************/
void MIO_input_keydown (OOTaddr *sp)
{
    void *array, *arrayDescriptor;

    MyExecutorScan (sp, "RR", &array, &arrayDescriptor);
    
    MIOWindow_GetKeyDown (array);
} // MIO_input_keydown


/************************************************************************/
/* Joy module								*/
/************************************************************************/
void MIO_joy_read (OOTaddr *sp)
{
    OOTint jnum;
    JOYPOSRECORD *jp;

    MyExecutorScan (sp, "IR", &jnum, &jp);

    MIOJoy_Read (jnum, jp);
} // MIO_joy_read

void MIO_joy_caps (OOTaddr *sp)
{
    OOTint jnum;
    JOYCAPSRECORD *jc;

    MyExecutorScan (sp, "IR", &jnum, &jc);

    MIOJoy_Caps (jnum, jc);
} // MIO_joy_caps


/************************************************************************/
/* Mouse module								*/
/************************************************************************/
void MIO_mouse_where (OOTaddr *sp)
{
    OOTint *x, *y, *btnUpDown;

    MyExecutorScan (sp, "RRR", &x, &y, &btnUpDown);

    MIOMouse_Where (x, y, btnUpDown);
} // MIO_mouse_where

void MIO_mouse_buttonmoved (OOTaddr *sp)
{
    OOTstring str;

    MyExecutorScan (sp, "rR", &str);
    RESULT_OOT_INT(sp, MIOMouse_ButtonMoved (str));
} // MIO_mouse_buttonmoved

void MIO_mouse_buttonwait (OOTaddr *sp)
{
    OOTstring str;
    OOTint *x, *y, *btnNumber, *btnUpDown;

    MyExecutorScan (sp, "RRRRR", &str, &x, &y, &btnNumber, &btnUpDown);
/*  EventDescriptor event;


    if (str == NULL) {
	// Detect the NULL write over when rerun
	return;
    }

    if (!EVENT_MouseButtonWait(DEFAULT_WIN, &event, str,
			       x, y, btnNumber, btnUpDown))
    {
	// Event not available, so queue
	Language_Execute_EventQueueInsert(TURINGSTDIN, &event);
    }
*/
    MIOMouse_ButtonWait (str, x, y, btnNumber, btnUpDown);
} // MIO_mouse_buttonwait

void MIO_mouse_buttonchoose (OOTaddr *sp)
{
    OOTstring str;

    MyExecutorScan (sp, "R", &str);
    MIOMouse_ButtonChoose(str);
} // MIO_mouse_buttonchoose

void MIO_mouse_hide (OOTaddr *sp)
{
    MIOMouse_Hide ();
} // MIO_mouse_hide

void MIO_mouse_show (OOTaddr *sp)
{
    MIOMouse_Show ();
} // MIO_mouse_show

void MIO_mouse_setposition (OOTaddr *sp)
{
    OOTint x, y;

    MyExecutorScan (sp, "II", &x, &y);
    MIOMouse_SetPosition (x, y);
} // MIO_mouse_setposition


/************************************************************************/
/* Net module								*/
/************************************************************************/
void MIO_net_available (OOTaddr *sp)
{
    OOTint netId, ty;

    MyExecutorScan (sp, "rII", &netId, &ty);

    RESULT_OOT_INT(sp, MIONet_Available (netId, ty));
} // MIO_net_available


void MIO_net_connectionstatus (OOTaddr *sp)
{
    OOTint netId;

    MyExecutorScan (sp, "rI", &netId);

    RESULT_OOT_INT(sp, MIONet_ConnectionStatus (netId));
} // MIO_net_available


void MIO_net_deletecookie (OOTaddr *sp)
{
    OOTstring url, name;

    MyExecutorScan (sp, "RR", &url, &name);

    MIONet_DeleteCookie (url, name);
} // MIO_net_deletecookie


void MIO_net_getaddr (OOTaddr *sp)
{
    OOTstring str;
    OOTint    netId;

    MyExecutorScan (sp, "IR", &netId, &str);

    MIONet_GetAddress (netId, str);
} // MIO_net_getaddr


void MIO_net_getcookie (OOTaddr *sp)
{
    OOTstring value, url, name;

    MyExecutorScan (sp, "RRR", &value, &url, &name);

    MIONet_GetCookie (value, url, name);
} // MIO_net_getcookie


void MIO_net_getcookies (OOTaddr *sp)
{
    OOTstring				url;
    OOTstring				*xArrayData, *yArrayData;
    struct Language_ArrayDescriptor	*xArrayDescriptor, *yArrayDescriptor;

    // Load an array of picIds, make certain numFrames <= arraySize
    MyExecutorScan (sp, "RRRRR", &url, &xArrayData, &xArrayDescriptor, 
		    &yArrayData, &yArrayDescriptor);

    MIONet_GetCookies (url, xArrayData, xArrayDescriptor -> elementCount, 
		       yArrayData, yArrayDescriptor -> elementCount);
} // MIO_net_getcookie


void MIO_net_getnumcookies (OOTaddr *sp)
{
    OOTstring url;

    MyExecutorScan (sp, "rR", &url);

    RESULT_OOT_INT(sp, MIONet_GetNumCookies (url));
} // MIO_net_getcookie


void MIO_net_gethostaddr (OOTaddr *sp)
{
    OOTstring name, addr;

    MyExecutorScan (sp, "RR", &addr, &name);

    MIONet_GetHostAddressByName (addr, name);
} // MIO_net_gethostaddr


void MIO_net_gethostname (OOTaddr *sp)
{
    OOTstring name, addr;

    MyExecutorScan (sp, "RR", &name, &addr);

    MIONet_GetHostNameByAddress(name, addr);
} // MIO_net_gethostname


void MIO_net_getlocaladdr (OOTaddr *sp)
{
    OOTstring str;

    MyExecutorScan (sp, "R", &str);

    MIONet_GetLocalAddress (str);
} // MIO_net_getlocaladdr


void MIO_net_getlocalname (OOTaddr *sp)
{
    OOTstring str;

    MyExecutorScan (sp, "R", &str);

    MIONet_GetLocalName (str);
} // MIO_net_getlocalname


void MIO_net_registerclose (OOTaddr *sp)
{
    OOTint netId;

    MyExecutorScan (sp, "I", &netId);
    MIONet_RegisterClose (netId);
} // MIO_net_registerclose


void MIO_net_registeropen (OOTaddr *sp)
{
    OOTint	netId;
    SRCPOS	srcPos;

    MyExecutorScan (sp, "I", &netId);

    Language_Execute_RunSrcPosition(&srcPos);

    // The call to open the connection has stored a pointer to the connection
    // information in the MIONet module.  The registeropen will link the 
    // connection with the stream ID that Turing supplied after the return
    // from the MIONet module.

    MIONet_RegisterOpen (netId, &srcPos);
} // MIO_net_registeropen


void MIO_net_setcookie (OOTaddr *sp)
{
    OOTstring url, name, value;
    OOTint expiryDate;

    MyExecutorScan (sp, "RRRI", &url, &name, &value, &expiryDate);

    MIONet_SetCookie (url, name, value, expiryDate);
} // MIO_net_setcookie


void MIO_net_waitforconnect (OOTaddr *sp)
{
    struct EventDescriptor event;

    OOTint netId;

    MyExecutorScan (sp, "I", &netId);

    if (netId == 0) 
    {
	// Trap rerun
	return;
    }

    if (MIONet_WaitForConnect (netId, &event)) 
    {
	*((OOTint *) sp) = 0;	 /* Overwrite pointer with NULL */
	Language_Execute_EventQueueInsert (netId, &event);
    }
} // MIO_net_waitforconnect


/************************************************************************/
/* Obsolete module							*/
/************************************************************************/
void MIO_obsolete (OOTaddr *sp)
{
    OOTstring routineName;
    char errorMessage [1024];
    
    MyExecutorScan (sp, "R", &routineName);
    
    strcpy (errorMessage, "Obsolete subprogram \"");
    strcat (errorMessage, routineName);
    strcat (errorMessage, "\" no longer available in Turing");
    
    Language_Execute_ExecutionError_Abort (E_NOT_SUPPORTED, errorMessage);
} // MIO_obsolete

void MIO_obsolete_replace (OOTaddr *sp)
{
    OOTstring routineName, replacementRoutineName;
    char errorMessage [1024];
    
    MyExecutorScan (sp, "R", &routineName, &replacementRoutineName);
    
    strcpy (errorMessage, "Obsolete subprogram \"");
    strcat (errorMessage, routineName);
    strcat (errorMessage, "\" no longer available in Turing.  Use \"");
    strcat (errorMessage, replacementRoutineName);
    strcat (errorMessage, "\" instead.");
    
    Language_Execute_ExecutionError_Abort (E_NOT_SUPPORTED, errorMessage);
} // MIO_obsolete_replace


/************************************************************************/
/* PC module								*/
/************************************************************************/
void MIO_pc_inport (OOTaddr *sp)
{
    OOTint portAddress;
    
    MyExecutorScan (sp, "rI", &portAddress);
    
    RESULT_OOT_INT(sp, MIOPC_InPort (portAddress));
} // MIO_pc_inport


void MIO_pc_outport (OOTaddr *sp)
{
    OOTint portAddress, value;

    MyExecutorScan (sp, "II", &portAddress, &value);
    MIOPC_OutPort (portAddress, value);
} // MIO_pc_outport


void MIO_pc_parallelget (OOTaddr *sp)
{
    OOTint port;
    
    MyExecutorScan (sp, "rI", &port);
    
    RESULT_OOT_INT(sp, MIOPC_ParallelGet (port));
} // MIO_pc_parallelgetone


void MIO_pc_parallelput (OOTaddr *sp)
{
    OOTint port, value;

    MyExecutorScan (sp, "II", &port, &value);
    MIOPC_ParallelPut (port, value);
} // MIO_pc_parallelputone


/************************************************************************/
/* Pic module								*/
/************************************************************************/
void MIO_pic_new (OOTaddr *sp)
{
    OOTint	x1, y1, x2, y2;
    SRCPOS	srcPos;

    MyExecutorScan (sp, "rIIII", &x1, &y1, &x2, &y2);

    Language_Execute_RunSrcPosition(&srcPos);

    RESULT_OOT_INT(sp, MIOPic_New (x1, y1, x2, y2, &srcPos));
} // MIO_pic_new


void MIO_pic_free (OOTaddr *sp)
{
    OOTint picId;

    MyExecutorScan (sp, "I", &picId);
    MIOPic_Free (picId);
} // MIO_pic_free


void MIO_pic_blend (OOTaddr *sp)
{
    OOTint	picId1, picId2, percent;
    SRCPOS	srcPos;

    MyExecutorScan (sp, "rIII", &picId1, &picId2, &percent);

    Language_Execute_RunSrcPosition(&srcPos);

    RESULT_OOT_INT(sp, MIOPic_Blend (picId1, picId2, percent, &srcPos));
} // MIO_pic_blend


void MIO_pic_blur (OOTaddr *sp)
{
    OOTint	picId, blurFactor;
    SRCPOS	srcPos;

    MyExecutorScan (sp, "rII", &picId, &blurFactor);

    Language_Execute_RunSrcPosition(&srcPos);

    RESULT_OOT_INT(sp, MIOPic_Blur (picId, blurFactor, &srcPos));
} // MIO_pic_blur


void MIO_pic_draw (OOTaddr *sp)
{
    OOTint picId, x, y, mode;

    MyExecutorScan (sp, "IIII", &picId, &x, &y, &mode);
    MIOPic_Draw (picId, x, y, mode);
} // MIO_pic_draw


//
// The logic for this is as follows: 
//
// When this is first called, we call MIOMusic_Play which sets up a set
// of tones to play and starts the first one playing.  It then returns,
// to MIO_music_play, placing an event on the queue and setting the 
// string argument to a null string.  The system every so often checks
// the event queue, which checks to see if the current note is finished 
// playing, and if so, plays the next note.  When the final note is finished
// playing, the check returns that it's okay to call MIO_music_play, and 
// it's called a second time.  However, this time, it's argument is null,
// so it turns the sound off and exits.
//
void MIO_pic_drawframes (OOTaddr *sp)
{
    struct EventDescriptor		event;
    OOTint				*arrayData;
    struct Language_ArrayDescriptor	*arrayDescriptor;
    OOTint				x, y, mode, numFrames, delayTime;
    OOTint				eraseAfter; // Actually a boolean

    // Load an array of picIds, make certain numFrames <= arraySize
    MyExecutorScan (sp, "RRIIIII", &arrayData, &arrayDescriptor, &x, &y, 
	&mode, &numFrames, &delayTime, &eraseAfter);

    if (arrayData == NULL)
    {
	// We're done.  This was the rerun.
	return;
    }
    
    if (MIOPic_DrawFrames (&event, arrayData, x, y, mode, numFrames, 
			   arrayDescriptor -> elementCount, delayTime,
			   (OOTboolean) eraseAfter))
    {
	* (OOTint *) sp = 0;	// Overwrite pointer with NULL
	Language_Execute_EventQueueInsert (TURINGSTDIN, &event);
    }
} // MIO_pic_drawframes


void MIO_pic_drawspecial (OOTaddr *sp)
{
    struct EventDescriptor	event;
    OOTint			picId, x, y, mode, transition, duration;

    // Load an array of picIds, make certain numFrames <= arraySize
    MyExecutorScan (sp, "IIIIII", &picId, &x, &y, &mode, &transition, 
	&duration);

    if (picId == -199)
    {
	// We're done.  This was the rerun.
	return;
    }
    
    if (MIOPic_DrawSpecial (&event, picId, x, y, mode, transition, duration))
    {
	* (OOTint *) sp = -199;	// Overwrite first parameter with -199
	Language_Execute_EventQueueInsert (TURINGSTDIN, &event);
    }
} // MIO_pic_drawspecial


void MIO_pic_filenew (OOTaddr *sp)
{
    OOTstring	fileName;
    SRCPOS	srcPos;

    MyExecutorScan (sp, "rR", &fileName);

    Language_Execute_RunSrcPosition(&srcPos);

    RESULT_OOT_INT(sp, MIOPic_FileNew (fileName, &srcPos));
} // MIO_pic_filenew


void MIO_pic_filenewframes (OOTaddr *sp)
{
    OOTint				*arrayData;
    struct Language_ArrayDescriptor	*arrayDescriptor;
    OOTint				*delayTime;
    OOTstring				fileName;
    SRCPOS				srcPos;

    MyExecutorScan (sp, "RRRR", &fileName, &arrayData, &arrayDescriptor, &delayTime);

    Language_Execute_RunSrcPosition(&srcPos);
    MIOPic_FileNewFrames (arrayData, arrayDescriptor -> elementCount, fileName, delayTime, &srcPos);
} // MIO_pic_filenewframes


void MIO_pic_filesave (OOTaddr *sp)
{
    OOTint     picId;
    OOTstring  fileName;

    MyExecutorScan (sp, "IR", &picId, &fileName);
    MIOPic_FileSave (picId, fileName);
} // MIO_pic_filesave


void MIO_pic_frames (OOTaddr *sp)
{
    OOTstring	fileName;

    MyExecutorScan (sp, "rR", &fileName);

    RESULT_OOT_INT(sp, MIOPic_Frames (fileName));
} // MIO_pic_frames


void MIO_pic_getdimensions (OOTaddr *sp)
{
    OOTint picId;
    OOTint *w, *h;

    MyExecutorScan (sp, "IRR", &picId, &w, &h);

    MIOPic_GetDimensions (picId, w, h);
} // MIO_pic_getdimensions


void MIO_pic_flip (OOTaddr *sp)
{
    OOTint picId;
    SRCPOS srcPos;
    
    MyExecutorScan (sp, "rI", &picId);

    Language_Execute_RunSrcPosition(&srcPos);

    RESULT_OOT_INT(sp, MIOPic_Flip (picId, TRUE, &srcPos));
} // MIO_pic_flipvertical


void MIO_pic_mirror (OOTaddr *sp)
{
    OOTint picId;
    SRCPOS srcPos;
    
    MyExecutorScan (sp, "rI", &picId);

    Language_Execute_RunSrcPosition(&srcPos);

    RESULT_OOT_INT(sp, MIOPic_Flip (picId, FALSE, &srcPos));
} // MIO_pic_fliphorizontal


void MIO_pic_rotate (OOTaddr *sp)
{
    OOTint picId, angle, xAxis, yAxis;
    SRCPOS srcPos;
    
    MyExecutorScan (sp, "rIIII", &picId, &angle, &xAxis, &yAxis);

    Language_Execute_RunSrcPosition(&srcPos);

    RESULT_OOT_INT(sp, MIOPic_Rotate (picId, angle, xAxis, yAxis, &srcPos));
} // MIO_pic_rotate


void MIO_pic_scale (OOTaddr *sp)
{
    OOTint picId, newWidth, newHeight;
    SRCPOS srcPos;
    
    MyExecutorScan (sp, "rIII", &picId, &newWidth, &newHeight);

    Language_Execute_RunSrcPosition(&srcPos);

    RESULT_OOT_INT(sp, MIOPic_Scale (picId, newWidth, newHeight, &srcPos));
} // MIO_pic_rotate


void MIO_pic_settransclr (OOTaddr *sp)
{
    OOTint picId, clrNum;

    MyExecutorScan (sp, "II", &picId, &clrNum);
    MIOPic_SetTransparentColour (picId, clrNum);
} // MIO_pic_settransclr


/************************************************************************/
/* RGB module								*/
/************************************************************************/

void MIO_rgb_addcolour (OOTaddr *sp)
{
    OOTreal red, green, blue;

    MyExecutorScan (sp, "r888", &red, &green, &blue);

    RESULT_OOT_INT(sp, MIORGB_AddColour (red, green, blue));
} // MIO_rgb_addcolour


void MIO_rgb_setcolour (OOTaddr *sp)
{
    OOTint  colourNum;
    OOTreal red, green, blue;

    MyExecutorScan (sp, "I888", &colourNum, &red, &green, &blue);

    MIORGB_SetColour (colourNum, red, green, blue);
} // MIO_rgb_setcolour


void MIO_rgb_getcolour (OOTaddr *sp)
{
    OOTint  colourNum;
    OOTreal *red, *green, *blue;

    MyExecutorScan (sp, "IRRR", &colourNum, &red, &green, &blue);

    MIORGB_GetColour (colourNum, red, green, blue);
} // MIO_rgb_getcolour

void MIO_rgb_maxcolour (OOTaddr *sp)
{
    RESULT_OOT_INT(sp, MIORGB_GetMaxColour ());
} // MIO_rgb_maxcolour


/************************************************************************/
/* Music module								*/
/************************************************************************/
void MIO_music_soundoff (OOTaddr *sp)
{
    MIOMusic_SoundOff ();
} // MIO_music_soundoff


void MIO_music_soundon (OOTaddr *sp)
{
    struct EventDescriptor event;

    OOTint freq, duration;

    MyExecutorScan (sp, "II", &freq, &duration);

    if ((freq == 0) || (duration == 0))
    {
	// Either it is trivial, or we're done
//	MIOMusic_SoundOff ();
	return;
    }

    if (MIOMusic_Sound (&event, freq, duration)) 
    {
	// Overwrite element on stack with 0.  This determines that the 
	// next time this is called, the freq is zero.
	*((OOTint *) sp) = 0;	

	Language_Execute_EventQueueInsert (TURINGSTDIN, &event);
    }
} // MIO_music_soundon


//
// The logic for this is as follows: 
//
// When this is first called, we call MIOMusic_Play which sets up a set
// of tones to play and starts the first one playing.  It then returns,
// to MIO_music_play, placing an event on the queue and setting the 
// string argument to a null string.  The system every so often checks
// the event queue, which checks to see if the current note is finished 
// playing, and if so, plays the next note.  When the final note is finished
// playing, the check returns that it's okay to call MIO_music_play, and 
// it's called a second time.  However, this time, it's argument is null,
// so it turns the sound off and exits.
//
void MIO_music_play (OOTaddr *sp)
{
    struct EventDescriptor event;

    OOTstring playStr;

    MyExecutorScan (sp, "R", &playStr);

    if (playStr == NULL) 
    {
	// We're done.  This was the rerun
	MIOMusic_SoundOff ();
	return;
    }

    if (MIOMusic_Play (&event, playStr)) 
    {
	*((OOTint *) sp) = 0;	// Overwrite pointer with NULL
	Language_Execute_EventQueueInsert (TURINGSTDIN, &event);
    }
} // MIO_music_play


void MIO_music_playdone (OOTaddr *sp)
{
    RESULT_OOT_INT(sp, MIOMusic_PlayDone ());
} // MIO_music_playdone


void MIO_music_settempo (OOTaddr *sp)
{
    OOTint wholeNoteDuration;

    MyExecutorScan (sp, "I", &wholeNoteDuration);

    MIOMusic_SetTempo (wholeNoteDuration);
} // MIO_music_settempo


void MIO_music_playfile (OOTaddr *sp)
{
    struct EventDescriptor event;

    OOTstring playFile;

    MyExecutorScan (sp, "R", &playFile);

    if (playFile == NULL) 
    {
	// We're done.  This was the rerun
	return;
    }

    if (MIOMusic_PlayFile (&event, playFile)) 
    {
	*((OOTint *) sp) = 0;	// Overwrite pointer with NULL
	Language_Execute_EventQueueInsert (TURINGSTDIN, &event);
    }
} // MIO_music_playfile


void MIO_music_playfilestop (OOTaddr *sp)
{
    MIOMusic_PlayFileStop ();
} // MIO_music_playfilestop


void MIO_music_playfiledone (OOTaddr *sp)
{
    RESULT_OOT_INT(sp, MIOMusic_PlayFileDone ());
} // MIO_music_playfiledone


void MIO_music_preload (OOTaddr *sp)
{
    OOTstring playFile;

    MyExecutorScan (sp, "R", &playFile);

    if (playFile == NULL) 
    {
	// We're done.  This was the rerun
	return;
    }

    MIOMusic_PreLoad (playFile);
} // MIO_music_playfile


/************************************************************************/
/* Sprite module							*/
/************************************************************************/
void MIO_sprite_new (OOTaddr *sp)
{
    OOTint	picId;
    SRCPOS	srcPos;

    MyExecutorScan (sp, "rI", &picId);

    Language_Execute_RunSrcPosition(&srcPos);

    RESULT_OOT_INT(sp, MIOSprite_New (picId, &srcPos));
} // MIO_sprite_new

void MIO_sprite_free (OOTaddr *sp)
{
    OOTint spriteId;

    MyExecutorScan (sp, "I", &spriteId);

    MIOSprite_Free (spriteId);
} // MIO_sprite_free

void MIO_sprite_show (OOTaddr *sp)
{
    OOTint spriteId;

    MyExecutorScan (sp, "I", &spriteId);

    MIOSprite_Show(spriteId);
} // MIO_sprite_show

void MIO_sprite_hide (OOTaddr *sp)
{
    OOTint spriteId;

    MyExecutorScan (sp, "I", &spriteId);

    MIOSprite_Hide(spriteId);
} // MIO_sprite_hide

void MIO_sprite_setframerate (OOTaddr *sp)
{
    OOTint frameRate;

    MyExecutorScan (sp, "I", &frameRate);

    MIOSprite_SetFrameRate(frameRate);
} // MIO_sprite_setframerate

void MIO_sprite_setposition (OOTaddr *sp)
{
    OOTint spriteId, x, y, centered;

    MyExecutorScan (sp, "IIII", &spriteId, &x, &y, &centered);

    MIOSprite_SetPosition(spriteId, x, y, centered);
} // MIO_sprite_setposition

void MIO_sprite_setheight (OOTaddr *sp)
{
    OOTint spriteId, height;

    MyExecutorScan (sp, "II", &spriteId, &height);

    MIOSprite_SetHeight (spriteId, height);
} // MIO_sprite_setheight

void MIO_sprite_changepic (OOTaddr *sp)
{
    OOTint spriteId, picId;

    MyExecutorScan (sp, "II", &spriteId, &picId);

    MIOSprite_ChangePic (spriteId, picId);
} // MIO_sprite_changepic

void MIO_sprite_animate (OOTaddr *sp)
{
    OOTint spriteId, picId, x, y, centered;

    MyExecutorScan (sp, "IIIII", &spriteId, &picId, &x, &y, &centered);

    MIOSprite_Animate (spriteId, picId, x, y, centered);
} // MIO_sprite_animate


/* -----------
 * Str module
 * ----------- */

/* This doesn't really belong here, but I want to write it in C, and
   this is the best place for it. */
void String_ReadLineFromCharArray (OOTaddr *sp)
{
    // chars : char (*), numChars : int, var line : string, var arrayPos : int
    char	*charArray;
    int		charArraySize;
    int		numChars;
    OOTstring	line;
    OOTint	*arrayPos;  // (1-based)
    int		len, maxLen;
    char	*ptr;

    MyExecutorScan (sp, "RIIRR", &charArray, &charArraySize, &numChars,
		    &line, &arrayPos);

    // Read from arrayPos into the array until end of line
    if ((*arrayPos < 1) || (*arrayPos > numChars))
    {
	// arrayPos must be between 1 and numChars
	SET_ERRNO (E_STR_ARRAY_POS_OUT_OF_BOUNDS);
	line [0] = 0;
	return;
    }
    if (numChars > charArraySize)
    {
	// numChars larger then array size
	SET_ERRNO (E_STR_NUM_CHARS_TOO_LARGE);
	line [0] = 0;
	return;
    }
    
    ptr = &charArray [*arrayPos - 1];
    len = 0;
    maxLen = numChars - *arrayPos + 1;
    for (;;)
    {
	if ((len >= maxLen) ||
	    (*ptr == '\r') ||
	    (*ptr == '\n') ||
	    (*ptr == 0) ||
	    (len >= 255))
	{
	    break;
	}
	len++;
	ptr++;
    }
    strncpy (line, &charArray [*arrayPos - 1], len);
    line [len] = 0;
    if (len >= maxLen)
    {
	// Set partial line error
	SET_ERRNO (E_STR_PARTIAL_LINE);
	*arrayPos = numChars + 1;
    }
    else if (*ptr == '\r')
    {
	ptr++;
	// We use this to skip over \r\n
	if (*ptr == '\n')
	{
	    len++;
	}
	// Certain telnets may put a 0 and \r, skip over
	// \r, \r\0, \r\0\n
	else if (*ptr == '0')
	{
	    len++;
	    if (*ptr == '\n')
	    {
		len++;
	    }
	}
	*arrayPos += len + 1;
    }
    else if ((*ptr == '\n') || (*ptr == 0))
    {
	*arrayPos += len + 1;
    }
    else
    {
	// Reached 255 chars
	SET_ERRNO (E_STR_LINE_TOO_LONG);
	*arrayPos += len;
    }
} // String_ReadLineFromCharArray

void String_WriteLineToCharArray (OOTaddr *sp)
{
    // var chars : char (*), line : string, var arrayPos : int
    char	*charArray;
    int		charArraySize;
    OOTstring	line;
    OOTint	*arrayPos;  // (1-based)
    int		len;
    char	*ptr;

    MyExecutorScan (sp, "RIRR", &charArray, &charArraySize, &line, &arrayPos);

    // Read from arrayPos into the array until end of line
    if ((*arrayPos < 1) || (*arrayPos > charArraySize))
    {
	// arrayPos must be between 1 and numChars
	SET_ERRNO (E_STR_ARRAY_POS_OUT_OF_BOUNDS);
	return;
    }
    
    ptr = &charArray [*arrayPos - 1];
    len = strlen (line);
    if (*arrayPos + len + 2 > charArraySize)
    {
	SET_ERRNO (E_STR_CHAR_ARRAY_TOO_SMALL_FOR_WRITE_LINE);
	return;
    }
    strncpy (&charArray [*arrayPos - 1], line, len);
    *arrayPos += len;

    // Add newline
    charArray [*arrayPos - 1] = '\r';
    *arrayPos += 1;
    charArray [*arrayPos - 1] = '\n';
    *arrayPos += 1;
} // String_WriteLineToCharArray

void String_WriteToCharArray (OOTaddr *sp)
{
    // var chars : char (*), line : string, var arrayPos : int
    char	*charArray;
    int		charArraySize;
    OOTstring	line;
    OOTint	*arrayPos;  // (1-based)
    int		len;
    char	*ptr;

    MyExecutorScan (sp, "RIRR", &charArray, &charArraySize, &line, &arrayPos);

    // Read from arrayPos into the array until end of line
    if ((*arrayPos < 1) || (*arrayPos > charArraySize))
    {
	// arrayPos must be between 1 and numChars
	SET_ERRNO (E_STR_ARRAY_POS_OUT_OF_BOUNDS);
	return;
    }
    
    ptr = &charArray [*arrayPos - 1];
    len = strlen (line);
    if (*arrayPos + len > charArraySize)
    {
	SET_ERRNO (E_STR_CHAR_ARRAY_TOO_SMALL_FOR_WRITE_LINE);
	return;
    }
    strncpy (&charArray [*arrayPos - 1], line, len);
    *arrayPos += len;
} // String_WriteToCharArray

void String_CharArrayCopy (OOTaddr *sp)
{
    // var srcChars : char (*), srcPos : int, destChars : char (*), 
    // destPos : int, len : int
    char	*srcCharArray, *destCharArray;
    int		srcCharArraySize, destCharArraySize;
    OOTint	srcArrayPos, destArrayPos;  // (1-based)
    int		len;

    MyExecutorScan (sp, "RIIRIII", &srcCharArray, &srcCharArraySize, 
	&srcArrayPos, &destCharArray, &destCharArraySize, &destArrayPos, &len);

    // Determine if srcPos is out of bounds
    if ((srcArrayPos < 1) || (srcArrayPos > srcCharArraySize))
    {
	// arrayPos must be between 1 and numChars
	SET_ERRNO (E_STR_SRC_ARRAY_POS_OUT_OF_BOUNDS);
	return;
    }
    
    // Determine if destPos is out of bounds
    if ((destArrayPos < 1) || (destArrayPos > destCharArraySize))
    {
	// arrayPos must be between 1 and numChars
	SET_ERRNO (E_STR_DEST_ARRAY_POS_OUT_OF_BOUNDS);
	return;
    }
    
    // Determine if destArray size would be exceeded
    if (destArrayPos + len > destCharArraySize)
    {
	SET_ERRNO (E_STR_CHAR_ARRAY_TOO_SMALL_FOR_CHAR_COPY);
	return;
    }

    strncpy (&destCharArray [destArrayPos - 1], &srcCharArray [srcArrayPos - 1], len);
} // String_CharArrayCopy

/* -----------
 * Sys module
 * ----------- */

void MIO_sys_exec (OOTaddr *sp)
{
    OOTstring	commandStr;

    MyExecutorScan (sp, "rR", &commandStr);

    RESULT_OOT_BOOL(sp, MIOSys_Exec (commandStr));
} // MIO_sys_exec

void MIO_sys_exit (OOTaddr *sp)
{
    MIOSys_Exit ();
} // MIO_sys_exec

void MIO_sys_getcomputername (OOTaddr *sp)
{
    OOTstring	resultStr;

    MyExecutorScan (sp, "R", &resultStr);

    MIOSys_GetComputerName (resultStr);
} // MIO_sys_getcomputername

void MIO_sys_getenv (OOTaddr *sp)
{
    OOTstring	symbolStr, resultStr;

    MyExecutorScan (sp, "RR", &resultStr, &symbolStr);

    MIOSys_GetEnv (symbolStr, resultStr);
} // MIO_sys_getenv

void MIO_sys_getpid (OOTaddr *sp)
{
    RESULT_OOT_INT(sp, MIOSys_GetPID ());
} // MIO_sys_getpid

void MIO_sys_getusername (OOTaddr *sp)
{
    OOTstring	resultStr;

    MyExecutorScan (sp, "R", &resultStr);

    MIOSys_GetUserName (resultStr);
} // MIO_sys_getusername


/* -----------
 * Text module
 * ----------- */

void MIO_text_cls (OOTaddr *sp)
{
    MIOText_Cls ();
} // MIO_text_cls 

void MIO_text_colour (OOTaddr *sp)
{
    OOTint clrNum;

    MyExecutorScan (sp, "I", &clrNum);
    MIOText_SetColour (clrNum);
} // MIO_text_colour

void MIO_text_colourback (OOTaddr *sp)
{
    OOTint clrNum;

    MyExecutorScan (sp, "I", &clrNum);
    MIOText_SetBackgroundColour (clrNum);
} // MIO_text_colourback

void MIO_text_locate (OOTaddr *sp)
{
    OOTint row, col;

    MyExecutorScan (sp, "II", &row, &col);
    MIOText_Locate (row, col);
} // MIO_text_locate

void MIO_text_locatexy (OOTaddr *sp)
{
    OOTint x, y;

    MyExecutorScan (sp, "II", &x, &y);
    MIOText_LocateXY (x, y);
} // MIO_text_locatexy

void MIO_text_maxcol (OOTaddr *sp)
{
    RESULT_OOT_INT(sp, MIOText_GetMaxCol ());
} // MIO_text_maxcol

void MIO_text_maxrow (OOTaddr *sp)
{
    RESULT_OOT_INT(sp, MIOText_GetMaxRow ());
} // MIO_text_maxrow

void MIO_text_whatcol (OOTaddr *sp)
{
    RESULT_OOT_INT(sp, MIOText_GetCurrentCol ());
} // MIO_text_whatcol

void MIO_text_whatrow (OOTaddr *sp)
{
    RESULT_OOT_INT(sp, MIOText_GetCurrentRow ());
} // MIO_text_whatrow

void MIO_text_whatcolour (OOTaddr *sp)
{
    RESULT_OOT_INT(sp, MIOText_GetCurrentColour ());
} // MIO_text_whatcolour

void MIO_text_whatcolourback (OOTaddr *sp)
{
    RESULT_OOT_INT(sp, MIOText_GetCurrentColourBack ());
} // MIO_text_whatcolourback

void MIO_text_whatchar (OOTaddr *sp)
{
    OOTint *chr, *fclr, *bclr;

    MyExecutorScan (sp, "RRR", &chr, &fclr, &bclr);

    // Produces an error
    MIOText_WhatChar ();
} // MIO_text_whatchar


/************************************************************************/
/* Time module								*/
/************************************************************************/
void MIO_time_sec (OOTaddr *sp)
{
    RESULT_OOT_INT(sp, MIOTime_Sec ());
} // MIO_time_sec


void MIO_time_date (OOTaddr *sp)
{
    OOTstring str;

    MyExecutorScan (sp, "R", &str);

    MIOTime_Date (str);
} // MIO_time_date


void MIO_time_secdate (OOTaddr *sp)
{
    OOTstring   str;
    OOTint      ootTime;

    MyExecutorScan (sp, "RI", &str, &ootTime);

    MIOTime_SecDate (str, ootTime);
} // MIO_time_secdate


void MIO_time_datesec (OOTaddr *sp)
{
    OOTstring str;

    MyExecutorScan (sp, "rR", &str);

    RESULT_OOT_INT(sp, MIOTime_DateSec (str));
} // MIO_time_datesec


void MIO_time_secparts (OOTaddr *sp)
{
    OOTint ootTime;
    OOTint *year, *mon, *day, *dow, *hour, *min, *sec;

    MyExecutorScan (sp, "IRRRRRRR", &ootTime,
	&year, &mon, &day, &dow, &hour, &min, &sec);

    MIOTime_SecParts (ootTime, year, mon, day, dow, hour, min, sec);
} // MIO_time_secparts


void MIO_time_partssec (OOTaddr *sp)
{
    OOTint ootTime;
    OOTint year, mon, day, hour, min, sec;

    MyExecutorScan (sp, "rIIIIII", &year, &mon, &day, &hour, &min, &sec);

    ootTime = MIOTime_PartsSec (year, mon, day, hour, min, sec);

    RESULT_OOT_INT(sp, ootTime);
} // MIO_time_partssec


void MIO_time_elapsed (OOTaddr *sp)
{
    RESULT_OOT_INT(sp, MIOTime_Elapsed ());
} // MIO_time_elapsed


void MIO_time_elapsedcpu (OOTaddr *sp)
{
    RESULT_OOT_INT(sp, MIOTime_ElapsedCPU ());
} // MIO_time_elapsedcpu


/*  Initialize for a delay of at least duration milliseconds.
 *
 *  OOT will then make a series of calls to MIO_HasEvent with the pointer to
 *  an event descriptor, and the count will be what we set here.
 */
void MIO_time_delay (OOTaddr *sp)
{
    struct EventDescriptor event;

    OOTint duration;

    MyExecutorScan (sp, "I", &duration);

    if (duration == 0) 
    {
	/* Either it is trivial, or we're done */
	return;
    }

    *((OOTint *) sp) = 0;
    
    event.mode  = EventMode_Delay;
    event.count = MIOTime_GetTicks () + duration;
    
    Language_Execute_EventQueueInsert (TURINGSTDIN, &event);
} // MIO_time_delay


void MIO_time_secstr (OOTaddr *sp)
{
    OOTstring  str, formatStr;
    OOTint     ootTime;

    MyExecutorScan (sp, "RIR", &str, &ootTime, &formatStr);

    MIOTime_SecStr(str, ootTime, formatStr);
} // MIO_time_secstr


/************************************************************************/
/* View module								*/
/************************************************************************/
void MIO_view_clipset (OOTaddr *sp)
{
    OOTint x1, y1, x2, y2;

    MyExecutorScan (sp, "IIII", &x1, &y1, &x2, &y2);
    MIOView_ClipSet (x1, y1, x2, y2);
} // MIO_view_clipset

void MIO_view_clipadd (OOTaddr *sp)
{
    OOTint x1, y1, x2, y2;

    MyExecutorScan (sp, "IIII", &x1, &y1, &x2, &y2);
    MIOView_ClipAdd (x1, y1, x2, y2);
} // MIO_view_clipadd

void MIO_view_clipaddoval (OOTaddr *sp)
{
    OOTint x, y, xr, yr;

    MyExecutorScan (sp, "IIII", &x, &y, &xr, &yr);
    MIOView_ClipAddOval (x, y, xr, yr);
} // MIO_view_clipadd

void MIO_view_clipoff (OOTaddr *sp)
{
    MIOView_ClipOff ();
} // MIO_view_clipoff

void MIO_view_set (OOTaddr *sp)
{
    OOTstring viewSetStr;

    MyExecutorScan (sp, "R", &viewSetStr);
    MIOWindow_SetScreen (CURRENTLY_SELECTED_WINDOW, viewSetStr);
} // MIO_view_set

void MIO_view_maxx (OOTaddr *sp)
{
    RESULT_OOT_INT(sp, MIOView_GetMaxx ());
} // MIO_view_maxx

void MIO_view_maxy (OOTaddr *sp)
{
    RESULT_OOT_INT(sp, MIOView_GetMaxy ());
} // MIO_view_maxy

void MIO_view_whatdotcolour (OOTaddr *sp)
{
    OOTint x, y;

    MyExecutorScan (sp, "rII", &x, &y);
    RESULT_OOT_INT(sp, MIOView_WhatDotColour (x, y));
} // MIO_view_whatdotcolour

void MIO_view_scroll (OOTaddr *sp)
{
    OOTint dx, dy, clear;

    MyExecutorScan (sp, "III", &dx, &dy, &clear);
    MIOView_Scroll (dx, dy, clear);
} // MIO_view_scroll

void MIO_view_settransclr (OOTaddr *sp)
{
    OOTint clrNum;

    MyExecutorScan (sp, "I", &clrNum);
    MIOWindow_SetTransparentColour (CURRENTLY_SELECTED_WINDOW, clrNum);
} // MIO_view_settransclr

void MIO_view_update (OOTaddr *sp)
{
    MIOWindow_Update (CURRENTLY_SELECTED_WINDOW);
} // MIO_view_update

void MIO_view_updatearea (OOTaddr *sp)
{
    OOTint x1, y1, x2, y2;
    
    MyExecutorScan (sp, "IIII", &x1, &y1, &x2, &y2);
    MIOWindow_UpdateArea (CURRENTLY_SELECTED_WINDOW, x1, y1, x2, y2);
} // MIO_view_update

/************************************************************************/
/* Window module							*/
/************************************************************************/
void MIO_win_registeropen (OOTaddr *sp)
{
    OOTint 	winId;
    SRCPOS	srcPos;

    MyExecutorScan (sp, "I", &winId);

    Language_Execute_RunSrcPosition(&srcPos);

    MIOWindow_RegisterOpen (winId, &srcPos);
} // MIO_win_registeropen

void MIO_win_registerclose (OOTaddr *sp)
{
    OOTint winId;

    MyExecutorScan (sp, "I", &winId);
//    MIOWindow_RegisterClose (winId);
} // MIO_win_registerclose

void MIO_win_set (OOTaddr *sp)
{
    OOTint    winId;
    OOTstring winSetStr;

    MyExecutorScan (sp, "IR", &winId, &winSetStr);
    MIOWindow_SetScreen (winId, winSetStr);
} // MIO_win_set

void MIO_win_select (OOTaddr *sp)
{
    /* Implemented complete within predef for now */
} // MIO_win_select

void MIO_win_getselect (OOTaddr *sp)
{
    RESULT_OOT_INT(sp, MIOWindow_GetSelect ());
} // MIO_win_getselect

void MIO_win_setactive (OOTaddr *sp)
{
    OOTint winId;

    MyExecutorScan (sp, "I", &winId);
    MIOWindow_SetActive (winId);
} // MIO_win_setactive

void MIO_win_getactive (OOTaddr *sp)
{
    RESULT_OOT_INT(sp, MIOWindow_GetActive());
} // MIO_win_getactive

void MIO_win_hide (OOTaddr *sp)
{
    OOTint winId;

    MyExecutorScan (sp, "I", &winId);
    MIOWindow_Hide (winId);
} // MIO_win_hide

void MIO_win_show (OOTaddr *sp)
{
    OOTint winId;

    MyExecutorScan (sp, "I", &winId);
    MIOWindow_Show (winId);
} // MIO_win_show

void MIO_win_isvisible (OOTaddr *sp)
{
    OOTint winId;

    MyExecutorScan (sp, "rI", &winId);
    RESULT_OOT_BOOL(sp, MIOWindow_IsVisible (winId));
} // MIO_win_hide

void MIO_win_setposition (OOTaddr *sp)
{
    OOTint winId, x, y;

    MyExecutorScan (sp, "III", &winId, &x, &y);
    MIOWindow_SetPosition (winId, x, y);
} // MIO_win_setposition

void MIO_win_getposition (OOTaddr *sp)
{
    OOTint winId, *x, *y;

    MyExecutorScan (sp, "IRR", &winId, &x, &y);
    MIOWindow_GetPosition (winId, x, y);
} // MIO_win_setposition

void MIO_win_settransclr (OOTaddr *sp)
{
    OOTint winId, clrNum;

    MyExecutorScan (sp, "II", &winId, &clrNum);
    MIOWindow_SetTransparentColour (winId, clrNum);
} // MIO_win_settransclr

void MIO_win_update (OOTaddr *sp)
{
    OOTint winId;

    MyExecutorScan (sp, "I", &winId);
    MIOWindow_Update (winId);
} // MIO_view_update

void MIO_win_updatearea (OOTaddr *sp)
{
    OOTint winId, x1, y1, x2, y2;

    MyExecutorScan (sp, "IIIII", &winId, &x1, &y1, &x2, &y2);
    MIOWindow_UpdateArea (winId, x1, y1, x2, y2);
} // MIO_view_update

/* ------------------------ Class Library Support ------------------------ */
/************************************************************************/
/* Lexer class								*/
/************************************************************************/
void MIO_lexer_begin (OOTaddr *sp)
{
    OOTstring	srcFile, tableFile;
    OOTint	flags;
    SRCPOS	srcPos;

    MyExecutorScan (sp, "rRRI", &srcFile, &tableFile, &flags);

    Language_Execute_RunSrcPosition (&srcPos);

    RESULT_OOT_INT(sp, MIOLexer_Begin (srcFile, tableFile, flags, &srcPos));
} // MIO_lexer_begin

void MIO_lexer_end (OOTaddr *sp)
{
    OOTint lexId;

    MyExecutorScan (sp, "I", &lexId);
    MIOLexer_End (lexId);
} // MIO_lexer_end

void MIO_lexer_scan (OOTaddr *sp)
{
    OOTint lexId;

    TOKEN_RECORD *tok;

    MyExecutorScan (sp, "IR", &lexId, &tok);
    MIOLexer_Scan (lexId, tok);
} // MIO_lexer_scan


/******************************/
/* Static callback procedures */
/******************************/

/*********************/
/* Static procedures */
/*********************/

/************************************************************************/
/* MyExecutorScan							*/
/*									*/
/* These routines handle the implementation dependant details of	*/
/* storing and retrieving values/*pointers from the OOT executor stack.	*/
/*									*/
/* Since MyExecutorScan uses varargs, it is platform dependant.		*/
/*									*/
/* Parameters:	sp	- executor stack pointer			*/
/* 		param   - a string which encodes the parameters of	*/
/*			  a pre-defined routine (see below)		*/
/*		...     - various pointers to locations to receive	*/
/*			  values or pointers.				*/
/*									*/
/* param string:							*/
/*									*/
/*   The param string provides a simple encoding of the parameter	*/
/*  list of a predefined function. Care must be taken to insure that	*/
/*  the elements of the param string match that of the predefine 	*/
/*  routine in both order and type.					*/
/*									*/
/*   The following letters are defined to obtain parameters:		*/
/*									*/
/*   A - Pointer to array 1..*						*/
/*   I - Integer number value						*/
/*   N - Natural number value						*/
/*   R - General reference (pointer) var parameters, regular strings, 	*/
/*	 and string returns.						*/
/*   S - string(*)							*/
/*   4 - Single precision real value					*/
/*   8 - Double precision real value					*/
/*									*/
/*   Also, the following are also defined:				*/
/*									*/
/*   r - skip over the function scalar return pointer			*/
/*									*/
/* Examples:								*/
/*									*/
/*    procedure Win.Set( winId: int, winSetupStr: string)		*/
/*									*/
/*    %  The executor stack contains an integer value at sp, and	*/
/*    %  a pointer to an OOT string at sp+intSize.			*/
/*									*/
/*    OOTint    winId;							*/
/*    OOTstring winSetupStr;						*/
/*									*/
/*    ExecutorScan(sp, "IR", &winId, &winSetupStr);			*/
/*    WIN_SET_IMPLEMENTATION(winId, winSetupStr);			*/
/*									*/
/*									*/
/*    function Time.DateSec(dateTimeStr: string): int			*/
/*									*/
/*    % We're returning an integer value, which will be placed at	*/
/*    % the location pointed to by the pointer at sp (this is the OOT	*/
/*    % executor's scalar result convention).				*/
/*									*/
/*    OOTstring dateTimeStr;						*/
/*									*/
/*    MyExecutorScan (sp, "rR", &dateTimeStr);				*/
/*									*/
/*    RESULT_OOT_INT(sp, TIME_DATESEC_IMPLEMENTATION(dateTimeStr));	*/
/************************************************************************/
static void	MyExecutorScan (OOTaddr *sp, const char *param, ...)
{
   const char *pf = param;

   va_list curr_arg;

   va_start(curr_arg, param);

   while (*pf) 
   {
	switch(*pf)
	{
	    case 'r':
	        // Skip the function result stack position
	    	sp += ADDR_SIZE;
	    	break;

	    case 'A':
	    	{
	    	    OOTaddr *arg_arrayp = va_arg(curr_arg, OOTaddr *);

	    	    *arg_arrayp =  (OOTaddr) *sp;
	    	    sp += ADDR_SIZE*2;
	        }
	    	break;

	    case 'I':
	    	{
	    	    OOTint *arg_intp = va_arg(curr_arg, OOTint *);

	    	    *arg_intp = *((OOTint *) sp);
	    	    sp += INT_SIZE;
	    	}
	    	break;

	    case 'N':
	    	{
	    	    OOTnat *arg_natp = va_arg(curr_arg, OOTnat *);

	    	    *arg_natp = *((OOTnat *) sp);

	    	    sp += INT_SIZE;
	    	}
	    	break;

	    case 'R':
	    	{
	    	    OOTstring *arg_strp = va_arg(curr_arg, OOTstring *);

	    	    *arg_strp = (OOTstring) *sp;
	    	    sp += ADDR_SIZE;
	    	}
	    	break;

	    case '4':
	    	{
	    	    OOTreal4 *arg_real4p = va_arg(curr_arg, OOTreal4 *);

	    	    *arg_real4p = *((OOTreal4 *) sp);

	    	    sp += REAL4_SIZE;
	    	}
	    	break;

	    case '8':
	        {
	    	    OOTreal8 *arg_realp = va_arg(curr_arg, OOTreal *);

	    	    *arg_realp = *((OOTreal *) sp);

	    	    sp += REAL8_SIZE;
	    	}
	        break;
	} // switch
	pf++;
    } // while
    va_end(curr_arg);
} // MyExecutorScan
