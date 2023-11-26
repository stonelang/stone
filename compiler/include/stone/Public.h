#ifndef STONE_PUBLIC_H
#define STONE_PUBLIC_H

#include "stone/Basic/CodeGenOptions.h"
#include "stone/Basic/FileMgr.h"
#include "stone/Basic/LangOptions.h"
#include "stone/Diag/DiagnosticEngine.h"

#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/StringSet.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/Mutex.h"
#include "llvm/Target/TargetMachine.h"

namespace stone {
class SyntaxListener;
class CodeGenOptions;
class CodeGenContext;
class CodeGenScope;
class TargetContext;
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
class SourceFile;
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
  virtual void OnParseSourceFile(SourceFile *sf) {}
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
  virtual void OnSourceFileTypeChecked(SourceFile *sourceFile) {}
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
using ModuleOrSourceFile = llvm::PointerUnion<ModuleDecl *, SourceFile *>;

using ModuleDeclOrModuleFile = llvm::PointerUnion<ModuleDecl *, ModuleFile *>;

/// Compile a single input file
bool CompileInputFile(const CompilerInputFile &inputFile, Compiler &instance);

/// Parse, type-check, resolve imports, and generate IR for the SourceFile.
//  This will allows for parallelization specially when you are just in parsing
//  mode.
/// Returns true if successfull
bool CompileSourceFile(SourceFile &sourceFile, Compiler &instance,
                       CodeGenContext *cgc = nullptr);

/// This walks the syntax to resolve imports.
/// Returns true is successfull
void ParseSourceFile(SourceFile &sourceFile, ASTContext &context,
                     SyntaxListener *syntaxListener,
                     LexerListener *lexerListener);

/// Dump the source file that we parsed
void DumpSourceFile(SourceFile &sourceFile, ASTContext &astContext);

/// This walks the syntax to resolve imports.
/// Returns true is successfull
void ResolveSourceFileImports(SourceFile &sourceFile);

/// Once import resolution is complete, this walks the syntax to resolve types
/// and diagnose problems therein.
/// Returns true is successfull
void TypeCheckSourceFile(
    SourceFile &sourceFile, TypeCheckerOptions &opts,
    TypeCheckerListener *listener =
        nullptr /*, TypeCheckSourceFileCallback* callback = nullptr*/);

/// Pretty print the type checked source file.
void PrintSourceFile(SourceFile &sourceFile, ASTContext &astContext);

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
std::unqiue_ptr<IRCodeGenResult> GenIR(IRCodeGenInvocation invocation);

// IRCodeGenOutput GenIRInParallel(ParallelCodeGenContext);

bool EmitImportedModules(ASTContext &context, ModuleDecl *mainModule,
                         const CompilerOptions &opts);

} // namespace stone

// Code generation
namespace stone {

/// Disable thumb-mode until debugger support is there.
bool ShouldRemoveTargetFeature(llvm::StringRef feature);

std::unique_ptr<llvm::TargetMachine>
CreateTargetMachine(const CodeGenOptions &codeGenOpts);

void OptimizeIR(llvm::Module *mod, const CodeGenOptions &opts,
                llvm::TargetMachine *target, DiagnosticEngine &diags);

/// Returns true is successfull
// You want IRCodeGenOutput
bool GenNative(CodeGenContext &cgc, llvm::StringRef outputFilename,
               CodeGenListener *listener = nullptr);

bool GenNative(CodeGenContext &cgc, llvm::StringRef outputFilename,
               llvm::sys::Mutex *diagMutex, llvm::GlobalVariable *hashGlobal,
               CodeGenListener *listener = nullptr);

bool WriteEmptyOutputFiles(std::vector<std::string> &parallelOutputFilenames,
                           const ASTContext &Context,
                           const CodeGenOptions &opts);

/// Returns true is successfull
bool WriteNative(CodeGenContext &cgc, llvm::raw_pwrite_stream &out,
                 llvm::sys::Mutex *diagMutex = nullptr,
                 CodeGenScope *parentScope = nullptr);

} // namespace stone
#endif
