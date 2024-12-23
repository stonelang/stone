#ifndef STONE_DIAG_DIAGNOSTICBASIC_H
#define STONE_DIAG_DIAGNOSTICBASIC_H

#include "stone/Basic/LLVM.h"
#include "stone/Diag/DiagnosticID.h"

namespace stone {
template <typename... ArgTypes> struct Diag;
namespace detail {
// These templates are used to help extract the type arguments of the
// DIAG/ERROR/WARNING/NOTE/REMARK/FIXIT macros.
template <typename T> struct DiagWithArguments;
template <typename... ArgTypes> struct DiagWithArguments<void(ArgTypes...)> {
  typedef Diag<ArgTypes...> type;
};
} // end namespace detail
namespace diag {
// Declare common diagnostics objects with their appropriate types.
#define DIAG(KIND, ID, Options, Text, Signature)                               \
  extern detail::DiagWithArguments<void Signature>::type ID;
#include "DiagnosticBasicKind.def"

} // namespace diag
} // end namespace stone

#endif
