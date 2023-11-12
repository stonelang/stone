#ifndef STONE_COMPILE_COMPILING_H
#define STONE_COMPILE_COMPILING_H

namespace stone {
class CompilerInstance;
namespace syn {
class SyntaxFile;
}

namespace compiling {

Status Compile(CompilerInstance &compiler);

// Syntax parsing
void VerifyCompilerInputFileTypes(CompilerInstance &compiler);
Status Parse(CompilerInstance &compiler);
Status ParseAndImportResolution(CompilerInstance &compiler);
Status DumpSyntax(CompilerInstance &compiler, syn::SyntaxFile &syntaxFile);

// Type checking
Status TypeCheck(CompilerInstance &compiler);

using TypeCheckingCompletedCallback =
    llvm::function_ref<Status(CompilerInstance &)>;

Status WithTypeChecking(CompilerInstance &Instance,
                        TypeCheckingCompletedCallback notify,
                        bool typeCheckDespiteErrors = false);

Status FinishTypeCheck(CompilerInstance &compiler);

// Replace with void
Status PrintSyntax(CompilerInstance &compiler);
Status CompileAfterTypeChecking(CompilerInstance &compiler);

/// Code generation
Status GenCode(CompilerInstance &compiler, CodeGenContext &codeGenContext);
Status GenIR(CompilerInstance &compiler, CodeGenContext &codeGenContext);
void DumpIR(CompilerInstance &compiler, CodeGenContext &codeGenContext);
void PrintIR(CompilerInstance &compiler, CodeGenContext &codeGenContext);

} // namespace compiling
} // namespace stone
#endif
