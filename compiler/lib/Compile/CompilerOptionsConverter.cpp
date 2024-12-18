#include "stone/Compile/CompilerOptionsConverter.h"
#include "stone/AST/DiagnosticsCompile.h"
#include "stone/Strings.h"
// #include "stone/Basic/Platform.h"
#include "stone/Compile/CompilerInputsConverter.h"
#include "stone/Compile/CompilerOutputsConverter.h"
#include "stone/Support/Options.h"
// #include "stone/Option/SanitizerOptions.h"
#include "stone/Parse/Lexer.h"
// #include "stone/Strings.h"

// Used to validate module name
#include "stone/Compile/CompilerInstance.h"

#include "llvm/ADT/STLExtras.h"
#include "llvm/Option/Arg.h"
#include "llvm/Option/ArgList.h"
#include "llvm/Option/Option.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/LineIterator.h"
#include "llvm/Support/Path.h"
#include "llvm/TargetParser/Triple.h"

using namespace stone;
using namespace llvm::opt;

CompilerOptionsConverter::CompilerOptionsConverter(
    const llvm::opt::ArgList &args, DiagnosticEngine &de, LangOptions &langOpts,
    CompilerOptions &compilerOpts)
    : args(args), de(de), langOpts(langOpts), compilerOpts(compilerOpts) {}

Status CompilerOptionsConverter::Convert(
    llvm::SmallVectorImpl<std::unique_ptr<llvm::MemoryBuffer>> *buffers) {

  Status status;

  compilerOpts.primaryActionKind =
      CompilerOptionsConverter::ComputeActionKind(args);
  if (!compilerOpts.HasPrimaryAction()) {
    return Status::MakeHasCompletionAndIsError();
  }

  if (compilerOpts.IsImmediateAction()) {
    return Status::MakeHasCompletion();
  }
  // TODO: OK for now
  // assert(compilerOpts.inputsAndOutputs.HasInputs() &&
  //       "Inputs and Outputs should be empty");

  std::optional<CompilerInputsAndOutputs> inputsAndOutputs =
      CompilerInputsConverter(de, args, compilerOpts).Convert(buffers);

  if (!inputsAndOutputs) {
    /// TODO: Diagnose
    return Status::MakeHasCompletionAndIsError();
  }

  if (!inputsAndOutputs->HasInputs()) {
    return Status::MakeHasCompletionAndIsError();
  }

  // Make sure that it is empty,
  bool haveNewInputsAndOutputs = false;
  if (compilerOpts.GetInputsAndOutputs().HasInputs()) {
    assert(!inputsAndOutputs->HasInputs());
  } else {
    haveNewInputsAndOutputs = true;
    compilerOpts.GetInputsAndOutputs() = std::move(inputsAndOutputs).value();

    if (compilerOpts.allowModuleWithCompilerErrors) {
      compilerOpts.GetInputsAndOutputs().SetShouldRecoverMissingInputs();
    }
  }

  if (!compilerOpts.GetInputsAndOutputs().HasInputs()) {
    return Status::MakeHasCompletionAndIsError();
  }

  if (compilerOpts.GetInputsAndOutputs().ShouldTreatAsModuleInterface()) {
    compilerOpts.parsingInputMode =
        CompilerOptions::ParsingInputMode::StoneModuleInterface;
  } else if (args.hasArg(opts::OPT_ParseAsLibrary)) {
    compilerOpts.parsingInputMode =
        CompilerOptions::ParsingInputMode::StoneLibrary;
  } else {
    compilerOpts.parsingInputMode = CompilerOptions::ParsingInputMode::Stone;
  }

  if (ComputeModuleName().IsError()) {
    return Status::MakeHasCompletionAndIsError();
  }
  return Status();
}

CompilerActionKind
CompilerOptionsConverter::ComputeActionKind(const ArgList &args) {

  auto const mode = args.getLastArg(opts::OPT_ModeGroup);
  if (!mode) {
    // We don't have a mode, so determine a default.
    if (args.hasArg(opts::OPT_EmitModule, opts::OPT_EmitModulePath)) {
      // We've been told to emit a module, but have no other mode indicators.
      // As a result, put the frontend into EmitModuleOnly mode.
      // (Setting up module output will be handled below.)
      return CompilerActionKind::EmitModule;
    }
    if (args.hasArg(opts::OPT_PrintVersion)) {
      return CompilerActionKind::PrintVersion;
    }
    return CompilerActionKind::EmitObject;
  }
  return CompilerOptionsConverter::ComputeActionKind(opts::GetArgID(mode));
}

CompilerActionKind
CompilerOptionsConverter::ComputeActionKind(const unsigned modeOptID) {
  switch (modeOptID) {
  case opts::OPT_Parse:
    return CompilerActionKind::Parse;
  case opts::OPT_ResolveImports:
    return CompilerActionKind::ResolveImports;
  case opts::OPT_EmitParse:
    return CompilerActionKind::EmitParse;
  case opts::OPT_TypeCheck:
    return CompilerActionKind::TypeCheck;
  case opts::OPT_EmitAST:
    return CompilerActionKind::EmitAST;
  case opts::OPT_EmitIR:
    return CompilerActionKind::EmitIR;
  case opts::OPT_EmitBC:
    return CompilerActionKind::EmitBC;
  case opts::OPT_EmitObject:
    return CompilerActionKind::EmitObject;
  case opts::OPT_EmitAssembly:
    return CompilerActionKind::EmitAssembly;
  case opts::OPT_EmitModule:
    return CompilerActionKind::EmitModule;
  case opts::OPT_MergeModules:
    return CompilerActionKind::MergeModules;
  case opts::OPT_PrintVersion:
    return CompilerActionKind::PrintVersion;
  case opts::OPT_PrintHelp:
    return CompilerActionKind::PrintHelp;
  case opts::OPT_PrintHelpHidden:
    return CompilerActionKind::PrintHelpHidden;
  case opts::OPT_PrintFeature:
    return CompilerActionKind::PrintFeature;
  }
  llvm_unreachable("Unhandled mode option");
}

Status CompilerOptionsConverter::ComputeModuleName() {
  // Module name must be computed before computing module
  // aliases. Instead of asserting, clearing ModuleAliasMap
  // here since it can be called redundantly in batch-mode
  compilerOpts.moduleOpts.moduleAliasMap.clear();

  const Arg *A = args.getLastArg(opts::OPT_ModuleName);
  if (A) {
    compilerOpts.moduleOpts.moduleName = A->getValue();
  } else if (compilerOpts.moduleOpts.moduleName.empty()) {
    // The user did not specify a module name, so determine a default fallback
    // based on other options.

    // Note: this code path will only be taken when running the invocation
    // directly; the driver should always pass -module-name when invoking the
    // invocation.
    if (ComputeFallbackModuleName().IsError())
      return Status::Error();
  }

  if (!CompilerOptions::IsValidModuleName(compilerOpts.moduleOpts.moduleName)
           .IsError()) {
    return Status();
  }

  if (compilerOpts.moduleOpts.moduleName != strings::STDLibName) {
    return Status();
  }

  if (compilerOpts.shouldParseAsStdLib) {
    return Status();
  }

  // TODO:
  //  if (Lexer::isIdentifier(compilerOpts.moduleName) &&
  //      (compilerOpts.moduleName != strings::STDLibName ||
  //      compilerOpts.parseStdLib)) {
  //    return false;
  //  }
  //  if (!CompilerOptions::NeedsProperModuleName(compilerOpts.modeKind) ||
  //      compilerOpts.IsCompilingExactlyOneStoneFile()) {
  //    compilerOpts.ModuleName = strings::MainFileName;
  //    return false;
  //  }
  //  auto DID = (compilerOpts.noduleName == STDLIB_NAME) ?
  //  diag::error_stdlib_module_name
  //                                              : diag::error_bad_module_name;
  //  Diags.diagnose(SourceLoc(), DID, Opts.ModuleName, A == nullptr);
  //  Opts.ModuleName = "__bad__";
  //  return false; // FIXME: Must continue to run to pass the tests, but should
  //  not
  //  // have to.

  return Status();
}

Status CompilerOptionsConverter::ComputeFallbackModuleName() {

  std::optional<std::vector<std::string>> outputFilenames =
      CompilerOutputFilesComputer::GetOutputFilenamesFromCommandLineOrFileList(
          args, de, opts::OPT_o, opts::OPT_OutputFileList);

  std::string nameToStem =
      outputFilenames && outputFilenames->size() == 1 &&
              outputFilenames->front() != "-" &&
              !llvm::sys::fs::is_directory(outputFilenames->front())
          ? outputFilenames->front()
          : compilerOpts.GetInputsAndOutputs().GetFilenameOfFirstInput();

  compilerOpts.moduleOpts.moduleName = llvm::sys::path::stem(nameToStem).str();

  return Status();
}
