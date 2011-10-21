#include "cinterface"
static struct {
    TLint4	dummy;
    char	dummy2[330];
} TLFTAB =
    { 7,
    {'/', 'e', 'x', 'p', 'o', 'r', 't', '/', 'h', 'o', 'm', 'e', '/', 't', 'u', 'r', 'i', 'n', 'g', '/', 'o', 'o', 't', '/', 's', 'r', 'c', '/', 'l', 'a', 'n', 'g', 'u', 'a', 'g', 'e', '/', 'p', 'a', 'r', 'a', '/', 'p', 'a', 'r', 'a', 'O', 'O', 'T', '.', 'c', 'h', '\0',
    '/', 'l', 'o', 'c', 'a', 'l', '/', 'i', 'n', 'c', 'l', 'u', 'd', 'e', '/', 't', 'p', 'l', 'u', 's', '/', 'D', 'O', 'S', '3', '2', '/', 'm', 'e', 'm', 'o', 'r', 'y', '\0',
    '/', 'l', 'o', 'c', 'a', 'l', '/', 'i', 'n', 'c', 'l', 'u', 'd', 'e', '/', 't', 'p', 'l', 'u', 's', '/', 'c', 'o', 'm', 'm', 'o', 'n', '/', 's', 'y', 's', 't', 'e', 'm', '\0',
    '/', 'e', 'x', 'p', 'o', 'r', 't', '/', 'h', 'o', 'm', 'e', '/', 't', 'u', 'r', 'i', 'n', 'g', '/', 'o', 'o', 't', '/', 's', 'r', 'c', '/', 'l', 'a', 'n', 'g', 'u', 'a', 'g', 'e', '/', 'p', 'a', 'r', 'a', '/', 'p', 'a', 'r', 'a', '.', 'i', 'n', '\0',
    '/', 'e', 'x', 'p', 'o', 'r', 't', '/', 'h', 'o', 'm', 'e', '/', 't', 'u', 'r', 'i', 'n', 'g', '/', 'o', 'o', 't', '/', 's', 'r', 'c', '/', 'l', 'a', 'n', 'g', 'u', 'a', 'g', 'e', '/', 'p', 'a', 'r', 'a', '/', 'c', 'h', 'a', 'r', 'c', 'l', 'a', 's', 's', '.', 'i', '\0',
    '/', 'e', 'x', 'p', 'o', 'r', 't', '/', 'h', 'o', 'm', 'e', '/', 't', 'u', 'r', 'i', 'n', 'g', '/', 'o', 'o', 't', '/', 's', 'r', 'c', '/', 'l', 'a', 'n', 'g', 'u', 'a', 'g', 'e', '/', 'p', 'a', 'r', 'a', '/', 'k', 'e', 'y', 'w', 'o', 'r', 'd', 's', '.', 'i', '\0',
    '/', 'e', 'x', 'p', 'o', 'r', 't', '/', 'h', 'o', 'm', 'e', '/', 't', 'u', 'r', 'i', 'n', 'g', '/', 'o', 'o', 't', '/', 's', 'r', 'c', '/', 'l', 'a', 'n', 'g', 'u', 'a', 'g', 'e', '/', 'p', 'a', 'r', 'a', '/', 'f', 'o', 'r', 'm', 'a', 't', '.', 'i', '\0'
    }};

void Paragrapher_Para ();

void Paragrapher_SetKeepCommentColumn ();
typedef	TLchar	Paragrapher___x316[255];
static Paragrapher___x316	Paragrapher_outLine;
static TLint2	Paragrapher_outLinePtr;
static TLint2	Paragrapher_currentLineLength;
static TLint2	Paragrapher_lastBlank;

extern TLaddressint malloc ();

extern TLaddressint realloc ();

extern void free ();
static TLaddressint	Paragrapher_outBuffer;
static TLint4	Paragrapher_outBufferPtr;
static TLint4	Paragrapher_outBufferSize;

static void Paragrapher_NewBuffer (newSize)
TLint4	newSize;
{
    TLaddressint	newBuffer;
    TLSTKCHKSLF(100056);
    TLSETF();
    TLSETL(100056);
    newBuffer = (TLaddressint) realloc((TLaddressint) Paragrapher_outBuffer, (TLint4) newSize);
    TLINCL();
    if ((unsigned long) newBuffer == 0) {
	TLSETL(100059);
	TL_TLE_TLEQUIT ((TLint4) 1, (char *) 0, 0);
    };
    TLSETL(100060);
    Paragrapher_outBuffer = (TLaddressint) newBuffer;
    TLINCL();
    Paragrapher_outBufferSize = newSize;
    TLRESTORELF();
}

extern void memcpy ();

static void Paragrapher_OutputPartialLine () {
    TLSTKCHKSLF(100068);
    TLSETF();
    TLSETL(100068);
    if (((Paragrapher_outBufferPtr + Paragrapher_outLinePtr) - 1) > Paragrapher_outBufferSize) {
	TLSETL(100070);
	Paragrapher_NewBuffer((TLint4) (Paragrapher_outBufferSize * 2));
    };
    TLSETL(100072);
    memcpy((TLaddressint) ((unsigned long) Paragrapher_outBuffer + Paragrapher_outBufferPtr), (TLaddressint) ((unsigned long)Paragrapher_outLine), (TLint4) (Paragrapher_outLinePtr - 1));
    Paragrapher_outBufferPtr += Paragrapher_outLinePtr - 1;
    TLINCL();
    Paragrapher_outLinePtr = 1;
    TLINCL();
    Paragrapher_lastBlank = 0;
    TLRESTORELF();
}

static void Paragrapher_OutputLine () {
    TLSTKCHKSLF(100081);
    TLSETF();
    TLSETL(100081);
    Paragrapher_OutputPartialLine();
    if ((Paragrapher_outBufferPtr + 1) > Paragrapher_outBufferSize) {
	TLSETL(100083);
	Paragrapher_NewBuffer((TLint4) (Paragrapher_outBufferSize * 2));
    };
    TLSETL(100084);
    (* (TLchar *) ((unsigned long) Paragrapher_outBuffer + Paragrapher_outBufferPtr)) = '\n';
    TLINCL();
    Paragrapher_outBufferPtr += 1;
    TLSETL(100087);
    Paragrapher_currentLineLength = 0;
    TLRESTORELF();
}
extern TLint4	TL_TLI_TLIARC;

extern void TL_TLI_TLIFA ();

extern void TL_TLX_TLXGE ();

extern void TL_TLX_TLXDT ();

extern void TL_TLX_TLXTM ();

extern void TL_TLX_TLXCL ();

extern void TL_TLX_TLXSC ();

extern void time ();

extern void TL_TLX_TLXSYS ();

extern TLint4 getpid ();

extern void TL_TLI_TLIFS ();

extern void TL_TLK_TLKUEXIT ();
extern TLnat4	TL_TLK_TLKTIME;
extern TLnat4	TL_TLK_TLKEPOCH;

extern void TL_TLK_TLKUDMPA ();

extern void TL_TLK_TLKCINI ();
extern TLboolean	TL_TLK_TLKCLKON;
extern TLnat4	TL_TLK_TLKHZ;
extern TLnat4	TL_TLK_TLKCRESO;
extern TLnat4	TL_TLK_TLKTIME;
extern TLnat4	TL_TLK_TLKEPOCH;

extern void TL_TLK_TLKPSID ();

extern TLnat4 TL_TLK_TLKPGID ();

extern void TL_TLK_TLKRSETP ();
typedef	TLint1	Paragrapher_State;
static Paragrapher_State	Paragrapher_state;
static Paragrapher_State	Paragrapher_commentState;
static TLboolean	Paragrapher_wasSplit;
static TLint2	Paragrapher_indent;
static TLnat1	Paragrapher_userIndent;
static TLboolean	Paragrapher_lastOutputWasNewline;
static TLstring	Paragrapher_inLine;
static TLstring	Paragrapher_token;
static TLint4	Paragrapher_lineIndex;
typedef	TLint1	Paragrapher_Tok;
typedef	Paragrapher_Tok	Paragrapher___x320[256];
static Paragrapher___x320	Paragrapher_charClass = 
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 8,
    8, 0, 0, 8, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 8, 0, 9, 3, 0, 1, 10, 9,
    10, 10, 10, 10, 10, 10, 2, 7, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6, 10, 10,
    10, 10, 10, 0, 10, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 0, 0, 0, 10, 4, 0, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 0, 10, 0, 10, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0};
typedef	TLint4	Paragrapher_Keyword;
typedef	TLint1	Paragrapher_Indent;
typedef	TLint1	Paragrapher_Space;
typedef	TLchar	Paragrapher___x321[12];
struct	Paragrapher_formatTableEntry {
    Paragrapher___x321	keywordText;
    Paragrapher_Indent	indentBefore, indentAfter;
    Paragrapher_Space	spacingBefore, spacingAfter;
};
typedef	struct Paragrapher_formatTableEntry	Paragrapher___x322[84];
static Paragrapher___x322	Paragrapher_formatTable = 
    {
	{"and", 0, 0, 0, 0},
    
	{"array", 0, 0, 1, 0},
    
	{"asm", 0, 0, 2, 0},
    
	{"assert", 0, 0, 2, 0},
    
	{"begin", 0, 1, 2, 1},
    
	{"bind", 0, 0, 2, 0},
    
	{"body", 0, 0, 2, 0},
    
	{"by", 0, 0, 0, 0},
    
	{"case", 0, 2, 2, 0},
    
	{"class", 0, 1, 2, 0},
    
	{"close", 0, 0, 2, 0},
    
	{"const", 0, 0, 1, 0},
    
	{"decreasing", 0, 0, 0, 0},
    
	{"div", 0, 0, 0, 0},
    
	{"def", 0, 0, 1, 0},
    
	{"deferred", 0, 0, 1, 0},
    
	{"else", 3, 1, 2, 1},
    
	{"elsif", 3, 1, 2, 0},
    
	{"end", 3, 0, 2, 0},
    
	{"export", 0, 0, 2, 0},
    
	{"external", 0, 0, 2, 0},
    
	{"fcn", 0, 1, 2, 0},
    
	{"flexible", 0, 0, 1, 0},
    
	{"for", 0, 1, 2, 0},
    
	{"fork", 0, 0, 2, 0},
    
	{"forward", 0, 0, 1, 0},
    
	{"free", 0, 0, 2, 0},
    
	{"function", 0, 1, 2, 0},
    
	{"get", 0, 0, 2, 0},
    
	{"handler", 0, 1, 2, 1},
    
	{"if", 0, 1, 2, 0},
    
	{"implement", 0, 0, 2, 0},
    
	{"import", 0, 0, 2, 0},
    
	{"in", 0, 0, 0, 0},
    
	{"include", 0, 0, 2, 0},
    
	{"inherit", 0, 0, 2, 0},
    
	{"init", 0, 0, 1, 0},
    
	{"invariant", 0, 0, 2, 0},
    
	{"label", 3, 1, 2, 0},
    
	{"loop", 0, 1, 2, 1},
    
	{"mod", 0, 0, 0, 0},
    
	{"module", 0, 1, 2, 0},
    
	{"monitor", 0, 1, 2, 0},
    
	{"new", 0, 0, 2, 0},
    
	{"not", 0, 0, 0, 0},
    
	{"of", 0, 0, 0, 0},
    
	{"opaque", 0, 0, 0, 0},
    
	{"open", 0, 0, 2, 0},
    
	{"or", 0, 0, 0, 0},
    
	{"packed", 0, 0, 1, 0},
    
	{"pause", 0, 0, 2, 0},
    
	{"pervasive", 0, 0, 0, 0},
    
	{"post", 0, 0, 2, 0},
    
	{"pre", 0, 0, 2, 0},
    
	{"priority", 0, 0, 1, 0},
    
	{"proc", 0, 1, 2, 0},
    
	{"procedure", 0, 1, 2, 0},
    
	{"process", 0, 1, 2, 0},
    
	{"put", 0, 0, 2, 0},
    
	{"quit", 0, 0, 2, 0},
    
	{"read", 0, 0, 2, 0},
    
	{"record", 1, 1, 2, 1},
    
	{"register", 0, 0, 0, 0},
    
	{"rem", 0, 0, 0, 0},
    
	{"result", 0, 0, 2, 0},
    
	{"seek", 0, 0, 2, 0},
    
	{"set", 0, 0, 1, 0},
    
	{"shl", 0, 0, 0, 0},
    
	{"shr", 0, 0, 0, 0},
    
	{"signal", 0, 0, 2, 0},
    
	{"tag", 0, 0, 2, 0},
    
	{"tell", 0, 0, 2, 0},
    
	{"then", 0, 0, 0, 1},
    
	{"timeout", 0, 0, 1, 0},
    
	{"to", 0, 0, 0, 0},
    
	{"type", 0, 0, 2, 0},
    
	{"unchecked", 0, 0, 1, 0},
    
	{"union", 1, 2, 2, 0},
    
	{"unqualified", 0, 0, 0, 0},
    
	{"var", 0, 0, 1, 0},
    
	{"wait", 0, 0, 2, 0},
    
	{"when", 0, 0, 0, 0},
    
	{"write", 0, 0, 2, 0},
    
	{"xor", 0, 0, 0, 0}};
typedef	Paragrapher_Keyword	Paragrapher___x323[26];
static Paragrapher___x323	Paragrapher_formatTableIndex = 
    {1, 5, 9, 13, 17, 22, 29, 30, 31, 0,
    0, 39, 41, 44, 46, 50, 60, 61, 66, 71,
    77, 80, 81, 84, 0, 0};
typedef	TLchar	Paragrapher_CString[256];
static TLboolean	Paragrapher_keepCommentColumn;
static TLint4	Paragrapher_currentInputColumn;
static Paragrapher_Tok	Paragrapher_tokenKind;
static Paragrapher_Tok	Paragrapher_lastTokenKind;
static Paragrapher_Tok	Paragrapher_lastLastTokenKind;
static Paragrapher_Keyword	Paragrapher_lastIdentifierKeyword;
static Paragrapher_Keyword	Paragrapher_lastLastIdentifierKeyword;
static TLchar	Paragrapher_lastSymbol;
static TLchar	Paragrapher_lastLastSymbol;
static Paragrapher_Keyword	Paragrapher_currentStatement;
static TLnat2	Paragrapher_parenDepth;
static TLint2	Paragrapher_maxLineLength;
static TLnat2	Paragrapher_maxIndent;

static void Paragrapher_MySetKeepCommentColumn (newKeepCommentColumn)
TLboolean	newKeepCommentColumn;
{
    TLSTKCHKSLF(400102);
    TLSETF();
    TLSETL(400102);
    Paragrapher_keepCommentColumn = newKeepCommentColumn;
    TLRESTORELF();
}

static void Paragrapher_AdvanceInputColumn (col)
TLint4	col;
{
    TLSTKCHKSLF(400109);
    TLSETF();
    TLSETL(400109);
    if (!Paragrapher_keepCommentColumn) {
	TLSETL(400111);
	TLRESTORELF();
	return;
    };
    TLSETL(400113);
    {
	register TLint4	cnt;
	TLint4	__x324;
	__x324 = col;
	cnt = 1;
	if (cnt <= __x324) {
	    for(;;) {
		TLSETL(400114);
		{
		    TLchar	__x325[2];
		    TL_TLS_TLSBX(__x325, (TLint4) cnt, Paragrapher_inLine);
		    if (strcmp(__x325, "\t") == 0) {
			TLSETL(400115);
			Paragrapher_currentInputColumn += 8 - ((Paragrapher_currentInputColumn - 1) % 8);
		    } else {
			TLSETL(400117);
			Paragrapher_currentInputColumn += 1;
		    };
		};
		if (cnt == __x324) break;
		cnt++;
	    }
	};
    };
    TLRESTORELF();
}

static void Paragrapher_DoIndent () {
    TLSTKCHKSLF(400124);
    TLSETF();
    TLSETL(400124);
    if (Paragrapher_indent < Paragrapher_maxIndent) {
	TLSETL(400125);
	Paragrapher_indent += 4;
    };
    TLSETL(400127);
    Paragrapher_userIndent = 0;
    TLRESTORELF();
}

static void Paragrapher_DoExdent () {
    TLSTKCHKSLF(400132);
    TLSETF();
    TLSETL(400132);
    if (Paragrapher_indent > 0) {
	TLSETL(400133);
	Paragrapher_indent -= 4;
    };
    TLSETL(400135);
    Paragrapher_userIndent = 0;
    TLRESTORELF();
}

static void Paragrapher_SplitOutputBuffer () {
    typedef	TLchar	__x326[255];
    __x326	tempBuff;
    register TLint2	i;
    TLint4	leftoverSize;
    TLSTKCHKSLF(400141);
    TLSETF();
    TLSETL(400141);
    if (Paragrapher_lastBlank == 0) {
	TLSETL(400144);
	Paragrapher_OutputLine();
	TLINCL();
	TLRESTORELF();
	return;
    };
    TLSETL(400149);
    i = 1;
    TLSETL(400151);
    {
	register TLint4	j;
	TLint4	__x327;
	__x327 = Paragrapher_outLinePtr - 1;
	j = Paragrapher_lastBlank;
	if (j <= __x327) {
	    for(;;) {
		TLSETL(400152);
		tempBuff[(i - 1)] = Paragrapher_outLine[(j - 1)];
		TLINCL();
		i += 1;
		if (j == __x327) break;
		j++;
	    }
	};
    };
    TLSETL(400155);
    leftoverSize = i - 1;
    TLSETL(400157);
    Paragrapher_outLinePtr = Paragrapher_lastBlank;
    TLSETL(400160);
    Paragrapher_OutputLine();
    i = 1;
    TLSETL(400164);
    for(;;) {
	TLSETL(400164);
	if ((Paragrapher_outLine[(i - 1)]) == ' ') {
	    TLSETL(400165);
	    Paragrapher_currentLineLength += 1;
	} else {
	    TLSETL(400166);
	    if ((Paragrapher_outLine[(i - 1)]) == '\t') {
		TLSETL(400167);
		Paragrapher_currentLineLength += 8;
	    } else {
		TLSETL(400170);
		break;
	    };
	};
	TLSETL(400171);
	i += 1;
    };
    TLSETL(400175);
    if ((Paragrapher_currentLineLength + 4) < Paragrapher_maxLineLength) {
	TLSETL(400176);
	{
	    register TLint4	__x250;
	    for (__x250 = 1; __x250 <= 4; __x250++) {
		TLSETL(400177);
		Paragrapher_outLine[(i - 1)] = ' ';
		TLINCL();
		i += 1;
	    };
	};
	TLSETL(400180);
	Paragrapher_currentLineLength += 4;
    };
    TLSETL(400184);
    {
	register TLint4	j;
	TLint4	__x328;
	__x328 = leftoverSize;
	j = 1;
	if (j <= __x328) {
	    for(;;) {
		TLSETL(400185);
		Paragrapher_outLine[(i - 1)] = tempBuff[(j - 1)];
		TLINCL();
		i += 1;
		TLINCL();
		Paragrapher_currentLineLength += 1;
		if (j == __x328) break;
		j++;
	    }
	};
    };
    TLSETL(400189);
    Paragrapher_outLinePtr = i;
    TLINCL();
    Paragrapher_lastBlank = 0;
    TLRESTORELF();
}

static void Paragrapher_AddCh (c)
TLchar	c;
{
    TLSTKCHKSLF(400195);
    TLSETF();
    TLSETL(400195);
    if (Paragrapher_outLinePtr == 255) {
	TLSETL(400198);
	Paragrapher_OutputPartialLine();
    };
    TLSETL(400199);
    Paragrapher_outLine[(Paragrapher_outLinePtr - 1)] = c;
    TLINCL();
    Paragrapher_outLinePtr += 1;
    TLINCL();
    Paragrapher_currentLineLength += 1;
    TLINCL();
    if (c == '\t') {
	TLSETL(400203);
	Paragrapher_currentLineLength += 7;
    } else {
	TLSETL(400204);
	if (Paragrapher_currentLineLength > Paragrapher_maxLineLength) {
	    TLSETL(400206);
	    Paragrapher_SplitOutputBuffer();
	} else {
	    TLSETL(400206);
	    if (c == ' ') {
		TLSETL(400207);
		Paragrapher_lastBlank = Paragrapher_outLinePtr;
	    };
	};
    };
    TLRESTORELF();
}

static void Paragrapher_AddString (cs)
Paragrapher_CString	cs;
{
    register TLint2	i;
    TLSTKCHKSLF(400213);
    TLSETF();
    TLSETL(400213);
    i = 1;
    TLSETL(400216);
    for(;;) {
	register TLchar	ch;
	TLSETL(400216);
	ch = cs[(i - 1)];
	TLSETL(400218);
	if (ch == '\0') {
	    break;
	};
	TLSETL(400220);
	if (Paragrapher_outLinePtr == 255) {
	    TLSETL(400223);
	    Paragrapher_OutputPartialLine();
	};
	TLSETL(400224);
	Paragrapher_outLine[(Paragrapher_outLinePtr - 1)] = ch;
	TLINCL();
	Paragrapher_outLinePtr += 1;
	TLINCL();
	Paragrapher_currentLineLength += 1;
	TLINCL();
	if ((Paragrapher_currentLineLength > Paragrapher_maxLineLength) && (Paragrapher_lastBlank != 0)) {
	    TLSETL(400229);
	    Paragrapher_SplitOutputBuffer();
	};
	TLSETL(400230);
	i += 1;
    };
    TLRESTORELF();
}

static void Paragrapher_NewLine () {
    TLSTKCHKSLF(400237);
    TLSETF();
    TLSETL(400237);
    Paragrapher_OutputLine();
    Paragrapher_lastOutputWasNewline = 1;
    TLINCL();
    Paragrapher_userIndent = 0;
    TLINCL();
    Paragrapher_lastTokenKind = 0;
    TLINCL();
    Paragrapher_lastLastTokenKind = 0;
    TLRESTORELF();
}

static void Paragrapher_PossibleNewLine () {
    TLSTKCHKSLF(400245);
    TLSETF();
    TLSETL(400245);
    if (!Paragrapher_lastOutputWasNewline) {
	TLSETL(400247);
	Paragrapher_OutputLine();
    };
    TLSETL(400248);
    Paragrapher_lastOutputWasNewline = 1;
    TLINCL();
    Paragrapher_userIndent = 0;
    TLINCL();
    Paragrapher_lastTokenKind = 0;
    TLINCL();
    Paragrapher_lastLastTokenKind = 0;
    TLRESTORELF();
}

static void Paragrapher_OutputIndentIfNecessary () {
    TLSTKCHKSLF(400256);
    TLSETF();
    TLSETL(400256);
    if (Paragrapher_lastOutputWasNewline) {
	register TLint4	ind;
	TLSETL(400257);
	ind = Paragrapher_indent + Paragrapher_userIndent;
	TLSETL(400259);
	if (ind > 0) {
	    TLSETL(400261);
	    {
		register TLint4	__x262;
		TLint4	__x329;
		__x329 = ind;
		__x262 = 1;
		if (__x262 <= __x329) {
		    for(;;) {
			TLSETL(400263);
			Paragrapher_AddCh((TLchar) ' ');
			if (__x262 == __x329) break;
			__x262++;
		    }
		};
	    };
	};
	TLSETL(400273);
	Paragrapher_lastOutputWasNewline = 0;
	TLSETL(400275);
	Paragrapher_lastBlank = 0;
    };
    TLRESTORELF();
}

static void Paragrapher_OutputEnd () {
    TLSTKCHKSLF(400282);
    TLSETF();
    TLSETL(400282);
    Paragrapher_AddString("end ");
    TLRESTORELF();
}

static void Paragrapher_OutputEndIfNecessary () {
    TLSTKCHKSLF(400286);
    TLSETF();
    TLSETL(400286);
    if (Paragrapher_state == 3) {
	TLSETL(400288);
	Paragrapher_OutputIndentIfNecessary();
	TLINCL();
	Paragrapher_OutputEnd();
	TLINCL();
	Paragrapher_NewLine();
	Paragrapher_state = 6;
    };
    TLRESTORELF();
}

static void Paragrapher_OutputToken () {
    TLSTKCHKSLF(400296);
    TLSETF();
    TLSETL(400296);
    switch (Paragrapher_tokenKind) {
	case 4:
	case 6:
	case 9:
	    {
		TLSETL(400299);
		if (((Paragrapher_lastTokenKind == 0) || (Paragrapher_lastTokenKind == 2)) || (Paragrapher_lastTokenKind == 3)) {
		    TLSETL(400302);
		    Paragrapher_AddString(Paragrapher_token);
		} else {
		    TLSETL(400302);
		    if (Paragrapher_lastTokenKind == 10) {
			TLSETL(400304);
			if (((Paragrapher_lastLastTokenKind == 10) && (Paragrapher_lastLastSymbol != ')')) && ((Paragrapher_lastSymbol == '-') || (Paragrapher_lastSymbol == '+'))) {
			    TLSETL(400308);
			    Paragrapher_AddString(Paragrapher_token);
			} else {
			    TLSETL(400308);
			    if ((Paragrapher_lastLastTokenKind == 11) && (Paragrapher_lastSymbol == '-')) {
				TLSETL(400313);
				Paragrapher_AddString(Paragrapher_token);
			    } else {
				TLSETL(400313);
				if ((Paragrapher_lastLastTokenKind == 2) && (Paragrapher_lastSymbol == '*')) {
				    TLSETL(400318);
				    Paragrapher_AddString(Paragrapher_token);
				} else {
				    TLSETL(400318);
				    if (((Paragrapher_lastLastTokenKind == 4) && (((Paragrapher_lastIdentifierKeyword == 20) || (Paragrapher_lastIdentifierKeyword == 80)) || (Paragrapher_lastIdentifierKeyword == 12))) && (Paragrapher_lastSymbol == '*')) {
					TLSETL(400326);
					Paragrapher_AddString(Paragrapher_token);
				    } else {
					TLSETL(400326);
					if (((Paragrapher_lastLastTokenKind == 10) && (Paragrapher_lastLastSymbol == ',')) && (Paragrapher_lastSymbol == '*')) {
					    TLSETL(400332);
					    Paragrapher_AddString(Paragrapher_token);
					} else {
					    TLSETL(400333);
					    switch (Paragrapher_lastSymbol) {
						case '=':
						case '+':
						case '-':
						case '*':
						case '/':
						case '<':
						case '>':
						case ':':
						case ')':
						case ',':
						case '|':
						case '&':
						    {
							TLSETL(400337);
							Paragrapher_AddCh((TLchar) ' ');
							TLSETL(400339);
							Paragrapher_AddString(Paragrapher_token);
						    }
						    break;
						default :
						    {
							TLSETL(400341);
							Paragrapher_AddString(Paragrapher_token);
						    }
						    break;
					    };
					};
				    };
				};
			    };
			};
		    } else {
			TLSETL(400345);
			Paragrapher_AddCh((TLchar) ' ');
			TLINCL();
			Paragrapher_AddString(Paragrapher_token);
		    };
		};
	    }
	    break;
	case 1:
	case 3:
	    {
		TLSETL(400349);
		if ((Paragrapher_lastTokenKind != 0) && (Paragrapher_lastTokenKind != 3)) {
		    TLSETL(400352);
		    Paragrapher_AddCh((TLchar) ' ');
		};
		TLSETL(400355);
		Paragrapher_AddString(Paragrapher_token);
	    }
	    break;
	case 10:
	    {
		register TLchar	sym;
		TLSETL(400356);
		{
		    TLchar	__x330[2];
		    TL_TLS_TLSBX(__x330, (TLint4) 1, Paragrapher_token);
		    sym = TLCVTTOCHR(__x330);
		};
		TLSETL(400358);
		switch (sym) {
		    case '(':
			{
			    TLSETL(400360);
			    Paragrapher_parenDepth += 1;
			    TLINCL();
			    if ((((Paragrapher_lastTokenKind == 10) && (Paragrapher_lastSymbol == '@')) || ((Paragrapher_lastTokenKind == 10) && (Paragrapher_lastSymbol == '('))) || (Paragrapher_lastTokenKind == 0)) {
				TLSETL(400365);
				Paragrapher_AddCh((TLchar) '(');
			    } else {
				TLSETL(400367);
				Paragrapher_AddString(" (");
			    };
			}
			break;
		    case ')':
			{
			    TLSETL(400370);
			    if (Paragrapher_parenDepth > 0) {
				TLSETL(400371);
				Paragrapher_parenDepth -= 1;
			    };
			    TLSETL(400375);
			    Paragrapher_AddCh((TLchar) ')');
			}
			break;
		    case ',':
		    case ';':
		    case '|':
		    case '@':
			{
			    TLSETL(400378);
			    Paragrapher_AddCh((TLchar) sym);
			}
			break;
		    default :
			{
			    TLSETL(400379);
			    if (((sym == '=') && (Paragrapher_lastTokenKind == 4)) && (((((((((Paragrapher_lastIdentifierKeyword == 1) || (Paragrapher_lastIdentifierKeyword == 14)) || (Paragrapher_lastIdentifierKeyword == 41)) || (Paragrapher_lastIdentifierKeyword == 45)) || (Paragrapher_lastIdentifierKeyword == 49)) || (Paragrapher_lastIdentifierKeyword == 64)) || (Paragrapher_lastIdentifierKeyword == 68)) || (Paragrapher_lastIdentifierKeyword == 69)) || (Paragrapher_lastIdentifierKeyword == 84))) {
				TLSETL(400390);
				Paragrapher_AddCh((TLchar) '=');
			    } else {
				TLSETL(400390);
				if (Paragrapher_lastTokenKind == 10) {
				    TLSETL(400391);
				    if (sym == '=') {
					TLSETL(400392);
					switch (Paragrapher_lastSymbol) {
					    case ':':
					    case '+':
					    case '-':
					    case '*':
					    case '<':
					    case '>':
					    case '~':
					    case '/':
						{
						    TLSETL(400396);
						    Paragrapher_AddCh((TLchar) '=');
						}
						break;
					    default :
						{
						    TLSETL(400398);
						    Paragrapher_AddCh((TLchar) ' ');
						    TLINCL();
						    Paragrapher_AddString(Paragrapher_token);
						}
						break;
					};
				    } else {
					TLSETL(400400);
					if (sym == '>') {
					    TLSETL(400401);
					    if ((Paragrapher_lastSymbol != '=') && (Paragrapher_lastSymbol != '-')) {
						TLSETL(400403);
						Paragrapher_AddString(" >");
					    } else {
						TLSETL(400405);
						Paragrapher_AddCh((TLchar) '>');
					    };
					} else {
					    TLSETL(400406);
					    if ((sym == '*') && ((Paragrapher_lastSymbol == '*') || (Paragrapher_lastSymbol == '('))) {
						TLSETL(400409);
						Paragrapher_AddCh((TLchar) '*');
					    } else {
						TLSETL(400409);
						if ((sym == '-') && (Paragrapher_lastSymbol == '(')) {
						    TLSETL(400412);
						    Paragrapher_AddCh((TLchar) '-');
						} else {
						    TLSETL(400412);
						    if ((sym == '+') && (Paragrapher_lastSymbol == '(')) {
							TLSETL(400415);
							Paragrapher_AddCh((TLchar) '+');
						    } else {
							TLSETL(400415);
							if (Paragrapher_lastSymbol == '@') {
							    TLSETL(400417);
							    Paragrapher_AddString(Paragrapher_token);
							} else {
							    TLSETL(400419);
							    Paragrapher_AddCh((TLchar) ' ');
							    TLINCL();
							    Paragrapher_AddString(Paragrapher_token);
							};
						    };
						};
					    };
					};
				    };
				} else {
				    TLSETL(400422);
				    if ((((Paragrapher_lastLastTokenKind == 10) && (Paragrapher_lastSymbol == '~')) && (Paragrapher_lastTokenKind == 2)) && (sym == '*')) {
					TLSETL(400429);
					Paragrapher_AddCh((TLchar) '*');
				    } else {
					TLSETL(400430);
					if (Paragrapher_lastTokenKind != 0) {
					    TLSETL(400432);
					    Paragrapher_AddCh((TLchar) ' ');
					};
					TLSETL(400434);
					Paragrapher_AddString(Paragrapher_token);
				    };
				};
			    };
			}
			break;
		};
		TLSETL(400437);
		Paragrapher_lastLastSymbol = Paragrapher_lastSymbol;
		TLINCL();
		Paragrapher_lastSymbol = sym;
	    }
	    break;
	case 2:
	    {
		TLSETL(400443);
		Paragrapher_AddCh((TLchar) '.');
	    }
	    break;
	case 11:
	    {
		TLSETL(400444);
		if (Paragrapher_lastTokenKind != 0) {
		    TLSETL(400446);
		    Paragrapher_AddCh((TLchar) ' ');
		};
		TLSETL(400448);
		Paragrapher_AddString("..");
	    }
	    break;
	default:
	    TLCASEABORT;
    };
    TLINCL();
    Paragrapher_lastLastTokenKind = Paragrapher_lastTokenKind;
    TLINCL();
    Paragrapher_lastTokenKind = Paragrapher_tokenKind;
    TLRESTORELF();
}

static void Paragrapher_SplitLine () {
    TLSTKCHKSLF(400455);
    TLSETF();
    TLSETL(400455);
    if (Paragrapher_lineIndex > 1) {
	TLSETL(400457);
	Paragrapher_AdvanceInputColumn((TLint4) (Paragrapher_lineIndex - 1));
	{
	    TLstring	__x331;
	    TL_TLS_TLSBXS(__x331, (TLint4) 0, (TLint4) Paragrapher_lineIndex, Paragrapher_inLine);
	    TLSTRASS(255, Paragrapher_inLine, __x331);
	};
    };
    TLSETL(400459);
    Paragrapher_wasSplit = 1;
    TLINCL();
    Paragrapher_userIndent = 0;
    TLINCL();
    if (strcmp(Paragrapher_inLine, "") == 0) {
	TLSETL(400463);
	Paragrapher_NewLine();
    };
    TLRESTORELF();
}

static void Paragrapher_FormatString () {
    TLSTKCHKSLF(400469);
    TLSETF();
    TLSETL(400469);
    Paragrapher_OutputEndIfNecessary();
    TLINCL();
    Paragrapher_OutputIndentIfNecessary();
    TLINCL();
    Paragrapher_OutputToken();
    if (Paragrapher_currentStatement != 21) {
	TLSETL(400472);
	Paragrapher_state = 4;
    };
    TLRESTORELF();
}

static void Paragrapher_FormatNumber () {
    TLSTKCHKSLF(400479);
    TLSETF();
    TLSETL(400479);
    Paragrapher_OutputEndIfNecessary();
    TLINCL();
    Paragrapher_OutputIndentIfNecessary();
    TLINCL();
    Paragrapher_OutputToken();
    if (Paragrapher_currentStatement != 21) {
	TLSETL(400482);
	Paragrapher_state = 4;
    };
    TLRESTORELF();
}

static void Paragrapher_FormatIdentifier () {
    TLSTKCHKSLF(400488);
    TLSETF();
    TLSETL(400488);
    if (Paragrapher_state == 4) {
	TLSETL(400490);
	Paragrapher_PossibleNewLine();
	TLINCL();
	Paragrapher_OutputIndentIfNecessary();
	Paragrapher_currentStatement = 0;
	TLINCL();
	Paragrapher_parenDepth = 0;
    } else {
	TLSETL(400493);
	if (Paragrapher_state == 3) {
	    TLSETL(400495);
	    Paragrapher_OutputIndentIfNecessary();
	    TLINCL();
	    Paragrapher_OutputEnd();
	} else {
	    TLSETL(400496);
	    if (Paragrapher_state == 6) {
		TLSETL(400498);
		Paragrapher_OutputIndentIfNecessary();
		Paragrapher_currentStatement = 0;
		TLINCL();
		Paragrapher_parenDepth = 0;
	    } else {
		TLSETL(400502);
		Paragrapher_OutputIndentIfNecessary();
	    };
	};
    };
    TLSETL(400504);
    Paragrapher_OutputToken();
    Paragrapher_state = 4;
    TLRESTORELF();
}

static void Paragrapher_FormatKeyword (keyword)
Paragrapher_Keyword	keyword;
{
    TLboolean	needNewLine;
    TLboolean	suppressIndent;
    TLSTKCHKSLF(400509);
    TLSETF();
    TLSETL(400509);
    if (keyword == 19) {
	TLSETL(400513);
	Paragrapher_OutputEndIfNecessary();
	TLASSERT ((Paragrapher_formatTable[18].spacingBefore) == 2);
	TLSETL(400517);
	Paragrapher_PossibleNewLine();
	TLASSERT ((Paragrapher_formatTable[18].indentBefore) == 3);
	TLSETL(400520);
	Paragrapher_DoExdent();
	if (Paragrapher_lineIndex > TL_TLS_TLSLEN(Paragrapher_inLine)) {
	    TLSETL(400523);
	    Paragrapher_OutputIndentIfNecessary();
	    TLINCL();
	    Paragrapher_OutputToken();
	    TLINCL();
	    Paragrapher_SplitLine();
	    Paragrapher_state = 6;
	} else {
	    TLSETL(400527);
	    Paragrapher_state = 3;
	};
	TLSETL(400529);
	Paragrapher_currentStatement = 19;
	TLINCL();
	Paragrapher_parenDepth = 0;
	TLSETL(400532);
	TLRESTORELF();
	return;
    } else {
	TLSETL(400532);
	if (Paragrapher_state == 3) {
	    TLSETL(400534);
	    if ((Paragrapher_formatTable[TLINRANGELOW(keyword, 1, 84, 1)].indentAfter) == 2) {
		TLSETL(400536);
		Paragrapher_DoExdent();
	    };
	    TLSETL(400538);
	    Paragrapher_OutputIndentIfNecessary();
	    TLINCL();
	    Paragrapher_OutputEnd();
	    switch (keyword) {
		case 9:
		case 24:
		case 30:
		case 31:
		case 40:
		    {
			TLSETL(400544);
			Paragrapher_OutputToken();
			Paragrapher_state = 4;
			TLSETL(400547);
			TLRESTORELF();
			return;
		    }
		    break;
		case 62:
		    {
			TLSETL(400550);
			Paragrapher_OutputToken();
			TLINCL();
			Paragrapher_DoExdent();
			Paragrapher_state = 4;
			TLSETL(400554);
			TLRESTORELF();
			return;
		    }
		    break;
		case 78:
		    {
			TLSETL(400556);
			Paragrapher_OutputToken();
			TLINCL();
			Paragrapher_DoExdent();
			Paragrapher_state = 4;
			TLSETL(400560);
			TLRESTORELF();
			return;
		    }
		    break;
		default :
		    {
			TLSETL(400562);
			Paragrapher_NewLine();
			Paragrapher_state = 6;
		    }
		    break;
	    };
	};
    };
    TLSETL(400567);
    needNewLine = 0;
    TLINCL();
    suppressIndent = 0;
    TLSETL(400570);
    if ((Paragrapher_formatTable[TLINRANGELOW(keyword, 1, 84, 1)].spacingBefore) == 2) {
	TLSETL(400571);
	switch (keyword) {
	    case 22:
	    case 28:
	    case 56:
	    case 57:
		{
		    TLSETL(400574);
		    if (Paragrapher_currentStatement != 7) {
			TLSETL(400575);
			switch (Paragrapher_state) {
			    case 4:
			    case 6:
				{
				    TLSETL(400577);
				    needNewLine = 1;
				}
				break;
			    default :
				{
				    TLSETL(400579);
				    suppressIndent = 1;
				}
				break;
			};
		    };
		}
		break;
	    case 29:
	    case 59:
	    case 61:
	    case 66:
	    case 83:
		{
		    TLSETL(400585);
		    if (Paragrapher_state != 2) {
			TLSETL(400586);
			needNewLine = 1;
		    };
		}
		break;
	    case 10:
		{
		    TLSETL(400590);
		    if (Paragrapher_currentStatement == 43) {
			TLSETL(400592);
			Paragrapher_DoExdent();
		    } else {
			TLSETL(400593);
			needNewLine = 1;
		    };
		}
		break;
	    default :
		{
		    TLSETL(400597);
		    needNewLine = 1;
		}
		break;
	};
    } else {
	TLSETL(400599);
	if (((Paragrapher_formatTable[TLINRANGELOW(keyword, 1, 84, 1)].spacingBefore) == 1) && (Paragrapher_state == 4)) {
	    TLSETL(400601);
	    needNewLine = 1;
	};
    };
    TLSETL(400605);
    switch (Paragrapher_formatTable[TLINRANGELOW(keyword, 1, 84, 1)].indentBefore) {
	case 3:
	    {
		TLSETL(400608);
		Paragrapher_DoExdent();
	    }
	    break;
	case 1:
	    {
		TLSETL(400610);
		Paragrapher_DoIndent();
	    }
	    break;
	default :
	    break;
    };
    TLSETL(400614);
    if (needNewLine) {
	TLSETL(400616);
	Paragrapher_PossibleNewLine();
	Paragrapher_currentStatement = keyword;
	TLINCL();
	Paragrapher_parenDepth = 0;
    } else {
	TLSETL(400618);
	if (Paragrapher_state == 6) {
	    TLSETL(400619);
	    Paragrapher_currentStatement = keyword;
	    TLINCL();
	    Paragrapher_parenDepth = 0;
	};
    };
    TLSETL(400624);
    Paragrapher_OutputIndentIfNecessary();
    TLSETL(400627);
    Paragrapher_OutputToken();
    if (!suppressIndent) {
	TLSETL(400628);
	switch (Paragrapher_formatTable[TLINRANGELOW(keyword, 1, 84, 1)].indentAfter) {
	    case 3:
		{
		    TLSETL(400631);
		    Paragrapher_DoExdent();
		}
		break;
	    case 1:
		{
		    TLSETL(400633);
		    Paragrapher_DoIndent();
		}
		break;
	    case 2:
		{
		    TLSETL(400635);
		    Paragrapher_DoIndent();
		    TLINCL();
		    Paragrapher_DoIndent();
		}
		break;
	    default :
		break;
	};
    };
    TLSETL(400641);
    switch (keyword) {
	case 29:
	case 59:
	case 66:
	case 61:
	case 83:
	case 41:
	    {
		TLSETL(400644);
		if (Paragrapher_state == 2) {
		    TLSETL(400645);
		    Paragrapher_state = 4;
		} else {
		    TLSETL(400647);
		    Paragrapher_state = 0;
		};
	    }
	    break;
	default :
	    {
		TLSETL(400651);
		if ((Paragrapher_formatTable[TLINRANGELOW(keyword, 1, 84, 1)].spacingAfter) == 1) {
		    TLSETL(400652);
		    Paragrapher_state = 4;
		} else {
		    TLSETL(400654);
		    Paragrapher_state = 0;
		};
	    }
	    break;
    };
    TLRESTORELF();
}

static void Paragrapher_FormatIdentifierKeyword () {
    TLchar	firstChar;
    TLSTKCHKSLF(400661);
    TLSETF();
    TLSETL(400661);
    {
	TLchar	__x332[2];
	TL_TLS_TLSBX(__x332, (TLint4) 1, Paragrapher_token);
	firstChar = TLCVTTOCHR(__x332);
    };
    TLSETL(400663);
    if ((Paragrapher_charClass[TLINRANGELOW(firstChar, 0, 255, 1)]) == 4) {
	Paragrapher_Keyword	keyword;
	TLSETL(400665);
	keyword = 0;
	TLSETL(400667);
	if ((firstChar >= 'a') && (firstChar <= 'z')) {
	    TLSETL(400670);
	    keyword = Paragrapher_formatTableIndex[TLINRANGELOW(firstChar, 97, 122, 1)];
	    TLSETL(400672);
	    if (keyword != 0) {
		Paragrapher_Keyword	kw;
		TLSETL(400673);
		kw = keyword;
		TLINCL();
		keyword = 0;
		TLSETL(400676);
		{
		    register TLint4	k;
		    for (k = kw; k <= 84; k++) {
			TLSETL(400677);
			if (strcmp(Paragrapher_formatTable[TLINRANGELOW(k, 1, 84, 1)].keywordText, Paragrapher_token) == 0) {
			    TLSETL(400678);
			    keyword = k;
			    TLSETL(400680);
			    break;
			};
			TLSETL(400681);
			{
			    TLchar	__x334[2];
			    TLCHRTOSTR(firstChar, __x334);
			    {
				TLchar	__x333[2];
				TL_TLS_TLSBX(__x333, (TLint4) 1, (Paragrapher_formatTable[TLINRANGELOW(k, 1, 84, 1)].keywordText));
				if (strcmp(__x333, __x334) != 0) {
				    break;
				};
			    };
			};
		    };
		};
	    };
	};
	TLSETL(400686);
	if (keyword == 0) {
	    TLSETL(400688);
	    Paragrapher_FormatIdentifier();
	    Paragrapher_lastLastIdentifierKeyword = Paragrapher_lastIdentifierKeyword;
	    TLINCL();
	    Paragrapher_lastIdentifierKeyword = 0;
	} else {
	    TLSETL(400692);
	    Paragrapher_FormatKeyword((Paragrapher_Keyword) keyword);
	    Paragrapher_lastLastIdentifierKeyword = Paragrapher_lastIdentifierKeyword;
	    TLINCL();
	    Paragrapher_lastIdentifierKeyword = keyword;
	};
    } else {
	TLSETL(400697);
	Paragrapher_FormatIdentifier();
	Paragrapher_lastLastIdentifierKeyword = Paragrapher_lastIdentifierKeyword;
	TLINCL();
	Paragrapher_lastIdentifierKeyword = 0;
    };
    TLRESTORELF();
}

static void Paragrapher_FormatDot () {
    TLSTKCHKSLF(400705);
    TLSETF();
    TLSETL(400705);
    Paragrapher_OutputEndIfNecessary();
    TLINCL();
    Paragrapher_OutputIndentIfNecessary();
    TLINCL();
    Paragrapher_OutputToken();
    Paragrapher_state = 0;
    TLRESTORELF();
}

static void Paragrapher_FormatDotDot () {
    TLSTKCHKSLF(400713);
    TLSETF();
    TLSETL(400713);
    Paragrapher_OutputEndIfNecessary();
    TLINCL();
    Paragrapher_OutputIndentIfNecessary();
    TLINCL();
    Paragrapher_OutputToken();
    if ((Paragrapher_currentStatement == 59) && (Paragrapher_parenDepth == 0)) {
	TLSETL(400716);
	Paragrapher_state = 4;
    } else {
	TLSETL(400718);
	Paragrapher_state = 0;
    };
    TLRESTORELF();
}

static void Paragrapher_FormatSymbol () {
    register TLchar	tok;
    TLSTKCHKSLF(400726);
    TLSETF();
    TLSETL(400726);
    Paragrapher_OutputEndIfNecessary();
    tok = (* (Paragrapher_CString *) Paragrapher_token)[(0)];
    TLSETL(400728);
    if (Paragrapher_state == 4) {
	TLSETL(400730);
	switch (tok) {
	    case '^':
	    case '#':
		{
		    TLSETL(400733);
		    Paragrapher_PossibleNewLine();
		    Paragrapher_currentStatement = 0;
		    TLINCL();
		    Paragrapher_parenDepth = 0;
		}
		break;
	    default :
		break;
	};
    };
    TLSETL(400740);
    Paragrapher_OutputIndentIfNecessary();
    TLSETL(400742);
    Paragrapher_OutputToken();
    switch (tok) {
	case ')':
	    {
		TLSETL(400748);
		if ((Paragrapher_currentStatement == 28) && (Paragrapher_parenDepth == 0)) {
		    TLSETL(400750);
		    Paragrapher_state = 0;
		} else {
		    TLSETL(400752);
		    Paragrapher_state = 4;
		};
	    }
	    break;
	case ';':
	    {
		TLSETL(400755);
		Paragrapher_state = 4;
	    }
	    break;
	case ':':
	    {
		TLSETL(400757);
		if (Paragrapher_currentStatement == 39) {
		    TLSETL(400759);
		    Paragrapher_state = 4;
		} else {
		    TLSETL(400761);
		    Paragrapher_state = 1;
		};
	    }
	    break;
	case '*':
	    {
		TLSETL(400764);
		if (Paragrapher_state == 1) {
		    TLSETL(400765);
		    Paragrapher_state = 4;
		} else {
		    TLSETL(400767);
		    Paragrapher_state = 0;
		};
	    }
	    break;
	case ',':
	    {
		TLSETL(400770);
		Paragrapher_state = 2;
	    }
	    break;
	default :
	    {
		TLSETL(400772);
		Paragrapher_state = 0;
	    }
	    break;
    };
    TLRESTORELF();
}

static void Paragrapher_FormatComment (isSlashStar, aloneOnLine)
TLboolean	isSlashStar;
TLboolean	aloneOnLine;
{
    TLSTKCHKSLF(400778);
    TLSETF();
    TLSETL(400778);
    if (Paragrapher_state == 3) {
	TLSETL(400780);
	Paragrapher_OutputIndentIfNecessary();
	TLINCL();
	Paragrapher_OutputEnd();
	Paragrapher_state = 4;
    };
    TLSETL(400783);
    if (strcmp(Paragrapher_token, "") != 0) {
	TLSETL(400785);
	Paragrapher_OutputIndentIfNecessary();
	if (isSlashStar) {
	    TLSETL(400787);
	    Paragrapher_AddCh((TLchar) ' ');
	};
	TLSETL(400788);
	if (Paragrapher_keepCommentColumn && (!aloneOnLine)) {
	    TLSETL(400789);
	    if (Paragrapher_currentLineLength < (Paragrapher_currentInputColumn - 2)) {
		TLSETL(400790);
		{
		    register TLint4	__x287;
		    TLint4	__x336;
		    __x336 = (Paragrapher_currentInputColumn - Paragrapher_currentLineLength) - 2;
		    __x287 = 1;
		    if (__x287 <= __x336) {
			for(;;) {
			    TLSETL(400792);
			    Paragrapher_AddCh((TLchar) ' ');
			    if (__x287 == __x336) break;
			    __x287++;
			}
		    };
		};
	    };
	};
	TLSETL(400796);
	Paragrapher_OutputToken();
    };
    TLRESTORELF();
}

static void Paragrapher_FormatHash (wasConditionalCompilation)
TLboolean	*wasConditionalCompilation;
{
    TLBINDREG((*cs), Paragrapher_CString);
    register TLint2	i;
    register TLchar	c;
    typedef	TLchar	__x338[6];
    __x338	dummy;
    TLSTKCHKSLF(400804);
    TLSETF();
    TLSETL(400804);
    Paragrapher_OutputEndIfNecessary();
    cs = Paragrapher_token;
    TLINCL();
    i = 2;
    TLSETL(400807);
    (*wasConditionalCompilation) = 0;
    TLSETL(400813);
    for(;;) {
	TLSETL(400813);
	c = (*cs)[(i - 1)];
	TLSETL(400815);
	if ((c == '\0') || ((Paragrapher_charClass[TLINRANGELOW(c, 0, 255, 1)]) != 8)) {
	    break;
	};
	TLSETL(400816);
	i += 1;
    };
    TLSETL(400819);
    Paragrapher_userIndent = 0;
    TLINCL();
    TLSTRASS(5, dummy, "");
    TLINCL();
    {
	register TLint4	__x294;
	for (__x294 = 1; __x294 <= 5; __x294++) {
	    TLSETL(400822);
	    if ((Paragrapher_charClass[TLINRANGELOW(c, 0, 255, 1)]) != 4) {
		break;
	    };
	    TLSETL(400823);
	    {
		TLchar	__x339[2];
		TLCHRTOSTR(c, __x339);
		TLSTRCATASS(dummy, __x339, 5);
	    };
	    TLINCL();
	    i += 1;
	    TLINCL();
	    c = (*cs)[(i - 1)];
	};
    };
    TLSETL(400827);
    if (((Paragrapher_charClass[TLINRANGELOW(c, 0, 255, 1)]) == 8) || ((Paragrapher_charClass[TLINRANGELOW(c, 0, 255, 1)]) == 0)) {
	TLSETL(400828);
	if (((strcmp(dummy, "end") == 0) || (strcmp(dummy, "else") == 0)) || (strcmp(dummy, "elsif") == 0)) {
	    TLSETL(400830);
	    Paragrapher_DoExdent();
	    TLINCL();
	    Paragrapher_OutputIndentIfNecessary();
	    TLINCL();
	    Paragrapher_OutputToken();
	    {
		TLchar	__x340[2];
		TL_TLS_TLSBX(__x340, (TLint4) 2, dummy);
		if (strcmp(__x340, "l") == 0) {
		    TLSETL(400834);
		    Paragrapher_DoIndent();
		};
	    };
	    TLSETL(400835);
	    (*wasConditionalCompilation) = 1;
	} else {
	    TLSETL(400836);
	    if (strcmp(dummy, "if") == 0) {
		TLSETL(400839);
		Paragrapher_OutputIndentIfNecessary();
		TLINCL();
		Paragrapher_OutputToken();
		TLINCL();
		Paragrapher_DoIndent();
		(*wasConditionalCompilation) = 1;
	    };
	};
    };
    TLSETL(400844);
    if (!(*wasConditionalCompilation)) {
	TLSETL(400845);
	TLSTRASS(255, Paragrapher_token, "#");
	TLINCL();
	if (Paragrapher_state == 4) {
	    TLSETL(400848);
	    Paragrapher_PossibleNewLine();
	    Paragrapher_currentStatement = 0;
	    TLINCL();
	    Paragrapher_parenDepth = 0;
	};
	TLSETL(400852);
	Paragrapher_OutputIndentIfNecessary();
	TLINCL();
	Paragrapher_OutputToken();
	Paragrapher_state = 0;
    };
    TLRESTORELF();
}

static void Paragrapher_TrimLine () {
    TLint4	l;
    TLint2	startLine;
    register TLint2	endLine;
    TLSTKCHKSLF(400859);
    TLSETF();
    TLSETL(400859);
    if (strcmp(Paragrapher_inLine, "") == 0) {
	TLSETL(400861);
	TLRESTORELF();
	return;
    };
    TLSETL(400863);
    l = TL_TLS_TLSLEN(Paragrapher_inLine);
    TLINCL();
    startLine = l;
    TLSETL(400867);
    {
	register TLint4	i;
	TLint4	__x341;
	__x341 = l;
	i = 1;
	if (i <= __x341) {
	    for(;;) {
		TLSETL(400868);
		if ((Paragrapher_charClass[TLINRANGELOW((* (Paragrapher_CString *) Paragrapher_inLine)[(i - 1)], 0, 255, 1)]) != 8) {
		    TLSETL(400869);
		    startLine = i;
		    TLSETL(400871);
		    break;
		};
		if (i == __x341) break;
		i++;
	    }
	};
    };
    TLSETL(400875);
    if ((Paragrapher_charClass[TLINRANGELOW((* (Paragrapher_CString *) Paragrapher_inLine)[(startLine - 1)], 0, 255, 1)]) == 8) {
	TLSETL(400876);
	TLSTRASS(255, Paragrapher_inLine, "");
	TLSETL(400878);
	TLRESTORELF();
	return;
    };
    TLSETL(400880);
    endLine = startLine;
    TLINCL();
    {
	register TLint4	i;
	TLint4	__x344;
	__x344 = startLine + 1;
	i = l;
	if (i >= __x344) {
	    for(;;) {
		TLSETL(400882);
		if ((Paragrapher_charClass[TLINRANGELOW((* (Paragrapher_CString *) Paragrapher_inLine)[(i - 1)], 0, 255, 1)]) != 8) {
		    TLSETL(400883);
		    endLine = i;
		    TLSETL(400885);
		    break;
		};
		if (i == __x344) break;
		i--;
	    }
	};
    };
    TLSETL(400889);
    Paragrapher_AdvanceInputColumn((TLint4) (startLine - 1));
    {
	TLstring	__x346;
	TL_TLS_TLSBXX(__x346, (TLint4) endLine, (TLint4) startLine, Paragrapher_inLine);
	TLSTRASS(255, Paragrapher_inLine, __x346);
    };
    TLRESTORELF();
}

static void Paragrapher_ParagraphLine () {
    register TLint4	charIndex;
    register TLint4	lineLength;
    TLint4	startIndex;
    TLSTKCHKSLF(400897);
    TLSETF();
    TLSETL(400897);
    TLPRE (!Paragrapher_wasSplit);
    TLSETL(400899);
    charIndex = 1;
    TLSETL(400902);
    Paragrapher_currentInputColumn = 1;
    TLSETL(400907);
    Paragrapher_TrimLine();
    Paragrapher_tokenKind = 1;
    TLSETL(400910);
    if (Paragrapher_state == 5) {
	register TLint4	lineLength;
	TLSETL(400911);
	lineLength = TL_TLS_TLSLEN(Paragrapher_inLine);
	TLSETL(400913);
	Paragrapher_userIndent = 0;
	TLSETL(400915);
	for(;;) {
	    TLSETL(400915);
	    if (charIndex > lineLength) {
		break;
	    };
	    TLSETL(400916);
	    if (((* (Paragrapher_CString *) Paragrapher_inLine)[(charIndex - 1)]) == '*') {
		TLSETL(400917);
		if ((charIndex < lineLength) && (((* (Paragrapher_CString *) Paragrapher_inLine)[((charIndex + 1) - 1)]) == '/')) {
		    TLSETL(400920);
		    charIndex += 2;
		    TLINCL();
		    Paragrapher_state = Paragrapher_commentState;
		    TLINCL();
		    TLASSERT (Paragrapher_state != 5);
		    TLINCL();
		    {
			TLstring	__x349;
			TL_TLS_TLSBXX(__x349, (TLint4) (charIndex - 1), (TLint4) 1, Paragrapher_inLine);
			TLSTRASS(255, Paragrapher_token, __x349);
		    };
		    TLSETL(400925);
		    Paragrapher_FormatComment((TLboolean) 1, (TLboolean) 1);
		    TLINCL();
		    break;
		};
	    };
	    TLSETL(400928);
	    charIndex += 1;
	};
	TLSETL(400930);
	if (Paragrapher_state == 5) {
	    TLSETL(400932);
	    {
		TLstring	__x350;
		TL_TLS_TLSBXX(__x350, (TLint4) (charIndex - 1), (TLint4) 1, Paragrapher_inLine);
		TLSTRASS(255, Paragrapher_token, __x350);
	    };
	    TLSETL(400934);
	    Paragrapher_FormatComment((TLboolean) 1, (TLboolean) 1);
	    TLINCL();
	    Paragrapher_NewLine();
	    TLINCL();
	    TLRESTORELF();
	    return;
	} else {
	    TLSETL(400939);
	    Paragrapher_AdvanceInputColumn((TLint4) (charIndex - 1));
	    {
		TLstring	__x351;
		TL_TLS_TLSBXS(__x351, (TLint4) 0, (TLint4) charIndex, Paragrapher_inLine);
		TLSTRASS(255, Paragrapher_inLine, __x351);
	    };
	    TLINCL();
	    charIndex = 1;
	};
    };
    TLSETL(400945);
    if (Paragrapher_state == 6) {
	TLSETL(400946);
	Paragrapher_userIndent = 0;
    } else {
	TLSETL(400948);
	Paragrapher_userIndent = 4;
    };
    TLSETL(400952);
    if ((strcmp(Paragrapher_inLine, "") == 0) || ((Paragrapher_charClass[TLINRANGELOW((* (Paragrapher_CString *) Paragrapher_inLine)[(0)], 0, 255, 1)]) == 1)) {
	TLSETL(400953);
	Paragrapher_userIndent = 0;
	TLINCL();
	if (strcmp(Paragrapher_inLine, "") != 0) {
	    TLSETL(400955);
	    TLSTRASS(255, Paragrapher_token, Paragrapher_inLine);
	    TLSETL(400957);
	    Paragrapher_FormatComment((TLboolean) 0, (TLboolean) 1);
	};
	TLSETL(400959);
	Paragrapher_NewLine();
	TLINCL();
	TLRESTORELF();
	return;
    };
    TLSETL(400963);
    lineLength = TL_TLS_TLSLEN(Paragrapher_inLine);
    TLSETL(400969);
    for(;;) {
	TLSETL(400970);
	for(;;) {
	    TLSETL(400970);
	    if ((charIndex > lineLength) || ((Paragrapher_charClass[TLINRANGELOW((* (Paragrapher_CString *) Paragrapher_inLine)[(charIndex - 1)], 0, 255, 1)]) != 8)) {
		break;
	    };
	    TLSETL(400972);
	    charIndex += 1;
	};
	TLSETL(400975);
	if (charIndex > lineLength) {
	    TLSETL(400978);
	    break;
	};
	TLSETL(400981);
	startIndex = charIndex;
	TLSETL(400983);
	Paragrapher_tokenKind = Paragrapher_charClass[TLINRANGELOW((* (Paragrapher_CString *) Paragrapher_inLine)[(charIndex - 1)], 0, 255, 1)];
	TLINCL();
	switch (Paragrapher_charClass[TLINRANGELOW((* (Paragrapher_CString *) Paragrapher_inLine)[(charIndex - 1)], 0, 255, 1)]) {
	    case 10:
		{
		    TLSETL(400987);
		    charIndex += 1;
		}
		break;
	    case 1:
		{
		    TLSETL(400991);
		    charIndex = lineLength + 1;
		}
		break;
	    case 3:
		{
		    TLSETL(400995);
		    charIndex = lineLength + 1;
		}
		break;
	    case 7:
		{
		    TLSETL(400999);
		    charIndex += 1;
		    TLINCL();
		    if (((* (Paragrapher_CString *) Paragrapher_inLine)[(charIndex - 1)]) == '*') {
			TLSETL(401002);
			charIndex += 1;
			TLINCL();
			Paragrapher_tokenKind = 1;
			TLINCL();
			TLASSERT (Paragrapher_state != 5);
			TLINCL();
			Paragrapher_commentState = Paragrapher_state;
			TLINCL();
			Paragrapher_state = 5;
			TLINCL();
			Paragrapher_userIndent = 0;
			TLSETL(401009);
			for(;;) {
			    TLSETL(401009);
			    if (charIndex > lineLength) {
				break;
			    };
			    TLSETL(401010);
			    if (((* (Paragrapher_CString *) Paragrapher_inLine)[(charIndex - 1)]) == '*') {
				TLSETL(401011);
				if ((charIndex < lineLength) && (((* (Paragrapher_CString *) Paragrapher_inLine)[((charIndex + 1) - 1)]) == '/')) {
				    TLSETL(401014);
				    charIndex += 2;
				    TLINCL();
				    Paragrapher_state = Paragrapher_commentState;
				    TLSETL(401017);
				    break;
				};
			    };
			    TLSETL(401019);
			    charIndex += 1;
			};
		    } else {
			TLSETL(401022);
			Paragrapher_tokenKind = 10;
		    };
		}
		break;
	    case 9:
		{
		    TLchar	endChar;
		    TLSETL(401027);
		    endChar = (* (Paragrapher_CString *) Paragrapher_inLine)[(charIndex - 1)];
		    TLSETL(401029);
		    charIndex += 1;
		    TLSETL(401031);
		    for(;;) {
			TLSETL(401031);
			if ((charIndex > lineLength) || (((* (Paragrapher_CString *) Paragrapher_inLine)[(charIndex - 1)]) == endChar)) {
			    break;
			};
			TLSETL(401034);
			if ((((* (Paragrapher_CString *) Paragrapher_inLine)[(charIndex - 1)]) == '\\') && ((charIndex + 1) < lineLength)) {
			    TLSETL(401036);
			    charIndex += 1;
			};
			TLSETL(401038);
			charIndex += 1;
		    };
		    TLSETL(401040);
		    if (charIndex <= lineLength) {
			TLSETL(401042);
			charIndex += 1;
		    };
		}
		break;
	    case 4:
		{
		    TLSETL(401047);
		    charIndex += 1;
		    TLSETL(401049);
		    for(;;) {
			TLSETL(401049);
			if ((charIndex > lineLength) || (((Paragrapher_charClass[TLINRANGELOW((* (Paragrapher_CString *) Paragrapher_inLine)[(charIndex - 1)], 0, 255, 1)]) != 4) && ((Paragrapher_charClass[TLINRANGELOW((* (Paragrapher_CString *) Paragrapher_inLine)[(charIndex - 1)], 0, 255, 1)]) != 6))) {
			    break;
			};
			TLSETL(401055);
			charIndex += 1;
		    };
		}
		break;
	    case 6:
		{
		    TLSETL(401061);
		    charIndex += 1;
		    TLSETL(401065);
		    for(;;) {
			TLSETL(401065);
			if ((charIndex > lineLength) || ((Paragrapher_charClass[TLINRANGELOW((* (Paragrapher_CString *) Paragrapher_inLine)[(charIndex - 1)], 0, 255, 1)]) != 6)) {
			    break;
			};
			TLSETL(401068);
			charIndex += 1;
		    };
		    TLSETL(401072);
		    if ((charIndex <= lineLength) && (((* (Paragrapher_CString *) Paragrapher_inLine)[(charIndex - 1)]) == '.')) {
			TLSETL(401074);
			charIndex += 1;
			TLINCL();
			if ((charIndex <= lineLength) && (((* (Paragrapher_CString *) Paragrapher_inLine)[(charIndex - 1)]) == '.')) {
			    TLSETL(401078);
			    charIndex -= 1;
			} else {
			    TLSETL(401081);
			    for(;;) {
				TLSETL(401081);
				if ((charIndex > lineLength) || ((Paragrapher_charClass[TLINRANGELOW((* (Paragrapher_CString *) Paragrapher_inLine)[(charIndex - 1)], 0, 255, 1)]) != 6)) {
				    break;
				};
				TLSETL(401084);
				charIndex += 1;
			    };
			};
			TLSETL(401087);
			if ((charIndex <= lineLength) && ((((* (Paragrapher_CString *) Paragrapher_inLine)[(charIndex - 1)]) == 'e') || (((* (Paragrapher_CString *) Paragrapher_inLine)[(charIndex - 1)]) == 'E'))) {
			    TLSETL(401090);
			    charIndex += 1;
			    TLINCL();
			    if ((charIndex <= lineLength) && ((((* (Paragrapher_CString *) Paragrapher_inLine)[(charIndex - 1)]) == '+') || (((* (Paragrapher_CString *) Paragrapher_inLine)[(charIndex - 1)]) == '-'))) {
				TLSETL(401094);
				charIndex += 1;
			    };
			    TLSETL(401097);
			    for(;;) {
				TLSETL(401097);
				if ((charIndex > lineLength) || ((Paragrapher_charClass[TLINRANGELOW((* (Paragrapher_CString *) Paragrapher_inLine)[(charIndex - 1)], 0, 255, 1)]) != 6)) {
				    break;
				};
				TLSETL(401100);
				charIndex += 1;
			    };
			};
		    } else {
			TLSETL(401103);
			if ((charIndex <= lineLength) && ((((* (Paragrapher_CString *) Paragrapher_inLine)[(charIndex - 1)]) == 'e') || (((* (Paragrapher_CString *) Paragrapher_inLine)[(charIndex - 1)]) == 'E'))) {
			    TLSETL(401106);
			    charIndex += 1;
			    TLINCL();
			    if ((charIndex <= lineLength) && ((((* (Paragrapher_CString *) Paragrapher_inLine)[(charIndex - 1)]) == '+') || (((* (Paragrapher_CString *) Paragrapher_inLine)[(charIndex - 1)]) == '-'))) {
				TLSETL(401110);
				charIndex += 1;
			    };
			    TLSETL(401113);
			    for(;;) {
				TLSETL(401113);
				if ((charIndex > lineLength) || ((Paragrapher_charClass[TLINRANGELOW((* (Paragrapher_CString *) Paragrapher_inLine)[(charIndex - 1)], 0, 255, 1)]) != 6)) {
				    break;
				};
				TLSETL(401116);
				charIndex += 1;
			    };
			} else {
			    TLSETL(401118);
			    if ((charIndex <= lineLength) && (((* (Paragrapher_CString *) Paragrapher_inLine)[(charIndex - 1)]) == '#')) {
				TLSETL(401121);
				charIndex += 1;
				TLSETL(401123);
				for(;;) {
				    TLSETL(401123);
				    if (charIndex > lineLength) {
					break;
				    };
				    TLSETL(401124);
				    switch ((* (Paragrapher_CString *) Paragrapher_inLine)[(charIndex - 1)]) {
					case '0':
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
					case '8':
					case '9':
					case 'a':
					case 'b':
					case 'c':
					case 'd':
					case 'e':
					case 'f':
					case 'A':
					case 'B':
					case 'C':
					case 'D':
					case 'E':
					case 'F':
					    {
						TLSETL(401129);
						charIndex += 1;
					    }
					    break;
					default :
					    {
						TLSETL(401133);
						goto __x397;
					    }
					    break;
				    };
				}
				__x397:;
			    };
			};
		    };
		}
		break;
	    case 2:
		{
		    TLSETL(401139);
		    charIndex += 1;
		    TLINCL();
		    if (charIndex <= lineLength) {
			TLSETL(401141);
			if (((* (Paragrapher_CString *) Paragrapher_inLine)[(charIndex - 1)]) == '.') {
			    TLSETL(401142);
			    charIndex += 1;
			    TLINCL();
			    Paragrapher_tokenKind = 11;
			} else {
			    TLSETL(401144);
			    if ((Paragrapher_charClass[TLINRANGELOW((* (Paragrapher_CString *) Paragrapher_inLine)[(charIndex - 1)], 0, 255, 1)]) == 6) {
				TLSETL(401146);
				Paragrapher_tokenKind = 6;
				TLINCL();
				charIndex += 1;
				TLSETL(401149);
				for(;;) {
				    TLSETL(401149);
				    if ((charIndex > lineLength) || ((Paragrapher_charClass[TLINRANGELOW((* (Paragrapher_CString *) Paragrapher_inLine)[(charIndex - 1)], 0, 255, 1)]) != 6)) {
					break;
				    };
				    TLSETL(401152);
				    charIndex += 1;
				};
				TLSETL(401154);
				if ((charIndex <= lineLength) && ((((* (Paragrapher_CString *) Paragrapher_inLine)[(charIndex - 1)]) == 'e') || (((* (Paragrapher_CString *) Paragrapher_inLine)[(charIndex - 1)]) == 'E'))) {
				    TLSETL(401157);
				    charIndex += 1;
				    TLINCL();
				    if ((charIndex <= lineLength) && ((((* (Paragrapher_CString *) Paragrapher_inLine)[(charIndex - 1)]) == '+') || (((* (Paragrapher_CString *) Paragrapher_inLine)[(charIndex - 1)]) == '-'))) {
					TLSETL(401162);
					charIndex += 1;
				    };
				    TLSETL(401165);
				    for(;;) {
					TLSETL(401165);
					if ((charIndex > lineLength) || ((Paragrapher_charClass[TLINRANGELOW((* (Paragrapher_CString *) Paragrapher_inLine)[(charIndex - 1)], 0, 255, 1)]) != 6)) {
					    break;
					};
					TLSETL(401168);
					charIndex += 1;
				    };
				};
			    };
			};
		    };
		}
		break;
	    default :
		{
		    TLSETL(401176);
		    Paragrapher_tokenKind = 1;
		    TLINCL();
		    charIndex += 1;
		}
		break;
	};
	TLSETL(401181);
	{
	    TLstring	__x388;
	    TL_TLS_TLSBXX(__x388, (TLint4) (charIndex - 1), (TLint4) startIndex, Paragrapher_inLine);
	    TLSTRASS(255, Paragrapher_token, __x388);
	};
	TLINCL();
	Paragrapher_lineIndex = charIndex;
	TLSETL(401184);
	switch (Paragrapher_tokenKind) {
	    case 9:
		{
		    TLSETL(401188);
		    Paragrapher_FormatString();
		}
		break;
	    case 6:
		{
		    TLSETL(401191);
		    Paragrapher_FormatNumber();
		}
		break;
	    case 4:
		{
		    TLSETL(401194);
		    Paragrapher_FormatIdentifierKeyword();
		}
		break;
	    case 2:
		{
		    TLSETL(401197);
		    Paragrapher_FormatDot();
		}
		break;
	    case 11:
		{
		    TLSETL(401200);
		    Paragrapher_FormatDotDot();
		}
		break;
	    case 10:
		{
		    TLSETL(401203);
		    Paragrapher_FormatSymbol();
		}
		break;
	    case 1:
		{
		    TLSETL(401205);
		    Paragrapher_AdvanceInputColumn((TLint4) (startIndex - 1));
		    TLSETL(401207);
		    Paragrapher_FormatComment((TLboolean) 0, (TLboolean) 0);
		}
		break;
	    case 3:
		{
		    TLboolean	wasConditionalComp;
		    TLSETL(401211);
		    Paragrapher_FormatHash(&(wasConditionalComp));
		    if (!wasConditionalComp) {
			TLSETL(401212);
			charIndex = startIndex + 1;
		    };
		}
		break;
	    default:
		TLCASEABORT;
	};
	TLSETL(401216);
	if ((charIndex > lineLength) || Paragrapher_wasSplit) {
	    break;
	};
    };
    TLSETL(401218);
    if (Paragrapher_wasSplit && (strcmp(Paragrapher_inLine, "") == 0)) {
	TLSETL(401219);
	Paragrapher_wasSplit = 0;
    } else {
	TLSETL(401222);
	Paragrapher_NewLine();
    };
    TLRESTORELF();
}

static void Paragrapher_Initialize () {
    TLSTKCHKSLF(401227);
    TLSETF();
    TLSETL(401227);
    Paragrapher_state = 6;
    TLINCL();
    Paragrapher_commentState = 6;
    TLINCL();
    Paragrapher_wasSplit = 0;
    TLINCL();
    Paragrapher_indent = 0;
    TLINCL();
    Paragrapher_userIndent = 0;
    TLINCL();
    Paragrapher_lastOutputWasNewline = 1;
    TLINCL();
    Paragrapher_currentStatement = 0;
    TLINCL();
    Paragrapher_parenDepth = 0;
    TLSETL(401236);
    Paragrapher_tokenKind = 0;
    TLINCL();
    Paragrapher_lastTokenKind = 0;
    TLINCL();
    Paragrapher_lastLastTokenKind = 0;
    TLINCL();
    Paragrapher_lastIdentifierKeyword = 0;
    TLINCL();
    Paragrapher_lastLastIdentifierKeyword = 0;
    TLSETL(401242);
    Paragrapher_outLinePtr = 1;
    TLSETL(401244);
    Paragrapher_maxLineLength = 200;
    TLINCL();
    Paragrapher_currentLineLength = 0;
    TLINCL();
    Paragrapher_lastBlank = 0;
    TLRESTORELF();
}

static void Paragrapher_CopyLine (inBuf, nextLine)
TLaddressint	*inBuf;
TLstring	nextLine;
{
    register TLaddressint	src;
    register TLaddressint	dst;
    TLSTKCHKSLF(100099);
    TLSETF();
    TLSETL(100099);
    src = (TLaddressint) (*inBuf);
    TLINCL();
    dst = (TLaddressint) ((unsigned long)nextLine);
    TLSETL(100103);
    for(;;) {
	TLSETL(100103);
	if (((* (TLchar *) src)) == '\0') {
	    TLSETL(100104);
	    (* (TLchar *) dst) = '\0';
	    TLINCL();
	    (*inBuf) = (TLaddressint) src;
	    TLSETL(100107);
	    TLRESTORELF();
	    return;
	};
	TLSETL(100109);
	if (((* (TLchar *) src)) == '\n') {
	    TLSETL(100110);
	    (* (TLchar *) dst) = '\0';
	    TLINCL();
	    (*inBuf) = (TLaddressint) ((unsigned long) src + 1);
	    TLSETL(100113);
	    TLRESTORELF();
	    return;
	};
	TLSETL(100115);
	(* (TLchar *) dst) = (* (TLchar *) src);
	TLINCL();
	dst += 1;
	TLINCL();
	src += 1;
	TLSETL(100119);
	if ((unsigned long) src >= ((unsigned long) (*inBuf) + sizeof(TLstring))) {
	    TLSETL(100122);
	    TL_TLE_TLEQUIT ((TLint4) 1, (char *) 0, 0);
	};
    };
    TLRESTORELF();
}

void Paragrapher_Para (buffer, bufferSize)
TLaddressint	*buffer;
TLint4	*bufferSize;
{
    struct TLHAREA	quitCode;
    TLSTKCHKSLF(100128);
    if (TLHANDENTER(quitCode)) {
	TLSETF();
	TLSETL(100128);
	if ((unsigned long) Paragrapher_outBuffer != 0) {
	    TLSETL(100131);
	    free((TLaddressint) Paragrapher_outBuffer);
	    Paragrapher_outBuffer = (TLaddressint) 0;
	};
	TLSETL(100133);
	(*buffer) = (TLaddressint) 0;
	TLINCL();
	(*bufferSize) = 0;
    } else {
	TLaddressint	bufPtr;
	TLSETF();
	TLSETL(100137);
	Paragrapher_outBufferSize = (*bufferSize) * 2;
	TLINCL();
	Paragrapher_outBuffer = (TLaddressint) malloc((TLint4) Paragrapher_outBufferSize);
	TLINCL();
	if ((unsigned long) Paragrapher_outBuffer == 0) {
	    TLSETL(100141);
	    TL_TLE_TLEQUIT ((TLint4) 1, (char *) 0, 0);
	};
	TLSETL(100142);
	Paragrapher_outBufferPtr = 0;
	TLSETL(100144);
	bufPtr = (TLaddressint) (*buffer);
	TLSETL(100148);
	Paragrapher_Initialize();
	Paragrapher_maxLineLength = 200;
	TLINCL();
	Paragrapher_maxIndent = (Paragrapher_maxLineLength * 3) / 4;
	TLSETL(100152);
	for(;;) {
	    TLSETL(100152);
	    if (Paragrapher_wasSplit) {
		TLSETL(100154);
		Paragrapher_wasSplit = 0;
	    } else {
		TLSETL(100156);
		if (((* (TLchar *) bufPtr)) == '\0') {
		    break;
		};
		TLSETL(100158);
		Paragrapher_CopyLine(&(bufPtr), Paragrapher_inLine);
	    };
	    TLSETL(100161);
	    Paragrapher_ParagraphLine();
	};
	TLSETL(100163);
	if (!Paragrapher_lastOutputWasNewline) {
	    TLSETL(100165);
	    Paragrapher_NewLine();
	};
	TLSETL(100167);
	if ((Paragrapher_outBufferPtr + 1) != Paragrapher_outBufferSize) {
	    TLSETL(100169);
	    Paragrapher_NewBuffer((TLint4) (Paragrapher_outBufferPtr + 1));
	};
	TLSETL(100170);
	(* (TLchar *) ((unsigned long) Paragrapher_outBuffer + Paragrapher_outBufferPtr)) = '\0';
	TLINCL();
	Paragrapher_outBufferPtr += 1;
	TLINCL();
	(*buffer) = (TLaddressint) Paragrapher_outBuffer;
	TLINCL();
	(*bufferSize) = Paragrapher_outBufferPtr;
	TLINCL();
	Paragrapher_outBuffer = (TLaddressint) 0;
	TL_TLE_TLEHX();
    }
}

void Paragrapher_SetKeepCommentColumn (keepColumn)
TLboolean	keepColumn;
{
    TLSTKCHKSLF(100179);
    TLSETF();
    TLSETL(100179);
    Paragrapher_MySetKeepCommentColumn((TLboolean) keepColumn);
    TLRESTORELF();
}

static TLboolean __x398;
void Paragrapher () {
    TLSAVELF();
    TLSETF();
    if (__x398 == 0) {
	__x398 = 1;
	TLSETL(400081);
	Paragrapher_keepCommentColumn = 0;
    }
    TLRESTORELF();
}
