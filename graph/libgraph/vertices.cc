#include "vertices.h"

namespace cavcom {
  namespace graph {

    Vertex &Vertices::push_back(void) {
      Vertex::VertexID id = next_id_++;
      VertexPos pos = vertices_.size();
      vertices_.emplace_back();
      Vertex &vertex = vertices_[pos];
      vertex.id(id);
      id_to_pos_.emplace(id, pos);
      return vertex;
    }

    Vertices::VertexPos Vertices::find(Vertex::VertexID id) {
      VertexMap::iterator where = id_to_pos_.find(id);
      return (where != id_to_pos_.end()) ? where->second : vertices_.size();
    }

  }  // namespace graph
}  // namespace cavcom
