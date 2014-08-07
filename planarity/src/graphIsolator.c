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

#define GRAPHISOLATOR_C

#include "graph.h"

/* Imported functions */

extern void _ClearVisitedFlags(graphP);

extern int  _GetNeighborOnExtFace(graphP theGraph, int curVertex, int *pPrevLink);
extern int  _JoinBicomps(graphP theGraph);

extern int _ChooseTypeOfNonplanarityMinor(graphP theGraph, int v, int R);

/* Private function declarations (exported within system) */

int _IsolateKuratowskiSubgraph(graphP theGraph, int v, int R);

int  _FindUnembeddedEdgeToAncestor(graphP theGraph, int cutVertex,
                                   int *pAncestor, int *pDescendant);
int  _FindUnembeddedEdgeToCurVertex(graphP theGraph, int cutVertex,
                                    int *pDescendant);
int  _FindUnembeddedEdgeToSubtree(graphP theGraph, int ancestor,
                                  int SubtreeRoot, int *pDescendant);

int  _MarkPathAlongBicompExtFace(graphP theGraph, int startVert, int endVert);

int  _AddAndMarkEdge(graphP theGraph, int ancestor, int descendant);
void _AddBackEdge(graphP theGraph, int ancestor, int descendant);
int  _DeleteUnmarkedVerticesAndEdges(graphP theGraph);

int  _InitializeIsolatorContext(graphP theGraph);

int  _IsolateMinorA(graphP theGraph);
int  _IsolateMinorB(graphP theGraph);
int  _IsolateMinorC(graphP theGraph);
int  _IsolateMinorD(graphP theGraph);
int  _IsolateMinorE(graphP theGraph);

int  _IsolateMinorE1(graphP theGraph);
int  _IsolateMinorE2(graphP theGraph);
int  _IsolateMinorE3(graphP theGraph);
int  _IsolateMinorE4(graphP theGraph);

int  _GetLeastAncestorConnection(graphP theGraph, int cutVertex);
int  _MarkDFSPathsToDescendants(graphP theGraph);
int  _AddAndMarkUnembeddedEdges(graphP theGraph);

/****************************************************************************
 gp_IsolateKuratowskiSubgraph()
 ****************************************************************************/

int  _IsolateKuratowskiSubgraph(graphP theGraph, int v, int R)
{
int  RetVal;

/* A subgraph homeomorphic to K_{3,3} or K_5 will be isolated by using the visited
   flags, set=keep edge/vertex and clear=omit. Here we initialize to omit all, then we
   subsequently set visited on all edges and vertices in the homeomorph. */

	 _ClearVisitedFlags(theGraph);

/* Next, we determine which of the non-planarity Minors was encountered
        and the principal bicomp on which the isolator will focus attention. */

     if (_ChooseTypeOfNonplanarityMinor(theGraph, v, R) != OK)
         return NOTOK;

     if (_InitializeIsolatorContext(theGraph) != OK)
         return NOTOK;

/* Call the appropriate isolator */

     if (theGraph->IC.minorType & MINORTYPE_A)
         RetVal = _IsolateMinorA(theGraph);
     else if (theGraph->IC.minorType & MINORTYPE_B)
         RetVal = _IsolateMinorB(theGraph);
     else if (theGraph->IC.minorType & MINORTYPE_C)
         RetVal = _IsolateMinorC(theGraph);
     else if (theGraph->IC.minorType & MINORTYPE_D)
         RetVal = _IsolateMinorD(theGraph);
     else if (theGraph->IC.minorType & MINORTYPE_E)
         RetVal = _IsolateMinorE(theGraph);
     else
    	 RetVal = NOTOK;

/* Delete the unmarked edges and vertices, and return */

     if (RetVal == OK)
         RetVal = _DeleteUnmarkedVerticesAndEdges(theGraph);

     return RetVal;
}

/****************************************************************************
 _InitializeIsolatorContext()
 ****************************************************************************/

int  _InitializeIsolatorContext(graphP theGraph)
{
isolatorContextP IC = &theGraph->IC;

/* Obtains the edges connecting X and Y to ancestors of the current vertex */

     if (_FindUnembeddedEdgeToAncestor(theGraph, IC->x, &IC->ux, &IC->dx) != TRUE ||
         _FindUnembeddedEdgeToAncestor(theGraph, IC->y, &IC->uy, &IC->dy) != TRUE)
         return NOTOK;

/* For Minor B, we seek the last pertinent child biconnected component, which
     is also future pertinent, and obtain the DFS child in its root edge.
     This child is the subtree root containing vertices with connections to
     both the current vertex and an ancestor of the current vertex. */

     if (theGraph->IC.minorType & MINORTYPE_B)
     {
    	 int SubtreeRoot = gp_GetVertexLastPertinentRootChild(theGraph, IC->w);

         IC->uz = gp_GetVertexLowpoint(theGraph, SubtreeRoot);

         if (_FindUnembeddedEdgeToSubtree(theGraph, IC->v, SubtreeRoot, &IC->dw) != TRUE ||
             _FindUnembeddedEdgeToSubtree(theGraph, IC->uz, SubtreeRoot, &IC->dz) != TRUE)
             return NOTOK;
     }

/* For all other minors, we obtain an unembedded connecting the current vertex to the
 	 pertinent vertex W, and for minor E we collect the additional unembedded ancestor
 	 connection for the future pertinent vertex Z. */

     else
     {
         if (_FindUnembeddedEdgeToCurVertex(theGraph, IC->w, &IC->dw) != TRUE)
             return NOTOK;

         if (theGraph->IC.minorType & MINORTYPE_E)
             if (_FindUnembeddedEdgeToAncestor(theGraph, IC->z, &IC->uz, &IC->dz) != TRUE)
                 return NOTOK;
     }

     return OK;
}

/****************************************************************************
 _IsolateMinorA(): Isolate a K3,3 homeomorph
 ****************************************************************************/

int  _IsolateMinorA(graphP theGraph)
{
isolatorContextP IC = &theGraph->IC;

     if (_MarkPathAlongBicompExtFace(theGraph, IC->r, IC->r) != OK ||
         theGraph->functions.fpMarkDFSPath(theGraph, MIN(IC->ux, IC->uy), IC->r) != OK ||
         _MarkDFSPathsToDescendants(theGraph) != OK ||
         _JoinBicomps(theGraph) != OK ||
         _AddAndMarkUnembeddedEdges(theGraph) != OK)
         return NOTOK;

     return OK;
}

/****************************************************************************
 _IsolateMinorB(): Isolate a K3,3 homeomorph
 ****************************************************************************/

int  _IsolateMinorB(graphP theGraph)
{
isolatorContextP IC = &theGraph->IC;

     if (_MarkPathAlongBicompExtFace(theGraph, IC->r, IC->r) != OK ||
         theGraph->functions.fpMarkDFSPath(theGraph, MIN3(IC->ux,IC->uy,IC->uz),
                                    MAX3(IC->ux,IC->uy,IC->uz)) != OK ||
         _MarkDFSPathsToDescendants(theGraph) != OK ||
         _JoinBicomps(theGraph) != OK ||
         _AddAndMarkUnembeddedEdges(theGraph) != OK)
         return NOTOK;

     return OK;
}

/****************************************************************************
 _IsolateMinorC(): Isolate a K3,3 homeomorph
 ****************************************************************************/

int  _IsolateMinorC(graphP theGraph)
{
isolatorContextP IC = &theGraph->IC;

     if (gp_GetVertexObstructionType(theGraph, IC->px) == VERTEX_OBSTRUCTIONTYPE_HIGH_RXW)
     {
     int highY = gp_GetVertexObstructionType(theGraph, IC->py) == VERTEX_OBSTRUCTIONTYPE_HIGH_RYW
                 ? IC->py : IC->y;
         if (_MarkPathAlongBicompExtFace(theGraph, IC->r, highY) != OK)
             return NOTOK;
     }
     else
     {
         if (_MarkPathAlongBicompExtFace(theGraph, IC->x, IC->r) != OK)
             return NOTOK;
     }

     // Note: The x-y path is already marked, due to identifying the type of non-planarity minor
     if (_MarkDFSPathsToDescendants(theGraph) != OK ||
         theGraph->functions.fpMarkDFSPath(theGraph, MIN(IC->ux, IC->uy), IC->r) != OK ||
         _JoinBicomps(theGraph) != OK ||
         _AddAndMarkUnembeddedEdges(theGraph) != OK)
         return NOTOK;

     return OK;
}

/****************************************************************************
 _IsolateMinorD(): Isolate a K3,3 homeomorph
 ****************************************************************************/

int  _IsolateMinorD(graphP theGraph)
{
isolatorContextP IC = &theGraph->IC;

	 // Note: The x-y and v-z paths are already marked, due to identifying the type of non-planarity minor
     if (_MarkPathAlongBicompExtFace(theGraph, IC->x, IC->y) != OK ||
         theGraph->functions.fpMarkDFSPath(theGraph, MIN(IC->ux, IC->uy), IC->r) != OK ||
         _MarkDFSPathsToDescendants(theGraph) != OK ||
         _JoinBicomps(theGraph) != OK ||
         _AddAndMarkUnembeddedEdges(theGraph) != OK)
         return NOTOK;

     return OK;
}

/****************************************************************************
 _IsolateMinorE()
 ****************************************************************************/

int  _IsolateMinorE(graphP theGraph)
{
isolatorContextP IC = &theGraph->IC;

/* Minor E1: Isolate a K3,3 homeomorph */

     if (IC->z != IC->w)
         return _IsolateMinorE1(theGraph);

/* Minor E2: Isolate a K3,3 homeomorph */

     if (IC->uz > MAX(IC->ux, IC->uy))
         return _IsolateMinorE2(theGraph);

/* Minor E3: Isolate a K3,3 homeomorph */

     if (IC->uz < MAX(IC->ux, IC->uy) && IC->ux != IC->uy)
         return _IsolateMinorE3(theGraph);

/* Minor E4: Isolate a K3,3 homeomorph */

     else if (IC->x != IC->px || IC->y != IC->py)
         return _IsolateMinorE4(theGraph);

/* Minor E: Isolate a K5 homeomorph */

     // Note: The x-y path is already marked, due to identifying the type of non-planarity minor
     if (_MarkPathAlongBicompExtFace(theGraph, IC->r, IC->r) != OK ||
         theGraph->functions.fpMarkDFSPath(theGraph, MIN3(IC->ux, IC->uy, IC->uz), IC->r) != OK ||
         _MarkDFSPathsToDescendants(theGraph) != OK ||
         _JoinBicomps(theGraph) != OK ||
         _AddAndMarkUnembeddedEdges(theGraph) != OK)
         return NOTOK;

     return OK;
}

/****************************************************************************
 _IsolateMinorE1()

 Reduce to Minor C if the vertex Z responsible for external activity
 below the X-Y path does not equal the pertinent vertex W.
 ****************************************************************************/

int  _IsolateMinorE1(graphP theGraph)
{
isolatorContextP IC = &theGraph->IC;

     if (gp_GetVertexObstructionType(theGraph, IC->z) == VERTEX_OBSTRUCTIONTYPE_LOW_RXW)
     {
         gp_ResetVertexObstructionType(theGraph, IC->px, VERTEX_OBSTRUCTIONTYPE_HIGH_RXW);
         IC->x=IC->z; IC->ux=IC->uz; IC->dx=IC->dz;
     }
     else if (gp_GetVertexObstructionType(theGraph, IC->z) == VERTEX_OBSTRUCTIONTYPE_LOW_RYW)
     {
         gp_ResetVertexObstructionType(theGraph, IC->py, VERTEX_OBSTRUCTIONTYPE_HIGH_RYW);
         IC->y=IC->z; IC->uy=IC->uz; IC->dy=IC->dz;
     }
     else return NOTOK;

     // Note: The x-y path is already marked, due to identifying E as the type of non-planarity minor,
     // but the x-y path is also included in minor C, so we let it stay marked since the minor C
     // isolator also assumes the x-y path has been marked by non-planarity minor type identification
     IC->z = IC->uz = IC->dz = NIL;
     theGraph->IC.minorType ^= MINORTYPE_E;
     theGraph->IC.minorType |= (MINORTYPE_C|MINORTYPE_E1);
     return _IsolateMinorC(theGraph);
}

/****************************************************************************
 _IsolateMinorE2()

 If uZ (which is the ancestor of v that is adjacent to Z) is a
 descendant of both uY and uX, then we reduce to Minor A
 ****************************************************************************/

int  _IsolateMinorE2(graphP theGraph)
{
isolatorContextP IC = &theGraph->IC;

     // Note: The x-y path was already marked, due to identifying E as the type of non-planarity minor,
     // but we're reducing to Minor A, which does not include the x-y path, so the visited flags are
     // cleared as a convenient, if somewhat wasteful, way to clear the marking on the x-y path
     _ClearVisitedFlags(theGraph);

     IC->v = IC->uz;
     IC->dw = IC->dz;
     IC->z = IC->uz = IC->dz = NIL;

     theGraph->IC.minorType ^= MINORTYPE_E;
     theGraph->IC.minorType |= (MINORTYPE_A|MINORTYPE_E2);
     return _IsolateMinorA(theGraph);
}

/****************************************************************************
 _IsolateMinorE3()
 ****************************************************************************/

int  _IsolateMinorE3(graphP theGraph)
{
isolatorContextP IC = &theGraph->IC;

     if (IC->ux < IC->uy)
     {
         if (_MarkPathAlongBicompExtFace(theGraph, IC->r, IC->px) != OK ||
             _MarkPathAlongBicompExtFace(theGraph, IC->w, IC->y) != OK)
             return NOTOK;
     }
     else
     {
         if (_MarkPathAlongBicompExtFace(theGraph, IC->x, IC->w) != OK ||
             _MarkPathAlongBicompExtFace(theGraph, IC->py, IC->r) != OK)
             return NOTOK;
     }

     // Note: The x-y path is already marked, due to identifying E as the type of non-planarity minor
     if (theGraph->functions.fpMarkDFSPath(theGraph, MIN3(IC->ux, IC->uy, IC->uz), IC->r) != OK ||
         _MarkDFSPathsToDescendants(theGraph) != OK ||
         _JoinBicomps(theGraph) != OK ||
         _AddAndMarkUnembeddedEdges(theGraph) != OK)
         return NOTOK;

     theGraph->IC.minorType |= MINORTYPE_E3;
     return OK;
}

/****************************************************************************
 _IsolateMinorE4()
 ****************************************************************************/

int  _IsolateMinorE4(graphP theGraph)
{
isolatorContextP IC = &theGraph->IC;

     if (IC->px != IC->x)
     {
         if (_MarkPathAlongBicompExtFace(theGraph, IC->r, IC->w) != OK ||
             _MarkPathAlongBicompExtFace(theGraph, IC->py, IC->r) != OK)
             return NOTOK;
     }
     else
     {
         if (_MarkPathAlongBicompExtFace(theGraph, IC->r, IC->px) != OK ||
             _MarkPathAlongBicompExtFace(theGraph, IC->w, IC->r) != OK)
             return NOTOK;
     }

     // Note: The x-y path is already marked, due to identifying E as the type of non-planarity minor
     if (theGraph->functions.fpMarkDFSPath(theGraph, MIN3(IC->ux, IC->uy, IC->uz),
                                    MAX3(IC->ux, IC->uy, IC->uz)) != OK ||
         _MarkDFSPathsToDescendants(theGraph) != OK ||
         _JoinBicomps(theGraph) != OK ||
         _AddAndMarkUnembeddedEdges(theGraph) != OK)
         return NOTOK;

     theGraph->IC.minorType |= MINORTYPE_E4;
     return OK;
}

/****************************************************************************
 _GetLeastAncestorConnection()

 This function searches for an ancestor of the current vertex v adjacent by a
 cycle edge to the given cutVertex or one of its DFS descendants appearing in
 a separated bicomp. The given cutVertex is assumed to be future pertinent
 such that either the leastAncestor or the lowpoint of a separated DFS child
 is less than v.  We obtain the minimum possible connection from the cutVertex
 to an ancestor of v.
 ****************************************************************************/

int  _GetLeastAncestorConnection(graphP theGraph, int cutVertex)
{
	int child;
	int ancestor = gp_GetVertexLeastAncestor(theGraph, cutVertex);

	child = gp_GetVertexFuturePertinentChild(theGraph, cutVertex);
	while (gp_IsVertex(child))
	{
		if (gp_IsSeparatedDFSChild(theGraph, child) &&
			ancestor > gp_GetVertexLowpoint(theGraph, child))
			ancestor = gp_GetVertexLowpoint(theGraph, child);

		child = gp_GetVertexNextDFSChild(theGraph, cutVertex, child);
	}

    return ancestor;
}

/****************************************************************************
 _FindUnembeddedEdgeToAncestor()

 This function searches for an ancestor of the current vertex v adjacent by a
 cycle edge to the given cutVertex or one of its DFS descendants appearing in
 a separated bicomp.

 The given cutVertex is assumed to be future pertinent such that either the
 leastAncestor or the lowpoint of a separated DFS child is less than v.
 We obtain the minimum possible connection from the cutVertex to an ancestor
 of v, then compute the descendant accordingly.

 Returns TRUE if found, FALSE otherwise.
 ****************************************************************************/

int  _FindUnembeddedEdgeToAncestor(graphP theGraph, int cutVertex,
                                   int *pAncestor, int *pDescendant)
{
 	int child, foundChild;
 	int ancestor = gp_GetVertexLeastAncestor(theGraph, cutVertex);

 	child = gp_GetVertexFuturePertinentChild(theGraph, cutVertex);
 	foundChild = NIL;
 	while (gp_IsVertex(child))
 	{
 		if (gp_IsSeparatedDFSChild(theGraph, child) &&
 			ancestor > gp_GetVertexLowpoint(theGraph, child))
 		{
 			ancestor = gp_GetVertexLowpoint(theGraph, child);
 			foundChild = child;
 		}
 		child = gp_GetVertexNextDFSChild(theGraph, cutVertex, child);
 	}

 	*pAncestor = ancestor;

 	// If the least ancestor connection was direct, then return the cutVertex as the descendant
 	if (ancestor == gp_GetVertexLeastAncestor(theGraph, cutVertex))
 	{
        *pDescendant = cutVertex;
        return TRUE;
 	}

 	// Otherwise find the descendant based on the separated child with least lowpoint
    return _FindUnembeddedEdgeToSubtree(theGraph, *pAncestor, foundChild, pDescendant);
}

/****************************************************************************
 _FindUnembeddedEdgeToCurVertex()

 Given the current vertex v, we search for an edge connecting v to either
 a given pertinent vertex W or one of its DFS descendants in the subtree
 indicated by the the last pertinent child biconnected component.
 Returns TRUE if founds, FALSE otherwise.
 ****************************************************************************/

int  _FindUnembeddedEdgeToCurVertex(graphP theGraph, int cutVertex, int *pDescendant)
{
     if (gp_IsArc(gp_GetVertexPertinentEdge(theGraph, cutVertex)))
     {
         *pDescendant = cutVertex;
         return TRUE;
     }
     else
     {
    	 int subtreeRoot = gp_GetVertexFirstPertinentRootChild(theGraph, cutVertex);

         return _FindUnembeddedEdgeToSubtree(theGraph, theGraph->IC.v,
                                             subtreeRoot, pDescendant);
     }
}

/****************************************************************************
 _FindUnembeddedEdgeToSubtree()

 Given the root vertex of a DFS subtree and an ancestor of that subtree,
 find a vertex in the subtree that is adjacent to the ancestor by a
 cycle edge.
 Returns TRUE if found, FALSE if not found.
 ****************************************************************************/

int  _FindUnembeddedEdgeToSubtree(graphP theGraph, int ancestor,
                                  int SubtreeRoot, int *pDescendant)
{
int  e, Z, ZNew;

     *pDescendant = NIL;

/* If SubtreeRoot is a root copy, then we change to the DFS child in the
        DFS tree root edge of the bicomp rooted by SubtreeRoot. */

     SubtreeRoot = gp_IsVirtualVertex(theGraph, SubtreeRoot)
    					? gp_GetDFSChildFromRoot(theGraph, SubtreeRoot)
    					: SubtreeRoot;

/* Find the least descendant of the cut vertex incident to the ancestor. */

     e = gp_GetVertexFwdArcList(theGraph, ancestor);
     while (gp_IsArc(e))
     {
          if (gp_GetNeighbor(theGraph, e) >= SubtreeRoot)
          {
              if (gp_IsNotVertex(*pDescendant) || *pDescendant > gp_GetNeighbor(theGraph, e))
                  *pDescendant = gp_GetNeighbor(theGraph, e);
          }

          e = gp_GetNextArc(theGraph, e);
          if (e == gp_GetVertexFwdArcList(theGraph, ancestor))
              e = NIL;
     }

     if (gp_IsNotVertex(*pDescendant))
    	 return FALSE;

/* Make sure the identified descendant actually descends from the cut vertex */

     Z = *pDescendant;
     while (Z != SubtreeRoot)
     {
         ZNew = gp_GetVertexParent(theGraph, Z);
         if (gp_IsNotVertex(ZNew) || ZNew == Z)
             return FALSE;
         Z = ZNew;
     }

/* Return successfully */

     return TRUE;
}


/****************************************************************************
 _MarkPathAlongBicompExtFace()

 Sets the visited flags of vertices and edges on the external face of a
 bicomp from startVert to endVert, inclusive, by following the 'first' arc
 link out of each visited vertex.
 ****************************************************************************/

int  _MarkPathAlongBicompExtFace(graphP theGraph, int startVert, int endVert)
{
int  Z, ZPrevLink, ZPrevArc;

/* Mark the start vertex (and if it is a root copy, mark the parent copy too. */

     gp_SetVertexVisited(theGraph, startVert);

/* For each vertex visited after the start vertex, mark the vertex and the
        edge used to get there.  Stop after marking the ending vertex. */

     Z = startVert;
     ZPrevLink = 1;
     do {
        Z = _GetNeighborOnExtFace(theGraph, Z, &ZPrevLink);

        ZPrevArc = gp_GetArc(theGraph, Z, ZPrevLink);

        gp_SetEdgeVisited(theGraph, ZPrevArc);
        gp_SetEdgeVisited(theGraph, gp_GetTwinArc(theGraph, ZPrevArc));
        gp_SetVertexVisited(theGraph, Z);

     } while (Z != endVert);

     return OK;
}

/****************************************************************************
 _MarkDFSPath()

 Sets visited flags of vertices and edges from descendant to ancestor,
 including root copy vertices, and including the step of hopping from
 a root copy to its parent copy.

 At each vertex, the edge record is obtained whose type indicates that it
 leads to the DFS parent.  An earlier implementation just used the DFS parent
 member of the vertex, but then had to find the edge to mark anyway.
 This method is more generalized because some extension algorithms reduce
 DFS paths to single DFS tree edges, in which case the edge record with type
 EDGE_TYPE_PARENT may indicate the DFS paent or an ancestor.
 ****************************************************************************/
int  _MarkDFSPath(graphP theGraph, int ancestor, int descendant)
{
int  e, parent;

     // If we are marking from a root (virtual) vertex upward, then go up to the parent
     // copy before starting the loop
     if (gp_IsVirtualVertex(theGraph, descendant))
         descendant = gp_GetPrimaryVertexFromRoot(theGraph, descendant);

     // Mark the lowest vertex (the one with the highest number).
     gp_SetVertexVisited(theGraph, descendant);

     // Mark all ancestors of the lowest vertex, and the edges used to reach
     // them, up to the given ancestor vertex.
     while (descendant != ancestor)
     {
          if (gp_IsNotVertex(descendant))
              return NOTOK;

          // If we are at a bicomp root, then ascend to its parent copy and
          // mark it as visited.
          if (gp_IsVirtualVertex(theGraph, descendant))
          {
              parent = gp_GetPrimaryVertexFromRoot(theGraph, descendant);
          }

          // If we are on a regular, non-virtual vertex then get the edge to the parent,
          // mark the edge, then fall through to the code that marks the parent vertex.
          else
          {
              // Scan the edges for the one marked as the DFS parent
              parent = NIL;
              e = gp_GetFirstArc(theGraph, descendant);
              while (gp_IsArc(e))
              {
                  if (gp_GetEdgeType(theGraph, e) == EDGE_TYPE_PARENT)
                  {
                      parent = gp_GetNeighbor(theGraph, e);
                      break;
                  }
                  e = gp_GetNextArc(theGraph, e);
              }

              // Sanity check on the data structure integrity
              if (gp_IsNotVertex(parent))
                  return NOTOK;

              // Mark the edge
              gp_SetEdgeVisited(theGraph, e);
              gp_SetEdgeVisited(theGraph, gp_GetTwinArc(theGraph, e));
          }

          // Mark the parent, then hop to the parent and reiterate
          gp_SetVertexVisited(theGraph, parent);
          descendant = parent;
     }

     return OK;
}

/****************************************************************************
 _MarkDFSPathsToDescendants()
 ****************************************************************************/

int  _MarkDFSPathsToDescendants(graphP theGraph)
{
isolatorContextP IC = &theGraph->IC;

     if (theGraph->functions.fpMarkDFSPath(theGraph, IC->x, IC->dx) != OK ||
         theGraph->functions.fpMarkDFSPath(theGraph, IC->y, IC->dy) != OK)
         return NOTOK;

     if (gp_IsVertex(IC->dw))
         if (theGraph->functions.fpMarkDFSPath(theGraph, IC->w, IC->dw) != OK)
             return NOTOK;

     if (gp_IsVertex(IC->dz))
         if (theGraph->functions.fpMarkDFSPath(theGraph, IC->w, IC->dz) != OK)
             return NOTOK;

     return OK;
}

/****************************************************************************
 _AddAndMarkUnembeddedEdges()
 ****************************************************************************/

int  _AddAndMarkUnembeddedEdges(graphP theGraph)
{
isolatorContextP IC = &theGraph->IC;

     if (_AddAndMarkEdge(theGraph, IC->ux, IC->dx) != OK ||
         _AddAndMarkEdge(theGraph, IC->uy, IC->dy) != OK)
         return NOTOK;

     if (gp_IsVertex(IC->dw))
         if (_AddAndMarkEdge(theGraph, IC->v, IC->dw) != OK)
             return NOTOK;

     if (gp_IsVertex(IC->dz))
         if (_AddAndMarkEdge(theGraph, IC->uz, IC->dz) != OK)
             return NOTOK;

     return OK;
}

/****************************************************************************
 _AddAndMarkEdge()

 Adds edge records for the edge (ancestor, descendant) and marks the edge
 records and vertex structures that represent the edge.
 ****************************************************************************/

int _AddAndMarkEdge(graphP theGraph, int ancestor, int descendant)
{
    _AddBackEdge(theGraph, ancestor, descendant);

    /* Mark the edge so it is not deleted */

    gp_SetVertexVisited(theGraph, ancestor);
    gp_SetEdgeVisited(theGraph, gp_GetFirstArc(theGraph, ancestor));
    gp_SetEdgeVisited(theGraph, gp_GetFirstArc(theGraph, descendant));
    gp_SetVertexVisited(theGraph, descendant);

    return OK;
}

/****************************************************************************
 _AddBackEdge()

 This function transfers the edge records for the edge between the ancestor
 and descendant from the forward edge list of the ancestor to the adjacency
 lists of the ancestor and descendant.
 ****************************************************************************/

void _AddBackEdge(graphP theGraph, int ancestor, int descendant)
{
int fwdArc, backArc;

    /* We get the two edge records of the back edge to embed. */

     fwdArc = gp_GetVertexFwdArcList(theGraph, ancestor);
     while (gp_IsArc(fwdArc))
     {
          if (gp_GetNeighbor(theGraph, fwdArc) == descendant)
              break;

          fwdArc = gp_GetNextArc(theGraph, fwdArc);
          if (fwdArc == gp_GetVertexFwdArcList(theGraph, ancestor))
              fwdArc = NIL;
     }

     if (gp_IsNotArc(fwdArc))
         return;

    backArc = gp_GetTwinArc(theGraph, fwdArc);

    /* The forward arc is removed from the fwdArcList of the ancestor. */
    if (gp_GetVertexFwdArcList(theGraph, ancestor) == fwdArc)
    {
        if (gp_GetNextArc(theGraph, fwdArc) == fwdArc)
             gp_SetVertexFwdArcList(theGraph, ancestor, NIL);
        else gp_SetVertexFwdArcList(theGraph, ancestor, gp_GetNextArc(theGraph, fwdArc));
    }

    gp_SetNextArc(theGraph, gp_GetPrevArc(theGraph, fwdArc), gp_GetNextArc(theGraph, fwdArc));
    gp_SetPrevArc(theGraph, gp_GetNextArc(theGraph, fwdArc), gp_GetPrevArc(theGraph, fwdArc));

    /* The forward arc is added to the adjacency list of the ancestor. */
    gp_SetPrevArc(theGraph, fwdArc, NIL);
    gp_SetNextArc(theGraph, fwdArc, gp_GetFirstArc(theGraph, ancestor));
    gp_SetPrevArc(theGraph, gp_GetFirstArc(theGraph, ancestor), fwdArc);
    gp_SetFirstArc(theGraph, ancestor, fwdArc);

    /* The back arc is added to the adjacency list of the descendant. */
    gp_SetPrevArc(theGraph, backArc, NIL);
    gp_SetNextArc(theGraph, backArc, gp_GetFirstArc(theGraph, descendant));
    gp_SetPrevArc(theGraph, gp_GetFirstArc(theGraph, descendant), backArc);
    gp_SetFirstArc(theGraph, descendant, backArc);

    gp_SetNeighbor(theGraph, backArc, ancestor);
}

/****************************************************************************
 _DeleteUnmarkedVerticesAndEdges()

 For each vertex, traverse its adjacency list and delete all unvisited edges.
 ****************************************************************************/

int  _DeleteUnmarkedVerticesAndEdges(graphP theGraph)
{
	 int  v, e;

     /* All of the forward and back arcs of all of the edge records
        were removed from the adjacency lists in the planarity algorithm
        preprocessing.  We now put them back into the adjacency lists
        (and we do not mark them), so they can be properly deleted below. */

	 for (v = gp_GetFirstVertex(theGraph); gp_VertexInRange(theGraph, v); v++)
     {
         while (gp_IsArc(e = gp_GetVertexFwdArcList(theGraph, v)))
             _AddBackEdge(theGraph, v, gp_GetNeighbor(theGraph, e));
     }

     /* Now we delete all unmarked edges.  We don't delete vertices from the
        embedding, but the ones we should delete will become degree zero. */

	 for (v = gp_GetFirstVertex(theGraph); gp_VertexInRange(theGraph, v); v++)
     {
    	  e = gp_GetFirstArc(theGraph, v);
          while (gp_IsArc(e))
          {
                if (gp_GetEdgeVisited(theGraph, e))
                     e = gp_GetNextArc(theGraph, e);
                else e = gp_DeleteEdge(theGraph, e, 0);
          }
     }

     return OK;
}
