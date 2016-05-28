# Planarity
Algorithms for graph planarity testing, forbidden subgraph finding, and planar embeddings.

This provides a Ptyhon interface for part of Boyer's (C) planarity algorithms found at <https://github.com/graph-algorithms/edge-addition-planarity-suite>

## Example

```python
In [1]: # Example of the complete graph of 5 nodes, K5

In [2]: # K5 is not planar

In [3]: import planarity

In [4]: edgelist = [('a', 'b'), ('a', 'c'), ('a', 'd'), ('a', 'e'),
   ...:             ('b', 'c'),('b', 'd'),('b', 'e'),
   ...:             ('c', 'd'), ('c', 'e'),
   ...:             ('d', 'e')]

In [5]: print(planarity.is_planar(edgelist))
False

In [6]: # remove an edge to make the graph planar

In [7]: edgelist.remove(('a','b'))

In [8]: print(planarity.is_planar(edgelist))
True

In [9]: # make an ascii text drawing

In [10]: print(planarity.ascii(edgelist))
```

<pre>
----1----
| | |   |
| | -3--|
| |  ||||
| -2--|||
|  |  |||
---4---||
 |     ||
 ---5----
</pre>

See <https://github.com/hagberg/planarity/tree/master/examples> for more examples.



## License
Distributed with a BSD license; see LICENSE.txt.
