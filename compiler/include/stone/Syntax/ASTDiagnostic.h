#ifndef STONE_SYNTAX_SYNTAXDIAGARGUMENT_H
#define STONE_SYNTAX_SYNTAXDIAGARGUMENT_H

#include "stone/Support/DiagnosticArgument.h"
#include "stone/Support/DiagnosticEngine.h"
#include "stone/Support/TextDiagnosticEmitter.h"
#include "stone/Support/TextDiagnosticConsumer.h"
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
};

class ASTTextDiagnosticConsumer final : public TextDiagnosticConsumer {

  
};

} // namespace stone

#endif
