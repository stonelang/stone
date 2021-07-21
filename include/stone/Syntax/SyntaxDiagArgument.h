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

struct DeclArgument final : public SyntaxArgument {
  syn::Decl *val;
  DeclArgument() = delete;
  explicit DeclArgument(const syn::Decl *val)
      : SyntaxArgument(SyntaxArgumentKind::Decl), val(val) {}
};

struct DeclContextArgument final : public SyntaxArgument {
  const syn::DeclContext *val;
  DeclContextArgument() = delete;
  explicit DeclContextArgument(const syn::DeclContext *val)
      : SyntaxArgument(SyntaxArgumentKind::DeclContext), (val) {}
};

struct IdentifierArgument final : public SyntaxArgument {

  const syn::Identifier *val;
  IdentifierArgument() = delete;
  explicit IdentifierArgument(const syn::Identifier *val)
      : SyntaxArgument(SyntaxArgumentKind::Identifier), (val) {}
};

struct TypeArgument final : public SyntaxArgument {
  const syn::Type *val;
  TypeArgument() = delete;
  explicit TypeArgument(const syn::Type *val)
      : SyntaxArgument(SyntaxArgumentKind::Type), (val) {}
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
