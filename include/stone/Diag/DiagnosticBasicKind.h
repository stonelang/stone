#ifndef STONE_DIAG_DIAGNOSTICBASIC_H
#define STONE_DIAG_DIAGNOSTICBASIC_H

#include "stone/Basic/LLVM.h"
#include "stone/Diag/DiagnosticID.h"

namespace stone {
namespace diags {

template <typename... ArgTypes> struct Diag;

// These templates are used to help extract the type arguments of the
// DIAG/ERROR/WARNING/NOTE/REMARK/FIXIT macros.
template <typename T> struct DiagWithArguments;

template <typename... ArgTypes> struct DiagWithArguments<void(ArgTypes...)> {
  typedef Diag<ArgTypes...> type;
};

// Declare common diagnostics objects with their appropriate types.
#define DIAG(KIND, ID, Options, Message, Signature)                            \
  extern DiagWithArguments<void Signature>::type ID;
#include "DiagnosticBasicKind.def"

} // namespace diags

} // end namespace stone

#endif
