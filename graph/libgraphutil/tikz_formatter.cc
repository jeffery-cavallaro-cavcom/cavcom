#include "tikz_formatter.h"

namespace cavcom {
  namespace graph {
    const std::string TikzFormatter::LABELED_STYLE = "every node/.style={draw,circle,minimum size=2em}";

    const std::string TikzFormatter::UNLABELED_STYLE =
      "every node/.style={draw,circle,fill=#1,inner sep=0cm,minimum size=0.2cm}"
      ", every node/.default={black}";

    TikzFormatter::TikzFormatter(std::ostream *out) : Formatter<std::string>(out) {}

    bool TikzFormatter::start_graph(const SimpleGraph &graph) {
      *out_ << "\\begin{tikzpicture}" << std::endl;
      return out_->good();
    }

    bool TikzFormatter::start_vertices(const SimpleGraph &graph) {
      *out_ << "\\begin{scope}[" << (vertex_labels() ? LABELED_STYLE : UNLABELED_STYLE) << "]" << std::endl;
      return out_->good();
    }

    bool TikzFormatter::format_vertex(const Vertex &vertex) {
      *out_ << "\\node";

      if (vertex_colors()) {
        const ColorType &color = get_color(vertex.color());
        if (!color.empty()) *out_ << " [fill=" << color << "]";
      }

      *out_ << " (" << vertex.id() << ") at (" << vertex.xpos() << "," << vertex.ypos() << ") {";

      if (vertex_labels()) {
        const Label &label = vertex.label();
        if (label.empty()) {
          *out_ << "\\(" << vertex.id() << "\\)";
        } else {
          *out_ << label;
        }
      }

      *out_ << "};" << std::endl;

      return out_->good();
    }

    bool TikzFormatter::finish_vertices(const SimpleGraph &graph) {
      *out_ << "\\end{scope}" << std::endl;
      return out_->good();
    }

    bool TikzFormatter::format_edge(const Edge &edge) {
      *out_ << "\\draw";

      if (edge_colors()) {
        const ColorType &color = get_color(edge.color());
        if (!color.empty()) *out_ << " [" << color << "]";
      }

      *out_ << " (" << edge.from() << ") edge";

      if (edge_labels()) {
        const Label &label = edge.label();
        if (!label.empty()) *out_ << " node [auto] {" << edge.label() << "}";
      }

      if (edge_weights()) *out_ << " node [auto,swap] {\\(" << edge.weight() << "\\)}";

      *out_ << " (" << edge.to() << ");" << std::endl;

      return out_->good();
    }

    bool TikzFormatter::finish_graph(const SimpleGraph &graph) {
      *out_ << "\\end{tikzpicture}" << std::endl;
      return out_->good();
    }

  }  // namespace graph
}  // namespace cavcom
