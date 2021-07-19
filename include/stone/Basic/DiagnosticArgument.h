#ifndef STONE_BASIC_DIAGNOSTICARGUMENT_H
#define STONE_BASIC_DIAGNOSTICARGUMENT_H

#include "stone/Basic/TokenType.h"
#include "llvm/ADT/StringRef.h"

#include <assert.h>

namespace stone {

/// Enumeration describing all of possible diagnostics.
///
/// Each of the diagnostics described in DiagnosticEngine.def has an entry in
/// this enumeration type that uniquely identifies it.
enum class DiagID : uint32_t;
enum class FixID : uint32_t;

/// Describes a diagnostic along with its argument types.
///
/// The diagnostics header introduces instances of this type for each
/// diagnostic, which provide both the set of argument types (used to
/// check/convert the arguments at each call site) and the diagnostic ID
/// (for other information about the diagnostic).
template <typename... argTypes> struct Diag {
  /// The diagnostic ID corresponding to this diagnostic.
  DiagID diagID;
};

namespace diag {
enum class ArgumentType {
  /// No argument
  None,
  /// std::string
  STDStr,

  /// const char *
  CStr,

  /// llvm::StringRef
  LLVMStr,

  /// int
  SInt,

  /// unsigned
  UInt,

  TokenType,

  /// custom argument
  Complex,

};

struct Argument {
  ArgumentType ty;
  friend class DiagnosticContext;
  Argument() {}
  int place;

public:
  Argument(ArgumentType ty) : ty(ty) {}

public:
  ArgumentType GetType() { return ty; }
  int GetPlace() { return place; }
};

struct STDStrArgument final : public Argument {
  const std::string val;

public:
  STDStrArgument(const std::string val)
      : Argument(ArgumentType::STDStr), val(val) {}

public:
  std::string GetVal() const { return val; }
};

struct LLVMStrArgument final : public Argument {
  const std::string val;

public:
  LLVMStrArgument(const llvm::StringRef val)
      : Argument(ArgumentType::LLVMStr), val(val) {}

public:
  std::string GetVal() const { return val; }
};

struct CStrArgument final : public Argument {
  const char *val;

public:
  CStrArgument(const char *val) : Argument(ArgumentType::CStr), val(val) {}

public:
  const char *GetVal() const { return val; }
};

struct SIntArgument final : public Argument {
  const int val;

public:
  SIntArgument(const int val) : Argument(ArgumentType::SInt), val(val) {}

public:
  int GetVal() const { return val; }
};

struct UIntArgument final : public Argument {
  unsigned val;

public:
  UIntArgument(const unsigned val) : Argument(ArgumentType::UInt), val(val) {}

public:
  unsigned GetVal() const { return val; }
};

struct TokenTypeArgument final : public Argument {
  tk::Type val;

public:
  TokenTypeArgument(const tk::Type val)
      : Argument(ArgumentType::TokenType), val(val) {}

public:
  tk::Type GetVal() const { return val; }
};

struct ComplexArgument : public Argument {
  const void *val;

public:
  ComplexArgument(const void *val) : Argument(ArgumentType::Complex) {}

  const void *GetVal() const { return val; }
};

} // namespace diag
} // namespace stone

#endif