/* makeg.c : Find all graphs with a given number of vertices,
             a given range of numbers of edges, and a given bound
             on the maximum degree.

             The output is to stdout in either y format or nauty format.
             The output graphs are optionally canonically labelled.

 Usage: makeg [-c -t -b -n -u -v -l] [-d<max>] n [mine [maxe [mod res]]]

             n    = the number of vertices (1..16)
             mine = the minimum number of edges (no bounds if missing)
             maxe = the maximum number of edges (same as mine if missing)
             mod, res = a way to restrict the output to a subset.
                        All the graphs in G(n,mine..maxe) are divided into
                        disjoint classes C(mod,0),C(mod,1),...,C(mod,mod-1),
                        of very approximately equal size.
                        Only the class C(mod,res) is written.
                        The usual relationships between modulo classes are
                        obeyed; for example C(4,3) = C(8,3) union C(8,7).
             -c    : only write connected graphs
             -t    : only generate triangle-free graphs
             -b    : only generate bipartite graphs
             -d<x> : specify an upper bound for the maximum degree.
                     The value of the upper bound must be adjacent to
                     the "d".  Example: -d6
             -n    : use nauty format instead of y format for output
             -u    : do not output any graphs, just generate and count them
             -v    : display counts by number of edges to stderr
             -l    : canonically label output graphs

Output formats.

  If -n is absent, any output graphs are written in y format.

    Each graph occupies one line with a terminating newline.
    Except for the newline, each byte has the format  01xxxxxx, where
    each "x" represents one bit of data.
    First byte:  xxxxxx is the number of vertices n
    Other ceiling(n(n-1)/12) bytes:  These contain the upper triangle of
    the adjacency matrix in column major order.  That is, the entries
    appear in the order (0,1),(0,2),(1,2),(0,3),(1,3),(2,3),(0,4),... .
    The bits are used in left to right order within each byte.
    Any unused bits on the end are set to zero.

  If -n is present, any output graphs are written in nauty format.

    For a graph of n vertices, the output consists of n+1 long ints
    (even if a setword is shorter than a long int). The first contains
    n, and the others contain the adjacency matrix.  Long int i of
    the adjacency matrix (0 <= i <= n-1) is the set of neighbours of
    vertex i, cast from setword to long int.

OUTPROC feature.

   By defining the C preprocessor variable OUTPROC at compile time
   (for Unix the syntax is  -DOUTPROC=procname  on the cc command),
   makeg can be made to call a procedure of your manufacture with each
   output graph instead of writing anything.  Your procedure needs to
   have type void and the argument list  (FILE *f, graph *g, int n).
   f is a stream open for writing (in fact, in the current version it
   is always stdout), g is the graph in nauty format, and n is the number
   of vertices.  Your procedure can be in a separate file so long as it
   is linked with makeg.  The global variables nooutput, nautyformat
   and canonise (all type boolean) can be used to test for the presence
   of the flags -u, -n and -l, respectively.

INSTRUMENT feature.

    If the C preprocessor variable INSTRUMENT is defined at compile time,
    extra code is inserted to collect statistics during execution, and
    more information is written to stderr at termination.

**************************************************************************

Sample performance statistics.

    Here we give some graph counts and execution times on a Sun
    Sparcstation ELC (nominally 24 mips).  makeg version of 11/3/92.


   General Graphs (to n(n-1)/4 edges)      makeg -u <n> 0 <m>
      1           1                         m = floor(n(n-1)/4)
      2           1
      3           2
      4           7
      5          20
      6          78   0.02 sec
      7         522   0.11 sec
      8        6996   1.22 sec
      9      154354   22.4 sec
     10     6002584   13.6 min
     11   509498932   18.9 hr
     12 90548672803   3293 hr (version of 5/6/93)

   Triangle-Free Graphs                    makeg -u -t <n>
      1          1
      2          2
      3          3
      4          7
      5         14
      6         38
      7        107   0.06 sec
      8        410   0.28 sec
      9       1897   1.32 sec
     10      12172   9.3 sec
     11     105071   80.5 sec
     12    1262180   16.1 min
     13   20797002   4.80 hr
     14  467871369   120 hr (est)

   Bipartite Graphs                        makeg -u -b <n>
      1          1
      2          2
      3          3
      4          7
      5         13
      6         35
      7         88   0.05 sec
      8        303   0.24 sec
      9       1119   1.0 sec
     10       5479   6.0 sec
     11      32303   40.7 sec
     12     251135   356 sec
     13    2527712   74 min
     14   33985853   20.1 hr
     15  611846940   430 hr (est)

**************************************************************************

    Author:   B. D. McKay, Sep 1991.
              Copyright  B. McKay (1991, 1992).  All rights reserved.
              This software is subject to the conditions and waivers
              detailed in the file nauty.h.

    Changes:  Nov 18, 1991 : added -d switch
                             fixed operation for n=16
              Nov 26, 1991 : added OUTPROC feature
              Nov 29, 1991 : -c implies mine >= n-1
              Jan  8, 1992 : make writeny() not static
              Jan 10, 1992 : added -n switch
              Feb  9, 1992 : fixed case of n=1
              Feb 16, 1992 : changed mine,maxe,maxdeg testing
              Feb 19, 1992 : added -b, -t and -u options
                             documented OUTPROC and added external
                                 declaration for it.
              Feb 20, 1992 : added -v option
              Feb 22, 1992 : added INSTRUMENT compile-time option
              Feb 23, 1992 : added xbnds() for more effective pruning
              Feb 24, 1992 : added -l option
              Feb 25, 1992 : changed writenauty() to use fwrite()
              Mar 11, 1992 : completely revised many parts, incl
                             new refinement procedure for fast rejection,
                             distance invariant for regular graphs
              May 19, 1992 : modified userautomproc slightly.  xorb[]
                             is no longer idempotent but it doesn't matter.
                             Speed-up of 2-5% achieved.
              June 5, 1993 : removed ";" after "CPUDEFS" to avoid illegal
                             empty declaration.
         November 24, 1994 : tested for 0 <= res < mod


**************************************************************************/

#define MAXN 16         /* not more than 16 */
#include "naututil.h"   /* which includes nauty.h and stdio.h */

CPUDEFS

static void (*outproc)();
#ifdef OUTPROC
extern void OUTPROC();
#endif

static FILE *outfile;           /* file for output graphs */
static FILE *msgfile;           /* file for messages */
static boolean connec;          /* presence of -c */
static boolean bipartite;       /* presence of -b */
static boolean trianglefree;    /* presence of -b or -t */
static boolean verbose;         /* presence of -v */
boolean nautyformat;            /* presence of -n */
boolean nooutput;               /* presence of -u */
boolean canonise;               /* presence of -l */
static int maxdeg,maxn,mine,maxe,nprune,mod,res,curres;
// CHANGE start
int g_maxn, g_mine, g_maxe, g_mod, g_res;
char g_command;
FILE *g_msgfile;
// CHANGE end
static graph gcan[MAXN];

static int xbit[] = {0x0001,0x0002,0x0004,0x0008,
                     0x0010,0x0020,0x0040,0x0080,
                     0x0100,0x0200,0x0400,0x0800,
                     0x1000,0x2000,0x4000,0x8000};

#define XFIRSTBIT(x) \
    ((x)&0xFF ? 7-leftbit[(x)&0xFF] : 15-leftbit[((x)>>8)&0xFF])
#define XPOPCOUNT(x) (bytecount[((x)>>8)&0xFF] + bytecount[(x)&0xFF])

typedef struct
{
    int ne,dmax;         /* values used for xlb,xub calculation */
    int xlb,xub;         /* saved bounds on extension degree */
    int lo,hi;           /* work purposes for orbit calculation */
    int xstart[MAXN+1];  /* index into xset[] for each cardinality */
    int *xset;           /* array of all x-sets in card order */
    int *xcard;          /* cardinalities of all x-sets */
    int *xinv;           /* map from x-set to index in xset */
    int *xorb;           /* min orbit representative */
    int *xx;             /* (-b or -t) all but largest legal x-set */
} leveldata;

static leveldata data[MAXN];      /* data[n] is data for n -> n+1 */
static long count[1+MAXN*(MAXN-1)/2];  /* counts by number of edges */

#ifdef INSTRUMENT
static long nodes[MAXN],rigidnodes[MAXN],fertilenodes[MAXN];
static long a1calls,a1nauty,a1succs;
static long a2calls,a2nauty,a2uniq,a2succs;
#endif

/************************************************************************/

void
writeny(f,g,n)       /* write graph g (n vertices) to file f in y format */
FILE *f;
graph *g;
int n;
{
        static char ybit[] = {32,16,8,4,2,1};
        char s[(MAXN*(MAXN-1)/2 + 5)/6 + 4];
        register int i,j,k;
        register char y,*sp;

        sp = s;
        *(sp++) = 0x40 | n;
        y = 0x40;

        k = -1;
        for (j = 1; j < n; ++j)
        for (i = 0; i < j; ++i)
        {
            if (++k == 6)
            {
                *(sp++) = y;
                y = 0x40;
                k = 0;
            }
            if (g[i] & bit[j]) y |= ybit[k];
        }
        if (n >= 2) *(sp++) = y;
        *(sp++) = '\n';
        *sp = '\0';

        if (fputs(s,f) == EOF || ferror(f))
        {
            fprintf(stderr,">E writeny : error on writing file\n");
            exit(2);
        }
}

/***********************************************************************/
// CHANGE start (comment out this function; changed comment style within)
/*
static void
nullwrite(f,g,n)  // don't write graph g (n vertices) to file f
FILE *f;
graph *g;
int n;
{
}
*/
// CHANGE end
/***********************************************************************/

void
writenauty(f,g,n)  /* write graph g (n vertices) to file f in nauty format */
FILE *f;
graph *g;
int n;
{
        long buffer[MAXN+1];
        register int i;

        buffer[0] = n;
        for (i = 0; i < n; ++i)
            buffer[i+1] = g[i];

        if (fwrite((char*)buffer,sizeof(long),n+1,f) != n+1)
        {
            fprintf(stderr,">E writenauty : error on writing file\n");
            exit(2);
        }
}

/*********************************************************************/

static boolean
isconnected(g,n)             /* test if g is connected */
graph *g;
int n;
{
        register setword seen,expanded,toexpand;
        register int i;

        seen = bit[0];
        expanded = 0;

        // CHANGE added extra parens around assignment to eliminate compile warning
        while ((toexpand = (seen & ~expanded)))             /* not == */
        {
            i = FIRSTBIT(toexpand);
            expanded |= bit[i];
            seen |= g[i];
        }

        return  POPCOUNT(seen) == n;
}

/**************************************************************************/

static boolean
distinvar(g,invar,n)    /* make distance invariant */
graph *g;               /* exit immediately FALSE if n-1 not maximal */
int *invar,n;           /* else exit TRUE */
{
        register int w;
        register setword workset,frontier;
        setword sofar;
        int inv,d,v;

        for (v = n-1; v >= 0; --v)
        {
            inv = 0;
            sofar = frontier = bit[v];
            for (d = 1; frontier != 0; ++d)
            {
                workset = 0;
                inv += POPCOUNT(frontier) ^ (0x57 + d);
                while (frontier)
                {
                    w = FIRSTBIT(frontier);
                    frontier &= ~bit[w];
                    workset |= g[w];
                }
                frontier = workset & ~sofar;
                sofar |= frontier;
            }
            invar[v] = inv;
            if (v < n-1 && inv > invar[n-1]) return FALSE;
        }
        return TRUE;
}

/**************************************************************************/

static void
makexgraph(g,h,n)       /* make x-format graph from nauty format graph */
graph *g;
int *h,n;
{
        register setword gi;
        register int i,j,hi;

        for (i = 0; i < n; ++i)
        {
            hi = 0;
            gi = g[i];
            while (gi)
            {
                j = FIRSTBIT(gi);
                gi &= ~bit[j];
                hi |= xbit[j];
            }
            h[i] = hi;
        }
}

/**************************************************************************/

static void
makebgraph(g,h,n)       /* make x-format graph of different colour graph */
graph *g;
int *h,n;
{
        register setword seen1,seen2,expanded,w;
        setword restv;
        register int i,xseen1,xseen2;

        restv = 0;
        for (i = 0; i < n; ++i)
            restv |= bit[i];

        seen1 = seen2 = 0;
        expanded = 0;

        while (TRUE)
        {
            if ((w = ((seen1 | seen2) & ~expanded)) == 0)
            {
                xseen1 = 0;
                w = seen1;
                while (w)
                {
                    i = FIRSTBIT(w);
                    w &= ~bit[i];
                    xseen1 |= xbit[i];
                }
                xseen2 = 0;
                w = seen2;
                while (w)
                {
                    i = FIRSTBIT(w);
                    w &= ~bit[i];
                    xseen2 |= xbit[i];
                }

                w = seen1;
                while (w)
                {
                    i = FIRSTBIT(w);
                    w &= ~bit[i];
                    h[i] = xseen2;
                }
                w = seen2;
                while (w)
                {
                    i = FIRSTBIT(w);
                    w &= ~bit[i];
                    h[i] = xseen1;
                }

                restv &= ~(seen1 | seen2);
                if (restv == 0) return;
                i = FIRSTBIT(restv);
                seen1 = bit[i];
                seen2 = 0;
            }
            else
                i = FIRSTBIT(w);

            expanded |= bit[i];
            if (bit[i] & seen1) seen2 |= g[i];
            else                seen1 |= g[i];
        }
}

/**************************************************************************/

static void
makeleveldata()      /* make the level data for each level */
{
        register int i,j,h;
        int n,nn,nxsets,tttn;
        long ncj;
        leveldata *d;
        int *xset,*xcard,*xinv;
        int xw,cw;
//        extern char *malloc();

        for (n = 1; n < maxn; ++n)
        {
            nn = maxdeg <= n ? maxdeg : n;
            ncj = nxsets = 1;
            for (j = 1; j <= nn; ++j)
            {
                ncj = (ncj * (n - j + 1)) / j;
                nxsets += ncj;
            }
            tttn = 1 << n;

            d = &data[n];

            d->xset = xset = (int*) malloc(nxsets * sizeof(int));
            d->xcard = xcard = (int*) malloc(nxsets * sizeof(int));
            d->xinv = xinv = (int*) malloc(tttn * sizeof(int));
            d->xorb = (int*) malloc(nxsets * sizeof(int));
            d->xx = d->xcard;

            if (xset==NULL || xcard==NULL || xinv==NULL || d->xorb==NULL)
            {
                fprintf(stderr,">E makeg: malloc failed in makeleveldata()\n");
                exit(2);
            }

            d->ne = d->dmax = d->xlb = d->xub = -1;

            j = 0;
            for (i = 0; i < tttn; ++i)
                if ((h = XPOPCOUNT(i)) <= maxdeg)
                {
                    xset[j] = i;
                    xcard[j] = h;
                    ++j;
                }

            if (j != nxsets)
            {
                fprintf(stderr,">E makeg: j=%d mxsets=%d\n",j,nxsets);
                exit(2);
            }

            h = 1;
            do
                h = 3 * h + 1;
            while (h < nxsets);

            do
            {
                for (i = h; i < nxsets; ++i)
                {
                    xw = xset[i];
                    cw = xcard[i];
                    for (j = i; xcard[j-h] > cw ||
                                 // CHANGE added parens around last && to eliminate compile warning
                                (xcard[j-h] == cw && xset[j-h] > xw); )
                    {
                        xset[j] = xset[j-h];
                        xcard[j] = xcard[j-h];
                        if ((j -= h) < h) break;
                    }
                    xset[j] = xw;
                    xcard[j] = cw;
                }
                h /= 3;
            }
            while (h > 0);

            for (i = 0; i < nxsets; ++i)
                xinv[xset[i]] = i;

            d->xstart[0] = 0;
            for (i = 1; i < nxsets; ++i)
                if (xcard[i] > xcard[i-1]) d->xstart[xcard[i]] = i;
            d->xstart[xcard[nxsets-1]+1] = nxsets;
        }
}

/**************************************************************************/

static UPROC
userautomproc(count,p,orbits,numorbits,stabvertex,n)
int count,numorbits,stabvertex,n;   /* form orbits on powerset of VG */
permutation *p;                     /* called by nauty */
nvector *orbits;                    /* operates on data[n] */
{
        register int i,j1,j2;
        register int moved,pxi,pi;
        int w,lo,hi;
        int *xorb,*xset,*xinv;

        xorb = data[n].xorb;
        xset = data[n].xset;
        xinv = data[n].xinv;
        lo = data[n].lo;
        hi = data[n].hi;

        if (count == 1)                         /* first automorphism */
            for (i = lo; i < hi; ++i)
                xorb[i] = i;

        moved = 0;
        for (i = 0; i < n; ++i)
            if (p[i] != i) moved |= xbit[i];

        for (i = lo; i < hi; ++i)
        {
            if ((w = xset[i] & moved) == 0) continue;
            pxi = xset[i] & ~moved;
            while (w)
            {
                j1 = XFIRSTBIT(w);
                w &= ~xbit[j1];
                pxi |= xbit[p[j1]];
            }
            pi = xinv[pxi];

            j1 = xorb[i];
            while (xorb[j1] != j1)
                j1 = xorb[j1];
            j2 = xorb[pi];
            while (xorb[j2] != j2)
                j2 = xorb[j2];

            if      (j1 < j2) xorb[j2] = xorb[i] = xorb[pi] = j1;
            else if (j1 > j2) xorb[j1] = xorb[i] = xorb[pi] = j2;
        }
}

/*****************************************************************************
*                                                                            *
*  refinex(g,lab,ptn,level,numcells,count,active,goodret,code,m,n) is a      *
*  custom version of refine() which can exit quickly if required.            *
*                                                                            *
*  Only use at level==0.                                                     *
*  goodret : whether to do an early return for code 1                        *
*  code := -1 for n-1 not max, 0 for maybe, 1 for definite                   *
*                                                                            *
*****************************************************************************/

static void
refinex(g,lab,ptn,level,numcells,count,active,goodret,code,m,n)
graph *g;
register nvector *lab,*ptn;
permutation *count;
int *numcells,level,m,n,*code;
set *active;
boolean goodret;
{
        register int i,c1,c2,labc1;
        register setword x;
        int split1,split2,cell1,cell2;
        int cnt,bmin,bmax;
        set *gptr;
        setword workset;
        int workperm[MAXN];
        int bucket[MAXN+2];

        if (n == 1)
        {
            *code = 1;
            return;
        }

        *code = 0;
        split1 = -1;
        while (*numcells < n && ((split1 = nextelement(active,1,split1)) >= 0
                             || (split1 = nextelement(active,1,-1)) >= 0))
        {
            DELELEM1(active,split1);
            for (split2 = split1; ptn[split2] > 0; ++split2)
            {}
            if (split1 == split2)       /* trivial splitting cell */
            {
                gptr = GRAPHROW(g,lab[split1],1);
                for (cell1 = 0; cell1 < n; cell1 = cell2 + 1)
                {
                    for (cell2 = cell1; ptn[cell2] > 0; ++cell2)
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
                        ptn[c2] = 0;
                        ++*numcells;
                        ADDELEM1(active,c1);
                    }
                }
            }

            else        /* nontrivial splitting cell */
            {
                workset = 0;
                for (i = split1; i <= split2; ++i)
                    workset |= bit[lab[i]];

                for (cell1 = 0; cell1 < n; cell1 = cell2 + 1)
                {
                    for (cell2 = cell1; ptn[cell2] > 0; ++cell2)
                    {}
                    if (cell1 == cell2)
                        continue;
                    i = cell1;
                    // CHANGE added extra parens around assignment to eliminate compile warning
                    if ((x = workset & g[lab[i]]))     /* not == */
                        cnt = POPCOUNT(x);
                    else
                        cnt = 0;
                    count[i] = bmin = bmax = cnt;
                    bucket[cnt] = 1;
                    while (++i <= cell2)
                    {
                        // CHANGE added extra parens around assignment to eliminate compile warning
                        if ((x = workset & g[lab[i]])) /* not == */
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
                        continue;
                    }
                    c1 = cell1;
                    for (i = bmin; i <= bmax; ++i)
                        if (bucket[i])
                        {
                            c2 = c1 + bucket[i];
                            bucket[i] = c1;
                            if (c1 != cell1)
                            {
                                ADDELEM1(active,c1);
                                ++*numcells;
                            }
                            if (c2 <= cell2)
                                ptn[c2-1] = 0;
                            c1 = c2;
                        }
                    for (i = cell1; i <= cell2; ++i)
                        workperm[bucket[count[i]]++] = lab[i];
                    for (i = cell1; i <= cell2; ++i)
                        lab[i] = workperm[i];
                }
            }

            if (ptn[n-2] == 0)
            {
                if (lab[n-1] == n-1)
                {
                    *code = 1;
                    if (goodret) return;
                }
                else
                {
                    *code = -1;
                    return;
                }
            }
            else
            {
                i = n - 1;
                while (1)
                {
                    if (lab[i] == n-1) break;
                    --i;
                    if (ptn[i] == 0)
                    {
                        *code = -1;
                        return;
                    }
                }
            }
        }
}

/**************************************************************************/

static void
makecanon(g,gcan,n)
graph *g,*gcan;                 /* gcan := canonise(g) */
int n;
{
        nvector lab[MAXN],ptn[MAXN],orbits[MAXN];
        statsblk stats;
        static DEFAULTOPTIONS(options);
        setword workspace[50];

        options.writemarkers = FALSE;
        options.writeautoms = FALSE;
        options.getcanon = TRUE;

        nauty(g,lab,ptn,NILSET,orbits,&options,&stats,workspace,50,1,n,gcan);
}

/**************************************************************************/

static boolean
accept1(g,n,x,gx,deg,rigid)     /* decide if n in theta(g+x) */
graph *g,*gx;                   /* version for n+1 < maxn */
int n,*deg;
int x;
boolean *rigid;
{
        register int i;
        nvector lab[MAXN],ptn[MAXN],orbits[MAXN];
        permutation count[MAXN];
        graph h[MAXN];
        int xw;
        int nx,numcells,code;
        int i0,i1,degn;
        set active[MAXM];
        statsblk stats;
        static DEFAULTOPTIONS(options);
        setword workspace[50];

#ifdef INSTRUMENT
        ++a1calls;
#endif

        nx = n + 1;
        for (i = 0; i < n; ++i)
            gx[i] = g[i];
        gx[n] = 0;
        deg[n] = degn = XPOPCOUNT(x);

        xw = x;
        while (xw)
        {
            i = XFIRSTBIT(xw);
            xw &= ~xbit[i];
            gx[i] |= bit[n];
            gx[n] |= bit[i];
            ++deg[i];
        }

        i0 = 0;
        i1 = n;
        for (i = 0; i < nx; ++i)
        {
            if (deg[i] == degn) lab[i1--] = i;
            else                lab[i0++] = i;
            ptn[i] = 1;
        }
        ptn[n] = 0;
        if (i0 == 0)
        {
            numcells = 1;
            active[0] = bit[0];
        }
        else
        {
            numcells = 2;
            active[0] = bit[0] | bit[i1+1];
            ptn[i1] = 0;
        }
        refinex(gx,lab,ptn,0,&numcells,count,active,FALSE,&code,1,nx);

        if (code < 0) return FALSE;

        if (numcells == nx)
        {
            *rigid = TRUE;
#ifdef INSTRUMENT
            ++a1succs;
#endif
            return TRUE;
        }

        options.writemarkers = FALSE;
        options.writeautoms = FALSE;
        options.getcanon = TRUE;
        options.defaultptn = FALSE;
        options.userautomproc = userautomproc;

        active[0] = 0;
#ifdef INSTRUMENT
        ++a1nauty;
#endif
        nauty(gx,lab,ptn,active,orbits,&options,&stats,workspace,50,1,nx,h);

        if (orbits[lab[n]] == orbits[n])
        {
            *rigid = stats.numorbits == nx;
#ifdef INSTRUMENT
            ++a1succs;
#endif
            return TRUE;
        }
        else
            return FALSE;
}

/**************************************************************************/

static boolean
accept2(g,n,x,gx,deg,nuniq)   /* decide if n in theta(g+x) */
graph *g,*gx;                 /* version for n+1 == maxn */
int n,x,deg[];
boolean nuniq;
{
        register int i;
        nvector lab[MAXN],ptn[MAXN],orbits[MAXN];
        int degx[MAXN],invar[MAXN];
        setword vmax,gv;
        int qn,qv;
        permutation count[MAXN];
        int xw;
        int nx,numcells,code;
        int degn,i0,i1,j,j0,j1;
        set active[MAXM];
        statsblk stats;
        static DEFAULTOPTIONS(options);
        setword workspace[50];

#ifdef INSTRUMENT
        ++a2calls;
        if (nuniq) ++a2uniq;
#endif
        nx = n + 1;
        for (i = 0; i < n; ++i)
        {
            gx[i] = g[i];
            degx[i] = deg[i];
        }
        gx[n] = 0;
        degx[n] = degn = XPOPCOUNT(x);

        xw = x;
        while (xw)
        {
            i = XFIRSTBIT(xw);
            xw &= ~xbit[i];
            gx[i] |= bit[n];
            gx[n] |= bit[i];
            ++degx[i];
        }

        if (nuniq)
        {
#ifdef INSTRUMENT
            ++a2succs;
#endif
            if (canonise) makecanon(gx,gcan,nx);
            return TRUE;
        }

        i0 = 0;
        i1 = n;
        for (i = 0; i < nx; ++i)
        {
            if (degx[i] == degn) lab[i1--] = i;
            else                 lab[i0++] = i;
            ptn[i] = 1;
        }
        ptn[n] = 0;
        if (i0 == 0)
        {
            numcells = 1;
            active[0] = bit[0];

            if (!distinvar(gx,invar,nx)) return FALSE;
            qn = invar[n];
            j0 = 0;
            j1 = n;
            while (j0 <= j1)
            {
                j = lab[j0];
                qv = invar[j];
                if (qv < qn)
                    ++j0;
                else
                {
                    lab[j0] = lab[j1];
                    lab[j1] = j;
                    --j1;
                }
            }
            if (j0 > 0)
            {
                if (j0 == n)
                {
#ifdef INSTRUMENT
                    ++a2succs;
#endif
                    if (canonise) makecanon(gx,gcan,nx);
                    return TRUE;
                }
                ptn[j1] = 0;
                ++numcells;
                active[0] |= bit[j0];
            }
        }
        else
        {
            numcells = 2;
            ptn[i1] = 0;
            active[0] = bit[0] | bit[i1+1];

            vmax = 0;
            for (i = i1+1; i < nx; ++i)
                vmax |= bit[lab[i]];

            gv = gx[n] & vmax;
            qn = POPCOUNT(gv);

            j0 = i1+1;
            j1 = n;
            while (j0 <= j1)
            {
                j = lab[j0];
                gv = gx[j] & vmax;
                qv = POPCOUNT(gv);
                if (qv > qn)
                    return FALSE;
                else if (qv < qn)
                    ++j0;
                else
                {
                    lab[j0] = lab[j1];
                    lab[j1] = j;
                    --j1;
                }
            }
            if (j0 > i1+1)
            {
                if (j0 == n)
                {
#ifdef INSTRUMENT
                    ++a2succs;
#endif
                    if (canonise) makecanon(gx,gcan,nx);
                    return TRUE;
                }
                ptn[j1] = 0;
                ++numcells;
                active[0] |= bit[j0];
            }
        }

        refinex(gx,lab,ptn,0,&numcells,count,active,TRUE,&code,1,nx);

        if (code < 0) return FALSE;
        else if (code > 0 || numcells >= nx-4)
        {
#ifdef INSTRUMENT
            ++a2succs;
#endif
            if (canonise) makecanon(gx,gcan,nx);
            return TRUE;
        }

        options.writemarkers = FALSE;
        options.writeautoms = FALSE;
        options.getcanon = TRUE;
        options.defaultptn = FALSE;

        active[0] = 0;
#ifdef INSTRUMENT
        ++a2nauty;
#endif
        nauty(gx,lab,ptn,active,orbits,&options,&stats,workspace,50,1,nx,gcan);

        if (orbits[lab[n]] == orbits[n])
        {
#ifdef INSTRUMENT
            ++a2succs;
#endif
            if (canonise) makecanon(gx,gcan,nx);
            return TRUE;
        }
        else
            return FALSE;
}

/**************************************************************************/

static void
xbnds(n,ne,dmax)       /* find bounds on extension degree */
int n,ne,dmax;         /* store answer in data[*].*  */
{
        register int xlb,xub,d,nn,m,xc;

        xlb = n == 1 ? 0 : (dmax > (2*ne + n - 2)/(n - 1) ?
                            dmax : (2*ne + n - 2)/(n - 1));
        xub = n < maxdeg ? n : maxdeg;

        for (xc = xub; xc >= xlb; --xc)
        {
            d = xc;
            m = ne + d;
            for (nn = n+1; nn < maxn; ++nn)
            {
                if (d < (2*m + nn - 2)/(nn - 1)) d = (2*m + nn - 2)/(nn - 1);
                m += d;
            }
            if (d > maxdeg || m > maxe) xub = xc - 1;
            else                        break;
        }

        if (ne + xlb < mine)
            for (xc = xlb; xc <= xub; ++xc)
            {
                m = ne + xc;
                for (nn = n + 1; nn < maxn; ++nn)
                    m += maxdeg < nn ? maxdeg : nn;
                if (m < mine) xlb = xc + 1;
                else          break;
            }

        data[n].ne = ne;
        data[n].dmax = dmax;
        data[n].xlb = xlb;
        data[n].xub = xub;
}

/**************************************************************************/

static void
tfextend(g,n,deg,ne,rigid,xlb,xub)        /* extend from n to n+1 */
graph *g;                     /* version for triangle-free graphs */
int n,*deg,ne,xlb,xub;
boolean rigid;
{
        register int x,d,bitv,hv;
        int *xinv,*xorb,xc;
        int nx,i,j,dmax,xlbx,xubx;
        graph gx[MAXN];
        int h[MAXN],*xx,v,ixx,nchild;
        int degx[MAXN];
        boolean rigidx;

#ifdef INSTRUMENT
        boolean haschild;

        haschild = FALSE;
        ++nodes[n];
        if (rigid) ++rigidnodes[n];
#endif

        nx = n + 1;
        dmax = deg[n-1];
        d = xbit[n-1];
        for (i = 0; i < n-1; ++i)
            if (deg[i] == dmax) d |= xbit[i];

        if (xlb == dmax && XPOPCOUNT(d) + dmax > n) ++xlb;
        if (xlb > xub) return;

        xinv = data[n].xinv;
        xorb = data[n].xorb;
        xx = data[n].xx;

        makexgraph(g,h,n);
        xx[0] = 0;
        nchild = 1;

        if (nx == maxn)
        {
            if (xlb == 0 && !connec && (nx != nprune
                  || (curres = curres == 0 ? mod-1 : curres-1) == 0)
                && accept2(g,n,0,gx,deg,FALSE))
            {
                ++count[ne];
#ifdef INSTRUMENT
                haschild = TRUE;
#endif
                (*outproc)(outfile,canonise ? gcan : gx,nx);
            }

            if (xub > 0)
            for (v = 0; v < n; ++v)
            {
                bitv = xbit[v];
                hv = h[v];

                for (ixx = nchild; --ixx >= 0;)
                if ((hv & xx[ixx]) == 0)
                {
                    x = xx[ixx] | bitv;
                    xc = XPOPCOUNT(x);
                    if (xc < xub) xx[nchild++] = x;

                    if (xc >= xlb && (rigid || xorb[xinv[x]] == xinv[x])
                               // CHANGE added parens around last && to eliminate compile warning
                               && (xc > dmax || (xc == dmax && (x & d) == 0)))
                    {
                        if ((nx != nprune
                            || (curres = curres == 0 ? mod-1 : curres-1) == 0)
                            && accept2(g,n,x,gx,deg,
                                // CHANGE added parens around last && to eliminate compile warning
                                xc > dmax+1 || (xc == dmax+1 && (x & d) == 0))
                            && (!connec || isconnected(gx,nx)))
                        {
                            ++count[ne+xc];
#ifdef INSTRUMENT
                            haschild = TRUE;
#endif
                            (*outproc)(outfile,canonise ? gcan : gx,nx);
                        }
                    }
                }
            }
        }
        else
        {
            if (xlb == 0 && (nx != nprune
                        || (curres = curres == 0 ? mod-1 : curres-1) == 0))
            {
                for (j = 0; j < n; ++j)
                    degx[j] = deg[j];
                if (data[nx].ne != ne || data[nx].dmax != 0)
                    xbnds(nx,ne,0);
                xlbx = data[nx].xlb;
                xubx = data[nx].xub;

                data[nx].lo = data[nx].xstart[xlbx];
                data[nx].hi = data[nx].xstart[xubx+1];
                if (xubx >= xlbx && accept1(g,n,0,gx,degx,&rigidx))
                {
#ifdef INSTRUMENT
                    haschild = TRUE;
#endif
                    tfextend(gx,nx,degx,ne,rigidx,xlbx,xubx);
                }
            }

            if (xub > 0)
            for (v = 0; v < n; ++v)
            {
                bitv = xbit[v];
                hv = h[v];

                for (ixx = nchild; --ixx >= 0;)
                if ((hv & xx[ixx]) == 0)
                {
                    x = xx[ixx] | bitv;
                    xc = XPOPCOUNT(x);
                    if (xc < xub) xx[nchild++] = x;

                    if (xc >= xlb && (rigid || xorb[xinv[x]] == xinv[x])
                               // CHANGE added parens around last && to eliminate compile warning
                               && (xc > dmax || (xc == dmax && (x & d) == 0)))
                    {
                        if (nx == nprune)
                        {
                            if (curres == 0) curres = mod;
                            if (--curres != 0) continue;
                        }
                        for (j = 0; j < n; ++j)
                            degx[j] = deg[j];
                        if (data[nx].ne != ne+xc || data[nx].dmax != xc)
                            xbnds(nx,ne+xc,xc);
                        xlbx = data[nx].xlb;
                        xubx = data[nx].xub;

                        data[nx].lo = data[nx].xstart[xlbx];
                        data[nx].hi = data[nx].xstart[xubx+1];

                        if (xubx >= xlbx && accept1(g,n,x,gx,degx,&rigidx))
                        {
#ifdef INSTRUMENT
                            haschild = TRUE;
#endif
                            tfextend(gx,nx,degx,ne+xc,rigidx,xlbx,xubx);
                        }
                    }
                }
            }
        }
#ifdef INSTRUMENT
        if (haschild) ++fertilenodes[n];
#endif
}

/**************************************************************************/

static void
bipextend(g,n,deg,ne,rigid,xlb,xub)        /* extend from n to n+1 */
graph *g;                     /* version for bipartite graphs */
int n,*deg,ne,xlb,xub;
boolean rigid;
{
        register int x,d,bitv,hv;
        int *xinv,*xorb,xc;
        int nx,i,j,dmax,xlbx,xubx;
        graph gx[MAXN];
        int h[MAXN],*xx,v,ixx,nchild;
        int degx[MAXN];
        boolean rigidx;

#ifdef INSTRUMENT
        boolean haschild;

        haschild = FALSE;
        ++nodes[n];
        if (rigid) ++rigidnodes[n];
#endif

        nx = n + 1;
        dmax = deg[n-1];
        d = xbit[n-1];
        for (i = 0; i < n-1; ++i)
            if (deg[i] == dmax) d |= xbit[i];

        if (xlb == dmax && XPOPCOUNT(d) + dmax > n) ++xlb;
        if (xlb > xub) return;

        xinv = data[n].xinv;
        xorb = data[n].xorb;
        xx = data[n].xx;

        makebgraph(g,h,n);
        xx[0] = 0;
        nchild = 1;

        if (nx == maxn)
        {
            if (xlb == 0 && !connec && (nx != nprune
                  || (curres = curres == 0 ? mod-1 : curres-1) == 0)
                && accept2(g,n,0,gx,deg,FALSE))
            {
                ++count[ne];
#ifdef INSTRUMENT
                haschild = TRUE;
#endif
                (*outproc)(outfile,canonise ? gcan : gx,nx);
            }

            if (xub > 0)
            for (v = 0; v < n; ++v)
            {
                bitv = xbit[v];
                hv = h[v];

                for (ixx = nchild; --ixx >= 0;)
                if ((hv & xx[ixx]) == 0)
                {
                    x = xx[ixx] | bitv;
                    xc = XPOPCOUNT(x);
                    if (xc < xub) xx[nchild++] = x;

                    if (xc >= xlb && (rigid || xorb[xinv[x]] == xinv[x])
                               // CHANGE added parens around last && to eliminate compile warning
                               && (xc > dmax || (xc == dmax && (x & d) == 0)))
                    {
                        if ((nx != nprune
                            || (curres = curres == 0 ? mod-1 : curres-1) == 0)
                            && accept2(g,n,x,gx,deg,
                                // CHANGE added parens around last && to eliminate compile warning
                                xc > dmax+1 || (xc == dmax+1 && (x & d) == 0))
                            && (!connec || isconnected(gx,nx)))
                        {
                            ++count[ne+xc];
#ifdef INSTRUMENT
                            haschild = TRUE;
#endif
                            (*outproc)(outfile,canonise ? gcan : gx,nx);
                        }
                    }
                }
            }
        }
        else
        {
            if (xlb == 0 && (nx != nprune
                        || (curres = curres == 0 ? mod-1 : curres-1) == 0))
            {
                for (j = 0; j < n; ++j)
                    degx[j] = deg[j];
                if (data[nx].ne != ne || data[nx].dmax != 0)
                    xbnds(nx,ne,0);
                xlbx = data[nx].xlb;
                xubx = data[nx].xub;

                data[nx].lo = data[nx].xstart[xlbx];
                data[nx].hi = data[nx].xstart[xubx+1];
                if (xubx >= xlbx && accept1(g,n,0,gx,degx,&rigidx))
                {
#ifdef INSTRUMENT
                    haschild = TRUE;
#endif
                    bipextend(gx,nx,degx,ne,rigidx,xlbx,xubx);
                }
            }

            if (xub > 0)
            for (v = 0; v < n; ++v)
            {
                bitv = xbit[v];
                hv = h[v];

                for (ixx = nchild; --ixx >= 0;)
                if ((hv & xx[ixx]) == 0)
                {
                    x = xx[ixx] | bitv;
                    xc = XPOPCOUNT(x);
                    if (xc < xub) xx[nchild++] = x;

                    if (xc >= xlb && (rigid || xorb[xinv[x]] == xinv[x])
                               // CHANGE added parens around last && to eliminate compile warning
                               && (xc > dmax || (xc == dmax && (x & d) == 0)))
                    {
                        if (nx == nprune)
                        {
                            if (curres == 0) curres = mod;
                            if (--curres != 0) continue;
                        }
                        for (j = 0; j < n; ++j)
                            degx[j] = deg[j];
                        if (data[nx].ne != ne+xc || data[nx].dmax != xc)
                            xbnds(nx,ne+xc,xc);
                        xlbx = data[nx].xlb;
                        xubx = data[nx].xub;

                        data[nx].lo = data[nx].xstart[xlbx];
                        data[nx].hi = data[nx].xstart[xubx+1];

                        if (xubx >= xlbx && accept1(g,n,x,gx,degx,&rigidx))
                        {
#ifdef INSTRUMENT
                            haschild = TRUE;
#endif
                            bipextend(gx,nx,degx,ne+xc,rigidx,xlbx,xubx);
                        }
                    }
                }
            }
        }
#ifdef INSTRUMENT
        if (haschild) ++fertilenodes[n];
#endif
}

/**************************************************************************/

static void
genextend(g,n,deg,ne,rigid,xlb,xub)        /* extend from n to n+1 */
graph *g;                            /* version for general graphs */
int n,*deg,ne,xlb,xub;
boolean rigid;
{
        register int x,d;
        int *xset,*xcard,*xorb,xc;
        int nx,i,j,imin,imax,dmax;
        int xlbx,xubx;
        graph gx[MAXN];
        int degx[MAXN];
        boolean rigidx;

#ifdef INSTRUMENT
        boolean haschild;

        haschild = FALSE;
        ++nodes[n];
        if (rigid) ++rigidnodes[n];
#endif

        nx = n + 1;
        dmax = deg[n-1];
        d = xbit[n-1];
        for (i = 0; i < n-1; ++i)
            if (deg[i] == dmax) d |= xbit[i];

        if (xlb == dmax && XPOPCOUNT(d) + dmax > n) ++xlb;
        if (xlb > xub) return;

        imin = data[n].xstart[xlb];
        imax = data[n].xstart[xub+1];
        xset = data[n].xset;
        xcard = data[n].xcard;
        xorb = data[n].xorb;

        if (nx == maxn)
            for (i = imin; i < imax; ++i)
            {
                if (!rigid && xorb[i] != i) continue;
                x = xset[i];
                xc = xcard[i];
                if (xc == dmax && (x & d) != 0) continue;

                if (nx == nprune)
                {
                    if (curres == 0) curres = mod;
                    if (--curres != 0) continue;
                }
                if (accept2(g,n,x,gx,deg,
                            // CHANGE added parens around last && to eliminate compile warning
                            xc > dmax+1 || (xc == dmax+1 && (x & d) == 0)))
                    if (!connec || isconnected(gx,nx))
                    {
                        ++count[ne+xc];
#ifdef INSTRUMENT
                        haschild = TRUE;
#endif
                        (*outproc)(outfile,canonise ? gcan : gx,nx);
                    }
            }
        else
            for (i = imin; i < imax; ++i)
            {
                if (!rigid && xorb[i] != i) continue;
                x = xset[i];
                xc = xcard[i];
                if (xc == dmax && (x & d) != 0) continue;
                if (nx == nprune)
                {
                    if (curres == 0) curres = mod;
                    if (--curres != 0) continue;
                }
                for (j = 0; j < n; ++j)
                    degx[j] = deg[j];
                if (data[nx].ne != ne+xc || data[nx].dmax != xc)
                    xbnds(nx,ne+xc,xc);
                xlbx = data[nx].xlb;
                xubx = data[nx].xub;
                if (xlbx > xubx) continue;

                data[nx].lo = data[nx].xstart[xlbx];
                data[nx].hi = data[nx].xstart[xubx+1];
                if (accept1(g,n,x,gx,degx,&rigidx))
                {
#ifdef INSTRUMENT
                    haschild = TRUE;
#endif
                    genextend(gx,nx,degx,ne+xc,rigidx,xlbx,xubx);
                }
            }
#ifdef INSTRUMENT
        if (haschild) ++fertilenodes[n];
#endif
}

/**************************************************************************/
/**************************************************************************/

// CHANGE start
// Added this include file and extern...
#include "outproc.h"
extern int errorFound;
// And changed main to makeg_main, changed prototype declaration,
// and added command char (e.g. p=planarity, d=planar drawing,
// o=outerplanarity, 2=K2,3 search, 3=K3,3 search, ...
int makeg_main(char command, int argc, char *argv[])
// CHANGE end
{
        char *arg;
        long ltemp;
        boolean badargs;
        int i,argsgot;
        graph g[1];
        int deg[1];
        long nout;
        double t1,t2;

        badargs = FALSE;
        connec = FALSE;
        trianglefree = FALSE;
        bipartite = FALSE;
        verbose = FALSE;
        nautyformat = FALSE;
        nooutput = FALSE;
        canonise = FALSE;

        maxdeg = MAXN;

        argsgot = 0;
        for (i = 1; !badargs && i < argc; ++i)
        {
            arg = argv[i];
            if (arg[0] == '-' && arg[1] != '\0')
            {
                if (arg[1] == 'c' || arg[1] == 'C') connec = TRUE;
                else if (arg[1] == 'd' || arg[1] == 'D')
                {
                    if (sscanf(arg+2,"%d",&maxdeg) != 1) badargs = TRUE;
                }
                else if (arg[1] == 'n' || arg[1] == 'N') nautyformat = TRUE;
                else if (arg[1] == 'u' || arg[1] == 'U') nooutput = TRUE;
                else if (arg[1] == 't' || arg[1] == 'T') trianglefree = TRUE;
                else if (arg[1] == 'b' || arg[1] == 'B') bipartite = TRUE;
                else if (arg[1] == 'v' || arg[1] == 'V') verbose = TRUE;
                else if (arg[1] == 'l' || arg[1] == 'L') canonise = TRUE;
                else badargs = TRUE;
            }
            else
            {
                if (argsgot > 4)
                    badargs = TRUE;
                else
                {
                    if (sscanf(arg,"%ld",&ltemp) != 1) badargs = TRUE;
                    else if (argsgot == 0) maxn = ltemp;
                    else if (argsgot == 1) mine = ltemp;
                    else if (argsgot == 2) maxe = ltemp;
                    else if (argsgot == 3) mod = ltemp;
                    else if (argsgot == 4) res = ltemp;
                }
                ++argsgot;
            }
        }

        if (argsgot == 0)
            badargs = TRUE;
        else if (maxn < 1 || maxn > MAXN)
        {
            fprintf(stderr,">E makeg: n must be in the range 1..%d\n",MAXN);
            badargs = TRUE;
        }

        if (argsgot == 1)
        {
            mine = 0;
            maxe = (maxn*maxn - maxn) / 2;
        }
        else if (argsgot == 2)
            maxe = mine;

        if (argsgot <= 3)
        {
            mod = 1;
            res = 0;
        }
        else if (argsgot == 4 || argsgot > 5)
            badargs = TRUE;

        if (maxdeg >= maxn) maxdeg = maxn - 1;
        if (maxe > maxn*maxdeg / 2) maxe =  maxn*maxdeg / 2;

        if (!badargs && (mine > maxe || maxe < 0 || maxdeg < 0))
        {
            fprintf(stderr,">E makeg: impossible mine,maxe,maxdeg values\n");
            badargs = TRUE;
        }

        if (!badargs && (res < 0 || res >= mod))
        {
            fprintf(stderr,">E makeg: must have 0 <= res < mod\n");
            badargs = TRUE;
        }

        if (connec && mine < maxn-1) mine = maxn - 1;
        if (bipartite) trianglefree = TRUE;
        if (trianglefree && maxe > (maxn/2)*(maxn-maxn/2))
            maxe = (maxn/2)*(maxn-maxn/2);

        if (badargs)
        {
            fprintf(stderr,
">E Usage: makeg [-c -t -b -n -u -v -l] [-d<max>] n [mine [maxe [mod res]]]\n");
// CHANGE start
            //exit(2);
            return 2;
// CHANGE end
        }

// CHANGE start
        g_maxn = maxn;
        g_mine = mine;
        g_maxe = maxe;
        g_mod = mod;
        g_res = res;
        g_command = command;
        errorFound = 0;
        outproc = outprocTest;
//        if (nautyformat)   outproc = writenauty;
//        else if (nooutput) outproc = nullwrite;
//        else               outproc = writeny;
// CHANGE end

        for (i = 0; i <= maxe; ++i)
            count[i] = 0;

        msgfile = stderr;
        outfile = stdout;
// CHANGE start
        g_msgfile = msgfile;
// CHANGE end

// CHANGE start (commented this out)
//        fprintf(msgfile,">A n=%d e=%d:%d d=%d class=%d/%d\n",
//                        maxn,mine,maxe,maxdeg,mod,res);
// CHANGE end

        g[0] = 0;
        deg[0] = 0;

        t1 = CPUTIME;

        if (maxn == 1)
        {
            if (res == 0)
            {
                ++count[0];
                (*outproc)(outfile,g,1);
            }
        }
        else
        {
            makeleveldata();
            curres = res;
            if (mod <= 1)       nprune = 0;
            else if (maxn >= 9) nprune = maxn - 2;
            else if (maxn >= 7) nprune = maxn - 1;
            else                nprune = maxn;

            xbnds(1,0,0);
            if (bipartite)
                bipextend(g,1,deg,0,TRUE,data[1].xlb,data[1].xub);
            else if (trianglefree)
                tfextend(g,1,deg,0,TRUE,data[1].xlb,data[1].xub);
            else
                genextend(g,1,deg,0,TRUE,data[1].xlb,data[1].xub);
        }
        t2 = CPUTIME;

        nout = 0;
        for (i = 0; i <= maxe; ++i)
            nout += count[i];

        if (verbose)
            for (i = 0; i <= maxe; ++i)
                if (count[i] > 0)
                    fprintf(msgfile,
                      ">C %7ld graphs with %d edges\n",count[i],i);

#ifdef INSTRUMENT
        fprintf(msgfile,"\n>N node counts\n");
        for (i = 1; i < maxn; ++i)
            fprintf(msgfile," level %2d: %7ld (%ld rigid, %ld fertile)\n",
                            i,nodes[i],rigidnodes[i],fertilenodes[i]);
        fprintf(msgfile,">A1 %ld calls to accept1, %ld nauty, %ld succeeded\n",
                        a1calls,a1nauty,a1succs);
        fprintf(msgfile,
             ">A2 %ld calls to accept2, %ld nuniq, %ld nauty, %ld succeeded\n",
                        a2calls,a2uniq,a2nauty,a2succs);
        fprintf(msgfile,"\n");
#endif

// CHANGE start
        Test_PrintStats(outfile);
// CHANGE end

// CHANGE start (commented this out)
//        fprintf(msgfile,">Z %ld graphs generated in %3.2f sec\n",nout,t2-t1);
// CHANGE end

// CHANGE start
        return 0;
        //exit(0);
// CHANGE end
}

