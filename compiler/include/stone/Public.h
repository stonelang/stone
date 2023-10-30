#ifndef STONE_PUBLIC_H
#define STONE_PUBLIC_H

#include "stone/Basic/CodeGenOptions.h"
#include "stone/Basic/Error.h"
#include "stone/Basic/FileMgr.h"
#include "stone/Basic/FileSystemOptions.h"
#include "stone/Basic/LangOptions.h"
#include "stone/Basic/Result.h"
#include "stone/Basic/SrcMgr.h"
#include "stone/Basic/StatisticEngine.h"
#include "stone/Diag/DiagUnit.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/StringSet.h"
#include "llvm/Support/Mutex.h"

namespace llvm {
class Module;
class MemoryBuffer;
class TargetOptions;
class TargetMachine;
class raw_pwrite_stream;
class GlobalVariable;
} // namespace llvm

namespace stone {
class ASTListener;
class CodeGenOptions;
class CodeGenContext;
class CodeGenScope;
class TargetOptions;
class LangOptions;
class DiagnosticEngine;
class CompilerInstance;
class TypeCheckerListener;
class CodeGenListener;
class TypeCheckerOptions;
class CodeGenListener;
class PrimaryFileSpecificPaths;
class CompilerOptions;

namespace syn {
class ASTContext;
class ASTFile;
class ModuleDecl;
} // namespace syn
} // namespace stone

namespace stone {

class LangContext final {
  FileMgr fm;
  SrcMgr sm;
  DiagnosticOptions diagOpts;
  DiagnosticEngine de;
  LangOptions langOpts;
  StatisticEngine se;
  ColorStream cos;
  FileSystemOptions fsOpts;

public:
  LangContext() : fm(fsOpts), de(diagOpts, sm), cos(llvm::outs()) {}
  ~LangContext() {}

public:
  ColorStream &Out() { return cos; }

  StatisticEngine &GetStatEngine() { return se; }
  const StatisticEngine &GeStatEngine() const { return se; }

  LangOptions &GetLangOptions() { return langOpts; }
  const LangOptions &GetLangOptions() const { return langOpts; }

  FileSystemOptions &GetFileSystemOptions() { return fsOpts; }
  const FileSystemOptions &GetFileSystemOptions() const { return fsOpts; }

  DiagnosticEngine &GetDiagnoticEngine() { return de; }

  FileMgr &GetFileMgr() { return fm; }
  SrcMgr &GetSrcMgr() { return sm; }
};
} // namespace stone

// Parsing and type-checking
namespace stone {
class CompilerListener;
int Compile(llvm::ArrayRef<const char *> args, const char *arg0, void *mainAddr,
            CompilerListener *listener = nullptr);

} // namespace stone

namespace stone {

class CodeGenContext;
class CompilerInstance;

using ModuleASTFileUnion =
    llvm::PointerUnion<syn::ModuleDecl *, syn::ASTFile *>;

// using ParseASTFileListener =
//      llvm::function_ref<void(syn::ASTFile &syntaxFile, bool *error)>;

// using SemanticConsumer = llvm::function_ref<void(
//     CompilerInstance &instance, ModuleASTFileUnion moduleASTFileUnion,
//     bool *error)>;

// using CodeGenConsumer = llvm::function_ref<Status(
//     CompilerInstance &instance, CodeGenContext &codeGenContext, bool
//     *error)>;

// using CodeGenMachineConsumer = llvm::function_ref<void(
//     CompilerInstance &instance, CodeGenContext &codeGenContext, bool
//     *error)>;

} // namespace stone
namespace stone {
class ClangContext;
class CompilerInstance;

// bool CompileFrontend(CompilerInstance &instance);
// bool CompileBackend(CompilerInstance &instance);

/// Parse, type-check, resolve imports, and generate IR for the ASTFile.
//  This will allows for parallelization specially when you are just in parsing
//  mode.
/// Returns true is successfull
bool CompileASTFile(syn::ASTFile &syntaxFile, CompilerInstance &instance,
                       CodeGenContext *cgc = nullptr);

/// This walks the syntax to resolve imports.
/// Returns true is successfull
void ParseASTFile(syn::ASTFile &syntaxFile, syn::ASTContext &context,
                     ASTListener *listener = nullptr);

/// This walks the syntax to resolve imports.
/// Returns true is successfull
void ResolveASTFileImports(syn::ASTFile &syntaxFile);

/// Once import resolution is complete, this walks the syntax to resolve types
/// and diagnose problems therein.
/// Returns true is successfull
void TypeCheckASTFile(
    syn::ASTFile &syntaxFile, TypeCheckerOptions &opts,
    TypeCheckerListener *listener =
        nullptr /*, TypeCheckASTFileCallback* callback = nullptr*/);

/// Now that we have type-checked an entire module, perform any type
/// checking that requires the full module.
///
/// Note that clients still perform this checking file-by-file to
/// provide a somewhat defined order in which diagnostics should be
/// emitted.
/// Returns true is successfull
void TypeCheckWholeModule(
    syn::ModuleDecl &moduleDecl, TypeCheckerOptions &opts,
    TypeCheckerListener *listener =
        nullptr /*, TypeCheckWholeModuleCallback* callback = nullptr*/);

/// Returns true is successfull
void SerializeASTFile(syn::ASTFile &syntaxFile);

/// Returns true is successfull
void SerializeModuleDecl(syn::ModuleDecl &moduleDecl);

/// GenIR for the ModuleFile
/// Returns true is successfull
void GenASTFileIR(CodeGenContext &cgc, llvm::StringRef moduleName,
                     syn::ASTFile *sf,
                     const PrimaryFileSpecificPaths specificPaths,
                     CodeGenListener *listener = nullptr);

/// Gen IR for the Module
/// Returns true is successfull
void GenModuleIR(CodeGenContext &cgc, llvm::StringRef moduleName,
                 syn::ModuleDecl *mod,
                 const PrimaryFileSpecificPaths specificPaths,
                 CodeGenListener *listener = nullptr);

bool EmitImportedModules(syn::ASTContext &context,
                         syn::ModuleDecl *mainModule,
                         const CompilerOptions &opts);

} // namespace stone

// Code generation
namespace stone {
/// Returns true is successfull
// bool CompileBackend(CodeGenContext &codeGenConext,
//                     syn::ASTContext &syntaxContext,
//                     CodeGenListener *listener = nullptr);

// std::unique_ptr<llvm::TargetMachine>
// CreateTargetMachine(CodeGenContext &context);

IRTargetOptions GetIRTargetOptions(const CodeGenOptions &opts,
                                   const LangOptions &langOpts,
                                   ClangContext &cc);

/// Disable thumb-mode until debugger support is there.
bool ShouldRemoveTargetFeature(llvm::StringRef feature);

std::unique_ptr<llvm::TargetMachine>
CreateTargetMachine(const CodeGenOptions &opts);

void OptimizeIR(llvm::Module *mod, const CodeGenOptions &opts,
                LangContext &langContext, llvm::TargetMachine *target);

/// Returns true is successfull
bool GenNative(CodeGenContext &cgc, syn::ASTContext &context,
               llvm::StringRef outputFilename,
               CodeGenListener *listener = nullptr);

bool WriteEmptyOutputFiles(std::vector<std::string> &parallelOutputFilenames,
                           const syn::ASTContext &Context,
                           const CodeGenOptions &opts);

/// Run the LLVM passes. In multi-threaded compilation this will be done for
/// multiple LLVM modules in parallel.
/// \param Diags The Diagnostic Engine.
/// \param DiagMutex in contexts that require parallel codegen, a mutex that the
///                  diagnostic engine uses to synchronize emission.
/// \param HashGlobal used with incremental LLVMCodeGen to know if a module
///                   was already compiled, may be null if not desired.
/// \param Module LLVM module to code gen, required.
/// \param TargetMachine target of code gen, required.
/// \param OutputFilename Filename for output.

bool GenNative(CodeGenContext &cgc, syn::ASTContext &context,
               llvm::StringRef outputFilename, llvm::sys::Mutex *diagMutex,
               llvm::GlobalVariable *hashGlobal,
               CodeGenListener *listener = nullptr);

/// Returns true is successfull
void WriteNative(CodeGenContext &cgc, llvm::raw_pwrite_stream &out,
                 llvm::sys::Mutex *diagMutex = nullptr,
                 CodeGenScope *parentScope = nullptr);

} // namespace stone
#endif
