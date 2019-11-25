#include <algorithm>

#include "errors.h"
#include "graph.h"

namespace cavcom {
  namespace graph {

    Graph::Graph(VertexNumber n, bool directed, bool multiple, bool loops)
      : connections_(n, directed, multiple, loops) {
      vertices_.reserve(n);
      for (VertexNumber iv = 0; iv < n; ++iv) {
        vertices_.add();
      }
    }

    Graph::Graph(const VertexValuesList &vertices, const EdgeValuesList &edges,
          bool directed, bool multiple, bool loops)
      : connections_(vertices.size(), directed, multiple, loops) {
      vertices_.reserve(vertices.size());
      for_each(vertices.cbegin(), vertices.cend(), [this](const VertexValues &vertex){ vertices_.add(vertex); });
      join(edges);
    }

    Graph::Graph(const Graph &source, bool noedges)
      : vertices_(source.vertices_, false, VertexNumbers()),
        connections_(source.order(), source.directed(), source.multiple(), source.loops()) {
      EdgeNumber m = (noedges ? 0 : source.size());
      for (EdgeNumber ie = 0; ie < m; ++ie) {
        const Edge &e = source.edge(ie);
        VertexNumber from, to;
        if (find_vertex(e.from(), &from) && find_vertex(e.to(), &to)) {
          join(from, to, e.label(), e.color(), e.weight());
        }
      }
    }

    Graph::Graph(const Graph &source, const VertexNumbers &vkeep)
      : vertices_(source.vertices_, true, vkeep),
        connections_(vkeep.size(), source.directed(), source.multiple(), source.loops()) {
      // Only add the edges with both endpoints in the subgraph.
      EdgeNumber m = source.size();
      for (EdgeNumber ie = 0; ie < m; ++ie) {
        const Edge &e = source.edge(ie);
        VertexNumber from, to;
        if (find_vertex(e.from(), &from) && find_vertex(e.to(), &to)) {
          join(from, to, e.label(), e.color(), e.weight());
        }
      }
    }

    Graph::Graph(const Graph &source, const VertexNumbers &vremove, const EdgeNumbers &eremove)
      : vertices_(source.vertices_, false, vremove),
        connections_(source.order() - vremove.size(), source.directed(), source.multiple(), source.loops()) {
      // Only add the edges that aren't marked for removal and with both endpoints in the subgraph.
      EdgeNumber m = source.size();
      for (EdgeNumber ie = 0; ie < m; ++ie) {
        if (eremove.find(ie) != eremove.cend()) continue;
        const Edge &e = source.edge(ie);
        VertexNumber from, to;
        if (find_vertex(e.from(), &from) && find_vertex(e.to(), &to)) {
          join(from, to, e.label(), e.color(), e.weight());
        }
      }
    }

    Graph::Graph(const Graph &source, VertexNumber from, VertexNumber to)
      : vertices_(source.vertices_, false, {from, to}),
        connections_(source.order() - 1, source.directed(), source.multiple(), source.loops()) {
      // The vertices must be distinct.
      if (from == to) throw SameVertexContractError(from);

      // Add the contracted vertex.
      const Vertex &vfrom = source.vertex(from);
      const Vertex &vto = source.vertex(to);
      vertices_.add(vto.label(), vto.color(), vto.xpos(), vto.ypos());

      // Mark the contracted vertex.
      VertexNumber cnum = vertices_.size() - 1;
      Vertex &cv = vertices_[cnum];

      // The contracted list for the new vertex is composed of the lists from the contracted vertices, and then
      // the contracted vertices themselves.
      contract(vfrom, vto, &cv.contracted_);

      // Merge edges.
      EdgeNumber m = source.size();
      for (EdgeNumber ie = 0; ie < m; ++ie) {
        const Edge &e = source.edge(ie);
        VertexNumber efrom = 0, eto = 0;
        source.find_vertex(e.from(), &efrom);
        source.find_vertex(e.to(), &eto);

        // Discard edges between the contracted vertices.
        if (((efrom == from) && (eto == to)) || ((efrom == to) && (eto == from))) continue;

        // Move edges incident to the contracted from vertex to the contracted to vertex.
        VertexNumber newfrom, newto;
        if ((efrom == from) || (efrom == to)) {
          newfrom = cnum;
          find_vertex(e.to(), &newto);
        } else if ((eto == from) || (eto == to)) {
          find_vertex(e.from(), &newfrom);
          newto = cnum;
        } else {
          find_vertex(e.from(), &newfrom);
          find_vertex(e.to(), &newto);
        }

        // Suppress multiple edges if disabled.
        if ((!adjacent(newfrom, newto)) || multiple()) join(newfrom, newto, e.label(), e.color(), e.weight());
      }
    }

    Degree Graph::join(VertexNumber from, VertexNumber to, const Label &label, Color color, Weight weight) {
      // Fetch the endpoint vertex IDs.  An out-of-range error will occur if the vertex numbers are invalid.
      VertexID fid = vertices_[from].id();
      VertexID tid = vertices_[to].id();

      // Construct and add the edge.
      EdgeNumber next = edges_.size();
      edges_.emplace_back(fid, tid, label, color, weight);
      return connections_.join(from, to, next);
    }

    Degree Graph::join(const EdgeValues &values) {
      return join(values.from, values.to, values.label, values.color, values.weight);
    }

    void Graph::join(const EdgeValuesList &values) {
      for_each(values.cbegin(), values.cend(), [this](const EdgeValues &edge){ join(edge); });
    }

    void Graph::contract(const Vertex &from, const Vertex &to, Contracted *contracted) {
      const Contracted &fc = from.contracted();
      if (fc.empty()) {
        contracted->push_back(from.id());
      } else {
        contracted->insert(contracted->end(), fc.cbegin(), fc.cend());
      }

      const Contracted &tc = to.contracted();
      if (tc.empty()) {
        contracted->push_back(to.id());
      } else {
        contracted->insert(contracted->end(), tc.cbegin(), tc.cend());
      }
    }

  }  // namespace graph
}  // namespace cavcom
