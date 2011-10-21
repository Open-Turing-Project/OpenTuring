#include "cinterface"
static struct {
    TLint4	dummy;
    char	dummy2[152];
} TLFTAB =
    { 3,
    {'/', 'e', 'x', 'p', 'o', 'r', 't', '/', 'h', 'o', 'm', 'e', '/', 't', 'u', 'r', 'i', 'n', 'g', '/', 'o', 'o', 't', '/', 's', 'r', 'c', '/', 'w', 'i', 'n', '/', 'f', 'i', 'l', 'e', 'm', 'a', 'n', '/', 'f', 'i', 'l', 'e', 'm', 'a', 'n', '.', 'b', 'd', '\0',
    '/', 'e', 'x', 'p', 'o', 'r', 't', '/', 'h', 'o', 'm', 'e', '/', 't', 'u', 'r', 'i', 'n', 'g', '/', 'o', 'o', 't', '/', 's', 'r', 'c', '/', 'w', 'i', 'n', '/', 'f', 'i', 'l', 'e', 'm', 'a', 'n', '/', 'f', 'i', 'l', 'e', 'm', 'a', 'n', '.', 's', 't', '\0',
    '/', 'e', 'x', 'p', 'o', 'r', 't', '/', 'h', 'o', 'm', 'e', '/', 't', 'u', 'r', 'i', 'n', 'g', '/', 'o', 'o', 't', '/', 's', 'r', 'c', '/', 'w', 'i', 'n', '/', 'f', 'i', 'l', 'e', 'm', 'a', 'n', '/', 'f', 'i', 'l', 'e', 'i', 'o', '.', 'i', 'n', '\0'
    }};
typedef	TLnat2	FileManager_FileNoType;
typedef	TLnat4	FileManager_FileTimeStamp;
typedef	TLint2	FileManager_ResultCode;

void FileManager_OpenNamedHandle ();

void FileManager_OpenUnnamedHandle ();

void FileManager_OpenFileHandle ();

void FileManager_SetModified ();

void FileManager_WriteFile ();

void FileManager_WriteBackup ();

void FileManager_WriteTemp ();

void FileManager_RmTemp ();

void FileManager_CloseFileHandle ();

void FileManager_OpenNamedText ();

void FileManager_OpenFileText ();

void FileManager_CloseFileText ();

void FileManager_CloseAllText ();

void FileManager_ChangeDirectory ();

void FileManager_GetDirectory ();

void FileManager_ChangeExecDirectory ();

void FileManager_GetExecDirectory ();

void FileManager_ShortName ();

void FileManager_PathName ();

void FileManager_ExecPathName ();

void FileManager_PrefixedName ();

void FileManager_ExtendName ();

void FileManager_SetLongFileNameFlag ();

void FileManager_FileName ();

TLboolean FileManager_IsUnnamed ();

TLboolean FileManager_IsOnDisk ();

TLboolean FileManager_IsSameFile ();

TLboolean FileManager_IsReadOK ();

TLboolean FileManager_IsWriteOK ();

TLboolean FileManager_IsOlder ();

TLboolean FileManager_FileExists ();

void FileManager_AddReference ();

void FileManager_DeleteReference ();

void FileManager_ReadTimeStamp ();

void FileManager_FlushUnreferencedFiles ();

void FileManager_CheckReferencedFiles ();

void FileManager_RmFile ();

void FileManager_SetDefaultInclude ();

void FileManager_SetHomeDir ();

void FileManager_SetJobDir ();

void FileManager_AssertTextClosed ();

void FileManager_Dump ();

extern void ReadOOTFile ();

extern void NullFile ();

extern void SaveFile ();

extern TLint4 CreateFile ();

extern void RemoveFile ();

extern void FreeFile ();

extern TLboolean Exists ();

extern TLboolean CanRead ();

extern TLboolean CanReadWrite ();

extern TLboolean CanCD ();

extern TLboolean IsRegularFile ();

extern void FullPath ();
struct	FileManager_FID {
    TLstring	n;
    TLint4	t;
};
struct FileManager_FID	FileManager_nullFID = 
    {"", 0};

extern void GetFID ();

extern TLint4 getpid ();

extern void unlink ();

extern void system ();
extern TLstring	DefaultInclude;

extern void CurrentDirectory ();

extern void TempDirectory ();

extern void IncludeDirectory ();

extern TLboolean IsSamePath ();
static TLstring	FileManager_wd;
static TLstring	FileManager_badwd;
static TLstring	FileManager_ed;
static TLstring	FileManager_baded;
static TLboolean	FileManager_longFileNames;
typedef	TLnat4	FileManager_TextNoType;
struct	FileManager_TextDef {
    TLnat4	refCount;
    TLnat4	openCount;
    TLnat4	fileRefs;
    FileManager_FileTimeStamp	timeStamp;
    struct FileManager_FID	fid;
    TLaddressint	textPtr;
    TLnat4	textSize;
};
struct	FileManager_FileDef {
    TLboolean	allocated;
    TLnat4	handleOpen;
    TLnat4	textOpen;
    TLnat4	refCount;
    FileManager_FileTimeStamp	lastStamp;
    TLstring	name;
    struct FileManager_FID	did;
    TLstring	uname;
    FileManager_TextNoType	textNo;
};
typedef	struct FileManager_TextDef	FileManager___x451[500];
static FileManager___x451	FileManager_textDescriptors;
typedef	struct FileManager_FileDef	FileManager___x452[500];
static FileManager___x452	FileManager_fileDescriptors;

extern void OkMsgBox ();
static TLstring	FileManager_TempDir;
typedef	TLstring	FileManager___x454[5];
static FileManager___x454	FileManager_prefix = 
    {"oot", "home", "job", "help", "tmp"};
typedef	TLstring	FileManager___x458[5];
static FileManager___x458	FileManager_Dirs;
static TLint4	FileManager_unnamedPrefixLen;
typedef	TLboolean	FileManager___x463[20];
static FileManager___x463	FileManager_unnamedFree;
static TLint4	FileManager_nextUnnamed;

static void FileManager_getUnName (uname)
TLstring	uname;
{
    TLint4	i;
    TLSTKCHKSLF(100155);
    TLSETF();
    TLSETL(100155);
    i = 1;
    TLSETL(100157);
    for(;;) {
	TLSETL(100157);
	if ((i > 20) || (FileManager_unnamedFree[TLINRANGELOW(i, 1, 20, 1)])) {
	    break;
	};
	TLSETL(100158);
	i += 1;
    };
    TLSETL(100161);
    if (i > 20) {
	TLSETL(100162);
	i = FileManager_nextUnnamed;
	TLINCL();
	FileManager_nextUnnamed += 1;
    };
    TLSETL(100166);
    TLSTRASS(255, uname, "unnamed");
    TLINCL();
    if (i > 1) {
	TLSETL(100168);
	{
	    TLstring	__x467;
	    TL_TLS_TLSVIS((TLint4) i, (TLint4) 0, (TLint4) 10, __x467);
	    {
		TLstring	__x466;
		TL_TLS_TLSCAT("#", __x467, __x466);
		TLSTRCATASS(uname, __x466, 255);
	    };
	};
    };
    TLSETL(100171);
    if (i <= 20) {
	TLSETL(100172);
	FileManager_unnamedFree[TLINRANGELOW(i, 1, 20, 1)] = 0;
    };
    TLRESTORELF();
}

static void FileManager_freeUnName (uname)
TLstring	uname;
{
    TLint4	ulen;
    TLint4	i;
    TLSTKCHKSLF(100180);
    TLSETF();
    TLSETL(100180);
    ulen = TL_TLS_TLSLEN(uname);
    TLINCL();
    TLASSERT (ulen >= FileManager_unnamedPrefixLen);
    TLSETL(100184);
    if (ulen == FileManager_unnamedPrefixLen) {
	TLSETL(100185);
	i = 1;
    } else {
	TLSETL(100187);
	{
	    TLstring	__x468;
	    TL_TLS_TLSBXX(__x468, (TLint4) ulen, (TLint4) (FileManager_unnamedPrefixLen + 2), uname);
	    i = TL_TLS_TLSVSI(__x468, (TLint4) 10);
	};
    };
    TLSETL(100190);
    if (i <= 20) {
	TLSETL(100191);
	TLASSERT (!(FileManager_unnamedFree[TLINRANGELOW(i, 1, 20, 1)]));
	TLINCL();
	FileManager_unnamedFree[TLINRANGELOW(i, 1, 20, 1)] = 1;
    };
    TLSETL(100195);
    TLSTRASS(255, uname, "unnamed#?");
    TLRESTORELF();
}

static void FileManager_allocateFile (fileNo)
FileManager_FileNoType	*fileNo;
{
    TLSTKCHKSLF(100205);
    TLSETF();
    TLSETL(100205);
    {
	register TLint4	i;
	for (i = 1; i <= 500; i++) {
	    TLBINDREG((*file), struct FileManager_FileDef);
	    TLSETL(100206);
	    file = &(FileManager_fileDescriptors[TLINRANGELOW(i, 1, 500, 1)]);
	    TLINCL();
	    if (!((*file).allocated)) {
		TLSETL(100209);
		(*file).handleOpen = 0;
		TLINCL();
		(*file).textOpen = 0;
		TLINCL();
		(*file).refCount = 0;
		TLINCL();
		TLSTRASS(255, (*file).name, "");
		TLINCL();
		TLSTRASS(255, (*file).uname, "");
		TLINCL();
		TLSTRCTASS((*file).did, FileManager_nullFID, struct FileManager_FID);
		TLINCL();
		(*file).textNo = 0;
		TLINCL();
		(*fileNo) = i;
		TLSETL(100218);
		TLRESTORELF();
		return;
	    };
	};
    };
    TLSETL(100220);
    (*fileNo) = 0;
    TLRESTORELF();
}

static void FileManager_allocateText (textNo)
FileManager_TextNoType	*textNo;
{
    TLSTKCHKSLF(100229);
    TLSETF();
    TLSETL(100229);
    {
	register TLint4	i;
	for (i = 1; i <= 500; i++) {
	    TLBINDREG((*text), struct FileManager_TextDef);
	    TLSETL(100230);
	    text = &(FileManager_textDescriptors[TLINRANGELOW(i, 1, 500, 1)]);
	    TLINCL();
	    if (((*text).refCount) == 0) {
		TLSETL(100233);
		(*text).openCount = 0;
		TLINCL();
		(*text).fileRefs = 0;
		TLINCL();
		TLSTRCTASS((*text).fid, FileManager_nullFID, struct FileManager_FID);
		TLINCL();
		(*text).textPtr = (TLaddressint) 0;
		TLINCL();
		(*text).textSize = 0;
		TLINCL();
		(*textNo) = i;
		TLSETL(100240);
		TLRESTORELF();
		return;
	    };
	};
    };
    TLSETL(100242);
    (*textNo) = 0;
    TLRESTORELF();
}

static TLboolean FileManager_sameFile (fid1, fid2)
struct FileManager_FID	*fid1;
struct FileManager_FID	*fid2;
{
    TLSTKCHKSLF(100250);
    TLSETF();
    TLSETL(100250);
    TLRESTORELF();
    return (IsSamePath((*fid1).n, (*fid2).n));
    TLRESTORELF();
    /* NOTREACHED */
    TLFCNRESULTABORT;
}

static void FileManager_lastComponent (f, __x263)
TLstring	f;
TLstring	__x263;
{
    TLint4	i;
    TLSTKCHKSLF(100264);
    TLSETF();
    TLSETL(100264);
    i = TL_TLS_TLSLEN(f);
    TLSETL(100266);
    for(;;) {
	TLSETL(100266);
	{
	    TLchar	__x471[2];
	    {
		TLchar	__x470[2];
		if (((i == 0) || ((TL_TLS_TLSBX(__x470, (TLint4) i, f), strcmp(__x470, "\\") == 0))) || ((TL_TLS_TLSBX(__x471, (TLint4) i, f), strcmp(__x471, "/") == 0))) {
		    break;
		};
	    };
	};
	TLSETL(100267);
	i -= 1;
    };
    TLSETL(100269);
    {
	{
	    TLstring	__x472;
	    TL_TLS_TLSBXS(__x472, (TLint4) 0, (TLint4) (i + 1), f);
	    TLSTRASS(255, __x263, __x472);
	};
	TLRESTORELF();
	return;
    };
    TLRESTORELF();
    /* NOTREACHED */
    TLFCNRESULTABORT;
}

static TLboolean FileManager_isUNCPath (f)
TLstring	f;
{
    TLSTKCHKSLF(100274);
    TLSETF();
    TLSETL(100274);
    {
	TLchar	__x476[2];
	{
	    TLchar	__x475[2];
	    {
		TLchar	__x474[2];
		{
		    TLchar	__x473[2];
		    TLRESTORELF();
		    return ((TL_TLS_TLSLEN(f) > 2) && ((TL_TLS_TLSBX(__x473, (TLint4) 1, f), ((strcmp(__x473, "\\") == 0) && ((TL_TLS_TLSBX(__x474, (TLint4) 2, f), strcmp(__x474, "\\") == 0))) || ((TL_TLS_TLSBX(__x475, (TLint4) 1, f), (strcmp(__x475, "/") == 0) && ((TL_TLS_TLSBX(__x476, (TLint4) 2, f), strcmp(__x476, "/") == 0)))))));
		};
	    };
	};
    };
    TLRESTORELF();
    /* NOTREACHED */
    TLFCNRESULTABORT;
}

static void FileManager_baseDir (f, __x270)
TLstring	f;
TLstring	__x270;
{
    TLint4	i;
    TLSTKCHKSLF(100284);
    TLSETF();
    TLSETL(100284);
    i = TL_TLS_TLSLEN(f);
    TLSETL(100286);
    for(;;) {
	TLSETL(100286);
	{
	    TLchar	__x478[2];
	    if ((i == 1) || ((TL_TLS_TLSBX(__x478, (TLint4) i, f), strcmp(__x478, "\\") == 0))) {
		break;
	    };
	};
	TLSETL(100287);
	i -= 1;
    };
    TLSETL(100294);
    if (FileManager_isUNCPath(f)) {
	TLint4	backslashCount;
	TLSETL(100295);
	backslashCount = 0;
	TLINCL();
	{
	    register TLint4	count;
	    TLint4	__x479;
	    __x479 = i;
	    count = 1;
	    if (count <= __x479) {
		for(;;) {
		    TLSETL(100297);
		    {
			TLchar	__x481[2];
			{
			    TLchar	__x480[2];
			    TL_TLS_TLSBX(__x480, (TLint4) count, f);
			    if ((strcmp(__x480, "\\") == 0) || ((TL_TLS_TLSBX(__x481, (TLint4) count, f), strcmp(__x481, "/") == 0))) {
				TLSETL(100298);
				backslashCount += 1;
				TLINCL();
				if (backslashCount > 4) {
				    TLSETL(100300);
				    {
					{
					    TLstring	__x482;
					    TL_TLS_TLSBXX(__x482, (TLint4) (i - 1), (TLint4) 1, f);
					    TLSTRASS(255, __x270, __x482);
					};
					TLRESTORELF();
					return;
				    };
				};
			    };
			};
		    };
		    if (count == __x479) break;
		    count++;
		}
	    };
	};
	TLSETL(100304);
	{
	    {
		TLstring	__x483;
		TL_TLS_TLSBXX(__x483, (TLint4) i, (TLint4) 1, f);
		TLSTRASS(255, __x270, __x483);
	    };
	    TLRESTORELF();
	    return;
	};
    };
    TLSETL(100308);
    if (i == 3) {
	TLSETL(100309);
	{
	    {
		TLstring	__x484;
		TL_TLS_TLSBXX(__x484, (TLint4) i, (TLint4) 1, f);
		TLSTRASS(255, __x270, __x484);
	    };
	    TLRESTORELF();
	    return;
	};
    } else {
	TLSETL(100311);
	{
	    {
		TLstring	__x485;
		TL_TLS_TLSBXX(__x485, (TLint4) (i - 1), (TLint4) 1, f);
		TLSTRASS(255, __x270, __x485);
	    };
	    TLRESTORELF();
	    return;
	};
    };
    TLRESTORELF();
    /* NOTREACHED */
    TLFCNRESULTABORT;
}

static void FileManager_convertName (name, __x276)
TLstring	name;
TLstring	__x276;
{
    TLnat4	len;
    TLnat4	ind;
    TLSTKCHKSLF(100320);
    TLSETF();
    TLSETL(100320);
    len = TL_TLS_TLSLEN(name);
    TLINCL();
    ind = TL_TLS_TLSIND(name, ".");
    TLSETL(100323);
    if (ind > 0) {
	TLnat4	ext;
	TLSETL(100324);
	ext =  TL_TLA_TLAIMN((TLint4) (len - ind), (TLint4) 3);
	TLSETL(100326);
	if (ind > 8) {
	    TLSETL(100327);
	    {
		{
		    TLstring	__x489;
		    TL_TLS_TLSBXX(__x489, (TLint4) (ind + ext), (TLint4) ind, name);
		    {
			TLchar	__x488[10];
			TL_TLS_TLSBXX(__x488, (TLint4) 8, (TLint4) 1, name);
			{
			    TLstring	__x487;
			    TL_TLS_TLSCAT(__x488, __x489, __x487);
			    TLSTRASS(255, __x276, __x487);
			};
		    };
		};
		TLRESTORELF();
		return;
	    };
	} else {
	    TLSETL(100329);
	    {
		{
		    TLstring	__x490;
		    TL_TLS_TLSBXX(__x490, (TLint4) (ind + ext), (TLint4) 1, name);
		    TLSTRASS(255, __x276, __x490);
		};
		TLRESTORELF();
		return;
	    };
	};
    } else {
	TLSETL(100332);
	if (len > 8) {
	    TLSETL(100333);
	    {
		{
		    TLchar	__x491[10];
		    TL_TLS_TLSBXX(__x491, (TLint4) 8, (TLint4) 1, name);
		    TLSTRASS(255, __x276, __x491);
		};
		TLRESTORELF();
		return;
	    };
	} else {
	    TLSETL(100335);
	    {
		TLSTRASS(255, __x276, name);
		TLRESTORELF();
		return;
	    };
	};
    };
    TLRESTORELF();
    /* NOTREACHED */
    TLFCNRESULTABORT;
}

static void FileManager_convertPath (opath, __x282)
TLstring	opath;
TLstring	__x282;
{
    TLstring	base;
    TLstring	name;
    TLSTKCHKSLF(100346);
    TLSETF();
    TLSETL(100346);
    {
	TLstring	__x493;
	FileManager_baseDir(opath, __x493);
	TLSTRASS(255, base, __x493);
    };
    TLINCL();
    {
	TLstring	__x494;
	FileManager_lastComponent(opath, __x494);
	TLSTRASS(255, name, __x494);
    };
    TLSETL(100349);
    if (TL_TLS_TLSLEN(base) == 0) {
	TLSETL(100350);
	{
	    {
		TLstring	__x495;
		FileManager_convertName(name, __x495);
		TLSTRASS(255, __x282, __x495);
	    };
	    TLRESTORELF();
	    return;
	};
    } else {
	TLSETL(100351);
	{
	    TLchar	__x496[2];
	    TL_TLS_TLSBS(__x496, (TLint4) 0, base);
	    if (strcmp(__x496, "\\") == 0) {
		TLSETL(100352);
		{
		    {
			TLstring	__x498;
			FileManager_convertName(name, __x498);
			{
			    TLstring	__x497;
			    TL_TLS_TLSCAT(base, __x498, __x497);
			    TLSTRASS(255, __x282, __x497);
			};
		    };
		    TLRESTORELF();
		    return;
		};
	    } else {
		TLSETL(100354);
		{
		    {
			TLstring	__x501;
			FileManager_convertName(name, __x501);
			{
			    TLstring	__x500;
			    TL_TLS_TLSCAT(base, "\\", __x500);
			    {
				TLstring	__x499;
				TL_TLS_TLSCAT(__x500, __x501, __x499);
				TLSTRASS(255, __x282, __x499);
			    };
			};
		    };
		    TLRESTORELF();
		    return;
		};
	    };
	};
    };
    TLRESTORELF();
    /* NOTREACHED */
    TLFCNRESULTABORT;
}

static void FileManager_percentExpand (f, lf, __x288)
TLstring	f;
TLnat4	lf;
TLstring	__x288;
{
    TLSTKCHKSLF(100366);
    TLSETF();
    TLSETL(100366);
    {
	register TLint4	i;
	for (i = 1; i <= 5; i++) {
	    TLint4	lp;
	    TLSETL(100367);
	    lp = TL_TLS_TLSLEN((FileManager_prefix[TLINRANGELOW(i, 1, 5, 1)])) + 1;
	    TLINCL();
	    {
		TLstring	__x503;
		if ((lf >= lp) && ((TL_TLS_TLSBXX(__x503, (TLint4) lp, (TLint4) 2, f), IsSamePath(__x503, FileManager_prefix[TLINRANGELOW(i, 1, 5, 1)])))) {
		    TLSETL(100369);
		    {
			TLchar	__x504[2];
			if ((lf > lp) && ((TL_TLS_TLSBX(__x504, (TLint4) (lp + 1), f), strcmp(__x504, "\\") == 0))) {
			    TLSETL(100370);
			    {
				{
				    TLstring	__x506;
				    TL_TLS_TLSBXS(__x506, (TLint4) 0, (TLint4) (lp + 2), f);
				    {
					TLstring	__x505;
					TL_TLS_TLSCAT(FileManager_Dirs[TLINRANGELOW(i, 1, 5, 1)], __x506, __x505);
					{
					    TLstring	__x507;
					    FullPath(__x505, __x507);
					    TLSTRASS(255, __x288, __x507);
					};
				    };
				};
				TLRESTORELF();
				return;
			    };
			} else {
			    TLSETL(100371);
			    if (lf == lp) {
				TLSETL(100372);
				{
				    TLSTRASS(255, __x288, FileManager_Dirs[TLINRANGELOW(i, 1, 5, 1)]);
				    TLRESTORELF();
				    return;
				};
			    };
			};
		    };
		};
	    };
	};
    };
    TLSETL(100376);
    {
	TLSTRASS(255, __x288, f);
	TLRESTORELF();
	return;
    };
    TLRESTORELF();
    /* NOTREACHED */
    TLFCNRESULTABORT;
}

static void FileManager_expand1 (file, relDir, srcFile, __x295)
TLstring	file;
TLstring	relDir;
FileManager_FileNoType	srcFile;
TLstring	__x295;
{

    extern void strlwr ();
    TLstring	f;
    TLint4	lf;
    TLBINDREG((*src), struct FileManager_FileDef);
    TLSTKCHKSLF(100390);
    TLSETF();
    TLSETL(100390);
    TLSTRASS(255, f, file);
    TLINCL();
    lf = TL_TLS_TLSLEN(f);
    TLSETL(100393);
    if (lf == 0) {
	TLSETL(100394);
	{
	    TLSTRASS(255, __x295, "");
	    TLRESTORELF();
	    return;
	};
    };
    TLSETL(100399);
    {
	register TLint4	i;
	TLint4	__x509;
	__x509 = lf;
	i = 1;
	if (i <= __x509) {
	    for(;;) {
		TLSETL(100400);
		{
		    TLchar	__x510[2];
		    TL_TLS_TLSBX(__x510, (TLint4) i, f);
		    if (strcmp(__x510, "/") == 0) {
			TLSETL(100401);
			{
			    TLstring	__x514;
			    TL_TLS_TLSBXS(__x514, (TLint4) 0, (TLint4) (i + 1), f);
			    {
				TLstring	__x513;
				TL_TLS_TLSBXX(__x513, (TLint4) (i - 1), (TLint4) 1, f);
				{
				    TLstring	__x512;
				    TL_TLS_TLSCAT(__x513, "\\", __x512);
				    {
					TLstring	__x511;
					TL_TLS_TLSCAT(__x512, __x514, __x511);
					TLSTRASS(255, f, __x511);
				    };
				};
			    };
			};
		    };
		};
		if (i == __x509) break;
		i++;
	    }
	};
    };
    TLSETL(100405);
    if (!FileManager_longFileNames) {
	TLSETL(100406);
	{
	    TLstring	__x515;
	    FileManager_convertPath(f, __x515);
	    TLSTRASS(255, f, __x515);
	};
	TLSETL(100408);
	strlwr(f);
    };
    TLSETL(100410);
    {
	TLchar	__x516[2];
	TL_TLS_TLSBX(__x516, (TLint4) 1, f);
	if (strcmp(__x516, "%") == 0) {
	    TLstring	pexp;
	    TLSETL(100411);
	    {
		TLstring	__x517;
		FileManager_percentExpand(f, (TLnat4) lf, __x517);
		TLSTRASS(255, pexp, __x517);
	    };
	    TLSETL(100415);
	    {
		TLchar	__x518[2];
		TL_TLS_TLSBX(__x518, (TLint4) 1, pexp);
		if (strcmp(__x518, "%") != 0) {
		    TLSETL(100416);
		    {
			TLSTRASS(255, __x295, pexp);
			TLRESTORELF();
			return;
		    };
		};
	    };
	} else {
	    TLSETL(100418);
	    {
		TLchar	__x519[2];
		TL_TLS_TLSBX(__x519, (TLint4) 1, f);
		if (strcmp(__x519, "~") == 0) {
		    TLSETL(100419);
		    if (lf > 1) {
			TLSETL(100420);
			{
			    {
				TLstring	__x521;
				TL_TLS_TLSBXS(__x521, (TLint4) 0, (TLint4) 2, f);
				{
				    TLstring	__x520;
				    TL_TLS_TLSCAT(FileManager_Dirs[1], __x521, __x520);
				    {
					TLstring	__x522;
					FullPath(__x520, __x522);
					TLSTRASS(255, __x295, __x522);
				    };
				};
			    };
			    TLRESTORELF();
			    return;
			};
		    } else {
			TLSETL(100422);
			{
			    TLSTRASS(255, __x295, FileManager_Dirs[1]);
			    TLRESTORELF();
			    return;
			};
		    };
		} else {
		    TLSETL(100424);
		    {
			TLchar	__x526[2];
			{
			    TLchar	__x525[2];
			    {
				TLchar	__x524[2];
				{
				    TLchar	__x523[2];
				    if ((((lf >= 3) && ((TL_TLS_TLSBX(__x523, (TLint4) 2, f), strcmp(__x523, ":") == 0))) && ((TL_TLS_TLSBX(__x524, (TLint4) 3, f), strcmp(__x524, "\\") == 0))) || ((TL_TLS_TLSBX(__x525, (TLint4) 1, f), (strcmp(__x525, "\\") == 0) || ((TL_TLS_TLSBX(__x526, (TLint4) 1, f), strcmp(__x526, "/") == 0))))) {
					TLSETL(100427);
					{
					    {
						TLstring	__x527;
						FullPath(f, __x527);
						TLSTRASS(255, __x295, __x527);
					    };
					    TLRESTORELF();
					    return;
					};
				    };
				};
			    };
			};
		    };
		};
	    };
	};
    };
    TLSETL(100432);
    if (srcFile == 0) {
	TLSETL(100434);
	{
	    {
		TLstring	__x528;
		TL_TLS_TLSCAT(relDir, f, __x528);
		{
		    TLstring	__x529;
		    FullPath(__x528, __x529);
		    TLSTRASS(255, __x295, __x529);
		};
	    };
	    TLRESTORELF();
	    return;
	};
    };
    TLSETL(100437);
    src = &(FileManager_fileDescriptors[TLINRANGELOW(srcFile, 1, 500, 1)]);
    TLSETL(100439);
    if (FileManager_sameFile(&((*src).did), &(FileManager_nullFID))) {
	TLSETL(100441);
	{
	    {
		TLstring	__x530;
		TL_TLS_TLSCAT(relDir, f, __x530);
		{
		    TLstring	__x531;
		    FullPath(__x530, __x531);
		    TLSTRASS(255, __x295, __x531);
		};
	    };
	    TLRESTORELF();
	    return;
	};
    } else {
	TLstring	base;
	TLSETL(100443);
	{
	    TLstring	__x532;
	    FileManager_baseDir((*src).name, __x532);
	    TLSTRASS(255, base, __x532);
	};
	TLINCL();
	{
	    TLchar	__x533[2];
	    TL_TLS_TLSBS(__x533, (TLint4) 0, base);
	    if (strcmp(__x533, "\\") == 0) {
		TLSETL(100445);
		{
		    {
			TLstring	__x535;
			FileManager_baseDir((*src).name, __x535);
			{
			    TLstring	__x534;
			    TL_TLS_TLSCAT(__x535, f, __x534);
			    {
				TLstring	__x536;
				FullPath(__x534, __x536);
				TLSTRASS(255, __x295, __x536);
			    };
			};
		    };
		    TLRESTORELF();
		    return;
		};
	    } else {
		TLSETL(100447);
		{
		    {
			TLstring	__x539;
			FileManager_baseDir((*src).name, __x539);
			{
			    TLstring	__x538;
			    TL_TLS_TLSCAT(__x539, "\\", __x538);
			    {
				TLstring	__x537;
				TL_TLS_TLSCAT(__x538, f, __x537);
				{
				    TLstring	__x540;
				    FullPath(__x537, __x540);
				    TLSTRASS(255, __x295, __x540);
				};
			    };
			};
		    };
		    TLRESTORELF();
		    return;
		};
	    };
	};
    };
    TLRESTORELF();
    /* NOTREACHED */
    TLFCNRESULTABORT;
}

static void FileManager_expand (file, srcFile, __x307)
TLstring	file;
FileManager_FileNoType	srcFile;
TLstring	__x307;
{
    TLSTKCHKSLF(100453);
    TLSETF();
    TLSETL(100453);
    {
	{
	    TLstring	__x542;
	    FileManager_expand1(file, FileManager_wd, (FileManager_FileNoType) srcFile, __x542);
	    TLSTRASS(255, __x307, __x542);
	};
	TLRESTORELF();
	return;
    };
    TLRESTORELF();
    /* NOTREACHED */
    TLFCNRESULTABORT;
}

static void FileManager_getFile (name, fileNo)
TLstring	name;
FileManager_FileNoType	*fileNo;
{
    struct FileManager_FID	did;
    TLstring	last;
    TLSTKCHKSLF(100468);
    TLSETF();
    TLSETL(100468);
    {
	TLchar	__x543[2];
	TL_TLS_TLSBX(__x543, (TLint4) 3, name);
	TLPRE ((strcmp(__x543, "\\") == 0) || FileManager_isUNCPath(name));
    };
    TLSETL(100473);
    {
	TLstring	__x544;
	FileManager_baseDir(name, __x544);
	{
	    struct FileManager_FID	__x545;
	    GetFID(__x544, &(__x545));
	    TLSTRCTASS(did, __x545, struct FileManager_FID);
	};
    };
    TLINCL();
    {
	TLstring	__x546;
	FileManager_lastComponent(name, __x546);
	TLSTRASS(255, last, __x546);
    };
    TLSETL(100476);
    (*fileNo) = 0;
    TLINCL();
    {
	register TLint4	i;
	for (i = 1; i <= 500; i++) {
	    TLBINDREG((*file), struct FileManager_FileDef);
	    TLSETL(100478);
	    file = &(FileManager_fileDescriptors[TLINRANGELOW(i, 1, 500, 1)]);
	    TLINCL();
	    {
		TLstring	__x547;
		if ((((*file).allocated) && FileManager_sameFile(&(did), &((*file).did))) && ((FileManager_lastComponent((*file).name, __x547), strcmp(last, __x547) == 0))) {
		    TLSETL(100482);
		    (*fileNo) = i;
		    TLSETL(100484);
		    break;
		};
	    };
	};
    };
    TLSETL(100487);
    if ((*fileNo) == 0) {
	TLSETL(100490);
	FileManager_allocateFile(&((*fileNo)));
	TLSTRASS(255, FileManager_fileDescriptors[TLINRANGELOW((*fileNo), 1, 500, 1)].name, name);
	TLINCL();
	TLSTRCTASS(FileManager_fileDescriptors[TLINRANGELOW((*fileNo), 1, 500, 1)].did, did, struct FileManager_FID);
    };
    TLRESTORELF();
}

static void FileManager_getText (name, textNo)
TLstring	name;
FileManager_TextNoType	*textNo;
{
    struct FileManager_FID	fid;
    TLSTKCHKSLF(100501);
    TLSETF();
    TLSETL(100501);
    {
	struct FileManager_FID	__x548;
	GetFID(name, &(__x548));
	TLSTRCTASS(fid, __x548, struct FileManager_FID);
    };
    TLINCL();
    (*textNo) = 0;
    TLINCL();
    {
	register TLint4	i;
	for (i = 1; i <= 500; i++) {
	    TLBINDREG((*text), struct FileManager_TextDef);
	    TLSETL(100504);
	    text = &(FileManager_textDescriptors[TLINRANGELOW(i, 1, 500, 1)]);
	    TLINCL();
	    if ((((*text).refCount) > 0) && FileManager_sameFile(&(fid), &((*text).fid))) {
		TLSETL(100506);
		(*textNo) = i;
		TLSETL(100508);
		break;
	    };
	};
    };
    TLSETL(100511);
    if ((*textNo) == 0) {
	TLSETL(100514);
	FileManager_allocateText(&((*textNo)));
	TLASSERT ((*textNo) != 0);
	TLINCL();
	TLSTRCTASS(FileManager_textDescriptors[TLINRANGELOW((*textNo), 1, 500, 1)].fid, fid, struct FileManager_FID);
	TLINCL();
	FileManager_textDescriptors[TLINRANGELOW((*textNo), 1, 500, 1)].fid.t = 0;
    };
    TLRESTORELF();
}

static void FileManager_bumpTimeStamp (fileNo)
FileManager_FileNoType	fileNo;
{
    TLBINDREG((*file), struct FileManager_FileDef);
    TLBINDREG((*text), struct FileManager_TextDef);
    TLSTKCHKSLF(100526);
    TLSETF();
    TLSETL(100526);
    file = &(FileManager_fileDescriptors[TLINRANGELOW(fileNo, 1, 500, 1)]);
    TLINCL();
    text = &(FileManager_textDescriptors[TLINRANGELOW((*file).textNo, 1, 500, 1)]);
    TLSETL(100529);
    if ((((*text).fileRefs) > 0) && ((((*text).fileRefs) > ((*file).refCount)) || (((*file).lastStamp) >= ((*text).timeStamp)))) {
	TLSETL(100532);
	(*text).timeStamp += 1;
	TLINCL();
	if (((*text).timeStamp) == 0) {
	    TLSETL(100535);
	    (*text).timeStamp = 1;
	};
    };
    TLRESTORELF();
}

static void FileManager_openFile (fileNo, rc)
FileManager_FileNoType	fileNo;
FileManager_ResultCode	*rc;
{
    TLBINDREG((*file), struct FileManager_FileDef);
    struct FileManager_FID	fid;
    TLBINDREG((*text), struct FileManager_TextDef);
    TLSTKCHKSLF(100546);
    TLSETF();
    TLSETL(100546);
    file = &(FileManager_fileDescriptors[TLINRANGELOW(fileNo, 1, 500, 1)]);
    TLSETL(100550);
    TLASSERT ((FileManager_textDescriptors[TLINRANGELOW((*file).textNo, 1, 500, 1)].openCount) == 0);
    TLSETL(100552);
    (*rc) = 0;
    TLSETL(100554);
    if (strcmp((*file).name, "") == 0) {
	TLSETL(100557);
	TLRESTORELF();
	return;
    };
    TLSETL(100559);
    {
	struct FileManager_FID	__x549;
	GetFID((*file).name, &(__x549));
	TLSTRCTASS(fid, __x549, struct FileManager_FID);
    };
    TLSETL(100561);
    if (FileManager_sameFile(&(FileManager_textDescriptors[TLINRANGELOW((*file).textNo, 1, 500, 1)].fid), &(FileManager_nullFID)) && FileManager_sameFile(&(fid), &(FileManager_nullFID))) {
	TLSETL(100566);
	TLRESTORELF();
	return;
    };
    TLSETL(100578);
    if ((!IsRegularFile((*file).name)) || (!CanRead((*file).name))) {
	TLBINDREG((*text), struct FileManager_TextDef);
	TLSETL(100581);
	text = &(FileManager_textDescriptors[TLINRANGELOW((*file).textNo, 1, 500, 1)]);
	TLSETL(100583);
	FileManager_bumpTimeStamp((FileManager_FileNoType) fileNo);
	TLSTRCTASS((*text).fid, fid, struct FileManager_FID);
	TLSETL(100585);
	FreeFile(&((*text).textPtr));
	TLINCL();
	NullFile(&((*text).textPtr), &((*text).textSize));
	(*rc) = 1;
	TLSETL(100588);
	TLRESTORELF();
	return;
    };
    TLSETL(100590);
    text = &(FileManager_textDescriptors[TLINRANGELOW((*file).textNo, 1, 500, 1)]);
    TLSETL(100592);
    if (((*text).fid.t) != (fid.t)) {
	TLSETL(100595);
	FreeFile(&((*text).textPtr));
	TLINCL();
	ReadOOTFile((*file).name, &((*text).textPtr), &((*text).textSize), &((*rc)));
	if (((*rc) != 0) && ((*rc) != 3)) {
	    TLSETL(100598);
	    NullFile(&((*text).textPtr), &((*text).textSize));
	};
	TLSETL(100599);
	(*text).fid.t = fid.t;
	TLSETL(100601);
	FileManager_bumpTimeStamp((FileManager_FileNoType) fileNo);
    };
    TLRESTORELF();
}

static void FileManager_openNamed (name, fileNo, rc)
TLstring	name;
FileManager_FileNoType	*fileNo;
FileManager_ResultCode	*rc;
{
    TLBINDREG((*file), struct FileManager_FileDef);
    TLBINDREG((*text), struct FileManager_TextDef);
    TLSTKCHKSLF(100613);
    TLSETF();
    TLSETL(100613);
    {
	TLchar	__x550[2];
	TL_TLS_TLSBX(__x550, (TLint4) 3, name);
	TLPRE ((strcmp(__x550, "\\") == 0) || FileManager_isUNCPath(name));
    };
    TLSETL(100618);
    (*fileNo) = 0;
    TLINCL();
    (*rc) = 0;
    TLSETL(100621);
    if ((!IsRegularFile(name)) || (!CanRead(name))) {
	TLSETL(100622);
	(*rc) = 1;
	TLSETL(100624);
	TLRESTORELF();
	return;
    };
    TLSETL(100628);
    FileManager_getFile(name, &((*fileNo)));
    if ((*fileNo) == 0) {
	TLSETL(100629);
	(*rc) = 4;
	TLSETL(100631);
	TLRESTORELF();
	return;
    };
    TLSETL(100633);
    file = &(FileManager_fileDescriptors[TLINRANGELOW((*fileNo), 1, 500, 1)]);
    TLSETL(100636);
    if (((*file).textNo) == 0) {
	TLSETL(100638);
	FileManager_getText(name, &((*file).textNo));
    };
    TLSETL(100639);
    text = &(FileManager_textDescriptors[TLINRANGELOW((*file).textNo, 1, 500, 1)]);
    TLSETL(100641);
    if (((*text).openCount) == 0) {
	TLSETL(100643);
	FileManager_openFile((FileManager_FileNoType) (*fileNo), &((*rc)));
	if (((*rc) != 0) && ((*rc) != 3)) {
	    TLSETL(100644);
	    if (((*text).refCount) == 0) {
		TLSETL(100646);
		FreeFile(&((*text).textPtr));
	    };
	    TLSETL(100647);
	    (*fileNo) = 0;
	    TLSETL(100649);
	    TLRESTORELF();
	    return;
	};
    };
    TLSETL(100652);
    if (!((*file).allocated)) {
	TLSETL(100654);
	(*file).allocated = 1;
	TLINCL();
	(*text).refCount += 1;
    };
    TLRESTORELF();
}

static void FileManager_save (fileNo, rc)
FileManager_FileNoType	fileNo;
FileManager_ResultCode	*rc;
{
    TLBINDREG((*file), struct FileManager_FileDef);
    TLBINDREG((*text), struct FileManager_TextDef);
    TLSTKCHKSLF(100664);
    TLSETF();
    TLSETL(100664);
    file = &(FileManager_fileDescriptors[TLINRANGELOW(fileNo, 1, 500, 1)]);
    TLINCL();
    text = &(FileManager_textDescriptors[TLINRANGELOW((*file).textNo, 1, 500, 1)]);
    TLSETL(100668);
    if (FileManager_sameFile(&((*file).did), &(FileManager_nullFID))) {
	TLSETL(100669);
	(*rc) = 2;
	TLSETL(100671);
	TLRESTORELF();
	return;
    };
    TLSETL(100674);
    SaveFile((*file).name, (TLaddressint) ((*text).textPtr), (TLnat4) ((*text).textSize), &((*rc)));
    if ((*rc) == 0) {
	TLSETL(100675);
	{
	    struct FileManager_FID	__x551;
	    GetFID((*file).name, &(__x551));
	    TLSTRCTASS((*text).fid, __x551, struct FileManager_FID);
	};
    };
    TLRESTORELF();
}

static void FileManager_saveBackup (fileName, ext, text, savedAs)
TLstring	fileName;
TLstring	ext;
struct FileManager_TextDef	*text;
TLstring	savedAs;
{
    TLstring	name;
    TLint4	ind;
    TLint4	i;
    FileManager_ResultCode	rc;
    TLSTKCHKSLF(100691);
    TLSETF();
    TLSETL(100691);
    ind = TL_TLS_TLSIND(fileName, ".");
    TLINCL();
    if (ind > 0) {
	TLSETL(100693);
	{
	    TLstring	__x552;
	    TL_TLS_TLSBXX(__x552, (TLint4) (ind - 1), (TLint4) 1, fileName);
	    TLSTRASS(255, name, __x552);
	};
    } else {
	TLSETL(100695);
	TLSTRASS(255, name, fileName);
    };
    TLSETL(100698);
    i = 0;
    TLINCL();
    {
	TLstring	__x556;
	TL_TLS_TLSVIS((TLint4) i, (TLint4) 0, (TLint4) 10, __x556);
	{
	    TLstring	__x555;
	    TL_TLS_TLSCAT(name, ".", __x555);
	    {
		TLstring	__x554;
		TL_TLS_TLSCAT(__x555, ext, __x554);
		{
		    TLstring	__x553;
		    TL_TLS_TLSCAT(__x554, __x556, __x553);
		    TLSTRASS(255, savedAs, __x553);
		};
	    };
	};
    };
    TLSETL(100701);
    for(;;) {
	TLSETL(100701);
	if (!Exists(savedAs)) {
	    break;
	};
	TLSETL(100702);
	i += 1;
	TLINCL();
	if (i < 10) {
	    TLSETL(100704);
	    {
		TLstring	__x560;
		TL_TLS_TLSVIS((TLint4) i, (TLint4) 0, (TLint4) 10, __x560);
		{
		    TLstring	__x559;
		    TL_TLS_TLSCAT(name, ".", __x559);
		    {
			TLstring	__x558;
			TL_TLS_TLSCAT(__x559, ext, __x558);
			{
			    TLstring	__x557;
			    TL_TLS_TLSCAT(__x558, __x560, __x557);
			    TLSTRASS(255, savedAs, __x557);
			};
		    };
		};
	    };
	} else {
	    TLSETL(100706);
	    {
		TLstring	__x563;
		TL_TLS_TLSCAT(name, ".", __x563);
		{
		    TLstring	__x562;
		    TL_TLS_TLSCAT(__x563, ext, __x562);
		    {
			TLstring	__x561;
			{
			    TLchar	__x564[2];
			    TLCHRTOSTR(((TLchar) ((97 + i) - 10)), __x564);
			    TL_TLS_TLSCAT(__x562, __x564, __x561);
			};
			TLSTRASS(255, savedAs, __x561);
		    };
		};
	    };
	};
    };
    TLSETL(100712);
    SaveFile(savedAs, (TLaddressint) ((*text).textPtr), (TLnat4) ((*text).textSize), &(rc));
    if (rc != 0) {
	TLSETL(100713);
	TLSTRASS(255, savedAs, "");
    };
    TLRESTORELF();
}

void FileManager_OpenNamedHandle (fileName, fileNo, textHandle, sizePtr, resultCode)
TLstring	fileName;
FileManager_FileNoType	*fileNo;
TLaddressint	*textHandle;
TLaddressint	*sizePtr;
FileManager_ResultCode	*resultCode;
{
    TLstring	name;
    TLBINDREG((*file), struct FileManager_FileDef);
    TLBINDREG((*text), struct FileManager_TextDef);
    TLSTKCHKSLF(100731);
    TLSETF();
    TLSETL(100731);
    {
	TLstring	__x565;
	FileManager_expand(fileName, (FileManager_FileNoType) 0, __x565);
	TLSTRASS(255, name, __x565);
    };
    TLSETL(100733);
    (*fileNo) = 0;
    TLINCL();
    (*textHandle) = (TLaddressint) 0;
    TLINCL();
    (*sizePtr) = (TLaddressint) 0;
    TLINCL();
    (*resultCode) = 0;
    TLSETL(100738);
    if (strcmp(name, "") == 0) {
	TLSETL(100739);
	(*resultCode) = 1;
	TLSETL(100741);
	TLRESTORELF();
	return;
    } else {
	TLSETL(100741);
	if (Exists(name)) {
	    TLSETL(100743);
	    FileManager_openNamed(name, &((*fileNo)), &((*resultCode)));
	    if ((*fileNo) == 0) {
		TLSETL(100745);
		TLRESTORELF();
		return;
	    };
	} else {
	    TLBINDREG((*file), struct FileManager_FileDef);
	    TLSETL(100749);
	    FileManager_getFile(name, &((*fileNo)));
	    if ((*fileNo) == 0) {
		TLSETL(100750);
		(*resultCode) = 4;
		TLSETL(100752);
		TLRESTORELF();
		return;
	    };
	    TLSETL(100754);
	    file = &(FileManager_fileDescriptors[TLINRANGELOW((*fileNo), 1, 500, 1)]);
	    TLSETL(100756);
	    if (((*file).textNo) == 0) {
		TLBINDREG((*text), struct FileManager_TextDef);
		TLSETL(100758);
		FileManager_allocateText(&((*file).textNo));
		TLASSERT (((*file).textNo) != 0);
		TLINCL();
		text = &(FileManager_textDescriptors[TLINRANGELOW((*file).textNo, 1, 500, 1)]);
		TLSETL(100761);
		NullFile(&((*text).textPtr), &((*text).textSize));
	    };
	};
    };
    TLSETL(100764);
    file = &(FileManager_fileDescriptors[TLINRANGELOW((*fileNo), 1, 500, 1)]);
    TLINCL();
    text = &(FileManager_textDescriptors[TLINRANGELOW((*file).textNo, 1, 500, 1)]);
    TLSETL(100767);
    if (!((*file).allocated)) {
	TLSETL(100768);
	(*file).allocated = 1;
	TLINCL();
	(*text).refCount += 1;
    };
    TLSETL(100771);
    (*file).handleOpen += 1;
    TLINCL();
    (*text).openCount += 1;
    TLSETL(100775);
    TLSTRASS(255, (*file).uname, fileName);
    TLSETL(100777);
    (*textHandle) = (TLaddressint) ((unsigned long)&((*text).textPtr));
    TLINCL();
    (*sizePtr) = (TLaddressint) ((unsigned long)&((*text).textSize));
    TLSETL(100780);
    if (((*text).openCount) > 1) {
	TLSETL(100782);
	(*resultCode) = 5;
    };
    TLRESTORELF();
}

void FileManager_OpenUnnamedHandle (fileNo, textHandle, sizePtr)
FileManager_FileNoType	*fileNo;
TLaddressint	*textHandle;
TLaddressint	*sizePtr;
{
    TLBINDREG((*file), struct FileManager_FileDef);
    TLBINDREG((*text), struct FileManager_TextDef);
    TLSTKCHKSLF(100793);
    TLSETF();
    TLSETL(100793);
    (*fileNo) = 0;
    TLINCL();
    (*textHandle) = (TLaddressint) 0;
    TLINCL();
    (*sizePtr) = (TLaddressint) 0;
    TLSETL(100798);
    FileManager_allocateFile(&((*fileNo)));
    if ((*fileNo) == 0) {
	TLSETL(100800);
	TLRESTORELF();
	return;
    };
    TLSETL(100802);
    file = &(FileManager_fileDescriptors[TLINRANGELOW((*fileNo), 1, 500, 1)]);
    TLSETL(100805);
    FileManager_allocateText(&((*file).textNo));
    TLASSERT (((*file).textNo) != 0);
    TLINCL();
    text = &(FileManager_textDescriptors[TLINRANGELOW((*file).textNo, 1, 500, 1)]);
    TLSETL(100808);
    (*file).allocated = 1;
    TLINCL();
    (*file).handleOpen = 1;
    TLSETL(100811);
    (*text).refCount = 1;
    TLINCL();
    (*text).openCount = 1;
    TLSETL(100816);
    NullFile(&((*text).textPtr), &((*text).textSize));
    (*textHandle) = (TLaddressint) ((unsigned long)&((*text).textPtr));
    TLINCL();
    (*sizePtr) = (TLaddressint) ((unsigned long)&((*text).textSize));
    TLSETL(100820);
    FileManager_getUnName((*file).uname);
    TLRESTORELF();
}

void FileManager_OpenFileHandle (fileNo, textHandle, sizePtr, resultCode)
FileManager_FileNoType	fileNo;
TLaddressint	*textHandle;
TLaddressint	*sizePtr;
FileManager_ResultCode	*resultCode;
{
    TLBINDREG((*file), struct FileManager_FileDef);
    TLBINDREG((*text), struct FileManager_TextDef);
    TLSTKCHKSLF(100831);
    TLSETF();
    TLSETL(100831);
    file = &(FileManager_fileDescriptors[TLINRANGELOW(fileNo, 1, 500, 1)]);
    TLINCL();
    text = &(FileManager_textDescriptors[TLINRANGELOW((*file).textNo, 1, 500, 1)]);
    TLINCL();
    TLASSERT ((*file).allocated);
    TLSETL(100835);
    if (((*text).openCount) == 0) {
	TLSETL(100837);
	FileManager_openFile((FileManager_FileNoType) fileNo, &((*resultCode)));
    };
    TLSETL(100839);
    (*file).handleOpen += 1;
    TLSETL(100841);
    if ((((*file).handleOpen) == 1) && FileManager_sameFile(&((*file).did), &(FileManager_nullFID))) {
	TLSETL(100843);
	FileManager_getUnName((*file).uname);
    };
    TLSETL(100845);
    (*text).openCount += 1;
    TLSETL(100847);
    (*textHandle) = (TLaddressint) ((unsigned long)&((*text).textPtr));
    TLINCL();
    (*sizePtr) = (TLaddressint) ((unsigned long)&((*text).textSize));
    TLRESTORELF();
}

void FileManager_SetModified (fileNo)
FileManager_FileNoType	fileNo;
{
    TLBINDREG((*file), struct FileManager_FileDef);
    TLBINDREG((*text), struct FileManager_TextDef);
    TLSTKCHKSLF(100856);
    TLSETF();
    TLSETL(100856);
    file = &(FileManager_fileDescriptors[TLINRANGELOW(fileNo, 1, 500, 1)]);
    TLINCL();
    text = &(FileManager_textDescriptors[TLINRANGELOW((*file).textNo, 1, 500, 1)]);
    TLINCL();
    TLASSERT (((*file).textOpen) == 0);
    TLSETL(100862);
    (*text).fid.t = 0;
    TLSETL(100864);
    FileManager_bumpTimeStamp((FileManager_FileNoType) fileNo);
    TLRESTORELF();
}

void FileManager_WriteFile (fileName, fileNo, textHandle, sizePtr, resultCode)
TLstring	fileName;
FileManager_FileNoType	*fileNo;
TLaddressint	*textHandle;
TLaddressint	*sizePtr;
FileManager_ResultCode	*resultCode;
{
    TLstring	name;
    TLboolean	nameExists;
    FileManager_FileNoType	newFileNo;
    TLBIND((*file), struct FileManager_FileDef);
    TLSTKCHKSLF(100875);
    TLSETF();
    TLSETL(100875);
    {
	TLstring	__x566;
	FileManager_expand(fileName, (FileManager_FileNoType) 0, __x566);
	TLSTRASS(255, name, __x566);
    };
    TLSETL(100877);
    if (strcmp(name, "") == 0) {
	TLSETL(100878);
	(*resultCode) = 2;
	TLSETL(100880);
	TLRESTORELF();
	return;
    };
    TLSETL(100882);
    nameExists = Exists(name);
    TLSETL(100884);
    if (nameExists && (!CanReadWrite(name))) {
	TLSETL(100885);
	(*resultCode) = 2;
	TLSETL(100887);
	TLRESTORELF();
	return;
    };
    TLSETL(100893);
    FileManager_getFile(name, &(newFileNo));
    if (newFileNo == 0) {
	TLSETL(100894);
	(*resultCode) = 4;
	TLSETL(100896);
	TLRESTORELF();
	return;
    };
    TLSETL(100898);
    if (newFileNo != (*fileNo)) {
	TLBINDREG((*file), struct FileManager_FileDef);
	TLBIND((*oldFile), struct FileManager_FileDef);
	TLBIND((*oldText), struct FileManager_TextDef);
	TLBINDREG((*text), struct FileManager_TextDef);
	TLSETL(100901);
	file = &(FileManager_fileDescriptors[TLINRANGELOW(newFileNo, 1, 500, 1)]);
	oldFile = &(FileManager_fileDescriptors[TLINRANGELOW((*fileNo), 1, 500, 1)]);
	TLSETL(100903);
	oldText = &(FileManager_textDescriptors[TLINRANGELOW((*oldFile).textNo, 1, 500, 1)]);
	TLSETL(100906);
	if (((*file).textNo) == 0) {
	    TLSETL(100907);
	    if (nameExists) {
		TLSETL(100909);
		FileManager_getText(name, &((*file).textNo));
	    } else {
		TLSETL(100911);
		FileManager_allocateText(&((*file).textNo));
	    };
	};
	TLSETL(100913);
	text = &(FileManager_textDescriptors[TLINRANGELOW((*file).textNo, 1, 500, 1)]);
	TLINCL();
	if (((*text).openCount) != 0) {
	    TLSETL(100915);
	    (*resultCode) = 5;
	    TLSETL(100917);
	    TLRESTORELF();
	    return;
	};
	TLSETL(100921);
	FreeFile(&((*text).textPtr));
	(*text).textPtr = (TLaddressint) ((*oldText).textPtr);
	TLINCL();
	(*text).textSize = (*oldText).textSize;
	TLINCL();
	(*oldText).textPtr = (TLaddressint) 0;
	TLINCL();
	(*oldText).textSize = 0;
	TLSETL(100927);
	if (!((*file).allocated)) {
	    TLSETL(100928);
	    (*file).allocated = 1;
	    TLINCL();
	    (*text).refCount += 1;
	};
	TLSETL(100931);
	(*file).handleOpen += 1;
	TLINCL();
	(*text).openCount += 1;
	TLSETL(100937);
	FileManager_CloseFileHandle((FileManager_FileNoType) (*fileNo));
	(*fileNo) = newFileNo;
	TLINCL();
	(*textHandle) = (TLaddressint) ((unsigned long)&((*text).textPtr));
	TLINCL();
	(*sizePtr) = (TLaddressint) ((unsigned long)&((*text).textSize));
    };
    TLSETL(100942);
    file = &(FileManager_fileDescriptors[TLINRANGELOW((*fileNo), 1, 500, 1)]);
    TLSETL(100945);
    TLSTRASS(255, (*file).uname, fileName);
    TLSETL(100948);
    FileManager_save((FileManager_FileNoType) (*fileNo), &((*resultCode)));
    TLRESTORELF();
}

void FileManager_WriteBackup (fileNo, savedAs)
FileManager_FileNoType	fileNo;
TLstring	savedAs;
{
    TLBINDREG((*file), struct FileManager_FileDef);
    TLBINDREG((*text), struct FileManager_TextDef);
    TLboolean	unnamed;
    TLstring	last;
    TLSTKCHKSLF(100958);
    TLSETF();
    TLSETL(100958);
    TLSTRASS(255, savedAs, "");
    TLSETL(100961);
    if ((fileNo < 1) || (fileNo > 500)) {
	TLSETL(100963);
	TLRESTORELF();
	return;
    };
    TLSETL(100965);
    file = &(FileManager_fileDescriptors[TLINRANGELOW(fileNo, 1, 500, 1)]);
    TLSETL(100967);
    if (!((*file).allocated)) {
	TLSETL(100969);
	TLRESTORELF();
	return;
    };
    TLSETL(100971);
    if ((((*file).textNo) < 1) || (((*file).textNo) > 500)) {
	TLSETL(100973);
	TLRESTORELF();
	return;
    };
    TLSETL(100975);
    text = &(FileManager_textDescriptors[TLINRANGELOW((*file).textNo, 1, 500, 1)]);
    TLSETL(100977);
    if ((((*text).refCount) <= 0) || (((*text).refCount) > 500)) {
	TLSETL(100979);
	TLRESTORELF();
	return;
    };
    TLSETL(100981);
    unnamed = FileManager_sameFile(&((*file).did), &(FileManager_nullFID));
    TLSETL(100984);
    if (!unnamed) {
	TLSETL(100986);
	FileManager_saveBackup((*file).name, "bk", &((*text)), savedAs);
	if (strcmp(savedAs, "") != 0) {
	    TLSETL(100988);
	    TLRESTORELF();
	    return;
	};
    };
    TLSETL(100993);
    if (unnamed) {
	TLSETL(100994);
	TLSTRASS(255, last, "unnamed");
    } else {
	TLSETL(100996);
	{
	    TLstring	__x567;
	    FileManager_lastComponent((*file).name, __x567);
	    TLSTRASS(255, last, __x567);
	};
    };
    TLSETL(101000);
    FileManager_saveBackup(last, "bk", &((*text)), savedAs);
    if (strcmp(savedAs, "") != 0) {
	TLSETL(101002);
	TLRESTORELF();
	return;
    };
    TLSETL(101008);
    {
	TLstring	__x568;
	TL_TLS_TLSCAT(FileManager_TempDir, last, __x568);
	FileManager_saveBackup(__x568, "bk", &((*text)), savedAs);
    };
    TLRESTORELF();
}

void FileManager_WriteTemp (fileNo, savedAs)
FileManager_FileNoType	fileNo;
TLstring	savedAs;
{
    TLBINDREG((*file), struct FileManager_FileDef);
    TLBINDREG((*text), struct FileManager_TextDef);
    TLSTKCHKSLF(101016);
    TLSETF();
    TLSETL(101016);
    file = &(FileManager_fileDescriptors[TLINRANGELOW(fileNo, 1, 500, 1)]);
    TLINCL();
    text = &(FileManager_textDescriptors[TLINRANGELOW((*file).textNo, 1, 500, 1)]);
    TLSETL(101019);
    {
	TLstring	__x571;
	TL_TLS_TLSVIS((TLint4) getpid(), (TLint4) 0, (TLint4) 10, __x571);
	{
	    TLstring	__x570;
	    TL_TLS_TLSCAT(FileManager_TempDir, "oot", __x570);
	    {
		TLstring	__x569;
		TL_TLS_TLSCAT(__x570, __x571, __x569);
		FileManager_saveBackup(__x569, "tm", &((*text)), savedAs);
	    };
	};
    };
    TLRESTORELF();
}

void FileManager_RmTemp () {
    TLSTKCHKSLF(101026);
    TLSETF();
    TLSETL(101026);
    {
	TLstring	__x576;
	TL_TLS_TLSVIS((TLint4) getpid(), (TLint4) 0, (TLint4) 10, __x576);
	{
	    TLstring	__x575;
	    TL_TLS_TLSCAT("del /y ", FileManager_TempDir, __x575);
	    {
		TLstring	__x574;
		TL_TLS_TLSCAT(__x575, "oot", __x574);
		{
		    TLstring	__x573;
		    TL_TLS_TLSCAT(__x574, __x576, __x573);
		    {
			TLstring	__x572;
			TL_TLS_TLSCAT(__x573, "*.*", __x572);
			system(__x572);
		    };
		};
	    };
	};
    };
    TLRESTORELF();
}

void FileManager_CloseFileHandle (fileNo)
FileManager_FileNoType	fileNo;
{
    TLBINDREG((*file), struct FileManager_FileDef);
    TLBINDREG((*text), struct FileManager_TextDef);
    TLSTKCHKSLF(101033);
    TLSETF();
    TLSETL(101033);
    file = &(FileManager_fileDescriptors[TLINRANGELOW(fileNo, 1, 500, 1)]);
    TLINCL();
    text = &(FileManager_textDescriptors[TLINRANGELOW((*file).textNo, 1, 500, 1)]);
    TLINCL();
    TLASSERT (((((*file).allocated) && (((*file).handleOpen) > 0)) && (((*text).refCount) > 0)) && (((*text).openCount) > 0));
    TLSETL(101038);
    (*file).handleOpen -= 1;
    TLINCL();
    if ((((*file).handleOpen) == 0) && FileManager_sameFile(&((*file).did), &(FileManager_nullFID))) {
	TLSETL(101041);
	FileManager_freeUnName((*file).uname);
    };
    TLSETL(101042);
    (*text).openCount -= 1;
    TLSETL(101044);
    if (((((*file).handleOpen) == 0) && (((*file).textOpen) == 0)) && (((*file).refCount) == 0)) {
	TLSETL(101045);
	(*file).allocated = 0;
	TLINCL();
	(*text).refCount -= 1;
	TLINCL();
	if (((*text).refCount) == 0) {
	    TLSETL(101049);
	    FreeFile(&((*text).textPtr));
	};
    };
    TLRESTORELF();
}

void FileManager_OpenNamedText (relName, srcFile, fileNo, textPtr, resultCode)
TLstring	relName;
FileManager_FileNoType	srcFile;
FileManager_FileNoType	*fileNo;
TLaddressint	*textPtr;
FileManager_ResultCode	*resultCode;
{
    TLstring	name;
    TLBINDREG((*file), struct FileManager_FileDef);
    TLBINDREG((*text), struct FileManager_TextDef);
    TLSTKCHKSLF(101071);
    TLSETF();
    TLSETL(101071);
    {
	TLstring	__x577;
	FileManager_expand(relName, (FileManager_FileNoType) srcFile, __x577);
	TLSTRASS(255, name, __x577);
    };
    TLSETL(101073);
    (*fileNo) = 0;
    TLINCL();
    (*textPtr) = (TLaddressint) 0;
    TLSETL(101076);
    if (strcmp(name, "") == 0) {
	TLSETL(101077);
	(*resultCode) = 1;
	TLSETL(101079);
	TLRESTORELF();
	return;
    };
    TLSETL(101082);
    FileManager_openNamed(name, &((*fileNo)), &((*resultCode)));
    if ((*fileNo) == 0) {
	TLSETL(101084);
	TLRESTORELF();
	return;
    };
    TLSETL(101086);
    file = &(FileManager_fileDescriptors[TLINRANGELOW((*fileNo), 1, 500, 1)]);
    TLINCL();
    text = &(FileManager_textDescriptors[TLINRANGELOW((*file).textNo, 1, 500, 1)]);
    TLSETL(101089);
    (*file).textOpen += 1;
    TLINCL();
    (*text).openCount += 1;
    TLSETL(101093);
    TLSTRASS(255, (*file).uname, relName);
    TLSETL(101095);
    (*textPtr) = (TLaddressint) ((*text).textPtr);
    TLSETL(101097);
    TLASSERT (((*file).allocated) && (((*text).refCount) > 0));
    TLRESTORELF();
}

void FileManager_OpenFileText (fileNo, textPtr, resultCode)
FileManager_FileNoType	fileNo;
TLaddressint	*textPtr;
FileManager_ResultCode	*resultCode;
{
    TLBINDREG((*file), struct FileManager_FileDef);
    TLBINDREG((*text), struct FileManager_TextDef);
    TLSTKCHKSLF(101109);
    TLSETF();
    TLSETL(101109);
    file = &(FileManager_fileDescriptors[TLINRANGELOW(fileNo, 1, 500, 1)]);
    TLINCL();
    text = &(FileManager_textDescriptors[TLINRANGELOW((*file).textNo, 1, 500, 1)]);
    TLINCL();
    TLASSERT ((*file).allocated);
    TLSETL(101113);
    (*resultCode) = 0;
    TLSETL(101115);
    (*file).textOpen += 1;
    TLINCL();
    (*text).openCount += 1;
    TLSETL(101118);
    (*textPtr) = (TLaddressint) ((*text).textPtr);
    TLRESTORELF();
}

void FileManager_CloseFileText (fileNo)
FileManager_FileNoType	fileNo;
{
    TLBINDREG((*file), struct FileManager_FileDef);
    TLBINDREG((*text), struct FileManager_TextDef);
    TLSTKCHKSLF(101127);
    TLSETF();
    TLSETL(101127);
    file = &(FileManager_fileDescriptors[TLINRANGELOW(fileNo, 1, 500, 1)]);
    TLINCL();
    text = &(FileManager_textDescriptors[TLINRANGELOW((*file).textNo, 1, 500, 1)]);
    TLSETL(101130);
    TLASSERT (((((*file).allocated) && (((*text).refCount) > 0)) && (((*text).openCount) > 0)) && (((*file).textOpen) > 0));
    TLSETL(101133);
    (*file).textOpen -= 1;
    TLINCL();
    (*text).openCount -= 1;
    TLRESTORELF();
}

void FileManager_CloseAllText () {
    TLSTKCHKSLF(101142);
    TLSETF();
    TLSETL(101142);
    {
	register TLint4	i;
	for (i = 1; i <= 500; i++) {
	    TLBINDREG((*file), struct FileManager_FileDef);
	    TLSETL(101143);
	    file = &(FileManager_fileDescriptors[TLINRANGELOW(i, 1, 500, 1)]);
	    TLINCL();
	    if (((*file).allocated) && (((*file).textOpen) > 0)) {
		TLBINDREG((*text), struct FileManager_TextDef);
		TLSETL(101145);
		text = &(FileManager_textDescriptors[TLINRANGELOW((*file).textNo, 1, 500, 1)]);
		TLINCL();
		(*text).openCount -= (*file).textOpen;
		TLINCL();
		(*file).textOpen = 0;
	    };
	};
    };
    TLRESTORELF();
}

void FileManager_ChangeDirectory (path)
TLstring	path;
{
    TLstring	ndir;
    TLSTKCHKSLF(101161);
    TLSETF();
    TLSETL(101161);
    {
	TLstring	__x578;
	FileManager_expand(path, (FileManager_FileNoType) 0, __x578);
	TLSTRASS(255, ndir, __x578);
    };
    TLINCL();
    if (CanCD(ndir)) {
	TLSETL(101163);
	{
	    TLchar	__x579[2];
	    TL_TLS_TLSBS(__x579, (TLint4) 0, ndir);
	    if (strcmp(__x579, "\\") != 0) {
		TLSETL(101164);
		{
		    TLstring	__x580;
		    TL_TLS_TLSCAT(ndir, "\\", __x580);
		    TLSTRASS(255, FileManager_wd, __x580);
		};
	    } else {
		TLSETL(101166);
		TLSTRASS(255, FileManager_wd, ndir);
	    };
	};
    } else {
	TLSETL(101172);
	{
	    TLstring	__x581;
	    TL_TLS_TLSCAT("Couldn\'t change to: ", ndir, __x581);
	    OkMsgBox(__x581);
	};
    };
    TLRESTORELF();
}

void FileManager_GetDirectory (absolute, display)
TLstring	absolute;
TLstring	display;
{
    TLSTKCHKSLF(101179);
    TLSETF();
    TLSETL(101179);
    TLSTRASS(255, absolute, FileManager_wd);
    TLINCL();
    TLSTRASS(255, display, FileManager_wd);
    TLRESTORELF();
}

void FileManager_ChangeExecDirectory (path)
TLstring	path;
{
    TLstring	ndir;
    TLSTKCHKSLF(101187);
    TLSETF();
    TLSETL(101187);
    {
	TLstring	__x582;
	FileManager_expand1(path, FileManager_ed, (FileManager_FileNoType) 0, __x582);
	TLSTRASS(255, ndir, __x582);
    };
    TLINCL();
    if (CanCD(ndir)) {
	TLSETL(101189);
	{
	    TLchar	__x583[2];
	    TL_TLS_TLSBS(__x583, (TLint4) 0, ndir);
	    if (strcmp(__x583, "\\") != 0) {
		TLSETL(101190);
		{
		    TLstring	__x584;
		    TL_TLS_TLSCAT(ndir, "\\", __x584);
		    TLSTRASS(255, FileManager_ed, __x584);
		};
	    } else {
		TLSETL(101192);
		TLSTRASS(255, FileManager_ed, ndir);
	    };
	};
    } else {
	TLSETL(101198);
	{
	    TLstring	__x585;
	    TL_TLS_TLSCAT("Couldn\'t change to: ", ndir, __x585);
	    OkMsgBox(__x585);
	};
    };
    TLRESTORELF();
}

void FileManager_GetExecDirectory (absolute)
TLstring	absolute;
{
    TLSTKCHKSLF(101205);
    TLSETF();
    TLSETL(101205);
    TLSTRASS(255, absolute, FileManager_ed);
    TLRESTORELF();
}

void FileManager_ShortName (name, __x92)
TLstring	name;
TLstring	__x92;
{
    TLstring	sname;
    TLSTKCHKSLF(101217);
    TLSETF();
    TLSETL(101217);
    {
	TLstring	__x586;
	FileManager_lastComponent(name, __x586);
	TLSTRASS(255, sname, __x586);
    };
    TLINCL();
    {
	TLchar	__x587[2];
	TL_TLS_TLSBS(__x587, (TLint4) 0, sname);
	if (strcmp(__x587, ".") == 0) {
	    TLSETL(101219);
	    {
		TLstring	__x588;
		TL_TLS_TLSBXS(__x588, (TLint4) -1, (TLint4) 1, sname);
		TLSTRASS(255, sname, __x588);
	    };
	};
    };
    TLSETL(101221);
    {
	TLSTRASS(255, __x92, sname);
	TLRESTORELF();
	return;
    };
    TLRESTORELF();
    /* NOTREACHED */
    TLFCNRESULTABORT;
}

void FileManager_PrefixedName (name, __x98)
TLstring	name;
TLstring	__x98;
{
    TLstring	path0;
    TLstring	tpath;
    TLint4	lenp0;
    TLint4	ind;
    TLint4	len;
    TLSTKCHKSLF(101228);
    TLSETF();
    TLSETL(101228);
    {
	TLstring	__x589;
	FileManager_expand(name, (FileManager_FileNoType) 0, __x589);
	TLSTRASS(255, path0, __x589);
    };
    TLSETL(101230);
    TLSTRASS(255, tpath, path0);
    TLINCL();
    {
	TLchar	__x590[2];
	TL_TLS_TLSBS(__x590, (TLint4) 0, tpath);
	if (strcmp(__x590, "\\") != 0) {
	    TLSETL(101232);
	    TLSTRCATASS(tpath, "\\", 255);
	};
    };
    TLSETL(101235);
    lenp0 = TL_TLS_TLSLEN(tpath);
    TLSETL(101237);
    ind = 0;
    TLINCL();
    len = 0;
    TLSETL(101242);
    {
	register TLint4	i;
	for (i = 1; i <= 5; i++) {
	    TLint4	lendn;
	    TLSETL(101243);
	    lendn = TL_TLS_TLSLEN((FileManager_Dirs[TLINRANGELOW(i, 1, 5, 1)]));
	    TLSETL(101245);
	    {
		TLstring	__x591;
		if ((lenp0 >= lendn) && ((TL_TLS_TLSBXX(__x591, (TLint4) lendn, (TLint4) 1, tpath), IsSamePath(__x591, FileManager_Dirs[TLINRANGELOW(i, 1, 5, 1)])))) {
		    TLSETL(101247);
		    if (lendn > len) {
			TLSETL(101248);
			ind = i;
			TLINCL();
			len = lendn;
		    };
		};
	    };
	};
    };
    TLSETL(101254);
    if (ind == 0) {
	TLSETL(101256);
	{
	    TLSTRASS(255, __x98, path0);
	    TLRESTORELF();
	    return;
	};
    } else {
	TLSETL(101257);
	if (lenp0 > TL_TLS_TLSLEN((FileManager_Dirs[TLINRANGELOW(ind, 1, 5, 1)]))) {
	    TLSETL(101258);
	    {
		{
		    TLstring	__x594;
		    TL_TLS_TLSBXS(__x594, (TLint4) 0, (TLint4) TL_TLS_TLSLEN((FileManager_Dirs[TLINRANGELOW(ind, 1, 5, 1)])), path0);
		    {
			TLstring	__x593;
			TL_TLS_TLSCAT("%", FileManager_prefix[TLINRANGELOW(ind, 1, 5, 1)], __x593);
			{
			    TLstring	__x592;
			    TL_TLS_TLSCAT(__x593, __x594, __x592);
			    TLSTRASS(255, __x98, __x592);
			};
		    };
		};
		TLRESTORELF();
		return;
	    };
	} else {
	    TLSETL(101260);
	    {
		{
		    TLstring	__x595;
		    TL_TLS_TLSCAT("%", FileManager_prefix[TLINRANGELOW(ind, 1, 5, 1)], __x595);
		    TLSTRASS(255, __x98, __x595);
		};
		TLRESTORELF();
		return;
	    };
	};
    };
    TLRESTORELF();
    /* NOTREACHED */
    TLFCNRESULTABORT;
}

void FileManager_PathName (name, __x94)
TLstring	name;
TLstring	__x94;
{
    TLSTKCHKSLF(101268);
    TLSETF();
    TLSETL(101268);
    {
	{
	    TLstring	__x596;
	    FileManager_expand(name, (FileManager_FileNoType) 0, __x596);
	    TLSTRASS(255, __x94, __x596);
	};
	TLRESTORELF();
	return;
    };
    TLRESTORELF();
    /* NOTREACHED */
    TLFCNRESULTABORT;
}

void FileManager_ExecPathName (name, __x96)
TLstring	name;
TLstring	__x96;
{
    TLSTKCHKSLF(101274);
    TLSETF();
    TLSETL(101274);
    {
	{
	    TLstring	__x597;
	    FileManager_expand1(name, FileManager_ed, (FileManager_FileNoType) 0, __x597);
	    TLSTRASS(255, __x96, __x597);
	};
	TLRESTORELF();
	return;
    };
    TLRESTORELF();
    /* NOTREACHED */
    TLFCNRESULTABORT;
}

void FileManager_ExtendName (path, prefix, postfix, __x102)
TLstring	path;
TLstring	prefix;
TLstring	postfix;
TLstring	__x102;
{
    TLSTKCHKSLF(101281);
    TLSETF();
    TLSETL(101281);
    {
	TLSTRASS(255, __x102, "");
	TLRESTORELF();
	return;
    };
    TLRESTORELF();
    /* NOTREACHED */
    TLFCNRESULTABORT;
}

void FileManager_SetLongFileNameFlag (flag)
TLboolean	flag;
{
    TLSTKCHKSLF(101288);
    TLSETF();
    TLSETL(101288);
    FileManager_longFileNames = flag;
    TLRESTORELF();
}

void FileManager_FileName (fileNo, __x105)
FileManager_FileNoType	fileNo;
TLstring	__x105;
{
    TLSTKCHKSLF(101300);
    TLSETF();
    TLSETL(101300);
    if (fileNo == 0) {
	TLSETL(101301);
	{
	    TLSTRASS(255, __x105, "<No file>");
	    TLRESTORELF();
	    return;
	};
    } else {
	TLBINDREG((*file), struct FileManager_FileDef);
	TLSETL(101303);
	file = &(FileManager_fileDescriptors[TLINRANGELOW(fileNo, 1, 500, 1)]);
	TLINCL();
	TLASSERT ((*file).allocated);
	TLSETL(101306);
	if (FileManager_sameFile(&(FileManager_nullFID), &((*file).did))) {
	    TLSETL(101307);
	    {
		TLSTRASS(255, __x105, (*file).uname);
		TLRESTORELF();
		return;
	    };
	} else {
	    TLSETL(101310);
	    {
		TLSTRASS(255, __x105, (*file).name);
		TLRESTORELF();
		return;
	    };
	};
    };
    TLRESTORELF();
    /* NOTREACHED */
    TLFCNRESULTABORT;
}

TLboolean FileManager_IsUnnamed (fileNo)
FileManager_FileNoType	fileNo;
{
    TLBINDREG((*file), struct FileManager_FileDef);
    TLSTKCHKSLF(101319);
    TLSETF();
    TLSETL(101319);
    file = &(FileManager_fileDescriptors[TLINRANGELOW(fileNo, 1, 500, 1)]);
    TLINCL();
    TLRESTORELF();
    return (FileManager_sameFile(&((*file).did), &(FileManager_nullFID)));
    TLRESTORELF();
    /* NOTREACHED */
    TLFCNRESULTABORT;
}

TLboolean FileManager_IsOnDisk (fileNo)
FileManager_FileNoType	fileNo;
{
    TLBINDREG((*file), struct FileManager_FileDef);
    TLBINDREG((*text), struct FileManager_TextDef);
    TLSTKCHKSLF(101327);
    TLSETF();
    TLSETL(101327);
    file = &(FileManager_fileDescriptors[TLINRANGELOW(fileNo, 1, 500, 1)]);
    TLINCL();
    text = &(FileManager_textDescriptors[TLINRANGELOW((*file).textNo, 1, 500, 1)]);
    TLINCL();
    TLRESTORELF();
    return ((!FileManager_sameFile(&((*text).fid), &(FileManager_nullFID))) && Exists((*file).name));
    TLRESTORELF();
    /* NOTREACHED */
    TLFCNRESULTABORT;
}

TLboolean FileManager_IsSameFile (fileNo1, fileNo2)
FileManager_FileNoType	fileNo1;
FileManager_FileNoType	fileNo2;
{
    TLBIND((*file1), struct FileManager_FileDef);
    TLBIND((*file2), struct FileManager_FileDef);
    TLSTKCHKSLF(101336);
    TLSETF();
    TLSETL(101336);
    file1 = &(FileManager_fileDescriptors[TLINRANGELOW(fileNo1, 1, 500, 1)]);
    file2 = &(FileManager_fileDescriptors[TLINRANGELOW(fileNo2, 1, 500, 1)]);
    TLSETL(101338);
    TLRESTORELF();
    return (((*file1).textNo) == ((*file2).textNo));
    TLRESTORELF();
    /* NOTREACHED */
    TLFCNRESULTABORT;
}

TLboolean FileManager_IsReadOK (path)
TLstring	path;
{
    TLstring	f;
    TLSTKCHKSLF(101352);
    TLSETF();
    TLSETL(101352);
    {
	TLstring	__x598;
	FileManager_expand(path, (FileManager_FileNoType) 0, __x598);
	TLSTRASS(255, f, __x598);
    };
    TLINCL();
    TLRESTORELF();
    return (Exists(f) && CanRead(f));
    TLRESTORELF();
    /* NOTREACHED */
    TLFCNRESULTABORT;
}

TLboolean FileManager_IsWriteOK (path)
TLstring	path;
{
    TLstring	f;
    TLSTKCHKSLF(101361);
    TLSETF();
    TLSETL(101361);
    {
	TLstring	__x599;
	FileManager_expand(path, (FileManager_FileNoType) 0, __x599);
	TLSTRASS(255, f, __x599);
    };
    TLINCL();
    if (Exists(f)) {
	TLSETL(101363);
	TLRESTORELF();
	return (CanReadWrite(f));
    } else {
	TLstring	dirname;
	TLSETL(101365);
	{
	    TLstring	__x600;
	    FileManager_baseDir(f, __x600);
	    TLSTRASS(255, dirname, __x600);
	};
	TLINCL();
	TLRESTORELF();
	return (Exists(dirname) && CanReadWrite(dirname));
    };
    TLRESTORELF();
    /* NOTREACHED */
    TLFCNRESULTABORT;
}

TLboolean FileManager_IsOlder (f1, f2)
TLstring	f1;
TLstring	f2;
{
    TLstring	n1;
    TLstring	n2;
    TLboolean	e1;
    TLboolean	e2;
    TLSTKCHKSLF(101375);
    TLSETF();
    TLSETL(101375);
    {
	TLstring	__x601;
	FileManager_expand(f1, (FileManager_FileNoType) 0, __x601);
	TLSTRASS(255, n1, __x601);
    };
    TLINCL();
    {
	TLstring	__x602;
	FileManager_expand(f2, (FileManager_FileNoType) 0, __x602);
	TLSTRASS(255, n2, __x602);
    };
    TLSETL(101378);
    e1 = Exists(n1);
    TLINCL();
    e2 = Exists(n2);
    TLSETL(101381);
    TLASSERT (e1 || e2);
    TLSETL(101383);
    if (e1 && e2) {
	struct FileManager_FID	fid1;
	struct FileManager_FID	fid2;
	TLSETL(101384);
	{
	    struct FileManager_FID	__x603;
	    GetFID(n1, &(__x603));
	    TLSTRCTASS(fid1, __x603, struct FileManager_FID);
	};
	TLINCL();
	{
	    struct FileManager_FID	__x604;
	    GetFID(n2, &(__x604));
	    TLSTRCTASS(fid2, __x604, struct FileManager_FID);
	};
	TLINCL();
	TLRESTORELF();
	return ((fid1.t) < (fid2.t));
    } else {
	TLSETL(101387);
	if (e1) {
	    TLSETL(101388);
	    TLRESTORELF();
	    return (1);
	} else {
	    TLSETL(101390);
	    TLRESTORELF();
	    return (0);
	};
    };
    TLRESTORELF();
    /* NOTREACHED */
    TLFCNRESULTABORT;
}

TLboolean FileManager_FileExists (f)
TLstring	f;
{
    TLstring	path;
    TLSTKCHKSLF(101398);
    TLSETF();
    TLSETL(101398);
    {
	TLstring	__x605;
	FileManager_expand(f, (FileManager_FileNoType) 0, __x605);
	TLSTRASS(255, path, __x605);
    };
    TLINCL();
    TLRESTORELF();
    return (Exists(path));
    TLRESTORELF();
    /* NOTREACHED */
    TLFCNRESULTABORT;
}

void FileManager_AddReference (fileNo)
FileManager_FileNoType	fileNo;
{
    TLBINDREG((*file), struct FileManager_FileDef);
    TLBINDREG((*text), struct FileManager_TextDef);
    TLSTKCHKSLF(101411);
    TLSETF();
    TLSETL(101411);
    file = &(FileManager_fileDescriptors[TLINRANGELOW(fileNo, 1, 500, 1)]);
    TLINCL();
    text = &(FileManager_textDescriptors[TLINRANGELOW((*file).textNo, 1, 500, 1)]);
    TLINCL();
    TLASSERT (((*file).allocated) && (((*text).refCount) > 0));
    TLSETL(101415);
    (*file).refCount += 1;
    TLINCL();
    (*text).fileRefs += 1;
    TLSETL(101418);
    if (((*text).fileRefs) == 1) {
	TLSETL(101419);
	(*text).timeStamp = 1;
    };
    TLRESTORELF();
}

void FileManager_DeleteReference (fileNo)
FileManager_FileNoType	fileNo;
{
    TLBINDREG((*file), struct FileManager_FileDef);
    TLBINDREG((*text), struct FileManager_TextDef);
    TLSTKCHKSLF(101428);
    TLSETF();
    TLSETL(101428);
    file = &(FileManager_fileDescriptors[TLINRANGELOW(fileNo, 1, 500, 1)]);
    TLINCL();
    text = &(FileManager_textDescriptors[TLINRANGELOW((*file).textNo, 1, 500, 1)]);
    TLINCL();
    TLASSERT (((((*file).allocated) && (((*text).refCount) > 0)) && (((*file).refCount) > 0)) && (((*text).fileRefs) >= ((*file).refCount)));
    TLSETL(101433);
    (*file).refCount -= 1;
    TLINCL();
    (*text).fileRefs -= 1;
    TLRESTORELF();
}

void FileManager_ReadTimeStamp (fileNo, timeStamp)
FileManager_FileNoType	fileNo;
FileManager_FileTimeStamp	*timeStamp;
{
    TLBINDREG((*file), struct FileManager_FileDef);
    TLBINDREG((*text), struct FileManager_TextDef);
    TLSTKCHKSLF(101443);
    TLSETF();
    TLSETL(101443);
    file = &(FileManager_fileDescriptors[TLINRANGELOW(fileNo, 1, 500, 1)]);
    TLINCL();
    text = &(FileManager_textDescriptors[TLINRANGELOW((*file).textNo, 1, 500, 1)]);
    TLINCL();
    TLASSERT ((((*file).allocated) && (((*file).refCount) > 0)) && (((*text).refCount) > 0));
    TLSETL(101447);
    (*timeStamp) = (*text).timeStamp;
    TLINCL();
    (*file).lastStamp = (*timeStamp);
    TLRESTORELF();
}

void FileManager_FlushUnreferencedFiles () {
    TLSTKCHKSLF(101456);
    TLSETF();
    TLSETL(101456);
    {
	register TLint4	i;
	for (i = 1; i <= 500; i++) {
	    TLBINDREG((*file), struct FileManager_FileDef);
	    TLSETL(101457);
	    file = &(FileManager_fileDescriptors[TLINRANGELOW(i, 1, 500, 1)]);
	    TLINCL();
	    if (((((*file).allocated) && (((*file).handleOpen) == 0)) && (((*file).textOpen) == 0)) && (((*file).refCount) == 0)) {
		TLBINDREG((*text), struct FileManager_TextDef);
		TLSETL(101461);
		text = &(FileManager_textDescriptors[TLINRANGELOW((*file).textNo, 1, 500, 1)]);
		TLINCL();
		(*file).allocated = 0;
		TLINCL();
		(*text).refCount -= 1;
		TLINCL();
		if (((*text).refCount) == 0) {
		    TLSETL(101466);
		    FreeFile(&((*text).textPtr));
		};
	    };
	};
    };
    TLRESTORELF();
}

void FileManager_CheckReferencedFiles () {
    TLSTKCHKSLF(101476);
    TLSETF();
    TLSETL(101476);
    {
	register TLint4	fileNo;
	for (fileNo = 1; fileNo <= 500; fileNo++) {
	    TLBINDREG((*file), struct FileManager_FileDef);
	    TLSETL(101477);
	    file = &(FileManager_fileDescriptors[TLINRANGELOW(fileNo, 1, 500, 1)]);
	    TLINCL();
	    if (((*file).allocated) && (((*file).refCount) > 0)) {
		TLBINDREG((*text), struct FileManager_TextDef);
		TLSETL(101479);
		text = &(FileManager_textDescriptors[TLINRANGELOW((*file).textNo, 1, 500, 1)]);
		TLSETL(101484);
		if (((*text).openCount) == 0) {
		    FileManager_ResultCode	rc;
		    TLSETL(101488);
		    FileManager_openFile((FileManager_FileNoType) fileNo, &(rc));
		};
	    };
	};
    };
    TLRESTORELF();
}

void FileManager_RmFile (path)
TLstring	path;
{
    TLSTKCHKSLF(101504);
    TLSETF();
    TLSETL(101504);
    {
	TLstring	__x606;
	FileManager_expand(path, (FileManager_FileNoType) 0, __x606);
	unlink(__x606);
    };
    TLRESTORELF();
}

void FileManager_SetDefaultInclude (path)
TLstring	path;
{
    TLstring	ndir;
    TLSTKCHKSLF(101510);
    TLSETF();
    TLSETL(101510);
    {
	TLstring	__x607;
	FileManager_expand(path, (FileManager_FileNoType) 0, __x607);
	TLSTRASS(255, ndir, __x607);
    };
    TLINCL();
    if (CanCD(ndir)) {
	TLSETL(101512);
	{
	    TLchar	__x608[2];
	    TL_TLS_TLSBS(__x608, (TLint4) 0, ndir);
	    if (strcmp(__x608, "\\") != 0) {
		TLSETL(101513);
		{
		    TLstring	__x609;
		    TL_TLS_TLSCAT(ndir, "\\", __x609);
		    TLSTRASS(255, FileManager_Dirs[0], __x609);
		};
	    } else {
		TLSETL(101515);
		TLSTRASS(255, FileManager_Dirs[0], ndir);
	    };
	};
	TLSETL(101517);
	{
	    TLstring	__x610;
	    TL_TLS_TLSCAT(FileManager_Dirs[0], "help\\", __x610);
	    TLSTRASS(255, FileManager_Dirs[3], __x610);
	};
    } else {
	TLSETL(101522);
	{
	    TLstring	__x612;
	    TL_TLS_TLSCAT("FileManager_SetDefaultInclude - bad directory \'", ndir, __x612);
	    {
		TLstring	__x611;
		TL_TLS_TLSCAT(__x612, "\'", __x611);
		OkMsgBox(__x611);
	    };
	};
    };
    TLRESTORELF();
}

void FileManager_SetHomeDir (path)
TLstring	path;
{
    TLstring	ndir;
    TLSTKCHKSLF(101529);
    TLSETF();
    TLSETL(101529);
    {
	TLstring	__x613;
	FileManager_expand(path, (FileManager_FileNoType) 0, __x613);
	TLSTRASS(255, ndir, __x613);
    };
    TLINCL();
    if (CanCD(ndir)) {
	TLSETL(101531);
	{
	    TLchar	__x614[2];
	    TL_TLS_TLSBS(__x614, (TLint4) 0, ndir);
	    if (strcmp(__x614, "\\") != 0) {
		TLSETL(101532);
		{
		    TLstring	__x615;
		    TL_TLS_TLSCAT(ndir, "\\", __x615);
		    TLSTRASS(255, FileManager_Dirs[1], __x615);
		};
	    } else {
		TLSETL(101534);
		TLSTRASS(255, FileManager_Dirs[1], ndir);
	    };
	};
    } else {
	TLSETL(101540);
	{
	    TLstring	__x617;
	    TL_TLS_TLSCAT("FileManager_SetHomeDir - bad directory \'", ndir, __x617);
	    {
		TLstring	__x616;
		TL_TLS_TLSCAT(__x617, "\'", __x616);
		OkMsgBox(__x616);
	    };
	};
    };
    TLRESTORELF();
}

void FileManager_SetJobDir (path)
TLstring	path;
{
    TLstring	ndir;
    TLSTKCHKSLF(101546);
    TLSETF();
    TLSETL(101546);
    {
	TLstring	__x618;
	FileManager_expand(path, (FileManager_FileNoType) 0, __x618);
	TLSTRASS(255, ndir, __x618);
    };
    TLINCL();
    if (CanCD(ndir)) {
	TLSETL(101548);
	{
	    TLchar	__x619[2];
	    TL_TLS_TLSBS(__x619, (TLint4) 0, ndir);
	    if (strcmp(__x619, "\\") != 0) {
		TLSETL(101549);
		{
		    TLstring	__x620;
		    TL_TLS_TLSCAT(ndir, "\\", __x620);
		    TLSTRASS(255, FileManager_Dirs[2], __x620);
		};
	    } else {
		TLSETL(101551);
		TLSTRASS(255, FileManager_Dirs[2], ndir);
	    };
	};
    } else {
	TLSETL(101557);
	{
	    TLstring	__x622;
	    TL_TLS_TLSCAT("FileManager_SetJobDir - bad directory \'", ndir, __x622);
	    {
		TLstring	__x621;
		TL_TLS_TLSCAT(__x622, "\'", __x621);
		OkMsgBox(__x621);
	    };
	};
    };
    TLRESTORELF();
}

void FileManager_AssertTextClosed () {
    TLSTKCHKSLF(101565);
    TLSETF();
    TLSETL(101565);
    {
	register TLint4	i;
	for (i = 1; i <= 500; i++) {
	    TLBINDREG((*file), struct FileManager_FileDef);
	    TLSETL(101566);
	    file = &(FileManager_fileDescriptors[TLINRANGELOW(i, 1, 500, 1)]);
	    TLINCL();
	    TLASSERT ((!((*file).allocated)) || (((*file).textOpen) == 0));
	};
    };
    TLRESTORELF();
}

void FileManager_Dump () {
    TLSTKCHKSLF(101575);
    TLSETF();
    TLSETL(101575);
    TL_TLI_TLISS ((TLint4) 0, (TLint2) 2);
    TL_TLI_TLIPS ((TLint4) 0, "FILE", (TLint2) 0);
    TL_TLI_TLIPK ((TLint2) 0);
    TLINCL();
    {
	register TLint4	i;
	for (i = 1; i <= 500; i++) {
	    TLBIND((*file), struct FileManager_FileDef);
	    TLSETL(101577);
	    file = &(FileManager_fileDescriptors[TLINRANGELOW(i, 1, 500, 1)]);
	    TLINCL();
	    if ((*file).allocated) {
		TLSETL(101579);
		TL_TLI_TLISS ((TLint4) 0, (TLint2) 2);
		TL_TLI_TLIPI ((TLint4) 2, (TLint4) i, (TLint2) 0);
		TL_TLI_TLIPS ((TLint4) 0, " ", (TLint2) 0);
		TL_TLI_TLIPS ((TLint4) 0, ((*file).name), (TLint2) 0);
		TL_TLI_TLIPS ((TLint4) 0, " text ", (TLint2) 0);
		TL_TLI_TLIPN ((TLint4) 0, (TLnat4) ((*file).textNo), (TLint2) 0);
		TL_TLI_TLIPS ((TLint4) 0, " (", (TLint2) 0);
		TL_TLI_TLIPN ((TLint4) 0, (TLnat4) ((*file).handleOpen), (TLint2) 0);
		TL_TLI_TLIPS ((TLint4) 0, ",", (TLint2) 0);
		TL_TLI_TLIPN ((TLint4) 0, (TLnat4) ((*file).textOpen), (TLint2) 0);
		TL_TLI_TLIPS ((TLint4) 0, ",", (TLint2) 0);
		TL_TLI_TLIPN ((TLint4) 0, (TLnat4) ((*file).refCount), (TLint2) 0);
		TL_TLI_TLIPS ((TLint4) 0, "), ", (TLint2) 0);
		TL_TLI_TLIPS ((TLint4) 0, " (", (TLint2) 0);
		TL_TLI_TLIPN ((TLint4) 0, (TLnat4) ((*file).lastStamp), (TLint2) 0);
		TL_TLI_TLIPS ((TLint4) 0, ")", (TLint2) 0);
		TL_TLI_TLIPK ((TLint2) 0);
	    };
	};
    };
    TLSETL(101587);
    TL_TLI_TLISS ((TLint4) 0, (TLint2) 2);
    TL_TLI_TLIPK ((TLint2) 0);
    TL_TLI_TLIPS ((TLint4) 0, "TEXT", (TLint2) 0);
    TL_TLI_TLIPK ((TLint2) 0);
    TLINCL();
    {
	register TLint4	i;
	for (i = 1; i <= 500; i++) {
	    TLBIND((*text), struct FileManager_TextDef);
	    TLSETL(101589);
	    text = &(FileManager_textDescriptors[TLINRANGELOW(i, 1, 500, 1)]);
	    TLINCL();
	    if (((*text).refCount) > 0) {
		TLSETL(101591);
		{
		    TLstring	__x623;
		    TL_TLS_TLSVNS((TLnat4) ((*text).textPtr), (TLint4) 0, (TLint4) 16, __x623);
		    TL_TLI_TLISS ((TLint4) 0, (TLint2) 2);
		    TL_TLI_TLIPI ((TLint4) 2, (TLint4) i, (TLint2) 0);
		    TL_TLI_TLIPS ((TLint4) 0, " ", (TLint2) 0);
		    TL_TLI_TLIPN ((TLint4) 0, (TLnat4) ((*text).textSize), (TLint2) 0);
		    TL_TLI_TLIPS ((TLint4) 0, " bytes @ 16#", (TLint2) 0);
		    TL_TLI_TLIPS ((TLint4) 0, __x623, (TLint2) 0);
		    TL_TLI_TLIPS ((TLint4) 0, " (", (TLint2) 0);
		    TL_TLI_TLIPN ((TLint4) 0, (TLnat4) ((*text).refCount), (TLint2) 0);
		    TL_TLI_TLIPS ((TLint4) 0, ",", (TLint2) 0);
		    TL_TLI_TLIPN ((TLint4) 0, (TLnat4) ((*text).openCount), (TLint2) 0);
		    TL_TLI_TLIPS ((TLint4) 0, ",", (TLint2) 0);
		    TL_TLI_TLIPN ((TLint4) 0, (TLnat4) ((*text).timeStamp), (TLint2) 0);
		    TL_TLI_TLIPS ((TLint4) 0, ")", (TLint2) 0);
		    TL_TLI_TLIPK ((TLint2) 0);
		};
	    };
	};
    };
    TLRESTORELF();
}

static TLboolean __x624;
void FileManager () {
    TLSAVELF();
    TLSETF();
    if (__x624 == 0) {
	__x624 = 1;
	TLSETL(100050);
	{
	    TLstring	__x449;
	    CurrentDirectory(__x449);
	    TLSTRASS(255, FileManager_wd, __x449);
	};
	TLSETL(100056);
	{
	    TLstring	__x450;
	    CurrentDirectory(__x450);
	    TLSTRASS(255, FileManager_ed, __x450);
	};
	TLSETL(100059);
	FileManager_longFileNames = 1;
	TLSETL(100111);
	{
	    register TLint4	i;
	    for (i = 1; i <= 500; i++) {
		TLSETL(100111);
		FileManager_textDescriptors[TLINRANGELOW(i, 1, 500, 1)].refCount = 0;
	    };
	};
	TLSETL(100114);
	{
	    register TLint4	i;
	    for (i = 1; i <= 500; i++) {
		TLSETL(100114);
		FileManager_fileDescriptors[TLINRANGELOW(i, 1, 500, 1)].allocated = 0;
	    };
	};
	TLSETL(100120);
	{
	    TLstring	__x453;
	    TempDirectory(__x453);
	    TLSTRASS(255, FileManager_TempDir, __x453);
	};
	TLSETL(100135);
	{
	    register TLint4	i;
	    for (i = 1; i <= 5; i++) {
		TLSETL(100136);
		{
		    TLstring	__x461;
		    IncludeDirectory(__x461);
		    TLSTRASS(255, FileManager_Dirs[TLINRANGELOW(i, 1, 5, 1)], __x461);
		};
	    };
	};
	TLSETL(100139);
	{
	    TLstring	__x462;
	    TempDirectory(__x462);
	    TLSTRASS(255, FileManager_Dirs[4], __x462);
	};
	TLSETL(100143);
	FileManager_unnamedPrefixLen = TL_TLS_TLSLEN("unnamed");
	TLSETL(100145);
	{
	    register TLint4	i;
	    for (i = 1; i <= 20; i++) {
		TLSETL(100146);
		FileManager_unnamedFree[TLINRANGELOW(i, 1, 20, 1)] = 1;
	    };
	};
	TLSETL(100148);
	FileManager_nextUnnamed = 21;
    }
    TLRESTORELF();
}
