#ifndef CAVCOM_GRAPH_LIBGRAPHUTIL_TIKZFORMATTER_H_
#define CAVCOM_GRAPH_LIBGRAPHUTIL_TIKZFORMATTER_H_

#include <ostream>
#include <string>

#include "formatter.h"

namespace cavcom {
  namespace graph {

    // Format a graph to tikz nodes and edges,.
    class TikzFormatter : public Formatter<std::string> {
     public:
      TikzFormatter(std::ostream *out);

     protected:
      virtual bool start_graph(const Graph &graph);
      virtual bool start_vertices(const Graph &graph);
      virtual bool format_vertex(const Vertex &vertex);
      virtual bool end_vertices(const Graph &graph);
      virtual bool format_edge(const Edge &edge);
      virtual bool finish_graph(const Graph &graph);

     private:
      // Styles for labeled and unlabeled vertices.
      static const std::string UNLABELED_STYLE;
      static const std::string LABELED_NODE;
    };

  }  // namespace graph
}  // namespace cavcom

#endif  // CAVCOM_GRAPH_LIBGRAPHUTIL_TIKZFORMATTER_H_
