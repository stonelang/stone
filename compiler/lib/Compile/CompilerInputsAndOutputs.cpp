#include "stone/Compile/CompilerInputsAndOutputs.h"
#include "stone/AST/DiagnosticsCompile.h"
#include "stone/Basic/FileType.h"
#include "stone/Basic/PrimaryFileSpecificPaths.h"
#include "stone/Basic/Range.h"
#include "stone/Compile/CompilerOptions.h"
#include "stone/Support/Options.h"

#include "llvm/Option/Arg.h"
#include "llvm/Option/ArgList.h"
#include "llvm/Option/Option.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/LineIterator.h"
#include "llvm/Support/Path.h"

using namespace stone;
using namespace llvm::opt;

// Constructors

CompilerInputsAndOutputs::CompilerInputsAndOutputs(
    const CompilerInputsAndOutputs &other) {
  for (CompilerInputFile input : other.inputs) {
    AddInput(input);
  }
  isSingleThreadedWMO = other.isSingleThreadedWMO;
  shouldRecoverMissingInputs = other.shouldRecoverMissingInputs;
}

CompilerInputsAndOutputs &
CompilerInputsAndOutputs::operator=(const CompilerInputsAndOutputs &other) {
  ClearInputs();
  for (CompilerInputFile input : other.inputs) {
    AddInput(input);
  }
  isSingleThreadedWMO = other.isSingleThreadedWMO;
  shouldRecoverMissingInputs = other.shouldRecoverMissingInputs;
  return *this;
}

// All inputs:

std::vector<std::string>
CompilerInputsAndOutputs::GetCompilerInputFilenames() const {
  std::vector<std::string> filenames;
  for (auto &input : inputs) {
    filenames.push_back(input.GetFileName());
  }
  return filenames;
}

bool CompilerInputsAndOutputs::IsReadingFromStdin() const {
  return HasSingleInput() && GetFilenameOfFirstInput() == "-";
}

const std::string &CompilerInputsAndOutputs::GetFilenameOfFirstInput() const {
  assert(HasInputs());
  const CompilerInputFile &firstInput = inputs[0];
  const std::string &f = firstInput.GetFileName();
  assert(!f.empty());
  return f;
}

bool CompilerInputsAndOutputs::ForEachInput(
    llvm::function_ref<bool(const CompilerInputFile &)> fn) const {
  for (const CompilerInputFile &input : inputs) {
    if (fn(input)) {
      return true;
    }
  }
  return false;
}

// Primaries:

const CompilerInputFile &CompilerInputsAndOutputs::FirstPrimaryInput() const {
  assert(!primaryInputsInOrder.empty());
  return inputs[primaryInputsInOrder.front()];
}

const CompilerInputFile &CompilerInputsAndOutputs::LastPrimaryInput() const {
  assert(!primaryInputsInOrder.empty());
  return inputs[primaryInputsInOrder.back()];
}

bool CompilerInputsAndOutputs::ForEachPrimaryInput(
    llvm::function_ref<bool(const CompilerInputFile &)> fn) const {
  for (unsigned i : primaryInputsInOrder) {
    if (fn(inputs[i])) {
      return true;
    }
  }
  return false;
}

bool CompilerInputsAndOutputs::ForEachPrimaryInputWithIndex(
    llvm::function_ref<bool(const CompilerInputFile &, unsigned index)> fn)
    const {
  for (unsigned i : primaryInputsInOrder) {
    if (fn(inputs[i], i)) {
      return true;
    }
  }
  return false;
}

bool CompilerInputsAndOutputs::ForEachNonPrimaryInput(
    llvm::function_ref<bool(const CompilerInputFile &)> fn) const {
  return ForEachInput([&](const CompilerInputFile &f) -> bool {
    return f.IsPrimary() ? false : fn(f);
  });
}

void CompilerInputsAndOutputs::AssertMustNotBeMoreThanOnePrimaryInput() const {
  assert(!HasMultiplePrimaryInputs() &&
         "have not implemented >1 primary input yet");
}

void CompilerInputsAndOutputs::
    AssertMustNotBeMoreThanOnePrimaryInputUnlessBatchModeChecksHaveBeenBypassed()
        const {
  if (!AreBatchModeChecksBypassed()) {
    AssertMustNotBeMoreThanOnePrimaryInput();
  }
}

const CompilerInputFile *
CompilerInputsAndOutputs::GetUniquePrimaryInput() const {
  AssertMustNotBeMoreThanOnePrimaryInput();
  return primaryInputsInOrder.empty() ? nullptr
                                      : &inputs[primaryInputsInOrder.front()];
}

const CompilerInputFile &
CompilerInputsAndOutputs::GetRequiredUniquePrimaryInput() const {
  if (const auto *input = GetUniquePrimaryInput()) {
    return *input;
  }
  llvm_unreachable("No primary when one is required");
}

std::string CompilerInputsAndOutputs::GetStatsFileMangledInputName() const {
  // Use the first primary, even if there are multiple primaries.
  // That's enough to keep the file names unique.
  return IsWholeModule() ? "all" : FirstPrimaryInput().GetFileName();
}

bool CompilerInputsAndOutputs::IsInputPrimary(StringRef file) const {
  return PrimaryInputNamed(file) != nullptr;
}

unsigned CompilerInputsAndOutputs::NumberOfPrimaryInputsEndingWith(
    StringRef extension) const {
  unsigned n = 0;
  (void)ForEachPrimaryInput([&](const CompilerInputFile &input) -> bool {
    if (llvm::sys::path::extension(input.GetFileName()).ends_with(extension))
      ++n;
    return false;
  });
  return n;
}

// Input queries

bool CompilerInputsAndOutputs::ShouldTreatAsLLVM() const {
  if (HasSingleInput()) {
    StringRef inputExt = llvm::sys::path::extension(GetFilenameOfFirstInput());
    switch (stone::GetFileTypeByExt(inputExt)) {
    case FileType::BC:
    case FileType::IR:
      return true;
    default:
      return false;
    }
  }
  return false;
}

bool CompilerInputsAndOutputs::ShouldTreatAsModuleInterface() const {
  if (!HasSingleInput()) {
    return false;
  }

  StringRef InputExt = llvm::sys::path::extension(GetFilenameOfFirstInput());
  FileType InputType = stone::GetFileTypeByExt(InputExt);
  return InputType == FileType::StoneModuleInterface;
}

bool CompilerInputsAndOutputs::VerifyInputs(DiagnosticEngine &diags,
                                            bool isNoneRequested) const {
  assert(false && "TODO: VerifyInputs");

  AssertMustNotBeMoreThanOnePrimaryInput();
  if (!isNoneRequested && !HasInputs()) {
    // diags.diagnose(SourceLoc(), diag::error_mode_requires_an_input_file);
    return true;
  }
  return false;
}

// Changing inputs

void CompilerInputsAndOutputs::ClearInputs() {
  inputs.clear();
  primaryInputsByName.clear();
  primaryInputsInOrder.clear();
}

void CompilerInputsAndOutputs::AddInput(const CompilerInputFile &input) {
  const unsigned index = inputs.size();
  inputs.push_back(input);
  if (input.IsPrimary()) {
    primaryInputsInOrder.push_back(index);
    primaryInputsByName.insert({inputs.back().GetFileName(), index});
  }
}

void CompilerInputsAndOutputs::AddCompilerInputFile(
    StringRef file, llvm::MemoryBuffer *buffer) {
  AddInput(CompilerInputFile(file, false, buffer));
}

void CompilerInputsAndOutputs::AddPrimaryCompilerInputFile(
    StringRef file, llvm::MemoryBuffer *buffer) {
  AddInput(CompilerInputFile(file, true, buffer));
}

unsigned CompilerInputsAndOutputs::CountOfInputsProducingMainOutputs() const {

  return IsSingleThreadedWMO() ? 1
         : HasPrimaryInputs()  ? PrimaryInputCount()
                               : InputCount();
}

const CompilerInputFile &
CompilerInputsAndOutputs::FirstInputProducingOutput() const {

  return IsSingleThreadedWMO() ? FirstInput()
         : HasPrimaryInputs()  ? FirstPrimaryInput()
                               : FirstInput();
}

const CompilerInputFile &
CompilerInputsAndOutputs::LastInputProducingOutput() const {
  return IsSingleThreadedWMO() ? FirstInput()
         : HasPrimaryInputs()  ? LastPrimaryInput()
                               : LastInput();
}

bool CompilerInputsAndOutputs::ForEachInputProducingAMainOutputFile(
    llvm::function_ref<bool(const CompilerInputFile &)> fn) const {

  return IsSingleThreadedWMO() ? fn(FirstInput())
         : HasPrimaryInputs()  ? ForEachPrimaryInput(fn)
                               : ForEachInput(fn);
}

void CompilerInputsAndOutputs::SetMainAndSupplementaryOutputs(
    ArrayRef<std::string> outputFiles,
    ArrayRef<SupplementaryOutputPaths> supplementaryOutputPaths,
    ArrayRef<std::string> outputFilesForIndexUnits) {

  if (outputFilesForIndexUnits.empty()) {
    outputFilesForIndexUnits = outputFiles;
  }

  assert(outputFiles.size() == outputFilesForIndexUnits.size() &&
         "Must have one index unit output path per main output");

  if (inputs.empty()) {
    assert(outputFiles.empty() && "Cannot have outputs without inputs");
    assert(supplementaryOutputPaths.empty() &&
           "Cannot have supplementary outputs without inputs");
    return;
  }
  if (HasPrimaryInputs()) {
    const auto N = PrimaryInputCount();
    assert(outputFiles.size() == N && "Must have one main output per primary");
    assert(supplementaryOutputPaths.size() == N &&
           "Must have one set of supplementary outputs per primary");
    (void)N;

    unsigned i = 0;
    for (auto &input : inputs) {
      if (input.IsPrimary()) {
        input.SetPrimaryFileSpecificPaths(PrimaryFileSpecificPaths(
            outputFiles[i], outputFilesForIndexUnits[i], input.GetFileName(),
            supplementaryOutputPaths[i]));
        ++i;
      }
    }
    return;
  }
  assert(supplementaryOutputPaths.size() == 1 &&
         "WMO only ever produces one set of supplementary outputs");
  if (outputFiles.size() == 1) {
    inputs.front().SetPrimaryFileSpecificPaths(PrimaryFileSpecificPaths(
        outputFiles.front(), outputFilesForIndexUnits.front(),
        FirstInputProducingOutput().GetFileName(),
        supplementaryOutputPaths.front()));
    return;
  }
  assert(outputFiles.size() == inputs.size() &&
         "Multi-threaded WMO requires one main output per input");
  for (auto i : indices(inputs)) {
    inputs[i].SetPrimaryFileSpecificPaths(PrimaryFileSpecificPaths(
        outputFiles[i], outputFilesForIndexUnits[i], outputFiles[i],
        i == 0 ? supplementaryOutputPaths.front()
               : SupplementaryOutputPaths()));
  }
}

std::vector<std::string> CompilerInputsAndOutputs::CopyOutputFilenames() const {
  std::vector<std::string> outputs;
  (void)ForEachInputProducingAMainOutputFile(
      [&](const CompilerInputFile &input) -> bool {
        outputs.push_back(input.OutputFilename());
        return false;
      });
  return outputs;
}

std::vector<std::string>
CompilerInputsAndOutputs::CopyIndexUnitOutputFilenames() const {
  std::vector<std::string> outputs;
  (void)ForEachInputProducingAMainOutputFile(
      [&](const CompilerInputFile &input) -> bool {
        outputs.push_back(input.IndexUnitOutputFilename());
        return false;
      });
  return outputs;
}

void CompilerInputsAndOutputs::ForEachOutputFilename(
    llvm::function_ref<void(StringRef)> fn) const {
  (void)ForEachInputProducingAMainOutputFile(
      [&](const CompilerInputFile &input) -> bool {
        fn(input.OutputFilename());
        return false;
      });
}

std::string CompilerInputsAndOutputs::GetSingleOutputFilename() const {
  AssertMustNotBeMoreThanOnePrimaryInputUnlessBatchModeChecksHaveBeenBypassed();
  return HasInputs() ? LastInputProducingOutput().OutputFilename()
                     : std::string();
}

std::string CompilerInputsAndOutputs::GetSingleIndexUnitOutputFilename() const {
  AssertMustNotBeMoreThanOnePrimaryInputUnlessBatchModeChecksHaveBeenBypassed();
  return HasInputs() ? LastInputProducingOutput().IndexUnitOutputFilename()
                     : std::string();
}

bool CompilerInputsAndOutputs::IsOutputFilenameStdout() const {
  return GetSingleOutputFilename() == "-";
}

bool CompilerInputsAndOutputs::IsOutputFileDirectory() const {
  return HasNamedOutputFile() &&
         llvm::sys::fs::is_directory(GetSingleOutputFilename());
}

bool CompilerInputsAndOutputs::HasNamedOutputFile() const {
  return HasInputs() && !IsOutputFilenameStdout();
}

// Supplementary outputs

unsigned
CompilerInputsAndOutputs::CountOfFilesProducingSupplementaryOutput() const {
  return HasPrimaryInputs() ? PrimaryInputCount() : HasInputs() ? 1 : 0;
}

bool CompilerInputsAndOutputs::ForEachInputProducingSupplementaryOutput(
    llvm::function_ref<bool(const CompilerInputFile &)> fn) const {
  return HasPrimaryInputs() ? ForEachPrimaryInput(fn)
         : HasInputs()      ? fn(FirstInput())
                            : false;
}

bool CompilerInputsAndOutputs::HasSupplementaryOutputPath(
    llvm::function_ref<const std::string &(const SupplementaryOutputPaths &)>
        extractorFn) const {
  return ForEachInputProducingSupplementaryOutput(
      [&](const CompilerInputFile &input) -> bool {
        return !extractorFn(input.GetPrimaryFileSpecificPaths()
                                .supplementaryOutputPaths)
                    .empty();
      });
}

bool CompilerInputsAndOutputs::HasDependenciesPath() const {
  return HasSupplementaryOutputPath(
      [](const SupplementaryOutputPaths &outs) -> const std::string & {
        return outs.dependenciesFilePath;
      });
}
bool CompilerInputsAndOutputs::HasReferenceDependenciesPath() const {
  return HasSupplementaryOutputPath(
      [](const SupplementaryOutputPaths &outs) -> const std::string & {
        return outs.referenceDependenciesFilePath;
      });
}
bool CompilerInputsAndOutputs::HasLoadedModuleTracePath() const {
  return HasSupplementaryOutputPath(
      [](const SupplementaryOutputPaths &outs) -> const std::string & {
        return outs.loadedModuleTracePath;
      });
}
bool CompilerInputsAndOutputs::HasModuleOutputPath() const {
  return HasSupplementaryOutputPath(
      [](const SupplementaryOutputPaths &outs) -> const std::string & {
        return outs.moduleOutputPath;
      });
}
bool CompilerInputsAndOutputs::HasModuleDocOutputPath() const {
  return HasSupplementaryOutputPath(
      [](const SupplementaryOutputPaths &outs) -> const std::string & {
        return outs.moduleDocOutputPath;
      });
}
bool CompilerInputsAndOutputs::HasModuleSourceInfoOutputPath() const {
  return HasSupplementaryOutputPath(
      [](const SupplementaryOutputPaths &outs) -> const std::string & {
        return outs.moduleSourceInfoOutputPath;
      });
}
bool CompilerInputsAndOutputs::HasModuleInterfaceOutputPath() const {
  return HasSupplementaryOutputPath(
      [](const SupplementaryOutputPaths &outs) -> const std::string & {
        return outs.moduleInterfaceOutputPath;
      });
}
bool CompilerInputsAndOutputs::HasPrivateModuleInterfaceOutputPath() const {
  return HasSupplementaryOutputPath(
      [](const SupplementaryOutputPaths &outs) -> const std::string & {
        return outs.privateModuleInterfaceOutputPath;
      });
}
bool CompilerInputsAndOutputs::HasABIDescriptorOutputPath() const {
  return HasSupplementaryOutputPath(
      [](const SupplementaryOutputPaths &outs) -> const std::string & {
        return outs.abiDescriptorOutputPath;
      });
}
bool CompilerInputsAndOutputs::HasModuleSemanticInfoOutputPath() const {
  return HasSupplementaryOutputPath(
      [](const SupplementaryOutputPaths &outs) -> const std::string & {
        return outs.moduleSemanticInfoOutputPath;
      });
}
bool CompilerInputsAndOutputs::HasModuleSummaryOutputPath() const {
  return HasSupplementaryOutputPath(
      [](const SupplementaryOutputPaths &outs) -> const std::string & {
        return outs.moduleSummaryOutputPath;
      });
}
bool CompilerInputsAndOutputs::HasTBDPath() const {
  return HasSupplementaryOutputPath(
      [](const SupplementaryOutputPaths &outs) -> const std::string & {
        return outs.tbdPath;
      });
}
bool CompilerInputsAndOutputs::HasYAMLOptRecordPath() const {
  return HasSupplementaryOutputPath(
      [](const SupplementaryOutputPaths &outs) -> const std::string & {
        return outs.yamlOptRecordPath;
      });
}
bool CompilerInputsAndOutputs::HasBitstreamOptRecordPath() const {
  return HasSupplementaryOutputPath(
      [](const SupplementaryOutputPaths &outs) -> const std::string & {
        return outs.bitstreamOptRecordPath;
      });
}

bool CompilerInputsAndOutputs::HasDependencyTrackerPath() const {
  return HasDependenciesPath() || HasReferenceDependenciesPath() ||
         HasLoadedModuleTracePath();
}

const PrimaryFileSpecificPaths &
CompilerInputsAndOutputs::GetPrimaryFileSpecificPathsForAtMostOnePrimary()
    const {
  AssertMustNotBeMoreThanOnePrimaryInputUnlessBatchModeChecksHaveBeenBypassed();
  static auto emptyPaths = PrimaryFileSpecificPaths();
  return HasInputs() ? FirstInputProducingOutput().GetPrimaryFileSpecificPaths()
                     : emptyPaths;
}

const PrimaryFileSpecificPaths &
CompilerInputsAndOutputs::GetPrimaryFileSpecificPathsForPrimary(
    StringRef filename) const {
  const CompilerInputFile *f = PrimaryInputNamed(filename);
  return f->GetPrimaryFileSpecificPaths();
}

const CompilerInputFile *
CompilerInputsAndOutputs::PrimaryInputNamed(StringRef name) const {
  assert(!name.empty() && "input files have names");
  StringRef correctedFile = CompilerInputFile::
      ConvertBufferNameFromLLVMGetFileOrSTDINToStoneConventions(name);
  auto iterator = primaryInputsByName.find(correctedFile);
  if (iterator == primaryInputsByName.end())
    return nullptr;
  const CompilerInputFile *f = &inputs[iterator->second];
  assert(f->IsPrimary() && "primaryInputsByName should only include primries");
  return f;
}
