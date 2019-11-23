#ifndef CAVCOM_GRAPH_LIBGRAPHALGO_BRON1_H_
#define CAVCOM_GRAPH_LIBGRAPHALGO_BRON1_H_

#include <vector>

#include "graph_algorithm.h"

namespace cavcom {
  namespace graph {

    // Version 1 of the Bron-Kerbocsh-Schell algorithm for finding all of the maximal cliques in a graph.
    class Bron1 : public GraphAlgorithm {
     public:
      using VertexNumberList = std::vector<VertexNumber>;
      using Clique = VertexNumberList;
      using Cliques = std::vector<Clique>;

      // Creates a new instance of a Bron-1 algorithm for the specified graph.
      Bron1(const SimpleGraph &graph);

      // Returns the list of all found maximal cliques.
      const Cliques &cliques(void) { return cliques_; }

     private:
      // The list of found maximal cliques.
      Cliques cliques_;

      // The current clique accumulator.
      Clique current_;

      // Resets all derived state and runs the algorithm.  Returns immediately for the null graph.
      virtual bool run();

      // Works through the specified list of candidate vertices attempting to construct maximal cliques using the
      // current state of the clique accumulator.  The used vertex list tracks vertices already used to ensure that
      // all found cliques are maximal.
      void extend(VertexNumberList *pcandidates, VertexNumberList *pused);
    };

  }  // namespace graph
}  // namespace cavcom

#endif  // CAVCOM_GRAPH_LIBGRAPHALGO_BRON1_H_
