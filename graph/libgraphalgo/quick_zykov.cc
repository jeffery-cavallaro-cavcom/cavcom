#include <algorithm>
#include <iostream>
#include <utility>

#include "quick_zykov.h"

#include "bron2.h"
#include "clique_edwards.h"
#include "greedy_coloring.h"

namespace cavcom {
  namespace graph {

    QuickZykov::QuickZykov(const SimpleGraph &graph, TikzFormatter *formatter)
      : VertexColoringAlgorithm(graph), kmin_(0), kmax_(0),
        formatter_(formatter), containers_(formatter ? formatter->out() : &std::cout) {
      reset_counters();
    }

    bool QuickZykov::run() {
      // Initialize the base and derived context.
      VertexColoringAlgorithm::run();
      kmin_ = 0;
      kmax_ = 0;
      reset_counters();
      trees_.clear();

      if (formatter_) {
        *formatter_->out() << "Initial graph:" << std::endl;
        formatter_->format(graph());
      }

      // Run the algorithm by calling the outer loop.
      outer_loop();

      // This algorithm always returns an answer.
      return true;
    }

    void QuickZykov::reset_counters(void) {
      edge_threshold_tries_ = 0;
      edge_threshold_hits_ = 0;
      small_degree_tries_ = 0;
      small_degree_hits_ = 0;
      neighborhood_subset_tries_ = 0;
      neighborhood_subset_hits_ = 0;
      common_neighbors_tries_ = 0;
      common_neighbors_hits_ = 0;
      bounding_tries_ = 0;
      bounding_hits_ = 0;
    }

    void QuickZykov::outer_loop(void) {
      // Set the upper and lower bounds.  If they match then accept the default greedy coloring.
      set_lower_bound();
      set_upper_bound();
      if (check_for_match()) return;

      // Target a vertex that occurs in the least number of MISs.  Each MIS that contains the target vertex results
      // in a tree that will need to be walked.  The trees are sorted by decreasing MIS length.
      make_trees();

      // Start with the lower bound.
      add_step();
      number_ = kmin_;
      if (formatter_) outer_prefix() << "Initialize: k=" << number_ << std::endl;

      // Loop on increased values of k until either kmax is achieved or until the recursive subroutine returns
      // true.  The former case accepts the initial greedy coloring as chromatic.  The latter occurs when the
      // current state of one of the Zykov trees has been found to be k-colorable.
      while (number_ < kmax_) {
        add_step();
        if (formatter_) {
          outer_prefix();
          *formatter_->out() << "k=" << number_ << "<" << kmax_ << "=kmax, continuing" << std::endl;
        }

        // Try each tree.  Stop on the first success.
        ZykovTreeList::size_type ntrees = trees_.size();
        for (ZykovTreeList::size_type itree = 0; itree < ntrees; ++itree) {
          ZykovTree &tree = trees_[itree];

          add_step();
          if (formatter_) {
            outer_prefix() << "Starting tree " << itree << std::endl;
            formatter_->format(tree.current());
          }

          if (tree.execute()) {
            // A solution has been found.  Construct the final chromatic coloring.
            add_step();
            tree.construct_coloring();
            coloring_ = tree.coloring();
            if (formatter_) {
              outer_prefix() << "Final coloring:" << std::endl;
              format_numbers_list_as_ids(graph(), coloring_, true);
            }
            return;
          }
        }

        // None of the graphs was k-colorable.  Try the next value of k.
        add_step();
        ++number_;
        if (formatter_) {
          outer_prefix();
          *formatter_->out() << "Incrementing: k=" << number_ << std::endl;
        }
      }

      add_step();
      if (formatter_) {
        outer_prefix();
        *formatter_->out() << "k=" << number_ << "=" << kmax_ << "=kmax, accepting greedy coloring" << std::endl;
      }
    }

    void QuickZykov::set_lower_bound(void) {
      add_step();
      Bron2 bron(graph(), Bron::MODE_MAX_ONLY, false);
      bron.execute();
      kmin_ = bron.number();
      if (formatter_) outer_prefix() << "xlb=" << kmin_ << std::endl;
    }

    void QuickZykov::set_upper_bound(void) {
      add_step();
      GreedyColoring greedy(graph());
      greedy.execute();
      kmax_ = greedy.number();
      coloring_ = greedy.coloring();
      number_ = kmax_;
      if (formatter_) outer_prefix() << "xub=" << kmax_ << std::endl;
    }

    bool QuickZykov::check_for_match(void) {
      add_step();
      bool match = (kmin_ >= kmax_);
      if (formatter_) {
        outer_prefix() << "xlb=" << kmin_;
        if (match) {
          *formatter_->out()  << "=" << kmax_ << "=xub, done" << std::endl;
        } else {
          *formatter_->out() << "<" << kmax_ << "=xub, continuing" << std::endl;
        }
      }
      return match;
    }

    void QuickZykov::make_trees(void) {
      // Find all the target MISs.
      VertexNumbersList targets;
      find_target_miss(&targets);
      VertexNumbersList::size_type ntargets = targets.size();

      // Create a tree for each target MIS.
      add_step();
      for (VertexNumbersList::size_type imis = 0; imis < ntargets; ++imis) {
        trees_.emplace_back(this, targets[imis], imis);
      }
      if (formatter_) outer_prefix() << "Created trees for MISs" << std::endl;
    }

    void QuickZykov::find_target_miss(VertexNumbersList *targets) {
      // Run the Bron Kerbosch algorithm to find all of the MISs for the input graph.
      add_step();
      SimpleGraph complement(graph(), true);
      Bron2 bron(complement);
      bron.execute();
      const VertexNumbersList &miss = bron.cliques();
      VertexNumbersList::size_type nmiss = miss.size();
      if (formatter_) outer_prefix() << "Total MISs=" << nmiss << std::endl;

      // Find the target vertex.
      VertexNumber target = find_target_vertex(miss);

      // Extract the MISs that contain the target vertex.
      add_step();
      VertexNumbersList unsorted;
      std::for_each(miss.cbegin(), miss.cend(),
                    [target, &unsorted](const VertexNumbers &mis){
                      if (mis.find(target) != mis.end()) unsorted.push_back(mis);
                    });
      if (formatter_) outer_prefix() << "Extracting target MISs" << std::endl;

      // Sort the target MISs by decreasing size.
      add_step();
      sort_target_miss(unsorted, targets);
      if (formatter_) {
        outer_prefix() << "Sorted target MISs:" << std::endl;
        format_numbers_list_as_ids(graph(), *targets, true);
      }
    }

    VertexNumber QuickZykov::find_target_vertex(const VertexNumbersList &miss) {
      // Count how many times each vertex occurs in a MIS.
      add_step();
      VertexNumber n = graph().order();
      std::vector<VertexNumbersList::size_type> occurs(n);
      std::for_each(miss.cbegin(), miss.cend(),
                    [&occurs](const VertexNumbers &mis){
                      for_each(mis.cbegin(), mis.cend(), [&occurs](VertexNumber iv){ ++occurs[iv]; });
                    });
      if (formatter_) outer_prefix() << "Counting vertex occurrences in MISs" << std::endl;

      // Target the vertex that occurs in the least number of MISs.
      add_step();
      VertexNumber target = 0;
      VertexNumbersList::size_type occurrences = occurs[0];
      for (VertexNumber iv = 1; iv < n; ++iv) {
        VertexNumbersList::size_type next = occurs[iv];
        if (next < occurrences) occurrences = next;
      }
      if (formatter_) outer_prefix() << "Vertex " << graph().vertex(target).id() << " occurs in "
                                     << occurrences << " MISs" << std::endl;

      return target;
    }

    void QuickZykov::sort_target_miss(const VertexNumbersList &unsorted, VertexNumbersList *sorted) {
      using SizeType = VertexNumbersList::size_type;
      SizeType n = unsorted.size();
      std::vector<SizeType> positions(n);
      for (SizeType i = 0; i < n; ++i) positions[i] = i;
      std::sort(positions.begin(), positions.end(),
                [&unsorted](SizeType i, SizeType j){
                  const VertexNumbers &ui = unsorted[i];
                  const VertexNumbers &uj = unsorted[j];
                  VertexNumber ni = ui.size();
                  VertexNumber nj = uj.size();
                  return ((ni > nj) || ((ni == nj) && (ui < uj)));
                });
      sorted->reserve(n);
      std::for_each(positions.cbegin(), positions.cend(),
                    [&unsorted, sorted](SizeType i){ sorted->push_back(unsorted[i]); });
    }

    void QuickZykov::edge_threshold_add(bool hit) {
      ++edge_threshold_tries_;
      if (hit) ++edge_threshold_hits_;
    }

    void QuickZykov::small_degree_add(bool hit) {
      ++small_degree_tries_;
      if (hit) ++small_degree_hits_;
    }

    void QuickZykov::neighborhood_subset_add(bool hit) {
      ++neighborhood_subset_tries_;
      if (hit) ++neighborhood_subset_hits_;
    }

    void QuickZykov::common_neighbors_add(bool hit) {
      ++common_neighbors_tries_;
      if (hit) ++common_neighbors_hits_;
    }

    void QuickZykov::bounding_add(bool hit) {
      ++bounding_tries_;
      if (hit) ++bounding_hits_;
    }

    std::ostream &QuickZykov::outer_prefix(void) {
      std::ostream &out = *formatter_->out();
      out << steps() << ". (outer) ";
      return out;
    }

    std::ostream &QuickZykov::format_ids(const VertexIDs &ids, bool pad) {
      return format_container(ids, pad);
    }

    std::ostream &QuickZykov::format_ids_list(const VertexIDsList &ids_list, bool pad) {
      std::for_each(ids_list.cbegin(), ids_list.cend(),
                    [this, pad](const VertexIDs &ids){ format_ids(ids, pad); });
      return containers_.out();
    }

    std::ostream &QuickZykov::format_numbers_as_ids(const SimpleGraph &g, const VertexNumbers &numbers, bool pad) {
      VertexIDs ids;
      std::for_each(numbers.cbegin(), numbers.cend(),
                    [&g, &ids](const VertexNumber iv){ ids.insert(g.vertex(iv).id()); });
      format_ids(ids, pad);
      return containers_.out();
    }

    std::ostream &QuickZykov::format_numbers_list_as_ids(const SimpleGraph &g,
                                                         const VertexNumbersList &numbers_list,
                                                         bool pad) {
      std::for_each(numbers_list.cbegin(), numbers_list.cend(),
                    [this, &g, pad](const VertexNumbers &numbers){
                      format_numbers_as_ids(g, numbers, pad);
                    });
      return containers_.out();
    }

    QuickZykov::ZykovTree::ZykovTree(QuickZykov *parent, const VertexNumbers &mis, ullong itree)
      : VertexColoringAlgorithm(parent->graph()), parent_(parent), premoved_(new VertexIDsList()), itree_(itree) {
      // Vertices in the MIS have the same color, which is different from all the vertices not in the MIS.
      VertexNumber contracted = 0;
      if (mis.size() > 1) {
        pgraph_.reset(new SimpleGraph(graph(), VertexNumbersList({ mis })));
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

    bool QuickZykov::ZykovTree::run() {
      number_ = parent_->number();
      add_call();
      bool success = is_k_colorable(&pgraph_, &premoved_);
      done_call();
      return success;
    }

    bool QuickZykov::ZykovTree::is_k_colorable(GraphPtr *ppgraph, VertexIDsListPtr *ppremoved) {
      GraphPtr &pg = *ppgraph;
      Degree b = 0;
      VertexNumber v1 = 0, v2 = 0;
      Degree b_nonadj = 0;
      VertexNumber v1_nonadj = 0, v2_nonadj = 0;

      // Attempt to bound before branching.
      while (true) {
        // Check for success (n <= k).
        if (check_for_success(*pg)) return true;

        // Calculate a maximum edge threshold.  Fail if the graph size exceeds this threshold.  Note that complete
        // graphs are always caught by this check.
        double a = max_edge_threshold(*pg);

        // Make sure that the current number of edges doesn't exceed the threshold.
        bool more = check_max_edges(*pg, a);
        parent_->edge_threshold_add(more);
        if (!more) return false;

        // Find all vertices with degree < k.
        VertexNumbers x;
        find_small_degree(*pg, &x);

        // Remove all such small degree vertices.
        more = remove_small_vertices(ppgraph, ppremoved, x);
        parent_->small_degree_add(more);
        if (more) continue;

        // Find the smallest number of common neighbors.
        b = 0;
        v1 = 0;
        v2 = 0;
        b_nonadj = 0;
        v1_nonadj = 0;
        v2_nonadj = 0;
        bool subset = find_common_neighbors(*pg, &b, &v1, &v2, &b_nonadj, &v1_nonadj, &v2_nonadj);

        // If the preceding call returns true then N(v1) is a subset of N(v2), so contract the two vertices.
        add_step();
        parent_->neighborhood_subset_add(subset);
        if (subset) {
          if (formatter()) sub_prefix() << "Neighborhood subset check: N(" << pg->vertex(v1).id() << ") in N("
                                        << pg->vertex(v2).id() << "), contracting"
                                        << std::endl;
          pg.reset(new SimpleGraph(*pg, v1, v2));
          if (formatter()) formatter()->format(*pg);
          continue;
        }
        if (formatter()) sub_prefix() << "Neighborhood subset check: none" << std::endl;

        // All of the reduction tasks have failed.
        break;
      }

      // Mark the two vertices with the smallest number of common neighbors.
      add_step();
      if (formatter()) sub_prefix() << "Smallest common neighbors: "
                                    << pg->vertex(v1).id() << " and " << pg->vertex(v2).id()
                                    << " have " << b << std::endl;

      // Calculate an upper bound for the minimum number of common neighbors.  If the minimum found exceeds this
      // then the graph is not k-colorable.
      double c = min_common_ub(*pg);

      // Check that the previously found minimum number of common neighbors does not exceed the upper bound.
      if (!check_common_ub(b, c)) return false;

      // Calculate a new lower bound for the chromatic number.
      Color lb = calc_lb(*pg);

      // Bound if the lower bound exceeds the current k value.
      if (check_bounding(lb)) return false;

      // Assume that the two nonadjacent vertices with the smallest number of common neighbors
      // smallest number of common neighbors.  Assume that they are the same color and contract them.
      if (contract_vertices(ppgraph, ppremoved, v1_nonadj, v2_nonadj)) return true;

      // Using the same color doesn't work, so try different colors.
      if (add_edge(ppgraph, ppremoved, v1_nonadj, v2_nonadj)) return true;

      // The graph is not k-colorable.
      add_step();
      if (formatter()) sub_prefix() << "Not " << number_ << "-colorable" << std::endl;
      return false;
    }

    bool QuickZykov::ZykovTree::check_for_success(const SimpleGraph &g) {
      add_step();
      VertexNumber n = g.order();
      bool success = (n <= number_);
      if (formatter()) {
        sub_prefix() << "Success check: n=" << n;
        if (success) {
          *formatter()->out() << "<=" << number_ << "=k, success" << std::endl;
        } else {
          *formatter()->out() << ">" << number_ << "=k, continuing" << std::endl;
        }
      }
      return success;
    }

    double QuickZykov::ZykovTree::max_edge_threshold(const SimpleGraph &g) {
      add_step();
      double n = static_cast<double>(g.order());
      double k = static_cast<double>(number_);
      double a = n*n*(k - 1)/(2*k);
      if (formatter()) sub_prefix() << "Maximum edge threshold: a=" << a << std::endl;
      return a;
    }

    bool QuickZykov::ZykovTree::check_max_edges(const SimpleGraph &g, double a) {
      add_step();
      double m = static_cast<double>(g.size());
      bool ok = (m <= a);
      if (formatter()) {
        sub_prefix() << "Compare: m=" << m;
        if (ok) {
          *formatter()->out() << "<=" << a << "=a, continuing" << std::endl;
        } else {
          *formatter()->out() << ">" << a << "=a, failed" << std::endl;
        }
      }
      return (ok);
    }

    void QuickZykov::ZykovTree::find_small_degree(const SimpleGraph &g,  VertexNumbers *x) {
      add_step();
      VertexNumber n = g.order();
      x->clear();
      for (VertexNumber iv = 0; iv < n; ++iv) {
        if (g.degree(iv) < number_) x->insert(iv);
      }
      if (formatter()) sub_prefix() << "Small degree check: found " << x->size() << std::endl;
    }

    bool QuickZykov::ZykovTree::remove_small_vertices(GraphPtr *ppgraph,
                                                      VertexIDsListPtr *ppremoved,
                                                      const VertexNumbers &x) {
      add_step();
      if (x.empty()) {
        if (formatter()) sub_prefix() << "Removing vertices: none" << std::endl;
        return false;
      }
      GraphPtr &pg = *ppgraph;
      VertexIDsListPtr &premoved = *ppremoved;
      VertexIDs ids;
      std::for_each(x.cbegin(), x.cend(),
                    [&pg, &premoved, &ids](VertexNumber iv){
                      const Vertex &v = pg->vertex(iv);
                      const VertexIDs &c = v.contracted();
                      ids.insert(v.id());
                      if (c.empty()) {
                        premoved->push_back(VertexIDs({v.id()}));
                      } else {
                        premoved->push_back(c);
                      }
                    });
      ppgraph->reset(new SimpleGraph(*pg, x, EdgeNumbers()));
      if (formatter()) {
        if (formatter()) sub_prefix() << "Removing vertices: ";
        parent_->format_ids(ids, false);
        formatter()->format(**ppgraph);
      }
      return true;
    }

    bool QuickZykov::ZykovTree::find_common_neighbors(const SimpleGraph &g,
                                                      Degree *smallest,
                                                      VertexNumber *v1, VertexNumber *v2,
                                                      Degree *smallest_nonadj,
                                                      VertexNumber *v1_nonadj, VertexNumber *v2_nonadj) {
      // At this point it is assumed that the graph is not complete (has non-adjacent vertices) and n >= 2.
      // Otherwise, previous bounding checks would have been triggered.
      add_step();
      if (formatter()) sub_prefix() << "Calculating common neighbors" << std::endl;

      // Scan the vertices, calculating the cardinality of their neighborhood intersections.
      VertexNumber n = g.order();
      VertexNumber last = n - 1;
      bool first = true;
      bool first_nonadj = true;

      for (VertexNumber i = 0; i < last; ++i) {
        for (VertexNumber j = i + 1; j < n; ++j) {
          // Determine the number of common neighbors.
          Degree common = 0;
          for (VertexNumber k = 0; k < n; ++k) {
            if (g.adjacent(i, k) && g.adjacent(j, k)) {
              ++common;
            }
          }

          // Check for a subset.
          bool nonadjacent = !g.adjacent(i, j);
          if (nonadjacent) {
            if (common == g.degree(i)) {
              *v1 = i;
              *v2 = j;
              return true;
            } else if (common == g.degree(j)) {
              *v1 = j;
              *v2 = i;
              return true;
            }
          }

          // Not a subset.  Check for the smallest intersection thus far.
          if (first || (common < *smallest)) {
            *smallest = common;
            *v1 = i;
            *v2 = j;
            first = false;
          }

          // Check for the smallest nonadjacent.
          if (nonadjacent) {
            if (first_nonadj || (common < *smallest_nonadj)) {
              *smallest_nonadj = common;
              *v1_nonadj = i;
              *v2_nonadj = j;
              first_nonadj = false;
            }
          }
        }
      }

      return false;
    }

    double QuickZykov::ZykovTree::min_common_ub(const SimpleGraph &g) {
      add_step();
      double n = static_cast<double>(g.order());
      double k = static_cast<double>(number_);
      double c = n - 2 - (n - 2)/(k - 1);
      if (formatter()) sub_prefix() << "Minimum common neighbors upper bound: c = " << c << std::endl;
      return c;
    }

    bool QuickZykov::ZykovTree::check_common_ub(double b, double c) {
      add_step();
      bool ok = (b < c);
      parent_->common_neighbors_add(ok);
      if (formatter()) {
        sub_prefix() << "Compare: b=" << b;
        if (ok) {
          *formatter()->out() << "<=" << c << "=c, continuing" << std::endl;
        } else {
          *formatter()->out() << ">" << c << "=c, failed" << std::endl;
        }
      }
      return ok;
    }

    Color QuickZykov::ZykovTree::calc_lb(const SimpleGraph &g) {
      add_step();
      CliqueEdwards ce(g);
      ce.execute();
      Color lb = ce.number();
      if (formatter()) sub_prefix() << "xlb=" << lb << std::endl;
      return lb;
    }

    bool QuickZykov::ZykovTree::check_bounding(Color lb) {
      add_step();
      bool bound = (lb > number_);
      parent_->bounding_add(bound);
      if (formatter()) {
        sub_prefix() << "Bounding: lb=" << lb;
        if (bound) {
          *formatter()->out() << ">" << number_ << "=k, bounding" << std::endl;
        } else {
          *formatter()->out() << "<=" << number_ << "=k, continuing" << std::endl;
        }
      }
      return bound;
    }

    bool QuickZykov::ZykovTree::contract_vertices(GraphPtr *ppgraph, VertexIDsListPtr *ppremoved,
                                                  VertexNumber v1, VertexNumber v2) {
      add_step();
      GraphPtr &pg = *ppgraph;
      VertexIDsListPtr &premoved = *ppremoved;
      if (formatter()) sub_prefix() << "Contracting: " << pg->vertex(v1).id() << " and "
                                    << pg->vertex(v2).id() << std::endl;
      GraphPtr next_graph(new SimpleGraph(*pg, v1, v2));
      VertexIDsListPtr next_removed(new VertexIDsList(*premoved));
      if (formatter()) formatter()->format(*next_graph);
      add_call();
      bool success = is_k_colorable(&next_graph, &next_removed);
      done_call();
      if (success) {
        pg = std::move(next_graph);
        premoved = std::move(next_removed);
      }
      return success;
    }

    bool QuickZykov::ZykovTree::add_edge(GraphPtr *ppgraph, VertexIDsListPtr *ppremoved,
                                         VertexNumber v1, VertexNumber v2) {
      add_step();
      GraphPtr &pg = *ppgraph;
      VertexIDsListPtr &premoved = *ppremoved;
      if (formatter()) sub_prefix() << "Joining: " << pg->vertex(v1).id() << " and "
                                    << pg->vertex(v2).id() << std::endl;
      GraphPtr next_graph(new SimpleGraph(*pg));
      next_graph->join(v1, v2);
      VertexIDsListPtr next_removed(new VertexIDsList(*premoved));
      if (formatter()) formatter()->format(*next_graph);
      add_call();
      bool success = is_k_colorable(&next_graph, &next_removed);
      done_call();
      if (success) {
        pg = std::move(next_graph);
        premoved = std::move(next_removed);
      }
      return success;
    }

    void QuickZykov::ZykovTree::construct_coloring() {
      const SimpleGraph &gi = graph();
      const SimpleGraph &gf = *pgraph_;
      const VertexIDsList &r = *premoved_;

      // Start a new coloring.
      coloring_.clear();
      coloring_.resize(number_);

      // Start with the vertices in the final graph.
      VertexNumber nf = gf.order();
      for (VertexNumber iv = 0; iv < nf; ++iv) {
        const Vertex &v = gf.vertex(iv);
        const VertexIDs &c = v.contracted();
        VertexNumbers color;
        if (c.empty()) {
          VertexNumber iv = 0;
          gi.find_vertex(v.id(), &iv);
          color.insert(iv);
        } else {
          gi.ids_to_numbers(c, &color);
        }
        coloring_[iv].insert(color.cbegin(), color.cend());
      }

      // Color the removed vertices in the order removed using a greedy coloring.  No new colors should be used.
      std::for_each(r.cbegin(), r.cend(),
                    [this, &gi](const VertexIDs &ids){
                      // Convert the IDs to numbers.
                      VertexNumbers numbers;
                      gi.ids_to_numbers(ids, &numbers);

                      // Determine how the colors are used by already colored vertices that are adjacent.
                      std::vector<bool> used(number_);
                      std::for_each(numbers.cbegin(), numbers.cend(),
                                    [this, &gi, &used](VertexNumber iv){
                                      for (Color ic = 0; ic < number_; ++ic) {
                                        const VertexNumbers &color = coloring_[ic];
                                        if (std::any_of(color.cbegin(), color.cend(),
                                                         [&gi, iv](VertexNumber jv){ return gi.adjacent(iv, jv); })) {
                                          used[ic] = true;
                                        }
                                      }
                                    });

                      // Select the first unused color.
                      Color ic = 0;
                      for (ic = 0; used[ic]; ++ic) {}

                      // Use the selected color.
                      coloring_.at(ic).insert(numbers.cbegin(), numbers.cend());
                    });
    }

    std::ostream &QuickZykov::ZykovTree::sub_prefix() {
      std::ostream &out = *formatter()->out();
      out << steps() << ". (" << itree_ << "-" << depth() << ") ";
      return out;
    }

  }  // namespace graph
}  // namespace cavcom
