#include <utility>

#include "quick_zykov.h"

namespace cavcom {
  namespace graph {

    QuickZykov::QuickZykov(const Graph &graph) : GraphAlgorithm(graph), k_(0) {
      reset_counters();
    }

    bool QuickZykov::run() {
      // Reset all of the derived context.
      k_ = 0;
      chromatic_.reset(nullptr);
      reset_counters();

      // Make a dynamic copy of the graph so that the algorithm can replace it with subgraphs.
      GraphPtr pg(new Graph(graph()));

      // Call the outer loop.  Note that the algorithm is always successful.
      outer_loop(&pg);

      return true;
    }

    void QuickZykov::outer_loop(GraphPtr *ppg) {
      GraphPtr &pg = *ppg;

      // Null graphs (n=0) are by definition 0-colorable.
      if (check_for_null(pg)) return;

      // Empty graphs (m=0) are 1-colorable.
      if (check_for_empty(pg)) return;

      // All other graphs are 2 or more colorable.
      add_step();
      k_ = 2;

      // Determine if the graph in the current state is k-colorable.  Is so, then done - the current value of k is
      // the chromatic number.  Otherwise, try the next value of k.  This is guaranteed to terminate since at the
      // very worst k will reach n.  Note that a subgraph may be returned if not k-colorable.
      while (!is_k_colorable(ppg)) {
        add_step();
        ++k_;
      }

      // Return the final graph.
      chromatic_ = std::move(pg);
    }

    bool QuickZykov::is_k_colorable(GraphPtr *ppg) {
      add_call();
      GraphPtr &pg = *ppg;
      VertexNumber v1 = 0, v2 = 0;

      // Attempt to bound before branching.
      while (true) {
        // Check for success.
        add_step();
        VertexNumber n = pg->order();
        if (n <= k_) return true;

        // Calculate a maximum edge threshold.  Fail if the graph size exceeds this threshold.  Note that complete
        // graphs are always caught by this check.
        add_step();
        EdgeNumber a = n*n*(k_-1)/(2*k_);

        add_step();
        ++edge_threshold_tries_;
        if (pg->size() > a) {
          ++edge_threshold_hits_;
          return false;
        }

        // Find all vertices with degree < k.
        add_step();
        VertexNumbers x;
        for (VertexNumber i = 0; i < n; ++i) {
          if (pg->degree(i) < k_) x.insert(i);
        }

        add_step();
        ++small_degree_tries_;
        if (!x.empty()) {
          ++small_degree_hits_;
          pg.reset(new Graph(*pg, x));
          continue;
        }

        // Find the smallest neighborhood intersection.  Also find the two such nonadjacent vertices.  If the
        // method return true then one vertex's neighborhood was found to be a subset of another and the former was
        // removed.
        Degree b = 0;
        v1 = 0;
        v2 = 0;
        if (find_neighborhood_subset(&pg, &b, &v1, &v2)) continue;

        // Calculate an upper bound for the minimum number of common neighbors.  If the minimum found exceeds this
        // then the graph is not k-colorable.
        add_step();
        double c = n - 2 - static_cast<double>((n - 2))/(k_ - 1);
        ++common_neighbors_tries_;
        if (b > c) {
          ++common_neighbors_hits_;
          return false;
        }

        // All of the bounding tests have failed.
        break;
      }

      // The preceding loop should terminate with v1 and v2 set to two non-adjacent vertices with the smallest
      // number of common neighbors.  Assume that they are the same color and contract them.
      add_step();
      GraphPtr recursive(new Graph(*pg, v1, v2));
      if (is_k_colorable(&recursive)) {
        chromatic_ = std::move(recursive);
        return true;
      }

      // Using the same color doesn't work, so try different colors.
      add_step();
      recursive.reset(new Graph(*pg));
      recursive->join(v1, v2);
      if (is_k_colorable(&recursive)) {
        chromatic_ = std::move(recursive);
        return true;
      }

      // The graph is not k-colorable.
      return false;
    }

    bool QuickZykov::check_for_null(const GraphPtr &pg) {
      add_step();
      if (!pg->null()) return false;
      k_ = 0;
      return true;
    }

    bool QuickZykov::check_for_empty(const GraphPtr &pg) {
      add_step();
      if (!pg->empty()) return false;
      k_ = 1;
      return true;
    }

    bool QuickZykov::find_neighborhood_subset(GraphPtr *ppg, Degree *b, VertexNumber *v1, VertexNumber *v2) {
      // At this point it is assumed that the graph is not complete (has non-adjacent vertices) and n >= 2.
      // Otherwise, previous bounding checks would have been triggered.
      GraphPtr &pg = *ppg;

      // Scan the vertices, calculating the cardinality of their neighborhood intersections.
      add_step();  // once for the intersection calculation.
      add_step();  // once for the subset check.
      ++neighborhood_subset_tries_;

      VertexNumber n = pg->order();
      VertexNumber last = n - 1;
      bool first = true;
      bool first_nonadj = true;
      Degree smallest_nonadj = 0;

      for (VertexNumber i = 0; i < last; ++i) {
        for (VertexNumber j = i + 1; j < n; ++j) {
          Degree subset = 0;
          for (VertexNumber k = 0; k < n; ++k) {
            if (pg->adjacent(i, k) && pg->adjacent(j, k)) ++subset;
          }

          // Check for a subset.
          VertexNumbers x;
          if (subset == pg->degree(i)) {
            x.insert(i);
          } else if (subset == pg->degree(j)) {
            x.insert(j);
          }
          if (!x.empty()) {
            ++neighborhood_subset_hits_;
            pg.reset(new Graph(*pg, x));
            return true;
          }

          // Not a subset.  Check for the smallest intersection thus far.
          if (first || (subset < *b)) {
            *b = subset;
            first = false;
          }

          // Check for the smallest nonadjacent.
          if (!pg->adjacent(i, j)) {
            if (first_nonadj || (subset < smallest_nonadj)) {
              smallest_nonadj = subset;
              *v1 = i;
              *v2 = j;
              first_nonadj = false;
            }
          }
        }
      }

      return false;
    }

    void QuickZykov::reset_counters() {
      edge_threshold_tries_ = 0;
      edge_threshold_hits_ = 0;
      small_degree_tries_ = 0;
      small_degree_hits_ = 0;
      neighborhood_subset_tries_ = 0;
      neighborhood_subset_hits_ = 0;
      common_neighbors_tries_ = 0;
      common_neighbors_hits_ = 0;
    }

  }  // namespace graph
}  // namespace cavcom
