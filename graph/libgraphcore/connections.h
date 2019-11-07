#ifndef CAVCOM_GRAPH_LIBGRAPHCORE_CONNECTIONS_H_
#define CAVCOM_GRAPH_LIBGRAPHCORE_CONNECTIONS_H_

#include <memory>
#include <vector>

#include "matrix.h"

#include "vertex.h"

namespace cavcom {
  namespace graph {
    // The degree of a vertex is the number of edges incident to a vertex.
    using Degree = Edges::size_type;

    // A graph's (square) connection matrix.  Indexed by from vertex and to vertex, in that order.  Entries are
    // lists of edge numbers.  Supports simple graphs, multigraphs, and digraphs.
    class Connections {
     public:
      // Creates a new connections matrix for the specified number of vertices and with the specified edge
      // semantics (simple by default).
      explicit Connections(VertexNumber order, bool directed = false, bool multiple = false, bool loops = false);

      // Returns the size of the matrix, which should be the order of the graph.
      VertexNumber size(void) const { return connections_->n(); }

      // Returns true for digraph edge semantics (otherwise undirected).
      bool directed(void) const { return directed_; }

      // Returns true if multiple edges (in the same direction in the case of digraphs) are allowed between the
      // same two endpoint vertices.
      bool multiple(void) const { return multiple_; }

      // Returns true if loop edges (on the same vertex) are allowed.
      bool loops(void) const { return loops_; }

      // Returns the list of edges that join the specified vertices.  An invalid vertex number throws an
      // out-of-range error.
      const Edges &edges(VertexNumber from, VertexNumber to) const { return connections_->at(from, to); }

      // Returns true if the specified edges are adjacent.
      bool adjacent(VertexNumber from, VertexNumber to) const { return !edges(from, to).empty(); }

      // Joins the specified endpoint vertices with the specified edge.  If the parent graph is undirected then
      // this will result in the edge being added as both (from, to) and (to, from).  Attempts to add multiple
      // edges will throw a multiple edge error if multiple edges are disabled.  Attempts to add loop edges will
      // throw a loop edge error if loop edges are disabled.  An invalid vertex number throws an out-of-range
      // error.  The degree counters are updated as necessary.  Returns the new number of edges between the two
      // endpoint vertices.
      Degree join(VertexNumber from, VertexNumber to, EdgeNumber edge);

      // Calculates the minimum and maximum degrees.  A synonym is provided for undirected graphs.  The results are
      // not cached, so these calls should be considered expensive.
      Degree minindeg(void) const;
      Degree maxindeg(void) const;

      Degree minoutdeg(void) const;
      Degree maxoutdeg(void) const;

      Degree mindeg(void) const { return minoutdeg(); }
      Degree maxdeg(void) const { return maxoutdeg(); }

      // Returns the degrees for a single vertex.  A synonym is provided for undirected graphs.  An invalid vertex
      // number throws an out-of-range error.
      Degree indeg(VertexNumber vertex) const { return indeg_.at(vertex); }
      Degree outdeg(VertexNumber vertex) const { return outdeg_.at(vertex); }
      Degree degree(VertexNumber vertex) const { return outdeg(vertex); }

     private:
      bool directed_;
      bool multiple_;
      bool loops_;

      using ConnectionMatrix = cavcom::utility::Matrix<Edges>;
      using ConnectionMatrixPtr = std::unique_ptr<ConnectionMatrix>;
      ConnectionMatrixPtr connections_;

      // The degrees for each vertex can be calculated from the matrix; however, these vaues are widely used in
      // various graph algorithms, and so they are stored here for efficiency.  Both input and output degrees are
      // stored in support of directed graphs.  Undirected graphs should use the out degrees.
      std::vector<Degree> indeg_;
      std::vector<Degree> outdeg_;
    };

  }  // namespace graph
}  // namespace cavcom

#endif  // CAVCOM_GRAPH_LIBGRAPHCORE_CONNECTIONS_H_
