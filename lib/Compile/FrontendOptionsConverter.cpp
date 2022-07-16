#include "stone/Compile/FrontendOptionsConverter.h"
#include "stone/Basic/Strings.h"
#include "stone/Compile/ModuleSystem.h"
#include "stone/Diag/FrontendDiagnostic.h"
//#include "stone/Basic/Platform.h"
#include "stone/Compile/FrontendInputsConverter.h"
#include "stone/Compile/FrontendOutputsConverter.h"
#include "stone/Session/Options.h"
//#include "stone/Session/SanitizerOptions.h"
#include "stone/Parse/Lexer.h"
//#include "stone/Strings.h"

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

stone::Error FrontendOptionsConverter::Convert(
    llvm::SmallVectorImpl<std::unique_ptr<llvm::MemoryBuffer>> *buffers) {

  // TODO: OK for now
  // assert(frontendOpts.inputsAndOutputs.HasInputs() &&
  //       "Inputs and Outputs should be empty");

  llvm::Optional<FrontendInputsAndOutputs> inputsAndOutputs =
      FrontendInputsConverter(de, args, frontendOpts).Convert(buffers);

  // None here means error, not just "no inputs". Propagage unconditionally.
  if (!inputsAndOutputs) {
    return stone::Error(true);
  }

  bool haveNewInputsAndOutputs = false;
  if (frontendOpts.inputsAndOutputs.HasInputs()) {
    assert(!inputsAndOutputs->HasInputs());
  } else {
    haveNewInputsAndOutputs = true;
    frontendOpts.inputsAndOutputs = std::move(inputsAndOutputs).getValue();
    if (frontendOpts.allowModuleWithCompilerErrors)
      frontendOpts.inputsAndOutputs.SetShouldRecoverMissingInputs();
  }

  if (frontendOpts.inputsAndOutputs.ShouldTreatAsModuleInterface()) {
    frontendOpts.inputFileMode =
        FrontendOptions::InputFileMode::StoneModuleInterface;
  } else if (args.hasArg(opts::ParseAsLibrary)) {
    frontendOpts.inputFileMode = FrontendOptions::InputFileMode::StoneLibrary;
  } else {
    frontendOpts.inputFileMode = FrontendOptions::InputFileMode::Stone;
  }

  if (ComputeModuleName().Has()) {
    return stone::Error(true);
  }

  return stone::Error();
}

stone::Error FrontendOptionsConverter::ComputeModuleName() {
  // Module name must be computed before computing module
  // aliases. Instead of asserting, clearing ModuleAliasMap
  // here since it can be called redundantly in batch-mode
  langOpts.moduleAliasMap.clear();

  const Arg *A = args.getLastArg(opts::ModuleName);
  if (A) {
    langOpts.moduleName = A->getValue();
  } else if (langOpts.moduleName.empty()) {
    // The user did not specify a module name, so determine a default fallback
    // based on other options.

    // Note: this code path will only be taken when running the frontend
    // directly; the driver should always pass -module-name when invoking the
    // frontend.
    if (ComputeFallbackModuleName().Has())
      return stone::Error(true);
  }

  if (!ModuleSystem::IsValidModuleName(langOpts.moduleName).Has()) {
    return stone::Error();
  }

  if (langOpts.moduleName != strings::StdLibName) {
    return stone::Error();
  }

  if (frontendOpts.shouldParseAsStdLib) {
    return stone::Error();
  }

  // TODO:
  //  if (Lexer::isIdentifier(frontendOpts.moduleName) &&
  //      (frontendOpts.moduleName != strings::StdLibName ||
  //      frontendOpts.parseStdLib)) {
  //    return false;
  //  }
  //  if (!FrontendOptions::NeedsProperModuleName(frontendOpts.modeKind) ||
  //      frontendOpts.IsCompilingExactlyOneStoneFile()) {
  //    frontendOpts.ModuleName = strings::MainFileName;
  //    return false;
  //  }
  //  auto DID = (frontendOpts.noduleName == STDLIB_NAME) ?
  //  diag::error_stdlib_module_name
  //                                              : diag::error_bad_module_name;
  //  Diags.diagnose(SourceLoc(), DID, Opts.ModuleName, A == nullptr);
  //  Opts.ModuleName = "__bad__";
  //  return false; // FIXME: Must continue to run to pass the tests, but should
  //  not
  //  // have to.

  return stone::Error();
}

stone::Error FrontendOptionsConverter::ComputeFallbackModuleName() {

  llvm::Optional<std::vector<std::string>> outputFilenames =
      FrontendOutputFilesComputer::GetOutputFilenamesFromCommandLineOrFileList(
          args, de, opts::o, opts::OutputFileList);

  std::string nameToStem =
      outputFilenames && outputFilenames->size() == 1 &&
              outputFilenames->front() != "-" &&
              !llvm::sys::fs::is_directory(outputFilenames->front())
          ? outputFilenames->front()
          : frontendOpts.inputsAndOutputs.GetFilenameOfFirstInput();

  frontendOpts.moduleName = llvm::sys::path::stem(nameToStem).str();

  return stone::Error();
}
