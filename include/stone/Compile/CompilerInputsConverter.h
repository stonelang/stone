#ifndef STONE_COMPILE_COMPILERINPUTSCONVERTER_H
#define STONE_COMPILE_COMPILERINPUTSCONVERTER_H

#include "stone/Compile/CompilerInputsAndOutputs.h"
#include "stone/Compile/CompilerOptions.h"

#include "llvm/ADT/SetVector.h"
#include "llvm/Option/ArgList.h"
#include <set>

namespace stone {

class DiagnosticEngine;

class CompilerInputsConverter {
  CompilerOptions &compilerOpts;
  DiagnosticEngine &de;
  const llvm::opt::ArgList &args;

  llvm::opt::Arg const *const fileListPathArg;
  llvm::opt::Arg const *const primaryFileListPathArg;
  llvm::opt::Arg const *const badFileDescriptorRetryCountArg;

  llvm::SetVector<llvm::StringRef> files;
  /// A place to keep alive any buffers that are loaded as part of setting up
  /// the invocation inputs.
  llvm::SmallVector<std::unique_ptr<llvm::MemoryBuffer>, 4>
      configurationFileBuffers;

public:
  CompilerInputsConverter(DiagnosticEngine &de, const llvm::opt::ArgList &args,
                          CompilerOptions &compilerOpts);

public:
  /// Produces a CompilerInputsAndOutputs object with the inputs populated from
  /// the arguments the converter was initialized with.
  ///
  /// \param buffers If present, buffers read in the processing of the
  /// invocation inputs will be saved here. These should only be used for
  /// debugging purposes.
  std::optional<CompilerInputsAndOutputs>
  Convert(SmallVectorImpl<std::unique_ptr<llvm::MemoryBuffer>> *buffers);

private:
  bool EnforceFilelistExclusion();
  bool ReadInputFilesFromCommandLine();
  bool ReadInputFilesFromFilelist();

  bool ForAllFilesInFileList(llvm::opt::Arg const *const pathArg,
                             llvm::function_ref<void(StringRef)> caller);

  bool AddFile(llvm::StringRef file);
  std::optional<std::set<StringRef>> ReadPrimaryFiles();

  /// Returns the newly set-up CompilerInputsAndOutputs, as well as a set of
  /// any unused primary files (those that do not correspond to an input).
  std::pair<CompilerInputsAndOutputs, std::set<llvm::StringRef>>
  CreateInputFilesConsumingPrimaries(std::set<llvm::StringRef> primaryFiles);

  /// Emits an error for each file in \p unusedPrimaryFiles.
  ///
  /// \returns true if \p unusedPrimaryFiles is non-empty.
  bool DiagnoseUnusedPrimaryFiles(std::set<llvm::StringRef> unusedPrimaryFiles);

  bool
  IsSingleThreadedWMO(const CompilerInputsAndOutputs &inputsAndOutputs) const;
};
} // namespace stone
#endif
