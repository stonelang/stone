#include "stone/AST/ASTDiagnosticArgument.h"
#include "stone/AST/Module.h"
#include "stone/Basic/Defer.h"
#include "stone/CodeCompletionListener.h"
#include "stone/Compile/CompilerInstance.h"
#include "stone/Compile/CompilerInvocation.h"
#include "stone/Diag/CompilerDiagnostic.h"
#include "stone/Diag/TextDiagnosticFormatter.h"
#include "stone/Diag/TextDiagnosticListener.h"
#include "stone/Lang.h"
#include "stone/Options/ModeKind.h"

#include "clang/Basic/TargetInfo.h"

#include "llvm/IR/Module.h"
#include "llvm/Support/Casting.h"

using namespace stone;
using namespace stone::ast;

class CodeAnalysis final {
public:
  static Status PerformSyntaxAnalysis(CompilerInstance &compiler);
  static Status
  PerformSyntaxAnalysisAndImportResoltuion(CompilerInstance &compiler);
  static void NotifySyntaxAnalysisCompleted(CompilerInstance &compiler);
  static void PerformDumpAST(CompilerInstance &compiler);

public:
  static Status PerformSemanticAnalysis(CompilerInstance &compiler);
  static void NotifySemanticAnalysisCompleted(CompilerInstance &compiler);
  static void PerformPrintAST(CompilerInstance &compiler);
};

Status CodeAnalysis::PerformSyntaxAnalysis(CompilerInstance &compiler) {

  for (auto moduleFile :
       compiler.GetModuleSystem().GetMainModule()->GetFiles()) {
    if (auto *astFile = llvm::dyn_cast<ast::ASTFile>(moduleFile)) {
      Lang::ParseASTFile(*asttaxFile, GetASTContext(),
                         invocation.GetListener());
    }
  }
  CodeAnalysis::NotifySyntaxAnalysisCompleted(compiler);
}

Status CodeAnalysis::PerformSyntaxAnalysisAndImportResoltuion(
    CompilerInstance &compiler) {
  if (CodeAnalysis::PerformSyntaxAnalysis(compiler).IsError()) {
    return Status::Error();
  }
}
void CodeAnalysis::NotifySyntaxAnalysisCompleted(CompilerInstance &compiler) {
  if (compiler.GetInvocation().GetListener()) {
    compiler.GetInvocation().GetListener()->OnSyntaxAnalysisCompleted(compiler);
  }
}

void CodeAnalysis::PerformDumpAST(CompilerInstance &compiler) {}

Status CodeAnalysis::PerformSemanticAnalysis(CompilerInstance &compiler) {

  compiler.ForEachASTFile([&](ASTFile &astFile,
                              TypeCheckerOptions &typeCheckerOpts,
                              stone::TypeCheckerListener *listener) {
    Lang::TypeCheckASTFile(astFile, typeCheckerOpts, listener);
  });

  NotifySemanticAnalysisCompleted(compiler);
}

void CodeAnalysis::NotifySemanticAnalysisCompleted(CompilerInstance &compiler) {
  if (compiler.GetInvocation().GetListener()) {
    compiler.GetInvocation().GetListener()->OnSemanticAnalysisCompleted(
        compiler);
  }
}

void CodeAnalysis::PerformPrintAST(CompilerInstance &compiler) {}

//// Execute only the analysis part of the compiler
bool Lang::CompileWithCodeAnalysis(CompilerInstance &compiler) {

  // At this point, everything requires syntax analysis.
  if (compiler.GetMode().IsParse()) {
    if (CodeAnalysis::PerformSyntaxAnalysis(compiler).IsError()) {
      return Status::Error();
    }
  }

  // Otherwise, default to performing syntax analysis with import resoltuion.
  if (CodeAnalysis::PerformSyntaxAnalysisAndImportResoltuion(compiler)
          .IsError()) {
    return Status::Error();
  }

  if (compiler.GetMode().IsResolveImports()) {
    return Status::Success();
  }

  // Are we trying to dump the AST?
  if (compiler.GetMode().IsDumpAST()) {
    if (CodeAnalysis::PerformDumpAST(compiler).IsError()) {
      return Status::Error();
    }
    return Status();
  }
  // At this point, everything requires type-checking
  if (CodeAnalysis::PerformSemanticAnalysis(compiler).IsError()) {
    return Status::Error();
  }

  if (GetMode().IsTypeCheck()) {
    return Status::Success();
  }
  // Are we trying to print the AST?
  if (GetMode().IsPrintAST()) {
    if (CodeAnalysis::PerformPrintAST(compiler).IsError()) {
      return Status::Error();
    }
  }
}