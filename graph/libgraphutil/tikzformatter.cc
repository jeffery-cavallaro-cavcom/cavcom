#include "tikzformatter.h"

namespace cavcom {
  namespace graph {
    TikzFormatter::TikzFormatter(std::ostream *out) : Formatter<std::string>(out) {}

    bool TikzFormatter::start_graph(const Graph &graph) {
      *out_ << "\\begin{tikzpicture}" << std::endl;
      return out_->good();
    }

    bool TikzFormatter::finish_graph(const Graph &graph) {
      *out_ << "\\end{tikzpicture}" << std::endl;
      return out_->good();
    }

    bool TikzFormatter::format_vertex(const Vertex &vertex) {
      const std::string &color = get_color(vertex.color());
      *out_ << "  \\node";
      if (!color.empty()) *out_ << " [fill=" << color << "]";
      *out_ << " (" << vertex.id() << ") at ("
            << vertex.xpos() << "," << vertex.ypos() << ") {"
            << vertex.label() << "};"
            << std::endl;
      return out_->good();
    }

    bool TikzFormatter::format_edge(const Edge &edge) {
      const std::string &color = get_color(edge.color());
      *out_ << "  \\draw";
      if (!color.empty()) *out_ << " [" << color << "]";
      *out_ << " (" << edge.from() << ") edge";
      if (!edge.label().empty()) *out_ << " node [auto] {" << edge.label() << "}";
      if (edge.weight() > 0.0) *out_ << " node [swap] {\\(" << edge.weight() << "\\)}";
      *out_ << " (" << edge.to() << ");" << std::endl;
      return out_->good();
    }

  }  // namespace graph
}  // namespace cavcom
