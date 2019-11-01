#ifndef CAVCOM_GRAPH_LIBGRAPH_ERRORS_H_
#define CAVCOM_GRAPH_LIBGRAPH_ERRORS_H_

#include <sstream>
#include <stdexcept>

#include "graphtypes.h"

namespace cavcom {
  namespace graph {

    // Thrown when attempting to add a new labeled vertex or when changing a vertex label and the new label is
    // already assigned to another vertex.
    class DuplicateLabelError : public std::runtime_error {
     public:
      DuplicateLabelError(const Label &label);
      virtual const char *what(void) const { return msg_.str().c_str(); }
      const Label &label(void) const { return label_; }
     private:
      const Label &label_;
      ostringsteam msg_;
    }

  }  // namespace graph
}  // namespace cavcom

#endif  // CAVCOM_GRAPH_LIBGRAPH_ERRORS_H_
