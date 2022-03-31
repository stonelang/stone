#ifndef STONE_CORE_DIAGNOSTICARGUMENT_H
#define STONE_CORE_DIAGNOSTICARGUMENT_H

#include <assert.h>

#include "stone/Core/TokenKind.h"
#include "llvm/ADT/StringRef.h"

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

template <typename... ArgTypes> struct Fix {
  /// The code fix ID corresponding to this fix.
  FixID ID;
};

namespace diag {
enum class ArgumentKind {
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

  TokenKind,

  /// custom argument
  Syntax,

};

enum { ArgumentAlignment = 8 };

struct alignas(ArgumentAlignment) Argument {
private:
  ArgumentKind kind = ArgumentKind::None;

public:
  Argument() = delete;
  explicit Argument(ArgumentKind kind) : kind(kind) {}
  ArgumentKind GetKind() { return kind; }
};

struct BoolArgument final : public Argument {
private:
  const bool val;

public:
  BoolArgument() = delete;
  explicit BoolArgument(bool val) : Argument(ArgumentKind::Bool), val(val) {}
};

struct STDStrArgument final : public Argument {
private:
  const std::string val;

public:
  STDStrArgument() = delete;

  explicit STDStrArgument(const std::string val)
      : Argument(ArgumentKind::STDStr), val(val) {}

public:
  std::string GetVal() const { return val; }
};

struct LLVMStrArgument final : public Argument {
private:
  const std::string val;

public:
  LLVMStrArgument() = delete;

  explicit LLVMStrArgument(const llvm::StringRef val)
      : Argument(ArgumentKind::LLVMStr), val(val) {}

public:
  std::string GetVal() const { return val; }
};

struct CStrArgument final : public Argument {
  const char *val;

public:
  CStrArgument() = delete;

  explicit CStrArgument(const char *val)
      : Argument(ArgumentKind::CStr), val(val) {}
  const char *GetVal() const { return val; }
};

struct SIntArgument final : public Argument {
  const int val;

public:
  SIntArgument() = delete;

  explicit SIntArgument(const int val)
      : Argument(ArgumentKind::SInt), val(val) {}

  int GetVal() const { return val; }
};

struct UIntArgument final : public Argument {
  unsigned val;

public:
  UIntArgument() = delete;
  explicit UIntArgument(const unsigned val)
      : Argument(ArgumentKind::UInt), val(val) {}
  unsigned GetVal() const { return val; }
};

struct TokenKindArgument final : public Argument {
  tok val;

public:
  TokenKindArgument() = delete;

  explicit TokenKindArgument(const tok val)
      : Argument(ArgumentKind::TokenKind), val(val) {}
  tok GetVal() const { return val; }
};

enum class SyntaxArgumentKind {
  None,
  Decl,
  DeclContext,
  Type,
  Identifier,
  Token
};
struct SyntaxArgument : public Argument {
  SyntaxArgumentKind kind = SyntaxArgumentKind::None;

public:
  SyntaxArgument() = delete;
  explicit SyntaxArgument(SyntaxArgumentKind kind)
      : Argument(ArgumentKind::Syntax), kind(kind) {}
  SyntaxArgumentKind GetSyntaxArgumentKind() { return kind; }
};

} // namespace diag

} // namespace stone

#endif