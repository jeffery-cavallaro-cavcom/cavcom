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
    };

  }  // namespace graph
}  // namespace cavcom

#endif  // CAVCOM_GRAPH_LIBGRAPH_UNDIRECTED_H_
