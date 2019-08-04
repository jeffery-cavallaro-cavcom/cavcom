#ifndef CAVCOM_UTILITY_LIBUTIL_MATRIX_BASE_H_
#define CAVCOM_UTILITY_LIBUTIL_MATRIX_BASE_H_

#include <stdexcept>

#include "types.h"

namespace cavcom {
  namespace utility {

    // This abstract base class defines the interface to a two-dimensional array (matrix).
    template <typename T> class MatrixBase {
     public:
      // Returns the number of row (m) or columns (n) in the matrix.
      virtual uint m(void) const = 0;
      virtual uint n(void) const = 0;

      // Returns a reference to the cell at row i and column j, with range checking.  Must throw an out-of-range
      // exception if the requested row or column is out of range.
      T &at(uint i, uint j) {
        check(i, j);
        return element(i, j);
      }

      const T &at(uint i, uint j) const {
        check(i, j);
        return element(i, j);
      }

     protected:
      // Returns a reference to the cell at row i and column j, without range checking.
      virtual T &element(uint i, uint j) = 0;
      virtual const T &element(uint i, uint j) const = 0;

     private:
      // Checks for out-of-range.
      void check(uint i, uint j) const {
        if ((i >= m()) || (j >= n())) throw std::out_of_range("bad matrix dimension");
      }
    };

  }  // namespace utility
}  // namespace cavcom

#endif  // CAVCOM_UTILITY_LIBUTIL_MATRIX_BASE_H_
