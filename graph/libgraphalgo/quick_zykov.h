#ifndef CAVCOM_GRAPH_LIBGRAPHALGO_QUICK_ZYKOV_H_
#define CAVCOM_GRAPH_LIBGRAPHALGO_QUICK_ZYKOV_H_

#include <memory>
#include <ostream>

#include "tikz_formatter.h"
#include "graph_algorithm.h"

namespace cavcom {
  namespace graph {

    // A modified version of aZykov algorithm to determine the chromatic number of a graph.  The algorithm is
    // composed of an outer method that loops on increasing values of k and a called method that determines if the
    // current state of the graph is k-colorable.  The first such k found is the chromatic number.
    class QuickZykov : public GraphAlgorithm {
     public:
      using GraphPtr = std::unique_ptr<SimpleGraph>;

      explicit QuickZykov(const SimpleGraph &g);

      // Returns the current k value.  If the algorithm is complete then this is the chromatic number.
      uint k() const { return k_; }

      // Enables a trace using the specified formatter.  Null disables tracing (default).  Information is output
      // describing each step, whether the step is in the outer loop or the called method, the depth of the called
      // method, and all graph mutations.
      void trace(TikzFormatter *formatter) { formatter_ = formatter; }
      bool tracing(void) { return(formatter_ != nullptr); }

      // The number of times that the edge threshold test was applied and the number of hits.
      ulong edge_threshold_tries() const { return edge_threshold_tries_; }
      ulong edge_threshold_hits() const { return edge_threshold_hits_; }

      // The number of times that removal of vertices with degree < h has been attempted and the number of hits.
      ulong small_degree_tries() const { return small_degree_tries_; }
      ulong small_degree_hits() const { return small_degree_hits_; }

      // The number of times that the neighborhood subset test was applied and the number of hits.
      ulong neighborhood_subset_tries() const { return neighborhood_subset_tries_; }
      ulong neighborhood_subset_hits() const { return neighborhood_subset_hits_; }

      // The number of times that the common neighbor upper bound test was applied and the number of hits.
      ulong common_neighbors_tries() const { return common_neighbors_tries_; }
      ulong common_neighbors_hits() const { return common_neighbors_hits_; }

      // Returns the final complete graph that represents the termination condition of the algorithm.  Note that it
      // may have less vertices than the chromatic number.  It is mainly for testing and tracing purposes.
      const SimpleGraph &chromatic() const { return *chromatic_; }

     private:
      uint k_;
      GraphPtr chromatic_;

      ulong edge_threshold_tries_;
      ulong edge_threshold_hits_;
      ulong small_degree_tries_;
      ulong small_degree_hits_;
      ulong neighborhood_subset_tries_;
      ulong neighborhood_subset_hits_;
      ulong common_neighbors_tries_;
      ulong common_neighbors_hits_;

      TikzFormatter *formatter_;

      // Resets all the counters and runs the algorithm.
      virtual bool run();

      // The steps of the outer loop are as follows:
      //
      //  1. Check for a null graph (n=0) condition.  If so, stop with k = 0.
      //
      //  2. Check for an empty graph (m=0) condition.  If so, stop with k = 1.
      //
      //  3. Initialize k to 2.
      //
      //  4. Call the method to determine if the current state of G is k-colorable, for the current value of k.  If
      //     so, then return with the current value of k.  If not, then the method will return a subgraph replacement
      //     for G to be used for the remainder of the algorithm (see the called method for details).
      //
      //  5. Increment k and go to step 4.
      //
      // Each of these steps is counted.
      void outer_loop(GraphPtr *ppg);

      // A wrapper for the recursive method to handle call/depth counting.
      bool subroutine(GraphPtr *ppg);

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
      //  5.  If such vertices exist, then remove then an go to step 1.
      //
      //  6.  Calculate the number of common vertices for each pair of vertices.
      //
      //  7.  If there exists a vertex whose neighborhood is a subset of another vertex then remove the former.
      //
      //  8.  Determine the smallest number of common neighbors between any two vertices b.
      //
      //  9.  Calculate an upper bound for the smallest number of common neighbors: c=n-2-(n-2)/(k-1).
      //
      //  10.  If b > c then return false.
      //
      //  11.  Determine two non-adjacent vertices with the smallest number of common neighbors: u, v.
      //
      //  12.  If is_k_colorable(G.uv) then return true.
      //
      //  13.  If is_k_colorable(G+u) then return true.
      //
      //  14.  Return false.
      //
      // Each of these steps is counted, as well as a count for the current call and each recursive call.
      bool is_k_colorable(GraphPtr *ppg);

      // Resets all the derived-class counters.
      void reset_counters();

      // Checks for a null graph.
      bool check_for_null(GraphPtr *ppg);

      // Checks for an empty graph.
      bool check_for_empty(GraphPtr *ppg);

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

      // Constructs a prefix for called method trace messages.
      void inner_prefix(void);

      // Identifies a vertex by number.  Either the label or vertex ID will be used.
      void identify_vertex(const GraphPtr &pg, VertexNumber iv);
    };

  }  // namespace graph
}  // namespace cavcom

#endif  // CAVCOM_GRAPH_LIBGRAPHALGO_QUICK_ZYKOV_H_
