#ifndef CAVCOM_GRAPH_LIBGRAPH_VERTEX_H_
#define CAVCOM_GRAPH_LIBGRAPH_VERTEX_H_

#include <string>

#include "types.h"

namespace cavcom {
  namespace graph {

    // The Vertex class represents a single vertex in a graph.  All vertices are assigned a graph-unique ID that is
    // invariant across graph mutations.  Other attributes include label and color.
    class Vertex {
     public:
      using VertexID = uint;
      using Color = uint;
      static constexpr Color NOCOLOR = 0;

      // Default constructor.  Creates a new initialized vertex (no ID, label, or color).
      Vertex(void) : id_(0), color_(NOCOLOR) {}

      // Copy constructor.  Copies all attributes.
      Vertex(const Vertex &from) : id_(from.id_), label_(from.label_), color_(from.color_) {}

      VertexID id(void) const { return id_; }

      const std::string &label(void) const { return label_; }
      void label(const std::string &label) { label_ = label; }

      Color color(void) const { return color_; }
      void color(Color color) { color_ = color; }

     private:
      VertexID id_;
      std::string label_;
      Color color_;

      // The ID is set when a new vertex is created and inserted into a vertex set.
      void id(VertexID id) { id_ = id; }

      friend class Vertices;
    };

  }  // namespace graph
}  // namespace cavcom

#endif  // CAVCOM_GRAPH_LIBGRAPH_VERTEX_H_
