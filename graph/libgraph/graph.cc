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

    Graph::Graph(const VertexValuesList &vertices, const EdgeValuesList &edges,
          bool directed, bool multiple, bool loops)
      : connections_(vertices.size(), directed, multiple, loops) {
      vertices_.reserve(vertices.size());
      for_each(vertices.cbegin(), vertices.cend(), [this](const VertexValues &vertex){ vertices_.add(vertex); });
      join(edges);
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

    Degree Graph::join(const EdgeValues &values) {
      return join(values.from, values.to, values.label, values.color, values.weight);
    }

    void Graph::join(const EdgeValuesList &values) {
      for_each(values.cbegin(), values.cend(), [this](const EdgeValues &edge){ join(edge); });
    }

  }  // namespace graph
}  // namespace cavcom
