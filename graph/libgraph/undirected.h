#ifndef CAVCOM_GRAPH_LIBGRAPH_UNDIRECTED_H_
#define CAVCOM_GRAPH_LIBGRAPH_UNDIRECTED_H_

#include "graph.h"

namespace cavcom {
  namespace graph {

    // Edges in undirected graphs are bidirectional.
    class UndirectedGraph : public Graph {
     public:
      // Default constructor.  Creates a null graph.
      UndirectedGraph() {}

      // The minimum vertex degree in the graph, usually known as small-delta.
      Degree mindeg(void) const { return minoutdeg(); }

      // The maximum vertex degree in the graph, usually known as capital-delta.
      Degree maxdeg(void) const { return maxoutdeg(); }
    };

  }  // namespace graph
}  // namespace cavcom

#endif  // CAVCOM_GRAPH_LIBGRAPH_UNDIRECTED_H_
