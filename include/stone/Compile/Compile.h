#ifndef STONE_COMPILE_COMPILE_H
#define STONE_COMPILE_COMPILE_H

#include "stone/AST/Module.h"
#include "stone/Basic/CodeGenOptions.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/IR/Module.h"

namespace stone {

class SourceFile;
class ModuleDecl;
class CodeGenResult;
class CompilerInstance;
class CompilerObservation;
class PrimaryFileSpecificPaths;
class CodeGenOptions;
class LangOptions;
class ClangImporter;

int Compile(llvm::ArrayRef<const char *> args, const char *arg0, void *mainAddr,
            CompilerObservation *observation = nullptr);

/// \return true if compilie is successful
bool PerformCompile(CompilerInstance &instance);

/// \return true if compilie is successful
bool PerformAction(CompilerInstance &instance);

/// \retyrb true if we compiled an ir file.
bool PerformCompileLLVM(CompilerInstance &compiler);

using PerformParseCallback = llvm::function_ref<bool(CompilerInstance &)>;

/// \return true if syntax analysis is successful
bool PerformParse(CompilerInstance &instance,
                  PerformParseCallback callback = nullptr);

/// \return true if syntax analysis is successful for a specific source file
bool PerformEmitParse(CompilerInstance &instance);

using PerformResolveImportsCallback =
    llvm::function_ref<bool(CompilerInstance &)>;

// \return true if syntax analysis is successful
bool PerformResolveImports(CompilerInstance &instance,
                           PerformResolveImportsCallback callback = nullptr);

using PerformSemanticAnalysisCallback =
    llvm::function_ref<bool(CompilerInstance &)>;

// \return true if semantic analysis is successful
bool PerformSemanticAnalysis(
    CompilerInstance &instance,
    PerformSemanticAnalysisCallback callback = nullptr);

// \return true if emit-ast is true
bool PerformEmitAST(CompilerInstance &instance);

// \return true if the code generation was successfull
bool PerformEmitCode(CompilerInstance &instance);

using PerformEmitIRCallback =
    llvm::function_ref<bool(CompilerInstance &, CodeGenResult &)>;

// \return true if the code generation was successfull
bool PerformEmitIR(CompilerInstance &instance, PerformEmitIRCallback callback);

// // \return true if the code generation was successfull
// bool PerformEmitIR(CompilerInstance &instance,
//                          ModuleDeclOrModuleFile moduleOrFile,
//                          const PrimaryFileSpecificPaths &sps);

// // \return llvm::Module if IR generation is successful
CodeGenResult PerformEmitSourceFile(CompilerInstance &instance,
                                    SourceFile &sourceFile,
                                    llvm::StringRef moduleName,
                                    const PrimaryFileSpecificPaths &sps,
                                    llvm::GlobalVariable *&globalHash);

// ///\return the generated module
CodeGenResult
PerformEmitModule(CompilerInstance &instance, ModuleDecl *moduleDecl,
                  llvm::StringRef moduleName,
                  const PrimaryFileSpecificPaths &sps,
                  llvm::ArrayRef<std::string> parallelOutputFilenames,
                  llvm::GlobalVariable *&globalHash);

// \return true if syntax analysis is successful
bool PerformEmitBackend(CompilerInstance &instance,
                        llvm::StringRef outputFilename, llvm::Module *module,
                        llvm::GlobalVariable *&globalHash);

// ///\return the IRTargetOptions
IRTargetOptions GetIRTargetOptions(const CodeGenOptions &codeGenOpts,
                                   const LangOptions &langOpts,
                                   ClangImporter &clangImporter);

} // namespace stone

#endif
