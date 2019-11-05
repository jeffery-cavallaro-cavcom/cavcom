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

      virtual bool start_graph(const Graph &graph);
      virtual bool finish_graph(const Graph &graph);
      virtual bool format_vertex(const Vertex &vertex);
      virtual bool format_edge(const Edge &edge);
    };

  }  // namespace graph
}  // namespace cavcom

#endif  // CAVCOM_GRAPH_LIBGRAPHUTIL_TIKZFORMATTER_H_
