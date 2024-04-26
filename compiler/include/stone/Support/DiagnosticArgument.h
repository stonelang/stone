#ifndef STONE_DIAG_DIAGNOSTICARGUMENT_H
#define STONE_DIAG_DIAGNOSTICARGUMENT_H

#include <assert.h>

#include "stone/Basic/TokenKind.h"
#include "llvm/ADT/StringRef.h"

namespace stone {
/// Enumeration describing all of possible diagnostics.
///
/// Each of the diagnostics described in DiagnosticEngine.def has an entry in
/// this enumeration type that uniquely identifies it.
enum class DiagID : uint32_t;
enum class FixID : uint32_t;

class Decl;
class DeclContext;
class Type;
class Token;
class Identifier;

/// Describes a diagnostic along with its argument types.
///
/// The diagnostics header introduces instances of this type for each
/// diagnostic, which provide both the set of argument types (used to
/// check/convert the arguments at each call site) and the diagnostic ID
/// (for other information about the diagnostic).
template <typename... ArgTypes> struct Diag {
  /// The diagnostic ID corresponding to this diagnostic.
  DiagID diagID;
};

template <typename... ArgTypes> struct Fix {
  /// The code fix ID corresponding to this fix.
  FixID fixID;
};

enum class DiagnosticArgumentKind : unsigned {
  /// bool
  Bool = 0,
  /// StringRef
  String,
  /// int
  Integer,
  /// unsigned
  Unsigned,
  ///
  Token,
  ///
  Decl,
  ///
  DeclContext,
  ///
  Type,
  ///
  Identifier,
};

constexpr size_t DiagnosticArgumentAlignment = 8;
class alignas(DiagnosticArgumentAlignment) DiagnosticArgument final {

  int integerValue;
  unsigned unsignedValue;
  bool boolValue;
  StringRef stringValue;
  const Token *tokenValue = nullptr;
  const Decl *declValue = nullptr;
  const Identifier *identifierValue = nullptr;
  const DeclContext *declContextValue = nullptr;
  const Type *typeValue = nullptr;

  DiagnosticArgumentKind kind;

public:
  DiagnosticArgument() = delete;
  ~DiagnosticArgument() = delete;

public:
  DiagnosticArgument(int val)
      : integerValue(val), kind(DiagnosticArgumentKind::Integer) {}
  int GetIntegerValue() { return integerValue; }

public:
  DiagnosticArgument(unsigned val)
      : unsignedValue(val), kind(DiagnosticArgumentKind::Unsigned) {}
  unsigned GetUnsignedValue() { return unsignedValue; }

public:
  DiagnosticArgument(bool val)
      : boolValue(val), kind(DiagnosticArgumentKind::Bool) {}
  bool GetBoolValue() { return boolValue; }

public:
  DiagnosticArgument(StringRef val)
      : stringValue(val), kind(DiagnosticArgumentKind::String) {}
  StringRef GetstringValue() { return stringValue; }

public:
  DiagnosticArgument(const Token *val)
      : tokenValue(val), kind(DiagnosticArgumentKind::Token) {}
  const Token *GetTokenValue() { return tokenValue; }

public:
  DiagnosticArgument(const Decl *val)
      : declValue(val), kind(DiagnosticArgumentKind::Decl) {}
  const Decl *GetDeclValue() { return declValue; }

public:
  DiagnosticArgument(const Identifier *val)
      : identifierValue(val), kind(DiagnosticArgumentKind::Identifier) {}
  const Identifier *GetIdentifierValue() { return identifierValue; }

public:
  DiagnosticArgument(const DeclContext *val)
      : declContextValue(val), kind(DiagnosticArgumentKind::DeclContext) {}
  const DeclContext *GetDeclContext() { return declContextValue; }

public:
  DiagnosticArgument(const Type *val)
      : typeValue(val), kind(DiagnosticArgumentKind::Type) {}
  const Type *GetTypeValue() { return typeValue; }

public:
  DiagnosticArgumentKind GetKind() { return kind; }
};

namespace diag {
enum class ArgumentKind : unsigned {
  /// No argument
  None = 0,
  /// bool
  Bool,
  /// std::string
  STDStr,
  /// const char *
  CStr,
  /// llvm::StringRef
  LLVMStr,
  /// int
  Int,
  /// unsigned
  UInt,
  ///
  Tok,
  /// custom AST argument
  AST,
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

struct Bool final : public Argument {
private:
  const bool val;

public:
  Bool() = delete;
  explicit Bool(bool val) : Argument(ArgumentKind::Bool), val(val) {}
};

struct STDStr final : public Argument {
private:
  const std::string val;

public:
  STDStr() = delete;

  explicit STDStr(const std::string val)
      : Argument(ArgumentKind::STDStr), val(val) {}

public:
  std::string GetVal() const { return val; }
};

struct LLVMStr final : public Argument {
private:
  const llvm::StringRef val;

public:
  LLVMStr() = delete;

  explicit LLVMStr(const llvm::StringRef val)
      : Argument(ArgumentKind::LLVMStr), val(val) {}

public:
  llvm::StringRef GetVal() const { return val; }
};

struct CStr final : public Argument {
  const char *val;

public:
  CStr() = delete;

  explicit CStr(const char *val) : Argument(ArgumentKind::CStr), val(val) {}
  const char *GetVal() const { return val; }
};

struct Int final : public Argument {
  const int val;

public:
  Int() = delete;

  explicit Int(const int val) : Argument(ArgumentKind::Int), val(val) {}

  int GetVal() const { return val; }
};

struct UInt final : public Argument {
  unsigned val;

public:
  UInt() = delete;
  explicit UInt(const unsigned val) : Argument(ArgumentKind::UInt), val(val) {}
  unsigned GetVal() const { return val; }
};

struct Tok final : public Argument {
  tok val;

public:
  Tok() = delete;

  explicit Tok(const tok val) : Argument(ArgumentKind::Tok), val(val) {}
  tok GetVal() const { return val; }
};

enum class ASTArgumentKind { None, Decl, DeclContext, Type, Identifier, Token };

struct ASTArgument : public Argument {
  ASTArgumentKind kind = ASTArgumentKind::None;

public:
  ASTArgument() = delete;
  explicit ASTArgument(ASTArgumentKind kind)
      : Argument(ArgumentKind::AST), kind(kind) {}
  ASTArgumentKind GetASTArgumentKind() const { return kind; }
};

} // namespace diag

} // namespace stone

#endif