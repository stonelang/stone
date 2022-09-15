#include "stone/Compile/CompilerOptionsConverter.h"
#include "stone/Basic/Strings.h"
#include "stone/Compile/ModuleSystem.h"
#include "stone/Diag/CompilerDiagnostic.h"
// #include "stone/Basic/Platform.h"
#include "stone/Compile/CompilerInputsConverter.h"
#include "stone/Compile/CompilerOutputsConverter.h"
#include "stone/Session/Options.h"
// #include "stone/Session/SanitizerOptions.h"
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

stone::Error CompilerOptionsConverter::Convert(
    llvm::SmallVectorImpl<std::unique_ptr<llvm::MemoryBuffer>> *buffers) {

  // TODO: OK for now
  // assert(invocationOpts.inputsAndOutputs.HasInputs() &&
  //       "Inputs and Outputs should be empty");

  llvm::Optional<CompilerInputsAndOutputs> inputsAndOutputs =
      CompilerInputsConverter(de, args, invocationOpts).Convert(buffers);

  // None here means error, not just "no inputs". Propagage unconditionally.
  if (!inputsAndOutputs) {
    return stone::Error(true);
  }

  bool haveNewInputsAndOutputs = false;
  if (invocationOpts.GetCompilerInputsAndOutputs().HasInputs()) {
    assert(!inputsAndOutputs->HasInputs());
  } else {

    haveNewInputsAndOutputs = true;
    invocationOpts.GetCompilerInputsAndOutputs() =
        std::move(inputsAndOutputs).getValue();

    if (invocationOpts.allowModuleWithCompilerErrors) {
      invocationOpts.GetCompilerInputsAndOutputs()
          .SetShouldRecoverMissingInputs();
    }
  }

  if (invocationOpts.GetCompilerInputsAndOutputs()
          .ShouldTreatAsModuleInterface()) {
    invocationOpts.inputFileMode =
        CompilerOptions::InputFileMode::StoneModuleInterface;
  } else if (args.hasArg(opts::ParseAsLibrary)) {
    invocationOpts.inputFileMode = CompilerOptions::InputFileMode::StoneLibrary;
  } else {
    invocationOpts.inputFileMode = CompilerOptions::InputFileMode::Stone;
  }

  if (ComputeModuleName().Has()) {
    return stone::Error(true);
  }

  return stone::Error();
}

stone::Error CompilerOptionsConverter::ComputeModuleName() {
  // Module name must be computed before computing module
  // aliases. Instead of asserting, clearing ModuleAliasMap
  // here since it can be called redundantly in batch-mode
  invocationOpts.moduleOpts.moduleAliasMap.clear();

  const Arg *A = args.getLastArg(opts::ModuleName);
  if (A) {
    invocationOpts.moduleOpts.moduleName = A->getValue();
  } else if (invocationOpts.moduleOpts.moduleName.empty()) {
    // The user did not specify a module name, so determine a default fallback
    // based on other options.

    // Note: this code path will only be taken when running the invocation
    // directly; the driver should always pass -module-name when invoking the
    // invocation.
    if (ComputeFallbackModuleName().Has())
      return stone::Error(true);
  }

  if (!ModuleSystem::IsValidModuleName(invocationOpts.moduleOpts.moduleName)
           .Has()) {
    return stone::Error();
  }

  if (invocationOpts.moduleOpts.moduleName != strings::StdLibName) {
    return stone::Error();
  }

  if (invocationOpts.shouldParseAsStdLib) {
    return stone::Error();
  }

  // TODO:
  //  if (Lexer::isIdentifier(invocationOpts.moduleName) &&
  //      (invocationOpts.moduleName != strings::StdLibName ||
  //      invocationOpts.parseStdLib)) {
  //    return false;
  //  }
  //  if (!CompilerOptions::NeedsProperModuleName(invocationOpts.modeKind) ||
  //      invocationOpts.IsCompilingExactlyOneStoneFile()) {
  //    invocationOpts.ModuleName = strings::MainFileName;
  //    return false;
  //  }
  //  auto DID = (invocationOpts.noduleName == STDLIB_NAME) ?
  //  diag::error_stdlib_module_name
  //                                              : diag::error_bad_module_name;
  //  Diags.diagnose(SourceLoc(), DID, Opts.ModuleName, A == nullptr);
  //  Opts.ModuleName = "__bad__";
  //  return false; // FIXME: Must continue to run to pass the tests, but should
  //  not
  //  // have to.

  return stone::Error();
}

stone::Error CompilerOptionsConverter::ComputeFallbackModuleName() {

  llvm::Optional<std::vector<std::string>> outputFilenames =
      CompilerOutputFilesComputer::GetOutputFilenamesFromCommandLineOrFileList(
          args, de, opts::o, opts::OutputFileList);

  std::string nameToStem =
      outputFilenames && outputFilenames->size() == 1 &&
              outputFilenames->front() != "-" &&
              !llvm::sys::fs::is_directory(outputFilenames->front())
          ? outputFilenames->front()
          : invocationOpts.GetCompilerInputsAndOutputs()
                .GetFilenameOfFirstInput();

  invocationOpts.moduleOpts.moduleName =
      llvm::sys::path::stem(nameToStem).str();

  return stone::Error();
}
