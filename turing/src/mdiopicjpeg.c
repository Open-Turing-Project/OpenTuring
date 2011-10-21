/*****************/
/* mdiopicjpeg.c */
/*****************/

/*******************/
/* System includes */
/*******************/
#include <windows.h>
#include <setjmp.h>

/****************/
/* Self include */
/****************/
#include "mdiopicjpeg.h"

/******************/
/* Other includes */
/******************/
#include "jpeg-6b\\jpeglib.h"
#include "jpeg-6b\\jerror.h"

#include "mio.h"
#include "mioerr.h"

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/
#define INPUT_BUF_SIZE  4096	/* choose an efficiently fread'able size */
#define BMP_ERROR_MESSAGE "Not a JPEG file: starts with 0x42 0x4d"
#define NEW_BMP_ERROR_MESSAGE "This is a BMP file, not a JPEG file. Try renaming the \".jpg\" to \".bmp\""
#define GIF_ERROR_MESSAGE "Not a JPEG file: starts with 0x47 0x49"
#define NEW_GIF_ERROR_MESSAGE "This is a GIF file, not a JPEG file. Try renaming the \".jpg\" to \".gif\""

/********************/
/* Global variables */
/********************/

/*********/
/* Types */
/*********/
typedef struct my_error_mgr 
{
  struct jpeg_error_mgr	pub;		/* "public" fields */
  jmp_buf 		setjmp_buffer;	/* for return to caller */
} *my_error_ptr;

#ifdef JUNK
typedef struct 
{
  struct jpeg_source_mgr pub;		/* public fields */

  FILE		*infile;		/* source stream */
  JOCTET	*buffer;		/* start of buffer */
  boolean	start_of_file;		/* have we gotten any data yet? */
} my_source_mgr, *my_src_ptr;
#endif
/**********************/
/* External variables */
/**********************/
// This routine is only for use by the mdiopic*.c group of files.
extern PIC	MDIOPic_MallocMDInfo (HBITMAP pmBMP1, HBITMAP pmBMP2);

/********************/
/* Static constants */
/********************/
HANDLE		stFile;
JOCTET 		stBuffer [INPUT_BUF_SIZE];
BOOL		stStartOfFile;
char		*stWholeImage;
int		stRowWidth, stDataWidth, stPadBytes, stCurrentOutputRow;
BITMAPINFOHEADER	stBitMapInfo;

/********************/
/* Static variables */
/********************/

/******************************/
/* Static callback procedures */
/******************************/

/*********************/
/* Static procedures */
/*********************/
static void		MyCreateBMPHeader (j_decompress_ptr pmCInfo);
METHODDEF(boolean)	MyFillInputBuffer (j_decompress_ptr pmCInfo);
METHODDEF(void)		MyInitSource (j_decompress_ptr pmCInfo);
METHODDEF(void) 	MyJPEGErrorHandler (j_common_ptr pmCinfo);
static void		MyOutputPixelRow (BYTE *pmBuffer, int pmWidth);
METHODDEF(void)		MySkipInputData (j_decompress_ptr pmCInfo, 
					 long pmNumBytes);
static void		MyStartBMP (j_decompress_ptr myCInfo);
METHODDEF(void)		MyTermSource (j_decompress_ptr pmCInfo);


/***********************/
/* External procedures */
/***********************/
/************************************************************************/
/* MDIOPic_LoadJPEG							*/
/************************************************************************/
BOOL	MDIOPic_LoadJPEG (const char *pmPathName, int *pmWidth,
			  int *pmHeight, PIC *pmPic)
{
    HBITMAP	myBitMap;
    
    // This struct contains the JPEG decompression parameters and pointers to
    // working space (which is allocated as needed by the JPEG library).
    struct jpeg_decompress_struct	myCInfo;
    struct jpeg_source_mgr		mySrc;
    
    // We use our private extension JPEG error handler.
    // Note that this struct must live as long as the main JPEG parameter
    // struct, to avoid dangling-pointer problems.
    struct my_error_mgr			myJpegError;
    // More stuff
    JSAMPROW	myOutputArray [1];	// Output row buffer
    int		myRowStride;		// physical row width in output buffer

    //
    // In this example we want to open the input file before doing anything else,
    // so that the setjmp() error recovery below can assume the file is open.
    // VERY IMPORTANT: use "b" option to fopen() if you are on a machine that
    // requires it in order to read binary files.
    //

    stFile = CreateFile (pmPathName, GENERIC_READ, FILE_SHARE_READ, NULL,
    			 OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (stFile == INVALID_HANDLE_VALUE)
    {
    	DWORD	myError = GetLastError ();
    	
    	if (myError == ERROR_FILE_NOT_FOUND)
    	{
	    SET_ERRMSG (E_FSYS_FILE_NOT_FOUND, "File '%s' not found", 
			pmPathName);
    	}
    	else if (myError == ERROR_PATH_NOT_FOUND)
    	{
    	    SET_ERRMSG (E_FSYS_PATH_NOT_FOUND, 
    	 	        "Path not found (couldn't find the directory "
    	 	        "that file '%s' is in)", pmPathName);
    	}
    	else
    	{
	    MIO_MoreInfo ("LoadImage: Problem opening graphics file '%s' (%d)",
		          pmPathName, GetLastError ());
	    SET_ERRNO (E_PIC_IO_ERROR);
	}		          
    	return FALSE;
    }
    
    //
    // Step 1: allocate and initialize JPEG decompression object
    //
    myOutputArray [0] = NULL;

    // We set up the normal JPEG error routines, then override error_exit.
    myCInfo.err = jpeg_std_error(&myJpegError.pub);
    myJpegError.pub.error_exit = MyJPEGErrorHandler;
    // Establish the setjmp return context for my_error_exit to use.
    if (setjmp (myJpegError.setjmp_buffer)) 
    {
        // If we get here, the JPEG code has signaled an error.
        // We need to clean up the JPEG object, close the input file, and return.
        jpeg_destroy_decompress(&myCInfo);
        
        // Error message is already set at this point

        if (myOutputArray [0] != NULL)
	{
	    free (myOutputArray [0]);
	}

    	CloseHandle (stFile);
    	return FALSE;
    }
    // Now we can initialize the JPEG decompression object.
    jpeg_create_decompress(&myCInfo);

    //
    // Step 2: specify data source (eg, a file)
    //

    //
    // The source object and input buffer are made permanent so that a series
    // of JPEG images can be read from the same file by calling jpeg_stdio_src
    // only before the first one.  (If we discarded the buffer at the end of
    // one image, we'd likely lose the start of the next one.)
    // This makes it unsafe to use this manager and a different source
    // manager serially with the same JPEG object.  Caveat programmer.
    //
    myCInfo.src = &mySrc;
    myCInfo.src -> init_source = MyInitSource;
    myCInfo.src -> fill_input_buffer = MyFillInputBuffer;
    myCInfo.src -> skip_input_data = MySkipInputData;
    myCInfo.src -> resync_to_restart = jpeg_resync_to_restart; /* use default method */
    myCInfo.src -> term_source = MyTermSource;
    myCInfo.src -> bytes_in_buffer = 0; /* forces fill_input_buffer on first read */
    myCInfo.src -> next_input_byte = NULL; /* until buffer loaded */

    //
    // Step 3: read file parameters with jpeg_read_header()
    //
    (void) jpeg_read_header (&myCInfo, TRUE);
    
    // We can ignore the return value from jpeg_read_header since
    //   (a) suspension is not possible with the stdio data source, and
    //   (b) we passed TRUE to reject a tables-only JPEG file as an error.
    // See libjpeg.doc for more info.

    //
    // Step 4: set parameters for decompression
    //
    
    // In this example, we don't need to change any of the defaults set by
    // jpeg_read_header(), so we do nothing here.

    MyStartBMP (&myCInfo);

    //
    // Step 5: Start decompressor
    //

    (void) jpeg_start_decompress (&myCInfo);
    
    // We can ignore the return value since suspension is not possible
    // with the stdio data source.

    // We may need to do some setup of our own at this point before reading
    // the data.  After jpeg_start_decompress() we have the correct scaled
    // output image dimensions available, as well as the output colormap
    // if we asked for color quantization.
    // In this example, we need to make an output work buffer of the right size.

    // JSAMPLEs per row in output buffer
    myRowStride = myCInfo.output_width * myCInfo.output_components;
    // Make a one-row-high sample array that will go away when done with image
    myOutputArray [0] = (JSAMPROW) malloc (myRowStride);

    //
    // Step 6: while (scan lines remain to be read)
    //           jpeg_read_scanlines(...);

    // Here we use the library's state variable cinfo.output_scanline as the
    // loop counter, so that we don't have to keep track ourselves.
    ///
    while (myCInfo.output_scanline < myCInfo.output_height) 
    {
        // jpeg_read_scanlines expects an array of pointers to scanlines.
        // Here the array is only one element long, but you could ask for
        // more than one scanline at a time if that's more convenient.
        (void) jpeg_read_scanlines (&myCInfo, (JSAMPARRAY) myOutputArray, 1);
        // Assume put_scanline_someplace wants a pointer and sample count.
        MyOutputPixelRow (myOutputArray [0], myCInfo.output_width);
    }

    // Step 7: Finish decompression */

    (void) jpeg_finish_decompress (&myCInfo);
    // We can ignore the return value since suspension is not possible
    // with the stdio data source.

    // Step 8: Release JPEG decompression object */

    // This is an important step since it will release a good deal of memory. */
    jpeg_destroy_decompress (&myCInfo);

    // After finish_decompress, we can close the input file.
    // Here we postpone it until after no more JPEG errors are possible,
    // so as to simplify the setjmp error logic above.  (Actually, I don't
    // think that jpeg_destroy can do an error exit, but why assume anything...)
    CloseHandle (stFile);
    free (myOutputArray [0]);

    // At this point you may want to check to see whether any corrupt-data
    // warnings occurred (test whether jerr.pub.num_warnings is nonzero).

    //
    // Convert the data to a DDB
    //
    MyCreateBMPHeader (&myCInfo);
    myBitMap = CreateDIBitmap (MIO_selectedRunWindowInfo -> offscreenDeviceContext,
    			       &stBitMapInfo, CBM_INIT, stWholeImage, 
			       (BITMAPINFO *) &stBitMapInfo, DIB_RGB_COLORS);
    if (myBitMap == NULL)
    {
	MIO_MoreInfo ("LoadImage: Unable to create device dep bitmap (%d)",
		      GetLastError ());
	SET_ERRNO (E_PIC_IO_ERROR);
    	GlobalFree (stWholeImage);
    	return FALSE;
    }
    
    *pmWidth = stBitMapInfo.biWidth;
    *pmHeight = stBitMapInfo.biHeight;
    
    GlobalFree (stWholeImage);
        
    *pmPic = MDIOPic_MallocMDInfo (myBitMap, NULL);
    
    return TRUE;
} // MDIOPic_LoadJPEG


/******************************/
/* Static callback procedures */
/******************************/


/*********************/
/* Static procedures */
/*********************/
/************************************************************************/
/* MyCreateBMPHeader							*/
/************************************************************************/
static void	MyCreateBMPHeader (j_decompress_ptr pmCInfo)
{
  /* Set unused fields of header to 0 */
  ZeroMemory (&stBitMapInfo, sizeof (BITMAPINFOHEADER));

  /* Fill the info header (Microsoft calls this a BITMAPINFOHEADER) */
  stBitMapInfo.biSize = 40;
  stBitMapInfo.biWidth = pmCInfo -> output_width;
  stBitMapInfo.biHeight = pmCInfo -> output_height;
  stBitMapInfo.biPlanes = 1;
  stBitMapInfo.biBitCount = 24;
} // MyCreateBMPHeader


/************************************************************************/
/* MyFillInputBuffer							*/
/************************************************************************/
METHODDEF(boolean)	MyFillInputBuffer (j_decompress_ptr pmCInfo)
{
    DWORD		myBytesRead;
    BOOL		mySuccess;
    
    mySuccess = ReadFile (stFile, &stBuffer, 
    			  INPUT_BUF_SIZE, &myBytesRead, NULL);
    if (!mySuccess || (myBytesRead == 0))
    {
    	if (!mySuccess)
    	{
	    MIO_MoreInfo ("LoadJPEGImage: Problem reading graphics file (%d)",
		          GetLastError ());
	}
	else
	{		        
	    MIO_MoreInfo ("LoadJPEGImage: Attempting to read beyond EOF");
	}
	
	SET_ERRNO (E_PIC_IO_ERROR);
        if (stStartOfFile)	/* Treat empty input file as fatal error */
        {
      	    ERREXIT (pmCInfo, JERR_INPUT_EMPTY);
      	}
        WARNMS(pmCInfo, JWRN_JPEG_EOF);
        
        /* Insert a fake EOI marker */
    	stBuffer [0] = (JOCTET) 0xFF;
    	stBuffer [1] = (JOCTET) JPEG_EOI;
    	myBytesRead = 2;
    }
    
    pmCInfo -> src -> next_input_byte = stBuffer;
    pmCInfo -> src -> bytes_in_buffer = myBytesRead;
    stStartOfFile = FALSE;

    return TRUE;
} // MyFillInputBuffer


/************************************************************************/
/* MyInitSource								*/
/************************************************************************/
METHODDEF(void)	MyInitSource (j_decompress_ptr pmCInfo)
{
    stStartOfFile = TRUE;
} // MyInitSource


/************************************************************************/
/* MyJPEGErrorHandler							*/
/************************************************************************/
METHODDEF(void) MyJPEGErrorHandler (j_common_ptr pmCInfo)
{
    // cinfo->err really points to a my_error_mgr struct, so coerce pointer
    my_error_ptr	myError = (my_error_ptr) pmCInfo -> err;
    char		myBuffer [JMSG_LENGTH_MAX];

    // Always display the message.
    // We could postpone this until after returning, if we chose.
    // TW

    // Create the message */
    (*pmCInfo -> err -> format_message) (pmCInfo, myBuffer);
    
    if (strcmp (myBuffer, BMP_ERROR_MESSAGE) == 0)
    {
    	strcpy (myBuffer, NEW_BMP_ERROR_MESSAGE);
    }
    else if (strcmp (myBuffer, GIF_ERROR_MESSAGE) == 0)
    {
    	strcpy (myBuffer, NEW_GIF_ERROR_MESSAGE);
    }

    SET_ERRMSG (E_PIC_IO_ERROR, myBuffer);

    // Return control to the setjmp point
    longjmp (myError -> setjmp_buffer, 1);
} // MyJPEGErrorHandler


/************************************************************************/
/* MySkipInputData							*/
/************************************************************************/
static void	MyOutputPixelRow (BYTE *pmBuffer, int pmWidth)
{
    BYTE    *myOutputPtr;
//    int	    myTriads = (pmWidth + 2) / 3;
    int	    cnt;

    /* Access next row in virtual array */
    stCurrentOutputRow--;
    myOutputPtr = &stWholeImage [stCurrentOutputRow * stRowWidth];
    
    // Transfer data.  Note destination values must be in BGR order
    // (even though Microsoft's own documents say the opposite).
    for (cnt = 0 ; cnt < pmWidth ; cnt++)
    {
	myOutputPtr [2] = *pmBuffer++;	/* can omit GETJSAMPLE() safely */
	myOutputPtr [1] = *pmBuffer++;
	myOutputPtr [0] = *pmBuffer++;
	myOutputPtr += 3;
    }
    
    /* Zero out the pad bytes. */
    for (cnt = 0 ; cnt < stPadBytes ; cnt++)
    {
	*myOutputPtr++ = 0;
    }
}


/************************************************************************/
/* MySkipInputData							*/
/************************************************************************/
METHODDEF(void)	MySkipInputData (j_decompress_ptr pmCInfo, long pmNumBytes)
{
//  my_src_ptr src = (my_src_ptr) cinfo->src;

  /* Just a dumb implementation for now.  Could use fseek() except
   * it doesn't work on pipes.  Not clear that being smart is worth
   * any trouble anyway --- large skips are infrequent.
   */
    if (pmNumBytes > 0) 
    {
        while (pmNumBytes > (long) pmCInfo -> src -> bytes_in_buffer) 
        {
            pmNumBytes -= (long) pmCInfo -> src -> bytes_in_buffer;
      	    (void) MyFillInputBuffer (pmCInfo);
            // note we assume that fill_input_buffer will never return FALSE,
       	    // so suspension need not be handled.
        }
        pmCInfo -> src -> next_input_byte += (size_t) pmNumBytes;
        pmCInfo -> src -> bytes_in_buffer -= (size_t) pmNumBytes;
    }
} // MySkipInputData


/************************************************************************/
/* MyStartBMP								*/
/************************************************************************/
static void MyStartBMP (j_decompress_ptr myCInfo)
{
    /* Calculate output image dimensions so we can allocate space */
    jpeg_calc_output_dimensions (myCInfo);
    
    /* Determine width of rows in the BMP file (padded to 4-byte boundary). */
    stRowWidth = myCInfo -> output_width * myCInfo -> output_components;
    stDataWidth = stRowWidth;
    while ((stRowWidth & 3) != 0) stRowWidth++;
    stPadBytes = (int) (stRowWidth - stDataWidth);
    
    /* Allocate space for inversion array, prepare for write pass */
    // Note that we must use GlobalAlloc rather than malloc because under
    // unspecified cases, CreateDIBitmap will fail (!) with an error code
    // of zero.  This is documented on Usenet, but not in MS documentation.
    stWholeImage = GlobalAlloc (GMEM_FIXED, 
				stRowWidth * myCInfo -> output_height);
    stCurrentOutputRow = myCInfo -> output_height;
} // MyStartBMP


/************************************************************************/
/* MyTermSource								*/
/************************************************************************/
METHODDEF(void)	MyTermSource (j_decompress_ptr pmCInfo)
{
  /* no work necessary here */
} // MyTermSource

