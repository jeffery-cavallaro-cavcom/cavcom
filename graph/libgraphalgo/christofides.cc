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
        Coloring next_coloring(coloring_);
        next_coloring.resize(coloring_.size() + 1);
        next_coloring.back().insert(mis.cbegin(), mis.cend());

        VertexNumbers next_vertices(vertices_);
        next_vertices.insert(mis.cbegin(), mis.cend());

        SimpleGraph next_subgraph(SimpleGraph(graph_, next_vertices, EdgeNumbers()), true);

        ChristofidesNode next(parent_, graph_, next_coloring, next_vertices, next_subgraph);
        next.execute();

        return true;
      }
    };

    Christofides::Christofides(const SimpleGraph &graph) : GraphAlgorithm(graph) {}

    bool Christofides::run() {
      chromatic_.clear();

      Coloring start;
      VertexNumbers none;
      SimpleGraph all(graph(), true);
      ChristofidesNode root(this, graph(), start, none, all);
      root.execute();

      return true;
    }

  }  // namespace graph
}  // namespace cavcom
