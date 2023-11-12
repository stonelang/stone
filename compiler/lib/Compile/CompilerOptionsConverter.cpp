#include "stone/Compile/CompilerOptionsConverter.h"
#include "stone/Basic/Strings.h"
#include "stone/Compile/ModuleSystem.h"
#include "stone/Diag/CompilerDiagnostic.h"
// #include "stone/Basic/Platform.h"
#include "stone/Compile/CompilerInputsConverter.h"
#include "stone/Compile/CompilerOutputsConverter.h"
#include "stone/Option/Options.h"
// #include "stone/Option/SanitizerOptions.h"
#include "stone/Parse/Lexer.h"
// #include "stone/Strings.h"

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
    DiagnosticEngine &de, const llvm::opt::ArgList &args, LangOptions &langOpts,
    CompilerOptions &compilerOpts, ModuleOptions &moduleOpts)
    : de(de), args(args), langOpts(langOpts), compilerOpts(compilerOpts),
      moduleOpts(moduleOpts) {}

Status CompilerOptionsConverter::Convert(
    llvm::SmallVectorImpl<std::unique_ptr<llvm::MemoryBuffer>> *buffers) {

  // TODO: OK for now
  // assert(compilerOpts.inputsAndOutputs.HasInputs() &&
  //       "Inputs and Outputs should be empty");

  llvm::Optional<CompilerInputsAndOutputs> inputsAndOutputs =
      CompilerInputsConverter(de, args, compilerOpts).Convert(buffers);

  // None here means error, not just "no inputs". Propagage unconditionally.
  if (!inputsAndOutputs) {
    return Status::Error();
  }

  bool haveNewInputsAndOutputs = false;
  if (compilerOpts.GetInputsAndOutputs().HasInputs()) {
    assert(!inputsAndOutputs->HasInputs());
  } else {

    haveNewInputsAndOutputs = true;
    compilerOpts.GetInputsAndOutputs() = std::move(inputsAndOutputs).getValue();

    if (compilerOpts.allowModuleWithCompilerErrors) {
      compilerOpts.GetInputsAndOutputs().SetShouldRecoverMissingInputs();
    }
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

  if (ComputeModuleName().IsError()) {
    return Status::Error();
  }

  return Status();
}

Status CompilerOptionsConverter::ComputeModuleName() {
  // Module name must be computed before computing module
  // aliases. Instead of asserting, clearing ModuleAliasMap
  // here since it can be called redundantly in batch-mode
  moduleOpts.moduleAliasMap.clear();

  const Arg *A = args.getLastArg(opts::ModuleName);
  if (A) {
    moduleOpts.moduleName = A->getValue();
  } else if (moduleOpts.moduleName.empty()) {
    // The user did not specify a module name, so determine a default fallback
    // based on other options.

    // Note: this code path will only be taken when running the invocation
    // directly; the driver should always pass -module-name when invoking the
    // invocation.
    if (ComputeFallbackModuleName().IsError())
      return Status::Error();
  }

  if (!ModuleSystem::IsValidModuleName(moduleOpts.moduleName).IsError()) {
    return Status();
  }

  if (moduleOpts.moduleName != strings::StdLibName) {
    return Status();
  }

  if (compilerOpts.shouldParseAsStdLib) {
    return Status();
  }

  // TODO:
  //  if (Lexer::isIdentifier(compilerOpts.moduleName) &&
  //      (compilerOpts.moduleName != strings::StdLibName ||
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

  moduleOpts.moduleName = llvm::sys::path::stem(nameToStem).str();

  return Status();
}
