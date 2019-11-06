// Derives and exercises a TikZ formatter class.

#include <sstream>

#include <libunittest/all.hpp>

#include "tikzformatter.h"

using namespace cavcom::graph;

static const VertexValuesList VERTICES = {{"v1"}, {"v2"}};
static const EdgeValuesList EDGES = {{0, 1, "e1"}};

TEST(format_unlabeled_graph) {
  Graph graph(VERTICES, EDGES);
  std::ostringstream out;
  TikzFormatter formatter(&out);
  UNITTEST_ASSERT_TRUE(formatter.format(graph));
  UNITTEST_ASSERT_EQUAL(out.str(),
                        "\\begin{tikzpicture}\n"
                        "\\end{tikzpicture}\n");
}
