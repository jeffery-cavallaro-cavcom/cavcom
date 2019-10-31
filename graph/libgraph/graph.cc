#include <algorithm>

#include "graph.h"

namespace cavcom {
  namespace graph {

    constexpr Graph::Color Graph::BLACK;

    bool Graph::find_vertex(VertexID id, VertexNumber *number) const {
      const auto &iv = id_to_number_.find(id);
      if (iv == id_to_number_.end()) {
        *number = 0;
        return false;
      }
      *number = iv->second;
      return true;
    }

    bool Graph::find_vertex(const Label &label, VertexNumber *number) const {
      const auto &iv = label_to_number_.find(label);
      if (iv == label_to_number_.end()) {
        *number = 0;
        return false;
      }
      *number = iv->second;
      return true;
    }

    void Graph::add_vertices(VertexNumber n) {
      // Don't bother if no new vertices are requested.
      if (n <= 0) return;

      // Add the new vertices to the end of the vertex list.
      VertexNumber old_size = vertices_.size();
      VertexNumber new_size = old_size + n;
      vertices_.resize(new_size);

      // Assign unique vertex IDs to the new vertices and add them to the lookup-by-id table.  These new vertices
      // don't have any labels yet, so there is no need to add them to the lookup-by-label table.
      for (VertexNumber iv = old_size; iv < new_size; ++iv) {
        VertexID next = next_++;
        vertices_[iv].id_ = next;
        id_to_number_.emplace(next, iv);
      }

      minindeg_ = 0;
      maxindeg_ = 0;
      minoutdeg_ = 0;
      maxoutdeg_ = 0;

      // Reconstruct the connection matrix and the degree attributes.
      reconnect();
    }

    void Graph::add_vertices(const VertexValuesList &vertices) {
      VertexNumber next = order();
      VertexNumber added = vertices.size();
      add_vertices(added);
      for (auto iv = vertices.cbegin(); iv != vertices.cend(); ++iv) {
        label_vertex(next, iv->label);
        vertex(next++).color(iv->color);
      }
    }

    void Graph::label_vertex(VertexNumber iv, const std::string &label) {
      Vertex &v = vertex(iv);
      if (! label_to_number_.emplace(label, iv).second) {
        throw DuplicateLabelError();
      }
      v.label_ = label;
    }

    void Graph::add_edge(VertexID from, VertexID to, const Label &label, Color color, Weight weight) {
      edges_.emplace_back(from, to, label, color, weight);
      join(edges_.size() - 1);
    }

    void Graph::reconnect(void) {
      // Reallocate for the exact number of vertices.
      connections_.reset(new Connections(order()));

      // Reset all of the vertex degrees.  They will be recalculated as edges are reapplied.
      for_each(vertices_.begin(), vertices_.end(),
               [](Vertex &v) {
                 v.indeg_ = 0;
                 v.outdeg_ = 0;
               });

      // Add the edges.
      EdgeNumber m = size();
      for (EdgeNumber ie = 0; ie < m; ++ie) {
        join(ie);
      }
    }

    void Graph::join(EdgeNumber ie) {
      // Make sure that the specified edge exists.
      Edge &e = edges_.at(ie);

      // Make sure that the endpoints exists.
      VertexNumber out, in;
      if ( ! (find_vertex(e.from(), &out) && find_vertex(e.to(), &in)) ) {
        throw NoSuchVertexError();
      }

      // Make the connection in the connection matrix.
      connections_->at(out, in).push_back(ie);

      // Update the vertex degrees.
      Vertex &vout = vertices_.at(out);
      Vertex &vin = vertices_.at(in);
      ++vout.outdeg_;
      ++vin.indeg_;

      // Update the graph degree extrema.
      if (vout.outdeg_ < minoutdeg_) minoutdeg_ = vout.outdeg_;
      if (vout.outdeg_ > maxoutdeg_) maxoutdeg_ = vout.outdeg_;
      if (vin.indeg_ < minindeg_) minindeg_ = vin.indeg_;
      if (vin.indeg_ > maxindeg_) maxindeg_ = vin.indeg_;
    }

  }  // namespace graph
}  // namespace cavcom
