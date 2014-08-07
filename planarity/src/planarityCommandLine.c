/*
Planarity-Related Graph Algorithms Project
Copyright (c) 1997-2012, John M. Boyer
All rights reserved. Includes a reference implementation of the following:

* John M. Boyer. "Subgraph Homeomorphism via the Edge Addition Planarity Algorithm".
  Journal of Graph Algorithms and Applications, Vol. 16, no. 2, pp. 381-410, 2012.
  http://www.jgaa.info/16/268.html

* John M. Boyer. "A New Method for Efficiently Generating Planar Graph
  Visibility Representations". In P. Eades and P. Healy, editors,
  Proceedings of the 13th International Conference on Graph Drawing 2005,
  Lecture Notes Comput. Sci., Volume 3843, pp. 508-511, Springer-Verlag, 2006.

* John M. Boyer and Wendy J. Myrvold. "On the Cutting Edge: Simplified O(n)
  Planarity by Edge Addition". Journal of Graph Algorithms and Applications,
  Vol. 8, No. 3, pp. 241-273, 2004.
  http://www.jgaa.info/08/91.html

* John M. Boyer. "Simplified O(n) Algorithms for Planar Graph Embedding,
  Kuratowski Subgraph Isolation, and Related Problems". Ph.D. Dissertation,
  University of Victoria, 2001.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice, this
  list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.

* Neither the name of the Planarity-Related Graph Algorithms Project nor the names
  of its contributors may be used to endorse or promote products derived from this
  software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "planarity.h"

#include <unistd.h>

int callNauty(int argc, char *argv[]);
int runQuickRegressionTests(int argc, char *argv[]);
int callRandomGraphs(int argc, char *argv[]);
int callSpecificGraph(int argc, char *argv[]);
int callRandomMaxPlanarGraph(int argc, char *argv[]);
int callRandomNonplanarGraph(int argc, char *argv[]);

/****************************************************************************
 Command Line Processor
 ****************************************************************************/

int commandLine(int argc, char *argv[])
{
	int Result = OK;

	if (argc >= 3 && strcmp(argv[2], "-q") == 0)
		quietMode = 'y';

	if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "-help") == 0)
	{
		Result = helpMessage(argc >= 3 ? argv[2] : NULL);
	}

	else if (strcmp(argv[1], "-i") == 0 || strcmp(argv[1], "-info") == 0)
	{
		Result = helpMessage(argv[1]);
	}

	else if (strcmp(argv[1], "-gen") == 0 || strcmp(argv[1], "-gens") == 0)
		Result = callNauty(argc, argv);

	else if (strcmp(argv[1], "-test") == 0)
		Result = runQuickRegressionTests(argc, argv);

	else if (strcmp(argv[1], "-r") == 0)
		Result = callRandomGraphs(argc, argv);

	else if (strcmp(argv[1], "-s") == 0)
		Result = callSpecificGraph(argc, argv);

	else if (strcmp(argv[1], "-rm") == 0)
		Result = callRandomMaxPlanarGraph(argc, argv);

	else if (strcmp(argv[1], "-rn") == 0)
		Result = callRandomNonplanarGraph(argc, argv);

	else
	{
		ErrorMessage("Unsupported command line.  Here is the help for this program.\n");
		helpMessage(NULL);
		Result = NOTOK;
	}

	return Result == OK ? 0 : (Result == NONEMBEDDABLE ? 1 : -1);
}

/****************************************************************************
 Legacy Command Line Processor from version 1.x
 ****************************************************************************/

int legacyCommandLine(int argc, char *argv[])
{
graphP theGraph = gp_New();
int Result;

	Result = gp_Read(theGraph, argv[1]);
	if (Result != OK)
	{
		if (Result != NONEMBEDDABLE)
		{
			if (strlen(argv[1]) > MAXLINE - 100)
				sprintf(Line, "Failed to read graph\n");
			else
				sprintf(Line, "Failed to read graph %s\n", argv[1]);
			ErrorMessage(Line);
			return -2;
		}
	}

	Result = gp_Embed(theGraph, EMBEDFLAGS_PLANAR);

	if (Result == OK)
	{
		gp_SortVertices(theGraph);
		gp_Write(theGraph, argv[2], WRITE_ADJLIST);
	}

	else if (Result == NONEMBEDDABLE)
	{
		if (argc >= 5 && strcmp(argv[3], "-n")==0)
		{
			gp_SortVertices(theGraph);
			gp_Write(theGraph, argv[4], WRITE_ADJLIST);
		}
	}
	else
		Result = NOTOK;

	gp_Free(&theGraph);

	// In the legacy 1.x versions, OK/NONEMBEDDABLE was 0 and NOTOK was -2
	return Result==OK || Result==NONEMBEDDABLE ? 0 : -2;
}

/****************************************************************************
 WriteTestFiles()
 A one-time-run piece of code that generates batch files which perform the
 incremental tests of all 12 vertex graphs.

 The format of the command line is:
 planarity -gen -q -a n j j 12 i > TestResult_n_j_j_12_i.txt

 The n is for the number of vertices.
 The j j is the number of edges, between 0 and n(n-1)/2.
 The mod divides the problem into partitions that can be run on separate threads.
 The i is a partition number between 0 and mod-1. Each test batch file receives
 a number i, and within it are all the command lines for that partition to test
 each number of edges.
 The numbers of edges are tested separately because the implementation makes
 a smaller data structure for graphs that are of a guaranteed maximum size.
 ****************************************************************************/

void WriteTestFiles(int n, int mod)
{
	char filename[64];
	FILE *outfile;
	int k, e;
	int maxe=n*(n-1)/2;

	for (k = 0; k < mod; k++)
	{
		sprintf(filename, "test_n%02d\\test_n%02d_mod%02d.bat", n, n, k);
		outfile = fopen(filename, "wt");
		if (outfile == NULL)
		{
			printf("Error creating test file %s\nRemember to create directory Test_n%02d\n", filename, n);
			return;
		}

		for (e = 0; e <= maxe; e++)
		{
			sprintf(filename, "results\\result_%02d_%02d_%02d_%02d_%02d.txt", n, e, e, mod, k);
			fprintf(outfile, "..\\planarity -gen -a %d %d %d %d %d > %s\n", n, e, e, mod, k, filename);
		}
		fclose(outfile);
	}

	printf("Created test files. Remember to create 'results' in test subdirectory\n");
}

/****************************************************************************
 Call Nauty's MAKEG graph generator.
 ****************************************************************************/

// 'planarity -gen [-q] C {ncl}': exhaustive tests with graphs generated by makeg,
// from McKay's Nauty package
// makeg [-c -t -b] [-d<max>] n [mine [maxe [mod res]]]

//// These externs can be obtained for debugging
////extern unsigned long numGraphs;
////extern unsigned long numErrors;
////extern unsigned long numOKs;

int callNauty(int argc, char *argv[])
{
	char command;
	int numArgs, argsOffset, i;
	char *args[12];
	int result;
	platform_time start, end;

//	WriteTestFiles(11, 12);
//	WriteTestFiles(12, 12);

	if (argc < 4 || argc > 12)
		return -1;

	// Determine the offset of the arguments after command C
	argsOffset = (argv[2][0] == '-' && argv[2][1] == 'q') ? 3 : 2;

	// Obtain the command C
	command = argv[argsOffset][1];

	// Same number of args, except exclude -gen, the optional -q, and the command C
	numArgs = argc-argsOffset;

	// Change 0th arg from planarity to makeg
	args[0] = "makeg";

	// Copy the rest of the args from argv, except -gen and command C
	for (i=1; i < numArgs; i++)
		args[i] = argv[i+argsOffset];

	// Generate order N graphs of all sizes requested on command line
    platform_GetTime(start);
	result = makeg_main(command, numArgs, args) == 0 ? 0 : -1;
	platform_GetTime(end);

	printf("\nTotal time = %.3lf seconds\n", platform_GetDuration(start,end));
	return result;
}

/****************************************************************************
 Quick regression test
 ****************************************************************************/

int runNautyTests(int argc, char *argv[]);
int runSpecificGraphTests();
int runSpecificGraphTest(char *command, char *infileName);

int runQuickRegressionTests(int argc, char *argv[])
{
	if (runSpecificGraphTests() < 0)
		return -1;

	return runNautyTests(argc, argv);
}

int runSpecificGraphTests()
{
	char origDir[2049];
	int retVal = 0;

	if (!getcwd(origDir, 2048))
		return -1;

	if (chdir("samples") != 0)
	{
		if (chdir("..") != 0 || chdir("samples") != 0)
		{
			// Warn but give success result
			printf("WARNING: Unable to change to samples directory to run tests on samples.\n");
			return 0;
		}
	}

#if NIL == 0
	if (runSpecificGraphTest("-p", "maxPlanar5.txt") < 0)
		retVal = -1;

	if (runSpecificGraphTest("-d", "maxPlanar5.txt") < 0)
		retVal = -1;

	if (runSpecificGraphTest("-d", "drawExample.txt") < 0)
		retVal = -1;

	if (runSpecificGraphTest("-p", "Petersen.txt") < 0)
		retVal = -1;

	if (runSpecificGraphTest("-o", "Petersen.txt") < 0)
		retVal = -1;

	if (runSpecificGraphTest("-2", "Petersen.txt") < 0)
		retVal = -1;

	if (runSpecificGraphTest("-3", "Petersen.txt") < 0)
		retVal = -1;

	if (runSpecificGraphTest("-4", "Petersen.txt") < 0)
		retVal = -1;

	if (runSpecificGraphTest("-c", "maxPlanar5.txt") < 0)
		retVal = -1;

	if (runSpecificGraphTest("-c", "Petersen.txt") < 0)
		retVal = -1;

	if (runSpecificGraphTest("-c", "drawExample.txt") < 0)
		retVal = -1;
#endif

	if (runSpecificGraphTest("-p", "maxPlanar5.0-based.txt") < 0)
		retVal = -1;

	if (runSpecificGraphTest("-d", "maxPlanar5.0-based.txt") < 0)
		retVal = -1;

	if (runSpecificGraphTest("-d", "drawExample.0-based.txt") < 0)
		retVal = -1;

	if (runSpecificGraphTest("-p", "Petersen.0-based.txt") < 0)
		retVal = -1;

	if (runSpecificGraphTest("-o", "Petersen.0-based.txt") < 0)
		retVal = -1;

	if (runSpecificGraphTest("-2", "Petersen.0-based.txt") < 0)
		retVal = -1;

	if (runSpecificGraphTest("-3", "Petersen.0-based.txt") < 0)
		retVal = -1;

	if (runSpecificGraphTest("-4", "Petersen.0-based.txt") < 0)
		retVal = -1;

	if (runSpecificGraphTest("-c", "maxPlanar5.0-based.txt") < 0)
		retVal = -1;

	if (runSpecificGraphTest("-c", "Petersen.0-based.txt") < 0)
		retVal = -1;

	if (runSpecificGraphTest("-c", "drawExample.0-based.txt") < 0)
		retVal = -1;

	if (retVal == 0)
		printf("Tests of all specific graphs succeeded\n");

	chdir(origDir);
    FlushConsole(stdout);
	return retVal;
}

int runSpecificGraphTest(char *command, char *infileName)
{
	char *commandLine[] = {
			"planarity", "-s", "C", "infile", "outfile", "outfile2"
	};
	char *outfileName = ConstructPrimaryOutputFilename(infileName, NULL, command[1]);
	char *outfile2Name = "";
	char *testfileName = strdup(outfileName);
	int Result = 0;

	if (testfileName == NULL)
		return -1;

	outfileName = strdup(strcat(outfileName, ".test.txt"));
	if (outfileName == NULL)
	{
		free(testfileName);
		return -1;
	}

	// 'planarity -s [-q] C I O [O2]': Specific graph
	commandLine[2] = command;
	commandLine[3] = infileName;
	commandLine[4] = outfileName;
	commandLine[5] = outfile2Name;

	Result = callSpecificGraph(6, commandLine);
	if (Result == OK || Result == NONEMBEDDABLE)
		Result = 0;
	else
	{
		ErrorMessage("Test failed (graph processor returned failure result).\n");
		Result = -1;
	}

	if (Result == 0)
	{
		if (FilesEqual(testfileName, outfileName) == TRUE)
		{
			Message("Test succeeded (result equal to exemplar).\n");
			unlink(outfileName);
		}
		else
		{
			ErrorMessage("Test failed (result not equal to exemplar).\n");
			Result = -1;
		}
	}

	// For graph drawing, secondary file is outfileName + ".render.txt"

	if (command[1] == 'd' && Result == 0)
	{
		outfile2Name = ConstructPrimaryOutputFilename(NULL, outfileName, command[1]);
		free(outfileName);
		outfileName = strdup(strcat(outfile2Name, ".render.txt"));

		free(testfileName);
		testfileName = ConstructPrimaryOutputFilename(infileName, NULL, command[1]);
		testfileName = strdup(strcat(testfileName, ".render.txt"));

		if (Result == 0)
		{
			if (FilesEqual(testfileName, outfileName) == TRUE)
			{
				Message("Test succeeded (secondary result equal to exemplar).\n");
				unlink(outfileName);
			}
			else
			{
				ErrorMessage("Test failed (secondary result not equal to exemplar).\n");
				Result = -1;
			}
		}
	}

	Message("\n");

	free(outfileName);
	free(testfileName);
	return Result;
}

#include "nauty/testFramework.h"
extern int unittestMode;
extern int errorFound;
//extern testResultFrameworkP testFramework;

int runNautyTests(int argc, char *argv[])
{
	platform_time start, end;
	int success = TRUE;
	char *commandLine[4] = {
			"planarity", "-gen", "-a", "9"
	};

	platform_GetTime(start);

	// If a single command test, then get the command, otherwise restore 'a' for all
	if (argc == 4 || (argc == 3 && quietMode != 'y'))
		commandLine[2] = argv[2 + (quietMode == 'y' ? 1 : 0)];
	else
		commandLine[2] = "-a";

	// Go to unit test mode, tell the user what is happening
	unittestMode = 1;
	printf("Testing all %s vertex graphs\n", commandLine[3]);

	// Run the test and obtain the result
	if (callNauty(4, commandLine) != 0)
		success = FALSE;
	else
	{
		if (errorFound)
			success = FALSE;
		errorFound = 0;
	}

	// Get out of unit test mode
	unittestMode = 0;

	// Report the time duration and the result
	platform_GetTime(end);
    printf("\nFinished all processing in %.3lf seconds.\n", platform_GetDuration(start,end));
    printf("Status = %s\n", success ? "SUCCESS" : "ERROR");

	return success ? 0 : -1;
}

/****************************************************************************
 callRandomGraphs()
 ****************************************************************************/

// 'planarity -r [-q] C K N': Random graphs
int callRandomGraphs(int argc, char *argv[])
{
	char Choice = 0;
	int offset = 0, NumGraphs, SizeOfGraphs;

	if (argc < 5)
		return -1;

	if (argv[2][0] == '-' && (Choice = argv[2][1]) == 'q')
	{
		Choice = argv[3][1];
		if (argc < 6)
			return -1;
		offset = 1;
	}

	NumGraphs = atoi(argv[3+offset]);
	SizeOfGraphs = atoi(argv[4+offset]);

    return RandomGraphs(Choice, NumGraphs, SizeOfGraphs);
}

/****************************************************************************
 callSpecificGraph()
 ****************************************************************************/

// 'planarity -s [-q] C I O [O2]': Specific graph
int callSpecificGraph(int argc, char *argv[])
{
	char Choice=0, *infileName=NULL, *outfileName=NULL, *outfile2Name=NULL;
	int offset = 0;

	if (argc < 5)
		return -1;

	if (argv[2][0] == '-' && (Choice = argv[2][1]) == 'q')
	{
		Choice = argv[3][1];
		if (argc < 6)
			return -1;
		offset = 1;
	}

	infileName = argv[3+offset];
	outfileName = argv[4+offset];
	if (argc == 6+offset)
	    outfile2Name = argv[5+offset];

	return SpecificGraph(Choice, infileName, outfileName, outfile2Name);
}

/****************************************************************************
 callRandomMaxPlanarGraph()
 ****************************************************************************/

// 'planarity -rm [-q] N O [O2]': Maximal planar random graph
int callRandomMaxPlanarGraph(int argc, char *argv[])
{
	int offset = 0, numVertices;
	char *outfileName = NULL, *outfile2Name = NULL;

	if (argc < 4)
		return -1;

	if (argv[2][0] == '-' && argv[2][1] == 'q')
	{
		if (argc < 5)
			return -1;
		offset = 1;
	}

	numVertices = atoi(argv[2+offset]);
	outfileName = argv[3+offset];
	if (argc == 5+offset)
	    outfile2Name = argv[4+offset];

	return RandomGraph('p', 0, numVertices, outfileName, outfile2Name);
}

/****************************************************************************
 callRandomNonplanarGraph()
 ****************************************************************************/

// 'planarity -rn [-q] N O [O2]': Non-planar random graph (maximal planar plus edge)
int callRandomNonplanarGraph(int argc, char *argv[])
{
	int offset = 0, numVertices;
	char *outfileName = NULL, *outfile2Name = NULL;

	if (argc < 4)
		return -1;

	if (argv[2][0] == '-' && argv[2][1] == 'q')
	{
		if (argc < 5)
			return -1;
		offset = 1;
	}

	numVertices = atoi(argv[2+offset]);
	outfileName = argv[3+offset];
	if (argc == 5+offset)
	    outfile2Name = argv[4+offset];

	return RandomGraph('p', 1, numVertices, outfileName, outfile2Name);
}
