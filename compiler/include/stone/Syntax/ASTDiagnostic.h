#ifndef STONE_SYNTAX_SYNTAXDIAGARGUMENT_H
#define STONE_SYNTAX_SYNTAXDIAGARGUMENT_H

#include "stone/Support/DiagnosticArgument.h"
#include "stone/Support/DiagnosticEngine.h"
#include "stone/Support/TextDiagnosticConsumer.h"
#include "stone/Support/TextDiagnosticEmitter.h"
#include "stone/Syntax/Decl.h"
#include "stone/Syntax/Identifier.h"

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

class ASTTextDiagnosticEmitter final : public TextDiagnosticEmitter {
public:
  ASTTextDiagnosticEmitter();

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

class ASTTextDiagnosticConsumer final : public TextDiagnosticConsumer {
public:
  ASTTextDiagnosticConsumer()
      : TextDiagnosticConsumer(ASTTextDiagnosticEmitter()) {}

public:
};

} // namespace stone

#endif
