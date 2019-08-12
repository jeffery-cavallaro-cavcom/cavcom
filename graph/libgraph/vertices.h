#ifndef CAVCOM_GRAPH_LIBGRAPH_VERTICES_H_
#define CAVCOM_GRAPH_LIBGRAPH_VERTICES_H_

#include <map>
#include <vector>

#include "vertex.h"

namespace cavcom {
  namespace graph {

    // The Vertices class implements the vertex set for a graph.
    class Vertices {
     public:
      using VertexList = std::vector<Vertex>;
      using VertexPos = VertexList::size_type;

      // Default constructor.  Creates a new empty vertex set.
      Vertices(void) : next_id_(0) {}

      // Returns the number of vertices in the set.
      VertexPos size(void) const { return vertices_.size(); }

      // Returns the vertex at the specified position with bounds checking.
      Vertex &operator[](VertexPos iv) { return vertices_.at(iv); }
      const Vertex &operator[](VertexPos iv) const { return vertices_.at(iv); }

      // Adds a new vertex to the vertex set.
      Vertex &push_back(void);

      // Finds a vertex by ID, or returns >= size if the vertex is not known.
      VertexPos find(Vertex::VertexID id);

     private:
      VertexList vertices_;

      using VertexMap = std::map<Vertex::VertexID, VertexPos>;
      VertexMap id_to_pos_;

      Vertex::VertexID next_id_;
    };

  }  // namespace graph
}  // namespace cavcom

#endif  // CAVCOM_GRAPH_LIBGRAPH_VERTICES_H_
