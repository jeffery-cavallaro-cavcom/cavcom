#include "vertices.h"

namespace cavcom {
  namespace graph {

    Vertices::Vertices(void) : next_(0), id_to_number_("vertex ID"), label_to_number_("vertex label"),
                               minindeg_(0), maxindeg_(0), minoutdeg_(0), maxoutdeg_(0) {}

    void Vertices::reserve(VertexNumber n) {
      if (vertices_.capacity() < n) vertices_.reserve(n);
    }

    void Vertices::add(const Label &label, Color color) {
      auto number = vertices_.size();
      if (!label.empty()) label_to_number_.add(label, number, true);
      id_to_number_.add(next_, number, true);
      vertices_.emplace_back(next_++, label, color);
      minindeg_ = 0;
      minoutdeg_ = 0;
    }

    bool Vertices::find(VertexID id, VertexNumber *number) {
      const VertexNumber *found = id_to_number_.find(id);
      if (!found) {
        *number = 0;
        return false;
      }
      *number = *found;
      return true;
    }

    bool Vertices::find(Label label, VertexNumber *number) {
      const VertexNumber *found = label_to_number_.find(label);
      if (!found) {
        *number = 0;
        return false;
      }
      *number = *found;
      return true;
    }

  }  // namespace graph
}  // namespace cavcom
