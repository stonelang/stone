#ifndef STONE_COMPILE_COMPILING_H
#define STONE_COMPILE_COMPILING_H

namespace stone {
class Compiler;
namespace syn {
class SourceFile;
}

namespace compiling {

Status Compile(Compiler &compiler);

// Syntax parsing
void VerifyCompilerInputFileTypes(Compiler &compiler);
Status Parse(Compiler &compiler);
Status ParseAndImportResolution(Compiler &compiler);
Status DumpSyntax(Compiler &compiler, SourceFile &sourceFile);

// Type checking
Status TypeCheck(Compiler &compiler);

using TypeCheckingCompletedCallback = llvm::function_ref<Status(Compiler &)>;

Status WithTypeChecking(Compiler &Instance,
                        TypeCheckingCompletedCallback notify,
                        bool typeCheckDespiteErrors = false);

Status FinishTypeCheck(Compiler &compiler);

// Replace with void
Status PrintSyntax(Compiler &compiler);
Status CompileAfterTypeChecking(Compiler &compiler);

/// Code generation
Status GenCode(Compiler &compiler, CodeGenContext &codeGenContext);
Status GenIR(Compiler &compiler, CodeGenContext &codeGenContext);
void DumpIR(Compiler &compiler, CodeGenContext &codeGenContext);
void PrintIR(Compiler &compiler, CodeGenContext &codeGenContext);

} // namespace compiling

// void PerformPrintHelp(Compiler &compiler);
// void PerformPrintVersion(Compiler &compiler);

// Status PerformParse(Compiler &compiler);
// Status PerformResolveImports(Compiler &compiler);
// Status PerformDumpSyntax(Compiler &compiler);

// Status PerformTypeCheck(Compiler &compiler);

// void PerformDumpTypeInfo(Compiler &compiler);

// void PerformPrintSyntax(Compiler &compiler);

// void PerformPrintIR(Compiler &compiler);

// Status PerformEmitIRBefore(Compiler &compiler);

// Status PerformEmitIRAfter(Compiler &compiler);
// Status PerformEmitBC(Compiler &compiler);

// Status PerformEmitObject(Compiler &compiler);
// Status PerformEmitLibrary(Compiler &compiler);

// Status PerformInitModule(Compiler &compiler);

// Status PerformEmitModule(Compiler &compiler);

// Status PerformEmitAssembly(Compiler &compiler);

// Status PerformMergeModules(Compiler &compiler);

} // namespace stone
#endif
