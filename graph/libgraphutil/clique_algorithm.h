#ifndef CAVCOM_GRAPH_LIBGRAPHUTIL_CLIQUE_ALGORITHM_H_
#define CAVCOM_GRAPH_LIBGRAPHUTIL_CLIQUE_ALGORITHM_H_

#include <vector>

#include "clique_number_algorithm.h"

namespace cavcom {
  namespace graph {

    // Any algorithm that finds all of the maximal cliques in a graph.
    class CliqueAlgorithm : public CliqueNumberAlgorithm {
     public:
      using Cliques = std::vector<VertexNumbers>;

      // The following mode flags control what is saved.  ALL means that all found maximal cliques are counted and
      // saved.  MAX_ONLY means that all found maximal cliques are counted; however, only maximum cliques are
      // saved.  FIRST_MAX means that only the first found maximum clique is saved.  Derived algorithms should use
      // FIRST_MAX as a hint to terminate branches where the number of candidate vertices is insufficent to
      // construct a maximal clique larger than the current maximum.  Thus, not all maximal cliques may be counted
      // in the FIRST_MAX case.
      static const int MODE_ALL = 1;
      static const int MODE_MAX_ONLY = 0;
      static const int MODE_FIRST_MAX = -1;

      // Creates a new instance of a clique algorithm for the specified graph.
      explicit CliqueAlgorithm(const SimpleGraph &graph, int mode = MODE_ALL, bool save = true);

      // Returns all found cliques, if cliques are being saved.  Otherwise, the list will contain a single entry
      // that is the first found maximum clique.
      const Cliques &cliques(void) const { return cliques_; }

      // Returns the set operating mode.
      int mode(void) const { return mode_; }

      // Returns the state of the save flag.  If save is true then all found maximal cliques are saved according to
      // the mode.  If false then maximal cliques are counted according to the mode; however, no cliques are saved.
      // If saving is disables then it is assumed that any needed clique processing is performed by the derived
      // class found method.
      bool save(void) const { return save_; }

      // Returns the total number of cliques found.  Note that this may not be the actual number of maximal cliques
      // in the graph in FIRST_MAX mode.
      ullong total(void) const { return total_; }

     protected:
      // Calls the base class method and resets the derived state.
      virtual bool run();

      // A method that is called as each maximal clique is identified.  A false return terminates the algorithm.
      virtual bool found(const VertexNumbers &clique) { return true; }

      // This method is called by derived classes to call the found method for a found maximal clique and to add
      // the clique to the found clique list.  The maximal clique will only be added if the found method returns
      // true and depending on the mode and save flag.  Returns the found method return value.
      bool add_clique(const VertexNumbers &next);

     private:
      // All found cliques, if being saved.  Otherwise, the first maximum clique found.
      Cliques cliques_;

      // Operating mode.
      int mode_;

      // Save found maximal cliques flag.
      bool save_;

      // Total number of maximal cliques found.
      ullong total_;
    };

  }  // namespace graph
}  // namespace cavcom

#endif  // CAVCOM_GRAPH_LIBGRAPHUTIL_CLIQUE_ALGORITHM_H_
