#ifndef CAVCOM_GRAPH_LIBGRAPHALGO_QUICK_ZYKOV_H_
#define CAVCOM_GRAPH_LIBGRAPHALGO_QUICK_ZYKOV_H_

#include <memory>

#include "graph_algorithm.h"

namespace cavcom {
  namespace graph {

    // A modified version of aZykov algorithm to determine the chromatic number of a graph.  The algorithm is
    // composed of an outer method that loops on increasing values of k and a called method that determines if the
    // current state of the graph is k-colorable.  The first such k found is the chromatic number.
    class QuickZykov : public GraphAlgorithm {
     public:
      using GraphPtr = std::unique_ptr<SimpleGraph>;

      QuickZykov(const SimpleGraph &g);

      // Returns the current k value.  If the algorithm is complete then this is the chromatic number.
      uint k() const { return k_; }

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

      // Returns the final complete graph that represents a chromatic coloring of the original G, or null if the
      // algorithm has not yet completed.
      const SimpleGraph &chromatic() const { return *chromatic_; }

     private:
      uint k_;
      GraphPtr chromatic_;

      ullong edge_threshold_tries_;
      ullong edge_threshold_hits_;
      ullong small_degree_tries_;
      ullong small_degree_hits_;
      ullong neighborhood_subset_tries_;
      ullong neighborhood_subset_hits_;
      ullong common_neighbors_tries_;
      ullong common_neighbors_hits_;

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
      //  6.  If there exists a vertex whose neighborhood is a subset of another vertex then remove the former.
      //
      //  7.  Determine the smallest number of common neighbors between any two vertices b.
      //
      //  8.  Calculate an upper bound for the smallest number of common neighbors: c=n-2-(n-2)/(k-1).
      //
      //  9.  If b > c then return false.
      //
      //  10.  Determine two non-adjacent vertices with the smallest number of common neighbors: u, v.
      //
      //  11.  If is_k_colorable(G.uv) then return true.
      //
      //  12.  If is_k_colorable(G+u) then return true.
      //
      //  13.  Return false.
      //
      // Each of these steps is counted, as well as a count for the current call and each recursive call.
      bool is_k_colorable(GraphPtr *ppg);

      // Determines the cardinality of the neighborhood intersection for each pair of vertices in the specified
      // graph.  Also finds such a non-adjacent pair.  If one vertex's neighborhood is found to be a subset of
      // another then the former is removed and the method returns true.  Otherwise, the smallest intersection
      // cardinality and the pair of non-adjacent vertices are returned.
      bool find_neighborhood_subset(GraphPtr *ppg, Degree *smallest, VertexNumber *v1, VertexNumber *v2);

      // Resets all the derived-class counters.
      void reset_counters();
    };

  }  // namespace graph
}  // namespace cavcom

#endif  // CAVCOM_GRAPH_LIBGRAPHALGO_QUICK_ZYKOV_H_
