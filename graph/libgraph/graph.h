#ifndef CAVCOM_GRAPH_LIBGRAPH_GRAPH_H_
#define CAVCOM_GRAPH_LIBGRAPH_GRAPH_H_

#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "matrix.h"

#include "vertex.h"

namespace cavcom {
  namespace graph {
    // Thrown when attempting to join an unknown vertex with an edge.
    class NoSuchVertexError : public std::runtime_error {
     public:
      NoSuchVertexError(void) : runtime_error("No such vertex") {}
    };

    // Thrown when attempting label a vertex with an already existing label.
    class DuplicateLabelError : public std::runtime_error {
     public:
      DuplicateLabelError(void) : runtime_error("Duplicate label") {}
    };

    // This is the base class for all graphs.  A graph consists of vertices and edges that join vertices.
    class Graph {
     public:
      // The number of vertices (order).
      VertexNumber order(void) const { return vertices_.size(); }

      // The number of edges (size).
      EdgeNumber size(void) const { return edges_.size(); }

      // Returns a vertex by position, with range checking.
      Vertex &operator[](VertexNumber i) { return vertices_.at(i); }
      const Vertex &operator[](VertexNumber i) const { return vertices_.at(i); }
      Vertex &vertex(VertexNumber i) { return (*this)[i]; }
      const Vertex &vertex(VertexNumber i) const { return (*this)[i]; }

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
      // Default constructor.  Creates a null graph with the specified number of isolated nodes, or a null graph if
      // a node count is not specified.  Graphs can only be created by a derived type that enforces whether the
      // graph is simple or multi and undirected or directed.
      explicit Graph(VertexNumber n = 0)
        : next_(0), connections_(new Connections(0)), minindeg_(0), maxindeg_(0), minoutdeg_(0), maxoutdeg_(0) {}

      // Different derived graph types will interpret these differently, so they are protected.
      Degree minindeg(void) const { return minindeg_; }
      Degree maxindeg(void) const { return maxindeg_; }
      Degree minoutdeg(void) const { return minoutdeg_; }
      Degree maxoutdeg(void) const { return maxoutdeg_; }

      // The rules for adding edges are enforced by the derived classes.  Derived classes can call this routine to
      // append an edge to the edge list, join the endpoint vertices, and update the various degree attributes.
      void add_edge(VertexID from, VertexID to,
                    const Label &label = std::string(), Color color = BLACK, Weight weight = 0.0);

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
      Degree minindeg_;
      Degree maxindeg_;
      Degree minoutdeg_;
      Degree maxoutdeg_;

      // Reconstructs the connection matrix from the current vertex and edge lists.
      void reconnect(void);

      // Uses the edge at the specified offset in the edge table to join its endpoint vertices and update all of the
      // relevant degree attributes.
      void join(EdgeNumber ie);
    };

  }  // namespace graph
}  // namespace cavcom

#endif  // CAVCOM_GRAPH_LIBGRAPH_GRAPH_H_
