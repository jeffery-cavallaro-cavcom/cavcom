#include "graph.h"

namespace cavcom {
  namespace graph {

    constexpr Graph::Color Graph::BLACK;

    void Graph::add_vertices(VertexNumber n) {
      // Don't bother is no new vertices are requested.
      if (n <= 0) return;

      // Add the new vertices to the end of the vertex list.
      VertexNumber old_size = vertices_.size();
      VertexNumber new_size = old_size + n;
      vertices_.resize(new_size, {});

      // Assign unique vertex IDs to the new vertices and add them to the lookup-by-id table.  These new vertices
      // don't have any labels yet, so there is no need to add them to the lookup-by-label table.
      for (VertexNumber iv = old_size; iv < new_size; ++iv) {
        VertexID next = next_++;
        vertices_[iv].id_ = next;
        id_to_number_.emplace(next, iv);
      }

      // There are isolated vertices now, so the minimum degree goes to 0.
      mindeg_ = 0;
    }

    void Graph::reconnect(void) {
      // Reallocate for the exact number of vertices.
      connections_.reset(new Connections(order()));

      // Add the edges.
      EdgeNumber m = size();
      for (EdgeNumber ie = 0; ie < m; ++ie) {
        Edge e = edges_[ie];
        VertexNumber from = id_to_number_[e.from_];
        VertexNumber to = id_to_number_[e.to_];
      }
    }

  }  // namespace graph
}  // namespace cavcom
