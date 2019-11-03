#ifndef CAVCOM_GRAPH_LIBGRAPH_ERRORS_H_
#define CAVCOM_GRAPH_LIBGRAPH_ERRORS_H_

#include <sstream>
#include <stdexcept>
#include <string>

#include "vertex.h"

namespace cavcom {
  namespace graph {

    // An attempt to add a multiple edge when multiple edges are disabled.
    class MultipleEdgeError : public std::runtime_error {
     public:
      MultipleEdgeError(VertexNumber from, VertexNumber to, EdgeNumber edge);
      virtual const char *what(void) const noexcept { return msg_.c_str(); }
      const VertexNumber from(void) const { return from_; }
      const VertexNumber to(void) const { return to_; }
      const EdgeNumber edge(void) const { return edge_; }

     private:
      const VertexNumber from_;
      const VertexNumber to_;
      const EdgeNumber edge_;
      std::string msg_;
    };

    // An attempt to add a loop edge when loop edges are disabled.
    class LoopEdgeError : public std::runtime_error {
     public:
      LoopEdgeError(VertexNumber vertex, EdgeNumber edge);
      virtual const char *what(void) const noexcept { return msg_.c_str(); }
      const VertexNumber vertex(void) const { return vertex_; }
      const EdgeNumber edge(void) const { return edge_; }

     private:
      const VertexNumber vertex_;
      const EdgeNumber edge_;
      std::string msg_;
    };

  }  // namespace graph
}  // namespace cavcom

#endif  // CAVCOM_GRAPH_LIBGRAPH_ERRORS_H_
