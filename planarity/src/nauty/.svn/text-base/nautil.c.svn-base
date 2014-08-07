/*****************************************************************************
*                                                                            *
*  Auxiliary source file for version 1.9 of nauty.                           *
*                                                                            *
*   Copyright (1984-1993) Brendan McKay.  All rights reserved.               *
*   Subject to waivers and disclaimers in nauty.h.                           *
*                                                                            *
*   CHANGE HISTORY                                                           *
*       10-Nov-87 : final changes for version 1.2                            *
*        5-Dec-87 : renamed to version 1.3 (no changes to this file)         *
*       28-Sep-88 : renamed to version 1.4 (no changes to this file)         *
*       23-Mar-89 : changes for version 1.5 :                                *
*                   - added procedure refine1()                              *
*                   - changed type of ptn from int* to nvector* in fmptn()   *
*                   - declared level in breakout()                           *
*                   - changed char[] to char* in a few places                *
*                   - minor rearrangement in bestcell()                      *
*       31-Mar-89 : - added procedure doref()                                *
*        5-Apr-89 : - changed MAKEEMPTY uses to EMPTYSET                     *
*       12-Apr-89 : - changed writeperm() and fmperm() to not use MARKing    *
*        5-May-89 : - redefined MASH to gain about 8% efficiency             *
*       18-Oct-90 : changes for version 1.6 :                                *
*                   - improved line breaking in writeperm()                  *
*       10-Nov-90 : - added dummy routine nautil_null()                      *
*       27-Aug-92 : changes for version 1.7 :                                *
*                   - made linelength <= 0 mean no line breaks               *
*        5-Jun-93 : renamed to version 1.7+ (no changes to this file)        *
*       18-Aug-93 : renamed to version 1.8 (no changes to this file)         *
*       17-Sep-93 : renamed to version 1.9 (no changes to this file)         *
*       29-Jun-95 : changes for version 1.10 :                               *
*                   - replaced loop in nextelement() to save reference past  *
*                     end of array (thanks to Kevin Maylsiak)                *
*                                                                            *
*****************************************************************************/

#define  EXTDEFS 1
#include "nauty.h"

    /* macros for hash-codes: */
#define MASH(l,i) ((((l) ^ 065435) + (i)) & 077777)
    /* : expression whose long value depends only on long l and int/long i.
	 Anything goes, preferably non-commutative. */

#define CLEANUP(l) ((int)((l) % INFINITY))
    /* : expression whose value depends on long l and is less than INFINITY
	 when converted to int then short.  Anything goes. */

#if  MAXM==1
#define M 1
#else
#define M m
#endif

static set workset[MAXM];   /* used for scratch work */
static permutation workperm[MAXN];
static short bucket[MAXN+2];

/*****************************************************************************
*                                                                            *
*  nextelement(set1,m,pos) = the position of the first element in set set1   *
*  which occupies a position greater than pos.  If no such element exists,   *
*  the value is -1.  pos can have any value less than n, including negative  *
*  values.                                                                   *
*                                                                            *
*  GLOBALS ACCESSED: none                                                    *
*                                                                            *
*****************************************************************************/

int
nextelement(set1,m,pos)
register set *set1;
int m,pos;
{
	register setword setwd;
	register int w;

#if  MAXM==1
	if (pos < 0)
	    setwd = set1[0];
	else
	    setwd = set1[0] & BITMASK(pos);
	if (setwd == 0)
	    return(-1);
	else
	    return(FIRSTBIT(setwd));
#else
	if (pos < 0)
	{
	    w = 0;
	    setwd = set1[0];
	}
	else
	{
	    w = SETWD(pos);
	    setwd = set1[w] & BITMASK(SETBT(pos));
	}
/*
	do
	{
	    if (setwd != 0)
	        return(TIMESWORDSIZE(w) + FIRSTBIT(setwd));
	    setwd = set1[++w];
	}
	while (w < m);

	return(-1);
*/
	for (;;)
	{
	    if (setwd != 0)
	        return(TIMESWORDSIZE(w) + FIRSTBIT(setwd));
	    if (++w == m) return -1;
	    setwd = set1[w];
	}

#endif
}

/*****************************************************************************
*                                                                            *
*  permset(set1,set2,m,perm)  defines set2 to be the set                     *
*  {perm[i] | i in set1}.                                                    *
*                                                                            *
*  GLOBALS ACCESSED: bit<r>,leftbit<r>                                       *
*                                                                            *
*****************************************************************************/

void
permset(set1,set2,m,perm)
set *set1,*set2;
permutation *perm;
int m;
{
	register setword setw;
	register int pos,w,b;

	EMPTYSET(set2,m);
	setw = set1[0];

#if  MAXM==1
	while (setw  != 0)
	{
	    b = FIRSTBIT(setw);
	    ZAPBIT(setw,b);
	    pos = perm[b];
	    ADDELEMENT(set2,pos);
	}
#else
	w = 0;
	do
	{
	    while (setw != 0)
	    {
	        b = FIRSTBIT(setw);
	        ZAPBIT(setw,b);
	        pos = perm[TIMESWORDSIZE(w) + b];
	        ADDELEMENT(set2,pos);
	    }
	    setw = set1[++w];
	}
	while (w < m);
#endif
}

/*****************************************************************************
*                                                                            *
*  isautom(g,perm,digraph,m,n) = TRUE iff perm is an automorphism of g       *
*  (i.e., g^perm = g).  Symmetry is assumed unless digraph = TRUE.           *
*                                                                            *
*  GLOBALS ACCESSED: bit<r>,nextelement()                                    *
*                                                                            *
*****************************************************************************/

boolean
isautom(g,perm,digraph,m,n)
graph *g;
register permutation *perm;
boolean digraph;
int m,n;
{
	register set *pg;
	register int pos;
	set *pgp;
	int posp,i;

	for (pg = g, i = 0; i < n; pg += M, ++i)
	{
	    pgp = GRAPHROW(g,perm[i],M);
	    pos = (digraph ? -1 : i);

	    while ((pos = nextelement(pg,M,pos)) >= 0)
	    {
	        posp = perm[pos];
	        if (!ISELEMENT(pgp,posp))
	            return(FALSE);
	    }
	}
	return(TRUE);
}

/*****************************************************************************
*                                                                            *
*  putstring(f,s) writes the nul-terminated string s to file f.              *
*                                                                            *
*****************************************************************************/

void
putstring(f,s)
FILE *f;
register char *s;
{
	while (*s != '\0')
	{
	    PUTC(*s,f);
	    ++s;
	}
}

/*****************************************************************************
*                                                                            *
*  itos(i,s) converts the int i to a nul-terminated decimal character        *
*  string s.  The value returned is the number of characters excluding       *
*  the nul.                                                                  *
*                                                                            *
*  GLOBALS ACCESSED: NONE                                                    *
*                                                                            *
*****************************************************************************/

int
itos(i,s)
register int i;
register char *s;
{
	register int digit,j,k;
	register char c;
	int ans;

	if (i < 0)
	{
	    k = 0;
	    i = -i;
	    j = 1;
	    s[0] = '-';
	}
	else
	{
	    k = -1;
	    j = 0;
	}

	do
	{
	    digit = i % 10;
	    i = i / 10;
	    s[++k] = digit + '0';
	}
	while (i);

	s[k+1] = '\0';
	ans = k + 1;

	for (;j < k; ++j, --k)
	{
	    c = s[j];
	    s[j] = s[k];
	    s[k] = c;
	}

	return(ans);
}

/*****************************************************************************
*                                                                            *
*  orbits represents a partition of {0,1,...,n-1}, by orbits[i] = the        *
*  smallest element in the same cell as i.  orbjoin(orbits,autom,n) updates  *
*  the partition orbits to the join of its current value and the cycle       *
*  partition of perm.  The function value returned is the new number of      *
*  cells.                                                                    *
*                                                                            *
*  GLOBALS ACCESSED: NONE                                                    *
*                                                                            *
*****************************************************************************/

int
orbjoin(orbits,perm,n)
register nvector *orbits;
permutation *perm;
int n;
{
	register int i,j1,j2;

	for (i = 0; i < n; ++i)
	{
	    j1 = orbits[i];
	    while (orbits[j1] != j1)
	        j1 = orbits[j1];
	    j2 = orbits[perm[i]];
	    while (orbits[j2] != j2)
	        j2 = orbits[j2];

	    if (j1 < j2)
	        orbits[j2] = j1;
	    else if (j1 > j2)
	        orbits[j1] = j2;
	}

	j1 = 0;
	for (i = 0; i < n; ++i)
	    if ((orbits[i] = orbits[orbits[i]]) == i)
	        ++j1;

	return(j1);
}

/*****************************************************************************
*                                                                            *
*  writeperm(f,perm,cartesian,linelength,n) writes the permutation perm to   *
*  the file f.  The cartesian representation (i.e. perm itself) is used if   *
*  cartesian != FALSE; otherwise the cyclic representation is used.  No      *
*  more than linelength characters (not counting '\n') are written on each   *
*  line, unless linelength is ridiculously small.  linelength<=0 causes no   *
*  line breaks at all to be made.  The global int labelorg is added to each  *
*  vertex number.                                                            *
*                                                                            *
*  GLOBALS ACCESSED: itos(),putstring()                                      *
*                                                                            *
*****************************************************************************/

void
writeperm(f,perm,cartesian,linelength,n)
FILE *f;
permutation *perm;
boolean cartesian;
int linelength,n;
{
	register int i,k,l,curlen,intlen;
	char s[30];

    /* CONDNL(x) writes end-of-line and 3 spaces if x characters
       won't fit on the current line. */
#define CONDNL(x) if (linelength>0 && curlen+(x)>linelength)\
	          {putstring(f,"\n   ");curlen=3;}

	curlen = 0;
	if (cartesian)
	{
	    for (i = 0; i < n; ++i)
	    {
	        intlen = itos(perm[i] + labelorg,s);
	        CONDNL(intlen+1);
	        PUTC(' ',f);
	        putstring(f,s);
	        curlen += intlen + 1;
	    }
	    PUTC('\n',f);
	}
	else
	{
	    for (i = n; --i >= 0;)
	        workperm[i] = 0;

	    for (i = 0; i < n; ++i)
	    {
	        if (workperm[i] == 0 && perm[i] != i)
	        {
	            l = i;
	            intlen = itos(l + labelorg,s);
	            if (curlen > 3)
	                CONDNL(2*intlen+4);
	            PUTC('(',f);
	            do
	            {
	                putstring(f,s);
	                curlen += intlen + 1;
	                k = l;
	                l = perm[l];
	                workperm[k] = 1;
	                if (l != i)
	                {
	                    intlen = itos(l + labelorg,s);
	                    CONDNL(intlen+2);
	                    PUTC(' ',f);
	                }
	            }
	            while (l != i);
	            PUTC(')',f);
	            ++curlen;
	        }
	    }

	    if (curlen == 0)
	        putstring(f,"(1)\n");
	    else
	        PUTC('\n',f);
	}
}

/*****************************************************************************
*                                                                            *
*  testcanlab(g,canong,lab,samerows,m,n) compares g^lab to canong,           *
*  using an ordering which is immaterial since it's only used here.  The     *
*  value returned is -1,0,1 if g^lab <,=,> canong.  *samerows is set to      *
*  the number of rows (0..n) of canong which are the same as those of g^lab. *
*                                                                            *
*  GLOBALS ACCESSED: workset<rw>,permset(),workperm<rw>                      *
*                                                                            *
*****************************************************************************/

int
testcanlab(g,canong,lab,samerows,m,n)
graph *g,*canong;
nvector *lab;
int m,n,*samerows;
{
	register int i,j;
	register set *ph;

	for (i = 0; i < n; ++i)
	    workperm[lab[i]] = i;

	for (i = 0, ph = canong; i < n; ++i, ph += M)
	{
	    permset(GRAPHROW(g,lab[i],M),workset,M,workperm);
	    for (j = 0; j < M; ++j)
	        if (workset[j] < ph[j])
	        {
	            *samerows = i;
	            return(-1);
	        }
	        else if (workset[j] > ph[j])
	        {
	            *samerows = i;
	            return(1);
	        }
	}

	*samerows = n;
	return(0);
}

/*****************************************************************************
*                                                                            *
*  updatecan(g,canong,lab,samerows,m,n) sets canong = g^lab, assuming        *
*  the first samerows of canong are ok already.                              *
*                                                                            *
*  GLOBALS ACCESSED: permset(),workperm<rw>                                  *
*                                                                            *
*****************************************************************************/

void
updatecan(g,canong,lab,samerows,m,n)
graph *g,*canong;
permutation *lab;
int m,n,samerows;
{
	register int i;
	register set *ph;

	for (i = 0; i < n; ++i)
	    workperm[lab[i]] = i;

	for (i = samerows, ph = GRAPHROW(canong,samerows,M);
	                                                   i < n; ++i, ph += M)
	    permset(GRAPHROW(g,lab[i],M),ph,M,workperm);
}

/*****************************************************************************
*                                                                            *
*  fmperm(perm,fix,mcr,m,n) uses perm to construct fix and mcr.  fix         *
*  contains those points are fixed by perm, while mcr contains the set of    *
*  those points which are least in their orbits.                             *
*                                                                            *
*  GLOBALS ACCESSED: bit<r>                                                  *
*                                                                            *
*****************************************************************************/

void
fmperm(perm,fix,mcr,m,n)
register permutation *perm;
set *fix,*mcr;
int m,n;
{
	register int i,k,l;

	EMPTYSET(fix,m);
	EMPTYSET(mcr,m);

	for (i = n; --i >= 0;)
	    workperm[i] = 0;

	for (i = 0; i < n; ++i)
	    if (perm[i] == i)
	    {
	        ADDELEMENT(fix,i);
	        ADDELEMENT(mcr,i);
	    }
	    else if (workperm[i] == 0)
	    {
	        l = i;
	        do
	        {
	            k = l;
	            l = perm[l];
	            workperm[k] = 1;
	        }
	        while (l != i);

	        ADDELEMENT(mcr,i);
	    }
}

/*****************************************************************************
*                                                                            *
*  fmptn(lab,ptn,level,fix,mcr,m,n) uses the partition at the specified      *
*  level in the partition nest (lab,ptn) to make sets fix and mcr.  fix      *
*  represents the points in trivial cells of the partition, while mcr        *
*  represents those points which are least in their cells.                   *
*                                                                            *
*  GLOBALS ACCESSED: bit<r>                                                  *
*                                                                            *
*****************************************************************************/

void
fmptn(lab,ptn,level,fix,mcr,m,n)
nvector *lab,*ptn;
register int level;
set *fix,*mcr;
int m,n;
{
	register int i,lmin;

	EMPTYSET(fix,m);
	EMPTYSET(mcr,m);

	for (i = 0; i < n; ++i)
	    if (ptn[i] <= level)
	    {
	        ADDELEMENT(fix,lab[i]);
	        ADDELEMENT(mcr,lab[i]);
	    }
	    else
	    {
	        lmin = lab[i];
	        do
	            if (lab[++i] < lmin)
	                lmin = lab[i];
	        while (ptn[i] > level);
	        ADDELEMENT(mcr,lmin);
	    }
}

/*****************************************************************************
*                                                                            *
*  refine(g,lab,ptn,level,numcells,count,active,code,m,n) performs a         *
*  refinement operation on the partition at the specified level of the       *
*  partition nest (lab,ptn).  *numcells is assumed to contain the number of  *
*  cells on input, and is updated.  The initial set of active cells (alpha   *
*  in the paper) is specified in the set active.  Precisely, x is in active  *
*  iff the cell starting at index x in lab is active.                        *
*  The resulting partition is equitable if active is correct (see the paper  *
*  and the Guide).                                                           *
*  *code is set to a value which depends on the fine detail of the           *
*  algorithm, but which is independent of the labelling of the graph.        *
*  count is used for work space.                                             *
*                                                                            *
*  GLOBALS ACCESSED:  workset<w>,bit<r>,nextelement(),bucket<w>,workperm<w>  *
*                                                                            *
*****************************************************************************/

UPROC
refine(g,lab,ptn,level,numcells,count,active,code,m,n)
graph *g;
register nvector *lab,*ptn;
permutation *count;
int *numcells,level,m,n,*code;
set *active;
{
	register int i,c1,c2,labc1;
	register setword x;
	register set *set1,*set2;
	int split1,split2,cell1,cell2;
	int cnt,bmin,bmax;
	long longcode;
	set *gptr;

	longcode = *numcells;
	split1 = -1;

	while (*numcells < n && ((split1 = nextelement(active,M,split1)) >= 0
	                     || (split1 = nextelement(active,M,-1)) >= 0))
	{
	    DELELEMENT(active,split1);
	    for (split2 = split1; ptn[split2] > level; ++split2)
	    {}
	    longcode = MASH(longcode,split1 + split2);
	    if (split1 == split2)       /* trivial splitting cell */
	    {
	        gptr = GRAPHROW(g,lab[split1],M);
	        for (cell1 = 0; cell1 < n; cell1 = cell2 + 1)
	        {
	            for (cell2 = cell1; ptn[cell2] > level; ++cell2)
	            {}
	            if (cell1 == cell2)
	                continue;
	            c1 = cell1;
	            c2 = cell2;
	            while (c1 <= c2)
	            {
	                labc1 = lab[c1];
	                if (ISELEMENT(gptr,labc1))
	                    ++c1;
	                else
	                {
	                    lab[c1] = lab[c2];
	                    lab[c2] = labc1;
	                    --c2;
	                }
	            }
	            if (c2 >= cell1 && c1 <= cell2)
	            {
	                ptn[c2] = level;
	                longcode = MASH(longcode,c2);
	                ++*numcells;
	                ADDELEMENT(active,c1);
	            }
	        }
	    }

	    else        /* nontrivial splitting cell */
	    {
	        EMPTYSET(workset,m);
	        for (i = split1; i <= split2; ++i)
	            ADDELEMENT(workset,lab[i]);
	        longcode = MASH(longcode,split2 - split1 + 1);

	        for (cell1 = 0; cell1 < n; cell1 = cell2 + 1)
	        {
	            for (cell2 = cell1; ptn[cell2] > level; ++cell2)
	            {}
	            if (cell1 == cell2)
	                continue;
	            i = cell1;
#if  MAXM==1
	            if (x = workset[0] & g[lab[i]])     /* not == */
	                cnt = POPCOUNT(x);
	            else
	                cnt = 0;
#else
	            set1 = workset;
	            set2 = GRAPHROW(g,lab[i],m);
	            cnt = 0;
	            for (c1 = m; --c1 >= 0;)
	                // CHANGE added extra parens around assignment to eliminate compile warning
	                if ((x = (*set1++) & (*set2++)))  /* not == */
	                    cnt += POPCOUNT(x);
#endif
	            count[i] = bmin = bmax = cnt;
	            bucket[cnt] = 1;
	            while (++i <= cell2)
	            {
#if  MAXM==1
	                if (x = workset[0] & g[lab[i]]) /* not == */
	                    cnt = POPCOUNT(x);
	                else
	                    cnt = 0;
#else
	                set1 = workset;
	                set2 = GRAPHROW(g,lab[i],m);
	                cnt = 0;
	                for (c1 = m; --c1 >= 0;)
	                    // CHANGE added extra parens around assignment to eliminate compile warning
	                    if ((x = (*set1++) & (*set2++)))      /* not == */
	                        cnt += POPCOUNT(x);
#endif
	                while (bmin > cnt)
	                    bucket[--bmin] = 0;
	                while (bmax < cnt)
	                    bucket[++bmax] = 0;
	                ++bucket[cnt];
	                count[i] = cnt;
	            }
	            if (bmin == bmax)
	            {
	                longcode = MASH(longcode,bmin + cell1);
	                continue;
	            }
	            c1 = cell1;
	            for (i = bmin; i <= bmax; ++i)
	                if (bucket[i])
	                {
	                    c2 = c1 + bucket[i];
	                    bucket[i] = c1;
	                    longcode = MASH(longcode,i + c1);
	                    if (c1 != cell1)
	                    {
	                        ADDELEMENT(active,c1);
	                        ++*numcells;
	                    }
	                    if (c2 <= cell2)
	                        ptn[c2-1] = level;
	                    c1 = c2;
	                }
	            for (i = cell1; i <= cell2; ++i)
	                workperm[bucket[count[i]]++] = lab[i];
	            for (i = cell1; i <= cell2; ++i)
	                lab[i] = workperm[i];
	        }
	    }
	}

	longcode = MASH(longcode,*numcells);
	*code = CLEANUP(longcode);
}

/*****************************************************************************
*                                                                            *
*  refine1(g,lab,ptn,level,numcells,count,active,code,m,n) is the same as    *
*  refine(g,lab,ptn,level,numcells,count,active,code,m,n), except that       *
*  m==1 is assumed for greater efficiency.  The results are identical in all *
*  respects.  See refine (above) for the specs.                              *
*                                                                            *
*****************************************************************************/

UPROC
refine1(g,lab,ptn,level,numcells,count,active,code,m,n)
graph *g;
register nvector *lab,*ptn;
permutation *count;
int *numcells,level,m,n,*code;
set *active;
{
	register int i,c1,c2,labc1;
	register setword x;
	int split1,split2,cell1,cell2;
	int cnt,bmin,bmax;
	long longcode;
	set *gptr;

	longcode = *numcells;
	split1 = -1;

	while (*numcells < n && ((split1 = nextelement(active,1,split1)) >= 0
	                     || (split1 = nextelement(active,1,-1)) >= 0))
	{
	    DELELEM1(active,split1);
	    for (split2 = split1; ptn[split2] > level; ++split2)
	    {}
	    longcode = MASH(longcode,split1 + split2);
	    if (split1 == split2)       /* trivial splitting cell */
	    {
	        gptr = GRAPHROW(g,lab[split1],1);
	        for (cell1 = 0; cell1 < n; cell1 = cell2 + 1)
	        {
	            for (cell2 = cell1; ptn[cell2] > level; ++cell2)
	            {}
	            if (cell1 == cell2)
	                continue;
	            c1 = cell1;
	            c2 = cell2;
	            while (c1 <= c2)
	            {
	                labc1 = lab[c1];
	                if (ISELEM1(gptr,labc1))
	                    ++c1;
	                else
	                {
	                    lab[c1] = lab[c2];
	                    lab[c2] = labc1;
	                    --c2;
	                }
	            }
	            if (c2 >= cell1 && c1 <= cell2)
	            {
	                ptn[c2] = level;
	                longcode = MASH(longcode,c2);
	                ++*numcells;
	                ADDELEM1(active,c1);
	            }
	        }
	    }

	    else        /* nontrivial splitting cell */
	    {
	        *workset = 0;
	        for (i = split1; i <= split2; ++i)
	            ADDELEM1(workset,lab[i]);
	        longcode = MASH(longcode,split2 - split1 + 1);

	        for (cell1 = 0; cell1 < n; cell1 = cell2 + 1)
	        {
	            for (cell2 = cell1; ptn[cell2] > level; ++cell2)
	            {}
	            if (cell1 == cell2)
	                continue;
	            i = cell1;
	            // CHANGE added extra parens around assignment to eliminate compile warning
	            if ((x = workset[0] & g[lab[i]]))     /* not == */
	                cnt = POPCOUNT(x);
	            else
	                cnt = 0;
	            count[i] = bmin = bmax = cnt;
	            bucket[cnt] = 1;
	            while (++i <= cell2)
	            {
	                // CHANGE added extra parens around assignment to eliminate compile warning
	                if ((x = workset[0] & g[lab[i]])) /* not == */
	                    cnt = POPCOUNT(x);
	                else
	                    cnt = 0;
	                while (bmin > cnt)
	                    bucket[--bmin] = 0;
	                while (bmax < cnt)
	                    bucket[++bmax] = 0;
	                ++bucket[cnt];
	                count[i] = cnt;
	            }
	            if (bmin == bmax)
	            {
	                longcode = MASH(longcode,bmin + cell1);
	                continue;
	            }
	            c1 = cell1;
	            for (i = bmin; i <= bmax; ++i)
	                if (bucket[i])
	                {
	                    c2 = c1 + bucket[i];
	                    bucket[i] = c1;
	                    longcode = MASH(longcode,i + c1);
	                    if (c1 != cell1)
	                    {
	                        ADDELEM1(active,c1);
	                        ++*numcells;
	                    }
	                    if (c2 <= cell2)
	                        ptn[c2-1] = level;
	                    c1 = c2;
	                }
	            for (i = cell1; i <= cell2; ++i)
	                workperm[bucket[count[i]]++] = lab[i];
	            for (i = cell1; i <= cell2; ++i)
	                lab[i] = workperm[i];
	        }
	    }
	}

	longcode = MASH(longcode,*numcells);
	*code = CLEANUP(longcode);
}

/*****************************************************************************
*                                                                            *
*  doref(g,lab,ptn,level,numcells,qinvar,invar,active,code,refproc,          *
*        invarproc,mininvarlev,maxinvarlev,invararg,digraph,m,n)             *
*  is used to perform a refinement on the partition at the given level in    *
*  (lab,ptn).  The number of cells is *numcells both for input and output.   *
*  The input active is the active set for input to the refinement procedure  *
*  (*refproc)(), which must have the argument list of refine().              *
*  active may be arbitrarily changed.  invar is used for working storage.    *
*  First, (*refproc)() is called.  Then, if invarproc!=NILFUNCTION and       *
*  |mininvarlev| <= level <= |maxinvarlev|, the routine (*invarproc)() is    *
*  used to compute a vertex-invariant which may refine the partition         *
*  further.  If it does, (*refproc)() is called again, using an active set   *
*  containing all but the first fragment of each old cell.  Unless g is a    *
*  digraph, this guarantees that the final partition is equitable.  The      *
*  arguments invararg and digraph are passed to (*invarproc)()               *
*  uninterpretted.  The output argument code is a composite of the codes     *
*  from all the calls to (*refproc)().  The output argument qinvar is set    *
*  to 0 if (*invarproc)() is not applied, 1 if it is applied but fails to    *
*  refine the partition, and 2 if it succeeds.                               *
*  See the file nautinv.c for a further discussion of vertex-invariants.     *
*  Note that the dreadnaut I command generates a call to  this procedure     *
*  with level = mininvarlevel = maxinvarlevel = 0.                           *
*                                                                            *
*****************************************************************************/

void
doref(g,lab,ptn,level,numcells,qinvar,invar,active,code,refproc,invarproc,
      mininvarlev,maxinvarlev,invararg,digraph,m,n)
graph *g;
nvector *lab,*ptn;
int level,*numcells,*qinvar,*code,mininvarlev,maxinvarlev,invararg,m,n;
boolean digraph;
permutation *invar;
set *active;
UPROC (*refproc)(),(*invarproc)();
{
	register int j,h;
	register permutation pw;
	nvector iw;
	int i,cell1,cell2,nc,tvpos,minlev,maxlev;
	long longcode;
	boolean same;

	if ((tvpos = nextelement(active,M,-1)) < 0)
	    tvpos = 0;

	(*refproc)(g,lab,ptn,level,numcells,invar,active,code,M,n);

	minlev = (mininvarlev < 0 ? -mininvarlev : mininvarlev);
	maxlev = (maxinvarlev < 0 ? -maxinvarlev : maxinvarlev);
	if (invarproc != NILFUNCTION && *numcells < n
	                    && level >= minlev && level <= maxlev)
	{
	    (*invarproc)(g,lab,ptn,level,*numcells,tvpos,invar,invararg,
	                                                         digraph,M,n);
	    EMPTYSET(active,m);
	    for (i = n; --i >= 0;)
	        workperm[i] = invar[lab[i]];
	    nc = *numcells;
	    for (cell1 = 0; cell1 < n; cell1 = cell2 + 1)
	    {
	        pw = workperm[cell1];
	        same = TRUE;
	        for (cell2 = cell1; ptn[cell2] > level; ++cell2)
	            if (workperm[cell2+1] != pw)
	                same = FALSE;
	        if (same)
	            continue;

	        j = (cell2 - cell1 + 1) / 3;
	        h = 1;
	        do
	            h = 3 * h + 1;
	        while (h < j);

	        do                      /* shell sort */
	        {
	            for (i = cell1 + h; i <= cell2; ++i)
	            {
	                iw = lab[i];
	                pw = workperm[i];
	                for (j = i; workperm[j-h] > pw; )
	                {
	                    workperm[j] = workperm[j-h];
	                    lab[j] = lab[j-h];
	                    if ((j -= h) < cell1 + h)
	                        break;
	                }
	                workperm[j] = pw;
	                lab[j] = iw;
	            }
	            h /= 3;
	        }
	        while (h > 0);

	        for (i = cell1 + 1; i <= cell2; ++i)
	            if (workperm[i] != workperm[i-1])
	            {
	                ptn[i-1] = level;
	                ++*numcells;
	                ADDELEMENT(active,i);
	            }
	    }

	    if (*numcells > nc)
	    {
	        *qinvar = 2;
	        longcode = *code;
	        (*refproc)(g,lab,ptn,level,numcells,invar,active,code,M,n);
	        longcode = MASH(longcode,*code);
	        *code = CLEANUP(longcode);
	    }
	    else
	        *qinvar = 1;
	}
	else
	    *qinvar = 0;
}

/*****************************************************************************
*                                                                            *
*  cheapautom(ptn,level,digraph,n) returns TRUE if the partition at the      *
*  specified level in the partition nest (lab,ptn) {lab is not needed here}  *
*  satisfies a simple sufficient condition for its cells to be the orbits of *
*  some subgroup of the automorphism group.  Otherwise it returns FALSE.     *
*  It always returns FALSE if digraph!=FALSE.                                *
*                                                                            *
*  nauty assumes that this function will always return TRUE for any          *
*  partition finer than one for which it returns TRUE.                       *
*                                                                            *
*****************************************************************************/

boolean
cheapautom(ptn,level,digraph,n)
register nvector *ptn;
register int level;
boolean digraph;
int n;
{
	register int i,k,nnt;

	if (digraph)
	    return(FALSE);

	k = n;
	nnt = 0;
	for (i = 0; i < n; ++i)
	{
	    --k;
	    if (ptn[i] > level)
	    {
	        ++nnt;
	        while (ptn[++i] > level)
	        {}
	    }
	}

	return(k <= nnt + 1 || k <= 4);
}

/*****************************************************************************
*                                                                            *
*  targetcell(g,lab,ptn,level,numcells,tcell,tcellsize,&cellpos,tc_level,    *
*             hint,m,n)                                                      *
*  examines the partition at the specified level in the partition nest       *
*  (lab,ptn) and finds a non-trival cell (if none, the first cell).          *
*  If hint >= 0 and there is a non-trivial cell starting at position hint    *
*      in lab, that cell is chosen.                                          *
*  Else, If level <= tc_level, bestcell is called to choose a cell.          *
*        Else, the first non-trivial cell is chosen.                         *
*  When a cell is chosen, tcell is set to its contents, *tcellsize to its    *
*  size, and cellpos to its starting position in lab.                        *
*                                                                            *
*  GLOBALS ACCESSED: bit<r>,bestcell()                                       *
*                                                                            *
*****************************************************************************/

UPROC
targetcell(g,lab,ptn,level,numcells,tcell,tcellsize,cellpos,tc_level,hint,m,n)
graph *g;
register nvector *lab,*ptn;
set *tcell;
register int level;
int numcells,*tcellsize,*cellpos,tc_level,hint,m,n;
{
	register int i,j,k;

	if (hint >= 0 && ptn[hint] > level &&
	                 (hint == 0 || ptn[hint-1] <= level))
	    i = hint;
	else if (level <= tc_level)
	    i = bestcell(g,lab,ptn,level,tc_level,m,n);
	else
	    for (i = 0; i < n && ptn[i] <= level; ++i)
	    {}
	if (i == n)
	    i = j = 0;
	else
	    for (j = i + 1; ptn[j] > level; ++j)
	    {}

	*tcellsize = j - i + 1;

	EMPTYSET(tcell,m);
	for (k = i; k <= j; ++k)
	    ADDELEMENT(tcell,lab[k]);

	*cellpos = i;
}

/*****************************************************************************
*                                                                            *
*  bestcell(g,lab,ptn,level,tc_level,m,n) returns the index in lab of the    *
*  start of the "best non-singleton cell" for fixing.  If there is no        *
*  non-singleton cell it returns n.                                          *
*  This implementation finds the first cell which is non-trivially joined    *
*  to the greatest number of other cells.                                    *
*                                                                            *
*  GLOBALS ACCESSED: bit<r>,workperm<rw>,workset<rw>,bucket<rw>              *
*                                                                            *
*****************************************************************************/

int
bestcell(g,lab,ptn,level,tc_level,m,n)
graph *g;
nvector *lab,*ptn;
int level,tc_level,m,n;
{
	register int i;
	set *gp;
	register setword setword1,setword2;
	int v1,v2,nnt;

   /* find non-singleton cells: put starts in workperm[0..nnt-1] */

	i = nnt = 0;

	while (i < n)
	{
	    if (ptn[i] > level)
	    {
	        workperm[nnt++] = i;
	        while (ptn[i] > level)
	            ++i;
	    }
	    ++i;
	}

	if (nnt == 0)
	    return(n);

	/* set bucket[i] to # non-trivial neighbours of n.s. cell i */

	for (i = nnt; --i >= 0;)
	    bucket[i] = 0;

	for (v2 = 1; v2 < nnt; ++v2)
	{
	    EMPTYSET(workset,m);
	    i = workperm[v2] - 1;
	    do
	    {
	        ++i;
	        ADDELEMENT(workset,lab[i]);
	    }
	    while (ptn[i] > level);
	    for (v1 = 0; v1 < v2; ++v1)
	    {
	        gp = GRAPHROW(g,lab[workperm[v1]],m);
#if  MAXM==1
	        setword1 = *workset & *gp;
	        setword2 = *workset & ~*gp;
#else
	        setword1 = setword2 = 0;
	        for (i = m; --i >= 0;)
	        {
	            setword1 |= workset[i] & gp[i];
	            setword2 |= workset[i] & ~gp[i];
	        }
#endif
	        if (setword1 != 0 && setword2 != 0)
	        {
	            ++bucket[v1];
	            ++bucket[v2];
	        }
	    }
	}

	/* find first greatest bucket value */

	v1 = 0;
	v2 = bucket[0];
	for (i = 1; i < nnt; ++i)
	    if (bucket[i] > v2)
	    {
	        v1 = i;
	        v2 = bucket[i];
	    }

	return((int)workperm[v1]);
}

/*****************************************************************************
*                                                                            *
*  shortprune(set1,set2,m) ANDs the contents of set set2 into set set1.      *
*                                                                            *
*  GLOBALS ACCESSED: NONE                                                    *
*                                                                            *
*****************************************************************************/

void
shortprune(set1,set2,m)
register set *set1,*set2;
register int m;
{
	register int i;

	for (i = 0; i < M; ++i)
	    INTERSECT(set1[i],set2[i]);
}

/*****************************************************************************
*                                                                            *
*  breakout(lab,ptn,level,tc,tv,active,m) operates on the partition at       *
*  the specified level in the partition nest (lab,ptn).  It finds the        *
*  element tv, which is in the cell C starting at index tc in lab (it had    *
*  better be) and splits C in the two cells {tv} and C\{tv}, in that order.  *
*  It also sets the set active to contain just the element tc.               *
*                                                                            *
*  GLOBALS ACCESSED: bit<r>                                                  *
*                                                                            *
*****************************************************************************/

void
breakout(lab,ptn,level,tc,tv,active,m)
nvector *lab,*ptn;
set *active;
int level,tc,tv,m;
{
	register int i,prev,next;

	EMPTYSET(active,m);
	ADDELEMENT(active,tc);

	i = tc;
	prev = tv;

	do
	{
	    next = lab[i];
	    lab[i++] = prev;
	    prev = next;
	}
	while (prev != tv);

	ptn[tc] = level;
}

/*****************************************************************************
*                                                                            *
*  longprune(tcell,fix,bottom,top,m) removes zero or elements of the set     *
*  tcell.  It is assumed that addresses bottom through top-1 contain         *
*  contiguous pairs of sets (f1,m1),(f2,m2), ... .  tcell is intersected     *
*  with each mi such that fi is a subset of fix.                             *
*                                                                            *
*  GLOBALS ACCESSED: NONE                                                    *
*                                                                            *
*****************************************************************************/

void
longprune(tcell,fix,bottom,top,m)
register set *tcell,*fix,*bottom;
set *top;
int m;
{
	register int i;

	while (bottom < top)
	{
	    for (i = 0; i < M; ++i)
	        if (NOTSUBSET(fix[i],bottom[i]))
	            break;
	    bottom += M;

	    if (i == M)
	        for (i = 0; i < M; ++i)
	            INTERSECT(tcell[i],bottom[i]);
	    bottom += M;
	}
}

/*****************************************************************************
*                                                                            *
*  nautil_null() does nothing.  See dreadnaut.c for its purpose.             *
*                                                                            *
*****************************************************************************/

void
nautil_null()
{
}
