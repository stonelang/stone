#ifndef STONE_COMPILE_COMPILING_H
#define STONE_COMPILE_COMPILING_H

#include "stone/Basic/Status.h"

namespace stone {

class CompilerInstance;
class Compiling final {
public:
  static Status PerformSyntaxAnalysis(CompilerInstance &compiler);
  static Status
  PerformSyntaxAnalysisAndImportResoltuion(CompilerInstance &compiler);
  static void NotifySyntaxAnalysisCompleted(CompilerInstance &compiler);
  static void PerformDumpAST(CompilerInstance &compiler);
  static Status PerformSemanticAnalysis(CompilerInstance &compiler);
  static Status FinishSemanticAnalysis(CompilerInstance &compiler);
  static void NotifySemanticAnalysisCompleted(CompilerInstance &compiler);
  static void PerformPrintAST(CompilerInstance &compiler);
  static void PerformDumpTypeInfo(CompilerInstance &compiler);

  static Status CompileWithCompilelysis(CompilerInstance &compiler);

public:
  static Status CompilePostCompilelysis(CompilerInstance &compiler);

public:
  // Some work here before code generation
  static Status CompileWithCodeGeneration(CompilerInstance &compiler);

  static Status PerformIRGeneration(CompilerInstance &compiler,
                                    CodeGenContext &codeGenContext);
  static void PerformDumpIR(CompilerInstance& compiler, CodeGenContext &codeGenContext);
  static void PerformPrintIR(CompilerInstance& compiler, CodeGenContext &codeGenContext);
  static Status PerformNativeGeneration(CompilerInstance &compiler,
                                        CodeGenContext &codeGenContext);
  static void NotifyNativeGenerationCompleted(CompilerInstance &compiler);
};

} // namespace stone
