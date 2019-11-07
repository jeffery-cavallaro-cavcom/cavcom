// Generates random graphs and checks the number of edges.

#include <libunittest/all.hpp>

#include "random_graph.h"

using namespace cavcom::graph;

static constexpr uint TRIALS = 1000;
static constexpr uint NODES = 5;
static constexpr uint EDGES = NODES*(NODES-1)/2;

TEST(generate_random_graphs) {
  // Repeat for various edge probabilities.
  for (double p = 0.1; p <= 0.9; p += 0.1) {
    double ev = EDGES*p;
    double var = ev*(1 - p);

    // Generate a bunch of random graphs, keeping a sum to total edges.
    ullong total = 0;
    for (uint i = 0; i < TRIALS; ++i) {
      RandomGraph graph(NODES, p);
      total += graph.size();
    }

    // Perform a hypothesis test on the sample mean at the 5% level (95% confidence).
    double sev = static_cast<double>(total)/TRIALS;  // sample expected value
    double ciub = 3.8416*var/TRIALS;  // 95% confidence interval squared upper bound.
    double ediff = ev - sev;
    double delta = ediff*ediff;
    UNITTEST_ASSERT_APPROX_EQUAL(delta, ciub, 0.1);  // be generous
  }
}

TEST(generate_empty_graph) {
  RandomGraph g1(NODES, 0.0);
  UNITTEST_ASSERT_EQUAL(g1.size(), 0);

  RandomGraph g2(NODES, -1.5);
  UNITTEST_ASSERT_EQUAL(g2.size(), 0);
}

TEST(generate_complete_graph) {
  RandomGraph g1(NODES, 1.0);
  UNITTEST_ASSERT_EQUAL(g1.size(), EDGES);

  RandomGraph g2(NODES, 1.5);
  UNITTEST_ASSERT_EQUAL(g2.size(), EDGES);
}
