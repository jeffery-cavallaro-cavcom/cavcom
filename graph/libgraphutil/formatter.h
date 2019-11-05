#ifndef CAVCOM_GRAPH_LIBGRAPHUTIL_FORMATTER_H_
#define CAVCOM_GRAPH_LIBGRAPHUTIL_FORMATTER_H_

#include <ostream>
#include <map>

#include "graph.h"

namespace cavcom {
  namespace graph {

    // The base class for a formatter to output the contents of a graph for presentation purposes.  A color lookup
    // table is provided for coloring algorithm output.  The C template argument specifies how colors are
    // represented in the target format.  The output stream is provided via the constructor, and must remain open
    // and valid.  All format calls return true if the output stream is in a good state.
    template <typename C>
    class Formatter {
     public:
      // The color lookup table.
      using ColorMap = std::map<Color, C>;

      // Creates a new formatter instance for the specified output stream.
      Formatter(std::ostream *out) : out_(out) {}

      // Formats the specified graph for presentation.  The formatter will call the start graph method, then the
      // format vertex method for each vertex, then the format edge method for each edge, and then the end graph
      // method to finish the format operation.  Returns true if the current output stream is in a good state.
      bool format(const Graph &graph) {
        // Start a new format operation.
        if (!start_graph(graph)) return false;

        // Format each vertex.
        VertexNumber n = graph.order();
        for (VertexNumber iv = 0; iv < n; ++iv) {
          if (!format_vertex(graph.vertex(iv))) return false;
        }

        // Format each edge.
        EdgeNumber m = graph.size();
        for (EdgeNumber ie = 0; ie < m; ++ie) {
          if (!format_edge(graph.edge(ie))) return false;
        }

        // Finish the format operation.
        return finish_graph(graph);
      }

      // Starts/finishes a graph formatting operation.  This allows the derived formatter to output any needed
      // header and trailer information.
      virtual bool start_graph(const Graph &graph) = 0;
      virtual bool finish_graph(const Graph &graph) = 0;

      // Formats the next vertex/edge to the current output stream.
      virtual bool format_vertex(const Vertex &vertex) = 0;
      virtual bool format_edge(const Edge &edge) = 0;

     protected:
      std::ostream *out_;

     private:
      ColorMap colors_;
    };

  }  // namespace graph
}  // namespace cavcom

#endif  // CAVCOM_GRAPH_LIBGRAPHUTIL_FORMATTER_H_
