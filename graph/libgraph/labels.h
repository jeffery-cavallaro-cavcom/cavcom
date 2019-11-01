#ifndef CAVCOM_GRAPH_LIBGRAPH_LABELS_H_
#define CAVCOM_GRAPH_LIBGRAPH_LABELS_H_

#include <map>

#include "vertex.h"

namespace cavcom {
  namespace graph {

    // A vertex label to vertex number lookup table.  Uniqueness is enforced.
    class Labels {
     public:
      // Creates a new empty lookup table.
      Labels(void) {}

      // Returns true and the corresponding vertex number if the specified label is known.  Otherwise, returns false.
      bool find(const Label &label, VertexNumber *number) const;

      // Adds the specified lookup.  Throws a duplicate label error if the specified label is already in use.
      void add(const Label &label, VertexNumber number);

     private:
      using LabelToNumber = std::map<Label, VertexNumber>;
      LabelToNumber labels_;
    };

  }  // namespace graph
}  // namespace cavcom

#endif  // CAVCOM_GRAPH_LIBGRAPH_LABELS_H_
