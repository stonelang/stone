#ifndef STONE_AST_ASTDIAGARGUMENT_H
#define STONE_AST_ASTDIAGARGUMENT_H

#include "stone/Support/DiagnosticArgument.h"
#include "stone/Support/DiagnosticEngine.h"
#include "stone/Support/TextDiagnosticConsumer.h"
#include "stone/Support/TextDiagnosticEmitter.h"
#include "stone/AST/Decl.h"
#include "stone/AST/Identifier.h"

using stone::Diagnostic;

#include <assert.h>

namespace stone {

class ASTDiagnostic : public Diagnostic {
public:
  explicit ASTDiagnostic(DiagID diagID, llvm::ArrayRef<DiagnosticArgument> args)
      : Diagnostic(diagID, args) {}
  ~ASTDiagnostic();

public:
};

class ASTDiagnosticEmitter final : public TextDiagnosticEmitter {
public:
  ASTDiagnosticEmitter();

public:
  void EmitDeclLoc();

  void
  Format(ColorStream &out, const Diagnostic &diagnostic,
         DiagnosticFormatOptions fmtOpts = DiagnosticFormatOptions()) override;

  void
  Format(ColorStream &out, llvm::StringRef text,
         llvm::ArrayRef<DiagnosticArgument> args,
         DiagnosticFormatOptions fmtOpts = DiagnosticFormatOptions()) override;

  void FormatArgument(ColorStream &out, llvm::StringRef modifier,
                      llvm::StringRef modifierArguments,
                      ArrayRef<DiagnosticArgument> args, unsigned argIndex,
                      DiagnosticFormatOptions fmtOpts) override;
};

class ASTDiagnosticConsumer final : public TextDiagnosticConsumer {
public:
  ASTDiagnosticConsumer()
      : TextDiagnosticConsumer(ASTDiagnosticEmitter()) {}

public:
};

} // namespace stone

#endif
