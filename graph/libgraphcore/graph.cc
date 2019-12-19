#include <algorithm>
#include <map>

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
      std::for_each(vertices.cbegin(), vertices.cend(),
                    [this](const VertexValues &vertex){ vertices_.add(vertex); });
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
      : Graph(source, VertexNumbersList({VertexNumbers({from, to})})) {
      if (from == to) throw SameVertexContractError(to);
    }

    Graph::Graph(const Graph &source, const VertexNumbersList &contract)
      : vertices_(source.vertices_, false, contract),
        connections_(after_contraction(vertices_.size(), contract),
                     source.directed(), source.multiple(), source.loops()) {
      // Construct a map that indicates where each vertex is going.  A vertex can only appear once.
      using VertexLookup = std::map<VertexNumber, VertexNumbersList::size_type>;
      VertexLookup where;
      VertexNumbersList::size_type ns = contract.size();
      for (VertexNumbersList::size_type is = 0, ic = 0; is < ns; ++is) {
        const VertexNumbers &s = contract[is];
        if (s.size() <= 1) continue;
        std::for_each(s.cbegin(), s.cend(),
                      [&](VertexNumber iv){
                        if (!where.insert({iv, ic}).second) throw SameVertexContractError(iv);
                      });
        ++ic;
      }

      // Contract each subset.
      std::vector<VertexNumbersList::size_type> vwhere;
      for (VertexNumbersList::size_type is = 0; is < ns; ++is) {
        const VertexNumbers &s = contract[is];

        // Ignore empty and singleton sets.
        if (s.size() <= 1) continue;

        // Add the contracted vertex based on the first vertex in the set.
        VertexNumber to = *s.cbegin();
        const Vertex &vto = source.vertex(to);
        vertices_.add(vto.label(), vto.color(), vto.xpos(), vto.ypos());

        // Mark the contracted vertex.
        VertexNumber cnum = vertices_.size() - 1;
        vwhere.push_back(cnum);
        Vertex &cv = vertices_[cnum];

        // Construct the new contracted list.
        Contracted &all = cv.contracted_;
        std::for_each(s.cbegin(), s.cend(),
                      [&](VertexNumber iv){
                        const Vertex &v = source.vertex(iv);
                        const Contracted &c = v.contracted();
                        if (c.empty()) {
                          all.insert(v.id());
                        } else {
                          all.insert(c.cbegin(), c.cend());
                        }
                      });
      }

      // Merge edges.
      EdgeNumber m = source.size();
      for (EdgeNumber ie = 0; ie < m; ++ie) {
        const Edge &e = source.edge(ie);

        // Determine where the edge was in the source graph.
        VertexNumber efrom = 0, eto = 0;
        source.find_vertex(e.from(), &efrom);
        source.find_vertex(e.to(), &eto);

        // Determine where the edge is in the new graph.
        VertexNumber newfrom = 0;
        VertexNumber newto = 0;
        VertexLookup::const_iterator found = where.find(efrom);
        if (found == where.cend()) {
          find_vertex(source.vertex(efrom).id(), &newfrom);
        } else {
          newfrom = vwhere[found->second];
        }

        found = where.find(eto);
        if (found == where.cend()) {
          find_vertex(source.vertex(eto).id(), &newto);
        } else {
          newto = vwhere[found->second];
        }

        // Discard edges between contracted vertices.
        if (newfrom == newto) continue;

        // Discard multiple edges if not allowed.
        if (adjacent(newfrom, newto) && (!multiple())) continue;

        // Join the new endpoints.
        join(newfrom, newto, e.label(), e.color(), e.weight());
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
      std::for_each(values.cbegin(), values.cend(), [this](const EdgeValues &edge){ join(edge); });
    }

    VertexNumber Graph::after_contraction(VertexNumber start, const VertexNumbersList &contract) {
      std::for_each(contract.cbegin(), contract.cend(),
                    [&](const VertexNumbers &s){
                      if (s.size() > 1) ++start;
                    });
      return start;
    }

  }  // namespace graph
}  // namespace cavcom
