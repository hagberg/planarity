#ifndef GRAPH_H
#define GRAPH_H

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

#ifdef __cplusplus
extern "C" {
#endif

#include "graphStructures.h"

#include "graphExtensions.h"

///////////////////////////////////////////////////////////////////////////////
// Definitions for higher-order operations at the vertex, edge and graph levels
///////////////////////////////////////////////////////////////////////////////

graphP	gp_New(void);

int		gp_InitGraph(graphP theGraph, int N);
void	gp_ReinitializeGraph(graphP theGraph);
int		gp_CopyAdjacencyLists(graphP dstGraph, graphP srcGraph);
int		gp_CopyGraph(graphP dstGraph, graphP srcGraph);
graphP	gp_DupGraph(graphP theGraph);

int		gp_CreateRandomGraph(graphP theGraph);
int		gp_CreateRandomGraphEx(graphP theGraph, int numEdges);

void	gp_Free(graphP *pGraph);

int		gp_Read(graphP theGraph, char *FileName);
#define WRITE_ADJLIST   1
#define WRITE_ADJMATRIX 2
#define WRITE_DEBUGINFO 3
int		gp_Write(graphP theGraph, char *FileName, int Mode);

int		gp_IsNeighbor(graphP theGraph, int u, int v);
int		gp_GetNeighborEdgeRecord(graphP theGraph, int u, int v);
int		gp_GetVertexDegree(graphP theGraph, int v);
int		gp_GetVertexInDegree(graphP theGraph, int v);
int		gp_GetVertexOutDegree(graphP theGraph, int v);

int		gp_GetArcCapacity(graphP theGraph);
int		gp_EnsureArcCapacity(graphP theGraph, int requiredArcCapacity);

int		gp_AddEdge(graphP theGraph, int u, int ulink, int v, int vlink);
int     gp_InsertEdge(graphP theGraph, int u, int e_u, int e_ulink,
                                       int v, int e_v, int e_vlink);

void	gp_HideEdge(graphP theGraph, int e);
void	gp_RestoreEdge(graphP theGraph, int e);
int		gp_HideVertex(graphP theGraph, int vertex);
int		gp_DeleteEdge(graphP theGraph, int e, int nextLink);

int		gp_ContractEdge(graphP theGraph, int e);
int		gp_IdentifyVertices(graphP theGraph, int u, int v, int eBefore);
int		gp_RestoreVertices(graphP theGraph);

int		gp_CreateDFSTree(graphP theGraph);
int		gp_SortVertices(graphP theGraph);
int 	gp_LowpointAndLeastAncestor(graphP theGraph);
int		gp_PreprocessForEmbedding(graphP theGraph);

int		gp_Embed(graphP theGraph, int embedFlags);
int		gp_TestEmbedResultIntegrity(graphP theGraph, graphP origGraph, int embedResult);

/* Possible Flags for gp_Embed.  The planar and outerplanar settings are supported
   natively.  The rest require extension modules. */

#define EMBEDFLAGS_PLANAR       1
#define EMBEDFLAGS_OUTERPLANAR  2

#define EMBEDFLAGS_DRAWPLANAR   (4|EMBEDFLAGS_PLANAR)

#define EMBEDFLAGS_SEARCHFORK23 (16|EMBEDFLAGS_OUTERPLANAR)
#define EMBEDFLAGS_SEARCHFORK4  (32|EMBEDFLAGS_OUTERPLANAR)
#define EMBEDFLAGS_SEARCHFORK33 (64|EMBEDFLAGS_PLANAR)

#define EMBEDFLAGS_SEARCHFORK5  (128|EMBEDFLAGS_PLANAR)

#define EMBEDFLAGS_MAXIMALPLANARSUBGRAPH    256
#define EMBEDFLAGS_PROJECTIVEPLANAR         512
#define EMBEDFLAGS_TOROIDAL                 1024

/* If LOGGING is defined, then write to the log, otherwise no-op
   By default, neither release nor DEBUG builds including LOGGING.
   Logging is useful for seeing details of how various algorithms
   handle a particular graph. */

//#define LOGGING
#ifdef LOGGING

#define gp_LogLine _LogLine
#define gp_Log _Log

void _LogLine(char *Line);
void _Log(char *Line);

#define gp_MakeLogStr1 _MakeLogStr1
#define gp_MakeLogStr2 _MakeLogStr2
#define gp_MakeLogStr3 _MakeLogStr3
#define gp_MakeLogStr4 _MakeLogStr4
#define gp_MakeLogStr5 _MakeLogStr5

char *_MakeLogStr1(char *format, int);
char *_MakeLogStr2(char *format, int, int);
char *_MakeLogStr3(char *format, int, int, int);
char *_MakeLogStr4(char *format, int, int, int, int);
char *_MakeLogStr5(char *format, int, int, int, int, int);

#else
#define gp_LogLine(Line)
#define gp_Log(Line)
#define gp_MakeLogStr1(format, one)
#define gp_MakeLogStr2(format, one, two)
#define gp_MakeLogStr3(format, one, two, three)
#define gp_MakeLogStr4(format, one, two, three, four)
#define gp_MakeLogStr5(format, one, two, three, four, five)
#endif

#ifdef __cplusplus
}
#endif

#endif
