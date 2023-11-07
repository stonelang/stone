#ifndef STONE_DIAG_BASICDIAGNOSTIC_H
#define STONE_DIAG_BASICDIAGNOSTIC_H

#include "stone/Basic/LLVM.h"
#include "stone/Basic/DiagnosticArgument.h"

namespace stone {
template <typename... argTypes> struct Diag;
template <typename... argTypes> struct Fix;

namespace detail {
// These templates are used to help extract the type arguments of the
// DIAG/ERROR/WARNING/NOTE/REMARK/FIXIT macros.

template <typename T> struct DiagWithArguments;
template <typename... argTypes> struct DiagWithArguments<void(argTypes...)> {
  typedef Diag<argTypes...> type;
};

template <typename T> struct FixWithArguments;
template <typename... ArgTypes> struct FixWithArguments<void(ArgTypes...)> {
  typedef Fix<ArgTypes...> type;
};

} // end namespace detail

enum class StaticSpellingKind : uint8_t;

namespace diag {
enum class RequirementKind : uint8_t;
// Declare common diagnostics objects with their appropriate types.
#define DIAG(KIND, ID, Options, Text, Signature)                               \
  extern detail::DiagWithArguments<void Signature>::type ID;
#include "CoreDiagnostic.def"

} // end namespace diag
} // end namespace stone

#endif
