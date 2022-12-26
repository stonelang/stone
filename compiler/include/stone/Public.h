#ifndef STONE_PUBLIC_H
#define STONE_PUBLIC_H

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
} // namespace llvm

namespace stone {
class Error;
class Status;
class SyntaxListener;
class CodeGenOptions;
class CodeGenContext;
class TargetOptions;
class LangOptions;
class DiagnosticEngine;
class CompilerInstance;
class TypeCheckerListener;
class CodeGenListener;
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
  DiagUnit du;
  LangOptions langOpts;
  StatisticEngine se;
  ColorfulStream cos;
  FileSystemOptions fsOpts;

public:
  LangContext() : fm(fsOpts), du(sm), cos(llvm::outs()) {}
  ~LangContext() {}

public:
  ColorfulStream &Out() { return cos; }

  StatisticEngine &GetStatEngine() { return se; }
  const StatisticEngine &GeStatEngine() const { return se; }

  LangOptions &GetLangOptions() { return langOpts; }
  const LangOptions &GetLangOptions() const { return langOpts; }

  FileSystemOptions &GetFileSystemOptions() { return fsOpts; }
  const FileSystemOptions &GetFileSystemOptions() const { return fsOpts; }

  DiagUnit &GetDiagUnit() { return du; }

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
class CompilerInstance;

/// Parse, type-check and generate IR for syntax files.
/// Returns true is successfull
bool CompileFrontend(CompilerInstance &instance/*, CompileFrontendCallback* callback = nullptr*/ );

/// Parse, type-check, resolve imports, and generate IR for the SyntaxFile.
//  This will allows for parallelization specially when you are just in parsing
//  mode.
/// Returns true is successfull
bool CompileSyntaxFile(syn::SyntaxFile &syntaxFile, syn::SyntaxContext &context,
                       SyntaxListener *listener = nullptr);

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
    syn::SyntaxFile &syntaxFile,
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
    syn::ModuleDecl &&moduleDecl,
    TypeCheckerListener *listener =
        nullptr /*, TypeCheckWholeModuleCallback* callback = nullptr*/);

/// Returns true is successfull
void SerializeSyntaxFile(syn::SyntaxFile &syntaxFile);

/// Returns true is successfull
void SerializeModule(syn::ModuleDecl &moduleDecl);

/// GenIR for the ModuleFile
/// Returns true is successfull
void GenSyntaxFileIR(syn::SyntaxFile *syntaxFile,
                     CodeGenListener *listener = nullptr);

/// Gen IR for the Module
/// Returns true is successfull
void GenModuleIR(syn::ModuleDecl *moduleDecl,
                 CodeGenListener *listener = nullptr);

} // namespace stone

// Code generation
namespace stone {
/// Returns true is successfull
bool CompileBackend(CodeGenContext &codeGenConext,
                    syn::SyntaxContext &syntaxContext,
                    CodeGenListener *listener = nullptr);

std::unique_ptr<llvm::TargetMachine>
CreateTargetMachine(CodeGenContext &context);

// using TargetOptionsContext = std::tuple<llvm::TargetOptions, std::string,
//                                         std::vector<std::string>,
//                                         std::string>;
// TargetOptionsContext GetIRTargetOptions(const IRGenOptions &Opts,
//                                         ASTContext &Ctx);

// std::unique_ptr<llvm::TargetMachine>
// CreateTargetMachine(const CodeGenOptions &codeGenOpts,
//                     SyntaxContext &syntaxCotext);

void OptimizeIR(llvm::Module *mod, const CodeGenOptions &opts, llvm::TargetMachine *target);

/// Returns true is successfull
// bool GenNative(llvm::Module *mod, syn::SyntaxContext &context,
//                const CodeGenOptions &opts llvm::StringRef outputFilename,
//                CodeGenListener *listener = nullptr);

// /// Returns true is successfull
// bool WriteNative(llvm::Module *mod, llvm::TargetMachine *targetMachine,
//                  const CodeGenOptions &opts, StatisticEngine *stats,
//                  DiagnosticEngine &diags, llvm::raw_pwrite_stream &out,
//                  llvm::sys::Mutex *diagMutex = nullptr);

} // namespace stone
#endif
