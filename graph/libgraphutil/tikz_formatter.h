#ifndef CAVCOM_GRAPH_LIBGRAPHUTIL_TIKZ_FORMATTER_H_
#define CAVCOM_GRAPH_LIBGRAPHUTIL_TIKZ_FORMATTER_H_

#include <ostream>
#include <string>

#include "formatter.h"

namespace cavcom {
  namespace graph {

    // Format a graph to TikZ nodes and edges,.
    class TikzFormatter : public Formatter<std::string> {
     public:
      TikzFormatter(std::ostream *out);

     protected:
      // Starts a TikZ picture.
      virtual bool start_graph(const SimpleGraph &graph);

      // Starts a scope for either labeled or unlabeled vertices.
      virtual bool start_vertices(const SimpleGraph &graph);

      virtual bool format_vertex(const Vertex &vertex);

      // Finishes the vertex scope.
      virtual bool finish_vertices(const SimpleGraph &graph);

      virtual bool format_edge(const Edge &edge);

      // Finishes a TikZ picture.
      virtual bool finish_graph(const SimpleGraph &graph);

     private:
      // Styles for labeled and unlabeled vertices.
      static const std::string LABELED_STYLE;
      static const std::string UNLABELED_STYLE;
    };

  }  // namespace graph
}  // namespace cavcom

#endif  // CAVCOM_GRAPH_LIBGRAPHUTIL_TIKZ_FORMATTER_H_
