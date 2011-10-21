#include <stdlib.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <ctype.h>
#include <string.h>
#include <fcntl.h>
#include <io.h>
#ifdef WIN32
#include <windows.h>
#endif

#define CTRLZ	26

/* Result codes */

extern int gFileManagerIgnoreFileOperation;

#define ResultCode_Ok		0
#define ResultCode_NoRead	1
#define ResultCode_NoWrite	2
#define ResultCode_NonAscii	3
#define ResultCode_NoSpace	4
#define ResultCode_IsOpen	5

/*
** The following string definitions give the default locations for the
** support files.  They are declared as full 256-byte strings to allow
** them to be changed using adb (or equivalent).
**
** These paths must be changed to conform to the actual installation
** of the support files.
*/

extern void vaOkMsgBox(char *str, ...);

void NullFile (char **pmTextHandle, long *pmTextSize);

static int IsLegalNoPrint(c)
    int c;
{
    return (c == '\n' || c == '\r' || c == '\t');
}

void ReadOOTFile (char *pmFileName, char **pmTextHandle, long *pmTextSize, 
		  short *pmResult)
{
    struct stat statBuf;
    int fd;
    char *src, *dst, *last;

    if (gFileManagerIgnoreFileOperation)
    {
    	NullFile (pmTextHandle, pmTextSize);
	*pmResult = ResultCode_Ok;
    	return;
    }
    *pmTextHandle = NULL;

    if (stat (pmFileName, &statBuf) != 0) 
    {
	*pmResult = ResultCode_NoRead;
	return;
    }

    if ((fd=open (pmFileName, O_RDONLY|O_BINARY)) < 0 )
    {
	*pmResult = ResultCode_NoRead;
	return;
    }

    if ((*pmTextHandle = malloc (statBuf.st_size+1)) == NULL )
    {
	(void) close(fd);
	*pmResult = ResultCode_NoSpace;
	return;
    }

    if (read (fd, *pmTextHandle, statBuf.st_size ) != statBuf.st_size )
    {
	(void) close(fd);
	free (*pmTextHandle);
	*pmTextHandle = NULL;
	*pmResult = ResultCode_NoRead;
	return;
    }
    
    (void) close(fd);

    /* Find first carriage return or line feed */

    *pmResult = ResultCode_Ok;

    last = *pmTextHandle + statBuf.st_size;
    dst = src = *pmTextHandle;

    while (src < last) {
	if ( isprint(*src) || IsLegalNoPrint(*src) ) {
	    *dst = *src;
	    dst++;
	}
	else if ( *src != CTRLZ)
	    *pmResult = ResultCode_NonAscii;
	src++;
    }
    *dst++ = '\0';

    *pmTextSize = dst - *pmTextHandle;
    if ((src = realloc (*pmTextHandle, *pmTextSize) ) != NULL )
    {
        *pmTextHandle = src;
    }
}


void NullFile (char **pmTextHandle, long *pmTextSize)
{
    if (gFileManagerIgnoreFileOperation)
    {
    	*pmTextSize = 0;
    	// We hope to cause a Bus error if this handle is ever actually used.
    	*pmTextHandle = (char *) 1; 
    }
    else
    {
    	*pmTextSize = 1;
    	if ((*pmTextHandle = malloc(1)) != NULL)
    	{
	    **pmTextHandle = '\0';
	}
    }
}


// Rewritten by Tom West (Oct 2000)
void SaveFile (char *pmfileName, char *pmTextHandle, long pmTextSize, 
	       short *pmResult)
{
    HANDLE	myHandle;
    DWORD	myNumBytesWritten;

    if (gFileManagerIgnoreFileOperation)
    {
    	*pmResult = 0;
    	return;
    }
    
    myHandle = CreateFile (pmfileName, GENERIC_WRITE, 0, NULL,
    	OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (myHandle == (HANDLE) INVALID_HANDLE_VALUE)
    {
	*pmResult = ResultCode_NoWrite;
	return;
    }

    if (!SetEndOfFile (myHandle))
    {
	CloseHandle (myHandle);
	*pmResult = ResultCode_NoWrite;
	return;
    }

    if (!WriteFile (myHandle, pmTextHandle, pmTextSize - 1, 
    		    &myNumBytesWritten, NULL))
    {
	CloseHandle (myHandle);
	*pmResult = ResultCode_NoWrite;
	return;
    }
    
    if (myNumBytesWritten != (DWORD) pmTextSize - 1)
    {
	CloseHandle (myHandle);
	*pmResult = ResultCode_NoWrite;
	return;
    }
    
    if (!FlushFileBuffers (myHandle))
    {
	CloseHandle (myHandle);
	*pmResult = ResultCode_NoWrite;
	return;
    }
    
    if (!CloseHandle (myHandle))
    {
	*pmResult = ResultCode_NoWrite;
	return;
    }

    *pmResult = ResultCode_Ok;
}

/*
int
CreateFile(fileName)
    char *fileName;
{
    int fd;

    if((fd=open(fileName, O_RDWR|O_CREAT|O_EXCL|O_BINARY,
	        S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP)) < 0)
	return(ResultCode_NoWrite);

    (void) close(fd);
    return(0);
}
*/

void
RemoveFile(fileName)
    char *fileName;
{
    (void) unlink(fileName);
}


void FreeFile (char **textHandle)
{
    if (!gFileManagerIgnoreFileOperation)
    {	
    	if(*textHandle)
    	{
	    free(*textHandle);
	}
    	*textHandle = 0;
    }
}



void
HomeDirectory(user,home)
    char *user, *home;
{
    home[0] = '\0';
}


// Rewritten by Tom West (Oct 2000)
void CurrentDirectory (char pmDirectory [256])
{
    DWORD	myLen;

    myLen = GetCurrentDirectory (254, pmDirectory);
    if ((myLen == 0) || (myLen > 254))
    {
    	pmDirectory [0] = 0;
    	return;
    }

    // Add a terminating "\" if necessary
    if (pmDirectory [myLen - 1] != '\\') 
    {
	pmDirectory [myLen] = '\\';
	pmDirectory [myLen + 1] = '\0';
    }
}


void
TempDirectory(path)
    char path[256];
{
    char *pnt;
    int len;

    #ifdef __WINDOWS_386__
        pnt = getenv("WINOOTTMP");
    #else
        pnt = getenv("OOTTMP");
    #endif

    if (pnt == NULL)
	pnt = getenv("TMP");

    if (pnt == NULL)
	pnt = getenv("TEMP");

    if (pnt == NULL || strlen(pnt) > 255)
	/* No useful environment variables, so use root of current drive */
	(void) _fullpath(path, "\\", 255);
    else
	(void) strcpy(path, pnt);

    len = strlen(path);
    if (path[len-1] != '\\') {
	path[len] = '\\';
	path[len+1] = '\0';
    }
}


// Rewritten by Tom West (Oct 2000)
void IncludeDirectory (char pmDirectory [256])
{
    DWORD	myLen;
    
    myLen = GetEnvironmentVariable ("TURINGDIR", pmDirectory, 254);
    if ((myLen == 0) || (myLen > 254))
    {
    	myLen = GetCurrentDirectory (255, pmDirectory);
    	if ((myLen == 0) || (myLen > 255))
    	{
    	    pmDirectory [0] = 0;
    	    return;
    	}
    }

    // Add a terminating "\" if necessary
    if (pmDirectory [myLen - 1] != '\\') 
    {
	pmDirectory [myLen] = '\\';
	pmDirectory [myLen + 1] = '\0';
    }
}

void
UserName(user)
    char *user;
{
    user[0] = '\0';
}


char
Exists(path)
    char *path;
{
    struct stat statBuf;

    if(!path || *path == '\0')
	return 0;
    else
	return (stat(path,&statBuf) == 0);
}


// Rewritten by Tom West (Oct 2000)
char CanRead (char *pmPathName)
{
    HANDLE	myFile;

    if ((pmPathName == NULL) || (pmPathName [0] == 0))
    {
    	return FALSE;
    }
        
    // Open the file if it already exists
    myFile = CreateFile (pmPathName, GENERIC_READ, 
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
    	OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    	
    // If call failed, then return FALSE
    if (myFile == (HANDLE) INVALID_HANDLE_VALUE)
    {
    	return FALSE;
    }
    
    CloseHandle (myFile);
	
    return TRUE;
}


// Rewritten by Tom West (Oct 2000)
char CanReadWrite (char *pmPathName)
{
    HANDLE	myFile;

    if ((pmPathName == NULL) || (pmPathName [0] == 0))
    {
    	return FALSE;
    }
        
    // Open the file if it already exists
    myFile = CreateFile (pmPathName, GENERIC_READ | GENERIC_WRITE, 
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
    	OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    	
    // If call failed, then return FALSE
    if (myFile == (HANDLE) INVALID_HANDLE_VALUE)
    {
    	return FALSE;
    }
    
    CloseHandle (myFile);
	
    return TRUE;
}


// Rewritten by Tom West (Oct 2000)
char CanCD (char *pmPathName)
{
    DWORD	myAttributes;
    
    myAttributes = GetFileAttributes (pmPathName);
    if ((myAttributes == -1) || 
    	((myAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0))
    {
    	return FALSE;
    }
    
    return TRUE;    	
}


// Rewritten by Tom West (Oct 2000)
char IsRegularFile (char *pmPathName)
{
    DWORD	myAttributes;
    
    myAttributes = GetFileAttributes (pmPathName);
    if ((myAttributes == -1) || 
    	((myAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0))
    {
    	return FALSE;
    }
    
    return TRUE;    	
}


// Rewritten by Tom West (Oct 2000)
char IsSamePath (char *pmPathName0, char *pmPathName1)
{
    return (char) (_stricmp (pmPathName0, pmPathName1) == 0);
}


static void ConvertPath(path)
    char *path;
{
    char *begp  = path+3;    /* ie. past the drive letter and colon */
    char *endp  = path+strlen(path);
    char *cpath = begp;

    char *dotp, *tmpp;

    while( (dotp = strstr(cpath, "..")) != NULL ) {
	/* While there are ..'s in the path */

	if (dotp == begp) {
	    /* ..'s are at beginning of absolute path, so just remove them */
	    memmove(begp, dotp+3, endp-(dotp+2));
	}
	else {
	    /* Backup over previous directory */
	    tmpp = dotp-2;
	    while(*tmpp != '\\')
		tmpp--;

	    if (dotp == endp-2) {
		/* End of path */
		*tmpp = '\0';
		 endp = tmpp;
		 cpath = endp;
	    }
	    else {
	        memmove(tmpp+1, dotp+3, endp-(dotp+2));
		endp = path+strlen(path);
	        cpath = tmpp+1;
	    }
	}
    }
}


/*   FullPath - Converts path to minimal absolute path
 *
 */

void FullPath(path, fpath)
    char path[256];
    char fpath[256];
{
#ifdef WIN32
    char *dummy;
    GetFullPathName (path, 255, fpath, &dummy);
#else
    int len = strlen(path);


    if (len > 3 && path[len-1] == '\\') {
        char tpath[256];

	strcpy(tpath, path);
        tpath[len-1] = '\0';
        _fullpath(fpath, tpath, 255);
        fpath[strlen(fpath)] = '\\';
        fpath[strlen(fpath)+1] = '\0';
    }
    else
        _fullpath(fpath, path, 255);

    if (strstr(fpath, "..") != NULL)
	ConvertPath(fpath);
#endif
}


struct FID {
    char n[256];
    long t;
};

void
GetFID( file, fid_p )
    char *file;
    struct FID *fid_p;
{
    struct stat statBuf;

    if( file == NULL || (*file) == '\0' || stat( file, &statBuf ) < 0 ) {
	fid_p->n[0] = '\0';
	fid_p->t = 0;
    }
    else {
#ifdef WIN32
	char *dummy;
	GetFullPathName (file, 255, fid_p->n, &dummy);
#else
        _fullpath(fid_p->n, file, 255);
        if (strstr(fid_p->n, "..") != NULL)
	    ConvertPath(fid_p->n);
#endif
    	fid_p->t = statBuf.st_mtime;
    }
}

