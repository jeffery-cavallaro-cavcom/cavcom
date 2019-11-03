#ifndef CAVCOM_GRAPH_LIBGRAPH_GRAPH_H_
#define CAVCOM_GRAPH_LIBGRAPH_GRAPH_H_

namespace cavcom {
  namespace graph {
    class NoSuchVertexNumberError : public std::runtime_error {
     public:
      NoSuchVertexError(void) : runtime_error("No such vertex") {}
    };

    // This is the base class for all graphs.  A graph consists of vertices and edges that join vertices.
    class Graph {
     public:
      // Returns a vertex by position, with range checking.
      Vertex &operator[](VertexNumber i) { return vertices_[i]; }
      const Vertex &operator[](VertexNumber i) const { return vertices_[i]; }

      // Finds a vertex number by vertex ID or label.  Returns order() if not found.
      bool find_vertex(VertexID id, VertexNumber *number) const;
      bool find_vertex(const Label &label, VertexNumber *number) const;

      // Returns the edge list for the specified from and to endpoints, with range checking.
      const Edges &edges(VertexNumber from, VertexNumber to) const { return connections_->at(from, to); }

      // Returns the specified edge between the specified two vertices, with range checking.
      EdgeNumber edge(VertexNumber from, VertexNumber to, Degree i) { return (connections_->at(from, to)).at(i); }

      // Returns true if there is an edge between the two vertices, with range checking.
      bool adjacent(VertexNumber from, VertexNumber to) const { return !edges(from, to).empty(); }

      // Adds the specified number of isolated vertices to the graph and then adjusts the connection matrix.  The
      // vertices are initialized with default attributes.
      void add_vertices(VertexNumber n);

      // Values for constructing vertices.
      struct VertexValues {
        Label label;
        Color color;
      };

      using VertexValuesList = std::vector<VertexValues>;

      // Adds isolated vertices using the specified values.
      void add_vertices(const VertexValuesList &vertices);

      // Labels the specified vertex.
      void label_vertex(VertexNumber iv, const std::string &label);

     protected:
      // The rules for adding edges are enforced by the derived classes.  Derived classes can call this routine to
      // append an edge to the edge list, join the endpoint vertices, and update the various degree attributes.
      void add_edge(VertexID from, VertexID to,
                    const Label &label = std::string(), Color color = BLACK, Weight weight = 0.0);

     private:
      // Uses the edge at the specified offset in the edge table to join its endpoint vertices and update all of the
      // relevant degree attributes.
      void join(EdgeNumber ie);
    };

  }  // namespace graph
}  // namespace cavcom

#endif  // CAVCOM_GRAPH_LIBGRAPH_GRAPH_H_
