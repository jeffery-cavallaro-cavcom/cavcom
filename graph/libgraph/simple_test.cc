// Creates and mutates simple graphs.

#include <stdexcept>

#include <libunittest/all.hpp>

#include "simple.h"

using cavcom::graph::SimpleGraph;

TEST(create_null_graph) {
  UNITTEST_TESTINFO("Creates a null graph and checks attributes");

  SimpleGraph g;

  UNITTEST_ASSERT_EQUAL(g.order(), 0);
  UNITTEST_ASSERT_EQUAL(g.size(), 0);
  UNITTEST_ASSERT_EQUAL(g.mindeg(), 0);
  UNITTEST_ASSERT_EQUAL(g.maxdeg(), 0);
  UNITTEST_ASSERT_THROW(std::out_of_range, [&g](){ return g[0]; });
  UNITTEST_ASSERT_THROW(std::out_of_range, [&g](){ return g.vertex(0); });
  UNITTEST_ASSERT_THROW(std::out_of_range, [&g](){ return g.edges(0, 0); });
  UNITTEST_ASSERT_THROW(std::out_of_range, [&g](){ return g.edge(0, 0, 0); });
  UNITTEST_ASSERT_THROW(std::out_of_range, [&g](){ return g.adjacent(0, 0); });
}
