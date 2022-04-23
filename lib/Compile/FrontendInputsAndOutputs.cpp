#include "stone/Compile/FrontendInputsAndOutputs.h"
#include "stone/Basic/FrontendDiagnostic.h"
#include "stone/Basic/Context.h"
#include "stone/Basic/File.h"
#include "stone/Basic/Range.h"
#include "stone/Compile/FrontendOptions.h"
#include "stone/Compile/PrimaryFileSpecificPaths.h"
#include "stone/Session/Options.h"

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

FrontendInputsAndOutputs::FrontendInputsAndOutputs(
    const FrontendInputsAndOutputs &other) {
  for (FrontendInputFile input : other.inputs) {
    AddInput(input);
  }
  isSingleThreadedWMO = other.isSingleThreadedWMO;
  shouldRecoverMissingInputs = other.shouldRecoverMissingInputs;
}

FrontendInputsAndOutputs &
FrontendInputsAndOutputs::operator=(const FrontendInputsAndOutputs &other) {
  ClearInputs();
  for (FrontendInputFile input : other.inputs) {
    AddInput(input);
  }
  isSingleThreadedWMO = other.isSingleThreadedWMO;
  shouldRecoverMissingInputs = other.shouldRecoverMissingInputs;
  return *this;
}

// All inputs:

std::vector<std::string>
FrontendInputsAndOutputs::GetFrontendInputFilenames() const {
  std::vector<std::string> filenames;
  for (auto &input : inputs) {
    filenames.push_back(input.GetFileName());
  }
  return filenames;
}

bool FrontendInputsAndOutputs::IsReadingFromStdin() const {
  return HasSingleInput() && GetFilenameOfFirstInput() == "-";
}

const std::string &FrontendInputsAndOutputs::GetFilenameOfFirstInput() const {
  assert(HasInputs());
  const FrontendInputFile &inp = inputs[0];
  const std::string &f = inp.GetFileName();
  assert(!f.empty());
  return f;
}

bool FrontendInputsAndOutputs::ForEachInput(
    llvm::function_ref<bool(const FrontendInputFile &)> fn) const {
  for (const FrontendInputFile &input : inputs) {
    if (fn(input)) {
      return true;
    }
  }
  return false;
}

// Primaries:

const FrontendInputFile &FrontendInputsAndOutputs::FirstPrimaryInput() const {
  assert(!primaryInputsInOrder.empty());
  return inputs[primaryInputsInOrder.front()];
}

const FrontendInputFile &FrontendInputsAndOutputs::LastPrimaryInput() const {
  assert(!primaryInputsInOrder.empty());
  return inputs[primaryInputsInOrder.back()];
}

bool FrontendInputsAndOutputs::ForEachPrimaryInput(
    llvm::function_ref<bool(const FrontendInputFile &)> fn) const {
  for (unsigned i : primaryInputsInOrder) {
    if (fn(inputs[i])) {
      return true;
    }
  }
  return false;
}

bool FrontendInputsAndOutputs::ForEachPrimaryInputWithIndex(
    llvm::function_ref<bool(const FrontendInputFile &, unsigned index)> fn)
    const {
  for (unsigned i : primaryInputsInOrder) {
    if (fn(inputs[i], i)) {
      return true;
    }
  }
  return false;
}

bool FrontendInputsAndOutputs::ForEachNonPrimaryInput(
    llvm::function_ref<bool(const FrontendInputFile &)> fn) const {
  return ForEachInput([&](const FrontendInputFile &f) -> bool {
    return f.IsPrimary() ? false : fn(f);
  });
}

void FrontendInputsAndOutputs::AssertMustNotBeMoreThanOnePrimaryInput() const {
  assert(!HasMultiplePrimaryInputs() &&
         "have not implemented >1 primary input yet");
}

void FrontendInputsAndOutputs::
    AssertMustNotBeMoreThanOnePrimaryInputUnlessBatchModeChecksHaveBeenBypassed()
        const {
  if (!AreBatchModeChecksBypassed()) {
    AssertMustNotBeMoreThanOnePrimaryInput();
  }
}

const FrontendInputFile *
FrontendInputsAndOutputs::GetUniquePrimaryInput() const {
  AssertMustNotBeMoreThanOnePrimaryInput();
  return primaryInputsInOrder.empty() ? nullptr
                                      : &inputs[primaryInputsInOrder.front()];
}

const FrontendInputFile &
FrontendInputsAndOutputs::GetRequiredUniquePrimaryInput() const {
  if (const auto *input = GetUniquePrimaryInput()) {
    return *input;
  }
  llvm_unreachable("No primary when one is required");
}

std::string FrontendInputsAndOutputs::GetStatsFileMangledInputName() const {
  // Use the first primary, even if there are multiple primaries.
  // That's enough to keep the file names unique.
  return IsWholeModule() ? "all" : FirstPrimaryInput().GetFileName();
}

bool FrontendInputsAndOutputs::IsInputPrimary(StringRef file) const {
  return PrimaryInputNamed(file) != nullptr;
}

unsigned FrontendInputsAndOutputs::NumberOfPrimaryInputsEndingWith(
    StringRef extension) const {
  unsigned n = 0;
  (void)ForEachPrimaryInput([&](const FrontendInputFile &input) -> bool {
    if (llvm::sys::path::extension(input.GetFileName()).endswith(extension))
      ++n;
    return false;
  });
  return n;
}

// Input queries

bool FrontendInputsAndOutputs::ShouldTreatAsLLVM() const {
  if (HasSingleInput()) {
    StringRef inputExt = llvm::sys::path::extension(GetFilenameOfFirstInput());
    switch (file::GetTypeByExt(inputExt)) {
    case file::Type::BC:
    case file::Type::IR:
      return true;
    default:
      return false;
    }
  }
  return false;
}

bool FrontendInputsAndOutputs::ShouldTreatAsModuleInterface() const {
  if (!HasSingleInput()) {
    return false;
  }

  StringRef InputExt = llvm::sys::path::extension(GetFilenameOfFirstInput());
  file::Type InputType = file::GetTypeByExt(InputExt);
  return InputType == file::Type::StoneModuleInterface;
}

bool FrontendInputsAndOutputs::VerifyInputs(DiagnosticEngine &diags,
                                            bool isNoneRequested) const {
  stone::Panic("TODO: VerifyInputs");

  AssertMustNotBeMoreThanOnePrimaryInput();
  if (!isNoneRequested && !HasInputs()) {
    // diags.PrintD(SourceLoc(), diag::error_mode_requires_an_input_file);
    return true;
  }
  return false;
}

// Changing inputs

void FrontendInputsAndOutputs::ClearInputs() {
  inputs.clear();
  primaryInputsByName.clear();
  primaryInputsInOrder.clear();
}

void FrontendInputsAndOutputs::AddInput(const FrontendInputFile &input) {
  const unsigned index = inputs.size();
  inputs.push_back(input);
  if (input.IsPrimary()) {
    primaryInputsInOrder.push_back(index);
    primaryInputsByName.insert({inputs.back().GetFileName(), index});
  }
}

void FrontendInputsAndOutputs::AddFrontendInputFile(
    StringRef file, llvm::MemoryBuffer *buffer) {
  AddInput(FrontendInputFile(file, false, buffer));
}

void FrontendInputsAndOutputs::AddPrimaryFrontendInputFile(
    StringRef file, llvm::MemoryBuffer *buffer) {
  AddInput(FrontendInputFile(file, true, buffer));
}

unsigned FrontendInputsAndOutputs::CountOfInputsProducingMainOutputs() const {

  return IsSingleThreadedWMO() ? 1
         : HasPrimaryInputs()  ? PrimaryInputCount()
                               : InputCount();
}

const FrontendInputFile &
FrontendInputsAndOutputs::FirstInputProducingOutput() const {

  return IsSingleThreadedWMO() ? FirstInput()
         : HasPrimaryInputs()  ? FirstPrimaryInput()
                               : FirstInput();
}

const FrontendInputFile &
FrontendInputsAndOutputs::LastInputProducingOutput() const {
  return IsSingleThreadedWMO() ? FirstInput()
         : HasPrimaryInputs()  ? LastPrimaryInput()
                               : LastInput();
}

bool FrontendInputsAndOutputs::ForEachInputProducingAMainOutputFile(
    llvm::function_ref<bool(const FrontendInputFile &)> fn) const {

  return IsSingleThreadedWMO() ? fn(FirstInput())
         : HasPrimaryInputs()  ? ForEachPrimaryInput(fn)
                               : ForEachInput(fn);
}

void FrontendInputsAndOutputs::SetMainAndSupplementaryOutputs(
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

std::vector<std::string> FrontendInputsAndOutputs::CopyOutputFilenames() const {
  std::vector<std::string> outputs;
  (void)ForEachInputProducingAMainOutputFile(
      [&](const FrontendInputFile &input) -> bool {
        outputs.push_back(input.OutputFilename());
        return false;
      });
  return outputs;
}

std::vector<std::string>
FrontendInputsAndOutputs::CopyIndexUnitOutputFilenames() const {
  std::vector<std::string> outputs;
  (void)ForEachInputProducingAMainOutputFile(
      [&](const FrontendInputFile &input) -> bool {
        outputs.push_back(input.IndexUnitOutputFilename());
        return false;
      });
  return outputs;
}

void FrontendInputsAndOutputs::ForEachOutputFilename(
    llvm::function_ref<void(StringRef)> fn) const {
  (void)ForEachInputProducingAMainOutputFile(
      [&](const FrontendInputFile &input) -> bool {
        fn(input.OutputFilename());
        return false;
      });
}

std::string FrontendInputsAndOutputs::GetSingleOutputFilename() const {
  AssertMustNotBeMoreThanOnePrimaryInputUnlessBatchModeChecksHaveBeenBypassed();
  return HasInputs() ? LastInputProducingOutput().OutputFilename()
                     : std::string();
}

std::string FrontendInputsAndOutputs::GetSingleIndexUnitOutputFilename() const {
  AssertMustNotBeMoreThanOnePrimaryInputUnlessBatchModeChecksHaveBeenBypassed();
  return HasInputs() ? LastInputProducingOutput().IndexUnitOutputFilename()
                     : std::string();
}

bool FrontendInputsAndOutputs::IsOutputFilenameStdout() const {
  return GetSingleOutputFilename() == "-";
}

bool FrontendInputsAndOutputs::IsOutputFileDirectory() const {
  return HasNamedOutputFile() &&
         llvm::sys::fs::is_directory(GetSingleOutputFilename());
}

bool FrontendInputsAndOutputs::HasNamedOutputFile() const {
  return HasInputs() && !IsOutputFilenameStdout();
}

// Supplementary outputs

unsigned
FrontendInputsAndOutputs::CountOfFilesProducingSupplementaryOutput() const {
  return HasPrimaryInputs() ? PrimaryInputCount() : HasInputs() ? 1 : 0;
}

bool FrontendInputsAndOutputs::ForEachInputProducingSupplementaryOutput(
    llvm::function_ref<bool(const FrontendInputFile &)> fn) const {
  return HasPrimaryInputs() ? ForEachPrimaryInput(fn)
         : HasInputs()      ? fn(FirstInput())
                            : false;
}

bool FrontendInputsAndOutputs::HasSupplementaryOutputPath(
    llvm::function_ref<const std::string &(const SupplementaryOutputPaths &)>
        extractorFn) const {
  return ForEachInputProducingSupplementaryOutput(
      [&](const FrontendInputFile &input) -> bool {
        return !extractorFn(input.GetPrimaryFileSpecificPaths()
                                .supplementaryOutputPaths)
                    .empty();
      });
}

bool FrontendInputsAndOutputs::HasDependenciesPath() const {
  return HasSupplementaryOutputPath(
      [](const SupplementaryOutputPaths &outs) -> const std::string & {
        return outs.dependenciesFilePath;
      });
}
bool FrontendInputsAndOutputs::HasReferenceDependenciesPath() const {
  return HasSupplementaryOutputPath(
      [](const SupplementaryOutputPaths &outs) -> const std::string & {
        return outs.referenceDependenciesFilePath;
      });
}
bool FrontendInputsAndOutputs::HasLoadedModuleTracePath() const {
  return HasSupplementaryOutputPath(
      [](const SupplementaryOutputPaths &outs) -> const std::string & {
        return outs.loadedModuleTracePath;
      });
}
bool FrontendInputsAndOutputs::HasModuleOutputPath() const {
  return HasSupplementaryOutputPath(
      [](const SupplementaryOutputPaths &outs) -> const std::string & {
        return outs.moduleOutputPath;
      });
}
bool FrontendInputsAndOutputs::HasModuleDocOutputPath() const {
  return HasSupplementaryOutputPath(
      [](const SupplementaryOutputPaths &outs) -> const std::string & {
        return outs.moduleDocOutputPath;
      });
}
bool FrontendInputsAndOutputs::HasModuleSourceInfoOutputPath() const {
  return HasSupplementaryOutputPath(
      [](const SupplementaryOutputPaths &outs) -> const std::string & {
        return outs.moduleSourceInfoOutputPath;
      });
}
bool FrontendInputsAndOutputs::HasModuleInterfaceOutputPath() const {
  return HasSupplementaryOutputPath(
      [](const SupplementaryOutputPaths &outs) -> const std::string & {
        return outs.moduleInterfaceOutputPath;
      });
}
bool FrontendInputsAndOutputs::HasPrivateModuleInterfaceOutputPath() const {
  return HasSupplementaryOutputPath(
      [](const SupplementaryOutputPaths &outs) -> const std::string & {
        return outs.privateModuleInterfaceOutputPath;
      });
}
bool FrontendInputsAndOutputs::HasABIDescriptorOutputPath() const {
  return HasSupplementaryOutputPath(
      [](const SupplementaryOutputPaths &outs) -> const std::string & {
        return outs.abiDescriptorOutputPath;
      });
}
bool FrontendInputsAndOutputs::HasModuleSemanticInfoOutputPath() const {
  return HasSupplementaryOutputPath(
      [](const SupplementaryOutputPaths &outs) -> const std::string & {
        return outs.moduleSemanticInfoOutputPath;
      });
}
bool FrontendInputsAndOutputs::HasModuleSummaryOutputPath() const {
  return HasSupplementaryOutputPath(
      [](const SupplementaryOutputPaths &outs) -> const std::string & {
        return outs.moduleSummaryOutputPath;
      });
}
bool FrontendInputsAndOutputs::HasTBDPath() const {
  return HasSupplementaryOutputPath(
      [](const SupplementaryOutputPaths &outs) -> const std::string & {
        return outs.tbdPath;
      });
}
bool FrontendInputsAndOutputs::HasYAMLOptRecordPath() const {
  return HasSupplementaryOutputPath(
      [](const SupplementaryOutputPaths &outs) -> const std::string & {
        return outs.yamlOptRecordPath;
      });
}
bool FrontendInputsAndOutputs::HasBitstreamOptRecordPath() const {
  return HasSupplementaryOutputPath(
      [](const SupplementaryOutputPaths &outs) -> const std::string & {
        return outs.bitstreamOptRecordPath;
      });
}

bool FrontendInputsAndOutputs::HasDependencyTrackerPath() const {
  return HasDependenciesPath() || HasReferenceDependenciesPath() ||
         HasLoadedModuleTracePath();
}

const PrimaryFileSpecificPaths &
FrontendInputsAndOutputs::GetPrimaryFileSpecificPathsForAtMostOnePrimary()
    const {
  AssertMustNotBeMoreThanOnePrimaryInputUnlessBatchModeChecksHaveBeenBypassed();
  static auto emptyPaths = PrimaryFileSpecificPaths();
  return HasInputs() ? FirstInputProducingOutput().GetPrimaryFileSpecificPaths()
                     : emptyPaths;
}

const PrimaryFileSpecificPaths &
FrontendInputsAndOutputs::GetPrimaryFileSpecificPathsForPrimary(
    StringRef filename) const {
  const FrontendInputFile *f = PrimaryInputNamed(filename);
  return f->GetPrimaryFileSpecificPaths();
}

const FrontendInputFile *
FrontendInputsAndOutputs::PrimaryInputNamed(StringRef name) const {
  assert(!name.empty() && "input files have names");
  StringRef correctedFile = FrontendInputFile::
      ConvertBufferNameFromLLVMGetFileOrSTDINToStoneConventions(name);
  auto iterator = primaryInputsByName.find(correctedFile);
  if (iterator == primaryInputsByName.end())
    return nullptr;
  const FrontendInputFile *f = &inputs[iterator->second];
  assert(f->IsPrimary() && "primaryInputsByName should only include primries");
  return f;
}
