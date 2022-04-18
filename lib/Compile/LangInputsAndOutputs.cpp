#include "stone/Compile/LangInputsAndOutputs.h"
#include "stone/Basic/CompileDiagnostic.h"
#include "stone/Basic/Context.h"
#include "stone/Basic/File.h"
#include "stone/Basic/Range.h"
#include "stone/Compile/LangOptions.h"
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

LangInputsAndOutputs::LangInputsAndOutputs(const LangInputsAndOutputs &other) {
  for (LangInputFile input : other.inputs) {
    AddInput(input);
  }
  isSingleThreadedWMO = other.isSingleThreadedWMO;
  shouldRecoverMissingInputs = other.shouldRecoverMissingInputs;
}

LangInputsAndOutputs &
LangInputsAndOutputs::operator=(const LangInputsAndOutputs &other) {
  ClearInputs();
  for (LangInputFile input : other.inputs) {
    AddInput(input);
  }
  isSingleThreadedWMO = other.isSingleThreadedWMO;
  shouldRecoverMissingInputs = other.shouldRecoverMissingInputs;
  return *this;
}

// All inputs:

std::vector<std::string> LangInputsAndOutputs::GetLangInputFilenames() const {
  std::vector<std::string> filenames;
  for (auto &input : inputs) {
    filenames.push_back(input.GetFileName());
  }
  return filenames;
}

bool LangInputsAndOutputs::IsReadingFromStdin() const {
  return HasSingleInput() && GetFilenameOfFirstInput() == "-";
}

const std::string &LangInputsAndOutputs::GetFilenameOfFirstInput() const {
  assert(HasInputs());
  const LangInputFile &inp = inputs[0];
  const std::string &f = inp.GetFileName();
  assert(!f.empty());
  return f;
}

bool LangInputsAndOutputs::ForEachInput(
    llvm::function_ref<bool(const LangInputFile &)> fn) const {
  for (const LangInputFile &input : inputs) {
    if (fn(input)) {
      return true;
    }
  }
  return false;
}

// Primaries:

const LangInputFile &LangInputsAndOutputs::FirstPrimaryInput() const {
  assert(!primaryInputsInOrder.empty());
  return inputs[primaryInputsInOrder.front()];
}

const LangInputFile &LangInputsAndOutputs::LastPrimaryInput() const {
  assert(!primaryInputsInOrder.empty());
  return inputs[primaryInputsInOrder.back()];
}

bool LangInputsAndOutputs::ForEachPrimaryInput(
    llvm::function_ref<bool(const LangInputFile &)> fn) const {
  for (unsigned i : primaryInputsInOrder) {
    if (fn(inputs[i])) {
      return true;
    }
  }
  return false;
}

bool LangInputsAndOutputs::ForEachPrimaryInputWithIndex(
    llvm::function_ref<bool(const LangInputFile &, unsigned index)> fn) const {
  for (unsigned i : primaryInputsInOrder) {
    if (fn(inputs[i], i)) {
      return true;
    }
  }
  return false;
}

bool LangInputsAndOutputs::ForEachNonPrimaryInput(
    llvm::function_ref<bool(const LangInputFile &)> fn) const {
  return ForEachInput([&](const LangInputFile &f) -> bool {
    return f.IsPrimary() ? false : fn(f);
  });
}

void LangInputsAndOutputs::AssertMustNotBeMoreThanOnePrimaryInput() const {
  assert(!HasMultiplePrimaryInputs() &&
         "have not implemented >1 primary input yet");
}

void LangInputsAndOutputs::
    AssertMustNotBeMoreThanOnePrimaryInputUnlessBatchModeChecksHaveBeenBypassed()
        const {
  if (!AreBatchModeChecksBypassed()) {
    AssertMustNotBeMoreThanOnePrimaryInput();
  }
}

const LangInputFile *LangInputsAndOutputs::GetUniquePrimaryInput() const {
  AssertMustNotBeMoreThanOnePrimaryInput();
  return primaryInputsInOrder.empty() ? nullptr
                                      : &inputs[primaryInputsInOrder.front()];
}

const LangInputFile &
LangInputsAndOutputs::GetRequiredUniquePrimaryInput() const {
  if (const auto *input = GetUniquePrimaryInput()) {
    return *input;
  }
  llvm_unreachable("No primary when one is required");
}

std::string LangInputsAndOutputs::GetStatsFileMangledInputName() const {
  // Use the first primary, even if there are multiple primaries.
  // That's enough to keep the file names unique.
  return IsWholeModule() ? "all" : FirstPrimaryInput().GetFileName();
}

bool LangInputsAndOutputs::IsInputPrimary(StringRef file) const {
  return PrimaryInputNamed(file) != nullptr;
}

unsigned LangInputsAndOutputs::NumberOfPrimaryInputsEndingWith(
    StringRef extension) const {
  unsigned n = 0;
  (void)ForEachPrimaryInput([&](const LangInputFile &input) -> bool {
    if (llvm::sys::path::extension(input.GetFileName()).endswith(extension))
      ++n;
    return false;
  });
  return n;
}

// Input queries

bool LangInputsAndOutputs::ShouldTreatAsLLVM() const {
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

bool LangInputsAndOutputs::ShouldTreatAsModuleInterface() const {
  if (!HasSingleInput()) {
    return false;
  }

  StringRef InputExt = llvm::sys::path::extension(GetFilenameOfFirstInput());
  file::Type InputType = file::GetTypeByExt(InputExt);
  return InputType == file::Type::StoneModuleInterface;
}

bool LangInputsAndOutputs::VerifyInputs(DiagnosticEngine &diags,
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

void LangInputsAndOutputs::ClearInputs() {
  inputs.clear();
  primaryInputsByName.clear();
  primaryInputsInOrder.clear();
}

void LangInputsAndOutputs::AddInput(const LangInputFile &input) {
  const unsigned index = inputs.size();
  inputs.push_back(input);
  if (input.IsPrimary()) {
    primaryInputsInOrder.push_back(index);
    primaryInputsByName.insert({inputs.back().GetFileName(), index});
  }
}

void LangInputsAndOutputs::AddLangInputFile(StringRef file,
                                            llvm::MemoryBuffer *buffer) {
  AddInput(LangInputFile(file, false, buffer));
}

void LangInputsAndOutputs::AddPrimaryLangInputFile(StringRef file,
                                                   llvm::MemoryBuffer *buffer) {
  AddInput(LangInputFile(file, true, buffer));
}

unsigned LangInputsAndOutputs::CountOfInputsProducingMainOutputs() const {

  return IsSingleThreadedWMO() ? 1
         : HasPrimaryInputs()  ? PrimaryInputCount()
                               : InputCount();
}

const LangInputFile &LangInputsAndOutputs::FirstInputProducingOutput() const {

  return IsSingleThreadedWMO() ? FirstInput()
         : HasPrimaryInputs()  ? FirstPrimaryInput()
                               : FirstInput();
}

const LangInputFile &LangInputsAndOutputs::LastInputProducingOutput() const {
  return IsSingleThreadedWMO() ? FirstInput()
         : HasPrimaryInputs()  ? LastPrimaryInput()
                               : LastInput();
}

bool LangInputsAndOutputs::ForEachInputProducingAMainOutputFile(
    llvm::function_ref<bool(const LangInputFile &)> fn) const {

  return IsSingleThreadedWMO() ? fn(FirstInput())
         : HasPrimaryInputs()  ? ForEachPrimaryInput(fn)
                               : ForEachInput(fn);
}

void LangInputsAndOutputs::SetMainAndSupplementaryOutputs(
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

std::vector<std::string> LangInputsAndOutputs::CopyOutputFilenames() const {
  std::vector<std::string> outputs;
  (void)ForEachInputProducingAMainOutputFile(
      [&](const LangInputFile &input) -> bool {
        outputs.push_back(input.OutputFilename());
        return false;
      });
  return outputs;
}

std::vector<std::string>
LangInputsAndOutputs::CopyIndexUnitOutputFilenames() const {
  std::vector<std::string> outputs;
  (void)ForEachInputProducingAMainOutputFile(
      [&](const LangInputFile &input) -> bool {
        outputs.push_back(input.IndexUnitOutputFilename());
        return false;
      });
  return outputs;
}

void LangInputsAndOutputs::ForEachOutputFilename(
    llvm::function_ref<void(StringRef)> fn) const {
  (void)ForEachInputProducingAMainOutputFile(
      [&](const LangInputFile &input) -> bool {
        fn(input.OutputFilename());
        return false;
      });
}

std::string LangInputsAndOutputs::GetSingleOutputFilename() const {
  AssertMustNotBeMoreThanOnePrimaryInputUnlessBatchModeChecksHaveBeenBypassed();
  return HasInputs() ? LastInputProducingOutput().OutputFilename()
                     : std::string();
}

std::string LangInputsAndOutputs::GetSingleIndexUnitOutputFilename() const {
  AssertMustNotBeMoreThanOnePrimaryInputUnlessBatchModeChecksHaveBeenBypassed();
  return HasInputs() ? LastInputProducingOutput().IndexUnitOutputFilename()
                     : std::string();
}

bool LangInputsAndOutputs::IsOutputFilenameStdout() const {
  return GetSingleOutputFilename() == "-";
}

bool LangInputsAndOutputs::IsOutputFileDirectory() const {
  return HasNamedOutputFile() &&
         llvm::sys::fs::is_directory(GetSingleOutputFilename());
}

bool LangInputsAndOutputs::HasNamedOutputFile() const {
  return HasInputs() && !IsOutputFilenameStdout();
}

// Supplementary outputs

unsigned
LangInputsAndOutputs::CountOfFilesProducingSupplementaryOutput() const {
  return HasPrimaryInputs() ? PrimaryInputCount() : HasInputs() ? 1 : 0;
}

bool LangInputsAndOutputs::ForEachInputProducingSupplementaryOutput(
    llvm::function_ref<bool(const LangInputFile &)> fn) const {
  return HasPrimaryInputs() ? ForEachPrimaryInput(fn)
         : HasInputs()      ? fn(FirstInput())
                            : false;
}

bool LangInputsAndOutputs::HasSupplementaryOutputPath(
    llvm::function_ref<const std::string &(const SupplementaryOutputPaths &)>
        extractorFn) const {
  return ForEachInputProducingSupplementaryOutput(
      [&](const LangInputFile &input) -> bool {
        return !extractorFn(input.GetPrimaryFileSpecificPaths()
                                .supplementaryOutputPaths)
                    .empty();
      });
}

bool LangInputsAndOutputs::HasDependenciesPath() const {
  return HasSupplementaryOutputPath(
      [](const SupplementaryOutputPaths &outs) -> const std::string & {
        return outs.dependenciesFilePath;
      });
}
bool LangInputsAndOutputs::HasReferenceDependenciesPath() const {
  return HasSupplementaryOutputPath(
      [](const SupplementaryOutputPaths &outs) -> const std::string & {
        return outs.referenceDependenciesFilePath;
      });
}
bool LangInputsAndOutputs::HasLoadedModuleTracePath() const {
  return HasSupplementaryOutputPath(
      [](const SupplementaryOutputPaths &outs) -> const std::string & {
        return outs.loadedModuleTracePath;
      });
}
bool LangInputsAndOutputs::HasModuleOutputPath() const {
  return HasSupplementaryOutputPath(
      [](const SupplementaryOutputPaths &outs) -> const std::string & {
        return outs.moduleOutputPath;
      });
}
bool LangInputsAndOutputs::HasModuleDocOutputPath() const {
  return HasSupplementaryOutputPath(
      [](const SupplementaryOutputPaths &outs) -> const std::string & {
        return outs.moduleDocOutputPath;
      });
}
bool LangInputsAndOutputs::HasModuleSourceInfoOutputPath() const {
  return HasSupplementaryOutputPath(
      [](const SupplementaryOutputPaths &outs) -> const std::string & {
        return outs.moduleSourceInfoOutputPath;
      });
}
bool LangInputsAndOutputs::HasModuleInterfaceOutputPath() const {
  return HasSupplementaryOutputPath(
      [](const SupplementaryOutputPaths &outs) -> const std::string & {
        return outs.moduleInterfaceOutputPath;
      });
}
bool LangInputsAndOutputs::HasPrivateModuleInterfaceOutputPath() const {
  return HasSupplementaryOutputPath(
      [](const SupplementaryOutputPaths &outs) -> const std::string & {
        return outs.privateModuleInterfaceOutputPath;
      });
}
bool LangInputsAndOutputs::HasABIDescriptorOutputPath() const {
  return HasSupplementaryOutputPath(
      [](const SupplementaryOutputPaths &outs) -> const std::string & {
        return outs.abiDescriptorOutputPath;
      });
}
bool LangInputsAndOutputs::HasModuleSemanticInfoOutputPath() const {
  return HasSupplementaryOutputPath(
      [](const SupplementaryOutputPaths &outs) -> const std::string & {
        return outs.moduleSemanticInfoOutputPath;
      });
}
bool LangInputsAndOutputs::HasModuleSummaryOutputPath() const {
  return HasSupplementaryOutputPath(
      [](const SupplementaryOutputPaths &outs) -> const std::string & {
        return outs.moduleSummaryOutputPath;
      });
}
bool LangInputsAndOutputs::HasTBDPath() const {
  return HasSupplementaryOutputPath(
      [](const SupplementaryOutputPaths &outs) -> const std::string & {
        return outs.tbdPath;
      });
}
bool LangInputsAndOutputs::HasYAMLOptRecordPath() const {
  return HasSupplementaryOutputPath(
      [](const SupplementaryOutputPaths &outs) -> const std::string & {
        return outs.yamlOptRecordPath;
      });
}
bool LangInputsAndOutputs::HasBitstreamOptRecordPath() const {
  return HasSupplementaryOutputPath(
      [](const SupplementaryOutputPaths &outs) -> const std::string & {
        return outs.bitstreamOptRecordPath;
      });
}

bool LangInputsAndOutputs::HasDependencyTrackerPath() const {
  return HasDependenciesPath() || HasReferenceDependenciesPath() ||
         HasLoadedModuleTracePath();
}

const PrimaryFileSpecificPaths &
LangInputsAndOutputs::GetPrimaryFileSpecificPathsForAtMostOnePrimary() const {
  AssertMustNotBeMoreThanOnePrimaryInputUnlessBatchModeChecksHaveBeenBypassed();
  static auto emptyPaths = PrimaryFileSpecificPaths();
  return HasInputs() ? FirstInputProducingOutput().GetPrimaryFileSpecificPaths()
                     : emptyPaths;
}

const PrimaryFileSpecificPaths &
LangInputsAndOutputs::GetPrimaryFileSpecificPathsForPrimary(
    StringRef filename) const {
  const LangInputFile *f = PrimaryInputNamed(filename);
  return f->GetPrimaryFileSpecificPaths();
}

const LangInputFile *
LangInputsAndOutputs::PrimaryInputNamed(StringRef name) const {
  assert(!name.empty() && "input files have names");
  StringRef correctedFile =
      LangInputFile::ConvertBufferNameFromLLVMGetFileOrSTDINToStoneConventions(
          name);
  auto iterator = primaryInputsByName.find(correctedFile);
  if (iterator == primaryInputsByName.end())
    return nullptr;
  const LangInputFile *f = &inputs[iterator->second];
  assert(f->IsPrimary() && "primaryInputsByName should only include primries");
  return f;
}
