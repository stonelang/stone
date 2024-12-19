#ifndef STONE_DIAGNOSTICSCOMMON_H
#define STONE_DIAGNOSTICSCOMMON_H

#include "stone/AST/DiagnosticID.h"
#include "stone/Basic/LLVM.h"
// #include "stone/Config.h"

namespace stone {
namespace diags {

template <typename... ArgTypes> struct Diag;

namespace detail {
// These templates are used to help extract the type arguments of the
// DIAG/ERROR/WARNING/NOTE/REMARK/FIXIT macros.
template <typename T> struct DiagWithArguments;
template <typename... ArgTypes> struct DiagWithArguments<void(ArgTypes...)> {
  typedef Diag<ArgTypes...> type;
};

} // end namespace detail

// Declare common diagnostics objects with their appropriate types.
#define DIAG(ENUM, FLAGS, DEFAULT_MAPPING, DESC, GROUP, SFINAE, NOWERROR,      \
             SHOWINSYSHEADER, SHOWINSYSMACRO, DEFERRABLE, CATEGORY)            \
  extern detail::DiagWithArguments<void Signature>::type ID;
#include "BasicDiagnostics.inc"

} // namespace diags

} // end namespace stone

#endif
