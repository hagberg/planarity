/*
Planarity-Related Graph Algorithms Project
Copyright (c) 1997-2010, John M. Boyer
All rights reserved. Includes a reference implementation of the following:

* John M. Boyer. "Simplified O(n) Algorithms for Planar Graph Embedding,
  Kuratowski Subgraph Isolation, and Related Problems". Ph.D. Dissertation,
  University of Victoria, 2001.

* John M. Boyer and Wendy J. Myrvold. "On the Cutting Edge: Simplified O(n)
  Planarity by Edge Addition". Journal of Graph Algorithms and Applications,
  Vol. 8, No. 3, pp. 241-273, 2004.

* John M. Boyer. "A New Method for Efficiently Generating Planar Graph
  Visibility Representations". In P. Eades and P. Healy, editors,
  Proceedings of the 13th International Conference on Graph Drawing 2005,
  Lecture Notes Comput. Sci., Volume 3843, pp. 508-511, Springer-Verlag, 2006.

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

#include <stdlib.h>

#include "testFramework.h"

char *commands = "pdo234c";

#include "../graphK23Search.h"
#include "../graphK33Search.h"
#include "../graphK4Search.h"
#include "../graphDrawPlanar.h"
#include "../graphColorVertices.h"

/* Forward Declarations of Private functions */
graphP createGraph(char command, int n, int maxe);
int attachAlgorithmExtension(char command, graphP aGraph);
void initBaseTestResult(baseTestResultStruct *pBaseResult);
int initTestResult(testResultP testResult, char command, int n, int maxe);
void releaseTestResult(testResultP testResult);

/***********************************************************************
 tf_AllocateTestFramework()
 ***********************************************************************/

testResultFrameworkP tf_AllocateTestFramework(char command, int n, int maxe)
{
	testResultFrameworkP framework = (testResultFrameworkP) malloc(sizeof(testResultFrameworkStruct));

	if (framework != NULL)
	{
		int i;

		framework->algResultsSize = command == 'a' ? NUMCOMMANDSTOTEST : 1;
		framework->algResults = (testResultP) malloc(framework->algResultsSize * sizeof(testResultStruct));
		for (i=0; i < framework->algResultsSize; i++)
			if (initTestResult(framework->algResults+i,
					command=='a'?commands[i]:command, n, maxe) != OK)
			{
				framework->algResultsSize = i;
				tf_FreeTestFramework(&framework);
				return NULL;
			}
	}

	return framework;
}

/***********************************************************************
 tf_FreeTestFramework()
 ***********************************************************************/

void tf_FreeTestFramework(testResultFrameworkP *pTestFramework)
{
	if (pTestFramework != NULL && *pTestFramework != NULL)
	{
		testResultFrameworkP framework = *pTestFramework;

		if (framework->algResults != NULL)
		{
			int i;
			for (i=0; i < framework->algResultsSize; i++)
				releaseTestResult(framework->algResults+i);

			free((void *) framework->algResults);
			framework->algResults = NULL;
			framework->algResultsSize = 0;
		}

		free(*pTestFramework);
		*pTestFramework = framework = NULL;
	}
}

/***********************************************************************
 tf_GetTestResult()
 ***********************************************************************/

testResultP tf_GetTestResult(testResultFrameworkP framework, char command)
{
	testResultP testResult = NULL;

	if (framework != NULL && framework->algResults != NULL)
	{
		int i;
		for (i=0; i < framework->algResultsSize; i++)
			if (framework->algResults[i].command == command)
				testResult = framework->algResults+i;
	}

	return testResult;
}

/***********************************************************************
 initTestResult()
 ***********************************************************************/

int initTestResult(testResultP testResult, char command, int n, int maxe)
{
	if (testResult != NULL)
	{
		testResult->command = command;
		initBaseTestResult(&testResult->result);
		testResult->edgeResults = NULL;
		testResult->edgeResultsSize = maxe;
		testResult->theGraph = NULL;
		testResult->origGraph = NULL;

		testResult->edgeResults = (baseTestResultStruct *) malloc((maxe+1) * sizeof(baseTestResultStruct));
		if (testResult->edgeResults == NULL)
		{
			releaseTestResult(testResult);
			return NOTOK;
		}
		else
		{
			int j;
			for (j=0; j <= maxe; j++)
				initBaseTestResult(&testResult->edgeResults[j]);
		}

		if ((testResult->theGraph = createGraph(command, n, maxe)) == NULL ||
			(testResult->origGraph = createGraph(command, n, maxe)) == NULL)
		{
			releaseTestResult(testResult);
			return NOTOK;
		}
	}

	return OK;
}

/***********************************************************************
 releaseTestResult()
 Releases all memory resources used by the testResult, but does not
 free the testResult since it points into an array.
 ***********************************************************************/

void releaseTestResult(testResultP testResult)
{
	if (testResult->edgeResults != NULL)
	{
		free(testResult->edgeResults);
		testResult->edgeResults = NULL;
	}
	gp_Free(&(testResult->theGraph));
	gp_Free(&(testResult->origGraph));
}

/***********************************************************************
 initBaseTestResult()
 ***********************************************************************/

void initBaseTestResult(baseTestResultStruct *pBaseResult)
{
	memset(pBaseResult, 0, sizeof(baseTestResultStruct));
}

/***********************************************************************
 createGraph()
 ***********************************************************************/

graphP createGraph(char command, int n, int maxe)
{
	graphP theGraph;
	int numArcs = 2*(maxe > 0 ? maxe : 1);

    if ((theGraph = gp_New()) != NULL)
    {
		if (gp_EnsureArcCapacity(theGraph, numArcs) != OK ||
				gp_InitGraph(theGraph, n) != OK ||
				attachAlgorithmExtension(command, theGraph) != OK)
			gp_Free(&theGraph);
    }

    return theGraph;
}

/***********************************************************************
 attachAlgorithmExtension()
 ***********************************************************************/

int attachAlgorithmExtension(char command, graphP aGraph)
{
    switch (command)
    {
		case 'p' : break;
		case 'd' : gp_AttachDrawPlanar(aGraph);	break;
		case 'o' : break;
		case '2' : gp_AttachK23Search(aGraph); break;
		case '3' : gp_AttachK33Search(aGraph); break;
		case '4' : gp_AttachK4Search(aGraph); break;
		case 'c' : gp_AttachColorVertices(aGraph); break;
		default  : return NOTOK;
    }

    return OK;
}
