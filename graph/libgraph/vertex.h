#ifndef CAVCOM_GRAPH_LIBGRAPH_VERTEX_H_
#define CAVCOM_GRAPH_LIBGRAPH_VERTEX_H_

#include "edge.h"

namespace cavcom {
  namespace graph {

    // The degree of a vertex is the number of edges incident to a vertex.
    using Degree = Edges::size_type;

    // A single vertex in a graph.
    class Vertex {
    public:
      // Each vertex maintains a list of vertex IDs that track vertex contractions.  An empty contracted list
      // indicates that the vertex is not the result of any contractions.
      using Contracted = std::vector<VertexID>;

      // Creates a new isolated vertex.  The ID is assigned by the parent graph.
      Vertex(const Label &label = Label(), Color color = BLACK)
        : id_(0), label_(label), color_(color), indeg_(0), outdeg_(0) {}

      // Accessors and mutators.
      VertexID id(void) const { return id_; }
      const Label &label(void) const { return label_; }

      const Color color(void) const { return color_; }
      void color(Color color) { color_ = color; }

      Degree indeg(void) const { return indeg_; }
      Degree outdeg(void) const { return outdeg_; }

      const Contracted &contracted(void) const { return contracted_; }

    private:
      VertexID id_;
      Label label_;
      Color color_;
      Degree indeg_;
      Degree outdeg_;
      Contracted contracted_;

      // The parent graph needs access during graph mutations.
      friend class Graph;
    };

    // Vertices are stored in the vertex table and are identified by a vertex ID that is invariant across graph
    // mutations.  Thus, a vertex that changes its position in the connection matrix is still identifiable with
    // the corresponding vertex in the original graph.  Vertices in a particular graph are also identified by
    // their position in the vertex table, which corresponds to their position in the connection matrix.
    using VertexTable = std::vector<Vertex>;
    using VertexNumber = VertexTable::size_type;

  }  // namespace graph
}  // namespace cavcom

#endif  // CAVCOM_GRAPH_LIBGRAPH_VERTEX_H_
