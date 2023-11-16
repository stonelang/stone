#ifndef STONE_COMPILE_COMPILERCOMMANDLINE_H
#define STONE_COMPILE_COMPILERCOMMANDLINE_H

#include "stone/Basic/CodeGenOptions.h"
#include "stone/Basic/FileSystemOptions.h"
#include "stone/Basic/LangOptions.h"
#include "stone/Basic/TargetContext.h"
#include "stone/Basic/ModuleOptions.h"
#include "stone/Compile/CompilerOptions.h"
#include "stone/Diag/DiagnosticEngine.h"
#include "stone/Diag/DiagnosticOptions.h"
#include "stone/Syntax/ASTOptions.h"
#include "stone/Syntax/TypeCheckerOptions.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/SetVector.h"
#include "llvm/Option/ArgList.h"

#include <deque>
#include <queue>

namespace llvm {
class raw_pwrite_stream;
class GlobalVariable;
class MemoryBuffer;
class Module;
class TargetOptions;
class TargetMachine;
} // namespace llvm

namespace stone {

using ConfigurationFileBuffers =
    llvm::SmallVector<std::unique_ptr<llvm::MemoryBuffer>, 4>;

struct ModuleBuffers {

  std::unique_ptr<llvm::MemoryBuffer> moduleBuffer;
  std::unique_ptr<llvm::MemoryBuffer> moduleDocBuffer;
  std::unique_ptr<llvm::MemoryBuffer> moduleSourceInfoBuffer;

  // Constructor
  ModuleBuffers(
      std::unique_ptr<llvm::MemoryBuffer> moduleBuffer,
      std::unique_ptr<llvm::MemoryBuffer> moduleDocBuffer = nullptr,
      std::unique_ptr<llvm::MemoryBuffer> moduleSourceInfoBuffer = nullptr)
      : moduleBuffer(std::move(moduleBuffer)),
        moduleDocBuffer(std::move(moduleDocBuffer)),
        moduleSourceInfoBuffer(std::move(moduleSourceInfoBuffer)) {}
};

using MemoryBuffers =
    llvm::SmallVectorImpl<std::unique_ptr<llvm::MemoryBuffer>>;

class CompilerInvocation final {

  CompilerOptions compilerOpts;

  /// Options for generating code
  CodeGenOptions codeGenOpts;

  /// The options for searching libs
  SearchPathOptions searchPathOpts;

  /// The options for type-checking
  TypeCheckerOptions typeCheckerOpts;

  LangOptions langOpts;

  TargetContext targetContext;

  ModuleOptions moduleOpts;

  ASTOptions syntaxOpts;

  DiagnosticOptions diagOpts;

  SrcMgr srcMgr;
  DiagnosticEngine diags{srcMgr};

public:
  CompilerInvocation();

public:
  void SetTargetTriple(llvm::StringRef triple);
  void SetTargetTriple(const llvm::Triple &Triple);
  void SetMainExecutable(const char *arg0, void *mainAddr);
  void SetSDKPath(const std::string &sdkPath);
  void AddDiagnosticConsumer(DiagnosticConsumer &consumer) {
    diags.AddConsumer(consumer);
  }
  // llvm::StringRef GetSDKPath() const { return searchPathOpts.GetSDKPath(); }

public:
  CompilerOptions &GetCompilerOptions() { return compilerOpts; }
  const CompilerOptions &GetCompilerOptions() const { return compilerOpts; }

  CodeGenOptions &GetCodeGenOptions() { return codeGenOpts; }
  const CodeGenOptions &GetCodeGenOptions() const { return codeGenOpts; }

  stone::TargetOptions &GetTargetOptions() { return targetOpts; }
  const stone::TargetOptions &GetTargetOptions() const { return targetOpts; }

  LangOptions &GetLangOptions() { return langOpts; }
  const LangOptions &GetLangOptions() const { return langOpts; }

  ASTOptions &GetASTOptions() { return syntaxOpts; }
  const ASTOptions &GetASTOptions() const { return syntaxOpts; }

  TypeCheckerOptions &GetTypeCheckerOptions() { return typeCheckerOpts; }
  const TypeCheckerOptions &GetTypeCheckerOptions() const {
    return typeCheckerOpts;
  }
  SearchPathOptions &GetSearchPathOptions() { return searchPathOpts; }
  const SearchPathOptions &GetSearchPathOptions() const {
    return searchPathOpts;
  }

  DiagnosticOptions &GetDiagnosticOptions() { return diagOpts; }
  const DiagnosticOptions &GetDiagnosticOptions() const { return diagOpts; }

  DiagnosticEngine &GetDiags() { return diags; }
  bool HasError() { return diags.HasError(); }

  SrcMgr &GetSrcMgr() { return srcMgr; }

  ModuleOptions &GetModuleOptions() { return moduleOpts; }
  const ModuleOptions &GetModuleOptions() const { return moduleOpts; }

  opt::Options &GetOptions() { return options; }

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
  template <typename... ArgTypes>
  InFlightDiagnostic
  PrintD(Diag<ArgTypes...> id,
         typename detail::PassArgument<ArgTypes>::type... args) {
    return GetDiags().PrintD(SrcLoc(), id, std::forward<ArgTypes>(args)...);
  }

public:
  Status ParseArgs(llvm::ArrayRef<const char *> args);
};

} // namespace stone

#endif
