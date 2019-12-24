#ifndef CAVCOM_GRAPH_LIBGRAPHALGO_QUICK_ZYKOV_H_
#define CAVCOM_GRAPH_LIBGRAPHALGO_QUICK_ZYKOV_H_

#include <memory>

#include "tikz_formatter.h"
#include "vertex_coloring_algorithm.h"

namespace cavcom {
  namespace graph {

    // A modified version of a Zykov algorithm to determine the chromatic number of a graph.  The algorithm is
    // composed of an outer loop on increasing values of k and a called recursive subroutine that determines if the
    // current state of the graph is k-colorable.  The first such k found is the chromatic number.  The first k
    // attempted (lower bound) is equal to the click number of the graph as determined by the Bron Kerbosch
    // algorithm.  The last possible k (upper bound) is determine by a last-first sequential (greedy) algorithm.
    class QuickZykov : public VertexColoringAlgorithm {
     public:
      // Creates a new algorithm instance for the specified graph.
      explicit QuickZykov(const SimpleGraph &g);

      // Enables a trace using the specified formatter.  Null disables tracing (default).  Information is output
      // describing each step, whether the step is in the outer loop (outer) or the called subroutine (sub), the
      // depth of the recursive called subroutine level, and all graph mutations.
      void trace(TikzFormatter *formatter) { formatter_ = formatter; }
      bool tracing(void) { return(formatter_ != nullptr); }

      // Returns calculated lower and upper bounds for the chromatic number.  The lower bound is the clique number
      // as determined by the Bron Kerbosch algorithm.  The upper bound is determined by a sequential (greedy)
      // last-first algorithm.
      Color lower_bound(void) const { return lower_bound_; }
      Color upper_bound(void) const { return upper_bound_; }

      // The number of times that the bounding test was applied and the number of hits.
      ullong bounding_tries() const { return bounding_tries_; }
      ullong bounding_hits() const { return bounding_hits_; }

      // The number of times that the edge threshold test was applied and the number of hits.
      ullong edge_threshold_tries() const { return edge_threshold_tries_; }
      ullong edge_threshold_hits() const { return edge_threshold_hits_; }

      // The number of times that removal of vertices with degree < h has been attempted and the number of hits.
      ullong small_degree_tries() const { return small_degree_tries_; }
      ullong small_degree_hits() const { return small_degree_hits_; }

      // The number of times that the neighborhood subset test was applied and the number of hits.
      ullong neighborhood_subset_tries() const { return neighborhood_subset_tries_; }
      ullong neighborhood_subset_hits() const { return neighborhood_subset_hits_; }

      // The number of times that the common neighbor upper bound test was applied and the number of hits.
      ullong common_neighbors_tries() const { return common_neighbors_tries_; }
      ullong common_neighbors_hits() const { return common_neighbors_hits_; }

     private:
      using GraphPtr = std::unique_ptr<SimpleGraph>;

      Color lower_bound_;
      Color upper_bound_;
      Color adjusted_upper_bound_;

      ullong bounding_tries_;
      ullong bounding_hits_;
      ullong edge_threshold_tries_;
      ullong edge_threshold_hits_;
      ullong small_degree_tries_;
      ullong small_degree_hits_;
      ullong neighborhood_subset_tries_;
      ullong neighborhood_subset_hits_;
      ullong common_neighbors_tries_;
      ullong common_neighbors_hits_;

      TikzFormatter *formatter_;

      // Calls the base method, resets all derived context, and runs the algorithm.
      virtual bool run();

      // The steps of the outer loop are as follows:
      //
      //  1. Use the Bron Kerbosch algorithm to calculate the click number of the graph and use this value as the
      //     lower bound for the chromatic number: kmin.
      //
      //  2. Use the sequential last-first algorithm to calculate an upper bound for the chromatic number: kmax.
      //
      //  3. Initialize k to kmin.
      //
      //  4. If k = kmax then return this value.
      //
      //  5. Call the subroutine to determine if the current state of G is k-colorable for the current value of k.
      //     If so, then return with the current value of k.  If not, then the method will return a subgraph
      //     replacement for G to be used for the remainder of the algorithm.
      //
      //  6. Increment k and go to step 4.
      //
      // Each of these steps is counted.
      void outer_loop(GraphPtr *ppg);

      // The steps of the inner loop are as follows:
      //
      //  1.  If n <= k then return true.
      //
      //  2.  Calculate a maximum edge threshold: a = n^2(k-1)/2k.
      //
      //  3.  If m > a then return false.
      //
      //  4.  Construct a list of all vertices with degree < k.
      //
      //  5.  If such vertices exist, then remove them an go to step 1.
      //
      //  6.  If n < kmax then kmax = n.
      //
      //  7.  Calculate a lower bound for the current graph.
      //
      //  8.  If the new lower bound equals or exceeds the current upper bound then return false.
      //
      //  9.  Calculate the number of common vertices for each pair of vertices.
      //
      //  10.  If there exists a vertex whose neighborhood is a subset of another vertex then contract the two
      //       vertices and go to step 1.
      //
      //  11.  Determine the smallest number of common neighbors between any two vertices b.
      //
      //  12.  Calculate an upper bound for the smallest number of common neighbors: c=n-2-(n-2)/(k-1).
      //
      //  13.  If b > c then return false.
      //
      //  14.  Determine two non-adjacent vertices with the smallest number of common neighbors: u, v.
      //
      //  15.  If is_k_colorable(G.uv) then return true.
      //
      //  16.  If is_k_colorable(G+u) then return true.
      //
      //  17.  Return false.
      //
      // Each of these steps is counted, as well as a count for the current call and each recursive call.
      bool is_k_colorable(GraphPtr *ppg);

      // Resets all the derived-class counters.
      void reset_counters();

      // Calculates a lower bound for the chromatic number using the Bron Kerbosch algorithm to find the clique
      // number of the graph.
      void calculate_lower();

      // Calculates an upper bound for the chromatic number using the sequential last-first algorithm.
      void calculate_upper();

      // Returns true if n <= k.
      bool check_for_success(GraphPtr *ppg);

      // Calculates the maximum edge threshold for a graph assuming that it is k-colorable.
      double max_edge_threshold(GraphPtr *ppg);

      // Returns true if the number of edges does not exceed the maximum edge threshold.
      bool check_max_edges(GraphPtr *ppg, double a);

      // Finds vertices with degree < k.
      void find_small_degree(GraphPtr *ppg, VertexNumbers *x);

      // Removes the specified list of vertices.  Returns true if vertices were removed.
      bool remove_small_vertices(GraphPtr *ppg, const VertexNumbers &x);

      // Uses the Edwards Elphick algorithm to calculate a lower bound for the current graph and check it against
      // the current k value.  If the lower bound is greater then returns true to bound.
      bool check_bounding(GraphPtr *ppg);

      // Determines the number of common neighbors for each pair of vertices in the specified graph.  If the
      // neighborhood of one vertex is found to be a subset of another then the method returns true with the
      // subset vertex in v1 and the superset vertex in v2.  Otherwise, the method returns false and the return
      // values indicate the overall and nonadjacent smallest neighborhood intersections.
      bool find_common_neighbors(GraphPtr *ppg,
                                 Degree *smallest, VertexNumber *v1, VertexNumber *v2,
                                 Degree *smallest_nonadj, VertexNumber *v1_nonadj, VertexNumber *v2_nonadj);

      // Removes vertex v1, which was found to have a neighborhood that is a subset of v2.
      void remove_subset(GraphPtr *ppg, VertexNumber v1, VertexNumber v2);

      // Calculates the minimum common neighbors upper bound for a graph assuming that it is k-colorable.
      double min_common_ub(GraphPtr *ppg);

      // Returns true if the specified minimum common neighbors count does not exceed the specified upper bound.
      bool check_common_ub(double b, double c);

      // Contracts the two specified vertices.  Returns true if the resulting graph is k-colorable.
      bool contract_vertices(GraphPtr *ppg, VertexNumber v1, VertexNumber v2);

      // Adds an edge to the two specified vertices.  Returns true if the resulting graph is k-colorable.
      bool add_edge(GraphPtr *ppg, VertexNumber v1, VertexNumber v2);

      // Constructs a prefix for outer loop trace messages.
      void outer_prefix(void);

      // Constructs a prefix for called subroutine trace messages.
      void sub_prefix(void);

      // Identifies a vertex by number.  Either the label or vertex ID will be used.
      void identify_vertex(const GraphPtr &pg, VertexNumber iv);
    };

  }  // namespace graph
}  // namespace cavcom

#endif  // CAVCOM_GRAPH_LIBGRAPHALGO_QUICK_ZYKOV_H_
