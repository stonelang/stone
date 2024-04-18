#ifndef STONE_BASIC_OPERATORKIND_H
#define STONE_BASIC_OPERATORKIND_H

namespace stone {
namespace opr {
/// Enumeration specifying the different kinds of C++ overloaded
/// operators.
enum OverloadedOperatorKind : int {
  None, ///< Not an overloaded operator
#define OVERLOADED_OPERATOR(Name, Spelling, Token, Unary, Binary, MemberOnly)  \
  Name,
#include "stone/Basic/OperatorKind.def"
  NUM_OVERLOADED_OPERATORS
};

/// Retrieve the spelling of the given overloaded operator, without
/// the preceding "operator" keyword.
const char *GetOperatorSpelling(OverloadedOperatorKind kind);

/// Get the other overloaded operator that the given operator can be rewritten
/// into, if any such operator exists.
inline OverloadedOperatorKind
GetRewrittenOverloadedOperator(OverloadedOperatorKind Kind) {
  switch (Kind) {
  case Less:
  case LessEqual:
  case Greater:
  case GreaterEqual:
    return Spaceship;
  case ExclaimEqual:
    return EqualEqual;
  default:
    return OverloadedOperatorKind::None;
  }
}
/// Determine if this is a compound assignment operator.
inline bool IsCompoundAssignmentOperator(OverloadedOperatorKind Kind) {
  return Kind >= PlusEqual && Kind <= PipeEqual;
}
} // namespace opr
} // namespace stone

#endif
