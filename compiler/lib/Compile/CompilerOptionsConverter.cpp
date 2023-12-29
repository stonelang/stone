#include "stone/Compile/CompilerOptionsConverter.h"
#include "stone/Diag/CompilerDiagnostic.h"
#include "stone/Strings.h"
// #include "stone/Basic/Platform.h"
#include "stone/Compile/CompilerInputsConverter.h"
#include "stone/Compile/CompilerOutputsConverter.h"
#include "stone/Option/Options.h"
// #include "stone/Option/SanitizerOptions.h"
#include "stone/Parse/Lexer.h"
// #include "stone/Strings.h"

// Used to validate module name
#include "stone/Compile/Compiler.h"

#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Option/Arg.h"
#include "llvm/Option/ArgList.h"
#include "llvm/Option/Option.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/LineIterator.h"
#include "llvm/Support/Path.h"

using namespace stone;
using namespace llvm::opt;

CompilerOptionsConverter::CompilerOptionsConverter(
    const llvm::opt::ArgList &args, DiagnosticEngine &de, LangOptions &langOpts,
    CompilerOptions &compilerOpts)
    : args(args), de(de), langOpts(langOpts), compilerOpts(compilerOpts) {}

Status CompilerOptionsConverter::Convert(
    llvm::SmallVectorImpl<std::unique_ptr<llvm::MemoryBuffer>> *buffers) {

  Status status;

  // TODO: OK for now
  // assert(compilerOpts.inputsAndOutputs.HasInputs() &&
  //       "Inputs and Outputs should be empty");

  llvm::Optional<CompilerInputsAndOutputs> inputsAndOutputs =
      CompilerInputsConverter(de, args, compilerOpts).Convert(buffers);

  if (!inputsAndOutputs) {
    status.SetIsError();
    status.SetHasCompletion();
    return status;
  }

  if (!inputsAndOutputs->HasInputs()) {
    status.SetIsError();
    status.SetHasCompletion();
    return status;
  }

  // Make sure that it is empty,
  bool haveNewInputsAndOutputs = false;
  if (compilerOpts.GetInputsAndOutputs().HasInputs()) {
    assert(!inputsAndOutputs->HasInputs());
  } else {

    haveNewInputsAndOutputs = true;
    compilerOpts.GetInputsAndOutputs() = std::move(inputsAndOutputs).getValue();
    compilerOpts.mainAction = opts::ParseAction(args);

    if (compilerOpts.allowModuleWithCompilerErrors) {
      compilerOpts.GetInputsAndOutputs().SetShouldRecoverMissingInputs();
    }
  }

  if (!compilerOpts.GetInputsAndOutputs().HasInputs()) {
    status.SetHasCompletion();
  }

  if (compilerOpts.GetInputsAndOutputs().ShouldTreatAsModuleInterface()) {
    compilerOpts.parsingInputMode =
        CompilerOptions::ParsingInputMode::StoneModuleInterface;
  } else if (args.hasArg(opts::ParseAsLibrary)) {
    compilerOpts.parsingInputMode =
        CompilerOptions::ParsingInputMode::StoneLibrary;
  } else {
    compilerOpts.parsingInputMode = CompilerOptions::ParsingInputMode::Stone;
  }

  status = ComputeModuleName();
  if (status.IsError()) {
    status.SetHasCompletion();
  }

  return status;
}

Status CompilerOptionsConverter::ComputeModuleName() {
  // Module name must be computed before computing module
  // aliases. Instead of asserting, clearing ModuleAliasMap
  // here since it can be called redundantly in batch-mode
  compilerOpts.moduleOpts.moduleAliasMap.clear();

  const Arg *A = args.getLastArg(opts::ModuleName);
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

  if (!Compiler::IsValidModuleName(compilerOpts.moduleOpts.moduleName)
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

  llvm::Optional<std::vector<std::string>> outputFilenames =
      CompilerOutputFilesComputer::GetOutputFilenamesFromCommandLineOrFileList(
          args, de, opts::o, opts::OutputFileList);

  std::string nameToStem =
      outputFilenames && outputFilenames->size() == 1 &&
              outputFilenames->front() != "-" &&
              !llvm::sys::fs::is_directory(outputFilenames->front())
          ? outputFilenames->front()
          : compilerOpts.GetInputsAndOutputs().GetFilenameOfFirstInput();

  compilerOpts.moduleOpts.moduleName = llvm::sys::path::stem(nameToStem).str();

  return Status();
}
