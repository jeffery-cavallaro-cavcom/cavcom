#ifndef CAVCOM_UTILITY_LIBUTIL_MATRIX_H_
#define CAVCOM_UTILITY_LIBUTIL_MATRIX_H_

#include <memory>

namespace cavcom {
  namespace utility {

    // A fully-allocated (as opposed to a sparse) matrix, implemented on top of a one-dimensional matrix,
    template <typename T> class Matrix {
     public:
      // Creates a new m x n matrix with default element initialization.
      Matrix(uint m, uint n) : m_(m), n_(n), elements_(new T[m_*n_]()) {}

      // Creates a new n x n square matrix with default element initialization.
      explicit Matrix(uint n) : Matrix(n, n) {}

      // Returns the number of rows (m) or columns (n) in the matrix.
      uint m(void) const { return m_; }
      uint n(void) const { return n_; }

      // Returns a reference to the cell at row i and column j, with range checking.
      T &at(uint i, uint j) {
        check(i, j);
        return element(i, j);
      }

      const T &at(uint i, uint j) const {
        check(i, j);
        return element(i, j);
      }

     private:
      uint m_;  // rows
      uint n_;  // columns
      std::unique_ptr<T[]> elements_;

      // Checks for a bad element access.
      void check(uint i, uint j) const {
        if ((i >= m_) || (j >= n_)) throw std::out_of_range("bad matrix dimension");
      }

      // Returns a reference to the cell at row i and column j, without range checking.
      T &element(uint i, uint j) { return elements_[n_*i + j]; }
      const T &element(uint i, uint j) const { return elements_[n_*i + j]; }
    };

  }  // namespace utility
}  // namespace cavcom

#endif  // CAVCOM_UTILITY_LIBUTIL_MATRIX_H_
