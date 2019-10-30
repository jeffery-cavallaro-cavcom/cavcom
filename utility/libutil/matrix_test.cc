// Tests the 2-D array Matrix class.

#include <ctime>
#include <memory>

#include <libunittest/all.hpp>

#include "matrix.h"

using TestMatrix = cavcom::utility::Matrix<int>;
using TestMatrixPtr = std::unique_ptr<TestMatrix>;

// A test fixture that creates a n x m matrix and fills it with random values from -100 to 100.
template <uint n, uint m> struct TestArrayFixture {
  uint nrows = n;
  uint ncols = m;
  uint nelements = nrows*ncols;

  int values[n][m];

  TestArrayFixture() {
    auto rng = unittest::make_random_value<int>(-100, 100);
    rng->seed(time(0));
    for (uint i = 0; i < nrows; ++i) {
      for (uint j = 0; j < ncols; ++j) {
        values[i][j] = rng->get();
      }
    }
  }

  void run_test() {
    TestMatrixPtr matrix;
    if (nrows == ncols) {
      matrix.reset(new TestMatrix(nrows));
    } else {
      matrix.reset(new TestMatrix(nrows, ncols));
    }
    UNITTEST_ASSERT_EQUAL(matrix->m(), nrows);
    UNITTEST_ASSERT_EQUAL(matrix->n(), ncols);

    // Initialized values:
    for (uint i = 0; i < nrows; ++i) {
      for (uint j = 0; j < ncols; ++j) {
        UNITTEST_ASSERT_EQUAL(matrix->at(i, j), 0);
      }
    }

    // Set new values.
    for (uint i = 0; i < nrows; ++i) {
      for (uint j = 0; j < ncols; ++j) {
        matrix->at(i, j) = values[i][j];
      }
    }

    // Sequentially:
    for (uint i = 0; i < nrows; ++i) {
      for (uint j = 0; j < ncols; ++j) {
        UNITTEST_ASSERT_EQUAL(matrix->at(i, j), values[i][j]);
      }
    }

    // Randomly:
    auto rng = unittest::make_random_value<uint>(0, nelements - 1);
    rng->seed(time(0));
    for (uint itrial = 0; itrial < 10*nelements; ++itrial) {
      uint next = rng->get();
      uint i = next / ncols;
      uint j = next % ncols;
      UNITTEST_ASSERT_EQUAL(matrix->at(i, j), values[i][j]);
    }
  }
};

using RaggedArrayFixture = TestArrayFixture<100, 29>;

TEST_FIXTURE(RaggedArrayFixture, create_and_access_matrix) {
  UNITTEST_TESTINFO("Create a matrix and check the element values");
  run_test();
}

using SquareArrayFixture = TestArrayFixture<50, 50>;

TEST_FIXTURE(SquareArrayFixture, create_and_access_square_matrix) {
  UNITTEST_TESTINFO("Create a square matrix and check the element values");
  run_test();
}

TEST(out_of_range_error) {
  constexpr uint NROWS = 10;
  constexpr uint NCOLS = 29;

  UNITTEST_TESTINFO("Check for an expected out-of-range error");
  TestMatrix matrix(NROWS, NCOLS);

  // Good:
  UNITTEST_ASSERT_NO_THROW([&matrix](){ return matrix.at(NROWS/2, NCOLS/2); });

  // Bad row:
  UNITTEST_ASSERT_THROW(std::out_of_range, [&matrix](){ return matrix.at(NROWS, NCOLS/2); });

  // Bad column:
  UNITTEST_ASSERT_THROW(std::out_of_range, [&matrix](){ return matrix.at(0, -1); });

  // Both bad:
  UNITTEST_ASSERT_THROW(std::out_of_range, [&matrix](){ return matrix.at(-10, 2*NCOLS); });
}
