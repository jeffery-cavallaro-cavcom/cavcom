// Derives and exercises a dummy formatter class.

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include <libunittest/all.hpp>

#include "formatter.h"

using namespace cavcom::graph;

class DummyFormatter : public Formatter<> {
 public:
  DummyFormatter(bool fail = false) : Formatter(&formatted_), fail_(fail) {}

  virtual bool start_graph(const SimpleGraph &graph) {
    *out_ << "start graph" << std::endl;
    return out_->good();
  }

  virtual bool start_vertices(const SimpleGraph &graph) {
    *out_ << "start vertices" << std::endl;
    return out_->good();
  }

  virtual bool format_vertex(const Vertex &vertex) {
    *out_ << "format vertex " << vertex.label() << std::endl;
    return out_->good();
  }

  virtual bool finish_vertices(const SimpleGraph &graph) {
    *out_ << "finish vertices" << std::endl;
    return out_->good();
  }

  virtual bool start_edges(const SimpleGraph &graph) {
    if (fail_) return false;
    *out_ << "start edges" << std::endl;
    return out_->good();
  }

  virtual bool format_edge(const Edge &edge) {
    *out_ << "format edge " << edge.label() << std::endl;
    return out_->good();
  }

  virtual bool finish_edges(const SimpleGraph &graph) {
    *out_ << "finish edges" << std::endl;
    return out_->good();
  }

  virtual bool finish_graph(const SimpleGraph &graph) {
    *out_ << "finish graph" << std::endl;
    return out_->good();
  }

  const std::string formatted(void) { return formatted_.str(); }

 private:
  std::ostringstream formatted_;
  bool fail_;
};

using FlagValues = struct { bool vlabels; bool vcolors; bool elabels; bool ecolors; bool eweights; };

static const FlagValues ALL_OFF = {false, false, false, false, false};
static const FlagValues ALL_ON = {true, true, true, true, true};

static const void check_flags(const DummyFormatter &formatter, const FlagValues &state) {
  UNITTEST_ASSERT_EQUAL(formatter.vertex_labels(), state.vlabels);
  UNITTEST_ASSERT_EQUAL(formatter.vertex_colors(), state.vcolors);
  UNITTEST_ASSERT_EQUAL(formatter.edge_labels(), state.elabels);
  UNITTEST_ASSERT_EQUAL(formatter.edge_colors(), state.ecolors);
  UNITTEST_ASSERT_EQUAL(formatter.edge_weights(), state.eweights);
}

TEST(toggle_flags) {
  DummyFormatter formatter;

  // By default the flags should all be disabled.
  check_flags(formatter, ALL_OFF);

  // Enable each flag separately.  The others should remain off.
  formatter.vertex_labels(true);
  check_flags(formatter, {true, false, false, false, false});
  formatter.vertex_labels(false);
  check_flags(formatter, ALL_OFF);

  formatter.vertex_colors(true);
  check_flags(formatter, {false, true, false, false, false});
  formatter.vertex_colors(false);
  check_flags(formatter, ALL_OFF);

  formatter.edge_labels(true);
  check_flags(formatter, {false, false, true, false, false});
  formatter.edge_labels(false);
  check_flags(formatter, ALL_OFF);

  formatter.edge_colors(true);
  check_flags(formatter, {false, false, false, true, false});
  formatter.edge_colors(false);
  check_flags(formatter, ALL_OFF);

  formatter.edge_weights(true);
  check_flags(formatter, {false, false, false, false, true});
  formatter.edge_weights(false);
  check_flags(formatter, ALL_OFF);

  // Disable each flag separately.  The others should remain on.
  formatter.vertex_labels(true);
  formatter.vertex_colors(true);
  formatter.edge_labels(true);
  formatter.edge_colors(true);
  formatter.edge_weights(true);

  check_flags(formatter, ALL_ON);

  formatter.vertex_labels(false);
  check_flags(formatter, {false, true, true, true, true});
  formatter.vertex_labels(true);
  check_flags(formatter, ALL_ON);

  formatter.vertex_colors(false);
  check_flags(formatter, {true, false, true, true, true});
  formatter.vertex_colors(true);
  check_flags(formatter, ALL_ON);

  formatter.edge_labels(false);
  check_flags(formatter, {true, true, false, true, true});
  formatter.edge_labels(true);
  check_flags(formatter, ALL_ON);

  formatter.edge_colors(false);
  check_flags(formatter, {true, true, true, false, true});
  formatter.edge_colors(true);
  check_flags(formatter, ALL_ON);

  formatter.edge_weights(false);
  check_flags(formatter, {true, true, true, true, false});
  formatter.edge_weights(true);
  check_flags(formatter, ALL_ON);
}

static const VertexValuesList VERTICES = {{"v1"}, {"v2"}};
static const EdgeValuesList EDGES = {{0, 1, "e1"}};

TEST(check_call_sequence) {
  // Use a graph with two vertices and one edge.
  SimpleGraph graph(VERTICES, EDGES);

  // Run the formatter.
  DummyFormatter formatter;
  UNITTEST_ASSERT_TRUE(formatter.format(graph));

  // The formatted output indicates the call sequence.
  UNITTEST_ASSERT_EQUAL(formatter.formatted(),
                        "start graph\n"
                        "start vertices\n"
                        "format vertex v1\n"
                        "format vertex v2\n"
                        "finish vertices\n"
                        "start edges\n"
                        "format edge e1\n"
                        "finish edges\n"
                        "finish graph\n");
}

TEST(check_failed_format) {
  SimpleGraph graph(VERTICES, EDGES);
  DummyFormatter formatter(true);
  UNITTEST_ASSERT_FALSE(formatter.format(graph));
  UNITTEST_ASSERT_EQUAL(formatter.formatted(),
                        "start graph\n"
                        "start vertices\n"
                        "format vertex v1\n"
                        "format vertex v2\n"
                        "finish vertices\n");
}

static const DummyFormatter::ColorValuesList COLORS = {{1, "green"}, {2, "blue"}, {3, "red"}};
static const DummyFormatter::ColorType OTHER = "orange";

TEST(add_and_check_colors) {
  DummyFormatter formatter;
  formatter.add_colors(COLORS);
  UNITTEST_ASSERT_EQUAL(formatter.colors(), COLORS.size());

  // Check the default color.
  UNITTEST_ASSERT_TRUE(formatter.get_color(NOCOLOR).empty());

  // Check the explicit colors.
  for_each(COLORS.cbegin(), COLORS.cend(), [&formatter](const DummyFormatter::ColorValues &values){
                                             UNITTEST_ASSERT_EQUAL(formatter.get_color(values.id), values.color);
                                           });

  // An out-of-range color ID should return the default color.
  UNITTEST_ASSERT_TRUE(formatter.get_color(4).empty());
}

TEST(change_default_color) {
  DummyFormatter formatter;
  formatter.add_colors(COLORS);
  formatter.add_color(NOCOLOR, OTHER);
  UNITTEST_ASSERT_EQUAL(formatter.get_color(NOCOLOR), OTHER);
  UNITTEST_ASSERT_EQUAL(formatter.get_color(5), OTHER);
}

TEST(change_colors) {
  DummyFormatter formatter;
  formatter.add_colors(COLORS);
  UNITTEST_ASSERT_EQUAL(formatter.get_color(2), "blue");
  formatter.add_color(2, OTHER);
  UNITTEST_ASSERT_EQUAL(formatter.get_color(2), OTHER);
}

TEST(remove_colors) {
  DummyFormatter formatter;
  formatter.add_colors(COLORS);
  formatter.add_color(NOCOLOR, OTHER);

  // Remove an explicit color; it should revert to the default color.
  UNITTEST_ASSERT_EQUAL(formatter.get_color(2), "blue");
  formatter.remove_color(2);
  UNITTEST_ASSERT_EQUAL(formatter.get_color(2), OTHER);

  // Remove (reset) the default color.
  UNITTEST_ASSERT_EQUAL(formatter.get_color(NOCOLOR), OTHER);
  formatter.remove_color(NOCOLOR);
  UNITTEST_ASSERT_TRUE(formatter.get_color(NOCOLOR).empty());
}
