#ifndef CAVCOM_GRAPH_LIBGRAPHCORE_VERTICES_H_
#define CAVCOM_GRAPH_LIBGRAPHCORE_VERTICES_H_

#include <set>
#include <vector>

#include "lookup.h"

#include "vertex.h"

namespace cavcom {
  namespace graph {
    // A list of vertex attributes for defining vertices.
    using VertexValuesList = std::vector<VertexValues>;

    // A set of vertices by vertex number.  Used during subgraph operations.
    using VertexNumbers = std::set<VertexNumber>;

    // A list of vertex subsets, used by various partitioning algorithms.
    using VertexNumbersList = std::vector<VertexNumbers>;

    // The vertices in a graph.
    class Vertices {
     public:
      // Public for access to exception types.
      using IDs = cavcom::utility::Lookup<VertexID, VertexNumber>;
      using Labels = cavcom::utility::Lookup<Label, VertexNumber>;

      // Creates a new, empty vertex table.
      Vertices(void);

      // Copies a vertex table, either keeping or removing the vertices in the specified set.
      Vertices(const Vertices &source, bool keep, const VertexNumbers &vertices);

      // Copies a vertex table, either keeping or removing the vertices in the specified list.  Empty and singleton
      // subsets are ignored.
      Vertices(const Vertices &source, bool keep, const VertexNumbersList &vertices);

      // Returns the number of vertices in the table.
      VertexNumber size(void) const { return vertices_.size(); }

      // A capacity hint for the number of vertices that will need to be stored.  This should be called prior to
      // adding a set of vertices in order to avoid needless reallocations.
      void reserve(VertexNumber n);

      // Returns the specified vertex by number.  Throws an out-of-range error if the vertex does not exist.
      Vertex &operator[](VertexNumber number) { return vertices_.at(number); }
      const Vertex &operator[](VertexNumber number) const { return vertices_.at(number); }

      // Adds a new isolated vertex to the vertex table.  The vertex is assigned the next vertex ID.  Throws a
      // duplicate label error if a label is specified that is already assigned to an existing vertex.
      void add(const Label &label = Label(), Color color = NOCOLOR, Dimension xpos = 0.0, Dimension ypos = 0.0);
      void add(const VertexValues &values);

      // Adds multiple isolated vertices to the vertex table.
      void add(const VertexValuesList &values);

      // Find vertex by ID or label.  Returns true if found.  Throws a lookup error if not found and errors are
      // enabled.
      bool find(VertexID id, VertexNumber *number, bool errors = false) const;
      bool find(Label label, VertexNumber *number, bool errors = false) const;

      // Changes (or sets) the label of an existing vertex.  An empty label results in an unlabeled vertex.
      // Attempting to set the label to the same value does nothing.  Throws a duplicate label error if the
      // specified new label is already in use by a different vertex.  Any old label is discarded and is available
      // for future use.  An invalid vertex number throws an out-of-range error.
      void label(VertexNumber number, const Label &newlabel = Label());

     private:
      // The vertices, indexed by vertex number.
      VertexTable vertices_;

      // The next invariant vertex ID to use for new vertices.
      VertexID next_;

      // Vertices can be found by ID or label.
      IDs id_to_number_;
      Labels label_to_number_;
    };

  }  // namespace graph
}  // namespace cavcom

#endif  // CAVCOM_GRAPH_LIBGRAPHCORE_VERTICES_H_
