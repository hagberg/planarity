"""Functional interface to planarity."""
import typing

import planarity

__all__ = [
    'is_planar',
    'kuratowski_edges',
    'ascii',
    'draw',
    'write',
    'mapping'
    ]


def is_planar(graph):
    """Tests whether or not the graph is planar.

    Constructs a
    :py:class:`~planarity.classic.planarity.PGraph`
    and calls its :py:meth:`~planarity.classic.planarity.PGraph.is_planar`
    method.

    Args:
        graph: A graph specified in a format that may be converted to a
            :py:class:`~planarity.classic.planarity.PGraph`.

    Returns:
        ``True`` if the graph is planar, or ``False`` if not.

    Raises:
        ValueError: if the given graph is already a
            :py:class:`~planarity.classic.planarity.PGraph`.
        RuntimeError: if the graph couldn't be converted to a
            :py:class:`~planarity.classic.planarity.PGraph`.
        RuntimeError: if an error was encountered by C-layer methods such
            as ``gp_Embed()``.
        RuntimeError: if a prior invocation of this method already failed.
        RuntimeError: if any embedding operation was performed other than
            the one indicated by ``EMBEDFLAGS_PLANAR``.
    """
    return planarity.PGraph(graph).is_planar()


def kuratowski_edges(graph):
    """Returns a list of the edges in a minimal non-planar subgraph of the graph.

    Constructs a
    :py:class:`~planarity.classic.planarity.PGraph`
    and calls its
    :py:meth:`~planarity.classic.planarity.PGraph.kuratowski_edges` method.

    Args:
        graph: A graph specified in a format that may be converted to a
            :py:class:`~planarity.classic.planarity.PGraph`.

    Returns:
        A list of the edges in a minimal non-planar subgraph of the graph,
        if it is non-planar, or an empty list if it is planar.

    Raises:
        ValueError: if the given graph is already a
            :py:class:`~planarity.classic.planarity.PGraph`.
        RuntimeError: if the graph couldn't be converted to a
            :py:class:`~planarity.classic.planarity.PGraph`.
        RuntimeError: if an error was encountered by C-layer methods such
            as ``gp_Embed()``.
        RuntimeError: if a prior invocation of this method already failed.
        RuntimeError: if any embedding operation was performed other than
            the one indicated by ``EMBEDFLAGS_PLANAR``.
    """
    return planarity.PGraph(graph).kuratowski_edges()


def ascii(graph) -> str:
    """Produces an ASCII string rendition of the graph, if it is planar.

    Constructs a
    :py:class:`~planarity.classic.planarity.PGraph`
    and calls its
    :py:meth:`~planarity.classic.planarity.PGraph.ascii` method.

    Args:
        graph: A graph specified in a format that may be converted to a
            :py:class:`~planarity.classic.planarity.PGraph`.

    Returns:
        An ASCII string rendition of a planar graph.

    Raises:
        ValueError: if the given graph is already a
            :py:class:`~planarity.classic.planarity.PGraph`.
        RuntimeError: if the graph couldn't be converted to a
            :py:class:`~planarity.classic.planarity.PGraph`.
        RuntimeError: if an error was encountered by C-layer methods
            such as ``gp_Embed()`` or ``gp_DrawPlanar_RenderToString()``.
        RuntimeError: if a prior invocation of this method already failed.
        RuntimeError: if any embedding operation was performed other than
            the one indicated by ``EMBEDFLAGS_DRAWPLANAR``.
        RuntimeError: if the graph is non-planar.
    """
    return planarity.PGraph(graph).ascii()


def draw(graph, labels=True, outfileName=None):
    """Draws the graph with Matplotlib, if it is planar.

    Constructs a
    :py:class:`~planarity.classic.planarity.PGraph`
    and calls its
    :py:meth:`~planarity.classic.planarity.PGraph.draw` method with the
    given ``labels`` and ``outfileName``.

    Args:
        graph: A graph specified in a format that may be converted to a
            :py:class:`~planarity.classic.planarity.PGraph`.
        labels (bool): If ``True``, vertex labels are rendered in the drawing.
            Otherwise, vertices are rendered unlabelled in the drawing.
        outfileName (:obj:`str`): File to which to output a Matplotlib
            rendering of the planar graph. If not given, then the caller can
            call :external+matplotlib:py:func:`matplotlib.pyplot.savefig`.

    Raises:
        ValueError: if the given graph is already a
            :py:class:`~planarity.classic.planarity.PGraph`.
        RuntimeError: if the graph couldn't be converted to a
            :py:class:`~planarity.classic.planarity.PGraph`.
        ImportError: if dependencies from Matplotlib fail to be imported.
        RuntimeError: if an error was encountered by C-layer methods
            such as ``gp_Embed()``.
        RuntimeError: if a prior invocation of this method already failed.
        RuntimeError: if any embedding operation was performed other than
            the one indicated by ``EMBEDFLAGS_DRAWPLANAR``.
        RuntimeError: if the graph is non-planar.
    """
    pgraph = planarity.PGraph(graph)

    try:
        pgraph.draw(labels, outfileName)
    except ImportError as import_error:
        raise ImportError(
            "Please install missing dependencies in your current environment "
            "and retry."
        ) from import_error


def write(graph, path: str = 'stdout') -> None:
    """Writes the graph to ``path``.

    Constructs a
    :py:class:`~planarity.classic.planarity.PGraph`
    and calls its
    :py:meth:`~planarity.classic.planarity.PGraph.write` method with the
    specified ``path``.

    Args:
        graph: A graph specified in a format that may be converted to a
            :py:class:`~planarity.classic.planarity.PGraph`.
        path (str):Path to which to write graph. Defaults to ``stdout``
            stream.

    Raises:
        ValueError: if the given graph is already a
            :py:class:`~planarity.classic.planarity.PGraph`.
        RuntimeError: if the graph couldn't be converted to a
            :py:class:`~planarity.classic.planarity.PGraph`.
        RuntimeError: if the C-layer ``gp_Write()`` failed.
    """
    planarity.PGraph(graph).write(path)


def mapping(graph) -> dict[int, typing.Any]:
    """Returns the map of integer vertex labels to their original labels.

    Constructs a
    :py:class:`~planarity.classic.planarity.PGraph`
    and calls its
    :py:meth:`~planarity.classic.planarity.PGraph.mapping` method.

    Args:
        graph: A graph specified in a format that may be converted to a
            :py:class:`~planarity.classic.planarity.PGraph`.

    Returns:
        A mapping between the integers assigned to the vertices, by
        :py:class:`~planarity.classic.planarity.PGraph` initialization,
        and their original labels provided to
        :py:class:`~planarity.classic.planarity.PGraph` initialization.

    Raises:
        ValueError: if the given graph is already a
            :py:class:`~planarity.classic.planarity.PGraph`.
        RuntimeError: if the graph couldn't be converted to a
            :py:class:`~planarity.classic.planarity.PGraph`.
    """
    return planarity.PGraph(graph).mapping()
