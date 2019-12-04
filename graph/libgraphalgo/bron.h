#ifndef CAVCOM_GRAPH_LIBGRAPHALGO_BRON_H_
#define CAVCOM_GRAPH_LIBGRAPHALGO_BRON_H_

#include "clique_algorithm.h"

namespace cavcom {
  namespace graph {

    // The base class for the Bron Kerbosch algorithm for finding all of the maximal cliques in a graph.  It
    // contains all common context for the two versions of the algorithm.
    class Bron : public CliqueAlgorithm {
     public:
      // Creates a new instance of a Bron algorithm for the specified graph.
      Bron(const SimpleGraph &graph, int mode = MODE_ALL, bool save = true);

     protected:
      using VertexNumberList = std::vector<VertexNumber>;

      // The current clique accumulator.
      VertexNumberList current_;

     private:
      // Resets all derived state and runs the algorithm.  Returns immediately for the null graph.
      virtual bool run();

      // Works through the specified list of candidate vertices attempting to construct maximal cliques using the
      // current state of the clique accumulator.  The used vertex list tracks vertices already used to ensure that
      // all found cliques are maximal.  Returns false if terminated by a found method.
      virtual bool extend(VertexNumberList *pcandidates, VertexNumberList *pused) = 0;
    };

  }  // namespace graph
}  // namespace cavcom

#endif  // CAVCOM_GRAPH_LIBGRAPHALGO_BRON_H_
