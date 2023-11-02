#ifndef STONE_PUBLIC_H
#define STONE_PUBLIC_H

#include "stone/Basic/CodeGenOptions.h"
#include "stone/Basic/FileMgr.h"
#include "stone/Basic/FileSystemOptions.h"
#include "stone/Basic/LangOptions.h"
#include "stone/Basic/SrcMgr.h"
#include "stone/Basic/StatisticEngine.h"
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

} // namespace stone

namespace stone {
namespace codegen {
class CodeGenContext;
}
} // namespace stone

namespace stone {
namespace ast {
class ASTContext;
class ASTFile;
class ModuleDecl;
} // namespace ast
} // namespace stone

namespace stone {

using ModuleOrASTFile = llvm::PointerUnion<ast::ModuleDecl *, ast::ASTFile *>;

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

  static int Main(llvm::ArrayRef<const char *> args, const char *arg0,
                  void *mainAddr, CompilationListener *listener);

public:
  /// This walks the asttax to resolve imports.
  /// Returns true is successfull
  static void ParseASTFile(ast::ASTFile &astFile, ast::ASTContext &context,
                           ASTListener *listener = nullptr);

  /// This walks the asttax to resolve imports.
  /// Returns true is successfull
  static void ResolveASTFileImports(ast::ASTFile &astFile);

  static void DumpAST(ast::ASTFile &astFile, ast::ASTContext &context);

  static void PrintAST(ast::ASTFile &astFile, ast::ASTContext &context);

  /// Once import resolution is complete, this walks the asttax to resolve types
  /// and diagnose problems therein.
  /// Returns true is successfull
  static void TypeCheckASTFile(
      ast::ASTFile &astFile, TypeCheckerOptions &opts,
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
      ast::ModuleDecl &moduleDecl, TypeCheckerOptions &opts,
      TypeCheckerListener *listener =
          nullptr /*, TypeCheckWholeModuleCallback* callback = nullptr*/);

  /// Returns true is successfull
  static void SerializeASTFile(ast::ASTFile &asttaxFile);

  /// Returns true is successfull
  static void SerializeModuleDecl(ast::ModuleDecl &moduleDecl);

  /// GenIR just for an ASTFile
  static void GenIR(codegen::CodeGenContext &cgc, llvm::StringRef moduleName,
                    ast::ASTFile *sf,
                    const PrimaryFileSpecificPaths specificPaths,
                    CodeGenListener *listener = nullptr);

  /// GenIR for the entire module
  static void GenIR(codegen::CodeGenContext &cgc, llvm::StringRef moduleName,
                    ast::ModuleDecl *mod,
                    const PrimaryFileSpecificPaths specificPaths,
                    CodeGenListener *listener = nullptr);

  static bool EmitImportedModules(ast::ASTContext &context,
                                  ast::ModuleDecl *mainModule,
                                  const CompilerOptions &opts);

public:
  static IRTargetOptions GetIRTargetOptions(const CodeGenOptions &opts,
                                            const LangOptions &langOpts,
                                            Clang &cc);

  /// Disable thumb-mode until debugger support is there.
  static bool ShouldRemoveTargetFeature(llvm::StringRef feature);

  static std::unique_ptr<llvm::TargetMachine>
  CreateTargetMachine(const CodeGenOptions &opts, ast::ASTContext &ac);

  static void OptimizeIR(codegen::CodeGenContext &cgc);

  static bool
  WriteEmptyOutputFiles(std::vector<std::string> &parallelOutputFilenames,
                        const ast::ASTContext &Context,
                        const CodeGenOptions &opts);

  /// Returns true is successfull
  static bool GenNative(codegen::CodeGenContext &cgc,
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
  static bool GenNative(codegen::CodeGenContext &cgc,
                        llvm::StringRef outputFilename,
                        llvm::sys::Mutex *diagMutex,
                        llvm::GlobalVariable *hashGlobal,
                        CodeGenListener *listener = nullptr);

  /// Returns true is successfull
  static void WriteNative(codegen::CodeGenContext &cgc,
                          llvm::raw_pwrite_stream &out,
                          llvm::sys::Mutex *diagMutex = nullptr);
};
} // namespace stone
#endif
