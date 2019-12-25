#include <algorithm>
#include <utility>

#include "quick_zykov.h"

#include "container_formatter.h"
#include "bron2.h"
#include "clique_edwards.h"
#include "greedy_coloring.h"

namespace cavcom {
  namespace graph {

    QuickZykov::QuickZykov(const SimpleGraph &graph, TikzFormatter *formatter)
      : VertexColoringAlgorithm(graph), kmin_(0), kmax_(0), kemax_(0), formatter_(formatter) {
      reset_counters();
    }

    bool QuickZykov::run() {
      // Initialize the base and derived context.
      VertexColoringAlgorithm::run();
      kmin_ = 0;
      kmax_ = 0;
      kemax_ = 0;
      reset_counters();
      trees_.clear();

      if (tracing()) {
        *formatter_->out() << "Initial graph:" << std::endl;
        formatter_->format(graph());
      }

      // Run the algorithm by calling the outer loop.
      outer_loop();

      // This algorithm always returns an answer.
      return true;
    }

    void QuickZykov::reset_counters(void) {
      ubadjust_tries_ = 0;
      ubadjust_hits_ = 0;
      bounding_tries_ = 0;
      bounding_hits_ = 0;
      edge_threshold_tries_ = 0;
      edge_threshold_hits_ = 0;
      small_degree_tries_ = 0;
      small_degree_hits_ = 0;
      neighborhood_subset_tries_ = 0;
      neighborhood_subset_hits_ = 0;
      common_neighbors_tries_ = 0;
      common_neighbors_hits_ = 0;
    }

    void QuickZykov::outer_loop(void) {
      // Set the upper and lower bounds.  If they match then accept the default greedy coloring.
      add_step();
      set_lower_bound();
      if (formatter_) {
        outer_prefix();
        *formatter_->out() << "xlb=" << kmin_ << std::endl;
      }

      add_step();
      set_upper_bound();
      if (formatter_) {
        outer_prefix();
        *formatter_->out() << "xub=" << kmax_ << std::endl;
      }

      add_step();
      if (formatter_) outer_prefix();
      if (kmin_ == kmax_) {
        *formatter_->out() << "xlb=" << kmin_ << "=" << kmax_ << "=xub, done" << std::endl;
        return;
      }
      if (formatter_) {
        *formatter_->out() << "xlb=" << kmin_ << "<" << kmax_ << "=xub, continuing" << std::endl;
      }

      // Target a vertex that occurs in the least number of MISs.  Each MIS that contains the target vertex results
      // in a tree that will need to be walked.
      establish_trees();
    }

    void QuickZykov::set_lower_bound(void) {
      Bron2 bron(graph(), Bron::MODE_MAX_ONLY, false);
      bron.execute();
      kmin_ = bron.number();
    }

    void QuickZykov::set_upper_bound(void) {
      GreedyColoring greedy(graph());
      greedy.execute();
      kmax_ = greedy.number();
      coloring_ = greedy.coloring();
      number_ = kmax_;
    }

    void QuickZykov::establish_trees(void) {
      // Run the Bron Kerbosch algorithm to find all of the MISs for the input graph.
      add_step();
      SimpleGraph complement(graph(), true);
      Bron2 bron(complement);
      bron.execute();
      const VertexNumbersList &miss = bron.cliques();
      VertexNumbersList::size_type nmiss = miss.size();
      if (formatter_) {
        outer_prefix();
        *formatter_->out() << "Total MISs=" << nmiss << std::endl;
      }

      // Count how many times each vertex occurs in a MIS.
      add_step();
      VertexNumber n = graph().order();
      std::vector<VertexNumbersList::size_type> occurs(n);
      std::for_each(bron.cliques().cbegin(), bron.cliques().cend(),
                    [&occurs](const VertexNumbers &mis){
                      for_each(mis.cbegin(), mis.cend(), [&occurs](VertexNumber iv){ ++occurs[iv]; });
                    });
      if (formatter_) {
        outer_prefix();
        *formatter_->out() << "Counting vertex occurrences in MISs" << std::endl;
      }

      // Target the vertex that occurs in the least number of MISs.
      add_step();
      VertexNumber target = 0;
      VertexNumbersList::size_type occurrences = occurs[0];
      for (VertexNumber iv = 1; iv < n; ++iv) {
        VertexNumbersList::size_type next = occurs[iv];
        if (next < occurrences) occurrences = next;
      }
      if (formatter_) {
        outer_prefix();
        *formatter_->out() << "Vertex " << target << " occurs in " << occurrences << " MISs" << std::endl;
      }

      // Create a tree for each MIS that contains the target vertex.
      add_step();
      for (VertexNumbersList::size_type imis = 0; imis < nmiss; ++imis) {
        const VertexNumbers &mis = miss[imis];
        if (mis.find(target) != mis.end()) trees_.emplace_back(this, mis, kmax_, formatter_, imis);
      }
      if (formatter_) {
        cavcom::utility::ContainerFormatter cf(formatter_->out());
        cf.start("  {");
        outer_prefix();
        *formatter_->out() << "Creating trees for MISs:" << std::endl;
        std::for_each(miss.cbegin(), miss.cend(),
                      [&cf](const VertexNumbers &mis){ cf(mis.cbegin(), mis.cend()) << std::endl; });
      }
    }

    void QuickZykov::outer_prefix(void) {
      *formatter_->out() << steps() << ". (outer) ";
    }

    QuickZykov::ZykovTree::ZykovTree(QuickZykov *parent,
                                     const VertexNumbers &mis,
                                     Color kmax,
                                     TikzFormatter *formatter,
                                     ullong itree)
      : VertexColoringAlgorithm(parent->graph()), parent_(parent), kmax_(kmax),
        formatter_(formatter), itree_(itree) {
      // Vertices in the MIS have the same color, which is different from all the vertices not in the MIS.
      VertexNumber contracted = 0;
      if (mis.size() > 1) {
        pgraph_.reset(new SimpleGraph(graph(), { mis }));
        contracted = pgraph_->order() - 1;
      } else {
        pgraph_.reset(new SimpleGraph(graph(), { mis }));
        contracted = *mis.cbegin();
      }

      // Vertices not in the MIS have a different color than vertices in the MIS.
      VertexNumber n = pgraph_->order();
      for (VertexNumber iv = 0; iv < n; ++iv) {
        if ((iv != contracted) && (!pgraph_->adjacent(iv, contracted))) pgraph_->join(iv, contracted);
      }
    }

#if 0
    bool QuickZykov::run() {
      // If tracing then output the final state of the graph.
      if (tracing())  {
        *formatter_->out() << "Final graph" << std::endl;
        formatter_->format(*pg);
      }

      // An answer is guaranteed.
      return true;
    }

    void QuickZykov::outer_loop(GraphPtr *ppg) {
      // Determine the lower and upper bounds.
      add_step();
      calculate_lower();
      if (tracing()) {
        outer_prefix();
        *formatter_->out() << "Lower bound: kmin=" << lower_bound_ << std::endl;
      }

      add_step();
      calculate_upper();
      if (tracing()) {
        outer_prefix();
        *formatter_->out() << "Upper bound: kmax=" << upper_bound_ << std::endl;
      }

      // Start with the lower bound.
      add_step();
      number_ = lower_bound_;
      if (tracing()) {
        outer_prefix();
        *formatter_->out() << "Initialized: k=" << number_ << std::endl;
      }

      // Determine if the graph in the current state is k-colorable.  This can be because either the upper bound
      // has be reached of the called subroutine indicates k-colorability.  If so, then done - the current value of
      // k is the chromatic number.  Otherwise, try the next value of k.  Note that a subgraph may be returned.
      while (number_ < adjusted_upper_bound_) {
        // Check for k-colorability.
        add_call();
        bool success = is_k_colorable(ppg);
        done_call();
        if (success) break;

        // No, so try the next value of k.
        add_step();
        ++number_;
        if (tracing()) {
          outer_prefix();
          *formatter_->out() << steps() << "Incrementing: k=" << number_ << std::endl;
        }
      }
    }

    bool QuickZykov::is_k_colorable(GraphPtr *ppg) {
      GraphPtr &pg = *ppg;
      Degree b = 0;
      VertexNumber v1 = 0, v2 = 0;
      Degree b_nonadj = 0;
      VertexNumber v1_nonadj = 0, v2_nonadj = 0;

      // Attempt to bound before branching.
      while (true) {
        // Check for success (n <= k).
        add_step();
        if (check_for_success(ppg)) return true;

        // Calculate a maximum edge threshold.  Fail if the graph size exceeds this threshold.  Note that complete
        // graphs are always caught by this check.
        add_step();
        double a = max_edge_threshold(ppg);

        // Make sure that the current number of edges doesn't exceed the threshold.
        add_step();
        ++edge_threshold_tries_;
        if (!check_max_edges(ppg, a)) {
          ++edge_threshold_hits_;
          return false;
        }

        // Find all vertices with degree < k.
        add_step();
        VertexNumbers x;
        find_small_degree(ppg, &x);

        // Remove all such small degree vertices.
        add_step();
        ++small_degree_tries_;
        if (remove_small_vertices(ppg, x)) {
          ++small_degree_hits_;
          continue;
        }

        // Check for bounding.
        add_step();
        if (pg->order() < adjusted_upper_bound_) adjusted_upper_bound_ = pg->order();

        add_step();
        ++bounding_tries_;
        if (check_bounding(&pg)) {
          ++bounding_hits_;
          return false;
        }

        // Find the smallest number of common neighbors.
        b = 0;
        v1 = 0, v2 = 0;
        b_nonadj = 0;
        v1_nonadj = 0, v2_nonadj = 0;
        add_step();
        bool subset = find_common_neighbors(&pg, &b, &v1, &v2, &b_nonadj, &v1_nonadj, &v2_nonadj);

        // If the preceding call returns true then N(v1) is a subset of N(v2), so contract the two vertices.
        add_step();
        ++neighborhood_subset_tries_;
        if (subset) {
          ++neighborhood_subset_hits_;
          remove_subset(ppg, v1, v2);
          continue;
        } else {
          if (tracing()) {
            sub_prefix();
            *formatter_->out() << "No neighborhood subsets found" << std::endl;
          }
        }

        // Mark the two vertices with the smallest number of common neighbors.
        add_step();
        if (tracing()) {
          sub_prefix();
          *formatter_->out() << "Smallest common neighbors: ";
          identify_vertex(pg, v1);
          *formatter_->out() << " and ";
          identify_vertex(pg, v2);
          *formatter_->out() << " have " << b << std::endl;
        }

        // Calculate an upper bound for the minimum number of common neighbors.  If the minimum found exceeds this
        // then the graph is not k-colorable.
        add_step();
        double c = min_common_ub(ppg);

        // Check that the previously found minimum number of common neighbors does not exceed the upper bound.
        add_step();
        ++common_neighbors_tries_;
        if (!check_common_ub(b, c)) {
          ++common_neighbors_hits_;
          return false;
        }

        // All of the bounding tests have failed.
        break;
      }

      // The preceding loop should terminate with v1_nonadj and v2_nonadj set to two non-adjacent vertices with the
      // smallest number of common neighbors.  Assume that they are the same color and contract them.
      add_step();
      if (contract_vertices(ppg, v1_nonadj, v2_nonadj)) return true;

      // Using the same color doesn't work, so try different colors.
      add_step();
      if (add_edge(ppg, v1_nonadj, v2_nonadj)) return true;

      // The graph is not k-colorable.
      add_step();
      if (tracing()) {
        sub_prefix();
        *formatter_->out() << "Not " << number_ << "-colorable" << std::endl;
      }
      return false;
    }

    bool QuickZykov::check_for_success(GraphPtr *ppg) {
      GraphPtr &pg = *ppg;
      VertexNumber n = pg->order();
      if (tracing()) {
        sub_prefix();
        *formatter_->out() << "Success check: n=" << n << ", k=" << number_ << ": ";
      }
      if (n <= number_) {
        if (tracing()) *formatter_->out() << "graph is " << number_ << "-colorable" << std::endl;
        return true;
      }
      if (tracing()) *formatter_->out() << "continue" << std::endl;
      return false;
    }

    double QuickZykov::max_edge_threshold(GraphPtr *ppg) {
      GraphPtr &pg = *ppg;
      double n = static_cast<double>(pg->order());
      double k = static_cast<double>(number_);
      double a = n*n*(k - 1)/(2*k);
      if (tracing()) {
        sub_prefix();
        *formatter_->out() << "Maximum edge threshold: a=" << a << std::endl;
      }
      return a;
    }

    bool QuickZykov::check_max_edges(GraphPtr *ppg, double a) {
      GraphPtr &pg = *ppg;
      double m = static_cast<double>(pg->size());
      if (tracing()) {
        sub_prefix();
        *formatter_->out() << "Compare: m=" << m << ", a=" << a << ": ";
      }
      if (m > a) {
        if (tracing()) *formatter_->out() << "not " << number_ << "-colorable" << std::endl;
        return false;
      }
      if (tracing()) *formatter_->out() << "continue" << std::endl;
      return true;
    }

    void QuickZykov::find_small_degree(GraphPtr *ppg, VertexNumbers *x) {
      GraphPtr &pg = *ppg;
      VertexNumber n = pg->order();
      if (tracing()) {
        sub_prefix();
        *formatter_->out() << "Finding degree < " << number_ << ": found ";
      }
      x->clear();
      for (VertexNumber i = 0; i < n; ++i) {
        if (pg->degree(i) < number_) x->insert(i);
      }
      if (tracing()) *formatter_->out() << x->size() << std::endl;
    }

    bool QuickZykov::remove_small_vertices(GraphPtr *ppg, const VertexNumbers &x) {
      GraphPtr &pg = *ppg;
      if (tracing()) {
        sub_prefix();
        *formatter_->out() << "Removing vertices:";
        if (x.empty()) {
          *formatter_->out() << " none" << std::endl;
        } else {
          for_each(x.begin(), x.end(), [&](VertexNumber iv)
                                       { *formatter_->out() << " ";
                                         identify_vertex(pg, iv); });
          *formatter_->out() << std::endl;
        }
      }
      if (x.empty()) return false;
      pg.reset(new SimpleGraph(*pg, x, EdgeNumbers()));
      if (tracing()) formatter_->format(*pg);
      return true;
    }

    bool QuickZykov::check_bounding(GraphPtr *ppg) {
      GraphPtr &pg = *ppg;
      CliqueEdwards ce(*pg);
      ce.execute();
      Color lb = ce.number();
      bool bound = (lb > number_);
      if (tracing() && bound) {
        sub_prefix();
        *formatter_->out() << "Bounding: lb=" << lb << ">" << number_ << "=k" << std::endl;
      }
      return bound;
    }

    bool QuickZykov::find_common_neighbors(GraphPtr *ppg,
                                           Degree *smallest,
                                           VertexNumber *v1, VertexNumber *v2,
                                           Degree *smallest_nonadj,
                                           VertexNumber *v1_nonadj, VertexNumber *v2_nonadj) {
      // At this point it is assumed that the graph is not complete (has non-adjacent vertices) and n >= 2.
      // Otherwise, previous bounding checks would have been triggered.
      GraphPtr &pg = *ppg;

      if (tracing()) {
        sub_prefix();
        *formatter_->out() << "Calculating common neighbors" << std::endl;
      }

      // Scan the vertices, calculating the cardinality of their neighborhood intersections.
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
          if (subset == pg->degree(i)) {
            *v1 = i;
            *v2 = j;
            return true;
          } else if (subset == pg->degree(j)) {
            *v1 = j;
            *v2 = i;
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

      return false;
    }

    void QuickZykov::remove_subset(GraphPtr *ppg, VertexNumber v1, VertexNumber v2) {
      GraphPtr &pg = *ppg;
      if (tracing()) {
        sub_prefix();
        *formatter_->out() << "N(";
        identify_vertex(pg, v1);
        *formatter_->out() << ") in N(";
        identify_vertex(pg, v2);
        *formatter_->out() << "): Contracting" << std::endl;
      }
      pg.reset(new SimpleGraph(*pg, v1, v2));
      if (tracing()) formatter_->format(*pg);
    }

    double QuickZykov::min_common_ub(GraphPtr *ppg) {
      GraphPtr &pg = *ppg;
      double n = static_cast<double>(pg->order());
      double k = static_cast<double>(number_);
      double c = n - 2 - (n - 2)/(k - 1);
      if (tracing()) {
        sub_prefix();
        *formatter_->out() << "Minimum common neighbors upper bound: c = " << c << std::endl;
      }
      return c;
    }

    bool QuickZykov::check_common_ub(double b, double c) {
      if (tracing()) {
        sub_prefix();
        *formatter_->out() << "Compare: b=" << b << ", c=" << c << ": ";
      }
      if (b > c) {
        if (tracing()) *formatter_->out() << "not " << number_ << "-colorable" << std::endl;
        return false;
      }
      if (tracing()) *formatter_->out() << "continue" << std::endl;
      return true;
    }

    bool QuickZykov::contract_vertices(GraphPtr *ppg, VertexNumber v1, VertexNumber v2) {
      GraphPtr &pg = *ppg;
      if (tracing()) {
        sub_prefix();
        *formatter_->out() << "Contracting: ";
        identify_vertex(pg, v1);
        *formatter_->out() << " and ";
        identify_vertex(pg, v2);
        *formatter_->out() << std::endl;
      }
      GraphPtr recursive(new SimpleGraph(*pg, v1, v2));
      if (tracing()) formatter_->format(*recursive);
      add_call();
      bool success = is_k_colorable(&recursive);
      done_call();
      if (success) pg = std::move(recursive);
      return success;
    }

    bool QuickZykov::add_edge(GraphPtr *ppg, VertexNumber v1, VertexNumber v2) {
      GraphPtr &pg = *ppg;
      if (tracing()) {
        sub_prefix();
        *formatter_->out() << "Adding edge: ";
        identify_vertex(pg, v1);
        *formatter_->out() << " and ";
        identify_vertex(pg, v2);
        *formatter_->out() << std::endl;
      }
      GraphPtr recursive(new SimpleGraph(*pg));
      recursive->join(v1, v2);
      if (tracing()) formatter_->format(*recursive);
      add_call();
      bool success = is_k_colorable(&recursive);
      done_call();
      if (success) pg = std::move(recursive);
      return success;
    }

    void QuickZykov::sub_prefix(void) {
      *formatter_->out() << steps() << ". (sub-" << depth() << ") ";
    }

    void QuickZykov::identify_vertex(const GraphPtr &pg, VertexNumber iv) {
      Vertex &vertex = pg->vertex(iv);
      const Label label = vertex.label();
      if (label.empty()) {
        *formatter_->out() << vertex.id();
      } else {
        *formatter_->out() << label;
      }
    }
#endif

  }  // namespace graph
}  // namespace cavcom
