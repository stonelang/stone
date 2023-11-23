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
#include "stone/Diag/DiagnosticEngine.h"
#include "stone/Diag/DiagnosticOptions.h"

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
class SyntaxListener;
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
class SyntaxContext;
class SyntaxFile;
class ModuleDecl;
} // namespace syn
} // namespace stone

namespace stone {

class LangContext final {
  FileMgr fm;
  SrcMgr sm;
  LangOptions langOpts;
  StatisticEngine se;
  ColorStream cos;
  FileSystemOptions fsOpts;

  DiagnosticEngine de;
  DiagnosticOptions diagOpts;
 

public:
  LangContext() : fm(fsOpts), de(diagOpts, sm), cos(llvm::outs()) {}
  ~LangContext() {}

public:
  ColorStream &Out() { return cos; }

  StatisticEngine &GetStats() { return se; }
  const StatisticEngine &GetStats() const { return se; }

  LangOptions &GetLangOptions() { return langOpts; }
  const LangOptions &GetLangOptions() const { return langOpts; }

  FileSystemOptions &GetFileSystemOptions() { return fsOpts; }
  const FileSystemOptions &GetFileSystemOptions() const { return fsOpts; }

  DiagnosticEngine &GetDiags() { return de; }
  const DiagnosticEngine &GetDiags() const { return de; }

  DiagnosticOptions &GetDiagOptions() { return diagOpts; }
  const DiagnosticOptions &GetDiagOptions() const { return diagOpts; }

  bool HasError() { return de.HasError(); }
  
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

using ModuleSyntaxFileUnion =
    llvm::PointerUnion<syn::ModuleDecl *, syn::SyntaxFile *>;

// using ParseSyntaxFileListener =
//      llvm::function_ref<void(syn::SyntaxFile &syntaxFile, bool *error)>;

// using SemanticConsumer = llvm::function_ref<void(
//     CompilerInstance &instance, ModuleSyntaxFileUnion moduleSyntaxFileUnion,
//     bool *error)>;

// using IRCodeGenConsumer = llvm::function_ref<Status(
//     CompilerInstance &instance, CodeGenContext &codeGenContext, bool
//     *error)>;

// using NativeCodeGenConsumer = llvm::function_ref<void(
//     CompilerInstance &instance, CodeGenContext &codeGenContext, bool
//     *error)>;

} // namespace stone
namespace stone {
class ClangContext;
class CompilerInstance;

// bool CompileFrontend(CompilerInstance &instance);
// bool CompileBackend(CompilerInstance &instance);

/// Parse, type-check, resolve imports, and generate IR for the SyntaxFile.
//  This will allows for parallelization specially when you are just in parsing
//  mode.
/// Returns true is successfull
bool CompileSyntaxFile(syn::SyntaxFile &syntaxFile, CompilerInstance &instance,
                       CodeGenContext *cgc = nullptr);

/// This walks the syntax to resolve imports.
/// Returns true is successfull
void ParseSyntaxFile(syn::SyntaxFile &syntaxFile, syn::SyntaxContext &context,
                     SyntaxListener *listener = nullptr);

/// This walks the syntax to resolve imports.
/// Returns true is successfull
void ResolveSyntaxFileImports(syn::SyntaxFile &syntaxFile);

/// Once import resolution is complete, this walks the syntax to resolve types
/// and diagnose problems therein.
/// Returns true is successfull
void TypeCheckSyntaxFile(
    syn::SyntaxFile &syntaxFile, TypeCheckerOptions &opts,
    TypeCheckerListener *listener =
        nullptr /*, TypeCheckSyntaxFileCallback* callback = nullptr*/);

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
void SerializeSyntaxFile(syn::SyntaxFile &syntaxFile);

/// Returns true is successfull
void SerializeModuleDecl(syn::ModuleDecl &moduleDecl);

/// GenIR for the ModuleFile
/// Returns true is successfull
void GenSyntaxFileIR(CodeGenContext &cgc, llvm::StringRef moduleName,
                     syn::SyntaxFile *sf,
                     const PrimaryFileSpecificPaths specificPaths,
                     CodeGenListener *listener = nullptr);

/// Gen IR for the Module
/// Returns true is successfull
void GenModuleIR(CodeGenContext &cgc, llvm::StringRef moduleName,
                 syn::ModuleDecl *mod,
                 const PrimaryFileSpecificPaths specificPaths,
                 CodeGenListener *listener = nullptr);

bool EmitImportedModules(syn::SyntaxContext &context,
                         syn::ModuleDecl *mainModule,
                         const CompilerOptions &opts);

} // namespace stone

// Code generation
namespace stone {
/// Returns true is successfull
// bool CompileBackend(CodeGenContext &codeGenConext,
//                     syn::SyntaxContext &syntaxContext,
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
bool GenNative(CodeGenContext &cgc, syn::SyntaxContext &context,
               llvm::StringRef outputFilename,
               CodeGenListener *listener = nullptr);

bool WriteEmptyOutputFiles(std::vector<std::string> &parallelOutputFilenames,
                           const syn::SyntaxContext &Context,
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

bool GenNative(CodeGenContext &cgc, syn::SyntaxContext &context,
               llvm::StringRef outputFilename, llvm::sys::Mutex *diagMutex,
               llvm::GlobalVariable *hashGlobal,
               CodeGenListener *listener = nullptr);

/// Returns true is successfull
void WriteNative(CodeGenContext &cgc, llvm::raw_pwrite_stream &out,
                 llvm::sys::Mutex *diagMutex = nullptr,
                 CodeGenScope *parentScope = nullptr);

} // namespace stone
#endif
