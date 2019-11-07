#include "tikz_formatter.h"

namespace cavcom {
  namespace graph {
    const std::string TikzFormatter::LABELED_STYLE = "labeled node/.style={draw,circle,minimum size=2em}";

    const std::string TikzFormatter::UNLABELED_STYLE =
      "unlabeled node/.style={draw,circle,fill=#1,inner sep=0cm,minimum size=0.2cm}"
      ", unlabeled node/.default={black}";

    TikzFormatter::TikzFormatter(std::ostream *out) : Formatter<std::string>(out) {}

    bool TikzFormatter::start_graph(const Graph &graph) {
      *out_ << "\\begin{tikzpicture}" << std::endl;
      return out_->good();
    }

    bool TikzFormatter::start_vertices(const Graph &graph) {
      *out_ << "  \\begin{scope}[" << (vertex_labels() ? LABELED_STYLE : UNLABELED_STYLE) << "]" << std::endl;
      return out_->good();
    }

    bool TikzFormatter::format_vertex(const Vertex &vertex) {
      //      const std::string &color = get_color(vertex.color());
      //      *out_ << "  \\node";
      //      if (!color.empty()) *out_ << " [fill=" << color << "]";
      //      *out_ << " (" << vertex.id() << ") at ("
      //            << vertex.xpos() << "," << vertex.ypos() << ") {"
      //            << vertex.label() << "};"
      //            << std::endl;
      return out_->good();
    }

    bool TikzFormatter::finish_vertices(const Graph &graph) {
      *out_ << "  \\end{scope}" << std::endl;
      return out_->good();
    }

    bool TikzFormatter::format_edge(const Edge &edge) {
      //      const std::string &color = get_color(edge.color());
      //      *out_ << "  \\draw";
      //      if (!color.empty()) *out_ << " [" << color << "]";
      //      *out_ << " (" << edge.from() << ") edge";
      //      if (!edge.label().empty()) *out_ << " node [auto] {" << edge.label() << "}";
      //      if (edge.weight() > 0.0) *out_ << " node [swap] {\\(" << edge.weight() << "\\)}";
      //      *out_ << " (" << edge.to() << ");" << std::endl;
      return out_->good();
    }

    bool TikzFormatter::finish_graph(const Graph &graph) {
      *out_ << "\\end{tikzpicture}" << std::endl;
      return out_->good();
    }

  }  // namespace graph
}  // namespace cavcom
