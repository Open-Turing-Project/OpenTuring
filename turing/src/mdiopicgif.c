/****************/
/* mdiopicgif.c */
/****************/

/*******************/
/* System includes */
/*******************/
#include <windows.h>
#include <io.h>
#include <crtdbg.h>
//#include <setjmp.h>

/****************/
/* Self include */
/****************/
#include "mdiopicgif.h"

/******************/
/* Other includes */
/******************/
#include "libungif\\gif_lib.h"

#include "mio.h"
#include "mioerr.h"
#include "miorgb.h"

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

/**********************/
/* External variables */
/**********************/
// This routine is only for use by the mdiopic*.c group of files.
extern PIC	MDIOPic_MallocMDInfo (HBITMAP pmBMP1, HBITMAP pmBMP2);
extern int	errno;

/********************/
/* Static constants */
/********************/
// The way Interlaced image should be read - offsets and jumps...
static int	stInterlacedOffset [] = { 0, 4, 2, 1 };
static int	stInterlacedJumps[] = { 8, 8, 4, 2 };

/********************/
/* Static variables */
/********************/

/******************************/
/* Static callback procedures */
/******************************/
static int	MyInputFunction (GifFileType* pmGif, GifByteType* pmBytes, 
				 int pmSize);

/*********************/
/* Static procedures */
/*********************/
static void	MyConvertGIFError (const char *pmPathName);
static BOOL	MyReadGIF (const char *pmPathName, GifFileType *pmGIF, 
			   int pmMaxImages, int *pmNumImages, 
			   HBITMAP *pmBitMap, int *pmTransparentColour, 
			   int *pmDelay);
static HBITMAP	MyReadSingleGIFImage (const char *pmPathName,
				      GifFileType *pmGIF, int *pmWidth, 
				      int *pmHeight, BOOL pmIgnoreImage);


/***********************/
/* External procedures */
/***********************/
/************************************************************************/
/* MDIOPic_GetGIFFrames							*/
/************************************************************************/
int	MDIOPic_GetGIFFrames (const char *pmPathName)
{
    HANDLE		myFile;
    GifFileType		*myGIF;
    int			myNumFrames;

    myFile = CreateFile (pmPathName, GENERIC_READ, FILE_SHARE_READ, NULL,
    			 OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (myFile == INVALID_HANDLE_VALUE)
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
    	return 0;
    }

    myGIF = DGifOpen (myFile, MyInputFunction);
    if (myGIF == NULL)
    {
	// An error occurred - return now
	MyConvertGIFError (pmPathName);
	CloseHandle (myFile);
	return 0;
    }

    if (!MyReadGIF (pmPathName, myGIF, 0, &myNumFrames, NULL, 0, NULL))
    {
	// Error message set in MyReadGIF - return now
	CloseHandle (myFile);
	return 0;
    }

    CloseHandle (myFile);

    DGifCloseFile (myGIF);

    return myNumFrames;
} // MDIOPic_GetGIFFrames


/************************************************************************/
/* MDIOPic_LoadGIF							*/
/************************************************************************/
BOOL	MDIOPic_LoadGIF (const char *pmPathName, int *pmWidth,
			 int *pmHeight, int *pmTransColour, PIC *pmPic)
{
    HANDLE		myFile;
    GifFileType		*myGIF;
    HBITMAP		myBitMap;
    
    myFile = CreateFile (pmPathName, GENERIC_READ, FILE_SHARE_READ, NULL,
    			 OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (myFile == INVALID_HANDLE_VALUE)
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

    myGIF = DGifOpen (myFile, MyInputFunction);
    if (myGIF == NULL)
    {
	// An error occurred - return now
	MyConvertGIFError (pmPathName);
	CloseHandle (myFile);
	return FALSE;
    }

    if (!MyReadGIF (pmPathName, myGIF, 1, NULL, &myBitMap, pmTransColour, NULL))
    {
	// Error message set in MyReadGIF - return now
	CloseHandle (myFile);
	return FALSE;
    }

    CloseHandle (myFile);

    *pmWidth = myGIF -> SWidth;
    *pmHeight = myGIF -> SHeight;

    DGifCloseFile (myGIF);

    *pmPic = MDIOPic_MallocMDInfo (myBitMap, NULL);

    return TRUE;
} // MDIOPic_LoadGIF


/************************************************************************/
/* MDIOPic_LoadGIFFrames						*/
/************************************************************************/
BOOL	MDIOPic_LoadGIFFrames (const char *pmPathName, int *pmWidth,
			       int *pmHeight, int *pmTransColour, 
			       PIC *pmMDPicInfo, int pmNumFrames,
			       int *pmDelay)
{
    HANDLE		myFile;
    GifFileType		*myGIF;
    HBITMAP		*myBitMaps;
    int			myNumFrames = 0;
    int			cnt;

    myFile = CreateFile (pmPathName, GENERIC_READ, FILE_SHARE_READ, NULL,
    			 OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (myFile == INVALID_HANDLE_VALUE)
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

    myGIF = DGifOpen (myFile, MyInputFunction);
    if (myGIF == NULL)
    {
	// An error occurred - return now
	MyConvertGIFError (pmPathName);
	CloseHandle (myFile);
	return FALSE;
    }

    myBitMaps = (HBITMAP *) malloc (sizeof (HBITMAP) * pmNumFrames);

    if (!MyReadGIF (pmPathName, myGIF, pmNumFrames, &myNumFrames, myBitMaps, pmTransColour, pmDelay))
    {
	// Error message set in MyReadGIF - return now
	CloseHandle (myFile);
	return FALSE;
    }

    CloseHandle (myFile);

    *pmWidth = myGIF -> SWidth;
    *pmHeight = myGIF -> SHeight;

    DGifCloseFile (myGIF);

    for (cnt = 0 ; cnt < myNumFrames ; cnt++)
    {
	pmMDPicInfo [cnt] = MDIOPic_MallocMDInfo (myBitMaps [cnt], NULL);
    }

    free (myBitMaps);

    return TRUE;
} // MDIOPic_LoadGIFFrames


/******************************/
/* Static callback procedures */
/******************************/
static int	MyInputFunction (GifFileType *pmGif, GifByteType *pmBytes, 
				 int pmSize)
{
    HANDLE	myFile;
    DWORD	myBytesRead;
    BOOL	mySuccess;
    
    myFile = (HANDLE) pmGif -> UserData;

    mySuccess = ReadFile (myFile, pmBytes, pmSize, &myBytesRead, NULL);
    if (!mySuccess || (myBytesRead == 0))
    {
    	if (!mySuccess)
    	{
	    MIO_MoreInfo ("LoadGIFImage: Problem reading graphics file (%d)",
		          GetLastError ());
	}
	else
	{		        
	    MIO_MoreInfo ("LoadGIFImage: Attempting to read beyond EOF");
	}
	
	return 0;
    }
    
    return myBytesRead;
} // MyInputFunction


/*********************/
/* Static procedures */
/*********************/
/************************************************************************/
/* MyConvertGIFError							*/
/************************************************************************/
static void	MyConvertGIFError (const char *pmPathName)
{
    int	myError = GifLastError ();

    switch (myError) 
    {
	case E_GIF_ERR_OPEN_FAILED:
    	    SET_ERRMSG (E_PIC_IO_ERROR, "Failed to open '%s' for writing", pmPathName);
	    break;
	case E_GIF_ERR_WRITE_FAILED:
    	    SET_ERRMSG (E_PIC_IO_ERROR, "Failed to write to '%s'", pmPathName);
	    break;
	case E_GIF_ERR_HAS_SCRN_DSCR:
	    MIO_MoreInfo ("SaveImage: Screen Descriptor already been set");
	    SET_ERRNO (E_PIC_IO_ERROR);
	    break;
	case E_GIF_ERR_HAS_IMAG_DSCR:
	    MIO_MoreInfo ("SaveImage: Image Descriptor is still active");
	    SET_ERRNO (E_PIC_IO_ERROR);
	    break;
	case E_GIF_ERR_NO_COLOR_MAP:
	    MIO_MoreInfo ("SaveImage: Image has no colour map");
	    SET_ERRNO (E_PIC_IO_ERROR);
	    break;
	case E_GIF_ERR_DATA_TOO_BIG:
	    MIO_MoreInfo ("SaveImage: Bad GIF File. #Pixels bigger than Width * Height");
	    SET_ERRNO (E_PIC_IO_ERROR);
	    break;
	case E_GIF_ERR_NOT_ENOUGH_MEM:
    	    SET_ERRMSG (E_PIC_IO_ERROR, "Unable to allocate required memory to write '%s'", pmPathName);
	    break;
	case E_GIF_ERR_DISK_IS_FULL:
    	    SET_ERRMSG (E_PIC_IO_ERROR, "Write of '%s' failed (disk full?)", pmPathName);
	    break;
	case E_GIF_ERR_CLOSE_FAILED:
    	    SET_ERRMSG (E_PIC_IO_ERROR, "Failed to close '%s'", pmPathName);
	    break;
	case E_GIF_ERR_NOT_WRITEABLE:
    	    SET_ERRMSG (E_PIC_IO_ERROR, "'%s' was not opened for write", pmPathName);
	    break;
	case D_GIF_ERR_OPEN_FAILED:
    	    SET_ERRMSG (E_PIC_IO_ERROR, "Failed to open '%s' for reading", pmPathName);
	    break;
	case D_GIF_ERR_READ_FAILED:
    	    SET_ERRMSG (E_PIC_IO_ERROR, "Failed to read from '%s'", pmPathName);
	    break;
	case D_GIF_ERR_NOT_GIF_FILE:
    	    SET_ERRMSG (E_PIC_IO_ERROR, "'%s' is NOT GIF file", pmPathName);
	    break;
	case D_GIF_ERR_NO_SCRN_DSCR:
	    MIO_MoreInfo ("LoadImage: No Screen Descriptor detected in '%s', pmPathName");
    	    SET_ERRMSG (E_PIC_IO_ERROR, "Bad GIF file '%s'", pmPathName);
	    break;
	case D_GIF_ERR_NO_IMAG_DSCR:
	    MIO_MoreInfo ("LoadImage: No Image Descriptor detected in '%s'", pmPathName);
    	    SET_ERRMSG (E_PIC_IO_ERROR, "Bad GIF File '%s'", pmPathName);
	    break;
	case D_GIF_ERR_NO_COLOR_MAP:
	    MIO_MoreInfo ("LoadImage: No colour map in '%s'", pmPathName);
    	    SET_ERRMSG (E_PIC_IO_ERROR, "Bad GIF File '%s'", pmPathName);
	    break;
	case D_GIF_ERR_WRONG_RECORD:
	    MIO_MoreInfo ("LoadImage: Wrong record type detected in '%s'", pmPathName);
    	    SET_ERRMSG (E_PIC_IO_ERROR, "Bad GIF File '%s'", pmPathName);
	    break;
	case D_GIF_ERR_DATA_TOO_BIG:
	    MIO_MoreInfo ("LoadImage: #Pixels bigger than Width * Height in '%s'", pmPathName);
    	    SET_ERRMSG (E_PIC_IO_ERROR, "Bad GIF File '%s'", pmPathName);
	    break;
	case D_GIF_ERR_NOT_ENOUGH_MEM:
    	    SET_ERRMSG (E_PIC_IO_ERROR, "Unable to allocate required memory to load '%s'", pmPathName);
	    break;
	case D_GIF_ERR_CLOSE_FAILED:
    	    SET_ERRMSG (E_PIC_IO_ERROR, "Failed to close '%s'", pmPathName);
	    break;
	case D_GIF_ERR_NOT_READABLE:
	    MIO_MoreInfo ("LoadImage: '%s' not open for read", pmPathName);
	    SET_ERRNO (E_PIC_IO_ERROR);
	    break;
	case D_GIF_ERR_IMAGE_DEFECT:
	    MIO_MoreInfo ("LoadImage: Image is defective, decoding of '%s' aborted", pmPathName);
    	    SET_ERRMSG (E_PIC_IO_ERROR, "Bad GIF File '%s'", pmPathName);
	    break;
	case D_GIF_ERR_EOF_TOO_SOON:
	    MIO_MoreInfo ("LoadImage: Image EOF detected before image complete. '%s' has been truncated", pmPathName);
    	    SET_ERRMSG (E_PIC_IO_ERROR, "Bad GIF File '%s'", pmPathName);
	    break;
	default:
	    MIO_MoreInfo ("Load/SaveImage: Unknown GIF-LIB error %d in '%s'", myError , pmPathName);
    	    SET_ERRNO (E_PIC_IO_ERROR);
	    break;
    }
} // MyConvertGIFError


/************************************************************************/
/* MyReadGIF								*/
/************************************************************************/
static BOOL	MyReadGIF (const char *pmPathName, GifFileType *pmGIF, 
			   int pmMaxImages, int *pmNumImages, HBITMAP *pmBitMap,
			   int *pmTransparentColour, int *pmDelay)
{
    HBITMAP		myBitMap;
    GifRecordType	myRecordType;
    GifByteType		*myExtension;
    int			myNumImages = 0;
    int			myImageWidth, myImageHeight, myExtensionCode;

    // Scan the content of the GIF file and load the image(s) in:
    do 
    {
	if (DGifGetRecordType (pmGIF, &myRecordType) == GIF_ERROR) 
	{
	    // Failure - return now
	    MyConvertGIFError (pmPathName);
	    return FALSE;
	}
	switch (myRecordType) 
	{
	    case IMAGE_DESC_RECORD_TYPE:
		myBitMap = MyReadSingleGIFImage (pmPathName, pmGIF, 
				&myImageWidth, &myImageHeight, 
				myNumImages < pmMaxImages);
		if (myNumImages < pmMaxImages)
		{
		    if (myBitMap == NULL)
		    {
			// Failure - return now
			MyConvertGIFError (pmPathName);
			return FALSE;
		    }
		    pmBitMap [myNumImages] = myBitMap;
		}
		myNumImages++;
		break;
	    case EXTENSION_RECORD_TYPE:
		// Check to see if this is the transparency block
		if (DGifGetExtension (pmGIF, &myExtensionCode, &myExtension) 
								== GIF_ERROR)
		{
		    // Failure - return now
		    MyConvertGIFError (pmPathName);
		    return FALSE;
		}

		if ((myExtensionCode == 0xF9) && (myExtension [0] == 4))
		{
		    if (pmTransparentColour != NULL)
		    {
			*pmTransparentColour = -1;
		    }
		    if ((myExtension [1] & 1) == 1)
		    {
			if (pmGIF -> SColorMap != NULL)
			{
			    int		myGIFColour;
			    double	myRed, myGreen, myBlue;

			    myGIFColour = myExtension [4];
			    myRed = pmGIF -> SColorMap -> Colors [myGIFColour].Red / 255.0;
			    myGreen = pmGIF -> SColorMap -> Colors [myGIFColour].Green / 255.0;
			    myBlue = pmGIF -> SColorMap -> Colors [myGIFColour].Blue / 255.0;
			    if (pmTransparentColour != NULL)
			    {
				*pmTransparentColour = MIORGB_AddColour (myRed, myGreen, myBlue);
			    }
			}
		    }
		    if (pmDelay != NULL)
		    {
			*pmDelay = (* (short *) &myExtension [2]) * 10;
		    }
		}

		while (myExtension != NULL) 
		{
		    if (DGifGetExtensionNext (pmGIF, &myExtension) == GIF_ERROR) 
		    {
			// Failure - return now
			MyConvertGIFError (pmPathName);
			return FALSE;
		    }
		}

		break;
	    case TERMINATE_RECORD_TYPE:
		break;
	    default:
		break;
	}
    }
    while (myRecordType != TERMINATE_RECORD_TYPE);

    if (pmNumImages != NULL)
    {
	*pmNumImages = myNumImages;
    }

    return TRUE;
} // MyReadGIF


/************************************************************************/
/* MyReadSingleGIFImage							*/
/************************************************************************/
static HBITMAP	MyReadSingleGIFImage (const char *pmPathName, 
				      GifFileType *pmGIF, int *pmWidth, 
				      int *pmHeight, BOOL pmUseImage)
{
    int			myRow, myCol, myWidth, myHeight;
    int			myRowWidth, myPass, myLine;
    char		*myScreenBuffer = NULL;
    BITMAPINFO		*myBitMapInfo;
    ColorMapObject	*myColourMap;
    int			myBitMapInfoSize;
    int			myNumColours, cnt;
    HBITMAP		myBitMap;
    int			myResult;

    if (DGifGetImageDesc (pmGIF) == GIF_ERROR) 
    {
	// Failure - return now
	MyConvertGIFError (pmPathName);
	return NULL;
    }

    myRowWidth = ((pmGIF -> SWidth + 3) / 4) * 4;
    myRow = pmGIF -> Image.Top; // Image Position relative to Screen
    myCol = pmGIF -> Image.Left;
    myWidth = pmGIF -> Image.Width;
    myHeight = pmGIF -> Image.Height;

    if (pmUseImage)
    {
	myScreenBuffer = GlobalAlloc (GMEM_FIXED, myRowWidth * pmGIF -> SHeight);
    }
    else
    {
	myScreenBuffer = GlobalAlloc (GMEM_FIXED, myRowWidth);
    }

    if (myScreenBuffer == NULL)
    {
	// Failure - return now
	SET_ERRMSG (E_PIC_IO_ERROR, "Unable to allocate required memory to load '%s'", pmPathName);
	return NULL;
    }

    if (pmUseImage)
    {
	// If the image doesn't exactly fit the GIF's screen size, set 
	// the background color on the entire buffer.
	if ((myRow != 0) || (myCol != 0) || (myWidth < pmGIF -> SWidth) ||
	    (myHeight < pmGIF -> SHeight))
	{
	    memset (myScreenBuffer, pmGIF -> SBackGroundColor, 
		myRowWidth * pmGIF -> SHeight);
	}
    }

    if ((myWidth > pmGIF -> SWidth) || (myHeight > pmGIF -> SHeight))
    {
	// Failure - return now
	MIO_MoreInfo ("LoadImage: width or height of image is larger than GIF screen in '%s', pmPathName");
	SET_ERRMSG (E_PIC_IO_ERROR, "Bad GIF file '%s'", pmPathName);
	if (myScreenBuffer != NULL)
	{
	    GlobalFree (myScreenBuffer);
	}
	return NULL;
    }

    if (pmGIF -> Image.Interlace) 
    {
	// Need to perform 4 passes on the images:
	for (myPass = 0; myPass < 4; myPass++)
	{
	    for (myLine = myRow + stInterlacedOffset [myPass]; 
		myLine < myRow + myHeight;
		myLine += stInterlacedJumps [myPass]) 
	    {
	        if (pmUseImage)
	        {
		    myResult = DGifGetLine (pmGIF, &myScreenBuffer [((myRow - 1) - myLine) * myRowWidth + myCol], myWidth);
		}
		else
		{
		    myResult = DGifGetLine (pmGIF, myScreenBuffer, myWidth);
		}

		if (myResult == GIF_ERROR) 
		{
		    // Failure - return now
		    MyConvertGIFError (pmPathName);
		    if (myScreenBuffer != NULL)
		    {
			GlobalFree (myScreenBuffer);
		    }
        	    return NULL;
		}
	    }
	}
    }
    else 
    {
	myRow += myHeight - 1;
	for (myLine = 0; myLine < myHeight; myLine++) 
	{
	    if (pmUseImage)
	    {
		myResult = DGifGetLine (pmGIF, &myScreenBuffer [myRow-- * myRowWidth + myCol], myWidth);
	    }
	    else
	    {
		myResult = DGifGetLine (pmGIF, myScreenBuffer, myWidth);
	    }

	    if (myResult == GIF_ERROR) 
	    {
		// Failure - return now
		MyConvertGIFError (pmPathName);
		if (myScreenBuffer != NULL)
		{
		    GlobalFree (myScreenBuffer);
		}
        	return NULL;
	    }
	}
    }

    // If we're skipping over this image, return now.
    if (!pmUseImage)
    {
	GlobalFree (myScreenBuffer);
	return NULL;
    }

    //
    // We've now filled the buffer, now we create the bitmap
    //

    // Get the color map for this image
    if (pmGIF -> Image.ColorMap != NULL)
    {
	myColourMap = pmGIF -> Image.ColorMap;
    }
    else
    {
	myColourMap = pmGIF -> SColorMap;
    }

    if (myColourMap == NULL)
    {
	myNumColours = 0;
    }
    else
    {
	myNumColours = myColourMap -> ColorCount;
    }

    myBitMapInfoSize = sizeof (BITMAPINFOHEADER) + 4 * myNumColours;
    myBitMapInfo = (BITMAPINFO *) calloc (1, myBitMapInfoSize);

    // Set unused fields of header to 0 */
    ZeroMemory (myBitMapInfo, sizeof (BITMAPINFOHEADER));
    
    // Fill the BITMAPINFOHEADER
    myBitMapInfo -> bmiHeader.biSize = 40;
    myBitMapInfo -> bmiHeader.biWidth = pmGIF -> Image.Width;
    myBitMapInfo -> bmiHeader.biHeight = pmGIF -> Image.Height;
    myBitMapInfo -> bmiHeader.biPlanes = 1;
    myBitMapInfo -> bmiHeader.biBitCount = 8;
    myBitMapInfo -> bmiHeader.biCompression = BI_RGB;
    myBitMapInfo -> bmiHeader.biClrUsed = myNumColours;
    for (cnt = 0 ; cnt < myNumColours ; cnt++)
    {
	myBitMapInfo -> bmiColors [cnt].rgbRed = myColourMap -> Colors [cnt].Red;
	myBitMapInfo -> bmiColors [cnt].rgbBlue = myColourMap -> Colors [cnt].Blue;
	myBitMapInfo -> bmiColors [cnt].rgbGreen = myColourMap -> Colors [cnt].Green;
    }

    myBitMap = CreateDIBitmap (MIO_selectedRunWindowInfo -> offscreenDeviceContext,
    			       (BITMAPINFOHEADER *) myBitMapInfo, CBM_INIT, myScreenBuffer, 
			       myBitMapInfo, DIB_RGB_COLORS);
    if (myBitMap == NULL)
    {
	MIO_MoreInfo ("LoadImage: Unable to create device dep bitmap (%d)",
		      GetLastError ());
	SET_ERRNO (E_PIC_IO_ERROR);

    	free (myBitMapInfo);
	GlobalFree (myScreenBuffer);

    	return NULL;
    }
    
    *pmWidth = myBitMapInfo -> bmiHeader.biWidth;
    *pmHeight = myBitMapInfo -> bmiHeader.biHeight;
    
    free (myBitMapInfo);
    GlobalFree (myScreenBuffer);

    return myBitMap;
} // MyReadSingleGIFImage


