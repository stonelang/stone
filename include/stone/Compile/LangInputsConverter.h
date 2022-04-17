#ifndef STONE_COMPILE_LANINPUTSSBUILDER_H
#define STONE_COMPILE_LANINPUTSSBUILDER_H

#include "stone/Basic/Context.h"
#include "stone/Compile/LangOptions.h"

#include "llvm/ADT/SetVector.h"
#include "llvm/Option/ArgList.h"
#include <set>

namespace stone {

class DiagnosticEngine;

class LangInputsConverter {
  DiagnosticEngine &de;
  const llvm::opt::ArgList &args;

  llvm::opt::Arg const *const fileListPathArg;
  llvm::opt::Arg const *const primaryFileListPathArg;
  llvm::opt::Arg const *const badFileDescriptorRetryCountArg;

  llvm::SetVector<llvm::StringRef> inputFiles;
  /// A place to keep alive any buffers that are loaded as part of setting up
  /// the frontend inputs.
  llvm::SmallVector<std::unique_ptr<llvm::MemoryBuffer>, 4>
      configurationFileBuffers;

public:
  LangInputsConverter(DiagnosticEngine &de, const llvm::opt::ArgList &args);

public:
  /// Produces a LangInputsAndOutputs object with the inputs populated from
  /// the arguments the converter was initialized with.
  ///
  /// \param buffers If present, buffers read in the processing of the frontend
  /// inputs will be saved here. These should only be used for debugging
  /// purposes.
  llvm::Optional<LangInputsAndOutputs>
  Convert(SmallVectorImpl<std::unique_ptr<llvm::MemoryBuffer>> *buffers);

private:
  bool EnforceFilelistExclusion();
  bool ReadInputFilesFromCommandLine();
  bool ReadInputFilesFromFilelist();
  bool RorAllFilesInFilelist(llvm::opt::Arg const *const pathArg,
                             llvm::function_ref<void(StringRef)> caller);

  bool AddFile(llvm::StringRef file);
  llvm::Optional<std::set<StringRef>> ReadPrimaryFiles();

  /// Returns the newly set-up FrontendInputsAndOutputs, as well as a set of
  /// any unused primary files (those that do not correspond to an input).
  std::pair<LangInputsAndOutputs, std::set<llvm::StringRef>>
  CreateInputFilesConsumingPrimaries(std::set<llvm::StringRef> primaryFiles);

  /// Emits an error for each file in \p unusedPrimaryFiles.
  ///
  /// \returns true if \p unusedPrimaryFiles is non-empty.
  bool DiagnoseUnusedPrimaryFiles(std::set<llvm::StringRef> unusedPrimaryFiles);

  bool IsSingleThreadedWMO(const LangInputsAndOutputs &inputsAndOutputs) const;
};
} // namespace stone
#endif
