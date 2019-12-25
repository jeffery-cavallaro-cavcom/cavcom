#ifndef CAVCOM_GRAPH_LIBGRAPHALGO_QUICK_ZYKOV_H_
#define CAVCOM_GRAPH_LIBGRAPHALGO_QUICK_ZYKOV_H_

#include <memory>
#include <vector>

#include "tikz_formatter.h"
#include "vertex_coloring_algorithm.h"

namespace cavcom {
  namespace graph {

    // A modified version of a Zykov algorithm to determine the chromatic number of a graph.  The algorithm is
    // composed of an outer loop on increasing values of k and a recursive subroutine that determines if the
    // current state of the graph is k-colorable.  The first such k found is the chromatic number.  The first k
    // attempted (lower bound) is equal to the click number of the graph as determined by the Bron Kerbosch
    // algorithm.  The last possible k (upper bound) is determine by a last-first sequential (greedy) with coloring
    // interchange algorithm.
    class QuickZykov : public VertexColoringAlgorithm {
     public:
      // Creates a new algorithm instance for the specified graph.  If a formatter is specified then tracing is
      // enabled.
      explicit QuickZykov(const SimpleGraph &graph, TikzFormatter *formatter = nullptr);

      // Returns true if tracing is enable.
      bool tracing(void) { return(formatter_ != nullptr); }

      // Returns the calculated lower and upper bounds for the chromatic number.  The lower bound is the clique
      // number as determined by the Bron Kerbosch algorithm.  The upper bound is determined by greedy last-first
      // with color interchange.
      Color kmin(void) const { return kmin_; }
      Color kmax(void) const { return kmax_; }

      // Peg counters that measure the effectiveness of the upper-bound adjust test.
      ullong ubadjust_tries() const { return ubadjust_tries_; }
      ullong ubadjust_hits() const { return ubadjust_hits_; }

      // Peg counters that measure the effectiveness of the lower bound exceeding the upper bound test.
      ullong bounding_tries() const { return bounding_tries_; }
      ullong bounding_hits() const { return bounding_hits_; }

      // Peg counters that measure the effectiveness of the edge threshold test.
      ullong edge_threshold_tries() const { return edge_threshold_tries_; }
      ullong edge_threshold_hits() const { return edge_threshold_hits_; }

      // Peg counters that measure the effectiveness of the small degree vertex removal test.
      ullong small_degree_tries() const { return small_degree_tries_; }
      ullong small_degree_hits() const { return small_degree_hits_; }

      // Peg counters that measure the effectiveness of the neighborhood subset test.
      ullong neighborhood_subset_tries() { return neighborhood_subset_tries_; }
      ullong neighborhood_subset_hits() { return neighborhood_subset_hits_; }

      // Peg counters that measure the effectiveness of the minimum common neighbor upper bound test.
      ullong common_neighbors_tries() { return common_neighbors_tries_; }
      ullong common_neighbors_hits() { return common_neighbors_hits_; }

     protected:
      // Resets the base and derived contexts and then calls the outer loop of the algorithm.  Always returns true
      // since a valid answer is always found.
      virtual bool run(void);

     private:
      // The lower and upper bounds for the chromatic number of the input graph.  The lower bound is determined using
      // the Bron Kerbosch algorithm.  The upper bound is determined using a greedy last-first coloring with color
      // interchange.
      Color kmin_;
      Color kmax_;

      // The effective upper bound for the chromatic number.  The initial upper bound is reduced when a state in
      // one of the Zykov trees is found to have an order less than the current upper bound.
      Color kemax_;

      // Peg counters for the various bounding mechanisms.
      ullong ubadjust_tries_;
      ullong ubadjust_hits_;
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

      // Formatter, if tracing is enabled.
      TikzFormatter *formatter_;

      // Each of the target MISs results in a separate Zykov tree.  The initial graph for each tree is the input
      // graph with all of the vertices in the MIS contracted and edges added between the vertices in the MIS and
      // all of the other vertices.
      class ZykovTree : public VertexColoringAlgorithm {
       public:
        using GraphPtr = std::unique_ptr<SimpleGraph>;
        using VertexNumbersListPtr = std::unique_ptr<VertexNumbersList>;

        // Creates a new Zykov tree for the input graph using the specified MIS.  The initial chromatic number
        // upper bound for the graph is specified.  Note that this upper bound may be reduced during branching,
        // which is part of the bounding condition.  If a formatter is specified then tracing is enabled.  Each
        // trace line is identified using the specified tree number.
        ZykovTree(QuickZykov *parent,
                  const VertexNumbers &mis,
                  Color kmax,
                  TikzFormatter *formatter = nullptr,
                  ullong itree = 0);

        // Returns the current chromatic number upper bound for graphs in this tree.
        Color kmax(void) const { return kmax_; }

        // Returns the removed vertex list.
        const VertexNumbersList &removed(void) { return *removed_; }

        // Executes the modified Zykov algorithm to determine if the graph associated with this tree is
        // k-colorable.  Note that the graph, removed vertex list, and effective kmax may be altered.
        bool is_k_colorable(Color k);

       private:
        // Parent algorithm context.  Use for step, call, and peg counter access.
        QuickZykov *parent_;

        // The current state of the graph.  Note that the initial graph may be reduced by vertex removals and
        // vertex contractions.  The final graph will be a complete graph that represents a chromatic coloring of
        // the remaining vertices.
        GraphPtr pgraph_;

        // The current list of removed vertices in the order removed.  Each entry corresponds to a single or
        // contracted vertex.
        VertexNumbersListPtr removed_;

        // The current upper bound for the chromatic number bounding this tree.
        Color kmax_;

        // Formatter and tree number to use when tracing.
        TikzFormatter *formatter_;
        ullong itree_;
      };

      // The list of MIS-related Zykov trees.
      using ZykovTreeList = std::vector<ZykovTree>;
      ZykovTreeList trees_;

      // Resets all of the bounding test counters.
      void reset_counters(void);

      // The steps of the outer loop are as follows:
      //
      //  1. Use the Bron Kerbosch algorithm to calculate the click number of the graph and use this value as the
      //     lower bound for the chromatic number: kmin.
      //
      //  2. Use the sequential last-first algorithm to calculate an upper bound for the chromatic number: kmax.
      //
      //  3. If kmin = kmax then accept the initial greedy coloring and return kmin.
      //
      //  4. Use the Bron Kerbosch algorithm on the complement of the input graph to find all MISs.
      //
      //  5. Count how many times each vertex occurs in a MIS.
      //
      //  6. Target a vertex that occurs in the least number of MISs.
      //
      //  7. For each MIS containing the target vertex, construct a root state for a Zykov tree where the initial
      //     graph for the state is the input graph with all of the vertices in the corresponding MIS contracted
      //     and edges added between all vertices in the MIS and all vertices not in the MIS.  The initial
      //     chromatic number upper bound for each graph is set to the upper bound for the input graph.
      //
      //  8. Initialize k to kmin.
      //
      //  9. If k = kmax then accept the initial greedy coloring and return k.
      //
      //  10. For each Zykov tree, determine if the graph is k-colorable.
      //
      //  11. If one of the graphs is k-colorable then construct and accept a coloring from the resulting
      //      complete graph and the removed vertex list and return k.
      //
      //  12. Increment k and go to step 9.
      //
      void outer_loop(void);

      // Runs the Bron Kerbosch algorithm to establish the chromatic number lower bound.
      void set_lower_bound(void);

      // Runs the last-first greedy algorithm to establish the chromatic number upper bound and a default coloring.
      void set_upper_bound(void);

      // Runs the Bron Kerbosch algorithm on the graphs complement to find all the MISs for the graph, targets the
      // vertex that occurs in the least number of MISs, and then establishes the list of Zykov trees that will
      // need to be walked.
      void establish_trees(void);

      // Constructs a prefix for outer loop trace messages.
      void outer_prefix(void);

#if 0
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

      // Constructs a prefix for called subroutine trace messages.
      void sub_prefix(void);

      // Identifies a vertex by number.  Either the label or vertex ID will be used.
      void identify_vertex(const GraphPtr &pg, VertexNumber iv);
#endif
    };

  }  // namespace graph
}  // namespace cavcom

#endif  // CAVCOM_GRAPH_LIBGRAPHALGO_QUICK_ZYKOV_H_
