// Derives and exercises a TikZ formatter class.

#include <sstream>

#include <libunittest/all.hpp>

#include "tikz_formatter.h"

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
                        "  \\begin{scope}[unlabeled node/.style={draw,circle,fill=#1,inner sep=0cm,"
                        "minimum size=0.2cm}, unlabeled node/.default={black}]\n"
                        "  \\end{scope}\n"
                        "\\end{tikzpicture}\n");
}
