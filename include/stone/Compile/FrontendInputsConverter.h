#ifndef STONE_COMPILE_LANINPUTSSBUILDER_H
#define STONE_COMPILE_LANINPUTSSBUILDER_H

#include "stone/Compile/FrontendOptions.h"
#include "stone/Context.h"

#include "llvm/ADT/SetVector.h"
#include "llvm/Option/ArgList.h"
#include <set>

namespace stone {

class DiagnosticEngine;

class FrontendInputsConverter {
  DiagnosticEngine &de;
  const llvm::opt::ArgList &args;

  llvm::opt::Arg const *const fileListPathArg;
  llvm::opt::Arg const *const primaryFileListPathArg;
  llvm::opt::Arg const *const badFileDescriptorRetryCountArg;

  llvm::SetVector<llvm::StringRef> files;
  /// A place to keep alive any buffers that are loaded as part of setting up
  /// the frontend inputs.
  llvm::SmallVector<std::unique_ptr<llvm::MemoryBuffer>, 4>
      configurationFileBuffers;

public:
  FrontendInputsConverter(DiagnosticEngine &de, const llvm::opt::ArgList &args);

public:
  /// Produces a FrontendInputsAndOutputs object with the inputs populated from
  /// the arguments the converter was initialized with.
  ///
  /// \param buffers If present, buffers read in the processing of the frontend
  /// inputs will be saved here. These should only be used for debugging
  /// purposes.
  llvm::Optional<FrontendInputsAndOutputs>
  Convert(SmallVectorImpl<std::unique_ptr<llvm::MemoryBuffer>> *buffers);

private:
  bool EnforceFilelistExclusion();
  bool ReadInputFilesFromCommandLine();
  bool ReadInputFilesFromFilelist();
  bool ForAllFilesInFileList(llvm::opt::Arg const *const pathArg,
                             llvm::function_ref<void(StringRef)> caller);

  bool AddFile(llvm::StringRef file);
  llvm::Optional<std::set<StringRef>> ReadPrimaryFiles();

  /// Returns the newly set-up FrontendInputsAndOutputs, as well as a set of
  /// any unused primary files (those that do not correspond to an input).
  std::pair<FrontendInputsAndOutputs, std::set<llvm::StringRef>>
  CreateInputFilesConsumingPrimaries(std::set<llvm::StringRef> primaryFiles);

  /// Emits an error for each file in \p unusedPrimaryFiles.
  ///
  /// \returns true if \p unusedPrimaryFiles is non-empty.
  bool DiagnoseUnusedPrimaryFiles(std::set<llvm::StringRef> unusedPrimaryFiles);

  bool
  IsSingleThreadedWMO(const FrontendInputsAndOutputs &inputsAndOutputs) const;
};
} // namespace stone
#endif
