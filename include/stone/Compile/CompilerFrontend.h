#ifndef STONE_COMPILE_FRONTEND_H
#define STONE_COMPILE_FRONTEND_H

#include "stone/Basic/CodeGenOptions.h"
#include "stone/CodeGen/CodeGenModule.h"
#include "stone/Compile/CompilerOptions.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/IR/Module.h"

namespace stone {
class SourceFile;
class LangOptions;
class ClangImporter;
class CompilerInstance;
class PrimaryFileSpecificPaths;
class CodeGenResult;

/// \return true if compilie is successful
bool PerformFrontend(CompilerInstance &instance);

/// \retyrb true if we compiled an ir file.
bool PerformCompileLLVM(CompilerInstance &compiler);

/// \return true if syntax analysis is successful
bool PerformParse(CompilerInstance &instance);

/// \return true if syntax analysis is successful for a specific source file
bool PerformParse(CompilerInstance &instance, SourceFile &sourceFile);

/// \return true if syntax analysis is successful for a specific source file
bool PerformEmitParse(CompilerInstance &instance);

// \return true if syntax analysis is successful
bool PerformResolveImports(CompilerInstance &instance);

// \resolve all of the source files for this import
bool PerformResolveImports(CompilerInstance &instance, SourceFile &sourceFile);

// \return true if semantic analysis is successful
bool PerformSemanticAnalysis(CompilerInstance &instance);

// \return true if semantic analysis is successful
bool PerformSemanticAnalysis(CompilerInstance &instance,
                             SourceFile &sourceFile);

// \return true if emit-ast is true
bool PerformEmitAST(CompilerInstance &instance);

// \return true if the code generation was successfull
bool CompletedSemanticAnalysis(CompilerInstance &instance);

// \return true if the code generation was successfull
bool EmitCode(CompilerInstance &instance, ModuleDeclOrModuleFile moduleOrFile,
              const PrimaryFileSpecificPaths &sps);

// \return llvm::Module if IR generation is successful
CodeGenResult PerformCodeGenIR(CompilerInstance &instance,
                               SourceFile *sourceFile,
                               llvm::StringRef moduleName,
                               const PrimaryFileSpecificPaths &sps,
                               llvm::GlobalVariable *&globalHash);

///\return the generated module
CodeGenResult PerformCodeGenIR(CompilerInstance &instance,
                               ModuleDecl *moduleDecl,
                               llvm::StringRef moduleName,
                               const PrimaryFileSpecificPaths &sps,
                               ArrayRef<std::string> parallelOutputFilenames,
                               llvm::GlobalVariable *&globalHash);

// \return true if syntax analysis is successful
bool PerformCodeGenBackend(CompilerInstance &instance,
                           llvm::StringRef outputFilename, llvm::Module *module,
                           llvm::GlobalVariable *&globalHash);

///\return the IRTargetOptions
IRTargetOptions GetIRTargetOptions(const CodeGenOptions &codeGenOpts,
                                   const LangOptions &langOpts,
                                   ClangImporter &clangImporter);
} // namespace stone

#endif
