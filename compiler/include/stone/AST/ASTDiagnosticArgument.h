#ifndef STONE_ASTDIAGARGUMENT_H
#define STONE_ASTDIAGARGUMENT_H

#include "stone/AST/Decl.h"
#include "stone/AST/Identifier.h"
#include "stone/Diag/DiagnosticArgument.h"
#include "stone/Diag/DiagnosticEngine.h"
#include "stone/Diag/TextDiagnosticEmitter.h"
#include "stone/Diag/TextDiagnosticFormatter.h"

using stone::Diagnostic;

#include <assert.h>

namespace stone {

namespace diag {

// struct TokenArgument final : public ASTArgument {
//   const ast::Token *val;
//   Decl() = delete;
//   explicit TokenArgument(const ast::Token *val)
//       : ASTArgument(ASTArgumentKind::Token), val(val) {}
// };

struct Decl final : public ASTArgument {
  const ast::Decl *val;
  Decl() = delete;
  explicit Decl(const ast::Decl *val)
      : ASTArgument(ASTArgumentKind::Decl), val(val) {}

public:
  static bool classof(const ASTArgument *a) {
    return a->GetASTArgumentKind() == ASTArgumentKind::Decl;
  }
};

struct DeclContext final : public ASTArgument {
  const ast::DeclContext *val;
  DeclContext() = delete;
  explicit DeclContext(const ast::DeclContext *val)
      : ASTArgument(ASTArgumentKind::DeclContext), val(val) {}

public:
  static bool classof(const ASTArgument *a) {
    return a->GetASTArgumentKind() == ASTArgumentKind::DeclContext;
  }
};

struct Identifier final : public ASTArgument {
  const ast::Identifier *val;
  Identifier() = delete;
  explicit Identifier(const ast::Identifier *val)
      : ASTArgument(ASTArgumentKind::Identifier), val(val) {}

public:
  static bool classof(const ASTArgument *a) {
    return a->GetASTArgumentKind() == ASTArgumentKind::Identifier;
  }
};

struct Type final : public ASTArgument {
  const ast::Type *val;
  Type() = delete;
  explicit Type(const ast::Type *val)
      : ASTArgument(ASTArgumentKind::Type), val(val) {}

public:
  static bool classof(const ASTArgument *a) {
    return a->GetASTArgumentKind() == ASTArgumentKind::Type;
  }
};

} // namespace diag

class ASTDiagnostic : public Diagnostic {
public:
  explicit ASTDiagnostic(Diagnostic detail) : Diagnostic(detail) {}
  ~ASTDiagnostic();

public:
};

class ASTDiagnosticFormatter : public TextDiagnosticFormatter {
public:
  ASTDiagnosticFormatter();

public:
  void
  Format(ColorStream &out, Diagnostic &detail,
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

class ASTDiagnosticEmitter : public TextDiagnosticEmitter {
public:
  ASTDiagnosticEmitter(ASTDiagnosticFormatter &formatter);

public:
  void EmitDeclLoc();
};

} // namespace stone

#endif
