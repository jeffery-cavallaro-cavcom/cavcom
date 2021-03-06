#ifndef CAVCOM_GRAPH_LIBGRAPHCORE_GRAPH_H_
#define CAVCOM_GRAPH_LIBGRAPHCORE_GRAPH_H_

#include <algorithm>
#include <set>
#include <vector>

#include "connections.h"
#include "vertices.h"

namespace cavcom {
  namespace graph {
    // A values structure for adding edges.
    struct EdgeValues {
      VertexNumber from;
      VertexNumber to;
      Label label;
      Color color;
      Weight weight;
    };

    using EdgeValuesList = std::vector<EdgeValues>;

    // A set of edges by edge number.  Used during subgraph operations.
    using EdgeNumbers = std::set<EdgeNumber>;

    // A graph is a mathematical object consisting of vertices and edges.  Graphs can be simple or allow multiple
    // edges, can be undirected or directed, and can optionally support loop edges.
    class Graph {
     public:
      // Creates an empty graph with the specified number of unlabeled vertices (or the null graph if n = 0).
      explicit Graph(VertexNumber n = 0, bool directed = false, bool multiple = false, bool loops = false);

      // Creates a graph with the specified vertices and edges.  The vertex numbers in the edge values correspond to
      // positions in the vertex list.
      Graph(const VertexValuesList &vertices, const EdgeValuesList &edges,
            bool directed = false, bool multiple = false, bool loops = false);

      // Creates a copy of the specified graph with the same vertex set and with or without the edges.
      Graph(const Graph &source, bool noedges = false);

      // Creates an induced subgraph of the specified graph by keeping only the specified vertices and their
      // incident edges.
      Graph(const Graph &source, const VertexNumbers &vkeep);

      // Creates a subgraph of the specified graph by removing the specified vertices and edges.
      Graph(const Graph &source, const VertexNumbers &vremove, const EdgeNumbers &eremove);

      // Creates a copy of the specified graph, but with the two specified vertices contracted.  The contracted
      // vertex will be a new vertex with a new vertex ID and will have the label, color, and position of the "to"
      // vertex.  The contracted lists of the two vertices will be concatenated and then each of the contracted
      // vertices that was not already contracted is appended to the list.  Multiple edges are suppressed if not
      // allowed.  Any edges between the two contracted vertices are discarded.  Throws a same vertex contraction
      // error if the from and to vertices are equal.
      Graph(const Graph &source, VertexNumber from, VertexNumber to);

      // Creates a copy of the specified graph, but with all of the vertices in each subset contracted.  Empty and
      // singleton subsets are ignored.
      Graph(const Graph &source, const VertexNumbersList &contract);

      // Returns true for digraph edge semantics (otherwise undirected).
      bool directed(void) const { return connections_.directed(); }

      // Returns true if multiple edges (in the same direction in the case of digraphs) are allowed between the
      // same two endpoint vertices.
      bool multiple(void) const { return connections_.multiple(); }

      // Returns true if loop edges (on the same vertex) are allowed.
      bool loops(void) const { return connections_.loops(); }

      // Returns the number of vertices (order).
      VertexNumber order(void) const { return vertices_.size(); }

      // Returns the number of edges (size).
      EdgeNumber size(void) const { return edges_.size(); }

      // Returns true if the graph is a null graph (n=m=0).
      bool null(void) const { return (order() == 0); }

      // Returns true if the graph is an empty graph (m=0).
      bool empty(void) const { return (size() == 0); }

      // Gets a vertex by vertex number.  An invalid vertex number throws an out-of-range error.
      Vertex &vertex(VertexNumber number) { return vertices_[number]; }
      const Vertex &vertex(VertexNumber number) const { return vertices_[number]; }

      // Gets a vertex number by vertex ID or label.  Returns true if found.  Throws a lookup error if not found
      // and errors are enabled.
      bool find_vertex(VertexID id, VertexNumber *found, bool errors = false) const {
        return vertices_.find(id, found, errors);
      }

      bool find_vertex(Label label, VertexNumber *found, bool errors = false) const {
        return vertices_.find(label, found, errors);
      }

      // Converts a container of vertex IDs to vertex numbers and adds the vertex numbers to the specified vertex
      // number set.  Throws a lookup error for any unknown vertex error.
      template <typename T> void ids_to_numbers(const T &ids, VertexNumbers *numbers) const {
        for_each(ids.cbegin(), ids.cend(), [this, numbers](VertexID id){
                                             VertexNumber number;
                                             find_vertex(id, &number, true);
                                             numbers->insert(number);
                                           });
      }

      // Gets an edge by edge number.  An invalid vertex number throws an out-of-range error.
      Edge &edge(EdgeNumber number) { return edges_.at(number); }
      const Edge &edge(EdgeNumber number) const { return edges_.at(number); }

      // Returns the edges that join the specified vertices.  An invalid vertex number throws an out-of-range
      // error.
      const Edges &edges(VertexNumber from, VertexNumber to) const { return connections_.edges(from, to); }

      // Returns true if the specified vertices are adjacent.
      bool adjacent(VertexNumber from, VertexNumber to) const { return connections_.adjacent(from, to); }

      // Adds an edge between the specified endpoint vertices.  An invalid vertex number throws an out-of-range
      // error.  Attempting to add a multiple or loop edge throw an error if disabled.  Returns the resulting
      // number of edges joining the two endpoint vertices.
      Degree join(VertexNumber from, VertexNumber to,
                  const Label &label = Label(), Color color = NOCOLOR, Weight weight = FREE);

      // Adds an edge using the specified values.
      Degree join(const EdgeValues &values);

      // Adds all of the specified edges.
      void join(const EdgeValuesList &values);

      // Calculates the minimum and maximum degrees.  The results are not cached, so these calls should be
      // considered expensive.
      Degree minindeg(void) const { return connections_.minindeg(); }
      Degree maxindeg(void) const { return connections_.maxindeg(); }

      Degree minoutdeg(void) const { return connections_.minoutdeg(); }
      Degree maxoutdeg(void) const { return connections_.maxoutdeg(); }

      // Returns the degrees for a single vertex.  An invalid vertex number throws an out-of-range error.
      Degree indeg(VertexNumber vertex) const { return connections_.indeg(vertex); }
      Degree outdeg(VertexNumber vertex) const { return connections_.outdeg(vertex); }

     private:
      // The vertices and edges.
      Vertices vertices_;
      EdgeTable edges_;

      // The connection matrix.  Indexed by from vertex and to vertex, in that order.
      Connections connections_;

      // Calculates the new number of nodes in a graph assuming contraction.
      static VertexNumber after_contraction(VertexNumber start, const VertexNumbersList &contract);
    };

  }  // namespace graph
}  // namespace cavcom

#endif  // CAVCOM_GRAPH_LIBGRAPHCORE_GRAPH_H_
