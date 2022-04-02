#ifndef STONE_SYNTAX_SYNTAXDIAGARGUMENT_H
#define STONE_SYNTAX_SYNTAXDIAGARGUMENT_H

#include "stone/Core/DiagnosticArgument.h"
#include "stone/Core/DiagnosticEngine.h"
#include "stone/Core/TextDiagnosticFormatter.h"
#include "stone/Syntax/Decl.h"
#include "stone/Syntax/Identifier.h"

using stone::Diagnostic;

#include <assert.h>

namespace stone {

namespace diag {

// struct TokenArgument final : public SyntaxArgument {
//   const syn::Token *val;
//   Decl() = delete;
//   explicit TokenArgument(const syn::Token *val)
//       : SyntaxArgument(SyntaxArgumentKind::Token), val(val) {}
// };

struct Decl final : public SyntaxArgument {
  const syn::Decl *val;
  Decl() = delete;
  explicit Decl(const syn::Decl *val)
      : SyntaxArgument(SyntaxArgumentKind::Decl), val(val) {}
};

struct DeclContext final : public SyntaxArgument {
  const syn::DeclContext *val;
  DeclContext() = delete;
  explicit DeclContext(const syn::DeclContext *val)
      : SyntaxArgument(SyntaxArgumentKind::DeclContext), val(val) {}
};

struct Identifier final : public SyntaxArgument {
  const syn::Identifier *val;
  Identifier() = delete;
  explicit Identifier(const syn::Identifier *val)
      : SyntaxArgument(SyntaxArgumentKind::Identifier), val(val) {}
};

struct Type final : public SyntaxArgument {
  const syn::Type *val;
  Type() = delete;
  explicit Type(const syn::Type *val)
      : SyntaxArgument(SyntaxArgumentKind::Type), val(val) {}
};

} // namespace diag

class SyntaxDiagnostic : public Diagnostic {
public:
  explicit SyntaxDiagnostic(DiagnosticDetail detail) : Diagnostic(detail) {}
  ~SyntaxDiagnostic();

public:
};

class SyntaxDiagnosticFormatter : public TextDiagnosticFormatter {
public:
  SyntaxDiagnosticFormatter();

public:
  void
  Format(llvm::raw_ostream &out, DiagnosticDetail &detail,
         DiagnosticFormatOptions fmtOpts = DiagnosticFormatOptions()) override;

  void
  Format(llvm::raw_ostream &out, llvm::StringRef text,
         llvm::ArrayRef<diag::Argument> args,
         DiagnosticFormatOptions fmtOpts = DiagnosticFormatOptions()) override;
};

} // namespace stone

#endif
