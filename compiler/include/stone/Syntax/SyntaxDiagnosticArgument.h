#ifndef STONE_SYNTAX_SYNTAXDIAGARGUMENT_H
#define STONE_SYNTAX_SYNTAXDIAGARGUMENT_H

#include "stone/Diag/DiagnosticArgument.h"
#include "stone/Diag/DiagnosticEngine.h"
#include "stone/Diag/TextDiagnosticEmitter.h"
#include "stone/Diag/TextDiagnosticFormatter.h"
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

public:
  static bool classof(const SyntaxArgument *a) {
    return a->GetSyntaxArgumentKind() == SyntaxArgumentKind::Decl;
  }
};

struct DeclContext final : public SyntaxArgument {
  const syn::DeclContext *val;
  DeclContext() = delete;
  explicit DeclContext(const syn::DeclContext *val)
      : SyntaxArgument(SyntaxArgumentKind::DeclContext), val(val) {}

public:
  static bool classof(const SyntaxArgument *a) {
    return a->GetSyntaxArgumentKind() == SyntaxArgumentKind::DeclContext;
  }
};

struct Identifier final : public SyntaxArgument {
  const syn::Identifier *val;
  Identifier() = delete;
  explicit Identifier(const syn::Identifier *val)
      : SyntaxArgument(SyntaxArgumentKind::Identifier), val(val) {}

public:
  static bool classof(const SyntaxArgument *a) {
    return a->GetSyntaxArgumentKind() == SyntaxArgumentKind::Identifier;
  }
};

struct Type final : public SyntaxArgument {
  const syn::Type *val;
  Type() = delete;
  explicit Type(const syn::Type *val)
      : SyntaxArgument(SyntaxArgumentKind::Type), val(val) {}

public:
  static bool classof(const SyntaxArgument *a) {
    return a->GetSyntaxArgumentKind() == SyntaxArgumentKind::Type;
  }
};

} // namespace diag

class SyntaxDiagnostic : public Diagnostic {
public:
  explicit SyntaxDiagnostic(DiagID diagID, llvm::ArrayRef<diag::Argument> args)
      : Diagnostic(diagID, args) {}
  ~SyntaxDiagnostic();

public:
};


class SyntaxDiagnosticFormatter : public TextDiagnosticFormatter {
public:
  SyntaxDiagnosticFormatter();

public:
  void
  Format(ColorStream &out, const Diagnostic &diagnostic,
         DiagnosticFormatOptions fmtOpts = DiagnosticFormatOptions()) override;

  void
  Format(ColorStream &out, llvm::StringRef text,
         llvm::ArrayRef<diag::Argument> args,
         DiagnosticFormatOptions fmtOpts = DiagnosticFormatOptions()) override;

  void FormatArgument(ColorStream &out, llvm::StringRef modifier,
                      llvm::StringRef modifierArguments,
                      ArrayRef<diag::Argument> args, unsigned argIndex,
                      DiagnosticFormatOptions fmtOpts) override;
};

class SyntaxDiagnosticEmitter : public TextDiagnosticEmitter {
public:
  SyntaxDiagnosticEmitter(SyntaxDiagnosticFormatter &formatter);

public:
  void EmitDeclLoc();
};

} // namespace stone

#endif
