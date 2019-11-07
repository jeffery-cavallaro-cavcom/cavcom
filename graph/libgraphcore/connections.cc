#include <algorithm>

#include "connections.h"
#include "errors.h"

namespace cavcom {
  namespace graph {

    Connections::Connections(VertexNumber order, bool directed, bool multiple, bool loops)
      : directed_(directed), multiple_(multiple), loops_(loops), connections_(new ConnectionMatrix(order)),
        indeg_(order, 0), outdeg_(order, 0) {}

    Degree Connections::join(VertexNumber from, VertexNumber to, EdgeNumber edge) {
      Edges &edges = connections_->at(from, to);

      // Guard against disabled multiple and loop edges.
      if ((edges.size() > 0) && (!multiple_)) throw MultipleEdgeError(from, to, edge);
      if ((from == to) && (!loops_)) throw LoopEdgeError(from, edge);

      // Join the endpoint vertices with the edge.
      edges.push_back(edge);
      ++outdeg_[from];
      ++indeg_[to];

      // If this is an undirected graph and the edge is not a loop then add the other direction.
      if ((!directed_) && (from != to)) {
        Edges &other_edges = connections_->at(to, from);
        other_edges.push_back(edge);
        ++outdeg_[to];
        ++indeg_[from];
      }

      return edges.size();
    }

    Degree Connections::minindeg(void) const {
      const auto &iv = min_element(indeg_.cbegin(), indeg_.cend());
      return (iv != indeg_.cend()) ? *iv : 0;
    }

    Degree Connections::maxindeg(void) const {
      const auto &iv = max_element(indeg_.cbegin(), indeg_.cend());
      return (iv != indeg_.cend()) ? *iv : 0;
    }

    Degree Connections::minoutdeg(void) const {
      const auto &iv = min_element(outdeg_.cbegin(), outdeg_.cend());
      return (iv != outdeg_.cend()) ? *iv : 0;
    }

    Degree Connections::maxoutdeg(void) const {
      const auto &iv = max_element(outdeg_.cbegin(), outdeg_.cend());
      return (iv != outdeg_.cend()) ? *iv : 0;
    }

  }  // namespace graph
}  // namespace cavcom
