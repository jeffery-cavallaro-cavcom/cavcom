#include "errors.h"

namespace cavcom {
  namespace graph {

    DuplicateLabelError::DuplicateLabelError(const Label &label) : runtime_error("Duplicate label"), label_(label) {
      msg << std::runtime_error::what();
      msg << ": ";
      msg << label;
    }

  }  // namespace graph
}  // namespace cavcom
