#include "bron2.h"

#include "christofides.h"

namespace cavcom {
  namespace graph {

    class ChristofidesNode : public Bron2 {
     public:
      using Coloring = Christofides::Coloring;

      ChristofidesNode(Christofides *parent,
                       const SimpleGraph &graph,
                       const Coloring &coloring,
                       VertexNumbers &vertices,
                       const SimpleGraph &subgraph)
        : Bron2(subgraph), parent_(parent), graph_(graph), coloring_(coloring), vertices_(vertices) {}

     private:
      Christofides *parent_;
      const SimpleGraph &graph_;
      const Coloring &coloring_;
      VertexNumbers &vertices_;

      virtual bool found(const Clique &mis) {
        parent_->add_call();

        // Use a new color for the current MIS.
        Coloring next_coloring(coloring_);
        next_coloring.resize(coloring_.size() + 1);
        next_coloring.back().insert(mis.cbegin(), mis.cend());

        // If all the vertices have been used then the coloring is complete.
        if (vertices_.size() + mis.size() >= graph_.order()) {
          parent_->chromatic_ = next_coloring;
          parent_->done_call();
          return false;
        }

        // Add the current MIS to the set of used vertices.
        VertexNumbers next_vertices(vertices_);
        next_vertices.insert(mis.cbegin(), mis.cend());

        // Remove all used vertices from the original graph.
        SimpleGraph next_subgraph(graph_, next_vertices, EdgeNumbers());

        // Repeat with MISs from the next subgraph.
        ChristofidesNode next(parent_, graph_, next_coloring, next_vertices, next_subgraph);
        bool more =  next.execute();
        parent_->done_call();
        return more;
      }
    };

    Christofides::Christofides(const SimpleGraph &graph) : GraphAlgorithm(graph) {}

    bool Christofides::run() {
      chromatic_.clear();

      // Guard against a null graph.
      if (graph.order() == 0) return;

      // A MIS in a graph G is a clique in the complement of G.
      SimpleGraph complement(graph(), true);

      // Start with an empty coloring.
      Coloring start;
      VertexNumbers none;

      // Execute the Bron algorithm to add MISs until all vertices are used.
      ChristofidesNode root(this, complement, start, none, complement);
      root.execute();

      return true;
    }

  }  // namespace graph
}  // namespace cavcom
