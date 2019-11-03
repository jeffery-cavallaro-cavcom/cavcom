#include "vertex.h"

namespace cavcom {
  namespace graph {

    Vertex::Vertex(VertexID id, const Label &label, Color color, Dimension xpos, Dimension ypos)
      : id_(id), label_(label), color_(color), xpos_(xpos), ypos_(ypos) {}

    Vertex::Vertex(VertexID id, const VertexValues &values)
      : Vertex(id, values.label, values.color, values.xpos, values.ypos) {}

    Vertex::Vertex(const Vertex &source)
      : id_(source.id_), label_(source.label_), color_(source.color_), contracted_(source.contracted_),
        xpos_(source.xpos_), ypos_(source.ypos_) {}

  }  // namespace graph
}  // namespace cavcom
