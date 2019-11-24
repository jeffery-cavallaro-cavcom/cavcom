#ifndef CAVCOM_GRAPH_LIBGRAPHALGO_BRON2_H_
#define CAVCOM_GRAPH_LIBGRAPHALGO_BRON2_H_

#include <vector>

#include "graph_algorithm.h"

namespace cavcom {
  namespace graph {

    // Version 2 of the Bron-Kerbocsh-Schell algorithm for finding all of the maximal cliques in a graph.  Version 1
    // differs from version 2 in the way that the next candidate is selected.
    class Bron2 : public GraphAlgorithm {
     public:
      using VertexNumberList = std::vector<VertexNumber>;
      using Clique = VertexNumberList;
      using Cliques = std::vector<Clique>;

      // The following mode flags control what the algorithm looks for and what is saved.  ALL means that all
      // found cliques are counted saved.  MAX means that all found cliques are counted; however, only the first
      // found maximum clique is saved.  MAX_ONLY means that only the first found maximum clique is saved and
      // branches where the number of candidates is insufficent to construct a maximal clique larger than the current
      // maximum are abandoned.  Thus, not all maximal cliques are counted in the MAX_ONLY case.
      static const int MODE_ALL = 1;
      static const int MODE_MAX = 0;
      static const int MODE_MAX_ONLY = -1;

      // Creates a new instance of a Bron-2 algorithm for the specified graph.  If "save" is true then all found
      // cliques are saved internally for later return to the caller.  Otherwise, only the first found maximum
      // clique is saved.
      Bron2(const SimpleGraph &graph, int mode = MODE_ALL);

      // Returns all found cliques, if cliques are being saved.  Otherwise, the list will contain a single entry
      // that is the first found maximum clique.
      const Cliques &cliques(void) const { return cliques_; }

      // Returns the set operating mode.
      int mode(void) const { return mode_; }

      // Returns the number of cliques found.
      ullong total(void) const { return total_; }

      // Returns the clique number of the graph.  This is the order of the largest found maximal clique.
      VertexNumber number(void) const { return number_; }

     protected:
      // A method that is called as each maximal click is identified.
      virtual void found(const Clique &clique) {}

     private:
      // The current clique accumulator.
      Clique current_;

      // All found cliques, if being saved.  Otherwise, the first maximum clique found.
      Cliques cliques_;

      // Operating mode.
      int mode_;

      // Number of cliques found.  If cliques are being saved then this value matches the size of that list.
      ullong total_;

      // The clique number of the graph.  This value matches the order of the largest saved maximal clique.
      VertexNumber number_;

      // Resets all derived state and runs the algorithm.  Returns immediately for the null graph.
      virtual bool run();

      // Works through the specified list of candidate vertices attempting to construct maximal cliques using the
      // current state of the clique accumulator.  The used vertex list tracks vertices already used to ensure that
      // all found cliques are maximal.
      void extend(VertexNumberList *pcandidates, VertexNumberList *pused);
    };

  }  // namespace graph
}  // namespace cavcom

#endif  // CAVCOM_GRAPH_LIBGRAPHALGO_BRON2_H_
