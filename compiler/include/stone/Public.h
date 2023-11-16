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
class TypeCheckerListener;
class CodeGenListener;
class TypeCheckerOptions;
class CodeGenListener;
class PrimaryFileSpecificPaths;
class CompilerOptions;
class InFlightDiagnostic;
class Compiler;
class CompilerTask;
class CompilerInvocation;
class ClangContext;
class ASTContext;
class ModuleDecl;
class Decl;
class Stmt;
class Expr;
class Token;
class ASTFile;
class CompilerInputFile;

} // namespace stone

namespace stone {

class LexerListener {
public:
  LexerListener() = default;
  virtual ~LexerListener() = default;

public:
  virtual void OnToken(const Token *token) {}
};

class SyntaxListener {
public:
  SyntaxListener() = default;
  virtual ~SyntaxListener() = default;

public:
  virtual void OnDone() {}
  virtual void OnError() {}

public:
  virtual void OnDecl(const Decl *decl, bool isTopLevel = false);
  virtual void OnStmt(const Stmt *stmt);
  virtual void OnExpr(const Expr *expr);

public:
  virtual void OnParseError() {}
  virtual void OnParseStarted() {}
  virtual void OnParseASTFile(ASTFile *sf) {}
  virtual void OnParseCompleted() {}
};

class TypeCheckerListener {
public:
  TypeCheckerListener() = default;
  virtual ~TypeCheckerListener() = default;

public:
  virtual void OnDeclTypeChecked(Decl *decl, bool isTopLvel = false);
  virtual void OnStmtTypeChecked(Stmt *stmt) {}
  virtual void OnExprTypeChecked(Expr *expr) {}

public:
  virtual void OnTypeCheckError();
  virtual void OnASTFileTypeChecked(ASTFile *astFile) {}
  virtual void OnModuleTypeChecked(ModuleDecl *mod) {}

public:
};

class CodeGenListener {
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
  virtual void CompletedCommandLineParsing(CompilerInvocation &invocation) {}
  virtual void CompletedConfiguration(Compiler &compiler) {}
  virtual void CompletedBuildingTasks(Compiler &compiler) {}

  virtual void ExecutingCompilerTasks(Compiler &compiler, CompilerTask *task) {}
  virtual void FinishedCompilerTasks(Compiler &compiler, CompilerTask *task) {}

  virtual void CompletedSyntaxAnalysis(Compiler &compiler) {}
  virtual void CompletedSemanticAnalysis(Compiler &compiler) {}
  virtual void CompletedCodeGeneration(Compiler &compiler) {}
  virtual void CompletedRunningTasks(Compiler &compiler) {}
};
} // namespace stone

// Parsing and type-checking
namespace stone {
using ModuleOrASTFile = llvm::PointerUnion<ModuleDecl *, ASTFile *>;

/// Compile a single input file
bool CompileInputFile(const CompilerInputFile &inputFile, Compiler &instance);

/// Parse, type-check, resolve imports, and generate IR for the ASTFile.
//  This will allows for parallelization specially when you are just in parsing
//  mode.
/// Returns true if successfull
bool CompileASTFile(ASTFile &astFile, Compiler &instance,
                    CodeGenContext *cgc = nullptr);

/// This walks the syntax to resolve imports.
/// Returns true is successfull
void ParseASTFile(ASTFile &astFile, ASTContext &context,
                  SyntaxListener *syntaxListener, LexerListener *lexerListener);

/// This walks the syntax to resolve imports.
/// Returns true is successfull
void ResolveASTFileImports(ASTFile &astFile);

/// Once import resolution is complete, this walks the syntax to resolve types
/// and diagnose problems therein.
/// Returns true is successfull
void TypeCheckASTFile(
    ASTFile &astFile, TypeCheckerOptions &opts,
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
    ModuleDecl &moduleDecl, TypeCheckerOptions &opts,
    TypeCheckerListener *listener =
        nullptr /*, TypeCheckWholeModuleCallback* callback = nullptr*/);

/// Returns true is successfull
void SerializeASTFile(ASTFile &astFile);

/// Returns true is successfull
void SerializeModuleDecl(ModuleDecl &moduleDecl);

/// GenIR for the ModuleFile
/// Returns true is successfull
void GenASTFileIR(CodeGenContext &cgc, llvm::StringRef moduleName, ASTFile *sf,
                  const PrimaryFileSpecificPaths specificPaths,
                  CodeGenListener *listener = nullptr);

/// Gen IR for the entire Module
/// Returns true is successfull
void GenWholeModuleIR(CodeGenContext &cgc, llvm::StringRef moduleName,
                      ModuleDecl *mod,
                      const PrimaryFileSpecificPaths specificPaths,
                      CodeGenListener *listener = nullptr);

bool EmitImportedModules(ASTContext &context, ModuleDecl *mainModule,
                         const CompilerOptions &opts);

} // namespace stone

// Code generation
namespace stone {

IRTargetOptions GetIRTargetOptions(const CodeGenOptions &opts,
                                   const LangOptions &langOpts,
                                   ClangContext &cc);

/// Disable thumb-mode until debugger support is there.
bool ShouldRemoveTargetFeature(llvm::StringRef feature);

std::unique_ptr<llvm::TargetMachine>
CreateTargetMachine(const CodeGenOptions &opts);

void OptimizeIR(llvm::Module *mod, const CodeGenOptions &opts,
                llvm::TargetMachine *target, DiagnosticEngine &diags);

/// Returns true is successfull
bool GenNative(CodeGenContext &cgc, ASTContext &context,
               llvm::StringRef outputFilename,
               CodeGenListener *listener = nullptr);

bool WriteEmptyOutputFiles(std::vector<std::string> &parallelOutputFilenames,
                           const ASTContext &Context,
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

bool GenNative(CodeGenContext &cgc, ASTContext &context,
               llvm::StringRef outputFilename, llvm::sys::Mutex *diagMutex,
               llvm::GlobalVariable *hashGlobal,
               CodeGenListener *listener = nullptr);

/// Returns true is successfull
void WriteNative(CodeGenContext &cgc, llvm::raw_pwrite_stream &out,
                 llvm::sys::Mutex *diagMutex = nullptr,
                 CodeGenScope *parentScope = nullptr);

} // namespace stone
#endif
