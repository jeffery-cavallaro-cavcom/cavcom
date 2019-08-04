// Tests the 2-D array Matrix class.

#include <ctime>

#include <libunittest/all.hpp>

#include "matrix.h"

using TestMatrix = cavcom::utility::Matrix<int>;

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
    TestMatrix matrix(nrows, ncols);
    UNITTEST_ASSERT_EQUAL(matrix.m(), nrows);
    UNITTEST_ASSERT_EQUAL(matrix.n(), ncols);

    // Initialized values:
    for (uint i = 0; i < nrows; ++i) {
      for (uint j = 0; j < ncols; ++j) {
        UNITTEST_ASSERT_EQUAL(matrix.at(i, j), 0);
      }
    }

    // Set new values.
    for (uint i = 0; i < nrows; ++i) {
      for (uint j = 0; j < ncols; ++j) {
        matrix.at(i, j) = values[i][j];
      }
    }

    // Sequentially:
    for (uint i = 0; i < nrows; ++i) {
      for (uint j = 0; j < ncols; ++j) {
        UNITTEST_ASSERT_EQUAL(matrix.at(i, j), values[i][j]);
      }
    }

    // Randomly:
    auto rng = unittest::make_random_value<uint>(0, nelements - 1);
    rng->seed(time(0));
    for (uint itrial = 0; itrial < 10*nelements; ++itrial) {
      uint next = rng->get();
      uint i = next / ncols;
      uint j = next % ncols;
      UNITTEST_ASSERT_EQUAL(matrix.at(i, j), values[i][j]);
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

// A functor that is used to force an out-of-range exception for invalid row and/or column indices.
class BadCaller {
 public:
  BadCaller(const TestMatrix &matrix, uint i, uint j) : matrix_(matrix), i_(i), j_(j) {}
  int operator()() { return matrix_.at(i_, j_); }

 private:
  const TestMatrix &matrix_;
  uint i_;
  uint j_;
};

TEST(out_of_range_error) {
  constexpr uint NROWS = 10;
  constexpr uint NCOLS = 29;

  UNITTEST_TESTINFO("Check for an expected out-of-range error");
  TestMatrix matrix(NROWS, NCOLS);

  // Good:
  BadCaller good(matrix, NROWS/2, NCOLS/2);
  UNITTEST_ASSERT_NO_THROW(good);

  // Bad row:
  BadCaller badrow(matrix, NROWS, NCOLS/2);
  UNITTEST_ASSERT_THROW(std::out_of_range, badrow);

  // Bad column:
  BadCaller badcol(matrix, 0, -1);
  UNITTEST_ASSERT_THROW(std::out_of_range, badcol);

  // Both bad:
  BadCaller badrowcol(matrix, -10, 2*NCOLS);
  UNITTEST_ASSERT_THROW(std::out_of_range, badrowcol);
}
