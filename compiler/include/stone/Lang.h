#ifndef STONE_LANG_H
#define STONE_LANG_H

#include "stone/Basic/CodeGenOptions.h"
#include "stone/Basic/DiagnosticEngine.h"
#include "stone/Basic/FileMgr.h"
#include "stone/Basic/FileSystemOptions.h"
#include "stone/Basic/LangOptions.h"
#include "stone/Basic/SrcMgr.h"
#include "stone/Basic/StatisticEngine.h"

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
class Clang;
class ASTListener;
class CompilerListener;
class CodeGenListener;
class TypeCheckerListener;
class CompilationListener;
class CodeGenOptions;
class TargetOptions;
class LangOptions;
class DiagnosticEngine;
class CompilerInstance;
class TypeCheckerOptions;
class PrimaryFileSpecificPaths;
class CompilerOptions;
class CodeGenContext;
class ASTContext;
class ASTFile;
class ModuleDecl;

using ModuleOrASTFile =
    llvm::PointerUnion<stone::ModuleDecl *, stone::ASTFile *>;

class Lang final {
  FileMgr fm;
  SrcMgr sm;
  diag::DiagnosticOptions diagOpts;
  DiagnosticEngine de;
  LangOptions langOpts;
  StatisticEngine se;
  ColorStream cos;
  FileSystemOptions fsOpts;

public:
  Lang() : fm(fsOpts), de(diagOpts, sm), cos(llvm::outs()) {}
  ~Lang() {}

public:
  ColorStream &Out() { return cos; }
  StatisticEngine &GetStats() { return se; }
  const StatisticEngine &GeStats() const { return se; }

  LangOptions &GetLangOptions() { return langOpts; }
  const LangOptions &GetLangOptions() const { return langOpts; }

  FileSystemOptions &GetFileSystemOptions() { return fsOpts; }
  const FileSystemOptions &GetFileSystemOptions() const { return fsOpts; }

  DiagnosticEngine &GetDiags() { return de; }

  FileMgr &GetFileMgr() { return fm; }
  SrcMgr &GetSrcMgr() { return sm; }

public:
  static int Compile(llvm::ArrayRef<const char *> args, const char *arg0,
                     void *mainAddr, CompilerListener *listener = nullptr);

  //// Executes only the analysis part of the compiler
  static bool CompileWithCodeAnalysis(CompilerInstance &compiler);

  /// Performs non-analysis stuff and then calls code generation
  static bool CompileAfterCodeAnalysis(CompilerInstance &compiler);

  /// Execute only the code generation
  static bool CompileWithCodeGeneration(CompilerInstance &compiler);

public:
  static int Main(llvm::ArrayRef<const char *> args, const char *arg0,
                  void *mainAddr, CompilationListener *listener);

public:
  /// This walks the asttax to resolve imports.
  /// Returns true is successfull
  static void ParseASTFile(stone::ASTFile &astFile, stone::ASTContext &context,
                           ASTListener *listener = nullptr);

  /// This walks the asttax to resolve imports.
  /// Returns true is successfull
  static void ResolveASTFileImports(stone::ASTFile &astFile);

  /// Just dump the AST created
  static void DumpAST(stone::ASTFile &astFile, stone::ASTContext &context);

  /// Once import resolution is complete, this walks the asttax to resolve types
  /// and diagnose problems therein.
  /// Returns true is successfull
  static void TypeCheckASTFile(
      stone::ASTFile &astFile, TypeCheckerOptions &opts,
      TypeCheckerListener *listener =
          nullptr /*, TypeCheckASTFileCallback* callback = nullptr*/);

  /// Now that we have type-checked an entire module, perform any type
  /// checking that requires the full module.
  ///
  /// Note that clients still perform this checking file-by-file to
  /// provide a somewhat defined order in which diagnostics should be
  /// emitted.
  /// Returns true is successfull
  static void TypeCheckWholeModule(
      stone::ModuleDecl &moduleDecl, TypeCheckerOptions &opts,
      TypeCheckerListener *listener =
          nullptr /*, TypeCheckWholeModuleCallback* callback = nullptr*/);

  /// Dump the AST created after type-checking
  static void PrintAST(stone::ASTFile &astFile, stone::ASTContext &context);

  /// Returns true is successfull
  static void SerializeASTFile(stone::ASTFile &asttaxFile);

  /// Returns true is successfull
  static void SerializeModuleDecl(stone::ModuleDecl &moduleDecl);

  /// GenIR just for an ASTFile
  static void GenIR(stone::CodeGenContext &cgc, llvm::StringRef moduleName,
                    stone::ASTFile *sf,
                    const PrimaryFileSpecificPaths specificPaths,
                    CodeGenListener *listener = nullptr);

  /// GenIR for the entire module
  static void GenIR(stone::CodeGenContext &cgc, llvm::StringRef moduleName,
                    stone::ModuleDecl *mod,
                    const PrimaryFileSpecificPaths specificPaths,
                    CodeGenListener *listener = nullptr);

  /// Emit the IR generated to the output screen
  static void EmitIR();

  static bool EmitImportedModules(stone::ASTContext &context,
                                  stone::ModuleDecl *mainModule,
                                  const CompilerOptions &opts);

public:
  static IRTargetOptions GetIRTargetOptions(const CodeGenOptions &opts,
                                            const LangOptions &langOpts,
                                            Clang &cc);

  /// Disable thumb-mode until debugger support is there.
  static bool ShouldRemoveTargetFeature(llvm::StringRef feature);

  static std::unique_ptr<llvm::TargetMachine>
  CreateTargetMachine(const CodeGenOptions &opts, stone::ASTContext &ac);

  static void OptimizeIR(stone::CodeGenContext &cgc);

  static bool
  WriteEmptyOutputFiles(std::vector<std::string> &parallelOutputFilenames,
                        const stone::ASTContext &Context,
                        const CodeGenOptions &opts);

  /// Returns true is successfull
  static bool GenNative(stone::CodeGenContext &cgc,
                        llvm::StringRef outputFilename,
                        CodeGenListener *listener = nullptr);

  /// Run the LLVM passes. In multi-threaded compilation this will be done for
  /// multiple LLVM modules in parallel.
  /// \param Diags The Diagnostic Engine.
  /// \param DiagMutex in contexts that require parallel codegen, a mutex that
  /// the
  ///                  diagnostic engine uses to astchronize emission.
  /// \param HashGlobal used with incremental LLVMCodeGen to know if a module
  ///                   was already compiled, may be null if not desired.
  /// \param Module LLVM module to code gen, required.
  /// \param TargetMachine target of code gen, required.
  /// \param OutputFilename Filename for output.
  static bool GenNative(stone::CodeGenContext &cgc,
                        llvm::StringRef outputFilename,
                        llvm::sys::Mutex *diagMutex,
                        llvm::GlobalVariable *hashGlobal,
                        CodeGenListener *listener = nullptr);

  /// Returns true is successfull
  static void WriteNative(stone::CodeGenContext &cgc,
                          llvm::raw_pwrite_stream &out,
                          llvm::sys::Mutex *diagMutex = nullptr);
};
} // namespace stone
#endif
