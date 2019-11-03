#ifndef CAVCOM_GRAPH_LIBGRAPH_GRAPH_H_
#define CAVCOM_GRAPH_LIBGRAPH_GRAPH_H_

#include "connections.h"
#include "vertices.h"

namespace cavcom {
  namespace graph {
    // A graph is a mathematical object consisting of vertices and edges.  Graphs can be simple or allow multiple
    // edges, can be undirected or directed, and can optionally support loop edges.
    class Graph {
     public:
      // Creates an empty graph with the specified number of unlabeled vertices (or the null graph if n = 0).
      explicit Graph(VertexNumber n = 0, bool directed = false, bool multiple = false, bool loops = false);

      // Returns true for digraph edge semantics (otherwise undirected).
      bool directed(void) const { return connections_.directed(); }

      // Returns true if multiple edges (in the same direction in the case of digraphs) are allowed between the
      // same two endpoint vertices.
      bool multiple(void) const { return connections_.multiple(); }

      // Returns true if loop edges (on the same vertex) are allowed.
      bool loops(void) const { return connections_.loops(); }

      // Returns the number of vertices (order).
      VertexNumber order(void) const { return vertices_.size(); }

      // Returns the number of edges (size).
      EdgeNumber size(void) const { return edges_.size(); }

      // Gets a vertex by vertex number.  An invalid vertex number throws an out-of-range error.
      Vertex &vertex(VertexNumber number) { return vertices_[number]; }

      // Gets an edge by edge number.  An invalid vertex number throws an out-of-range error.
      Edge &edge(EdgeNumber number) { return edges_.at(number); }

      // Adds an edge between the specified endpoint vertices.  An invalid vertex number throws an out-of-range
      // error.  Attempting to add a multiple or loop edge throw an error if disabled.  Returns the resulting
      // number of edges joining the two endpoint vertices.
      Degree join(VertexNumber from, VertexNumber to,
                  const Label &label = Label(), Color color = BLACK, Weight weight = FREE);

      // Calculates the minimum and maximum degrees.  A synonym is provided for undirected graphs.  The results are
      // not cached, so these calls should be considered expensive.
      Degree minindeg(void) const { return connections_.minindeg(); }
      Degree maxindeg(void) const { return connections_.maxindeg(); }

      Degree minoutdeg(void) const { return connections_.minoutdeg(); }
      Degree maxoutdeg(void) const { return connections_.maxoutdeg(); }

      Degree mindeg(void) const { return minoutdeg(); }
      Degree maxdeg(void) const { return maxoutdeg(); }

      // Returns the degrees for a single vertex.  A synonym is provided for undirected graphs.  An invalid vertex
      // number throws an out-of-range error.
      Degree indeg(VertexNumber vertex) const { return connections_.indeg(vertex); }
      Degree outdeg(VertexNumber vertex) const { return connections_.outdeg(vertex); }
      Degree degree(VertexNumber vertex) const { return outdeg(vertex); }

    private:
      // The vertices and edges.
      Vertices vertices_;
      EdgeTable edges_;

      // The connection matrix.  Indexed by from vertex and to vertex, in that order.
      Connections connections_;
    };

  }  // namespace graph
}  // namespace cavcom

#endif  // CAVCOM_GRAPH_LIBGRAPH_GRAPH_H_
