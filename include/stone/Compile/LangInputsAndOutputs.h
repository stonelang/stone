#ifndef STONE_COMPILE_LANGINPUTSANDOUTPUTS_H
#define STONE_COMPILE_LANGINPUTSANDOUTPUTS_H

#include "stone/Compile/LangInputFile.h"
#include "stone/Compile/PrimaryFileSpecificPaths.h"
#include "stone/Compile/SupplementaryOutputPaths.h"

#include "llvm/ADT/Hashing.h"
#include "llvm/ADT/StringMap.h"

#include <string>
#include <vector>

namespace llvm {
class MemoryBuffer;
}

namespace stone {

class DiagnosticEngine;
/// Information about all the inputs and outputs to the frontend.

class LangInputsAndOutputs {
  friend class LangInputsConverter;

  std::vector<LangInputFile> inputs;
  llvm::StringMap<unsigned> primaryInputsByName;
  std::vector<unsigned> primaryInputsInOrder;

  /// In Single-threaded WMO mode, all inputs are used
  /// both for importing and compiling.
  bool isSingleThreadedWMO = false;

  /// Punt where needed to enable batch mode experiments.
  bool areBatchModeChecksBypassed = false;

  /// Recover missing inputs. Note that recovery itself is users responsibility.
  bool shouldRecoverMissingInputs = false;

public:
  bool AreBatchModeChecksBypassed() const { return areBatchModeChecksBypassed; }
  void SetBypassBatchModeChecks(bool bbc) { areBatchModeChecksBypassed = bbc; }

  LangInputsAndOutputs() = default;
  LangInputsAndOutputs(const LangInputsAndOutputs &other);
  LangInputsAndOutputs &operator=(const LangInputsAndOutputs &other);

  // Whole-module-optimization (WMO) routines:

  // SingleThreadedWMO produces only main output file. In contrast,
  // multi-threaded WMO produces one main output per input, as single-file and
  // batch-mode do for each primary. Both WMO modes produce only one set of
  // supplementary outputs.

  bool IsSingleThreadedWMO() const { return isSingleThreadedWMO; }
  void SetIsSingleThreadedWMO(bool istw) { isSingleThreadedWMO = istw; }

  bool IsWholeModule() const { return !HasPrimaryInputs(); }

  bool ShouldRecoverMissingInputs() { return shouldRecoverMissingInputs; }
  void SetShouldRecoverMissingInputs() { shouldRecoverMissingInputs = true; }

  // Readers:
  // All inputs:
  ArrayRef<LangInputFile> GetInputs() const { return inputs; }
  std::vector<std::string> GetLangInputFilenames() const;

  /// \return nullptr if not a primary input file.
  const LangInputFile *PrimaryInputNamed(StringRef name) const;

  unsigned InputCount() const { return inputs.size(); }

  bool HasInputs() const { return !inputs.empty(); }
  bool HasSingleInput() const { return InputCount() == 1; }

  const LangInputFile &FirstInput() const { return inputs[0]; }
  LangInputFile &FirstInput() { return inputs[0]; }

  const LangInputFile &LastInput() const { return inputs.back(); }

  const std::string &GetFilenameOfFirstInput() const;

  bool IsReadingFromStdin() const;

  /// If \p fn returns true, exits early and returns true.
  bool ForEachInput(llvm::function_ref<bool(const LangInputFile &)> fn) const;

  // Primaries:

  const LangInputFile &FirstPrimaryInput() const;
  const LangInputFile &LastPrimaryInput() const;

  /// If \p fn returns true, exit early and return true.
  bool
  ForEachPrimaryInput(llvm::function_ref<bool(const LangInputFile &)> fn) const;

  /// Iterates over primary inputs, exposing their unique ordered index
  /// If \p fn returns true, exit early and return true.
  bool ForEachPrimaryInputWithIndex(
      llvm::function_ref<bool(const LangInputFile &, unsigned index)> fn) const;

  /// If \p fn returns true, exit early and return true.
  bool ForEachNonPrimaryInput(
      llvm::function_ref<bool(const LangInputFile &)> fn) const;

  unsigned PrimaryInputCount() const { return primaryInputsInOrder.size(); }

  // Primary count readers:

  bool HasUniquePrimaryInput() const { return PrimaryInputCount() == 1; }

  bool HasPrimaryInputs() const { return PrimaryInputCount() > 0; }

  bool HasMultiplePrimaryInputs() const { return PrimaryInputCount() > 1; }

  /// Fails an assertion if there is more than one primary input.
  /// Used in situations where only one primary input can be handled
  /// and where batch mode has not been implemented yet.
  void AssertMustNotBeMoreThanOnePrimaryInput() const;

  /// Fails an assertion when there is more than one primary input unless
  /// the experimental -bypass-batch-mode-checks argument was passed to
  /// the front end.
  /// FIXME: When batch mode is complete, this function should be obsolete.
  void
  AssertMustNotBeMoreThanOnePrimaryInputUnlessBatchModeChecksHaveBeenBypassed()
      const;

  // Count-dependend readers:

  /// \return the unique primary input, if one exIsts.
  const LangInputFile *GetUniquePrimaryInput() const;

  const LangInputFile &GetRequiredUniquePrimaryInput() const;

  /// FIXME: Should combine all primaries for the result
  /// instead of just answering "batch" if there is more than one primary.
  std::string GetStatsFileMangledInputName() const;

  bool IsInputPrimary(StringRef file) const;

  unsigned NumberOfPrimaryInputsEndingWith(StringRef extension) const;

  // Multi-facet readers

  // If we have exactly one input filename, and its extension is "bc" or "ll",
  // treat the input as LLVM_IR.
  bool ShouldTreatAsLLVM() const;
  bool ShouldTreatAsSIL() const;
  bool ShouldTreatAsModuleInterface() const;
  bool ShouldTreatAsObjCHeader() const;

  bool AreAllNonPrimariesSIB() const;

  /// \return true for error
  bool VerifyInputs(DiagnosticEngine &de, bool isNoneRequested) const;

  // Changing inputs

public:
  void ClearInputs();
  void AddInput(const LangInputFile &input);
  void AddLangInputFile(StringRef file, llvm::MemoryBuffer *buffer = nullptr);
  void AddPrimaryLangInputFile(StringRef file,
                               llvm::MemoryBuffer *buffer = nullptr);

  // Outputs
private:
  friend class LangOptionsConverter;
  friend struct InterfaceSubContextDelegateImpl;

  void SetMainAndAdditionalOutputs(
      ArrayRef<std::string> outputFiles,
      ArrayRef<SupplementaryOutputPaths> supplementaryOutputs,
      ArrayRef<std::string> outputFilesForIndexUnits = None);

public:
  unsigned CountOfInputsProducingMainOutputs() const;

  bool HasInputsProducingMainOutputs() const {
    return CountOfInputsProducingMainOutputs() != 0;
  }

  const LangInputFile &FirstInputProducingOutput() const;
  const LangInputFile &LastInputProducingOutput() const;

  /// Under single-threaded WMO, we pretend that the first input
  /// generates the main output, even though it will include code
  /// generated from all of them.
  ///
  /// If \p fn returns true, return early and return true.
  bool ForEachInputProducingAMainOutputFile(
      llvm::function_ref<bool(const LangInputFile &)> fn) const;

  std::vector<std::string> CopyOutputFilenames() const;
  std::vector<std::string> CopyIndexUnitOutputFilenames() const;

  void ForEachOutputFilename(llvm::function_ref<void(StringRef)> fn) const;

  /// Gets the name of the specified output filename.
  /// If multiple files are specified, the last one is returned.
  std::string GetSingleOutputFilename() const;

  /// Gets the name of the specified output filename to record in the index unit
  /// output files. If multiple are specified, the last one is returned.
  std::string GetSingleIndexUnitOutputFilename() const;

  bool IsOutputFilenameStdout() const;
  bool IsOutputFileDirectory() const;
  bool HasNamedOutputFile() const;

  // Additional outputs

  unsigned CountOfFilesProducingAdditionalOutput() const;

  /// If \p fn returns true, exit early and return true.
  bool ForEachInputProducingAdditionalOutput(
      llvm::function_ref<bool(const LangInputFile &)> fn) const;

  /// Assumes there is not more than one primary input file, if any.
  /// Otherwise, you would need to call GetPrimaryFileSpecificPathsForPrimary
  /// to tell it which primary input you wanted the outputs for.
  const PrimaryFileSpecificPaths &
  GetPrimaryFileSpecificPathsForAtMostOnePrimary() const;

  const PrimaryFileSpecificPaths &
      GetPrimaryFileSpecificPathsForPrimary(StringRef) const;

  bool HasAdditionalOutputPath(
      llvm::function_ref<const std::string &(const SupplementaryOutputPaths &)>
          extractorFn) const;

  bool HasDependenciesPath() const;
  bool HasReferenceDependenciesPath() const;
  bool HasLoadedModuleTracePath() const;
  bool HasModuleOutputPath() const;
  bool HasModuleDocOutputPath() const;
  bool HasModuleSourceInfoOutputPath() const;
  bool HasModuleInterfaceOutputPath() const;
  bool HasPrivateModuleInterfaceOutputPath() const;
  bool HasABIDescriptorOutputPath() const;
  bool HasModuleSemanticInfoOutputPath() const;
  bool HasModuleSummaryOutputPath() const;
  bool HasTBDPath() const;
  bool HasYAMLOptRecordPath() const;
  bool HasBitstreamOptRecordPath() const;
  bool HasDependencyTrackerPath() const;
};

} // namespace stone

#endif // STONE_COMPILE_LANGINPUTSANDOUTPUTS_H
