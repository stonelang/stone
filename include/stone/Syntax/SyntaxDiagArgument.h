#ifndef STONE_SYNTAX_SYNTAXDIAGARGUMENT_H
#define STONE_SYNTAX_SYNTAXDIAGARGUMENT_H

#include "stone/Basic/DiagnosticArgument.h"
#include "stone/Basic/DiagnosticEngine.h"
#include "stone/Syntax/Decl.h"
#include "stone/Syntax/Identifier.h"

using stone::Diagnostic;

#include <assert.h>

namespace stone {

class DiagnosticEngine;

namespace diag {
enum class SyntaxArgumentType { Decl, DeclContext, Type, Identifier };

struct SyntaxArgument : public ComplexArgument {
  SyntaxArgumentType synTy;

public:
  SyntaxArgument(SyntaxArgumentType synType, const void *val)
      : ComplexArgument(val), synTy(synTy) {}
};

struct DeclArgument final : public SyntaxArgument {
public:
  DeclArgument(const syn::Decl *val)
      : SyntaxArgument(SyntaxArgumentType::Decl, val) {}
};

struct DeclContextArgument final : public SyntaxArgument {
public:
  DeclContextArgument(const syn::DeclContext *val)
      : SyntaxArgument(SyntaxArgumentType::DeclContext, val) {}
};

struct IdentifierArgument final : public SyntaxArgument {
public:
  IdentifierArgument(const syn::Identifier *val)
      : SyntaxArgument(SyntaxArgumentType::Identifier, val) {}
};

struct TypeArgument final : public SyntaxArgument {
public:
  TypeArgument(const syn::Type *val)
      : SyntaxArgument(SyntaxArgumentType::Type, val) {}
};

} // namespace diag
class SyntaxDiagnostic final : public Diagnostic {
public:
  explicit SyntaxDiagnostic(DiagnosticContext context) : Diagnostic(context) {}

public:
  void Format(llvm::SmallVectorImpl<char> &outStr,
              const DiagnosticFormatOptions &fmtOptions) const override;

  void Format(const char *diagStr, const char *diagEnd,
              llvm::SmallVectorImpl<char> &outStr,
              const DiagnosticFormatOptions &fmtOptions) const override;
};

} // namespace stone

#endif
