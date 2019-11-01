#include "errors.h"
#include "labels.h"

namespace cavcom {
  namespace graph {

    bool find(const Label &label, VertexNumber *number) const {
      const auto &iv = labels_.find(label);
      if (iv == label_to_number_.end()) {
        *number = 0;
        return false;
      }
      *number = iv->second;
      return true;
    }

    void add(const Label &label, VertexNumber number) {
      if (!labels.emplace(label, number).second) {
        throw DuplicateLabelError(label);
      }
    }

  }  // namespace graph
}  // namespace cavcom
