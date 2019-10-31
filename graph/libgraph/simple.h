#ifndef CAVCOM_GRAPH_LIBGRAPH_SIMPLE_H_
#define CAVCOM_GRAPH_LIBGRAPH_SIMPLE_H_

#include "undirected.h"

namespace cavcom {
  namespace graph {

    // Simple graphs are undirected graphs that do not allow multiple or loop edges.
    class SimpleGraph : public UndirectedGraph {
     public:
      // Default constructor.  Creates a null graph.
      SimpleGraph() {}
    };

  }  // namespace graph
}  // namespace cavcom

#endif  // CAVCOM_GRAPH_LIBGRAPH_SIMPLE_H_
