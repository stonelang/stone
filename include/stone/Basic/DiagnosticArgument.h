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

  Bool,
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

enum { ArgumentAlignment = 8 };

struct alignas(ArgumentAlignment) Argument {
private:
  ArgumentType ty;

public:
  Argument() = delete;
  explicit Argument(ArgumentType ty) : ty(ty) {}
  ArgumentType GetType() { return ty; }
};

struct BoolArgument final : public Argument {
private:
  const bool val;

public:
  BoolArgument() = delete;
  explicit BoolArgument(bool val) : Argument(ArgumentType::Bool), val(val) {}
};

struct STDStrArgument final : public Argument {
private:
  const std::string val;

public:
  STDStrArgument() = delete;

  explicit STDStrArgument(const std::string val)
      : Argument(ArgumentType::STDStr), val(val) {}

public:
  std::string GetVal() const { return val; }
};

struct LLVMStrArgument final : public Argument {

private:
  const std::string val;

public:
  LLVMStrArgument() = delete;

  explicit LLVMStrArgument(const llvm::StringRef val)
      : Argument(ArgumentType::LLVMStr), val(val) {}

public:
  std::string GetVal() const { return val; }
};

struct CStrArgument final : public Argument {
  const char *val;

public:
  CStrArgument() = delete;

  explicit CStrArgument(const char *val)
      : Argument(ArgumentType::CStr), val(val) {}
  const char *GetVal() const { return val; }
};

struct SIntArgument final : public Argument {
  const int val;

public:
  SIntArgument() = delete;

  explicit SIntArgument(const int val)
      : Argument(ArgumentType::SInt), val(val) {}

  int GetVal() const { return val; }
};

struct UIntArgument final : public Argument {
  unsigned val;

public:
  UIntArgument() = delete;
  explicit UIntArgument(const unsigned val)
      : Argument(ArgumentType::UInt), val(val) {}
  unsigned GetVal() const { return val; }
};

struct TokenTypeArgument final : public Argument {
  tk::Type val;

public:
  TokenTypeArgument() = delete;

  explicit TokenTypeArgument(const tk::Type val)
      : Argument(ArgumentType::TokenType), val(val) {}
  tk::Type GetVal() const { return val; }
};

// TODO: Think about -- it may be better to have ComplexArgumentType { Decl,
// Type, ...}
// here since they are not the actual types.
struct ComplexArgument : public Argument {
  const void *val;

public:
  ComplexArgument() = delete;
  explicit ComplexArgument(const void *val) : Argument(ArgumentType::Complex) {}
  const void *GetVal() const { return val; }
};
} // namespace diag

} // namespace stone

#endif