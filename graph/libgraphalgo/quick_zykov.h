#ifndef CAVCOM_GRAPH_LIBGRAPHALGO_QUICK_ZYKOV_H_
#define CAVCOM_GRAPH_LIBGRAPHALGO_QUICK_ZYKOV_H_

#include <memory>
#include <string>
#include <vector>

#include "container_formatter.h"
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

      // Returns the formatter if tracing, else null.
      TikzFormatter *formatter(void) { return formatter_; }

      // Returns the calculated lower and upper bounds for the chromatic number.  The lower bound is the clique
      // number as determined by the Bron Kerbosch algorithm.  The upper bound is determined by greedy last-first
      // with color interchange.
      Color kmin(void) const { return kmin_; }
      Color kmax(void) const { return kmax_; }

      // Peg counters that measure the effectiveness of the edge threshold test.
      ullong edge_threshold_tries() const { return edge_threshold_tries_; }
      ullong edge_threshold_hits() const { return edge_threshold_hits_; }
      void edge_threshold_add(bool hit = false);

      // Peg counters that measure the effectiveness of the small degree vertex removal test.
      ullong small_degree_tries() const { return small_degree_tries_; }
      ullong small_degree_hits() const { return small_degree_hits_; }
      void small_degree_add(bool hit = false);

      // Peg counters that measure the effectiveness of the neighborhood subset test.
      ullong neighborhood_subset_tries() const { return neighborhood_subset_tries_; }
      ullong neighborhood_subset_hits() const { return neighborhood_subset_hits_; }
      void neighborhood_subset_add(bool hit = false);

      // Peg counters that measure the effectiveness of the minimum common neighbor upper bound test.
      ullong common_neighbors_tries() const { return common_neighbors_tries_; }
      ullong common_neighbors_hits() const { return common_neighbors_hits_; }
      void common_neighbors_add(bool hit = false);

      // Peg counters that measure the effectiveness of the lower bound exceeding the upper bound test.
      ullong bounding_tries() const { return bounding_tries_; }
      ullong bounding_hits() const { return bounding_hits_; }
      void bounding_add(bool hit = false);

      // Methods to format containers when tracing.
      std::ostream &format_ids(const VertexIDs &ids, bool pad);
      std::ostream &format_ids_list(const VertexIDsList &ids_list, bool pad);
      std::ostream &format_numbers_as_ids(const SimpleGraph &g, const VertexNumbers &numbers, bool pad);
      std::ostream &format_numbers_list_as_ids(const SimpleGraph &g, const VertexNumbersList &numbers_list, bool pad);

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

      // Peg counters for the various bounding mechanisms.
      ullong edge_threshold_tries_;
      ullong edge_threshold_hits_;
      ullong small_degree_tries_;
      ullong small_degree_hits_;
      ullong neighborhood_subset_tries_;
      ullong neighborhood_subset_hits_;
      ullong common_neighbors_tries_;
      ullong common_neighbors_hits_;
      ullong bounding_tries_;
      ullong bounding_hits_;

      // Formatter, if tracing is enabled.
      TikzFormatter *formatter_;
      cavcom::utility::ContainerFormatter containers_;

      // Each of the target MISs results in a separate Zykov tree.  The initial graph for each tree is the input
      // graph with all of the vertices in the MIS contracted and edges added between the vertices in the MIS and
      // all of the other vertices.
      class ZykovTree : public VertexColoringAlgorithm {
       public:
        using GraphPtr = std::unique_ptr<SimpleGraph>;
        using VertexIDsListPtr = std::unique_ptr<VertexIDsList>;

        // Creates a new Zykov tree for the input graph using the specified MIS.  If a formatter is specified then
        // tracing is enabled.  Each trace line is identified using the specified tree number.
        ZykovTree(QuickZykov *parent, const VertexNumbers &mis, ullong itree = 0);

        // Returns the current state of the reduced graph.
        const SimpleGraph &current(void) const { return *pgraph_; }

        // Returns the removed vertex list.
        const VertexIDsList &removed(void) { return *premoved_; }

        // Constructs the final chromatic coloring based on the specified complete graph and removed vertex list.
        // Each vertex in the complete graph is assigned its own color, which is used for all of the contracted
        // vertices.  The removed vertices are then greedy colored (without color interchange) in reverse removal
        // order.
        void construct_coloring();

        // Count steps and calls in the parent.
        void add_step() { parent_->add_step(); }
        ullong steps() { return parent_->steps(); }

        void add_call() { parent_->add_call(); }
        void done_call() { parent_->done_call(); }
        ullong calls() { return parent_->calls(); }

        ullong depth() { return parent_->depth(); }

       protected:
        // Executes the modified Zykov algorithm.  Returns true if a k-colorable solution is found in the tree.
        virtual bool run();

       private:
        // Parent algorithm context.  Use for step, call, and peg counter access.
        QuickZykov *parent_;

        // The current state of the graph.  Note that the initial graph may be reduced by vertex removals and
        // vertex contractions.  The final graph will be a complete graph that represents a chromatic coloring of
        // the remaining vertices.
        GraphPtr pgraph_;

        // The current list of removed vertices in the order removed.  Each entry corresponds to a single or
        // contracted vertex.
        VertexIDsListPtr premoved_;

        // Tree number to use when tracing.
        ullong itree_;

        // The steps of the recursive subroutine are as follows:
        //
        //  1. If n <= k then return true.
        //
        //  2. Calculate a maximum edge threshold: a = n^2(k-1)/2k.
        //
        //  3. If m > a then return false.
        //
        //  4. Construct a list of all vertices with degree < k.
        //
        //  5. If such vertices exist, then remove them an go to step 1.
        //
        //  6. Calculate the number of common vertices for each pair of vertices.
        //
        //  7. If there exists a vertex whose neighborhood is a subset of another vertex then contract the two
        //     vertices and go to step 1.
        //
        //  8. Determine the smallest number of common neighbors between any two vertices b.
        //
        //  9. Calculate an upper bound for the smallest number of common neighbors: c=n-2-(n-2)/(k-1).
        //
        //  10. If b > c then return false.
        //
        //  12. Calculate a lower bound w for the current graph.
        //
        //  13. If w > k return false.
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
        bool is_k_colorable(GraphPtr *ppgraph, VertexIDsListPtr *ppremoved);

        // Returns true if n <= k.
        bool check_for_success(const SimpleGraph &g);

        // Calculates the maximum edge threshold for a graph assuming that it is k-colorable.
        double max_edge_threshold(const SimpleGraph &g);

        // Returns false if the number of edges exceeds the maximum edge threshold.
        bool check_max_edges(const SimpleGraph &g, double a);

        // Finds vertices with degree < k.
        void find_small_degree(const SimpleGraph &g, VertexNumbers *x);

        // Removes the specified list of vertices.  Returns true if vertices were removed.
        bool remove_small_vertices(GraphPtr *ppgraph, VertexIDsListPtr *ppremoved, const VertexNumbers &x);

        // Determines the number of common neighbors for each pair of vertices in the specified graph.  If the
        // neighborhood of one vertex is found to be a subset of another then the method returns true with the
        // subset vertex in v1 and the superset vertex in v2.  Otherwise, the method returns false and the return
        // values indicate the overall and nonadjacent smallest neighborhood intersections.
        bool find_common_neighbors(const SimpleGraph &g,
                                   Degree *smallest, VertexNumber *v1, VertexNumber *v2,
                                   Degree *smallest_nonadj, VertexNumber *v1_nonadj, VertexNumber *v2_nonadj);

        // Removes vertex v1, which was found to have a neighborhood that is a subset of v2.
        void remove_subset(GraphPtr *ppgraph, VertexNumber v1, VertexNumber v2);

        // Calculates the minimum common neighbors upper bound for a graph assuming that it is k-colorable.
        double min_common_ub(const SimpleGraph &g);

        // Returns true if the specified minimum common neighbors count does not exceed the specified upper bound.
        bool check_common_ub(double b, double c);

        // Calculates a new lower bound for the specified graph.
        Color calc_lb(const SimpleGraph &g);

        // Returns true if bounding should occur.
        bool check_bounding(Color lb);

        // Contracts the two specified vertices.  Returns true if the resulting graph is k-colorable.
        bool contract_vertices(GraphPtr *ppgraph, VertexIDsListPtr *ppremoved, VertexNumber v1, VertexNumber v2);

        // Adds an edge to the two specified vertices.  Returns true if the resulting graph is k-colorable.
        bool add_edge(GraphPtr *ppgraph, VertexIDsListPtr *ppremoved, VertexNumber v1, VertexNumber v2);

        // Returns the parent formatter.
        TikzFormatter *formatter(void) { return parent_->formatter(); }

        // Constructs a prefix for recursive subroutine trace messages.
        std::ostream &sub_prefix(void);
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
      //  7. Extract the MISs containing the target vertex.
      //
      //  8. Sort the target MISs by decreasing size.
      //
      //  9. For each MIS containing the target vertex, construct a root state for a Zykov tree where the initial
      //     graph for the state is the input graph with all of the vertices in the corresponding MIS contracted
      //     and edges added between all vertices in the MIS and all vertices not in the MIS.
      //
      //  10. Initialize k to kmin.
      //
      //  11. If k = kmax then accept the initial greedy coloring and return k.
      //
      //  12. For each Zykov tree, determine if the graph is k-colorable.
      //
      //  13. If one of the graphs is k-colorable then construct and accept a coloring from the resulting complete
      //      graph and removed vertex list and return k.
      //
      //  14. Increment k and go to step 9.
      //
      void outer_loop(void);

      // Runs the Bron Kerbosch algorithm to establish the chromatic number lower bound for the input graph.
      void set_lower_bound(void);

      // Runs the last-first greedy algorithm to establish the chromatic number upper bound and a default coloring
      // for the input graph.
      void set_upper_bound(void);

      // Returns true if the found chromatic number lower and upper bounds for the input graph match.
      bool check_for_match(void);

      // Runs the Bron Kerbosch algorithm on the input graph's complement to find all the MISs for the graph,
      // targets the vertex that occurs in the least number of MISs, extracts thoses MISs and sorts them by
      // decreasing length, and then establishes the list of Zykov trees, one per target MIS, that will need to be
      // walked.
      void make_trees(void);

      // Finds the target MISs in the input graph.
      void find_target_miss(VertexNumbersList *targets);

      // Determines which vertex occurs in the least number of MISs.
      VertexNumber find_target_vertex(const VertexNumbersList &miss);

      // Sorts the target MISs by decreasing size.
      void sort_target_miss(const VertexNumbersList &unsorted, VertexNumbersList *sorted);

      // Constructs a prefix for outer loop trace messages.
      std::ostream &outer_prefix(void);

      // Formats the specified container.
      template <typename C>
      std::ostream &format_container(const C &data, bool pad) {
        containers_.start(pad ? "  {" : "{");
        containers_(data.cbegin(), data.cend()) << std::endl;
        return containers_.out();
      }
    };

  }  // namespace graph
}  // namespace cavcom

#endif  // CAVCOM_GRAPH_LIBGRAPHALGO_QUICK_ZYKOV_H_
