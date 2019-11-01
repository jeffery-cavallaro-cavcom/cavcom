#ifndef CAVCOM_GRAPH_LIBGRAPH_VERTICES_H_
#define CAVCOM_GRAPH_LIBGRAPH_VERTICES_H_

#include "vertex.h"
#include "labels.h"

namespace cavcom {
  namespace graph {

    // The vertices in a graph.
    class Vertices {
     public:
      // Creates a new, empty vertex table.
      Vertices(void);

      // A capacity hint for the number of vertices that will need to be stored.  This should be called prior to
      // adding a set of vertices in order to avoid needless reallocations.
      void reserve(VertexNumber n);

      // Returns the specified vertex by number.  Throws an out-of-range error if the vertex does not exist.
      Vertex &operator[](VertexNumber number) { return vertices_.at(number); }
      const Vertex &operator[](VertexNumber number) const { return vertices_.at(number); }

      // Adds a new isolated vertex to the vertex table.  Note that this forces the min/max degrees to 0.  The
      // vertex is assigned the next vertex ID.  Throws a duplicate label error if a label is specified that is
      // already assigned to an existing vertex.
      void add(const Label &label = Label(), Color color = BLACK);

      // Min and max degrees.  A synonym is provided for undirected graphs.
      Degree minindeg(void) const { return minindeg_; }
      Degree maxindeg(void) const { return maxindeg_; }
      
      Degree minoutdeg(void) const { return minoutdeg_; }
      Degree maxoutdeg(void) const { return maxoutdeg_; }

      Degree mindeg(void) const { return minoutdeg(); }
      Degree maxdeg(void) const { return maxoutdeg(); }

    private:
      // The vertices, indexed by vertex number.
      VertexTable vertices_;

      // The next invariant vertex ID to use for new vertices.
      VertexID next_;

      // Vertices can be found by ID or label.
      IDs id_to_number_;
      Labels label_to_number_;

      // The following values can be determined from the connection matrix; however, they are widely used in various
      // graph algorithms, and so they are stored here for efficiency.  Both input and output degrees are stored in
      // support of directed graphs.  Undirected graphs should use the out degrees.
      Degree minindeg_;
      Degree maxindeg_;
      Degree minoutdeg_;
      Degree maxoutdeg_;

      // Disable copy and assignment.
      Vertices(const Vertices &source) {}
      Vertices &operator=(const Vertices &source) { return *this; }
    };

  }  // namespace graph
}  // namespace cavcom

#endif  // CAVCOM_GRAPH_LIBGRAPH_VERTICES_H_
