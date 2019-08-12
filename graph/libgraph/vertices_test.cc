// Constructs vertex sets.

#include <stdexcept>

#include <libunittest/all.hpp>

#include "vertices.h"

using cavcom::graph::Vertex;
using cavcom::graph::Vertices;

static constexpr const char *TEST_LABELS[] = {"one", "two", "three", "four", "five",
                                              "six", "seven", "eight", "nine", "ten"};
static constexpr uint TEST_NLABELS = 10;

// A functor that throws an out-of-range exception.
class OutOfRangeAccessor {
 public:
  OutOfRangeAccessor(Vertices vertices) : vertices_(vertices) {}
  Vertex &operator()() { return vertices_[vertices_.size()]; }

 private:
  Vertices vertices_;
};

TEST(create_and_fill_vertex_set) {
  UNITTEST_TESTINFO("Creates and checks a vertex set");

  // Create and check an empty list.
  Vertices vertices;
  UNITTEST_ASSERT_EQUAL(vertices.size(), 0);
  OutOfRangeAccessor badaccess(vertices);
  UNITTEST_ASSERT_THROW(std::out_of_range, badaccess);

  // Add a bunch of test vertices.
  for (uint i = 0; i < TEST_NLABELS; ++i) {
    auto &vertex = vertices.push_back();
    UNITTEST_ASSERT_EQUAL(vertices.size(), i + 1);
    vertex.label(TEST_LABELS[i]);
    vertex.color(5*i);
    UNITTEST_ASSERT_EQUAL(vertices.find(vertex.id()), i);
  }
}
