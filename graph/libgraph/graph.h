#ifndef CAVCOM_GRAPH_LIBGRAPH_GRAPH_H_
#define CAVCOM_GRAPH_LIBGRAPH_GRAPH_H_

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "types.h"
#include "matrix.h"

namespace cavcom {
  namespace graph {

    // This is the base class for all graphs.  A graph consists of vertices and edges that join vertices.
    class Graph {
     public:
      // Vertex and edge labels are strings.
      using Label = std::string;

      // Vertex and edge colors are represented by unsigned integer values.  These values can be used as indexes
      // into an external color array for application-specific color assignment.  The default color value is 0,
      // which should correspond to black.
      using Color = uint;
      static constexpr Color BLACK = 0;

      // Edges can be assign weights.  An edge's weight can be interpreted as the cost to traverse the edge between
      // the endpoint vertices.
      using Weight = double;

      // Vertices are stored in the vertex table and are identified by a vertex ID that is invariant across graph
      // mutations.  Thus, a vertex that changes its position in the connection matrix is still identifiable with
      // the corresponding vertex in the original graph.  Vertices in a particular graph are also identified by
      // their position in the vertex table, which corresponds to their position in the connection matrix.
      class Vertex;
      using VertexTable = std::vector<Vertex>;
      using VertexID = VertexTable::size_type;
      using VertexNumber = VertexTable::size_type;

      // Edges are stored in an edge table and are referenced by index in the connection matrix.
      class Edge;
      using EdgeTable = std::vector<Edge>;
      using EdgeNumber = EdgeTable::size_type;
      using Edges = std::vector<EdgeNumber>;

      // The degree of a vertex is the number of edges incident to a vertex.
      using Degree = Edges::size_type;

      // Each vertex also maintains a list of vertex IDs that track vertex contractions.  An empty contracted list
      // indicates that the vertex is not the result of any contractions.
      using Contracted = std::vector<VertexID>;

      // A single vertex in a graph.  Although a vertex's in and out degrees can be determined from the connection
      // matrix, they are stored as attributes for efficiency.
      class Vertex {
       public:
        // Accessors and mutators.
        VertexID id(void) const { return id_; }
        const Label &label(void) const { return label_; }

        const Color color(void) const { return color_; }
        void color(Color color) { color_ = color; }

        Degree indeg(void) const { return indeg_; }
        Degree outdeg(void) const { return outdeg_; }

        const Contracted &contracted(void) const { return contracted_; }

       private:
        VertexID id_;
        Label label_;
        Color color_;
        Degree indeg_;
        Degree outdeg_;
        Contracted contracted_;

        // Creates a new isolated vertex with the specified label and color.
        Vertex(const Label &label = std::string(), Color color = BLACK)
          : id_(0), label_(label), color_(color), indeg_(0), outdeg_(0) {}

        // The parent graph needs access during graph mutations.
        friend class Graph;
      };

      // A single edge, used to join endpoint vertices.  Edges can have an identifying label, a color, and a
      // weight.  The color can be used to partition edges.  The weight can be interpreted as the cost of
      // traversing the edge between its endpoint vertices.  Whether or not an edge is directed depends on the
      // parent graph.
      class Edge {
       public:
        // Accessors and mutators.  Only the parent graph can set the endpoints.
        VertexID from(void) const { return from_; }
        VertexID to(void) const { return to_; }

        const Label &label(void) const { return label_; }
        void label(const Label &label) { label_ = label; }

        const Color color(void) const { return color_; }
        void color(Color color) { color_ = color; }

        const Weight weight(void) const { return weight_; }
        void weight(Weight weight) { weight_ = weight; }

       private:
        VertexID from_;
        VertexID to_;
        Label label_;
        Color color_;
        Weight weight_;

        // Creates a new edge with the specified label and weight between the specified endpoints.
        Edge(VertexID from, VertexID to, const Label &label = std::string(), Color color = BLACK, Weight weight = 0.0)
          : from_(from), to_(to), label_(label), color_(color), weight_(weight) {}

        // The parent graph needs access during graph mutations.
        friend class Graph;
      };

     public:
      // The number of vertices (order).
      VertexNumber order(void) const { return vertices_.size(); }

      // The number of edges (size).
      EdgeNumber size(void) const { return edges_.size(); }

      // The minimum vertex degree in the graph, usually known as small-delta.
      Degree mindeg(void) const { return mindeg_; }

      // The maximum vertex degree in the graph, usually known as capital-delta.
      Degree maxdeg(void) const { return maxdeg_; }

      // Returns a vertex by position, with range checking.
      Vertex &operator[](VertexNumber i) { return vertices_.at(i); }
      Vertex &vertex(VertexNumber i) { return (*this)[i]; }

      // Finds a vertex number by vertex ID or label.
      VertexNumber find_vertex(VertexID id) const;
      VertexNumber find_vertex(Label label) const;

      // Returns the edge list for the specified from and to endpoints, with range checking.
      const Edges &edges(VertexNumber from, VertexNumber to) const { return connections_->at(from, to); }

      // Returns the specified edge between the specified two vertices, with range checking.
      EdgeNumber edge(VertexNumber from, VertexNumber to, Degree i) { return (connections_->at(from, to))[i]; }

      // Returns true if there is an edge between the two vertices, with range checking.
      bool adjacent(VertexNumber from, VertexNumber to) const { return !edges(from, to).empty(); }

      // Adds the specified number of isolated vertices to the graph and then adjusts the connection matrix.  The
      // vertices are initialized with default attributes.
      void add_vertices(VertexNumber n);

     protected:
      // Default constructor.  Creates a null graph with the specified number of isolated nodes, or a null graph if
      // a node count is not specified.  Graphs can only be created by a derived type that enforces whether the
      // graph is simple or multi and undirected or directed.
      explicit Graph(VertexNumber n = 0) : next_(0), connections_(new Connections(0)), mindeg_(0), maxdeg_(0) {}

     private:
      // The vertices, in no particular order.
      VertexTable vertices_;

      // The next invariant vertex ID to use for new vertices.
      VertexID next_;

      // Vertices can be found by ID or label.
      using VertexByID = std::map<VertexID, VertexNumber>;
      VertexByID id_to_number_;

      using VertexByLabel = std::map<Label, VertexNumber>;
      VertexByLabel label_to_number_;

      // The edges, in no particular order.
      EdgeTable edges_;

      // The connection matrix.  Indexed by from vertex and to vertex, in that order.
      using Connections = cavcom::utility::Matrix<Edges>;
      using ConnectionsPtr = std::unique_ptr<Connections>;
      ConnectionsPtr connections_;

      // The following values can be determined from the connection matrix; however, they are stored as attributes
      // for efficiency.
      Degree mindeg_;
      Degree maxdeg_;

      // Reconstructs the connection matrix from the current vertex and edge lists.
      void reconnect(void);
    };

  }  // namespace graph
}  // namespace cavcom

#endif  // CAVCOM_GRAPH_LIBGRAPH_GRAPH_H_
