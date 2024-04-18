#ifndef STONE_SYNTAX_SYNTAXDIAGARGUMENT_H
#define STONE_SYNTAX_SYNTAXDIAGARGUMENT_H

#include "stone/Support/DiagnosticArgument.h"
#include "stone/Support/DiagnosticEngine.h"
#include "stone/Support/TextDiagnosticEmitter.h"
#include "stone/Support/TextDiagnosticFormatter.h"
#include "stone/Syntax/Decl.h"
#include "stone/Syntax/Identifier.h"

using stone::Diagnostic;

#include <assert.h>

namespace stone {

namespace diag {

// struct TokenArgument final : public ASTArgument {
//   const Token *val;
//   Decl() = delete;
//   explicit TokenArgument(const Token *val)
//       : ASTArgument(ASTArgumentKind::Token), val(val) {}
// };

struct Decl final : public ASTArgument {
  const Decl *val;
  Decl() = delete;
  explicit Decl(const Decl *val)
      : ASTArgument(ASTArgumentKind::Decl), val(val) {}

public:
  static bool classof(const ASTArgument *a) {
    return a->GetASTArgumentKind() == ASTArgumentKind::Decl;
  }
};

struct DeclContext final : public ASTArgument {
  const DeclContext *val;
  DeclContext() = delete;
  explicit DeclContext(const DeclContext *val)
      : ASTArgument(ASTArgumentKind::DeclContext), val(val) {}

public:
  static bool classof(const ASTArgument *a) {
    return a->GetASTArgumentKind() == ASTArgumentKind::DeclContext;
  }
};

struct Identifier final : public ASTArgument {
  const Identifier *val;
  Identifier() = delete;
  explicit Identifier(const Identifier *val)
      : ASTArgument(ASTArgumentKind::Identifier), val(val) {}

public:
  static bool classof(const ASTArgument *a) {
    return a->GetASTArgumentKind() == ASTArgumentKind::Identifier;
  }
};

struct Type final : public ASTArgument {
  const Type *val;
  Type() = delete;
  explicit Type(const Type *val)
      : ASTArgument(ASTArgumentKind::Type), val(val) {}

public:
  static bool classof(const ASTArgument *a) {
    return a->GetASTArgumentKind() == ASTArgumentKind::Type;
  }
};

} // namespace diag

class ASTDiagnostic : public Diagnostic {
public:
  explicit ASTDiagnostic(DiagID diagID, llvm::ArrayRef<diag::Argument> args)
      : Diagnostic(diagID, args) {}
  ~ASTDiagnostic();

public:
};

class ASTDiagnosticFormatter : public TextDiagnosticFormatter {
public:
  ASTDiagnosticFormatter();

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

class ASTDiagnosticEmitter : public TextDiagnosticEmitter {
public:
  ASTDiagnosticEmitter(ASTDiagnosticFormatter &formatter);

public:
  void EmitDeclLoc();
};

} // namespace stone

#endif
