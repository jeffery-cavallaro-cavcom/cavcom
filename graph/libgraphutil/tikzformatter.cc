#include "tikzformatter.h"

namespace cavcom {
  namespace graph {
    TikzFormatter::TikzFormatter(std::ostream *out) : Formatter(out) {}

    bool TikzFormatter::start_graph(const Graph &graph) {
      *out_ << "\\begin{tikzpicture}" << std::endl;
      return out_->good();
    }

    bool TikzFormatter::finish_graph(const Graph &graph) {
      return out_->good();
    }

    bool TikzFormatter::format_vertex(const Vertex &vertex) {
      return out_->good();
    }

    bool TikzFormatter::format_edge(const Edge &edge) {
      *out_ << "\\end{tikzpicture}" << std::endl;
      return out_->good();
    }

  }  // namespace graph
}  // namespace cavcom
