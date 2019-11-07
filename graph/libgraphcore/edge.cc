#include "edge.h"

namespace cavcom {
  namespace graph {

    // Creates a new edge with the specified attributes between the specified edges.
    Edge::Edge(VertexID from, VertexID to, const Label &label, Color color, Weight weight)
      : from_(from), to_(to), label_(label), color_(color), weight_(weight) {}

    // Copy constructor.
    Edge::Edge(const Edge &source)
      : from_(source.from_), to_(source.to_),
        label_(source.label_), color_(source.color_), weight_(source.weight_) {}

  }  // namespace graph
}  // namespace cavcom
