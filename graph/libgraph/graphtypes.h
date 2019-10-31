#ifndef CAVCOM_GRAPH_LIBGRAPH_GRAPHTYPES_H_
#define CAVCOM_GRAPH_LIBGRAPH_GRAPHTYPES_H_

#include <string>

#include "types.h"

namespace cavcom {
  namespace graph {

    // Vertex and edge labels.
    using Label = std::string;

    // Vertex and edge colors are represented by unsigned integer values.  These values can be used as indexes into
    // an external color table for application-specific color assignment.  The default color value is 0, which
    // should correspond to black.
    using Color = uint;
    constexpr Color BLACK = 0;

    // An edge's weight can be interpreted as the cost to traverse the edge between the endpoint vertices.
    using Weight = double;
    constexpr Weight FREE = 0.0;

    // Vertices are assigned an identifier that is invariant across graph mutations.  Thus, a vertex that changes
    // its position in the connection matrix is still identifiable with the corresponding vertex in the original
    // graph.
    using VertexID = uint;

  }  // namespace graph
}  // namespace cavcom

#endif  // CAVCOM_GRAPH_LIBGRAPH_GRAPHTYPES_H_
