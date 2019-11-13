#ifndef CAVCOM_GRAPH_LIBGRAPHCORE_SIMPLE_GRAPH_H_
#define CAVCOM_GRAPH_LIBGRAPHCORE_SIMPLE_GRAPH_H_

#include "graph.h"

namespace cavcom {
  namespace graph {
    // Simple graphs are undirected and do not allow multiple or loop edges.
    class SimpleGraph : public Graph {
     public:
      // Creates an empty simple graph with the specified number of unlabeled vertices (or the null graph if n = 0).
      explicit SimpleGraph(VertexNumber n = 0);

      // Creates a simple graph with the specified vertices and edges.
      SimpleGraph(const VertexValuesList &vertices, const EdgeValuesList &edges);

      // Creates a subgraph of the specified simple graph by removing the specified vertices and/or edges.
      SimpleGraph(const SimpleGraph &source,
                  const VertexNumbers &vremove = VertexNumbers(), const EdgeNumbers &eremove = EdgeNumbers());

      // Creates a copy of the specified simple graph, but with the two specified vertices contracted.
      SimpleGraph(const SimpleGraph &source, VertexNumber from, VertexNumber to);

      // Returns true if the graph is a complete graph.  This value is not cached so it should be considered
      // expensive to calculate.
      bool complete(void) const;

      // Makes the graph complete by adding any missing edges.
      void make_complete(void);

      // Calculates the minimum and maximum vertex degrees.  These values are not cached, so these calls should be
      // considered expensive.
      Degree mindeg(void) const { return minoutdeg(); }
      Degree maxdeg(void) const { return maxoutdeg(); }

      // Returns the degree for a single vertex.  An invalid vertex number throws an out-of-range error.
      Degree degree(VertexNumber vertex) const { return outdeg(vertex); }
    };

  }  // namespace graph
}  // namespace cavcom

#endif  // CAVCOM_GRAPH_LIBGRAPHCORE_SIMPLE_GRAPH_H_