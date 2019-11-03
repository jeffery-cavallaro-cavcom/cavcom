#include <algorithm>

#include "graph.h"

namespace cavcom {
  namespace graph {

    Graph::Graph(VertexNumber n, bool directed, bool multiple, bool loops)
      : connections_(n, directed, multiple, loops) {
      vertices_.reserve(n);
      for (VertexNumber iv = 0; iv < n; ++iv) {
        vertices_.add();
      }
    }

    Degree Graph::join(VertexNumber from, VertexNumber to, const Label &label, Color color, Weight weight) {
      // Fetch the endpoint vertex IDs.  An out-of-range error will occur if the vertex numbers are invalid.
      VertexID fid = vertices_[from].id();
      VertexID tid = vertices_[to].id();

      // Construct and add the edge.
      EdgeNumber next = edges_.size();
      edges_.emplace_back(fid, tid, label, color, weight);
      return connections_.join(from, to, next);
    }

  }  // namespace graph
}  // namespace cavcom
