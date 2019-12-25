#ifndef CAVCOM_GRAPH_LIBGRAPHUTIL_COMPONENT_ALGORITHM_H_
#define CAVCOM_GRAPH_LIBGRAPHUTIL_COMPONENT_ALGORITHM_H_

#include <vector>

#include "component_number_algorithm.h"

namespace cavcom {
  namespace graph {

    // Any algorithm that finds all of the components in a graph.
    class ComponentAlgorithm : public ComponentNumberAlgorithm {
     public:
      // Creates a new instance of a component algorithm for the specified graph.
      explicit ComponentAlgorithm(const SimpleGraph &graph, bool save = true);

      // Returns all found components, if components are being saved.
      const VertexNumbersList &components(void) const { return components_; }

      // If true then components are being saved when found.
      bool save(void) const { return save_; }

     protected:
      // Calls the base class method and resets the derived state.
      virtual bool run();

      // A method that is called as each component is identified.  A false return terminates the algorithm.
      virtual bool found(const VertexNumbers &component) { return true; }

      // This method is called by derived classes to call the found method for a found component and to add the
      // component to the found component list (if save is true).  Returns the found method return value.
      bool add_component(const VertexNumbers &next);

     private:
      // All found components, if being saved.  Otherwise, the first maximum component found.
      VertexNumbersList components_;

      // Save found components flag.
      bool save_;
    };

  }  // namespace graph
}  // namespace cavcom

#endif  // CAVCOM_GRAPH_LIBGRAPHUTIL_COMPONENT_ALGORITHM_H_
