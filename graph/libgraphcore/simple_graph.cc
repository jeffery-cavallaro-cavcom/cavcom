#include "simple_graph.h"

namespace cavcom {
  namespace graph {

    SimpleGraph::SimpleGraph(VertexNumber n) : Graph(n, false, false, false) {}

    SimpleGraph::SimpleGraph(const VertexValuesList &vertices, const EdgeValuesList &edges)
      : Graph(vertices, edges, false, false, false) {}

    SimpleGraph::SimpleGraph(const SimpleGraph &source, bool complement) : Graph(source, complement) {
      VertexNumber n = order();
      if (!complement || (n < 2)) return;
      VertexNumber last = n - 1;
      for (VertexNumber i = 0; i < last; ++i) {
        for (VertexNumber j = i + 1; j < n; ++j) {
          if (!source.adjacent(i, j)) join(i, j);
        }
      }
    }

    SimpleGraph::SimpleGraph(const SimpleGraph &source, const VertexNumbers &vkeep) : Graph(source, vkeep) {}

    SimpleGraph::SimpleGraph(const SimpleGraph &source, const VertexNumbers &vremove, const EdgeNumbers &eremove)
      : Graph(source, vremove, eremove) {}

    SimpleGraph::SimpleGraph(const SimpleGraph &source, VertexNumber from, VertexNumber to)
      : Graph(source, from, to) {}

    SimpleGraph::SimpleGraph(const SimpleGraph &source, const VertexNumbersList &contract)
      : Graph(source, contract) {}

    bool SimpleGraph::complete(void) const {
      VertexNumber n = order();
      if (n < 2) return true;
      VertexNumber universal = n - 1;
      for (VertexNumber i = 0; i < n; ++i) {
        if (degree(i) < universal) return false;
      }
      return true;
    }

    void SimpleGraph::make_complete(void) {
      VertexNumber n = order();
      if (n < 2) return;
      VertexNumber last = n - 1;
      for (VertexNumber i = 0; i < last; ++i) {
        for (VertexNumber j = i + 1; j < n; ++j) {
          if (!adjacent(i, j)) join(i, j);
        }
      }
    }

    bool SimpleGraph::proper(void) const {
      VertexNumber n = order();
      VertexNumber last = n - 1;
      if (vertex(last).color() == NOCOLOR) return false;
      for (VertexNumber i = 0; i < last; ++i) {
        Color ci = vertex(i).color();
        if (ci == NOCOLOR) return false;
        for (VertexNumber j = i + 1; j < n; ++j) {
          if (adjacent(i, j) && (ci == vertex(j).color())) return false;
        }
      }
      return true;
    }

  }  // namespace graph
}  // namespace cavcom
