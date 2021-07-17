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

enum class SyntaxDiagnosticArgumentType { Decl, DeclContext, Type, Identifier };

struct SyntaxDiagnosticArgument : public ComplexDiagnosticArgument {
  SyntaxDiagnosticArgumentType synTy;

public:
  SyntaxDiagnosticArgument(SyntaxDiagnosticArgumentType synType,
                           const void *val)
      : ComplexDiagnosticArgument(val), synTy(synTy) {}
};

struct DeclDiagnosticArgument final : public SyntaxDiagnosticArgument {
public:
  DeclDiagnosticArgument(const syn::Decl *val)
      : SyntaxDiagnosticArgument(SyntaxDiagnosticArgumentType::Decl, val) {}
};

struct DeclContextDiagnosticArgument final : public SyntaxDiagnosticArgument {
public:
  DeclContextDiagnosticArgument(const syn::DeclContext *val)
      : SyntaxDiagnosticArgument(SyntaxDiagnosticArgumentType::DeclContext,
                                 val) {}
};

struct IdentifierDiagnosticArgument final : public SyntaxDiagnosticArgument {
public:
  IdentifierDiagnosticArgument(const syn::Identifier *val)
      : SyntaxDiagnosticArgument(SyntaxDiagnosticArgumentType::Identifier,
                                 val) {}
};

struct TypeDiagnosticArgument final : public SyntaxDiagnosticArgument {
public:
  TypeDiagnosticArgument(const syn::Type *val)
      : SyntaxDiagnosticArgument(SyntaxDiagnosticArgumentType::Type, val) {}
};

class SyntaxDiagnostic final : public Diagnostic {
public:
  explicit SyntaxDiagnostic(const DiagID diagID, const DiagnosticEngine *de)
      : Diagnostic(diagID, de) {}

public:
  void Format(llvm::SmallVectorImpl<char> &outStr,
              const DiagnosticFormatOptions &fmtOptions) const override;

  void Format(const char *diagStr, const char *diagEnd,
              llvm::SmallVectorImpl<char> &outStr,
              const DiagnosticFormatOptions &fmtOptions) const override;
};

} // namespace stone

#endif
