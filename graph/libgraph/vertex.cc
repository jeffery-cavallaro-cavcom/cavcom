#include "vertex.h"

namespace cavcom {
  namespace graph {

    Vertex::Vertex(VertexID id, const Label &label, Color color) : id_(id), label_(label), color_(color) {}

    Vertex::Vertex(const Vertex &source)
      : id_(source.id_), label_(source.label_), color_(source.color_), contracted_(source.contracted_) {}

  }  // namespace graph
}  // namespace cavcom
