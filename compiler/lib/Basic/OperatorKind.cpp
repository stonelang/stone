#include "stone/Basic/OperatorKind.h"

using namespace stone;

/// Retrieve the spelling of the given overloaded operator, without
/// the preceding "operator" keyword.
const char *opr::GetOperatorSpelling(OverloadedOperatorKind kind) { return ""; }