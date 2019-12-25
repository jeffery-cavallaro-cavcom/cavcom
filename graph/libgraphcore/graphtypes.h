#ifndef CAVCOM_GRAPH_LIBGRAPHCORE_GRAPHTYPES_H_
#define CAVCOM_GRAPH_LIBGRAPHCORE_GRAPHTYPES_H_

#include <set>
#include <string>
#include <vector>

#include "types.h"

namespace cavcom {
  namespace graph {

    // Vertex and edge labels.
    using Label = std::string;

    // Vertex and edge colors are represented by unsigned integer values.  These values can be used as indexes into an
    // external color table for application-specific color assignments.  The color ID = 0 is interpreted as "none".
    using Color = uint;
    constexpr Color NOCOLOR = 0;

    // An edge's weight can be interpreted as the cost to traverse the edge between the endpoint vertices.
    using Weight = double;
    constexpr Weight FREE = 0.0;

    // Vertices are assigned an identifier that is invariant across graph mutations.  Thus, a vertex that changes
    // its position in the connection matrix is still identifiable with the corresponding vertex in the original
    // graph.
    using VertexID = uint;
    using VertexIDs = std::set<VertexID>;
    using VertexIDsList = std::vector<VertexIDs>;

    // Vertices can be assigned and X and Y position to aid drawing software.
    using Dimension = double;

  }  // namespace graph
}  // namespace cavcom

#endif  // CAVCOM_GRAPH_LIBGRAPHCORE_GRAPHTYPES_H_
