#ifndef STONE_PUBLIC_H
#define STONE_PUBLIC_H

#include "stone/Basic/CodeGenOptions.h"
#include "stone/Basic/FileMgr.h"
#include "stone/Basic/FileSystemOptions.h"
#include "stone/Basic/LangOptions.h"
#include "stone/Basic/SrcMgr.h"
#include "stone/Basic/StatisticEngine.h"
#include "stone/Basic/Status.h"
#include "stone/Diag/DiagnosticEngine.h"

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
class Compiler;
class TypeCheckerListener;
class CodeGenListener;
class TypeCheckerOptions;
class CodeGenListener;
class PrimaryFileSpecificPaths;
class CompilerOptions;
class InFlightDiagnostic;
class Compiler;
class CompilerConfiguration;
class ClangContext;

namespace syn {
class SyntaxContext;
class ModuleDecl;
class Decl;
class Stmt;
class Expr;
class Module;
class Token;
class SyntaxFile;
} // namespace syn
} // namespace stone

namespace stone {
class CodeCompletionListener {
public:
  CodeCompletionListener() = default;
  virtual ~CodeCompletionListener() = default;
};

class LexerListener : public CodeCompletionListener {
public:
  LexerListener() = default;
  virtual ~LexerListener() = default;

public:
  virtual void OnToken(const syn::Token *token) {}
};

class SyntaxListener : public CodeCompletionListener {
public:
  SyntaxListener() = default;
  virtual ~SyntaxListener() = default;

public:
  virtual void OnDone() {}
  virtual void OnError() {}

public:
  virtual void OnDecl(const syn::Decl *decl, bool isTopLevel = false);
  virtual void OnStmt(const syn::Stmt *stmt);
  virtual void OnExpr(const syn::Expr *expr);

public:
  virtual void OnParseError() {}
  virtual void OnParseStarted() {}
  virtual void OnParseSyntaxFile(syn::SyntaxFile *sf) {}
  virtual void OnParseCompleted() {}
};

class TypeCheckerListener : public CodeCompletionListener {
public:
  TypeCheckerListener() = default;
  virtual ~TypeCheckerListener() = default;

public:
  virtual void OnDeclTypeChecked(syn::Decl *decl, bool isTopLvel = false);
  virtual void OnStmtTypeChecked(syn::Stmt *stmt) {}
  virtual void OnExprTypeChecked(syn::Expr *expr) {}

public:
  virtual void OnTypeCheckError();
  virtual void OnSyntaxFileTypeChecked(syn::SyntaxFile *syntaxFile) {}
  virtual void OnModuleTypeChecked(syn::Module *mod) {}

public:
};

class CodeGenListener : public CodeCompletionListener {
public:
  CodeGenListener() = default;
  virtual ~CodeGenListener() = default;

public:
  virtual void OnEmitIRError() {}
  virtual void OnEmitIRCompleted(llvm::Module *m) {}
  virtual void OnEmitObjectError() {}
  virtual void OnEmitObject() {}
  virtual void OnEmitObjectCompleted() {}
  virtual void OnEmitBitCodeError() {}
  virtual void OnEmitBitCode() {}
  virtual void OnEmitBitCodeCompleted() {}
  virtual void OnEmitModuleError() {}
  virtual void OnEmitModule() {}
  virtual void OnEmitModuleCompleted() {}
  virtual void OnEmitLibraryError() {}
  virtual void OnEmitLibrary() {}
  virtual void OnEmitLibraryCompleted() {}
};

class CompilerListener final {
  LexerListener *lexerListener = nullptr;
  SyntaxListener *syntaxListener = nullptr;
  TypeCheckerListener *typeCheckerListener = nullptr;
  CodeGenListener *codeGenListener = nullptr;

public:
  CompilerListener() = default;
  virtual ~CompilerListener() = default;

public:
  void SetLexerListener(LexerListener *listener) { lexerListener = listener; }
  LexerListener *GetLexerListener() { return lexerListener; }

  void SetSyntaxListener(SyntaxListener *listener) {
    syntaxListener = listener;
  }
  SyntaxListener *GetSyntaxListener() { return syntaxListener; }

  void SetTypeCheckerListener(TypeCheckerListener *listener) {
    typeCheckerListener = listener;
  }
  TypeCheckerListener *GetTypeCheckerListener() { return typeCheckerListener; }

  void SetCodeGenListener(CodeGenListener *listener) {
    codeGenListener = listener;
  }
  CodeGenListener *GetCodeGenListener() { return codeGenListener; }

public:
  virtual void OnCompileConfigured(CompilerConfiguration &invocation) {}
  virtual void OnCompileStarted(Compiler &instance) {}
  virtual void OnSyntaxAnalysisCompleted(Compiler &instance) {}
  virtual void OnSemanticAnalysisCompleted(Compiler &instance) {}
  virtual void OnCodeGenCompleted(Compiler &instance) {}
  virtual void OnCompileCompleted(Compiler &instance) {}
};
} // namespace stone

namespace stone {
class LangContext final {
  FileMgr fm;
  SrcMgr sm;
  StatisticEngine se;
  ColorStream cos;
  FileSystemOptions fsOpts;
  DiagnosticEngine de;
  DiagnosticOptions diagOpts;
  LangOptions langOpts;

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

  DiagnosticEngine &GetDiags() { return de; }
  const DiagnosticEngine &GetDiags() const { return de; }

  DiagnosticOptions &GetDiagOptions() { return diagOpts; }
  const DiagnosticOptions &GetDiagOptions() const { return diagOpts; }

  bool HasError() { return de.HasError(); }

  FileMgr &GetFileMgr() { return fm; }
  SrcMgr &GetSrcMgr() { return sm; }

public:
  InFlightDiagnostic PrintD(const Diagnostic &diagnostic) {
    return PrintD(SrcLoc(), diagnostic);
  }
  InFlightDiagnostic PrintD(SrcLoc loc, const Diagnostic &diagnostic) {
    return GetDiags().PrintD(loc, diagnostic);
  }

  InFlightDiagnostic PrintD(DiagID diagID,
                            llvm::ArrayRef<diag::Argument> args) {
    return PrintD(SrcLoc(), diagID, args);
  }
  InFlightDiagnostic PrintD(SrcLoc loc, DiagID diagID,
                            llvm::ArrayRef<diag::Argument> args) {
    return GetDiags().PrintD(loc, diagID, args);
  }

  InFlightDiagnostic PrintD(DiagID diagID) { return PrintD(SrcLoc(), diagID); }

  InFlightDiagnostic PrintD(SrcLoc loc, DiagID diagID) {
    return GetDiags().PrintD(loc, diagID);
  }
  template <typename... ArgTypes>
  InFlightDiagnostic
  PrintD(SrcLoc loc, Diag<ArgTypes...> id,
         typename detail::PassArgument<ArgTypes>::type... args) {
    return GetDiags().PrintD(loc, id, std::forward<ArgTypes>(args)...);
  }
};
} // namespace stone

// Parsing and type-checking
namespace stone {
class CompilerListener;
int Compile(llvm::ArrayRef<const char *> args, const char *arg0, void *mainAddr,
            CompilerListener *listener = nullptr);

} // namespace stone

namespace stone {
using ModuleSyntaxFileUnion =
    llvm::PointerUnion<syn::ModuleDecl *, syn::SyntaxFile *>;

/// Parse, type-check, resolve imports, and generate IR for the SyntaxFile.
//  This will allows for parallelization specially when you are just in parsing
//  mode.
/// Returns true is successfull
bool CompileSyntaxFile(syn::SyntaxFile &syntaxFile, Compiler &instance,
                       CodeGenContext *cgc = nullptr);

/// This walks the syntax to resolve imports.
/// Returns true is successfull
void ParseSyntaxFile(syn::SyntaxFile &syntaxFile, syn::SyntaxContext &context,
                     SyntaxListener *syntaxListener,
                     LexerListener *lexerListener);

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
