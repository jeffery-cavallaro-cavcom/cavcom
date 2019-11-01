#include "vertices.h"

namespace cavcom {
  namespace graph {

    Vertices::Vertices(void) : next_(0), minindeg_(0), maxindeg_(0), minoutdeg_(0), maxoutdeg_(0) {}

    void Vertices::reserve(VertexNumber n) {
      if (vertices_.capacity() < n) vertices_.reserve(n);
    }

    void Vertices::add(const Label &label, Color color) {
    }

  }  // namespace graph
}  // namespace cavcom
