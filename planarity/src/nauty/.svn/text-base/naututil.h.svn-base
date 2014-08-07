/*****************************************************************************
*                                                                            *
* This is the header file for versions 1.9+ of naututil.c and dreadnaut.c.   *
*                                                                            *
*   Copyright (1984-1993) Brendan McKay.  All rights reserved.               *
*   Subject to the waivers and disclaimers in nauty.h.                       *
*                                                                            *
*   CHANGE HISTORY                                                           *
*       10-Nov-87 : final changes for version 1.2                            *
*        5-Dec-87 : changes for version 1.3 :                                *
*                   - added declarations of readinteger() and readstring()   *
*                   - added definition of DEFEXT : default file-name         *
*                     extension for dreadnaut input files                    *
*       28-Sep-88 : changes for version 1.4 :                                *
*                   - added support for PC Turbo C                           *
*       29-Nov-88 : - added getc macro for AZTEC C on MAC                    *
*       23-Mar-89 : changes for version 1.5 :                                *
*                   - added DREADVERSION macro                               *
*                   - added optional ANSI function prototypes                *
*                   - changed file name to naututil.h                        *
*                   - moved ALLOCS to nauty.h and defined DYNALLOC           *
*       25-Mar-89 : - added declaration of twopaths()                        *
*       29-Mar-89 : - added declaration of putmapping()                      *
*        4-Apr-89 : - added declarations of triples, quadruples, adjtriang   *
*                   - only define ERRFILE if not in nauty.h                  *
*       25-Apr-89 : - added declarations of cellquads,distances,getbigcells  *
*       26-Apr-89 : - added declarations of indsets,cliques,cellquins        *
*                   - removed declarations of ptncode and equitable          *
*       27-Apr-89 : - added declaration of putquotient                       *
*       18-Aug-89 : - added new arg to putset, and changed mathon            *
*        2-Mar-90 : - added declarations of celltrips, cellcliq, cellind     *
*                   - changed declarations to use EXTPROC                    *
*       12-Mar-90 : - added changes for Cray version                         *
*       20-Mar-90 : - added changes for THINK version                        *
*       27-Mar-90 : - split SYS_MSDOS into SYS_PCMS4 and SYS_PCMS5           *
*       13-Oct-90 : changes for version 1.6 :                                *
*                   - changed CPUTIME to use HZ on Unix for times()          *
*       14-Oct-90 : - added SYS_APOLLO variant                               *
*       19-Oct-90 : - changed CPUTIME defs for BSDUNIX to avoid conficting   *
*                     declarations of size_t and ptrdiff_t in gcc            *
*       27-Aug-92 : changes for version 1.7 :                                *
*                   - added SYS_IBMC variant                                 *
*                   - removed workaround for bad gcc installation            *
*        5-Jun-93 : changes for version 1.8 :                                *
*                   - changed CRAY version of CPUTIME to use CLK_TCK         *
*                     if HZ could not be found (making 1.7+)                 *
*       30-Jul-93 : - added SYS_ALPHA variant                                *
*       17-Sep-93 : changes for version 1.9 :                                *
*                   - declared adjacencies()                                 *
*       24-Feb-94 : changes for version 1.10 :                               *
*                   - added version SYS_AMIGAAZT (making 1.9+)               *
*       19-Apr-95 : - added C++ prototype wrapper                            *
*                                                                            *
*****************************************************************************/

#include "nauty.h"              /* which includes stdio.h */

extern long seed;               /* declared in naututil.c */

#ifdef __cplusplus
extern "C" {
#endif

EXTPROC(UPROC adjacencies,(graph*,nvector*,nvector*,int,int,int,permutation*,
                      int,boolean,int,int))
EXTPROC(UPROC adjtriang,(graph*,nvector*,nvector*,int,int,int,permutation*,
                      int,boolean,int,int))
EXTPROC(UPROC cellcliq,(graph*,nvector*,nvector*,int,int,int,permutation*,
                      int,boolean,int,int))
EXTPROC(UPROC cellind,(graph*,nvector*,nvector*,int,int,int,permutation*,
                      int,boolean,int,int))
EXTPROC(UPROC cellquads,(graph*,nvector*,nvector*,int,int,int,permutation*,
                      int,boolean,int,int))
EXTPROC(UPROC cellquins,(graph*,nvector*,nvector*,int,int,int,permutation*,
                      int,boolean,int,int))
EXTPROC(UPROC celltrips,(graph*,nvector*,nvector*,int,int,int,permutation*,
                      int,boolean,int,int))
EXTPROC(void cellstarts,(nvector*,int,set*,int,int))
EXTPROC(UPROC cliques,(graph*,nvector*,nvector*,int,int,int,permutation*,
                      int,boolean,int,int))
EXTPROC(void complement,(graph*,int,int))
EXTPROC(void copycomment,(FILE*,FILE*,int))
EXTPROC(UPROC distances,(graph*,nvector*,nvector*,int,int,int,permutation*,
                      int,boolean,int,int))
EXTPROC(void flushline,(FILE*))
EXTPROC(void fixit,(nvector*,nvector*,int*,int,int))
EXTPROC(void getbigcells,(nvector*,int,int,int*,short*,short*,int))
EXTPROC(int getint,(FILE*))
EXTPROC(long hash,(set*,long,int))
EXTPROC(UPROC indsets,(graph*,nvector*,nvector*,int,int,int,permutation*,
                      int,boolean,int,int))
EXTPROC(void mathon,(graph*,int,int,graph*,int,int))
EXTPROC(void putcanon,(FILE*,nvector*,graph*,int,int,int))
EXTPROC(void putdegs,(FILE*,graph*,int,int,int))
EXTPROC(void putgraph,(FILE*,graph*,int,int,int))
EXTPROC(void putmapping,(FILE*,nvector*,int,nvector*,int,int,int))
EXTPROC(void putorbits,(FILE*,nvector*,int,int))
EXTPROC(void putptn,(FILE*,nvector*,nvector*,int,int,int))
EXTPROC(void putquotient,(FILE*,graph*,nvector*,nvector*,int,int,int,int))
EXTPROC(void putset,(FILE*,set*,int*,int,int,boolean))
EXTPROC(UPROC quadruples,(graph*,nvector*,nvector*,int,int,int,permutation*,
                      int,boolean,int,int))
EXTPROC(void rangraph,(graph*,boolean,int,int,int))
EXTPROC(void ranperm,(permutation*,int))
EXTPROC(void readgraph,(FILE*,graph*,boolean,boolean,boolean,int,int,int))
EXTPROC(boolean readinteger,(FILE*,int*))
EXTPROC(void readperm,(FILE*,permutation*,boolean,int))
EXTPROC(void readptn,(FILE*,nvector*,nvector*,int*,boolean,int))
EXTPROC(boolean readstring,(FILE*,char*))
EXTPROC(void relabel,(graph*,nvector*,permutation*,graph*,int,int))
EXTPROC(int setinter,(set*,set*,int))
EXTPROC(int setsize,(set*,int))
EXTPROC(UPROC triples,(graph*,nvector*,nvector*,int,int,int,permutation*,
                      int,boolean,int,int))
EXTPROC(UPROC twopaths,(graph*,nvector*,nvector*,int,int,int,permutation*,
                      int,boolean,int,int))
EXTPROC(void unitptn,(nvector*,nvector*,int*,int))

#ifdef __cplusplus
}
#endif

#define DREADVERSION  NAUTYVERSION

/*-------------------------------------------------------------------------*/

/* This is the place to work around the system dependencies that dreadnaut **
** may run into.  Most of these involve system calls that not all C        **
** implementations provide.                                                **
** Changes for nauty.c or nautil.c (hopefully few) should go in nauty.h.   **
** It is assumed that the SYS_ symbols in nauty.h are defined correctly.   **
** To add a new machine, create a new SYS_ symbol and edit both .h files.  */

/**************************************************************************
(1) dreadnaut prompts to file PROMPTFILE if DOPROMPT(fp) is TRUE, where fp
is the file pointer for the current input file
If you need any extra declarations for this, put then in PROMPTDECLS      */

#define PROMPTFILE stdout

#if (IS_UNIX | IS_PC | SYS_VAXVMS | SYS_MACAZT)
EXTPROC(int isatty,(int))
#define DOPROMPT(fp) (isatty(fileno(fp)) && isatty(fileno(PROMPTFILE)))
#endif

#if  SYS_MACMPW
#include <ioctl.h>
#define DOPROMPT(fp) (ioctl(fileno(fp),FIOINTERACTIVE,NULL) == 0 && \
                      ioctl(fileno(PROMPTFILE),FIOINTERACTIVE,NULL) == 0)
#endif

#if  IS_AMIGA
EXTPROC(int IsInteractive,(int))
#define DOPROMPT(fp) (IsInteractive(fileno(fp)) && \
                      IsInteractive(fileno(PROMPTFILE)))
#endif

#if  (SYS_MACLSC | SYS_MACTHINK | SYS_IBMC | SYS_MISC)
#define DOPROMPT(fp) (curfile==0)        /* stdin yes, others no */
#endif

/**************************************************************************
(2) dreadnaut writes error messages to ERRFILE.  This is allowed to be the
same as PROMPTFILE.   There might be a prior definition in nauty.h.       */

#ifndef ERRFILE
#define ERRFILE stderr
#endif

/**************************************************************************
(3) Dreadnaut allows up to MAXIFILES input files concurrently open,
including stdin.  Allow for up to 4 output files.                         */

#define MAXIFILES 6

/**************************************************************************
(4) Dreadnaut uses OPENOUT(fp,name,append) to open output files other than
stdout.  fp is the file pointer variable, name is a string containing the
name, and append is a boolean specifying whether an existing file is to be
openned for appending to, or a new file created.  The code should leave
fp==NULL if the openning operation failed.                                */

#if  SYS_MACMPW
#include <types.h>
#include <files.h>
extern OSErr GetFInfo(),SetFInfo();
#define OPENOUT(fp,name,append) { FInfo fileinfo;\
    if((fp = fopen(name,(append)?"a":"w")) != NULL &&\
       !(append) && GetFInfo(name,0,&fileinfo) == 0)\
       {fileinfo.fdType = 'TEXT'; SetFInfo(name,0,&fileinfo);}}
#else
#define OPENOUT(fp,name,append) fp = fopen(name,(append)?"a":"w")
#endif

/**************************************************************************
(5) Dreadnaut supposes that CPUTIME is an expression with a double value
equal to the current CPU usage in seconds.  If no fine resolution cpu timer
is available, use a real time function.  If nothing sensible is possible,
leave CPUTIME undefined.   The comma operator is useful here.
CPUDEFS should contain the necessary data declarations, if any.           */

#if (SYS_VAXVMS | SYS_UNIX | SYS_CRAY)
#define CPUDEFS static struct \
   {int u_time,s_time,cu_time,cs_time;} timebuffer;
#endif

#if  SYS_VAXVMS
#define CPUTIME (times(&timebuffer),\
                (timebuffer.u_time + timebuffer.s_time) * 0.01)
#endif

#if  SYS_UNIX
#ifndef HZ
#include <sys/param.h>
#ifndef HZ
#define HZ   60
#endif
#endif
#define CPUTIME (times(&timebuffer),\
                (double)(timebuffer.u_time + timebuffer.s_time) / HZ)
#endif

#if  SYS_CRAY
#ifndef HZ
#include <sys/types.h>
#ifndef HZ
/* #include <sys/times.h> */
#include <time.h>
#ifndef HZ
#ifdef CLK_TCK
#define HZ CLK_TCK
#endif
#ifndef HZ
#define HZ   1000000
#endif
#endif
#endif
#endif
#define CPUTIME (times(&timebuffer),\
                (double)(timebuffer.u_time + timebuffer.s_time) / HZ)
#endif

#if  IS_MAC
#define CPUTIME ((double)*(long*)0x16A * 0.0166666667)
#endif

#if  SYS_AMIGALC
#include <dos.h>
#define CPUDEFS static char clk[8];
#define CPUTIME (getclk(clk),\
            86400.0*clk[3]+3600.0*clk[4]+60.0*clk[5]+1.0*clk[6]+0.01*clk[7])
#endif

#if (SYS_BSDUNIX | SYS_VAXBSD | SYS_APOLLO | SYS_ALPHA)
/* The following definitions were once needed to avoid conficting
declarations in gcc.  It seems it was due to bad installation. */
/* #define size_t SIZ_T_TMP       */
/* #define ptrdiff_t PTRDF_T_TMP  */
#include <sys/time.h>
#include <sys/resource.h>
extern int getrusage();
#define CPUDEFS struct rusage ruse;
#define CPUTIME (getrusage(RUSAGE_SELF,&ruse),\
  ruse.ru_utime.tv_sec + ruse.ru_stime.tv_sec + \
  1e-6 * (ruse.ru_utime.tv_usec + ruse.ru_stime.tv_usec))
/* #undef size_t        -- see above */
/* #undef ptrdiff_t  */
#endif

#if (SYS_PCMS4 | SYS_PCMS5)
#define CPUDEFS struct timeb {\
  long    time;\
  unsigned short millitm;\
  short   timezone;\
  short   dstflag;};\
  extern int ftime();\
  static struct timeb eusage;
#define CPUTIME (ftime(&eusage),\
                (double)eusage.time+(double)eusage.millitm*0.001)
#endif

#if  SYS_PCTURBO
#include <dos.h>
#define CPUDEFS static struct time tbuf;
#define CPUTIME (gettime(&tbuf), (double)tbuf.ti_hour*3600.0 + \
  (double)tbuf.ti_min*60.0 + (double)tbuf.ti_sec + (double)tbuf.ti_hund*0.01)
#endif

#if  SYS_IBMC
#include <time.h>
#define CPUTIME ((double) clock() / CLOCKS_PER_SEC)
#endif

/**************************************************************************
(6) Some implementations don't allow declarations of very large structures
other than dynamically.  The problem cases are the graphs g, canong, savedg
and to a lesser extent the work area workspace.
Alternatively, there may be a limit on the total size of statically
allocated data which prevents MAXN from being very large.
dreadnaut uses ALLOCS and FREES (defined in nauty.h) for g, canong and
savedg always.  To get it to use ALLOCS for workspace and a bunch of smaller
things as well, define DYNALLOC.                                          */

#if (IS_MAC | IS_PC | IS_AMIGA)
#define DYNALLOC 1
#endif

/**************************************************************************
(7) dreadnaut uses random numbers for the 'j' and 's' commands.  The long
int variable "seed" can be used for this.  It is initialised to 1.  If
a more random initialisation is available, define INITSEED to do it.
The expression RAN(k) should be a random integer in the range [0..k-1].
The statistical quality is unimporatant.                                  */

#if SYS_AMIGALC
/* #include <dos.h> -- included above in Section 6. */
#define INITSEED {char clk[8];\
                  getclk(clk);\
                  seed = 12000*clk[5] + 200*clk[6] + 2*clk[7] + 1;}
#endif

#if SYS_AMIGAAZT
#include <libraries/dos.h>
EXTPROC(unsigned long curticks,(void))
#define INITSEED seed = curticks()
#endif

#if  IS_MAC
#define INITSEED seed = 2 * *(long*)0x16A + 1
#endif

#if (SYS_VAXVMS | IS_UNIX | SYS_PCTURBO)
#if !SYS_ALPHA
EXTPROC(long time,(long*))
#endif
#define INITSEED  seed = ((time((long*)NULL)<<1) | 1) & 017777777777L
#endif

#define RAN(k) (((seed=(seed*65539L)&017777777777L)/7)%(k))

/**************************************************************************
(8) dreadnaut provides nauty with enough space to keep data on at least
WORKSIZE automorphisms.  To ensure this, it provides 2 * WORKSIZE * MAXM
setwords.  This number must be at most the size of the largest int.
Allow for command-line definition.                                        */

#ifndef  WORKSIZE
#define WORKSIZE 60
#endif

/**************************************************************************
(9) dreadnaut stops execution with EXIT.                                  */

#if  SYS_VAXVMS
#define EXIT exit(1)
#else
#define EXIT exit(0)
#endif

/**************************************************************************
(10) dreadnaut allows for extra initialization code to be executed when it
starts up.  If anything is needed, put the necesssary decarations in
EXTRADECLS and the initialization code in INITIALIZE.                     */

#if  SYS_MACMPW
#include <ioctl.h>
#define INITIALIZE {if (ioctl(fileno(stdout),FIOINTERACTIVE,0)==0)\
                        setvbuf(stdout,NULL,_IOLBF,BUFSIZ);\
                    if (prompt) putc('\f',PROMPTFILE);}
#endif

#if  IS_AMIGA
#define INITIALIZE  {if (IsInteractive(fileno(stdout))) \
                         setvbuf(stdout,NULL,_IOLBF,BUFSIZ);}
#endif

#if  SYS_IBMC
#define INITIALIZE {setbuf(PROMPTFILE,NULL); setbuf(PROMPTFILE,NULL);}
#endif

/**************************************************************************
(11) Define the symbol NLMAP if '\n' and '\r' are equal.
Leave it undefined otherwise.                                             */

#if  SYS_MACMPW
#define NLMAP
#endif

/**************************************************************************
(12) Define DEFEXT to be a sensible file-name extension for dreadnaut
input files.  This is used only by the dreadnaut '<' command.
If possible, use either ".dre" or ".DRE".                                 */

#define DEFEXT ".dre"

/**************************************************************************
(13) dreadnaut assumes that '\n' is returned by getc at end of line.  It
doesn't matter if '\r' is returned as well, but '\r' by itself won't do.  */

#if  SYS_MACAZT
#define getc agetc
#endif

/**************************************************************************
(etc) Other things that dreadnaut uses: fprintf, fopen, fclose, ungetc,
putc.                                                                     */
