#include <algorithm>
#include <vector>

#include "vertices.h"

namespace cavcom {
  namespace graph {

    Vertices::Vertices(void) : next_(0), id_to_number_("vertex id"), label_to_number_("vertex label") {}

    Vertices::Vertices(const Vertices &source, bool keep, const VertexNumbers &vertices) : Vertices() {
      // Construct the new set of vertices.
      VertexNumber n = source.size();
      for (VertexNumber iv = 0, ov = 0; iv < n; ++iv) {
        bool found = (vertices.find(iv) != vertices.cend());
        if ((keep && !found) || (!keep && found)) continue;
        const Vertex &v = source[iv];
        next_ = v.id();
        add(v.label(), v.color(), v.xpos(), v.ypos());
        vertices_[ov++].contracted_ = v.contracted_;
      }
      next_ = source.next_;
    }

    Vertices::Vertices(const Vertices &source, bool keep, const VertexNumbersList &vertices) : Vertices() {
      // Flatten out the list.
      VertexNumbers targets;
      std::for_each(vertices.cbegin(), vertices.cend(),
                    [&targets](VertexNumbers subset) {
                      if (subset.size() > 1) targets.insert(subset.cbegin(), subset.cend());
                    });

      // Construct the new set of vertices.
      VertexNumber n = source.size();
      for (VertexNumber iv = 0, ov = 0; iv < n; ++iv) {
        bool found = (targets.find(iv) != targets.cend());
        if ((keep && !found) || (!keep && found)) continue;
        const Vertex &v = source[iv];
        next_ = v.id();
        add(v.label(), v.color(), v.xpos(), v.ypos());
        vertices_[ov++].contracted_ = v.contracted_;
      }
      next_ = source.next_;
    }

    void Vertices::reserve(VertexNumber n) {
      if (vertices_.capacity() < n) vertices_.reserve(n);
    }

    void Vertices::add(const Label &label, Color color, Dimension xpos, Dimension ypos) {
      // Mark what the new vertex's number will be when added.
      auto number = vertices_.size();

      // Register any label first.  This keeps the context valid if a duplicate label error is thrown.
      if (!label.empty()) label_to_number_.add(label, number, true);
      id_to_number_.add(next_, number, true);

      // Add the new vertex.
      vertices_.emplace_back(next_++, label, color, xpos, ypos);
    }

    void Vertices::add(const VertexValues &values) {
      add(values.label, values.color, values.xpos, values.ypos);
    }

    void Vertices::add(const VertexValuesList &values) {
      for_each(values.cbegin(), values.cend(), [this](const VertexValues &vertex){ add(vertex); });
    }

    bool Vertices::find(VertexID id, VertexNumber *number, bool errors) const {
      const VertexNumber *found = id_to_number_.find(id, errors);
      if (!found) {
        *number = 0;
        return false;
      }
      *number = *found;
      return true;
    }

    bool Vertices::find(Label label, VertexNumber *number, bool errors) const {
      const VertexNumber *found = label_to_number_.find(label, errors);
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
