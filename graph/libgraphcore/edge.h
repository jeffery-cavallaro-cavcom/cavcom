#ifndef CAVCOM_GRAPH_LIBGRAPHCORE_EDGE_H_
#define CAVCOM_GRAPH_LIBGRAPHCORE_EDGE_H_

#include <vector>

#include "graphtypes.h"

namespace cavcom {
  namespace graph {
    // A single edge, used to join endpoint vertices.
    class Edge {
     public:
      // Creates a new edge with the specified attributes between the specified edges.
      Edge(VertexID from, VertexID to, const Label &label = Label(), Color color = NOCOLOR, Weight weight = FREE);

      // Copy constructor.
      Edge(const Edge &source);

      // Accessors and mutators.
      VertexID from(void) const { return from_; }
      VertexID to(void) const { return to_; }

      const Label &label(void) const { return label_; }
      void label(const Label &label) { label_ = label; }

      const Color color(void) const { return color_; }
      void color(Color color) { color_ = color; }

      const Weight weight(void) const { return weight_; }
      void weight(Weight weight) { weight_ = weight; }

     private:
      VertexID from_;
      VertexID to_;
      Label label_;
      Color color_;
      Weight weight_;

      // Disable assignment.
      Edge &operator=(const Edge &source) { return *this; }
    };

    // Edges are stored in an edge table and are referenced by index in the connection matrix.
    using EdgeTable = std::vector<Edge>;
    using EdgeNumber = EdgeTable::size_type;
    using Edges = std::vector<EdgeNumber>;

  }  // namespace graph
}  // namespace cavcom

#endif  // CAVCOM_GRAPH_LIBGRAPHCORE_EDGE_H_
