// Throws the various graph error conditions.

#include <libunittest/all.hpp>

#include "errors.h"

using namespace cavcom::graph;

TEST(throw_multiple_edge_exception) {
  constexpr VertexNumber FROM = 1;
  constexpr VertexNumber TO = 2;
  constexpr EdgeNumber EDGE = 3;
  const std::string MESSAGE = "Attempted multiple edge from vertex 1 to vertex 2 with edge 3";

  try {
    throw MultipleEdgeError(FROM, TO, EDGE);
  }
  catch (const MultipleEdgeError &error) {
    UNITTEST_ASSERT_EQUAL(error.from(), FROM);
    UNITTEST_ASSERT_EQUAL(error.to(), TO);
    UNITTEST_ASSERT_EQUAL(error.edge(), EDGE);
    UNITTEST_ASSERT_EQUAL(error.what(), MESSAGE);
  }
}

TEST(throw_loop_edge_exception) {
  constexpr VertexNumber VERTEX = 5;
  constexpr EdgeNumber EDGE = 1;
  const std::string MESSAGE = "Attempted loop edge on vertex 5 with edge 1";

  try {
    throw LoopEdgeError(VERTEX, EDGE);
  }
  catch (const LoopEdgeError &error) {
    UNITTEST_ASSERT_EQUAL(error.vertex(), VERTEX);
    UNITTEST_ASSERT_EQUAL(error.edge(), EDGE);
    UNITTEST_ASSERT_EQUAL(error.what(), MESSAGE);
  }
}

TEST(throw_contractions_exception) {
  constexpr VertexNumber VERTEX = 3;
  const std::string MESSAGE = "Cannot contract vertex 3 with itself";

  try {
    throw SameVertexContractError(VERTEX);
  }
  catch (const SameVertexContractError &error) {
    UNITTEST_ASSERT_EQUAL(error.vertex(), VERTEX);
    UNITTEST_ASSERT_EQUAL(error.what(), MESSAGE);
  }
}
