#ifndef CAVCOM_UTILITY_LIBUTIL_MATRIX_H_
#define CAVCOM_UTILITY_LIBUTIL_MATRIX_H_

#include <memory>

#include "matrix_base.h"

namespace cavcom {
  namespace utility {

    // A fully-allocated (as opposed to a sparse) matrix, implemented on top of a one-dimensional matrix,
    template <typename T> class Matrix : public MatrixBase<T> {
     public:
      // Creates a new matrix with m rows and n columns with element initialization.
      Matrix(uint m, uint n) : m_(m), n_(n), elements_(new T[m_*n_]()) {}

      // Returns the number of row (m) or columns (n) in the matrix.
      uint m(void) const { return m_; }
      uint n(void) const { return n_; }

     protected:
      // Returns a reference to the cell at row i and column j, without range checking.
      T &element(uint i, uint j) { return elements_[n_*i + j]; }
      const T &element(uint i, uint j) const { return elements_[n_*i + j]; }

     private:
      uint m_;  // rows
      uint n_;  // columns
      std::unique_ptr<T[]> elements_;
    };

  }  // namespace utility
}  // namespace cavcom

#endif  // CAVCOM_UTILITY_LIBUTIL_MATRIX_H_
