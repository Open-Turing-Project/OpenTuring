#include "cinterface"

void Star_initialize ();

void Star_getToken ();

void Star_finalize ();

extern TLaddressint malloc ();

extern void free ();
typedef	TLaddressint	Star_commentListPtr;
struct	Star_commentType {
    TLstring	comment;
    Star_commentListPtr	next;
};
typedef	TLint4	Star___x131[256];
typedef	TLchar	Star___x130[255];
struct	Star_tokenType {
    TLnat4	tokenNo;
    TLstring	tokenString;
    TLnat4	lineno;
    TLint4	fileNo;
    Star_commentListPtr	beginComment;
    Star___x130	fileContents;
    TLint4	lengthFileContents;
    TLnat4	positionInFileContents;
    TLboolean	endOfFile;
    TLchar	nextCh;
    Star___x131	charTokenMap;
};
typedef	TLchar	Star___x134[21];
struct	Star_keys {
    Star___x134	id;
    TLnat4	token;
};
typedef	struct Star_keys	Star___x135[26];
static Star___x135	Star_keywords = 
    {
	{"procedure", 10},
    
	{"proc", 10},
    
	{"function", 11},
    
	{"fcn", 11},
    
	{"unit", 12},
    
	{"export", 13},
    
	{"deferred", 14},
    
	{"type", 15},
    
	{"const", 16},
    
	{"module", 17},
    
	{"class", 18},
    
	{"monitor", 19},
    
	{"var", 20},
    
	{"end", 21},
    
	{"import", 22},
    
	{"body", 23},
    
	{"pervasive", 34},
    
	{"opaque", 35},
    
	{"unqualified", 9},
    
	{"inherit", 32},
    
	{"implement", 33},
    
	{"pointer", 36},
    
	{"set", 37},
    
	{"contain", 25},
    
	{"include", 38},
    
	{"by", 40}};
typedef	TLaddressint	Star_symbolPtr;
typedef	TLchar	Star___x136[21];
struct	Star_symbol {
    Star___x136	id;
    TLnat4	token;
    Star_symbolPtr	next;
};
typedef	Star_symbolPtr	Star___x137[12];
static Star___x137	Star_symtable;

static TLnat4 Star_hash (str)
TLstring	str;
{
    TLnat4	sum;
    TLSTACKCHECK();
    sum = 0;
    {
	register TLint4	i;
	TLint4	__x138;
	__x138 = TL_TLS_TLSLEN(str);
	i = 1;
	if (i <= __x138) {
	    for(;;) {
		{
		    TLchar	__x139[2];
		    TL_TLS_TLSBX(__x139, (TLint4) i, str);
		    sum += ((TLnat4) TLCVTTOCHR(__x139));
		};
		if (i == __x138) break;
		i++;
	    }
	};
    };
    return (sum % 0xB);
    /* NOTREACHED */
    TLFCNRESULTABORT;
}

static void Star_readBuffer_ext (token)
struct Star_tokenType	*token;
{
    TLint4	success;
    TLSTACKCHECK();
    TL_TLI_TLISSS (&success, (TLint4) ((*token).fileNo), (TLint2) 3);
    TL_TLI_TLIRER (&((*token).lengthFileContents), (*token).fileContents, (TLint4) 255, &success, (TLint2) (*token).fileNo);
    (*token).positionInFileContents = 1;
    (*token).endOfFile = ((*token).lengthFileContents) == 0;
}

static void Star_getNextCh_ext (token)
struct Star_tokenType	*token;
{
    TLSTACKCHECK();
    if (((*token).positionInFileContents) > ((*token).lengthFileContents)) {
	Star_readBuffer_ext(&((*token)));
    };
    if (!((*token).endOfFile)) {
	(*token).nextCh = (*token).fileContents[(((*token).positionInFileContents) - 1)];
	(*token).positionInFileContents += 1;
	{
	    TLchar	__x140[2];
	    TLCHRTOSTR((*token).nextCh, __x140);
	    if (strcmp(__x140, "\n") == 0) {
		(*token).lineno += 1;
	    };
	};
    } else {
	(*token).nextCh = '$';
    };
}

static void Star_openFile_ext (fileName, token)
TLstring	fileName;
struct Star_tokenType	*token;
{
    TLSTACKCHECK();
    TL_TLI_TLIOF ((TLnat2) 8, fileName, &((*token).fileNo));
    if (((*token).fileNo) == 0) {
	{
	    TLstring	__x141;
	    TL_TLS_TLSCAT("Sorry, can\'t open: ", fileName, __x141);
	    TL_TLI_TLISSO ();
	    TL_TLI_TLIPS ((TLint4) 0, __x141, (TLint2) -1);
	    TL_TLI_TLIPK ((TLint2) -1);
	};
    };
    Star_readBuffer_ext(&((*token)));
}

static void Star_skipWhiteSpace_ext (token)
struct Star_tokenType	*token;
{
    TLSTACKCHECK();
    for(;;) {
	if (!(((*token).charTokenMap[TLINRANGELOW(((TLnat4) ((*token).nextCh)), 0, 255, 1)]) == 4)) {
	    break;
	};
	Star_getNextCh_ext(&((*token)));
    };
}

static void Star_finalize_ext (token)
struct Star_tokenType	*token;
{
    TLSTACKCHECK();
    TL_TLI_TLICL ((TLint4) ((*token).fileNo));
}

static void Star_getLongComment_ext (token)
struct Star_tokenType	*token;
{
    TLint4	count;
    Star_commentListPtr	commentPtr;
    TLboolean	mightExit;
    TLSTACKCHECK();
    count = 1;
    (*token).tokenNo = 8;
    TLSTRASS(255, (*token).tokenString, "");
    (*token).beginComment = (TLaddressint) malloc((TLint4) sizeof(struct Star_commentType));
    (* (struct Star_commentType *) ((*token).beginComment)).next = (TLaddressint) 0;
    TLSTRASS(255, (* (struct Star_commentType *) ((*token).beginComment)).comment, " ");
    commentPtr = (TLaddressint) ((*token).beginComment);
    mightExit = 0;
    for(;;) {
	Star_getNextCh_ext(&((*token)));
	if (mightExit) {
	    if (((*token).nextCh) == '/') {
		{
		    TLstring	__x145;
		    TL_TLS_TLSBXS(__x145, (TLint4) -1, (TLint4) 1, ((* (struct Star_commentType *) commentPtr).comment));
		    TLSTRASS(255, (* (struct Star_commentType *) commentPtr).comment, __x145);
		};
		break;
	    } else {
		if (((*token).nextCh) != '*') {
		    mightExit = 0;
		};
	    };
	};
	if ((!mightExit) && (((*token).nextCh) == '*')) {
	    mightExit = 1;
	};
	if ((count < 255) && (((*token).nextCh) != '\n')) {
	    {
		TLchar	__x148[2];
		TLCHRTOSTR((*token).nextCh, __x148);
		TLSTRCATASS((* (struct Star_commentType *) commentPtr).comment, __x148, 255);
	    };
	    count += 1;
	} else {
	    count = 1;
	    (* (struct Star_commentType *) commentPtr).next = (TLaddressint) malloc((TLint4) sizeof(struct Star_commentType));
	    commentPtr = (TLaddressint) ((* (struct Star_commentType *) commentPtr).next);
	    (* (struct Star_commentType *) commentPtr).next = (TLaddressint) 0;
	    if (((*token).nextCh) == '\n') {
		TLSTRASS(255, (* (struct Star_commentType *) commentPtr).comment, "");
	    } else {
		{
		    TLchar	__x154[2];
		    TLCHRTOSTR((*token).nextCh, __x154);
		    TLSTRASS(255, (* (struct Star_commentType *) commentPtr).comment, __x154);
		};
	    };
	};
    };
    Star_getNextCh_ext(&((*token)));
}

static void Star_getEndOfLineComment_ext (token)
struct Star_tokenType	*token;
{
    TLint4	count;
    Star_commentListPtr	commentPtr;
    TLSTACKCHECK();
    count = 1;
    (*token).tokenNo = 7;
    TLSTRASS(255, (*token).tokenString, "");
    (*token).beginComment = (TLaddressint) malloc((TLint4) sizeof(struct Star_commentType));
    (* (struct Star_commentType *) ((*token).beginComment)).next = (TLaddressint) 0;
    TLSTRASS(255, (* (struct Star_commentType *) ((*token).beginComment)).comment, " ");
    commentPtr = (TLaddressint) ((*token).beginComment);
    for(;;) {
	Star_getNextCh_ext(&((*token)));
	if (((*token).nextCh) == '\n') {
	    break;
	};
	if (count < 255) {
	    {
		TLchar	__x158[2];
		TLCHRTOSTR((*token).nextCh, __x158);
		TLSTRCATASS((* (struct Star_commentType *) commentPtr).comment, __x158, 255);
	    };
	    count += 1;
	};
    };
    Star_getNextCh_ext(&((*token)));
}

static void Star_getSpecialCharToken_ext (token)
struct Star_tokenType	*token;
{
    TLSTACKCHECK();
    switch ((*token).nextCh) {
	case '/':
	    {
		Star_getNextCh_ext(&((*token)));
		if (((*token).nextCh) == '*') {
		    Star_getLongComment_ext(&((*token)));
		} else {
		    TLSTRASS(255, (*token).tokenString, "/");
		    (*token).tokenNo = 1;
		};
	    }
	    break;
	case '~':
	    {
		(*token).tokenNo = 1;
		TLSTRASS(255, (*token).tokenString, "~");
		Star_getNextCh_ext(&((*token)));
		if (((*token).nextCh) == '.') {
		    (*token).tokenNo = 9;
		    TLSTRASS(255, (*token).tokenString, "~.");
		    Star_getNextCh_ext(&((*token)));
		};
	    }
	    break;
	case '%':
	    {
		Star_getEndOfLineComment_ext(&((*token)));
	    }
	    break;
	case '$':
	    {
		if ((*token).endOfFile) {
		    (*token).tokenNo = 2;
		    TLSTRASS(255, (*token).tokenString, "$");
		};
		Star_getNextCh_ext(&((*token)));
	    }
	    break;
	case '(':
	    {
		(*token).tokenNo = 24;
		{
		    TLchar	__x159[2];
		    TLCHRTOSTR((*token).nextCh, __x159);
		    TLSTRASS(255, (*token).tokenString, __x159);
		};
		Star_getNextCh_ext(&((*token)));
	    }
	    break;
	case ')':
	    {
		(*token).tokenNo = 26;
		{
		    TLchar	__x160[2];
		    TLCHRTOSTR((*token).nextCh, __x160);
		    TLSTRASS(255, (*token).tokenString, __x160);
		};
		Star_getNextCh_ext(&((*token)));
	    }
	    break;
	case ':':
	    {
		(*token).tokenNo = 27;
		{
		    TLchar	__x161[2];
		    TLCHRTOSTR((*token).nextCh, __x161);
		    TLSTRASS(255, (*token).tokenString, __x161);
		};
		Star_getNextCh_ext(&((*token)));
	    }
	    break;
	case '*':
	    {
		(*token).tokenNo = 28;
		{
		    TLchar	__x162[2];
		    TLCHRTOSTR((*token).nextCh, __x162);
		    TLSTRASS(255, (*token).tokenString, __x162);
		};
		Star_getNextCh_ext(&((*token)));
	    }
	    break;
	case ',':
	    {
		(*token).tokenNo = 29;
		{
		    TLchar	__x163[2];
		    TLCHRTOSTR((*token).nextCh, __x163);
		    TLSTRASS(255, (*token).tokenString, __x163);
		};
		Star_getNextCh_ext(&((*token)));
	    }
	    break;
	case '^':
	    {
		(*token).tokenNo = 31;
		{
		    TLchar	__x164[2];
		    TLCHRTOSTR((*token).nextCh, __x164);
		    TLSTRASS(255, (*token).tokenString, __x164);
		};
		Star_getNextCh_ext(&((*token)));
	    }
	    break;
	case '"':
	    {
		(*token).tokenNo = 39;
		{
		    TLchar	__x165[2];
		    TLCHRTOSTR((*token).nextCh, __x165);
		    TLSTRASS(255, (*token).tokenString, __x165);
		};
		Star_getNextCh_ext(&((*token)));
		{
		    TLchar	__x166[2];
		    TLCHRTOSTR((*token).nextCh, __x166);
		    if (strcmp(__x166, "%") == 0) {
			TLSTRASS(255, (*token).tokenString, "\"%");
			(*token).tokenNo = 41;
			Star_getNextCh_ext(&((*token)));
		    };
		};
	    }
	    break;
	default :
	    {
		(*token).tokenNo = 1;
		{
		    TLchar	__x167[2];
		    TLCHRTOSTR((*token).nextCh, __x167);
		    TLSTRASS(255, (*token).tokenString, __x167);
		};
		Star_getNextCh_ext(&((*token)));
	    }
	    break;
    };
}

static void Star_getIdToken_ext (token)
struct Star_tokenType	*token;
{
    TLnat4	head;
    TLSTACKCHECK();
    {
	TLchar	__x168[2];
	TLCHRTOSTR((*token).nextCh, __x168);
	TLSTRASS(255, (*token).tokenString, __x168);
    };
    for(;;) {
	Star_getNextCh_ext(&((*token)));
	if (!(((*token).charTokenMap[TLINRANGELOW(((TLnat4) ((*token).nextCh)), 0, 255, 1)]) == 5)) {
	    break;
	};
	{
	    TLchar	__x169[2];
	    TLCHRTOSTR((*token).nextCh, __x169);
	    TLSTRCATASS((*token).tokenString, __x169, 255);
	};
    };
    head = Star_symtable[TLINRANGELOW(Star_hash((*token).tokenString), 0, 11, 1)];
    (*token).tokenNo = 3;
    for(;;) {
	if (head == 0) {
	    break;
	};
	if (strcmp((* (struct Star_symbol *) head).id, (*token).tokenString) == 0) {
	    (*token).tokenNo = (* (struct Star_symbol *) head).token;
	    break;
	} else {
	    head = (* (struct Star_symbol *) head).next;
	};
    };
}

static void Star_getToken_ext (token)
struct Star_tokenType	*token;
{
    TLSTACKCHECK();
    Star_skipWhiteSpace_ext(&((*token)));
    if (((*token).charTokenMap[TLINRANGELOW(((TLnat4) ((*token).nextCh)), 0, 255, 1)]) == 5) {
	Star_getIdToken_ext(&((*token)));
    } else {
	Star_getSpecialCharToken_ext(&((*token)));
    };
}

static void Star_initialize_ext (fileName, token)
TLstring	fileName;
struct Star_tokenType	*token;
{
    TLSTACKCHECK();
    (*token).lineno = 1;
    (*token).endOfFile = 0;
    {
	register TLint4	i;
	for (i = 0; i <= 255; i++) {
	    (*token).charTokenMap[TLINRANGELOW(i, 0, 255, 1)] = 6;
	};
    };
    (*token).charTokenMap[32] = 4;
    (*token).charTokenMap[9] = 4;
    (*token).charTokenMap[10] = 4;
    {
	register TLint4	L;
	for (L = 97; L <= 122; L++) {
	    (*token).charTokenMap[TLINRANGELOW(L, 0, 255, 1)] = 5;
	};
    };
    {
	register TLint4	L;
	for (L = 65; L <= 90; L++) {
	    (*token).charTokenMap[TLINRANGELOW(L, 0, 255, 1)] = 5;
	};
    };
    (*token).charTokenMap[95] = 5;
    (*token).charTokenMap[46] = 5;
    {
	register TLint4	L;
	for (L = 48; L <= 57; L++) {
	    (*token).charTokenMap[TLINRANGELOW(L, 0, 255, 1)] = 5;
	};
    };
    {
	register TLint4	i;
	for (i = 0; i <= 11; i++) {
	    Star_symtable[TLINRANGELOW(i, 0, 11, 1)] = (TLaddressint) 0;
	};
    };
    {
	register TLint4	i;
	for (i = 1; i <= 26; i++) {
	    Star_symbolPtr	newsym;
	    TLnat4	p;
	    p = Star_hash(Star_keywords[TLINRANGELOW(i, 1, 26, 1)].id);
	    newsym = (TLaddressint) malloc((TLint4) sizeof(struct Star_symbol));
	    TLSTRASS(20, (* (struct Star_symbol *) newsym).id, Star_keywords[TLINRANGELOW(i, 1, 26, 1)].id);
	    (* (struct Star_symbol *) newsym).token = Star_keywords[TLINRANGELOW(i, 1, 26, 1)].token;
	    (* (struct Star_symbol *) newsym).next = (TLaddressint) (Star_symtable[TLINRANGELOW(p, 0, 11, 1)]);
	    Star_symtable[TLINRANGELOW(p, 0, 11, 1)] = (TLaddressint) newsym;
	};
    };
    Star_openFile_ext(fileName, &((*token)));
    Star_getNextCh_ext(&((*token)));
    Star_getToken_ext(&((*token)));
}

void Star_initialize (sp)
TLaddressint	sp;
{
    TLSTACKCHECK();
    Star_initialize_ext((* (TLstring *) ((* (TLaddressint *) sp))), &((* (struct Star_tokenType *) ((* (TLaddressint *) ((unsigned long) sp + 4))))));
}

void Star_getToken (sp)
TLaddressint	sp;
{
    TLSTACKCHECK();
    Star_getToken_ext(&((* (struct Star_tokenType *) ((* (TLaddressint *) sp)))));
}

void Star_finalize (sp)
TLaddressint	sp;
{
    TLSTACKCHECK();
    Star_finalize_ext(&((* (struct Star_tokenType *) ((* (TLaddressint *) sp)))));
}

static TLboolean __x184;
void Star () {
    if (__x184 == 0) {
	__x184 = 1;
    }
}
