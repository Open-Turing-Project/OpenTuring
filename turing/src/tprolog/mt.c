#include "cinterface"

void MT_InterpretIfc ();
struct	MT_FreeStrings {
    struct MT_StrRegs	*h;
    TLnat4	n;
};
struct	MT_StrRegs {
    TLstring	s;
    TLboolean	t;
    struct MT_StrRegs	*n;
};
typedef	TLnat4	MT_StringIndex;
typedef	TLnat4	MT___x577[20];
typedef	struct MT_StrRegs	*MT___x576[256];
struct	MT_RegisterDef {
    TLnat4	ps;
    TLnat4	pc;
    TLnat4	oldpc;
    TLnat4	breakpoint;
    TLnat4	mt;
    TLnat4	oldmt;
    MT___x576	st;
    MT_StringIndex	ns;
    MT___x577	display;
    TLnat4	timeSlice;
};
typedef	TLint4	MT_PFstatus;
typedef	TLint4	MT_FaultKind;
typedef	TLint4	MT_Pbit;
typedef	TLnat1	MT_Pset;
struct	MT_PageInfo {
    MT_Pset	status;
    TLnat4	indx;
};
typedef	struct MT_PageInfo	MT___x580[32];
typedef	MT___x580	MT_PageList;
struct	MT_Pages {
    TLaddressint	baseAddr;
    TLnat4	numPages;
    MT_FaultKind	fault;
    MT_PFstatus	status;
    TLnat4	faultAddr;
    MT_PageList	pages;
};
typedef	TLint4	MT_T;
typedef	TLint4	MT_Instr;
typedef	struct MT_StrRegs	*MT___x581[256];

static void MT_onews (fs, st, ns, ref, isTemp)
struct MT_FreeStrings	*fs;
MT___x581	st;
MT_StringIndex	*ns;
TLnat4	*ref;
TLboolean	isTemp;
{
    TLBIND((*sr), struct MT_StrRegs *);
    TLSTACKCHECK();
    (*ref) = (*ns);
    for(;;) {
	if ((st[TLINRANGELOW((*ref), 1, 256, 1)]) == ((struct MT_StrRegs *) 0)) {
	    break;
	};
	(*ref) = ((*ref) & 255) + 1;
	if ((*ref) == (*ns)) {
	    break;
	};
    };
    (*ns) = ((*ref) & 255) + 1;
    sr = &(st[TLINRANGELOW((*ref), 1, 256, 1)]);
    if ((*sr) == ((struct MT_StrRegs *) 0)) {
	(*sr) = (*fs).h;
	(*fs).h = (*fs).h->n;
	(*fs).n -= 1;
	(*sr)->t = isTemp;
	TLSTRASS(255, (*sr)->s, "");
    } else {
	(*ref) = 0;
    };
}
typedef	struct MT_StrRegs	*MT___x582[256];

static void MT_frees (fs, st, ref)
struct MT_FreeStrings	*fs;
MT___x582	st;
MT_StringIndex	ref;
{
    TLBIND((*sr), struct MT_StrRegs *);
    TLSTACKCHECK();
    sr = &(st[TLINRANGELOW(ref, 1, 256, 1)]);
    (*sr)->n = (*fs).h;
    (*fs).h = (*sr);
    (*fs).n += 1;
    (*sr) = (struct MT_StrRegs *) 0;
}

static void MT_load (p, virtAddr, v)
struct MT_Pages	*p;
TLnat4	virtAddr;
TLnat4	*v;
{
    TLnat4	virtPage;
    TLBIND((*page), struct MT_PageInfo);
    TLSTACKCHECK();
    virtPage = virtAddr >> 0x9;
    page = ((unsigned long) ((unsigned long)(*p).pages) + (virtPage * sizeof(struct MT_PageInfo)));
    if ((virtPage >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
	(*p).fault = 1;
	(*p).faultAddr = virtAddr;
	TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
    } else {
	(*v) = (* (TLnat4 *) (((*page).indx) + (virtAddr & 0x1FF)));
	(*page).status |= 0x20;
    };
}

static void MT_store (p, virtAddr, v)
struct MT_Pages	*p;
TLnat4	virtAddr;
TLnat4	v;
{
    TLnat4	virtPage;
    TLBIND((*page), struct MT_PageInfo);
    TLSTACKCHECK();
    virtPage = virtAddr >> 0x9;
    page = ((unsigned long) ((unsigned long)(*p).pages) + (virtPage * sizeof(struct MT_PageInfo)));
    if ((virtPage >= ((*p).numPages)) || ((TLSMLSMLSETCONST(3) & ((*page).status)) == 0)) {
	(*p).fault = 2;
	(*p).faultAddr = virtAddr;
	TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
    } else {
	(* (TLnat4 *) (((*page).indx) + (virtAddr & 0x1FF))) = v;
	(*page).status |= 0x60;
    };
}

static void MT_loadb (p, virtAddr, b)
struct MT_Pages	*p;
TLnat4	virtAddr;
TLnat1	*b;
{
    TLnat4	virtPage;
    TLBIND((*page), struct MT_PageInfo);
    TLSTACKCHECK();
    virtPage = virtAddr >> 0x9;
    page = ((unsigned long) ((unsigned long)(*p).pages) + (virtPage * sizeof(struct MT_PageInfo)));
    if ((virtPage >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
	(*p).fault = 1;
	(*p).faultAddr = virtAddr;
	TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
    } else {
	(*b) = (* (TLnat1 *) (((*page).indx) + (virtAddr & 0x1FF)));
	(*page).status |= 0x20;
    };
}

static void MT_storeb (p, virtAddr, b)
struct MT_Pages	*p;
TLnat4	virtAddr;
TLnat1	b;
{
    TLnat4	virtPage;
    TLBIND((*page), struct MT_PageInfo);
    TLSTACKCHECK();
    virtPage = virtAddr >> 0x9;
    page = ((unsigned long) ((unsigned long)(*p).pages) + (virtPage * sizeof(struct MT_PageInfo)));
    if ((virtPage >= ((*p).numPages)) || ((TLSMLSMLSETCONST(3) & ((*page).status)) == 0)) {
	(*p).fault = 2;
	(*p).faultAddr = virtAddr;
	TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
    } else {
	(* (TLnat1 *) (((*page).indx) + (virtAddr & 0x1FF))) = b;
	(*page).status |= 0x60;
    };
}

static void MT_loadcb (p, virtAddr, b)
struct MT_Pages	*p;
TLnat4	virtAddr;
TLnat1	*b;
{
    TLnat4	virtPage;
    TLBIND((*page), struct MT_PageInfo);
    TLSTACKCHECK();
    virtPage = virtAddr >> 0x9;
    page = ((unsigned long) ((unsigned long)(*p).pages) + (virtPage * sizeof(struct MT_PageInfo)));
    if ((virtPage >= ((*p).numPages)) || ((TLSMLSMLSETCONST(4) & ((*page).status)) == 0)) {
	(*p).fault = 3;
	(*p).faultAddr = virtAddr;
	TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
    } else {
	(*b) = (* (TLnat1 *) (((*page).indx) + (virtAddr & 0x1FF)));
	(*page).status |= 0x20;
    };
}

static void MT_interpret (cpu, p, fs, tf)
struct MT_RegisterDef	*cpu;
struct MT_Pages	*p;
struct MT_FreeStrings	*fs;
TLint4	tf;
{
    TLBIND((*pc), TLnat4);
    TLBIND((*oldpc), TLnat4);
    TLBIND((*oldmt), TLnat4);
    TLBIND((*ps), TLnat4);
    TLBIND((*mt), TLnat4);
    TLBIND((*st), MT___x576);
    TLBIND((*ns), MT_StringIndex);
    TLBIND((*display), MT___x577);
    TLBIND((*tS), TLnat4);
    TLBIND((*bp), TLnat4);
    TLchar	ch;
    TLnat1	LL;
    TLnat1	op;
    TLnat1	b;
    TLnat1	c;
    TLnat4	a;
    TLnat4	n;
    TLnat4	l;
    TLnat4	x;
    TLnat4	y;
    TLnat4	z;
    TLnat4	v;
    TLnat4	b1;
    TLnat4	b2;
    typedef	TLnat1	__x593[4];
    __x593	w;
    TLSTACKCHECK();
    pc = &((*cpu).pc);
    oldpc = &((*cpu).oldpc);
    oldmt = &((*cpu).oldmt);
    ps = &((*cpu).ps);
    mt = &((*cpu).mt);
    st = (*cpu).st;
    ns = &((*cpu).ns);
    display = (*cpu).display;
    tS = &((*cpu).timeSlice);
    bp = &((*cpu).breakpoint);
    for(;;) {
	if (((*fs).n) == 0) {
	    break;
	};
	(*oldpc) = (*pc);
	(*oldmt) = (*mt);
	{
	    TLnat4	vp;
	    TLBIND((*page), struct MT_PageInfo);
	    vp = (* (TLnat4 *) &(*pc)) >> 0x9;
	    page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
	    if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(4) & ((*page).status)) == 0)) {
		(*p).fault = 3;
		(*p).faultAddr = * (TLnat4 *) &(*pc);
		TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
	    } else {
		op = (* (TLnat1 *) (((*page).indx) + ((* (TLnat4 *) &(*pc)) & 0x1FF)));
		(*page).status |= 0x20;
	    };
	};
	(*pc) += 1;
	switch (op) {
	    case 0:
		{
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*pc)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(4) & ((*page).status)) == 0)) {
			    (*p).fault = 3;
			    (*p).faultAddr = * (TLnat4 *) &(*pc);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    LL = (* (TLnat1 *) (((*page).indx) + ((* (TLnat4 *) &(*pc)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    (*pc) += 1;
		    if (LL >= 20) {
			(*ps) = 2;
			TL_TLE_TLEQUIT ((TLint4) 202, (char *) 0, 0);
		    };
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*pc)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(4) & ((*page).status)) == 0)) {
			    (*p).fault = 3;
			    (*p).faultAddr = * (TLnat4 *) &(*pc);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    w[0] = (* (TLnat1 *) (((*page).indx) + ((* (TLnat4 *) &(*pc)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    (*pc) += 1;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*pc)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(4) & ((*page).status)) == 0)) {
			    (*p).fault = 3;
			    (*p).faultAddr = * (TLnat4 *) &(*pc);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    w[1] = (* (TLnat1 *) (((*page).indx) + ((* (TLnat4 *) &(*pc)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    (*pc) += 1;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*pc)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(4) & ((*page).status)) == 0)) {
			    (*p).fault = 3;
			    (*p).faultAddr = * (TLnat4 *) &(*pc);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    w[2] = (* (TLnat1 *) (((*page).indx) + ((* (TLnat4 *) &(*pc)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    (*pc) += 1;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*pc)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(4) & ((*page).status)) == 0)) {
			    (*p).fault = 3;
			    (*p).faultAddr = * (TLnat4 *) &(*pc);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    w[3] = (* (TLnat1 *) (((*page).indx) + ((* (TLnat4 *) &(*pc)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    (*pc) += 1;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(3) & ((*page).status)) == 0)) {
			    (*p).fault = 2;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF))) = (TLnat4) (((*display)[TLINRANGELOW(LL, 0, 19, 1)]) + ((* (TLint4 *) w)));
			    (*page).status |= 0x60;
			};
		    };
		    (*mt) += 4;
		}
		break;
	    case 1:
		{
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    a = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    if ((a + 3) >= (*mt)) {
			(*ps) = 1;
			TL_TLE_TLEQUIT ((TLint4) 202, (char *) 0, 0);
		    };
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &a) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &a;
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    v = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &a) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(3) & ((*page).status)) == 0)) {
			    (*p).fault = 2;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF))) = v;
			    (*page).status |= 0x60;
			};
		    };
		    (*mt) += 4;
		}
		break;
	    case 2:
		{
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    a = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    if (a >= (*mt)) {
			(*ps) = 1;
			TL_TLE_TLEQUIT ((TLint4) 202, (char *) 0, 0);
		    };
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &a) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &a;
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    b = (* (TLnat1 *) (((*page).indx) + ((* (TLnat4 *) &a) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(3) & ((*page).status)) == 0)) {
			    (*p).fault = 2;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF))) = b;
			    (*page).status |= 0x60;
			};
		    };
		    (*mt) += 4;
		}
		break;
	    case 3:
		{
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    v = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    a = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    if ((a + 3) >= (*mt)) {
			(*ps) = 1;
			TL_TLE_TLEQUIT ((TLint4) 202, (char *) 0, 0);
		    };
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &a) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(3) & ((*page).status)) == 0)) {
			    (*p).fault = 2;
			    (*p).faultAddr = * (TLnat4 *) &a;
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &a) & 0x1FF))) = v;
			    (*page).status |= 0x60;
			};
		    };
		}
		break;
	    case 4:
		{
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    v = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    a = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    if (a >= (*mt)) {
			(*ps) = 1;
			TL_TLE_TLEQUIT ((TLint4) 202, (char *) 0, 0);
		    };
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &a) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(3) & ((*page).status)) == 0)) {
			    (*p).fault = 2;
			    (*p).faultAddr = * (TLnat4 *) &a;
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    (* (TLnat1 *) (((*page).indx) + ((* (TLnat4 *) &a) & 0x1FF))) = v % 256;
			    (*page).status |= 0x60;
			};
		    };
		}
		break;
	    case 5:
		{
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*pc)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(4) & ((*page).status)) == 0)) {
			    (*p).fault = 3;
			    (*p).faultAddr = * (TLnat4 *) &(*pc);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    w[0] = (* (TLnat1 *) (((*page).indx) + ((* (TLnat4 *) &(*pc)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    (*pc) += 1;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*pc)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(4) & ((*page).status)) == 0)) {
			    (*p).fault = 3;
			    (*p).faultAddr = * (TLnat4 *) &(*pc);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    w[1] = (* (TLnat1 *) (((*page).indx) + ((* (TLnat4 *) &(*pc)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    (*pc) += 1;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*pc)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(4) & ((*page).status)) == 0)) {
			    (*p).fault = 3;
			    (*p).faultAddr = * (TLnat4 *) &(*pc);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    w[2] = (* (TLnat1 *) (((*page).indx) + ((* (TLnat4 *) &(*pc)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    (*pc) += 1;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*pc)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(4) & ((*page).status)) == 0)) {
			    (*p).fault = 3;
			    (*p).faultAddr = * (TLnat4 *) &(*pc);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    w[3] = (* (TLnat1 *) (((*page).indx) + ((* (TLnat4 *) &(*pc)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    (*pc) += 1;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(3) & ((*page).status)) == 0)) {
			    (*p).fault = 2;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF))) = (* (TLnat4 *) w);
			    (*page).status |= 0x60;
			};
		    };
		    (*mt) += 4;
		}
		break;
	    case 6:
		{
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(3) & ((*page).status)) == 0)) {
			    (*p).fault = 2;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF))) = (*mt);
			    (*page).status |= 0x60;
			};
		    };
		    (*mt) += 4;
		}
		break;
	    case 7:
		{
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(3) & ((*page).status)) == 0)) {
			    (*p).fault = 2;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF))) = (*pc);
			    (*page).status |= 0x60;
			};
		    };
		    (*mt) += 4;
		}
		break;
	    case 8:
		{
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*pc)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(4) & ((*page).status)) == 0)) {
			    (*p).fault = 3;
			    (*p).faultAddr = * (TLnat4 *) &(*pc);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    LL = (* (TLnat1 *) (((*page).indx) + ((* (TLnat4 *) &(*pc)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    (*pc) += 1;
		    if (LL >= 20) {
			(*ps) = 2;
			TL_TLE_TLEQUIT ((TLint4) 202, (char *) 0, 0);
		    };
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(3) & ((*page).status)) == 0)) {
			    (*p).fault = 2;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF))) = (*display)[TLINRANGELOW(LL, 0, 19, 1)];
			    (*page).status |= 0x60;
			};
		    };
		    (*mt) += 4;
		}
		break;
	    case 9:
		{
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*pc)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(4) & ((*page).status)) == 0)) {
			    (*p).fault = 3;
			    (*p).faultAddr = * (TLnat4 *) &(*pc);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    LL = (* (TLnat1 *) (((*page).indx) + ((* (TLnat4 *) &(*pc)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    (*pc) += 1;
		    if (LL >= 20) {
			(*ps) = 2;
			TL_TLE_TLEQUIT ((TLint4) 202, (char *) 0, 0);
		    };
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    (*display)[TLINRANGELOW(LL, 0, 19, 1)] = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		}
		break;
	    case 10:
		{
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    n = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    (*mt) -= 4 * n;
		}
		break;
	    case 11:
		{
		    TLnat4	newMt;
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    n = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    v = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    newMt = (*mt) + (n * 4);
		    (*mt) += n * 4;
		    {
			register TLint4	__x301;
			TLint4	__x664;
			__x664 = n;
			__x301 = 1;
			if (__x301 <= __x664) {
			    for(;;) {
				(*mt) -= 4;
				{
				    TLnat4	vp;
				    TLBIND((*page), struct MT_PageInfo);
				    vp = (* (TLnat4 *) &(*mt)) >> 0x9;
				    page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
				    if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(3) & ((*page).status)) == 0)) {
					(*p).fault = 2;
					(*p).faultAddr = * (TLnat4 *) &(*mt);
					TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
				    } else {
					(* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF))) = v;
					(*page).status |= 0x60;
				    };
				};
				if (__x301 == __x664) break;
				__x301++;
			    }
			};
		    };
		    (*mt) = newMt;
		}
		break;
	    case 12:
	    case 14:
	    case 15:
		{
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    (*pc) = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		}
		break;
	    case 13:
		{
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    a = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    v = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    if (v == 0) {
			(*pc) = a;
		    };
		}
		break;
	    case 16:
		{
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    v = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    (*pc) += ((* (TLint4 *) &v)) - 1;
		}
		break;
	    case 17:
		{
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    a = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    v = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    if (v == 0) {
			(*pc) += ((* (TLint4 *) &a)) - 1;
		    };
		}
		break;
	    case 18:
		{
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    y = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    x = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(3) & ((*page).status)) == 0)) {
			    (*p).fault = 2;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF))) = (TLnat4) (((* (TLint4 *) &x)) + ((* (TLint4 *) &y)));
			    (*page).status |= 0x60;
			};
		    };
		    (*mt) += 4;
		}
		break;
	    case 19:
		{
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    a = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    if ((a + 3) >= (*mt)) {
			(*ps) = 1;
			TL_TLE_TLEQUIT ((TLint4) 202, (char *) 0, 0);
		    };
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &a) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &a;
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    x = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &a) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &a) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(3) & ((*page).status)) == 0)) {
			    (*p).fault = 2;
			    (*p).faultAddr = * (TLnat4 *) &a;
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &a) & 0x1FF))) = (TLnat4) (((* (TLint4 *) &x)) + 1);
			    (*page).status |= 0x60;
			};
		    };
		}
		break;
	    case 20:
		{
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    y = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    x = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(3) & ((*page).status)) == 0)) {
			    (*p).fault = 2;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF))) = (TLnat4) (((* (TLint4 *) &x)) - ((* (TLint4 *) &y)));
			    (*page).status |= 0x60;
			};
		    };
		    (*mt) += 4;
		}
		break;
	    case 21:
		{
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    a = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    if ((a + 3) >= (*mt)) {
			(*ps) = 1;
			TL_TLE_TLEQUIT ((TLint4) 202, (char *) 0, 0);
		    };
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &a) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &a;
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    x = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &a) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &a) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(3) & ((*page).status)) == 0)) {
			    (*p).fault = 2;
			    (*p).faultAddr = * (TLnat4 *) &a;
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &a) & 0x1FF))) = (TLnat4) (((* (TLint4 *) &x)) - 1);
			    (*page).status |= 0x60;
			};
		    };
		}
		break;
	    case 22:
		{
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    y = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    x = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(3) & ((*page).status)) == 0)) {
			    (*p).fault = 2;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF))) = (TLnat4) (((* (TLint4 *) &x)) * ((* (TLint4 *) &y)));
			    (*page).status |= 0x60;
			};
		    };
		    (*mt) += 4;
		}
		break;
	    case 23:
		{
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    y = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    x = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(3) & ((*page).status)) == 0)) {
			    (*p).fault = 2;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF))) = (TLnat4) (((* (TLint4 *) &x)) / ((* (TLint4 *) &y)));
			    (*page).status |= 0x60;
			};
		    };
		    (*mt) += 4;
		}
		break;
	    case 24:
		{
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    y = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    x = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(3) & ((*page).status)) == 0)) {
			    (*p).fault = 2;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF))) = (TLnat4) (((* (TLint4 *) &x)) % ((* (TLint4 *) &y)));
			    (*page).status |= 0x60;
			};
		    };
		    (*mt) += 4;
		}
		break;
	    case 25:
		{
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    y = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    x = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(3) & ((*page).status)) == 0)) {
			    (*p).fault = 2;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF))) = (TLnat4) TL_TLA_TLAPII((TLint4) ((* (TLint4 *) &y)), (TLint4) ((* (TLint4 *) &x)));
			    (*page).status |= 0x60;
			};
		    };
		    (*mt) += 4;
		}
		break;
	    case 26:
		{
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    y = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    x = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    if (((* (TLint4 *) &x)) == ((* (TLint4 *) &y))) {
			{
			    TLnat4	vp;
			    TLBIND((*page), struct MT_PageInfo);
			    vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			    page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			    if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(3) & ((*page).status)) == 0)) {
				(*p).fault = 2;
				(*p).faultAddr = * (TLnat4 *) &(*mt);
				TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			    } else {
				(* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF))) = 1;
				(*page).status |= 0x60;
			    };
			};
			(*mt) += 4;
		    } else {
			{
			    TLnat4	vp;
			    TLBIND((*page), struct MT_PageInfo);
			    vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			    page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			    if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(3) & ((*page).status)) == 0)) {
				(*p).fault = 2;
				(*p).faultAddr = * (TLnat4 *) &(*mt);
				TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			    } else {
				(* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF))) = 0;
				(*page).status |= 0x60;
			    };
			};
			(*mt) += 4;
		    };
		}
		break;
	    case 27:
		{
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    y = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    x = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    if (((* (TLint4 *) &x)) < ((* (TLint4 *) &y))) {
			{
			    TLnat4	vp;
			    TLBIND((*page), struct MT_PageInfo);
			    vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			    page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			    if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(3) & ((*page).status)) == 0)) {
				(*p).fault = 2;
				(*p).faultAddr = * (TLnat4 *) &(*mt);
				TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			    } else {
				(* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF))) = 1;
				(*page).status |= 0x60;
			    };
			};
			(*mt) += 4;
		    } else {
			{
			    TLnat4	vp;
			    TLBIND((*page), struct MT_PageInfo);
			    vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			    page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			    if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(3) & ((*page).status)) == 0)) {
				(*p).fault = 2;
				(*p).faultAddr = * (TLnat4 *) &(*mt);
				TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			    } else {
				(* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF))) = 0;
				(*page).status |= 0x60;
			    };
			};
			(*mt) += 4;
		    };
		}
		break;
	    case 28:
		{
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    b1 = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    b2 = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    if ((b1 == 1) && (b2 == 1)) {
			{
			    TLnat4	vp;
			    TLBIND((*page), struct MT_PageInfo);
			    vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			    page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			    if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(3) & ((*page).status)) == 0)) {
				(*p).fault = 2;
				(*p).faultAddr = * (TLnat4 *) &(*mt);
				TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			    } else {
				(* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF))) = 1;
				(*page).status |= 0x60;
			    };
			};
			(*mt) += 4;
		    } else {
			{
			    TLnat4	vp;
			    TLBIND((*page), struct MT_PageInfo);
			    vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			    page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			    if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(3) & ((*page).status)) == 0)) {
				(*p).fault = 2;
				(*p).faultAddr = * (TLnat4 *) &(*mt);
				TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			    } else {
				(* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF))) = 0;
				(*page).status |= 0x60;
			    };
			};
			(*mt) += 4;
		    };
		}
		break;
	    case 29:
		{
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    b1 = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    b2 = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    if ((b1 == 1) || (b2 == 1)) {
			{
			    TLnat4	vp;
			    TLBIND((*page), struct MT_PageInfo);
			    vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			    page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			    if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(3) & ((*page).status)) == 0)) {
				(*p).fault = 2;
				(*p).faultAddr = * (TLnat4 *) &(*mt);
				TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			    } else {
				(* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF))) = 1;
				(*page).status |= 0x60;
			    };
			};
			(*mt) += 4;
		    } else {
			{
			    TLnat4	vp;
			    TLBIND((*page), struct MT_PageInfo);
			    vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			    page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			    if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(3) & ((*page).status)) == 0)) {
				(*p).fault = 2;
				(*p).faultAddr = * (TLnat4 *) &(*mt);
				TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			    } else {
				(* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF))) = 0;
				(*page).status |= 0x60;
			    };
			};
			(*mt) += 4;
		    };
		}
		break;
	    case 30:
		{
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    b1 = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    if (b1 == 0) {
			{
			    TLnat4	vp;
			    TLBIND((*page), struct MT_PageInfo);
			    vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			    page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			    if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(3) & ((*page).status)) == 0)) {
				(*p).fault = 2;
				(*p).faultAddr = * (TLnat4 *) &(*mt);
				TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			    } else {
				(* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF))) = 1;
				(*page).status |= 0x60;
			    };
			};
			(*mt) += 4;
		    } else {
			{
			    TLnat4	vp;
			    TLBIND((*page), struct MT_PageInfo);
			    vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			    page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			    if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(3) & ((*page).status)) == 0)) {
				(*p).fault = 2;
				(*p).faultAddr = * (TLnat4 *) &(*mt);
				TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			    } else {
				(* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF))) = 0;
				(*page).status |= 0x60;
			    };
			};
			(*mt) += 4;
		    };
		}
		break;
	    case 43:
		{
		    TLnat4	firstPage;
		    TLnat4	lastPage;
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    n = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    firstPage = (*mt) / 0x200;
		    lastPage = (((*mt) + (4 * n)) - 1) / 0x200;
		    {
			register TLint4	vPage;
			TLint4	__x787;
			__x787 = lastPage;
			vPage = firstPage;
			if (vPage <= __x787) {
			    for(;;) {
				{
				    TLnat4	vp;
				    TLBIND((*page), struct MT_PageInfo);
				    vp = ((TLnat4) (vPage * 0x200)) >> 0x9;
				    page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
				    if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
					(*p).fault = 1;
					(*p).faultAddr = (TLnat4) (vPage * 0x200);
					TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
				    } else {
					b = (* (TLnat1 *) (((*page).indx) + (((TLnat4) (vPage * 0x200)) & 0x1FF)));
					(*page).status |= 0x20;
				    };
				};
				if (vPage == __x787) break;
				vPage++;
			    }
			};
		    };
		    if (((*fs).n) < n) {
			(*pc) = (*oldpc);
			(*mt) = (*oldmt);
			goto __x944;
		    };
		    {
			register TLint4	i;
			TLint4	__x790;
			__x790 = n;
			i = 1;
			if (i <= __x790) {
			    for(;;) {
				if (((*st)[TLINRANGELOW((*ns), 1, 256, 1)]) == ((struct MT_StrRegs *) 0)) {
				    TLBINDREG((*sr), struct MT_StrRegs *);
				    sr = &((*st)[TLINRANGELOW((*ns), 1, 256, 1)]);
				    x = (*ns);
				    (*ns) = (x & 255) + 1;
				    (*sr) = (*fs).h;
				    (*fs).h = (*sr)->n;
				    (*fs).n -= 1;
				    (*sr)->t = 0;
				    TLSTRASS(255, (*sr)->s, "");
				} else {
				    MT_onews(&((*fs)), (*st), &((*ns)), &(x), (TLboolean) 0);
				};
				{
				    TLnat4	vp;
				    TLBIND((*page), struct MT_PageInfo);
				    vp = (* (TLnat4 *) &(*mt)) >> 0x9;
				    page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
				    if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(3) & ((*page).status)) == 0)) {
					(*p).fault = 2;
					(*p).faultAddr = * (TLnat4 *) &(*mt);
					TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
				    } else {
					(* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF))) = x;
					(*page).status |= 0x60;
				    };
				};
				(*mt) += 4;
				if (i == __x790) break;
				i++;
			    }
			};
		    };
		}
		break;
	    case 47:
		{
		    TLstring	tempStr;
		    TLSTRASS(255, tempStr, "");
		    for(;;) {
			{
			    TLnat4	vp;
			    TLBIND((*page), struct MT_PageInfo);
			    vp = (* (TLnat4 *) &(*pc)) >> 0x9;
			    page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			    if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(4) & ((*page).status)) == 0)) {
				(*p).fault = 3;
				(*p).faultAddr = * (TLnat4 *) &(*pc);
				TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			    } else {
				c = (* (TLnat1 *) (((*page).indx) + ((* (TLnat4 *) &(*pc)) & 0x1FF)));
				(*page).status |= 0x20;
			    };
			};
			(*pc) += 1;
			if (c == 0) {
			    break;
			};
			{
			    TLstring	__x795;
			    {
				TLchar	__x796[2];
				TLCHRTOSTR(((TLchar) c), __x796);
				TL_TLS_TLSCAT(tempStr, __x796, __x795);
			    };
			    TLSTRASS(255, tempStr, __x795);
			};
		    };
		    if (((*st)[TLINRANGELOW((*ns), 1, 256, 1)]) == ((struct MT_StrRegs *) 0)) {
			TLBINDREG((*sr), struct MT_StrRegs *);
			sr = &((*st)[TLINRANGELOW((*ns), 1, 256, 1)]);
			x = (*ns);
			(*ns) = (x & 255) + 1;
			(*sr) = (*fs).h;
			(*fs).h = (*sr)->n;
			(*fs).n -= 1;
			(*sr)->t = 1;
			TLSTRASS(255, (*sr)->s, "");
		    } else {
			MT_onews(&((*fs)), (*st), &((*ns)), &(x), (TLboolean) 1);
		    };
		    TLSTRASS(255, (*st)[TLINRANGELOW(x, 1, 256, 1)]->s, tempStr);
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(3) & ((*page).status)) == 0)) {
			    (*p).fault = 2;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF))) = x;
			    (*page).status |= 0x60;
			};
		    };
		    (*mt) += 4;
		}
		break;
	    case 44:
		{
		    TLnat4	lastPage;
		    TLnat4	firstPage;
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    n = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    lastPage = ((*mt) - 1) / 0x200;
		    firstPage = ((*mt) - (4 * n)) / 0x200;
		    {
			register TLint4	vPage;
			TLint4	__x801;
			__x801 = lastPage;
			vPage = firstPage;
			if (vPage <= __x801) {
			    for(;;) {
				{
				    TLnat4	vp;
				    TLBIND((*page), struct MT_PageInfo);
				    vp = ((TLnat4) (vPage * 0x200)) >> 0x9;
				    page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
				    if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
					(*p).fault = 1;
					(*p).faultAddr = (TLnat4) (vPage * 0x200);
					TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
				    } else {
					b = (* (TLnat1 *) (((*page).indx) + (((TLnat4) (vPage * 0x200)) & 0x1FF)));
					(*page).status |= 0x20;
				    };
				};
				if (vPage == __x801) break;
				vPage++;
			    }
			};
		    };
		    {
			register TLint4	__x426;
			TLint4	__x804;
			__x804 = n;
			__x426 = 1;
			if (__x426 <= __x804) {
			    for(;;) {
				(*mt) -= 4;
				{
				    TLnat4	vp;
				    TLBIND((*page), struct MT_PageInfo);
				    vp = (* (TLnat4 *) &(*mt)) >> 0x9;
				    page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
				    if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
					(*p).fault = 1;
					(*p).faultAddr = * (TLnat4 *) &(*mt);
					TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
				    } else {
					x = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
					(*page).status |= 0x20;
				    };
				};
				{
				    TLBIND((*sr), struct MT_StrRegs *);
				    sr = &((*st)[TLINRANGELOW(x, 1, 256, 1)]);
				    (*sr)->n = (*fs).h;
				    (*fs).h = (*sr);
				    (*fs).n += 1;
				    (*sr) = (struct MT_StrRegs *) 0;
				};
				if (__x426 == __x804) break;
				__x426++;
			    }
			};
		    };
		}
		break;
	    case 49:
		{
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    x = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    a = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &a) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &a;
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    y = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &a) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    TLSTRASS(255, (*st)[TLINRANGELOW(y, 1, 256, 1)]->s, (*st)[TLINRANGELOW(x, 1, 256, 1)]->s);
		    if ((*st)[TLINRANGELOW(x, 1, 256, 1)]->t) {
			{
			    TLBIND((*sr), struct MT_StrRegs *);
			    sr = &((*st)[TLINRANGELOW(x, 1, 256, 1)]);
			    (*sr)->n = (*fs).h;
			    (*fs).h = (*sr);
			    (*fs).n += 1;
			    (*sr) = (struct MT_StrRegs *) 0;
			};
		    };
		}
		break;
	    case 50:
		{
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    x = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    if ((*st)[TLINRANGELOW(x, 1, 256, 1)]->t) {
			(*st)[TLINRANGELOW(x, 1, 256, 1)]->t = 0;
			(*mt) += 4;
		    } else {
			if (((*st)[TLINRANGELOW((*ns), 1, 256, 1)]) == ((struct MT_StrRegs *) 0)) {
			    TLBINDREG((*sr), struct MT_StrRegs *);
			    sr = &((*st)[TLINRANGELOW((*ns), 1, 256, 1)]);
			    y = (*ns);
			    (*ns) = (y & 255) + 1;
			    (*sr) = (*fs).h;
			    (*fs).h = (*sr)->n;
			    (*fs).n -= 1;
			    (*sr)->t = 0;
			    TLSTRASS(255, (*sr)->s, "");
			} else {
			    MT_onews(&((*fs)), (*st), &((*ns)), &(y), (TLboolean) 0);
			};
			TLSTRASS(255, (*st)[TLINRANGELOW(y, 1, 256, 1)]->s, (*st)[TLINRANGELOW(x, 1, 256, 1)]->s);
			{
			    TLnat4	vp;
			    TLBIND((*page), struct MT_PageInfo);
			    vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			    page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			    if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(3) & ((*page).status)) == 0)) {
				(*p).fault = 2;
				(*p).faultAddr = * (TLnat4 *) &(*mt);
				TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			    } else {
				(* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF))) = y;
				(*page).status |= 0x60;
			    };
			};
			(*mt) += 4;
		    };
		}
		break;
	    case 31:
		{
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    x = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(3) & ((*page).status)) == 0)) {
			    (*p).fault = 2;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF))) = (TLnat4) TL_TLS_TLSLEN(((*st)[TLINRANGELOW(x, 1, 256, 1)]->s));
			    (*page).status |= 0x60;
			};
		    };
		    (*mt) += 4;
		    if ((*st)[TLINRANGELOW(x, 1, 256, 1)]->t) {
			{
			    TLBIND((*sr), struct MT_StrRegs *);
			    sr = &((*st)[TLINRANGELOW(x, 1, 256, 1)]);
			    (*sr)->n = (*fs).h;
			    (*fs).h = (*sr);
			    (*fs).n += 1;
			    (*sr) = (struct MT_StrRegs *) 0;
			};
		    };
		}
		break;
	    case 32:
		{
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    y = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    x = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    if ((*st)[TLINRANGELOW(x, 1, 256, 1)]->t) {
			{
			    TLstring	__x825;
			    TL_TLS_TLSCAT((*st)[TLINRANGELOW(x, 1, 256, 1)]->s, (*st)[TLINRANGELOW(y, 1, 256, 1)]->s, __x825);
			    TLSTRASS(255, (*st)[TLINRANGELOW(x, 1, 256, 1)]->s, __x825);
			};
			(*mt) += 4;
		    } else {
			if (((*st)[TLINRANGELOW((*ns), 1, 256, 1)]) == ((struct MT_StrRegs *) 0)) {
			    TLBINDREG((*sr), struct MT_StrRegs *);
			    sr = &((*st)[TLINRANGELOW((*ns), 1, 256, 1)]);
			    z = (*ns);
			    (*ns) = (z & 255) + 1;
			    (*sr) = (*fs).h;
			    (*fs).h = (*sr)->n;
			    (*fs).n -= 1;
			    (*sr)->t = 1;
			    TLSTRASS(255, (*sr)->s, "");
			} else {
			    MT_onews(&((*fs)), (*st), &((*ns)), &(z), (TLboolean) 1);
			};
			{
			    TLstring	__x826;
			    TL_TLS_TLSCAT((*st)[TLINRANGELOW(x, 1, 256, 1)]->s, (*st)[TLINRANGELOW(y, 1, 256, 1)]->s, __x826);
			    TLSTRASS(255, (*st)[TLINRANGELOW(z, 1, 256, 1)]->s, __x826);
			};
			{
			    TLnat4	vp;
			    TLBIND((*page), struct MT_PageInfo);
			    vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			    page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			    if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(3) & ((*page).status)) == 0)) {
				(*p).fault = 2;
				(*p).faultAddr = * (TLnat4 *) &(*mt);
				TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			    } else {
				(* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF))) = z;
				(*page).status |= 0x60;
			    };
			};
			(*mt) += 4;
		    };
		    if ((*st)[TLINRANGELOW(y, 1, 256, 1)]->t) {
			{
			    TLBIND((*sr), struct MT_StrRegs *);
			    sr = &((*st)[TLINRANGELOW(y, 1, 256, 1)]);
			    (*sr)->n = (*fs).h;
			    (*fs).h = (*sr);
			    (*fs).n += 1;
			    (*sr) = (struct MT_StrRegs *) 0;
			};
		    };
		}
		break;
	    case 33:
		{
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    n = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    x = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    a = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    if (((*st)[TLINRANGELOW((*ns), 1, 256, 1)]) == ((struct MT_StrRegs *) 0)) {
			TLBINDREG((*sr), struct MT_StrRegs *);
			sr = &((*st)[TLINRANGELOW((*ns), 1, 256, 1)]);
			z = (*ns);
			(*ns) = (z & 255) + 1;
			(*sr) = (*fs).h;
			(*fs).h = (*sr)->n;
			(*fs).n -= 1;
			(*sr)->t = 1;
			TLSTRASS(255, (*sr)->s, "");
		    } else {
			MT_onews(&((*fs)), (*st), &((*ns)), &(z), (TLboolean) 1);
		    };
		    {
			TLstring	__x835;
			TL_TLS_TLSBXX(__x835, (TLint4) n, (TLint4) a, ((*st)[TLINRANGELOW(x, 1, 256, 1)]->s));
			TLSTRASS(255, (*st)[TLINRANGELOW(z, 1, 256, 1)]->s, __x835);
		    };
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(3) & ((*page).status)) == 0)) {
			    (*p).fault = 2;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF))) = z;
			    (*page).status |= 0x60;
			};
		    };
		    (*mt) += 4;
		    if ((*st)[TLINRANGELOW(x, 1, 256, 1)]->t) {
			{
			    TLBIND((*sr), struct MT_StrRegs *);
			    sr = &((*st)[TLINRANGELOW(x, 1, 256, 1)]);
			    (*sr)->n = (*fs).h;
			    (*fs).h = (*sr);
			    (*fs).n += 1;
			    (*sr) = (struct MT_StrRegs *) 0;
			};
		    };
		}
		break;
	    case 34:
		{
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    a = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    x = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    if ((*st)[TLINRANGELOW(x, 1, 256, 1)]->t) {
			{
			    TLchar	__x842[2];
			    TL_TLS_TLSBX(__x842, (TLint4) a, ((*st)[TLINRANGELOW(x, 1, 256, 1)]->s));
			    TLSTRASS(255, (*st)[TLINRANGELOW(x, 1, 256, 1)]->s, __x842);
			};
			(*mt) += 4;
		    } else {
			if (((*st)[TLINRANGELOW((*ns), 1, 256, 1)]) == ((struct MT_StrRegs *) 0)) {
			    TLBINDREG((*sr), struct MT_StrRegs *);
			    sr = &((*st)[TLINRANGELOW((*ns), 1, 256, 1)]);
			    z = (*ns);
			    (*ns) = (z & 255) + 1;
			    (*sr) = (*fs).h;
			    (*fs).h = (*sr)->n;
			    (*fs).n -= 1;
			    (*sr)->t = 1;
			    TLSTRASS(255, (*sr)->s, "");
			} else {
			    MT_onews(&((*fs)), (*st), &((*ns)), &(z), (TLboolean) 1);
			};
			{
			    TLchar	__x843[2];
			    TL_TLS_TLSBX(__x843, (TLint4) a, ((*st)[TLINRANGELOW(x, 1, 256, 1)]->s));
			    TLSTRASS(255, (*st)[TLINRANGELOW(z, 1, 256, 1)]->s, __x843);
			};
			{
			    TLnat4	vp;
			    TLBIND((*page), struct MT_PageInfo);
			    vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			    page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			    if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(3) & ((*page).status)) == 0)) {
				(*p).fault = 2;
				(*p).faultAddr = * (TLnat4 *) &(*mt);
				TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			    } else {
				(* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF))) = z;
				(*page).status |= 0x60;
			    };
			};
			(*mt) += 4;
		    };
		}
		break;
	    case 35:
		{
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    y = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    x = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    if (strcmp((*st)[TLINRANGELOW(x, 1, 256, 1)]->s, (*st)[TLINRANGELOW(y, 1, 256, 1)]->s) == 0) {
			{
			    TLnat4	vp;
			    TLBIND((*page), struct MT_PageInfo);
			    vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			    page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			    if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(3) & ((*page).status)) == 0)) {
				(*p).fault = 2;
				(*p).faultAddr = * (TLnat4 *) &(*mt);
				TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			    } else {
				(* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF))) = 1;
				(*page).status |= 0x60;
			    };
			};
			(*mt) += 4;
		    } else {
			{
			    TLnat4	vp;
			    TLBIND((*page), struct MT_PageInfo);
			    vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			    page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			    if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(3) & ((*page).status)) == 0)) {
				(*p).fault = 2;
				(*p).faultAddr = * (TLnat4 *) &(*mt);
				TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			    } else {
				(* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF))) = 0;
				(*page).status |= 0x60;
			    };
			};
			(*mt) += 4;
		    };
		    if ((*st)[TLINRANGELOW(x, 1, 256, 1)]->t) {
			{
			    TLBIND((*sr), struct MT_StrRegs *);
			    sr = &((*st)[TLINRANGELOW(x, 1, 256, 1)]);
			    (*sr)->n = (*fs).h;
			    (*fs).h = (*sr);
			    (*fs).n += 1;
			    (*sr) = (struct MT_StrRegs *) 0;
			};
		    };
		    if ((*st)[TLINRANGELOW(y, 1, 256, 1)]->t) {
			{
			    TLBIND((*sr), struct MT_StrRegs *);
			    sr = &((*st)[TLINRANGELOW(y, 1, 256, 1)]);
			    (*sr)->n = (*fs).h;
			    (*fs).h = (*sr);
			    (*fs).n += 1;
			    (*sr) = (struct MT_StrRegs *) 0;
			};
		    };
		}
		break;
	    case 36:
		{
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    y = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    x = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    if (strcmp((*st)[TLINRANGELOW(x, 1, 256, 1)]->s, (*st)[TLINRANGELOW(y, 1, 256, 1)]->s) < 0) {
			{
			    TLnat4	vp;
			    TLBIND((*page), struct MT_PageInfo);
			    vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			    page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			    if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(3) & ((*page).status)) == 0)) {
				(*p).fault = 2;
				(*p).faultAddr = * (TLnat4 *) &(*mt);
				TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			    } else {
				(* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF))) = 1;
				(*page).status |= 0x60;
			    };
			};
			(*mt) += 4;
		    } else {
			{
			    TLnat4	vp;
			    TLBIND((*page), struct MT_PageInfo);
			    vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			    page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			    if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(3) & ((*page).status)) == 0)) {
				(*p).fault = 2;
				(*p).faultAddr = * (TLnat4 *) &(*mt);
				TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			    } else {
				(* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF))) = 0;
				(*page).status |= 0x60;
			    };
			};
			(*mt) += 4;
		    };
		    if ((*st)[TLINRANGELOW(x, 1, 256, 1)]->t) {
			{
			    TLBIND((*sr), struct MT_StrRegs *);
			    sr = &((*st)[TLINRANGELOW(x, 1, 256, 1)]);
			    (*sr)->n = (*fs).h;
			    (*fs).h = (*sr);
			    (*fs).n += 1;
			    (*sr) = (struct MT_StrRegs *) 0;
			};
		    };
		    if ((*st)[TLINRANGELOW(y, 1, 256, 1)]->t) {
			{
			    TLBIND((*sr), struct MT_StrRegs *);
			    sr = &((*st)[TLINRANGELOW(y, 1, 256, 1)]);
			    (*sr)->n = (*fs).h;
			    (*fs).h = (*sr);
			    (*fs).n += 1;
			    (*sr) = (struct MT_StrRegs *) 0;
			};
		    };
		}
		break;
	    case 48:
		{
		    TLBIND((*stv), TLstring);
		    TLnat4	firstPage;
		    TLnat4	lastPage;
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    n = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    x = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    stv = (*st)[TLINRANGELOW(x, 1, 256, 1)]->s;
		    firstPage = (*mt) / 0x200;
		    lastPage = (((*mt) + (4 * n)) - 1) / 0x200;
		    {
			register TLint4	vPage;
			TLint4	__x866;
			__x866 = lastPage;
			vPage = firstPage;
			if (vPage <= __x866) {
			    for(;;) {
				{
				    TLnat4	vp;
				    TLBIND((*page), struct MT_PageInfo);
				    vp = ((TLnat4) (vPage * 0x200)) >> 0x9;
				    page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
				    if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
					(*p).fault = 1;
					(*p).faultAddr = (TLnat4) (vPage * 0x200);
					TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
				    } else {
					b = (* (TLnat1 *) (((*page).indx) + (((TLnat4) (vPage * 0x200)) & 0x1FF)));
					(*page).status |= 0x20;
				    };
				};
				if (vPage == __x866) break;
				vPage++;
			    }
			};
		    };
		    {
			register TLint4	__x502;
			TLint4	__x869;
			__x869 = n;
			__x502 = 1;
			if (__x502 <= __x869) {
			    for(;;) {
				if (((*st)[TLINRANGELOW((*ns), 1, 256, 1)]) == ((struct MT_StrRegs *) 0)) {
				    TLBINDREG((*sr), struct MT_StrRegs *);
				    sr = &((*st)[TLINRANGELOW((*ns), 1, 256, 1)]);
				    z = (*ns);
				    (*ns) = (z & 255) + 1;
				    (*sr) = (*fs).h;
				    (*fs).h = (*sr)->n;
				    (*fs).n -= 1;
				    (*sr)->t = 1;
				    TLSTRASS(255, (*sr)->s, "");
				} else {
				    MT_onews(&((*fs)), (*st), &((*ns)), &(z), (TLboolean) 1);
				};
				TLSTRASS(255, (*st)[TLINRANGELOW(z, 1, 256, 1)]->s, (*stv));
				{
				    TLnat4	vp;
				    TLBIND((*page), struct MT_PageInfo);
				    vp = (* (TLnat4 *) &(*mt)) >> 0x9;
				    page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
				    if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(3) & ((*page).status)) == 0)) {
					(*p).fault = 2;
					(*p).faultAddr = * (TLnat4 *) &(*mt);
					TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
				    } else {
					(* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF))) = z;
					(*page).status |= 0x60;
				    };
				};
				(*mt) += 4;
				if (__x502 == __x869) break;
				__x502++;
			    }
			};
		    };
		    if ((*st)[TLINRANGELOW(x, 1, 256, 1)]->t) {
			{
			    TLBIND((*sr), struct MT_StrRegs *);
			    sr = &((*st)[TLINRANGELOW(x, 1, 256, 1)]);
			    (*sr)->n = (*fs).h;
			    (*fs).h = (*sr);
			    (*fs).n += 1;
			    (*sr) = (struct MT_StrRegs *) 0;
			};
		    };
		}
		break;
	    case 37:
		{
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    l = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    n = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    if (((*st)[TLINRANGELOW((*ns), 1, 256, 1)]) == ((struct MT_StrRegs *) 0)) {
			TLBINDREG((*sr), struct MT_StrRegs *);
			sr = &((*st)[TLINRANGELOW((*ns), 1, 256, 1)]);
			z = (*ns);
			(*ns) = (z & 255) + 1;
			(*sr) = (*fs).h;
			(*fs).h = (*sr)->n;
			(*fs).n -= 1;
			(*sr)->t = 1;
			TLSTRASS(255, (*sr)->s, "");
		    } else {
			MT_onews(&((*fs)), (*st), &((*ns)), &(z), (TLboolean) 1);
		    };
		    {
			TLstring	__x878;
			TL_TLS_TLSVIS((TLint4) ((* (TLint4 *) &n)), (TLint4) ((* (TLint4 *) &l)), (TLint4) 10, __x878);
			TLSTRASS(255, (*st)[TLINRANGELOW(z, 1, 256, 1)]->s, __x878);
		    };
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(3) & ((*page).status)) == 0)) {
			    (*p).fault = 2;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF))) = z;
			    (*page).status |= 0x60;
			};
		    };
		    (*mt) += 4;
		}
		break;
	    case 38:
		{
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    x = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(3) & ((*page).status)) == 0)) {
			    (*p).fault = 2;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF))) = (TLnat4) TL_TLS_TLSVSI(((*st)[TLINRANGELOW(x, 1, 256, 1)]->s), (TLint4) 10);
			    (*page).status |= 0x60;
			};
		    };
		    (*mt) += 4;
		    if ((*st)[TLINRANGELOW(x, 1, 256, 1)]->t) {
			{
			    TLBIND((*sr), struct MT_StrRegs *);
			    sr = &((*st)[TLINRANGELOW(x, 1, 256, 1)]);
			    (*sr)->n = (*fs).h;
			    (*fs).h = (*sr);
			    (*fs).n += 1;
			    (*sr) = (struct MT_StrRegs *) 0;
			};
		    };
		}
		break;
	    case 39:
		{
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    y = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    x = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(3) & ((*page).status)) == 0)) {
			    (*p).fault = 2;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF))) = (TLnat4) TL_TLS_TLSIND(((*st)[TLINRANGELOW(x, 1, 256, 1)]->s), ((*st)[TLINRANGELOW(y, 1, 256, 1)]->s));
			    (*page).status |= 0x60;
			};
		    };
		    (*mt) += 4;
		    if ((*st)[TLINRANGELOW(x, 1, 256, 1)]->t) {
			{
			    TLBIND((*sr), struct MT_StrRegs *);
			    sr = &((*st)[TLINRANGELOW(x, 1, 256, 1)]);
			    (*sr)->n = (*fs).h;
			    (*fs).h = (*sr);
			    (*fs).n += 1;
			    (*sr) = (struct MT_StrRegs *) 0;
			};
		    };
		    if ((*st)[TLINRANGELOW(y, 1, 256, 1)]->t) {
			{
			    TLBIND((*sr), struct MT_StrRegs *);
			    sr = &((*st)[TLINRANGELOW(y, 1, 256, 1)]);
			    (*sr)->n = (*fs).h;
			    (*fs).h = (*sr);
			    (*fs).n += 1;
			    (*sr) = (struct MT_StrRegs *) 0;
			};
		    };
		}
		break;
	    case 40:
		{
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    n = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    x = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    if ((*st)[TLINRANGELOW(x, 1, 256, 1)]->t) {
			{
			    TLstring	__x895;
			    TL_TLS_TLSRPT(((*st)[TLINRANGELOW(x, 1, 256, 1)]->s), (TLint4) n, __x895);
			    TLSTRASS(255, (*st)[TLINRANGELOW(x, 1, 256, 1)]->s, __x895);
			};
			(*mt) += 4;
		    } else {
			if (((*st)[TLINRANGELOW((*ns), 1, 256, 1)]) == ((struct MT_StrRegs *) 0)) {
			    TLBINDREG((*sr), struct MT_StrRegs *);
			    sr = &((*st)[TLINRANGELOW((*ns), 1, 256, 1)]);
			    z = (*ns);
			    (*ns) = (z & 255) + 1;
			    (*sr) = (*fs).h;
			    (*fs).h = (*sr)->n;
			    (*fs).n -= 1;
			    (*sr)->t = 1;
			    TLSTRASS(255, (*sr)->s, "");
			} else {
			    MT_onews(&((*fs)), (*st), &((*ns)), &(z), (TLboolean) 1);
			};
			{
			    TLstring	__x896;
			    TL_TLS_TLSRPT(((*st)[TLINRANGELOW(x, 1, 256, 1)]->s), (TLint4) n, __x896);
			    TLSTRASS(255, (*st)[TLINRANGELOW(z, 1, 256, 1)]->s, __x896);
			};
			{
			    TLnat4	vp;
			    TLBIND((*page), struct MT_PageInfo);
			    vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			    page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			    if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(3) & ((*page).status)) == 0)) {
				(*p).fault = 2;
				(*p).faultAddr = * (TLnat4 *) &(*mt);
				TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			    } else {
				(* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF))) = z;
				(*page).status |= 0x60;
			    };
			};
			(*mt) += 4;
		    };
		}
		break;
	    case 41:
		{
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    n = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    if (((*st)[TLINRANGELOW((*ns), 1, 256, 1)]) == ((struct MT_StrRegs *) 0)) {
			TLBINDREG((*sr), struct MT_StrRegs *);
			sr = &((*st)[TLINRANGELOW((*ns), 1, 256, 1)]);
			z = (*ns);
			(*ns) = (z & 255) + 1;
			(*sr) = (*fs).h;
			(*fs).h = (*sr)->n;
			(*fs).n -= 1;
			(*sr)->t = 1;
			TLSTRASS(255, (*sr)->s, "");
		    } else {
			MT_onews(&((*fs)), (*st), &((*ns)), &(z), (TLboolean) 1);
		    };
		    {
			TLchar	__x901[2];
			TLCHRTOSTR(((TLchar) n), __x901);
			TLSTRASS(255, (*st)[TLINRANGELOW(z, 1, 256, 1)]->s, __x901);
		    };
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(3) & ((*page).status)) == 0)) {
			    (*p).fault = 2;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF))) = z;
			    (*page).status |= 0x60;
			};
		    };
		    (*mt) += 4;
		}
		break;
	    case 42:
		{
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    x = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(3) & ((*page).status)) == 0)) {
			    (*p).fault = 2;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    {
				TLchar	__x908[2];
				TL_TLS_TLSBX(__x908, (TLint4) 1, ((*st)[TLINRANGELOW(x, 1, 256, 1)]->s));
				(* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF))) = ((TLnat4) TLCVTTOCHR(__x908));
			    };
			    (*page).status |= 0x60;
			};
		    };
		    (*mt) += 4;
		    if ((*st)[TLINRANGELOW(x, 1, 256, 1)]->t) {
			{
			    TLBIND((*sr), struct MT_StrRegs *);
			    sr = &((*st)[TLINRANGELOW(x, 1, 256, 1)]);
			    (*sr)->n = (*fs).h;
			    (*fs).h = (*sr);
			    (*fs).n += 1;
			    (*sr) = (struct MT_StrRegs *) 0;
			};
		    };
		}
		break;
	    case 46:
		{
		    TLBIND((*stx), TLstring);
		    TLint4	last;
		    TLnat4	sta;
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    x = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    a = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    stx = (*st)[TLINRANGELOW(x, 1, 256, 1)]->s;
		    last = (a + TL_TLS_TLSLEN((*stx))) - 1;
		    if (last >= (*mt)) {
			(*ps) = 1;
			TL_TLE_TLEQUIT ((TLint4) 202, (char *) 0, 0);
		    };
		    sta = ((unsigned long)(*stx));
		    {
			register TLint4	i;
			TLint4	__x913;
			__x913 = last - a;
			i = 0;
			if (i <= __x913) {
			    for(;;) {
				{
				    TLnat4	vp;
				    TLBIND((*page), struct MT_PageInfo);
				    vp = ((TLnat4) (a + i)) >> 0x9;
				    page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
				    if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(3) & ((*page).status)) == 0)) {
					(*p).fault = 2;
					(*p).faultAddr = (TLnat4) (a + i);
					TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
				    } else {
					(* (TLnat1 *) (((*page).indx) + (((TLnat4) (a + i)) & 0x1FF))) = (* (TLnat1 *) (sta + i));
					(*page).status |= 0x60;
				    };
				};
				if (i == __x913) break;
				i++;
			    }
			};
		    };
		    if ((*st)[TLINRANGELOW(x, 1, 256, 1)]->t) {
			{
			    TLBIND((*sr), struct MT_StrRegs *);
			    sr = &((*st)[TLINRANGELOW(x, 1, 256, 1)]);
			    (*sr)->n = (*fs).h;
			    (*fs).h = (*sr);
			    (*fs).n += 1;
			    (*sr) = (struct MT_StrRegs *) 0;
			};
		    };
		}
		break;
	    case 45:
		{
		    TLnat4	last;
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    n = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    a = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    x = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    last = (a + n) - 1;
		    if (last >= (*mt)) {
			(*ps) = 1;
			TL_TLE_TLEQUIT ((TLint4) 202, (char *) 0, 0);
		    };
		    TLSTRASS(255, (*st)[TLINRANGELOW(x, 1, 256, 1)]->s, "");
		    {
			register TLint4	i;
			TLint4	__x923;
			__x923 = last;
			i = a;
			if (i <= __x923) {
			    for(;;) {
				{
				    TLnat4	vp;
				    TLBIND((*page), struct MT_PageInfo);
				    vp = ((TLnat4) i) >> 0x9;
				    page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
				    if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
					(*p).fault = 1;
					(*p).faultAddr = (TLnat4) i;
					TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
				    } else {
					c = (* (TLnat1 *) (((*page).indx) + (((TLnat4) i) & 0x1FF)));
					(*page).status |= 0x20;
				    };
				};
				if (c == 0) {
				    break;
				};
				{
				    TLstring	__x926;
				    {
					TLchar	__x927[2];
					TLCHRTOSTR(((TLchar) c), __x927);
					TL_TLS_TLSCAT((*st)[TLINRANGELOW(x, 1, 256, 1)]->s, __x927, __x926);
				    };
				    TLSTRASS(255, (*st)[TLINRANGELOW(x, 1, 256, 1)]->s, __x926);
				};
				if (i == __x923) break;
				i++;
			    }
			};
		    };
		}
		break;
	    case 51:
		{
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    y = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    x = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(3) & ((*page).status)) == 0)) {
			    (*p).fault = 2;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF))) = y;
			    (*page).status |= 0x60;
			};
		    };
		    (*mt) += 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(3) & ((*page).status)) == 0)) {
			    (*p).fault = 2;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF))) = x;
			    (*page).status |= 0x60;
			};
		    };
		    (*mt) += 4;
		}
		break;
	    case 52:
		{
		    (*mt) -= 4;
		    {
			TLnat4	vp;
			TLBIND((*page), struct MT_PageInfo);
			vp = (* (TLnat4 *) &(*mt)) >> 0x9;
			page = ((unsigned long) ((unsigned long)(*p).pages) + (vp * sizeof(struct MT_PageInfo)));
			if ((vp >= ((*p).numPages)) || ((TLSMLSMLSETCONST(2) & ((*page).status)) == 0)) {
			    (*p).fault = 1;
			    (*p).faultAddr = * (TLnat4 *) &(*mt);
			    TL_TLE_TLEQUIT ((TLint4) 201, (char *) 0, -1);
			} else {
			    (*ps) = (* (TLnat4 *) (((*page).indx) + ((* (TLnat4 *) &(*mt)) & 0x1FF)));
			    (*page).status |= 0x20;
			};
		    };
		    TL_TLE_TLEQUIT ((TLint4) 202, (char *) 0, 0);
		}
		break;
	    case 53:
		{
		    (*ps) = 0;
		    TL_TLE_TLEQUIT ((TLint4) 202, (char *) 0, 0);
		}
		break;
	    case 54:
		break;
	    default:
		TLCASEABORT;
	};
	if ((*tS) > 0) {
	    (*tS) -= 1;
	    if ((*tS) == 0) {
		break;
	    };
	};
	if ((*pc) == (*bp)) {
	    (*ps) = 4;
	    TL_TLE_TLEQUIT ((TLint4) 202, (char *) 0, 0);
	};
    }
    __x944:;
}
static TLint4	MT_tf;

void MT_InterpretIfc (sp)
TLaddressint	sp;
{
    TLSTACKCHECK();
    MT_interpret(&((* (struct MT_RegisterDef *) ((* (TLaddressint *) sp)))), &((* (struct MT_Pages *) ((* (TLaddressint *) ((unsigned long) sp + 4))))), &((* (struct MT_FreeStrings *) ((* (TLaddressint *) ((unsigned long) sp + 8))))), (TLint4) MT_tf);
}

static TLboolean __x945;
void MT () {
    if (__x945 == 0) {
	__x945 = 1;
	TLASSERT (TL_TLA_TLAPII((TLint4) 8, (TLint4) 2) == 256);
	TLASSERT (TL_TLA_TLAPII((TLint4) 0x9, (TLint4) 2) == 0x200);
	MT_tf = 0;
    }
}
