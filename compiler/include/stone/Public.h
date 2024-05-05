#ifndef STONE_PUBLIC_H
#define STONE_PUBLIC_H

#include "stone/AST/Diagnostics.h"
#include "stone/Basic/CodeGenOptions.h"
#include "stone/Basic/LangOptions.h"
#include "stone/IDE.h"

#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/StringSet.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/Mutex.h"
#include "llvm/Target/TargetMachine.h"

namespace stone {
class CodeGenOptions;
class LangOptions;
class CompilerOptions;
class Options;
class Compiler;
class CompilerInvocation;
class ASTContext;
class ModuleDecl;
class SourceFile;
class ModuleFile;
class CompilerInputFile;
class IRGenRequest;
class IRGenResult;
class TypeCheckerOptions;
class ClangContext;
class PrimaryFileSpecificPaths;

using ModuleDeclOrModuleFile = llvm::PointerUnion<ModuleDecl *, ModuleFile *>;

struct ModuleInfo final {
  ModuleDeclOrModuleFile info;

  ModuleInfo(ModuleDeclOrModuleFile info) : info(info) {}

  ModuleFile *GetAsModuleFile() const { return info.get<ModuleFile *>(); }
  ModuleDecl *GetAsModuleDecl() const { return info.get<ModuleDecl *>(); }
};

/// This walks the syntax to resolve imports.
/// Returns true is successfull
void ParseSourceFile(SourceFile &sourceFile, ASTContext &context,
                     CodeCompletionCallbacks *callbacks);

/// Dump the source file that we parsed
void DumpSourceFile(SourceFile &sourceFile, ASTContext &astContext);

/// This walks the syntax to resolve imports.
/// Returns true is successfull
void ResolveSourceFileImports(SourceFile &sourceFile);

/// Once import resolution is complete, this walks the syntax to resolve types
/// and diagnose problems therein.
/// Returns true is successfull
void TypeCheckSourceFile(SourceFile &sourceFile, TypeCheckerOptions &opts);

/// Pretty print the type checked source file.
void PrintSourceFile(SourceFile &sourceFile, ASTContext &astContext);

/// Now that we have type-checked an entire module, perform any type
/// checking that requires the full module.
///
/// Note that clients still perform this checking file-by-file to
/// provide a somewhat defined order in which diagnostics should be
/// emitted.
/// Returns true is successfull
void TypeCheckWholeModule(ModuleDecl &moduleDecl, TypeCheckerOptions &opts);

/// Returns true is successfull
void SerializeSourceFile(SourceFile &sourceFile);

/// Returns true is successfull
void SerializeModuleDecl(ModuleDecl &moduleDecl);

// TargetOptions for IR generation
IRTargetOptions GetIRTargetOptions(const CodeGenOptions &opts,
                                   const LangOptions &langOpts,
                                   ClangContext &clangContext);
/// GenIR for the ModuleFile
/// Returns true is successfull
// IRCoeGenRequest
// TODO, you may just want to return a pointer
IRGenResult *GenIR(IRGenRequest request);

std::unique_ptr<llvm::Module>
GenIR(const CodeGenOptions &codeGenOpts, ModuleDecl *moduleDecl,
      const llvm::StringRef moduleName, ASTContext &astContext,
      const PrimaryFileSpecificPaths psps,
      llvm::ArrayRef<std::string> parallelOutputFilenames,
      llvm::GlobalVariable *outModuleHash = nullptr);

std::unique_ptr<llvm::Module>
GenIR(const CodeGenOptions &codeGenOpts, ModuleFile *moduleFile,
      const llvm::StringRef moduleName, ASTContext &astContext,
      const PrimaryFileSpecificPaths psps,
      llvm::GlobalVariable *outModuleHash = nullptr);

bool EmitIR();

// IRGenResult GenIRInParallel(ParallelCodeGenContext);

bool EmitImportedModules(ASTContext &context, ModuleDecl *mainModule,
                         const CompilerOptions &opts);

/// Disable thumb-mode until debugger support is there.
bool ShouldRemoveTargetFeature(llvm::StringRef feature);

std::unique_ptr<llvm::TargetMachine>
CreateTargetMachine(const CodeGenOptions &codeGenOpts);

void OptimizeIR(const CodeGenOptions &opts, llvm::Module *llvmModule,
                llvm::TargetMachine *target, DiagnosticEngine &diags);

/// Returns true is successfull
// You want IRGenOutput
bool EmitNative(const CodeGenOptions &codeGenOpts, llvm::Module *llvmModule,
                ASTContext &astContext, llvm::StringRef outputFilename);

bool EmitNative(const CodeGenOptions &codeGenOpts, llvm::Module *llvmModule,
                llvm::StringRef outputFilename, llvm::sys::Mutex *diagMutex,
                llvm::GlobalVariable *hashGlobal,
                llvm::TargetMachine *targetMachine);

bool EmitNativeWithParallelization();

bool WriteEmptyOutputFiles(std::vector<std::string> &parallelOutputFilenames,
                           const ASTContext &Context,
                           const CodeGenOptions &opts);

void EmbedBitCode(const CodeGenOptions &codeGenOpts, llvm::Module *llvmModule);

/// Print the compiler version
void PrintVersion();

/// Print the help for the driver
void PrintHelp(const Options &options);

/// Print compiler features
void PrintFeature();

/// Print information about the target
void PrintTargetInfo(const CompilerInvocation &invocation, ColorStream &out);

/// Print information about the target
void PrintTripleInfo(const llvm::Triple &triple,
                     std::optional<llvm::VersionTuple> runtimeVersion,
                     ColorStream &out);
} // namespace stone
#endif
