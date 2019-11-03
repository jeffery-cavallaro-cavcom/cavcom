#include "vertices.h"

namespace cavcom {
  namespace graph {

    Vertices::Vertices(void) : next_(0), id_to_number_("vertex ID"), label_to_number_("vertex label") {}

    void Vertices::reserve(VertexNumber n) {
      if (vertices_.capacity() < n) vertices_.reserve(n);
    }

    void Vertices::add(const Label &label, Color color) {
      // Mark what the new vertex's number will be when added.
      auto number = vertices_.size();

      // Register any label first.  This keeps the context valid if a duplicate label error is thrown.
      if (!label.empty()) label_to_number_.add(label, number, true);
      id_to_number_.add(next_, number, true);

      // Add the new vertex.
      vertices_.emplace_back(next_++, label, color);
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

    void Vertices::label(VertexNumber number, const Label &newlabel) {
      // Get the target vertex.  Note that an out-of-range exception will the thrown if the vertex number is
      // invalid.
      Vertex &v = vertices_.at(number);

      // Do nothing if the new label matches the old (including both empty).
      if (newlabel == v.label()) return;

      // Unlabel the vertex  first.  This keeps the context valid.
      if (!v.label().empty()) label_to_number_.remove(v.label());
      v.label_.clear();

      // Stop if the vertex is to be unlabeled.
      if (newlabel.empty()) return;

      // Label the vertex with the new label.  Note that a duplicate label will be thrown if the label is already
      // in use by another vertex.
      label_to_number_.add(newlabel, number, true);
      v.label_ = newlabel;
    }

  }  // namespace graph
}  // namespace cavcom