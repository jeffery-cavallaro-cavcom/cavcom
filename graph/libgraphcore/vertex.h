#ifndef CAVCOM_GRAPH_LIBGRAPHCORE_VERTEX_H_
#define CAVCOM_GRAPH_LIBGRAPHCORE_VERTEX_H_

#include <set>
#include <vector>

#include "edge.h"

namespace cavcom {
  namespace graph {
    // A values structure for creating a vertex.
    struct VertexValues {
      Label label;
      Color color;
      Dimension xpos;
      Dimension ypos;
    };

    // A single vertex in a graph.
    class Vertex {
     public:
      // Creates a new isolated vertex with the specified attributes.
      explicit Vertex(VertexID id, const Label &label = Label(), Color color = NOCOLOR,
                      Dimension xpos = 0.0, Dimension ypos = 0.0);

      // Creates a new isolated vertex using the specified attribute values.  Note that the vertex ID is assigned
      // by the parent graph and thus is not part of the values structure.
      Vertex(VertexID id, const VertexValues &values);

      // Copy constructor.
      Vertex(const Vertex &source);

      // The vertex ID assigned by the parent graph when the vertex is first added to the graph.  Although a vertex's
      // number (position in the vertex table) may change across a graph mutation, the vertex ID does not.
      VertexID id(void) const { return id_; }

      // Vertex labels can only be changed via the parent graph in order to guarantee uniqueness.
      const Label &label(void) const { return label_; }

      // Vertex colors are typically used in partitioning applications.
      Color color(void) const { return color_; }
      void color(Color color) { color_ = color; }

      const VertexIDs &contracted(void) const { return contracted_; }

      // Vertex location, for drawing add-ons.
      Dimension xpos(void) const { return xpos_; }
      void xpos(Dimension x) { xpos_ = x; }

      Dimension ypos(void) const { return ypos_; }
      void ypos(Dimension y) { ypos_ = y; }

     private:
      VertexID id_;
      Label label_;
      Color color_;
      VertexIDs contracted_;
      Dimension xpos_;
      Dimension ypos_;

      // The parent graph needs access during graph mutations.
      friend class Vertices;
      friend class Graph;
    };

    // Vertices are stored in the vertex table and are identified by a vertex ID that is invariant across graph
    // mutations.  Thus, a vertex that changes its position in the connection matrix is still identifiable with
    // the corresponding vertex in the original graph.  Vertices in a particular graph are also identified by
    // their position in the vertex table, which corresponds to their position in the connection matrix.
    using VertexTable = std::vector<Vertex>;
    using VertexNumber = VertexTable::size_type;
    using VertexNumbers = std::set<VertexNumber>;
    using VertexNumbersList = std::vector<VertexNumbers>;

  }  // namespace graph
}  // namespace cavcom

#endif  // CAVCOM_GRAPH_LIBGRAPHCORE_VERTEX_H_
