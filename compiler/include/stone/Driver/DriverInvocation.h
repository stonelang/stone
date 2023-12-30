#ifndef STONE_DRIVER_DRIVER_INVOCATION_H
#define STONE_DRIVER_DRIVER_INVOCATION_H

#include "stone/Basic/CodeGenOptions.h"
#include "stone/Basic/FileMgr.h"
#include "stone/Basic/FileSystemOptions.h"
#include "stone/Basic/LangOptions.h"
#include "stone/Basic/STDAlias.h"
#include "stone/Basic/Status.h"
#include "stone/Diag/DiagnosticEngine.h"
#include "stone/Diag/DiagnosticOptions.h"
#include "stone/Diag/DriverDiagnostic.h"
#include "stone/Driver/DriverOptions.h"
#include "stone/Option/Action.h"
#include "stone/Option/Options.h"

#include "llvm/Option/Arg.h"
#include "llvm/Option/ArgList.h"

using namespace llvm::opt;

namespace stone {

// class JobOutput;

// class DriverInvocation final {

//   SrcMgr srcMgr;
//   DiagnosticEngine diags{srcMgr};

//   DriverOptions driverOpts;
//   DriverOptions driverOpts;
//   DiagnosticOptions diagOpts;
//   LangOptions langOpts;
//   FileSystemOptions fileSystemOpts;

//   std::unique_ptr<llvm::opt::OptTable> optTable;
//   std::unique_ptr<llvm::opt::InputArgList> inputArgList;
//   std::unique_ptr<llvm::opt::DerivedArgList> derivedArgList;

// public:
//   DriverInvocation();

// public:
//   DriverOptions &GetDriverOptions() { return driverOpts; }
//   const DriverOptions &GetDriverOptions() const { return driverOpts; }

//   bool HasAction() { return !driverOpts.mainAction.IsAlien(); }
//   Action &GetAction() { return driverOpts.mainAction; }
//   const Action &GetAction() const { return driverOpts.mainAction; }
//   ToolChainKind GetToolChainKind() const { return driverOpts.toolChainKind; }

// public:
//   DriverOptions &GetDriverOptions() { return driverOpts; }
//   const DriverOptions &GetDriverOptions() const {
//     return driverOpts;
//   }
//   LinkMode GetLinkMode() const { return driverOpts.linkMode; }
//   CompilationKind GetCompilationKind() const {
//     return driverOpts.compilationKind;
//   }

// public:
//   LangOptions &GetLangOptions() { return langOpts; }
//   const LangOptions &GetLangOptions() const { return langOpts; }

//   FileSystemOptions &GetFileSystemOptions() { return fileSystemOpts; }
//   const FileSystemOptions &GetFileSystemOptions() const {
//     return fileSystemOpts;
//   }
//   // void SetTargetTriple(llvm::StringRef triple);

//   void SetMainExecutablePath(llvm::StringRef mainExecutablePath) {
//     driverOpts.mainExecutablePath = mainExecutablePath;
//   }
//   void SetMainExecutableName(llvm::StringRef mainExecutableName) {
//     driverOpts.mainExecutablePath = mainExecutableName;
//   }

// public:
//   DiagnosticEngine &GetDiags() { return diags; }
//   DiagnosticOptions &GetDiagnosticOptions() { return diagOpts; }
//   const DiagnosticOptions &GetDiagnosticOptions() const { return diagOpts; }
//   void AddDiagnosticConsumer(DiagnosticConsumer &consumer) {
//     diags.AddConsumer(consumer);
//   }
//   void RemoveDiagnosticConsumer(DiagnosticConsumer &consumer) {
//     diags.RemoveConsumer(consumer);
//   }

// public:
//   llvm::opt::OptTable &GetOptTable() { return *optTable; }
//   const llvm::opt::OptTable &GetOptTable() const { return *optTable; }

//   llvm::opt::InputArgList &GetInputArgList() { return *inputArgList; }
//   llvm::opt::DerivedArgList &GetDerivedArgList() { return *derivedArgList; }

// public:
//   Status ParseArgs(llvm::ArrayRef<const char *> args);

// private:
//   Status TranslateInputArgList(const llvm::opt::InputArgList &inputArgList);

//   Status ParseDriverOptions(const llvm::opt::ArgList &argList);
//   Status ParseDriverOptions(const llvm::opt::ArgList &argList);

//   void ComputeLinkMode(const llvm::opt::ArgList &argList);
//   Status ParseToolChainKind(const llvm::opt::ArgList &argList);
//   Status ComputeCompilationKind(const llvm::opt::ArgList &argList);

//   /// Construct the list of inputs and their types from the given arguments.
//   ///
//   /// \param args The input arguments.
//   /// \param[out] Inputs The list in which to store the resulting compilation
//   /// inputs.
//   Status BuildInputFiles(const llvm::opt::ArgList &argList,
//                          InputFileList &inputFiles);

//   Status ParseOutputFileType(const llvm::opt::ArgList &argList);

// public:
//   void ForEachInputFile(std::function<void(InputFile &input)> callback);

//   bool ShouldCompile() const { return GetAction().CanCompile(); }
//   bool ShouldLink() const { return (GetLinkMode() != LinkMode::None); }

//   bool IsCompileOnly() const { return (ShouldCompile() && !ShouldLink()); }
//   bool IsLinkOnly() const { return (ShouldLink() && !ShouldCompile()); }

// public:
//   /// Might this sort of compile have explicit primary inputs?
//   /// When running a single compile for the whole module (in other words
//   /// "whole-module-optimization" mode) there must be no -primary-input's and
//   /// nothing in a (preferably non-existent) -primary-filelist. Left to its
//   own
//   /// devices, the driver would forget to omit the primary input files, so
//   /// return a flag here.
//   Status MightHaveExplicitPrimaryInputs(const JobOutput &jobOutput) const;
// };

} // namespace stone

#endif