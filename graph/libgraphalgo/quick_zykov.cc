#include <utility>

#include "quick_zykov.h"

namespace cavcom {
  namespace graph {

    QuickZykov::QuickZykov(const SimpleGraph &graph)
      : GraphAlgorithm(graph), k_(0), out_(nullptr), formatter_(nullptr)  {
      reset_counters();
    }

    void QuickZykov::trace(std::ostream *out, TikzFormatter *formatter) {
      out_ = out;
      formatter_ = formatter;
    }

    bool QuickZykov::run() {
      // Reset all of the derived context.
      k_ = 0;
      chromatic_.reset(nullptr);
      reset_counters();

      // Make a dynamic copy of the graph so that the algorithm can replace it with subgraphs.
      GraphPtr pg(new SimpleGraph(graph()));

      // Call the outer loop.  Note that the algorithm is always successful.
      outer_loop(&pg);

      return true;
    }

    void QuickZykov::outer_loop(GraphPtr *ppg) {
      GraphPtr &pg = *ppg;
      if (out_) *out_ << steps() << ". Initial graph" << std::endl;
      if (formatter_) formatter_->format(*pg);

      // Null graphs (n=0) are by definition 0-colorable.
      add_step();
      if (out_) *out_ << steps() << ". Null check: n=" << pg->order() << ": ";
      if (pg->null()) {
        chromatic_ = std::move(pg);
        k_ = 0;
        if (out_) *out_ << "null graph is 0-colorable" << std::endl;
        return;
      }
      if (out_) *out_ << "not a null graph" << std::endl;

      // Empty graphs (m=0) are 1-colorable.
      add_step();
      if (out_) *out_ << steps() << ". Empty check: m=" << pg->size() << ": ";
      if (pg->empty()) {
        chromatic_ = std::move(pg);
        k_ = 1;
        if (out_) *out_ << "empty graph is 1-colorable" << std::endl;
        return;
      }
      if (out_) *out_ << "not an empty graph" << std::endl;

      // All other graphs are 2 or more colorable.
      add_step();
      k_ = 2;
      if (out_) *out_ << steps() << ". Initialized: k=" << k_ << std::endl;

      // Determine if the graph in the current state is k-colorable.  Is so, then done - the current value of k is
      // the chromatic number.  Otherwise, try the next value of k.  This is guaranteed to terminate since at the
      // very worst k will reach n.  Note that a subgraph may be returned if not k-colorable.
      while (!is_k_colorable(ppg)) {
        add_step();
        ++k_;
        if (out_) *out_ << steps() << ". Incrementing: k=" << k_ << std::endl;
      }

      // Return the final graph.
      chromatic_ = std::move(pg);
      if (out_)  *out_ << steps() << ". Final graph" << std::endl;
      if (formatter_) formatter_->format(*chromatic_);
    }

    bool QuickZykov::is_k_colorable(GraphPtr *ppg) {
      add_call();
      GraphPtr &pg = *ppg;
      Degree b = 0;
      VertexNumber v1 = 0, v2 = 0;
      Degree b_nonadj = 0;
      VertexNumber v1_nonadj = 0, v2_nonadj = 0;

      // Attempt to bound before branching.
      while (true) {
        // Check for success.
        add_step();
        VertexNumber n = pg->order();
        if (out_) *out_ << steps() << ". Success check: n=" << n << ", k=" << k_ << ": ";
        if (n <= k_) {
          if (out_) *out_ << "graph is " << k_ << "-colorable" << std::endl;
          return true;
        }
        if (out_) *out_ << "continue" << std::endl;

        // Calculate a maximum edge threshold.  Fail if the graph size exceeds this threshold.  Note that complete
        // graphs are always caught by this check.
        add_step();
        EdgeNumber a = n*n*(k_-1)/(2*k_);
        if (out_) *out_ << steps() << ". Maximum edge threshold: a=" << a << std::endl;

        add_step();
        if (out_) *out_ << steps() << ". Compare: m=" << pg->size() << ", a=" << a << ": ";
        ++edge_threshold_tries_;
        if (pg->size() > a) {
          ++edge_threshold_hits_;
          if (out_) *out_ << "not " << k_ << "-colorable" << std::endl;
          return false;
        }
        if (out_) *out_ << "continue" << std::endl;

        // Find all vertices with degree < k.
        add_step();
        if (out_) *out_ << steps() << ". Finding degree < " << k_ << ":";
        VertexNumbers x;
        for (VertexNumber i = 0; i < n; ++i) {
          if (pg->degree(i) < k_) x.insert(i);
        }
        if (out_) {
          if (x.empty()) {
            *out_ << " none" << std::endl;
          } else {
            for_each(x.begin(), x.end(), [&](VertexNumber iv)
                                         { *out_ << " ";
                                           identify_vertex(pg, iv); });
            *out_ << std::endl;
          }
        }

        add_step();
        ++small_degree_tries_;
        if (!x.empty()) {
          ++small_degree_hits_;
          pg.reset(new SimpleGraph(*pg, x));
          if (out_) *out_ << steps() << ". Removing " << x.size() << " vertices" << std::endl;
          if (formatter_) formatter_->format(*pg);
          continue;
        }
        if (out_) *out_ << steps() << ". Removed no vertices" << std::endl;

        // Find the smallest neighborhood intersection.  Also find the two such nonadjacent vertices.  If the
        // method return true then one vertex's neighborhood was found to be a subset of another and the former was
        // removed.
        b = 0;
        v1 = 0, v2 = 0;
        b_nonadj = 0;
        v1_nonadj = 0, v2_nonadj = 0;
        if (find_neighborhood_subset(&pg, &b, &v1, &v2, &b_nonadj, &v1_nonadj, &v2_nonadj)) continue;
        add_step();
        if (out_) {
          *out_ << steps() << ". Smallest common neighbors: ";
          identify_vertex(pg, v1);
          *out_ << " and ";
          identify_vertex(pg, v2);
          *out_ << " have " << b << std::endl;
        }

        // Calculate an upper bound for the minimum number of common neighbors.  If the minimum found exceeds this
        // then the graph is not k-colorable.
        add_step();
        double c = n - 2 - static_cast<double>(n - 2)/(k_ - 1);
        if (out_) {
          *out_ << steps() << ". Minimum common neighbors upper bound: c = " << c << std::endl;
        }

        add_step();
        if (out_) {
          *out_ << steps() << ". Check upper bound: b=" << b << ", c=" << c << ": ";
        }
        ++common_neighbors_tries_;
        if (b > c) {
          if (out_) {
            *out_ << "not " << k_ << "-colorable" << std::endl;
          }
          ++common_neighbors_hits_;
          return false;
        }
        if (out_) {
          *out_ << "continue" << std::endl;
        }

        // All of the bounding tests have failed.
        break;
      }

      // The preceding loop should terminate with v1 and v2 set to two non-adjacent vertices with the smallest
      // number of common neighbors.  Assume that they are the same color and contract them.
      add_step();
      GraphPtr recursive(new SimpleGraph(*pg, v1_nonadj, v2_nonadj));
      if (out_) {
        *out_ << steps() << ". Contracting: ";
        identify_vertex(pg, v1_nonadj);
        *out_ << " and ";
        identify_vertex(pg, v2_nonadj);
        *out_ << std::endl;
      }
      if (formatter_) formatter_->format(*recursive);
      if (is_k_colorable(&recursive)) {
        pg = std::move(recursive);
        return true;
      }

      // Using the same color doesn't work, so try different colors.
      add_step();
      recursive.reset(new SimpleGraph(*pg));
      recursive->join(v1_nonadj, v2_nonadj);
      if (out_) {
        *out_ << steps() << ". Adding edge: ";
        identify_vertex(pg, v1_nonadj);
        *out_ << " and ";
        identify_vertex(pg, v2_nonadj);
        *out_ << std::endl;
      }
      if (formatter_) formatter_->format(*recursive);
      if (is_k_colorable(&recursive)) {
        pg = std::move(recursive);
        return true;
      }

      // The graph is not k-colorable.
      add_step();
      if (out_) {
        *out_ << steps() << ". Not " << k_ << "-colorable" << std::endl;
      }
      return false;
    }

    bool QuickZykov::find_neighborhood_subset(GraphPtr *ppg,
                                              Degree *smallest,
                                              VertexNumber *v1, VertexNumber *v2,
                                              Degree *smallest_nonadj,
                                              VertexNumber *v1_nonadj, VertexNumber *v2_nonadj) {
      // At this point it is assumed that the graph is not complete (has non-adjacent vertices) and n >= 2.
      // Otherwise, previous bounding checks would have been triggered.
      GraphPtr &pg = *ppg;

      // Scan the vertices, calculating the cardinality of their neighborhood intersections.
      add_step();  // for the subset check
      ++neighborhood_subset_tries_;
      if (out_) *out_ << steps() << ". Neighborhood subset removal: ";

      VertexNumber n = pg->order();
      VertexNumber last = n - 1;
      bool first = true;
      bool first_nonadj = true;

      for (VertexNumber i = 0; i < last; ++i) {
        for (VertexNumber j = i + 1; j < n; ++j) {
          // Determine the number of common neighbors.
          Degree subset = 0;
          for (VertexNumber k = 0; k < n; ++k) {
            if (pg->adjacent(i, k) && pg->adjacent(j, k)) {
              ++subset;
            }
          }

          // Check for a subset.
          VertexNumbers x;
          VertexNumber sub;
          VertexNumber sup;
          if (subset == pg->degree(i)) {
            x.insert(i);
            sub = i;
            sup = j;
          } else if (subset == pg->degree(j)) {
            x.insert(j);
            sub = j;
            sup = i;
          }
          if (!x.empty()) {
            ++neighborhood_subset_hits_;
            if (out_) {
              *out_ << "N(";
              identify_vertex(pg, sub);
              *out_ << ") in N(";
              identify_vertex(pg, sup);
              *out_ << ")" << std::endl;
            }
            pg.reset(new SimpleGraph(*pg, x));
            if (formatter_) formatter_->format(*pg);
            return true;
          }

          // Not a subset.  Check for the smallest intersection thus far.
          if (first || (subset < *smallest)) {
            *smallest = subset;
            *v1 = i;
            *v2 = j;
            first = false;
          }

          // Check for the smallest nonadjacent.
          if (!pg->adjacent(i, j)) {
            if (first_nonadj || (subset < *smallest_nonadj)) {
              *smallest_nonadj = subset;
              *v1_nonadj = i;
              *v2_nonadj = j;
              first_nonadj = false;
            }
          }
        }
      }
      if (out_) *out_ << "none found" << std::endl;

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

    void QuickZykov::identify_vertex(const GraphPtr &pg, VertexNumber iv) {
      Vertex &vertex = pg->vertex(iv);
      const Label label = vertex.label();
      if (label.empty()) {
        *out_ << vertex.id();
      } else {
        *out_ << label;
      }
    }

  }  // namespace graph
}  // namespace cavcom
