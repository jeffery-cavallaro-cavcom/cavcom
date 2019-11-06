#ifndef CAVCOM_GRAPH_LIBGRAPHUTIL_FORMATTER_H_
#define CAVCOM_GRAPH_LIBGRAPHUTIL_FORMATTER_H_

#include <map>
#include <ostream>
#include <string>

#include "graph.h"

namespace cavcom {
  namespace graph {

    // The base class for a formatter to output the contents of a graph for presentation purposes.  A color lookup
    // table is provided for coloring algorithm output.  The C template argument specifies how colors are
    // represented in the target format.  The output stream is provided via the constructor, and must remain open
    // and valid.  All format calls return true if the output stream is in a good state.
    template <typename C = std::string>
    class Formatter {
     public:
      using ColorType = C;

      // The color lookup table.
      using ColorMap = std::map<Color, ColorType>;

      // Creates a new formatter instance for the specified output stream and the specified default color.
      Formatter(std::ostream *out, const C &defcolor = ColorType())
        : out_(out), defcolor_(defcolor),
          vertex_labels_(false), vertex_colors_(false),
          edge_labels_(false), edge_colors_(false), edge_weights_(false) {}

      virtual ~Formatter() {}

      // The following are used to enable/disable the output of various vertex and edge attributes.  All attributes
      // are disabled by default in a new formatter.
      bool vertex_labels(void) const { return vertex_labels_; }
      void vertex_labels(bool enabled = true) { vertex_labels_ = enabled; }

      bool vertex_colors(void) const { return vertex_colors_; }
      void vertex_colors(bool enabled = true) { vertex_colors_ = enabled; }

      bool edge_labels(void) const { return edge_labels_; }
      void edge_labels(bool enabled = true) { edge_labels_ = enabled; }

      bool edge_colors(void) const { return edge_colors_; }
      void edge_colors(bool enabled = true) { edge_colors_ = enabled; }

      bool edge_weights(void) const { return edge_weights_; }
      void edge_weights(bool enabled = true) { edge_weights_ = enabled; }

      // Formats the specified graph for presentation.  The formatter will call the start graph method, then the
      // format vertex method for each vertex, then the format edge method for each edge, and then the end graph
      // method to finish the format operation.  Returns true if the current output stream is in a good state.
      bool format(const Graph &graph) {
        // Start a new format operation.
        if (!start_graph(graph)) return false;

        // Format each vertex.
        if (!start_vertices(graph)) return false;
        VertexNumber n = graph.order();
        for (VertexNumber iv = 0; iv < n; ++iv) {
          if (!format_vertex(graph.vertex(iv))) return false;
        }
        if (!finish_vertices(graph)) return false;

        // Format each edge.
        if (!start_edges(graph)) return false;
        EdgeNumber m = graph.size();
        for (EdgeNumber ie = 0; ie < m; ++ie) {
          if (!format_edge(graph.edge(ie))) return false;
        }
        if (!finish_edges(graph)) return false;

        // Finish the format operation.
        return finish_graph(graph);
      }

      // The following functions are called in the listed order to format a graph.  Only the vertex and edge
      // formatter are required.  Derived class functions should output the necessary information to the
      // protected out_ stream according to the settings of the various flags.
      virtual bool start_graph(const Graph &graph) {return out_->good(); };
      virtual bool start_vertices(const Graph &graph) { return out_->good(); };
      virtual bool format_vertex(const Vertex &vertex) = 0;
      virtual bool finish_vertices(const Graph &graph) { return out_->good(); };
      virtual bool start_edges(const Graph &graph) { return out_->good(); };
      virtual bool format_edge(const Edge &edge) = 0;
      virtual bool finish_edges(const Graph &graph) { return out_->good(); };
      virtual bool finish_graph(const Graph &graph) { return out_->good(); };

      // Adds/replaces a color in the color map.  A color ID of 0 changes the default color.
      void add_color(Color id, const ColorType &value) {
        if (id == 0) {
          defcolor_ = value;
          return;
        }
        auto &ipos = colors_.insert(colors_.cbegin(), id, value);
        if (ipos->second != value) ipos->second = value;
      }

      // Removes a color from the color map.
      void remove_color(Color id) {
        colors_.erase(id);
      }

      // Returns the color associated with the specified ID, or the default color if the ID is not known.
      const ColorType &get_color(Color id) const {
        const auto &ifound = colors_.find(id);
        return (ifound == colors_.cend() ? defcolor_ : ifound->second);
      }

     protected:
      std::ostream *out_;

     private:
      ColorMap colors_;
      ColorType defcolor_;

      bool vertex_labels_;
      bool vertex_colors_;
      bool edge_labels_;
      bool edge_colors_;
      bool edge_weights_;
    };

  }  // namespace graph
}  // namespace cavcom

#endif  // CAVCOM_GRAPH_LIBGRAPHUTIL_FORMATTER_H_
