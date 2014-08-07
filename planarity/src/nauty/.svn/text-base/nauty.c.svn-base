/*****************************************************************************
*                                                                            *
*  Main source file for version 1.9 of nauty.                                *
*                                                                            *
*   Copyright (1984-1993) Brendan McKay.  All rights reserved.  Permission   *
*   Subject to the waivers and disclaimers in nauty.h.                       *
*                                                                            *
*   CHANGE HISTORY                                                           *
*       10-Nov-87 : final changes for version 1.2                            *
*        5-Dec-87 : renamed to version 1.3 (no changes to this file)         *
*       28-Sep-88 : renamed to version 1.4 (no changes to this file)         *
*       23-Mar-89 : changes for version 1.5 :                                *
*                   - add use of refine1 instead of refine for m==1          *
*                   - changes for new optionblk syntax                       *
*                   - disable tc_level use for digraphs                      *
*                   - interposed doref() interface to refine() so that       *
*                        options.invarproc can be supported                  *
*                   - declared local routines static                         *
*       28-Mar-89 : - implemented mininvarlevel/maxinvarlevel < 0 options    *
*        2-Apr-89 : - added invarproc fields in stats                        *
*        5-Apr-89 : - modified error returns from nauty()                    *
*                   - added error message to ERRFILE                         *
*                   - changed MAKEEMPTY uses to EMPTYSET                     *
*       18-Apr-89 : - added MTOOBIG and CANONGNIL                            *
*        8-May-89 : - changed firstcode[] and canoncode[] to short           *
*       10-Nov-90 : changes for version 1.6 :                                *
*                   - added dummy routine nauty_null (see dreadnaut.c)       *
*        2-Sep-91 : changes for version 1.7 :                                *
*                   - moved MULTIPLY into nauty.h                            *
*       27-Mar-92 : - changed 'n' into 'm' in error message in nauty()       *
*        5-Jun-93 : renamed to version 1.7+ (no changes to this file)        *
*       18-Aug-93 : renamed to version 1.8 (no changes to this file)         *
*       17-Sep-93 : renamed to version 1.9 (no changes to this file)         *
*                                                                            *
*****************************************************************************/

#define EXTDEFS 1
#include "nauty.h"

static int  firstpathnode();
static void firstterminal();
static int  othernode();
static int  processnode();
static void recover();
static void writemarker();

#if  MAXM==1
#define M 1
#else
#define M m
#endif

#define OPTCALL(proc) if (proc != NILFUNCTION) (*proc)

    /* copies of some of the options: */
static boolean getcanon,digraph,writeautoms,domarkers,cartesian;
static int linelength,tc_level,mininvarlevel,maxinvarlevel,invararg;
static UPROC (*usernodeproc)(),(*userautomproc)(),(*userlevelproc)(),
             (*refproc)(),(*tcellproc)(),(*invarproc)();
static FILE *outfile;

    /* local versions of some of the arguments: */
static int m,n;
static graph *g,*canong;
static nvector *orbits;
static statsblk *stats;
    /* temporary versions of some stats: */
static long invapplics,invsuccesses;
static int invarsuclevel;

    /* working variables: <the "bsf leaf" is the leaf which is best guess so
                                far at the canonical leaf>  */
static int gca_first,     /* level of greatest common ancestor of current
                                node and first leaf */
           gca_canon,     /* ditto for current node and bsf leaf */
           noncheaplevel, /* level of greatest ancestor for which cheapautom
                                == FALSE */
           allsamelevel,  /* level of least ancestor of first leaf for
                             which all descendant leaves are known to be
                             equivalent */
           eqlev_first,   /* level to which codes for this node match those
                                for first leaf */
           eqlev_canon,   /* level to which codes for this node match those
                                for the bsf leaf. */
           comp_canon,    /* -1,0,1 according as code at eqlev_canon+1 is
                                <,==,> that for bsf leaf.  Also used for
                                similar purpose during leaf processing */
           samerows,      /* number of rows of canong which are correct for
                                the bsf leaf  BDM:correct? */
           canonlevel,    /* level of bsf leaf */
           stabvertex,    /* point fixed in ancestor of first leaf at level
                                gca_canon */
           cosetindex;    /* the point being fixed at level gca_first */

static boolean needshortprune;       /* used to flag calls to shortprune */

static set defltwork[2*MAXM];        /* workspace in case none provided */
static permutation workperm[MAXN];   /* various scratch uses */
static set fixedpts[MAXM];           /* points which were explicitly
                                        fixed to get current node */
static permutation firstlab[MAXN],   /* label from first leaf */
                   canonlab[MAXN];   /* label from bsf leaf */
static short firstcode[MAXN+2],      /* codes for first leaf */
             canoncode[MAXN+2];      /* codes for bsf leaf */
static short firsttc[MAXN+2];        /* index of target cell for left path */
static set active[MAXM];             /* used to contain index to cells now
                                        active for refinement purposes */
static set *workspace,*worktop;      /* first and just-after-last addresses of
                                        work area to hold automorphism data */
static set *fmptr;                   /* pointer into workspace */

/*****************************************************************************
*                                                                            *
*  This procedure finds generators for the automorphism group of a           *
*  vertex-coloured graph and optionally finds a canonically labelled         *
*  isomorph.  A description of the data structures can be found in           *
*  nauty.h and in the "nauty User's Guide".  The Guide also gives            *
*  many more details about its use, and implementation notes.                *
*                                                                            *
*  Parameters - <r> means read-only, <w> means write-only, <wr> means both:  *
*           g <r>  - the graph                                               *
*     lab,ptn <rw> - used for the partition nest which defines the colouring *
*                  of g.  The initial colouring will be set by the program,  *
*                  using the same colour for every vertex, if                *
*                  options->defaultptn!=FALSE.  Otherwise, you must set it   *
*                  yourself (see the Guide). If options->getcanon!=FALSE,    *
*                  the contents of lab on return give the labelling of g     *
*                  corresponding to canong.  This does not change the        *
*                  initial colouring of g as defined by (lab,ptn), since     *
*                  the labelling is consistent with the colouring.           *
*     active  <r>  - If this is not NILSET and options->defaultptn==FALSE,   *
*                  it is a set indicating the initial set of active colours. *
*                  See the Guide for details.                                *
*     orbits  <w>  - On return, orbits[i] contains the number of the         *
*                  least-numbered vertex in the same orbit as i, for         *
*                  i=0,1,...,n-1.                                            *
*    options  <r>  - A list of options.  See nauty.h and/or the Guide        *
*                  for details.                                              *
*      stats  <w>  - A list of statistics produced by the procedure.  See    *
*                  nauty.h and/or the Guide for details.                     *
*  workspace  <w>  - A chunk of memory for working storage.                  *
*  worksize   <r>  - The number of setwords in workspace.  See the Guide     *
*                  for guidance.                                             *
*          m  <r>  - The number of setwords in sets.  This must be at        *
*                  least ceil(n / WORDSIZE) and at most MAXM.                *
*          n  <r>  - The number of vertices.  This must be at least 1 and    *
*                  at most MAXN.                                             *
*     canong  <w>  - The canononically labelled isomorph of g.  This is      *
*                  only produced if options->getcanon!=FALSE, and can be     *
*                  given as NILGRAPH otherwise.                              *
*                                                                            *
*  FUNCTIONS CALLED: firstpathnode(),updatecan()                             *
*                                                                            *
*****************************************************************************/

void
nauty(g_arg,lab,ptn,active_arg,orbits_arg,options,stats_arg,
                ws_arg,worksize,m_arg,n_arg,canong_arg)
graph *g_arg,*canong_arg;
set *active_arg,*ws_arg;
nvector *lab,*ptn,*orbits_arg;
int worksize,m_arg,n_arg;
optionblk *options;
statsblk *stats_arg;
{
        register int i;
        int numcells;

    /* check for excessive sizes: */
        if (m_arg > MAXM)
        {
            stats_arg->errstatus = MTOOBIG;
#ifdef  ERRFILE
            fprintf(ERRFILE,"nauty: need m <= %d\n\n",MAXM);
#endif
            return;
        }
        if (n_arg > MAXN || n_arg > WORDSIZE * m_arg)
        {
            stats_arg->errstatus = NTOOBIG;
#ifdef  ERRFILE
            fprintf(ERRFILE,
                    "nauty: need n <= min(%d,%d*m)\n\n",MAXM,WORDSIZE);
#endif
            return;
        }

    /* take copies of some args, and options: */
        m = m_arg;
        n = n_arg;
        g = g_arg;
        orbits = orbits_arg;
        stats = stats_arg;

        getcanon = options->getcanon;
        digraph = options->digraph;
        writeautoms = options->writeautoms;
        domarkers = options->writemarkers;
        cartesian = options->cartesian;
        linelength = options->linelength;
        if (digraph)
            tc_level = 0;
        else
            tc_level = options->tc_level;
        outfile = (options->outfile == (FILE*)NULL ?
                   stdout : options->outfile);
        usernodeproc = options->usernodeproc;
        userautomproc = options->userautomproc;
        userlevelproc = options->userlevelproc;

        if (options->userrefproc == NILFUNCTION)
            if (m == 1)
                refproc = refine1;
            else
                refproc = refine;
        else
            refproc = options->userrefproc;

        if (options->usertcellproc == NILFUNCTION)
            tcellproc = targetcell;
        else
            tcellproc = options->usertcellproc;

        invarproc = options->invarproc;
        if (options->mininvarlevel < 0 && options->getcanon)
            mininvarlevel = -options->mininvarlevel;
        else
            mininvarlevel = options->mininvarlevel;
        if (options->maxinvarlevel < 0 && options->getcanon)
            maxinvarlevel = -options->maxinvarlevel;
        else
            maxinvarlevel = options->maxinvarlevel;
        invararg = options->invararg;

// CHANGE added braces around if (getcanon) to eliminate compile warning
        if (getcanon)
        {
            if (canong_arg == NILGRAPH)
            {
                stats_arg->errstatus = CANONGNIL;
#ifdef  ERRFILE
                fprintf(ERRFILE,
                      "nauty: canong=NILGRAPH but options.getcanon=TRUE\n\n");
#endif
                return;
            }
            else
                canong = canong_arg;
        }

    /* initialize everything: */
        if (options->defaultptn)
        {
            for (i = 0; i < n; ++i)   /* give all verts same colour */
            {
                lab[i] = i;
                ptn[i] = INFINITY;
            }
            ptn[n-1] = 0;
            EMPTYSET(active,m);
            ADDELEMENT(active,0);
            numcells = 1;
        }
        else
        {
            ptn[n-1] = 0;
            numcells = 0;
            for (i = 0; i < n; ++i)
                if (ptn[i] != 0)
                    ptn[i] = INFINITY;
                else
                    ++numcells;
            if (active_arg == NILSET)
            {
                EMPTYSET(active,m);
                for (i = 0; i < n; ++i)
                {
                    ADDELEMENT(active,i);
                    while (ptn[i])
                        ++i;
                }
            }
            else
                for (i = 0; i < M; ++i)
                    active[i] = active_arg[i];
        }

        for (i = 0; i < n; ++i)
            orbits[i] = i;
        stats->grpsize1 = 1.0;
        stats->grpsize2 = 0;
        stats->numgenerators = 0;
        stats->numnodes = 0;
        stats->numbadleaves = 0;
        stats->tctotal = 0;
        stats->canupdates = 0;
        stats->numorbits = n;
        EMPTYSET(fixedpts,m);
        noncheaplevel = 1;
        eqlev_canon = -1;       /* needed even if !getcanon */

        if (worksize >= 2 * m)
            workspace = ws_arg;
        else
        {
            workspace = defltwork;
            worksize = 2 * MAXM;
        }
        worktop = workspace + (worksize - worksize % (2 * m));
        fmptr = workspace;

    /* here goes: */
        stats->errstatus = 0;
        needshortprune = FALSE;
        invarsuclevel = INFINITY;
        invapplics = invsuccesses = 0;

        firstpathnode(lab,ptn,1,numcells);

        if (getcanon)
        {
            updatecan(g,canong,canonlab,samerows,M,n);
            for (i = 0; i < n; ++i)
                lab[i] = canonlab[i];
        }
        stats->invarsuclevel =
             (invarsuclevel == INFINITY ? 0 : invarsuclevel);
        stats->invapplics = invapplics;
        stats->invsuccesses = invsuccesses;
}

/*****************************************************************************
*                                                                            *
*  firstpathnode(lab,ptn,level,numcells) produces a node on the leftmost     *
*  path down the tree.  The parameters describe the level and the current    *
*  colour partition.  The set of active cells is taken from the global set   *
*  'active'.  If the refined partition is not discrete, the leftmost child   *
*  is produced by calling firstpathnode, and the other children by calling   *
*  othernode.                                                                *
*  The value returned is the level to return to.                             *
*                                                                            *
*  FUNCTIONS CALLED: (*usernodeproc)(),doref(),cheapautom(),                 *
*                    firstterminal(),nextelement(),breakout(),               *
*                    firstpathnode(),othernode(),recover(),writestats(),     *
*                    (*userlevelproc)(),(*tcellproc)(),shortprune()          *
*                                                                            *
*****************************************************************************/

static int
firstpathnode(lab,ptn,level,numcells)
nvector *lab,*ptn;
int level,numcells;
{
        register int tv;
        // CHANGE assigned 0 to childcount to eliminate compile warning
        int tv1,index,rtnlevel,tcellsize,tc,childcount=0,qinvar,refcode;
        set tcell[MAXM];

        ++stats->numnodes;

    /* refine partition : */
        doref(g,lab,ptn,level,&numcells,&qinvar,workperm,
              active,&refcode,refproc,invarproc,
              mininvarlevel,maxinvarlevel,invararg,digraph,M,n);
        firstcode[level] = (short)refcode;
        if (qinvar > 0)
        {
            ++invapplics;
            if (qinvar == 2)
            {
                ++invsuccesses;
                if (mininvarlevel < 0)
                    mininvarlevel = level;
                if (maxinvarlevel < 0)
                    maxinvarlevel = level;
                if (level < invarsuclevel)
                    invarsuclevel = level;
            }
        }

        tc = -1;
        if (numcells != n)
        {
     /* locate new target cell, setting tc to its position in lab, tcell
                          to its contents, and tcellsize to its size: */
            (*tcellproc)(g,lab,ptn,level,numcells,tcell,&tcellsize,
                                                        &tc,tc_level,-1,M,n);
            stats->tctotal += tcellsize;
        }
        firsttc[level] = tc;

    /* optionally call user-defined node examination procedure: */
        OPTCALL(usernodeproc)
                       (g,lab,ptn,level,numcells,tc,(int)firstcode[level],M,n);

        if (numcells == n)      /* found first leaf? */
        {
            firstterminal(lab,level);
            OPTCALL(userlevelproc)(lab,ptn,level,orbits,stats,0,1,1,n,0,n);
            return(level - 1);
        }

        if (noncheaplevel >= level && !cheapautom(ptn,level,digraph,n))
            noncheaplevel = level + 1;

    /* use the elements of the target cell to produce the children: */
        index = 0;
        for (tv1 = tv = nextelement(tcell,M,-1); tv >= 0;
                                        tv = nextelement(tcell,M,tv))
        {
            if (orbits[tv] == tv)   /* ie, not equiv to previous child */
            {
                breakout(lab,ptn,level + 1,tc,tv,active,M);
                ADDELEMENT(fixedpts,tv);
                cosetindex = tv;
                if (tv == tv1)
                {
                    rtnlevel = firstpathnode(lab,ptn,level + 1,numcells + 1);
                    childcount = 1;
                    gca_first = level;
                    stabvertex = tv1;
                }
                else
                {
                    rtnlevel = othernode(lab,ptn,level + 1,numcells + 1);
                    ++childcount;
                }
                DELELEMENT(fixedpts,tv);
                if (rtnlevel < level)
                    return(rtnlevel);
                if (needshortprune)
                {
                    needshortprune = FALSE;
                    shortprune(tcell,fmptr - M,M);
                }
                recover(ptn,level);
            }
            if (orbits[tv] == tv1)  /* ie, in same orbit as tv1 */
                ++index;
        }
        MULTIPLY(stats->grpsize1,stats->grpsize2,index);

        if (tcellsize == index && allsamelevel == level + 1)
            --allsamelevel;

        if (domarkers)
            writemarker(level,tv1,index,tcellsize,stats->numorbits,numcells);
        OPTCALL(userlevelproc)(lab,ptn,level,orbits,stats,tv1,index,tcellsize,
                                                        numcells,childcount,n);
        return(level - 1);
}

/*****************************************************************************
*                                                                            *
*  othernode(lab,ptn,level,numcells) produces a node other than an ancestor  *
*  of the first leaf.  The parameters describe the level and the colour      *
*  partition.  The list of active cells is found in the global set 'active'. *
*  The value returned is the level to return to.                             *
*                                                                            *
*  FUNCTIONS CALLED: (*usernodeproc)(),doref(),refine(),recover(),           *
*                    processnode(),cheapautom(),(*tcellproc)(),shortprune(), *
*                    nextelement(),breakout(),othernode(),longprune()        *
*                                                                            *
*****************************************************************************/

static int
othernode(lab,ptn,level,numcells)
nvector *lab,*ptn;
int level,numcells;
{
        register int tv;
        set tcell[MAXM];
        int tv1,refcode,rtnlevel,tcellsize,tc,qinvar;
        short code;

        ++stats->numnodes;

    /* refine partition : */
        doref(g,lab,ptn,level,&numcells,&qinvar,workperm,active,
              &refcode,refproc,invarproc,mininvarlevel,maxinvarlevel,
              invararg,digraph,M,n);
        code = (short)refcode;
        if (qinvar > 0)
        {
            ++invapplics;
            if (qinvar == 2)
            {
                ++invsuccesses;
                if (level < invarsuclevel)
                    invarsuclevel = level;
            }
        }

        if (eqlev_first == level - 1 && code == firstcode[level])
            eqlev_first = level;
        if (getcanon)
        {
            if (eqlev_canon == level - 1)
            {
                if (code < canoncode[level])
                    comp_canon = -1;
                else if (code > canoncode[level])
                    comp_canon = 1;
                else
                {
                    comp_canon = 0;
                    eqlev_canon = level;
                }
            }
            if (comp_canon > 0)
                canoncode[level] = code;
        }

        tc = -1;
   /* If children will be required, find new target cell and set tc to its
      position in lab, tcell to its contents, and tcellsize to its size: */

        if (numcells < n && (eqlev_first == level ||
                             // CHANGE added parens around last && to eliminate compile warning
                             (getcanon && comp_canon >= 0)))
        {
            if (!getcanon || comp_canon < 0)
            {
                (*tcellproc)(g,lab,ptn,level,numcells,tcell,&tcellsize,
                                            &tc,tc_level,firsttc[level],M,n);
                if (tc != firsttc[level])
                    eqlev_first = level - 1;
            }
            else
                (*tcellproc)(g,lab,ptn,level,numcells,tcell,&tcellsize,
                                                    &tc,tc_level,-1,M,n);
            stats->tctotal += tcellsize;
        }

    /* optionally call user-defined node examination procedure: */
        OPTCALL(usernodeproc)(g,lab,ptn,level,numcells,tc,(int)code,M,n);

    /* call processnode to classify the type of this node: */

        rtnlevel = processnode(lab,ptn,level,numcells);
        if (rtnlevel < level)   /* keep returning if necessary */
            return(rtnlevel);
        if (needshortprune)
        {
            needshortprune = FALSE;
            shortprune(tcell,fmptr - M,M);
        }

        if (!cheapautom(ptn,level,digraph,n))
            noncheaplevel = level + 1;

    /* use the elements of the target cell to produce the children: */
        for (tv1 = tv = nextelement(tcell,M,-1); tv >= 0;
                                        tv = nextelement(tcell,M,tv))
        {
            breakout(lab,ptn,level + 1,tc,tv,active,M);
            ADDELEMENT(fixedpts,tv);
            rtnlevel = othernode(lab,ptn,level + 1,numcells + 1);
            DELELEMENT(fixedpts,tv);

            if (rtnlevel < level)
                return(rtnlevel);
    /* use stored automorphism data to prune target cell: */
            if (needshortprune)
            {
                needshortprune = FALSE;
                shortprune(tcell,fmptr - M,M);
            }
            if (tv == tv1)
                longprune(tcell,fixedpts,workspace,fmptr,M);

            recover(ptn,level);
        }

        return(level - 1);
}

/*****************************************************************************
*                                                                            *
*  Process the first leaf of the tree.                                       *
*                                                                            *
*  FUNCTIONS CALLED: NONE                                                    *
*                                                                            *
*****************************************************************************/

static void
firstterminal(lab,level)
nvector *lab;
register int level;
{
        register int i;

        stats->maxlevel = level;
        gca_first = allsamelevel = eqlev_first = level;
        firstcode[level + 1] = INFINITY;
        firsttc[level + 1] = -1;

        for (i = 0; i < n; ++i)
            firstlab[i] = lab[i];

        if (getcanon)
        {
            canonlevel = eqlev_canon = gca_canon = level;
            comp_canon = 0;
            samerows = 0;
            for (i = 0; i < n; ++i)
                canonlab[i] = lab[i];
            for (i = 0; i <= level; ++i)
                canoncode[i] = firstcode[i];
            canoncode[level + 1] = INFINITY;
            stats->canupdates = 1;
        }
}

/*****************************************************************************
*                                                                            *
*  Process a node other than the first leaf or its ancestors.  It is first   *
*  classified into one of five types and then action is taken appropriate    *
*  to that type.  The types are                                              *
*                                                                            *
*  0:   Nothing unusual.  This is just a node internal to the tree whose     *
*         children need to be generated sometime.                            *
*  1:   This is a leaf equivalent to the first leaf.  The mapping from       *
*         firstlab to lab is thus an automorphism.  After processing the     *
*         automorphism, we can return all the way to the closest invocation  *
*         of firstpathnode.                                                  *
*  2:   This is a leaf equivalent to the bsf leaf.  Again, we have found an  *
*         automorphism, but it may or may not be as useful as one from a     *
*         type-1 node.  Return as far up the tree as possible.               *
*  3:   This is a new bsf node, provably better than the previous bsf node.  *
*         After updating canonlab etc., treat it the same as type 4.         *
*  4:   This is a leaf for which we can prove that no descendant is          *
*         equivalent to the first or bsf leaf or better than the bsf leaf.   *
*         Return up the tree as far as possible, but this may only be by     *
*         one level.                                                         *
*                                                                            *
*  Types 2 and 3 can't occur if getcanon==FALSE.                             *
*  The value returned is the level in the tree to return to, which can be    *
*  anywhere up to the closest invocation of firstpathnode.                   *
*                                                                            *
*  FUNCTIONS CALLED:    isautom(),updatecan(),testcanlab(),fmperm(),         *
*                       writeperm(),(*userautomproc)(),orbjoin(),            *
*                       shortprune(),fmptn()                                 *
*                                                                            *
*****************************************************************************/

static int
processnode(lab,ptn,level,numcells)
nvector *lab,*ptn;
int level,numcells;
{
        register int i,code,save,newlevel;
        boolean ispruneok;
        int sr;

        code = 0;
        if (eqlev_first != level && (!getcanon || comp_canon < 0))
            code = 4;
        else if (numcells == n)
        {
            if (eqlev_first == level)
            {
                for (i = 0; i < n; ++i)
                    workperm[firstlab[i]] = lab[i];

                if (gca_first >= noncheaplevel ||
                                    isautom(g,workperm,digraph,M,n))
                    code = 1;
            }
            // CHANGE added braces on this if stmt to eliminate compile warning
            if (code == 0)
            {
                if (getcanon)
                {
                    sr = 0;
                    if (comp_canon == 0)
                    {
                        if (level < canonlevel)
                            comp_canon = 1;
                        else
                        {
                            updatecan(g,canong,canonlab,
                                                samerows,M,n);
                            samerows = n;
                            comp_canon = testcanlab(g,canong,lab,&sr,M,n);
                        }
                    }
                    if (comp_canon == 0)
                    {
                        for (i = 0; i < n; ++i)
                            workperm[canonlab[i]] = lab[i];
                        code = 2;
                    }
                    else if (comp_canon > 0)
                        code = 3;
                    else
                        code = 4;
                }
                else
                    code = 4;
            }
        }

        if (code != 0 && level > stats->maxlevel)
            stats->maxlevel = level;

        switch (code)
        {
        case 0:                 /* nothing unusual noticed */
            return(level);

        case 1:                 /* lab is equivalent to firstlab */
            if (fmptr == worktop)
                fmptr -= 2 * M;
            fmperm(workperm,fmptr,fmptr + M,M,n);
            fmptr += 2 * M;
            if (writeautoms)
                writeperm(outfile,workperm,cartesian,linelength,n);
            stats->numorbits = orbjoin(orbits,workperm,n);
            ++stats->numgenerators;
            OPTCALL(userautomproc)(stats->numgenerators,workperm,orbits,
                                        stats->numorbits,stabvertex,n);
            return(gca_first);

        case 2:                 /* lab is equivalent to canonlab */
            if (fmptr == worktop)
                fmptr -= 2 * M;
            fmperm(workperm,fmptr,fmptr + M,M,n);
            fmptr += 2 * M;
            save = stats->numorbits;
            stats->numorbits = orbjoin(orbits,workperm,n);
            if (stats->numorbits == save)
            {
                if (gca_canon != gca_first)
                    needshortprune = TRUE;
                return(gca_canon);
            }
            if (writeautoms)
                writeperm(outfile,workperm,cartesian,linelength,n);
            ++stats->numgenerators;
            OPTCALL(userautomproc)(stats->numgenerators,workperm,orbits,
                                        stats->numorbits,stabvertex,n);
            if (orbits[cosetindex] < cosetindex)
                return(gca_first);
            if (gca_canon != gca_first)
                needshortprune = TRUE;
            return(gca_canon);

        case 3:                 /* lab is better than canonlab */
            ++stats->canupdates;
            for (i = 0; i < n; ++i)
                canonlab[i] = lab[i];
            canonlevel = eqlev_canon = gca_canon = level;
            comp_canon = 0;
            canoncode[level + 1] = INFINITY;
            samerows = sr;
            break;

        case 4:                /* non-automorphism terminal node */
            ++stats->numbadleaves;
            break;
        }  /* end of switch statement */

    /* only cases 3 and 4 get this far: */
        if (level != noncheaplevel)
        {
            ispruneok = TRUE;
            if (fmptr == worktop)
                fmptr -= 2 * M;
            fmptn(lab,ptn,noncheaplevel,fmptr,fmptr + M,M,n);
            fmptr += 2 * M;
        }
        else
            ispruneok = FALSE;

        save = (allsamelevel > eqlev_canon ? allsamelevel - 1 : eqlev_canon);
        newlevel = (noncheaplevel <= save ? noncheaplevel - 1 : save);

        if (ispruneok && newlevel != gca_first)
            needshortprune = TRUE;
        return(newlevel);
 }

/*****************************************************************************
*                                                                            *
*  Recover the partition nest at level 'level' and update various other      *
*  parameters.                                                               *
*                                                                            *
*  FUNCTIONS CALLED: NONE                                                    *
*                                                                            *
*****************************************************************************/

static void
recover(ptn,level)
register nvector *ptn;
register int level;
{
        register int i;

        for (i = 0; i < n; ++i)
            if (ptn[i] > level)
                ptn[i] = INFINITY;

        if (level < noncheaplevel)
            noncheaplevel = level + 1;
        if (level < eqlev_first)
            eqlev_first = level;
        if (getcanon)
        {
            if (level < gca_canon)
                gca_canon = level;
            if (level <= eqlev_canon)
            {
                eqlev_canon = level;
                comp_canon = 0;
            }
        }
}

/*****************************************************************************
*                                                                            *
*  Write statistics concerning an ancestor of the first leaf.                *
*                                                                            *
*  level = its level                                                         *
*  tv = the vertex fixed to get the first child = the smallest-numbered      *
*               vertex in the target cell                                    *
*  cellsize = the size of the target cell                                    *
*  index = the number of vertices in the target cell which were equivalent   *
*               to tv = the index of the stabiliser of tv in the group       *
*               fixing the colour partition at this level                    *
*                                                                            *
*  numorbits = the number of orbits of the group generated by all the        *
*               automorphisms so far discovered                              *
*                                                                            *
*  numcells = the total number of cells in the equitable partition at this   *
*               level                                                        *
*                                                                            *
*  FUNCTIONS CALLED: itos(),putstring()                                      *
*                                                                            *
*****************************************************************************/

static void
writemarker(level,tv,index,tcellsize,numorbits,numcells)
int level,tv,index,tcellsize,numorbits,numcells;
{
        char s[30];

#define PUTINT(i) itos(i,s); putstring(outfile,s)
#define PUTSTR(x) putstring(outfile,x)

        PUTSTR("level ");
        PUTINT(level);
        PUTSTR(":  ");
        if (numcells != numorbits)
        {
            PUTINT(numcells);
            PUTSTR(" cell");
            if (numcells == 1)
                PUTSTR("; ");
            else
                PUTSTR("s; ");
        }
            PUTINT(numorbits);
        PUTSTR(" orbit");
        if (numorbits == 1)
        PUTSTR("; ");
        else
            PUTSTR("s; ");
        PUTINT(tv + labelorg);
        PUTSTR(" fixed; index ");
        PUTINT(index);
        if (tcellsize != index)
        {
            PUTSTR("/");
            PUTINT(tcellsize);
        }
        PUTSTR("\n");
}

/*****************************************************************************
*                                                                            *
*  nauty_null() does nothing.  See dreadnaut.c for its purpose.              *
*                                                                            *
*****************************************************************************/

void
nauty_null()
{
}
