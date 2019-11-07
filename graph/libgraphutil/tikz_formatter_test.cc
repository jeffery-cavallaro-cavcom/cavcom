// Derives and exercises a TikZ formatter class.

#include <sstream>

#include <libunittest/all.hpp>

#include "tikz_formatter.h"

using namespace cavcom::graph;

static const VertexValuesList VERTICES = {{"\\(v_1\\)", NOCOLOR, 0, 0}, {Label(), 1, 4, 0}, {"\\(v_3\\)", 2, 1, 3}};
static const EdgeValuesList EDGES = {{0, 1, "\\(e_1\\)", 0, 1.5}, {1, 2, Label(), 3, 0.0}};

static const TikzFormatter::ColorType C1 = "green!25!white";
static const TikzFormatter::ColorType C2 = "blue!25!white";
static const TikzFormatter::ColorType C3 = "red";
static const TikzFormatter::ColorValuesList COLORS = {{1, C1}, {2, C2}, {3, C3}};

TEST(format_unlabeled_graph) {
  Graph graph(VERTICES, EDGES);
  std::ostringstream out;
  TikzFormatter formatter(&out);
  UNITTEST_ASSERT_TRUE(formatter.format(graph));
  UNITTEST_ASSERT_EQUAL(out.str(),
                        "\\begin{tikzpicture}\n"
                        "\\begin{scope}[every node/.style={draw,circle,fill=#1,inner sep=0cm,"
                        "minimum size=0.2cm}, every node/.default={black}]\n"
                        "\\node (0) at (0,0) {};\n"
                        "\\node (1) at (4,0) {};\n"
                        "\\node (2) at (1,3) {};\n"
                        "\\end{scope}\n"
                        "\\draw (0) edge (1);\n"
                        "\\draw (1) edge (2);\n"
                        "\\end{tikzpicture}\n");
}

TEST(format_labeled_graph) {
  Graph graph(VERTICES, EDGES);
  std::ostringstream out;
  TikzFormatter formatter(&out);
  formatter.vertex_labels(true);
  UNITTEST_ASSERT_TRUE(formatter.format(graph));

  UNITTEST_ASSERT_EQUAL(out.str(),
                        "\\begin{tikzpicture}\n"
                        "\\begin{scope}[every node/.style={draw,circle,minimum size=2em}]\n"
                        "\\node (0) at (0,0) {\\(v_1\\)};\n"
                        "\\node (1) at (4,0) {\\(1\\)};\n"
                        "\\node (2) at (1,3) {\\(v_3\\)};\n"
                        "\\end{scope}\n"
                        "\\draw (0) edge (1);\n"
                        "\\draw (1) edge (2);\n"
                        "\\end{tikzpicture}\n");
}

TEST(format_unlabeled_vertex_colors) {
  Graph graph(VERTICES, EDGES);
  std::ostringstream out;
  TikzFormatter formatter(&out);
  formatter.add_colors(COLORS);
  formatter.vertex_colors(true);
  UNITTEST_ASSERT_TRUE(formatter.format(graph));

  UNITTEST_ASSERT_EQUAL(out.str(),
                        "\\begin{tikzpicture}\n"
                        "\\begin{scope}[every node/.style={draw,circle,fill=#1,inner sep=0cm,"
                        "minimum size=0.2cm}, every node/.default={black}]\n"
                        "\\node (0) at (0,0) {};\n"
                        "\\node [fill=green!25!white] (1) at (4,0) {};\n"
                        "\\node [fill=blue!25!white] (2) at (1,3) {};\n"
                        "\\end{scope}\n"
                        "\\draw (0) edge (1);\n"
                        "\\draw (1) edge (2);\n"
                        "\\end{tikzpicture}\n");
}

TEST(format_labeled_vertex_colors) {
  Graph graph(VERTICES, EDGES);
  std::ostringstream out;
  TikzFormatter formatter(&out);
  formatter.add_colors(COLORS);
  formatter.vertex_labels(true);
  formatter.vertex_colors(true);
  UNITTEST_ASSERT_TRUE(formatter.format(graph));

  UNITTEST_ASSERT_EQUAL(out.str(),
                        "\\begin{tikzpicture}\n"
                        "\\begin{scope}[every node/.style={draw,circle,minimum size=2em}]\n"
                        "\\node (0) at (0,0) {\\(v_1\\)};\n"
                        "\\node [fill=green!25!white] (1) at (4,0) {\\(1\\)};\n"
                        "\\node [fill=blue!25!white] (2) at (1,3) {\\(v_3\\)};\n"
                        "\\end{scope}\n"
                        "\\draw (0) edge (1);\n"
                        "\\draw (1) edge (2);\n"
                        "\\end{tikzpicture}\n");
}

TEST(format_edges) {
  Graph graph(VERTICES, EDGES);
  std::ostringstream out;
  TikzFormatter formatter(&out);
  formatter.add_colors(COLORS);
  formatter.vertex_labels(true);
  formatter.vertex_colors(true);
  formatter.edge_labels(true);
  formatter.edge_colors(true);
  formatter.edge_weights(true);
  UNITTEST_ASSERT_TRUE(formatter.format(graph));

  UNITTEST_ASSERT_EQUAL(out.str(),
                        "\\begin{tikzpicture}\n"
                        "\\begin{scope}[every node/.style={draw,circle,minimum size=2em}]\n"
                        "\\node (0) at (0,0) {\\(v_1\\)};\n"
                        "\\node [fill=green!25!white] (1) at (4,0) {\\(1\\)};\n"
                        "\\node [fill=blue!25!white] (2) at (1,3) {\\(v_3\\)};\n"
                        "\\end{scope}\n"
                        "\\draw (0) edge node [auto] {\\(e_1\\)} node [auto,swap] {\\(1.5\\)} (1);\n"
                        "\\draw [red] (1) edge node [auto,swap] {\\(0\\)} (2);\n"
                        "\\end{tikzpicture}\n");
}
