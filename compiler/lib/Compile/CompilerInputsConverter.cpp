#include "stone/Compile/CompilerInputsConverter.h"
#include "stone/AST/DiagnosticsCompile.h"
#include "stone/Basic/Defer.h"
#include "stone/Compile/CompilerOptions.h"
#include "stone/Compile/CompilerOutputsConverter.h"

#include "llvm/Option/Arg.h"
#include "llvm/Option/ArgList.h"
#include "llvm/Option/Option.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/LineIterator.h"
#include "llvm/Support/Path.h"

using namespace stone;
using namespace stone::opts;
using namespace llvm::opt;

CompilerInputsConverter::CompilerInputsConverter(DiagnosticEngine &de,
                                                 const llvm::opt::ArgList &args,
                                                 CompilerOptions &compilerOpts)

    : de(de), args(args), compilerOpts(compilerOpts),
      fileListPathArg(args.getLastArg(opts::OPT_FileList)),
      primaryFileListPathArg(args.getLastArg(opts::OPT_PrimaryFileList)),
      badFileDescriptorRetryCountArg(
          args.getLastArg(opts::OPT_BadFileDescriptorRetryCount)) {}

std::optional<CompilerInputsAndOutputs> CompilerInputsConverter::Convert(
    llvm::SmallVectorImpl<std::unique_ptr<llvm::MemoryBuffer>> *buffers) {

  STONE_DEFER {
    if (buffers) {
      std::move(configurationFileBuffers.begin(),
                configurationFileBuffers.end(), std::back_inserter(*buffers));
      // Clearing the original list of buffers isn't strictly necessary, but
      // makes the behavior more sensible if we were to call convert() again.
      configurationFileBuffers.clear();
    }
  };

  if (EnforceFilelistExclusion()) {
    return std::nullopt;
  }

  if (fileListPathArg ? ReadInputFilesFromFilelist()
                      : ReadInputFilesFromCommandLine()) {
    return std::nullopt;
  }

  std::optional<std::set<llvm::StringRef>> primaryFiles = ReadPrimaryFiles();
  if (!primaryFiles) {
    return std::nullopt;
  }

  CompilerInputsAndOutputs inputsAndOutputs;
  std::set<llvm::StringRef> unusedPrimaryFiles;
  std::tie(inputsAndOutputs, unusedPrimaryFiles) =
      CreateInputFilesConsumingPrimaries(*primaryFiles);

  if (DiagnoseUnusedPrimaryFiles(unusedPrimaryFiles)) {
    return std::nullopt;
  }

  // Must be set before iterating over inputs needing outputs.
  inputsAndOutputs.SetBypassBatchModeChecks(
      args.hasArg(opts::OPT_BypassBatchModeChecks));

  return std::move(inputsAndOutputs);
}

bool CompilerInputsConverter::EnforceFilelistExclusion() {
  if (args.hasArg(opts::OPT_INPUT) && fileListPathArg) {
    de.diagnose(SrcLoc(), diag::error_cannot_have_input_files_with_file_list);
    return true;
  }
  // The following is not strictly necessary, but the restriction makes
  // it easier to understand a given command line:
  if (args.hasArg(opts::OPT_PrimaryFile) && primaryFileListPathArg) {
    de.diagnose(SrcLoc(),
                diag::error_cannot_have_primary_files_with_primary_file_list);
    return true;
  }
  return false;
}

bool CompilerInputsConverter::ReadInputFilesFromCommandLine() {
  bool hasDuplicate = false;
  for (const Arg *A : args.filtered(opts::OPT_INPUT, opts::OPT_PrimaryFile)) {
    hasDuplicate = AddFile(A->getValue());
    if (hasDuplicate && !compilerOpts.shouldProcessDuplicateInputFile) {
      return true;
    }
  }
  return false;
}

bool CompilerInputsConverter::ReadInputFilesFromFilelist() {
  bool hasDuplicate = false;
  bool hadError =
      ForAllFilesInFileList(fileListPathArg, [&](StringRef file) -> void {
        hasDuplicate = AddFile(file) || hasDuplicate;
      });

  if (hadError) {
    return true;
  }
  if (hasDuplicate && compilerOpts.shouldProcessDuplicateInputFile) {
    return true;
  }
  return false;
}

bool CompilerInputsConverter::ForAllFilesInFileList(
    Arg const *const pathArg, llvm::function_ref<void(StringRef)> fn) {

  if (!pathArg) {
    return false;
  }
  StringRef path = pathArg->getValue();

  // Honor -bad-file-descriptor-retry-count from the argument list
  unsigned RetryCount = 0;
  if (badFileDescriptorRetryCountArg &&
      llvm::StringRef(badFileDescriptorRetryCountArg->getValue())
          .getAsInteger(10, RetryCount)) {
    de.diagnose(SrcLoc(), diag::error_invalid_arg_value,
                badFileDescriptorRetryCountArg->getAsString(args),
                badFileDescriptorRetryCountArg->getValue());
    return true;
  }

  llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> filelistBufferOrError =
      nullptr;
  for (unsigned I = 0; I < RetryCount + 1; ++I) {
    filelistBufferOrError = llvm::MemoryBuffer::getFile(path);
    if (filelistBufferOrError) {
      break;
    }
    if (filelistBufferOrError.getError().value() != EBADF) {
      break;
    }
  }
  if (!filelistBufferOrError) {
    de.diagnose(SrcLoc(), diag::error_cannot_open_file, path,
                filelistBufferOrError.getError().message());
    return true;
  }
  for (auto file :
       llvm::make_range(llvm::line_iterator(*filelistBufferOrError->get()),
                        llvm::line_iterator())) {
    fn(file);
  }
  configurationFileBuffers.push_back(std::move(*filelistBufferOrError));
  return false;
}

bool CompilerInputsConverter::AddFile(llvm::StringRef file) {
  if (files.insert(file)) {
    return false;
  }
  de.diagnose(SrcLoc(), diag::error_duplicate_input_file, file);
  return true;
}

std::optional<std::set<StringRef>> CompilerInputsConverter::ReadPrimaryFiles() {
  std::set<StringRef> primaryFiles;
  for (const Arg *A : args.filtered(opts::OPT_PrimaryFile)) {
    primaryFiles.insert(A->getValue());
  }
  if (ForAllFilesInFileList(
          primaryFileListPathArg,
          [&](StringRef file) -> void { primaryFiles.insert(file); })) {
    return std::nullopt;
  }
  return primaryFiles;
}

std::pair<CompilerInputsAndOutputs, std::set<llvm::StringRef>>
CompilerInputsConverter::CreateInputFilesConsumingPrimaries(
    std::set<llvm::StringRef> primaryFiles) {

  bool hasAnyPrimaryFiles = !primaryFiles.empty();
  CompilerInputsAndOutputs invocationInputsAndOutputs;
  for (auto &file : files) {
    bool isPrimary = primaryFiles.count(file) > 0;
    invocationInputsAndOutputs.AddInput(CompilerInputFile(file, isPrimary));
    if (isPrimary) {
      primaryFiles.erase(file);
    }
  }

  if (!files.empty() && !hasAnyPrimaryFiles) {
    std::optional<std::vector<std::string>> userSuppliedNamesOrErr =
        CompilerOutputFilesComputer::
            GetOutputFilenamesFromCommandLineOrFileList(
                args, de, opts::OPT_o, opts::OPT_OutputFileList);
    if (userSuppliedNamesOrErr && userSuppliedNamesOrErr->size() == 1) {
      invocationInputsAndOutputs.SetIsSingleThreadedWMO(true);
    }
  }

  return {std::move(invocationInputsAndOutputs), std::move(primaryFiles)};
}

bool CompilerInputsConverter::DiagnoseUnusedPrimaryFiles(
    std::set<StringRef> primaryFiles) {
  for (auto &file : primaryFiles) {
    // Catch "stone-compile  -c -filelist foo -primary-file
    // some-file-not-in-foo".
    assert(fileListPathArg && "Unused primary with no filelist");
    de.diagnose(SrcLoc(), diag::error_primary_file_not_found, file,
                fileListPathArg->getValue());
  }
  return !primaryFiles.empty();
}
