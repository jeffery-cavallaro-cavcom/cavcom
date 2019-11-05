#include "errors.h"

namespace cavcom {
  namespace graph {

    MultipleEdgeError::MultipleEdgeError(VertexNumber from, VertexNumber to, EdgeNumber edge)
      : std::runtime_error("Attempted multiple edge"), from_(from), to_(to), edge_(edge) {
      std::ostringstream formatter;
      formatter << std::runtime_error::what();
      formatter << " from vertex ";
      formatter << from_;
      formatter << " to vertex ";
      formatter << to_;
      formatter << " with edge ";
      formatter << edge_;
      msg_ = formatter.str();
    }

    LoopEdgeError::LoopEdgeError(VertexNumber vertex, EdgeNumber edge)
      : std::runtime_error("Attempted loop edge"), vertex_(vertex), edge_(edge) {
      std::ostringstream formatter;
      formatter << std::runtime_error::what();
      formatter << " on vertex ";
      formatter << vertex_;
      formatter << " with edge ";
      formatter << edge_;
      msg_ = formatter.str();
    }

    SameVertexContractError::SameVertexContractError(VertexNumber vertex)
      : std::runtime_error("Cannot contract vertex"), vertex_(vertex) {
      std::ostringstream formatter;
      formatter << std::runtime_error::what();
      formatter << " ";
      formatter << vertex_;
      formatter << " with itself";
      msg_ = formatter.str();
    }

  }  // namespace graph
}  // namespace cavcom
