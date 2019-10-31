#include "vertex.h"

namespace cavcom {
  namespace graph {

    Vertex::Vertex(const Label &label, Color color) : id_(0), label_(label), color_(color), indeg_(0), outdeg_(0) {}

    Vertex::Vertex(const Vertex &source)
      : id_(source.id_),
        label_(source.label_),
        color_(source.color_),
        indeg_(source.indeg_),
        outdeg_(source.outdeg_),
        contracted_(source.contracted_) {}

  }  // namespace graph
}  // namespace cavcom
