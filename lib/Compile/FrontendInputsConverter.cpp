#include "stone/Compile/FrontendInputsConverter.h"
#include "stone/Basic/Context.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/FrontendDiagnostic.h"
#include "stone/Compile/FrontendOptions.h"
#include "stone/Compile/FrontendOutputsConverter.h"

#include "llvm/Option/Arg.h"
#include "llvm/Option/ArgList.h"
#include "llvm/Option/Option.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/LineIterator.h"
#include "llvm/Support/Path.h"

using namespace stone;
using namespace llvm::opt;

FrontendInputsConverter::FrontendInputsConverter(DiagnosticEngine &de,
                                                 const llvm::opt::ArgList &args)
    : de(de), args(args), fileListPathArg(args.getLastArg(opts::FileList)),
      primaryFileListPathArg(args.getLastArg(opts::PrimaryFileList)),
      badFileDescriptorRetryCountArg(
          args.getLastArg(opts::BadFileDescriptorRetryCount)) {}

llvm::Optional<FrontendInputsAndOutputs> FrontendInputsConverter::Convert(
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
    return llvm::None;
  }

  if (fileListPathArg ? ReadInputFilesFromFilelist()
                      : ReadInputFilesFromCommandLine())
    return None;

  llvm::Optional<std::set<llvm::StringRef>> primaryFiles = ReadPrimaryFiles();
  if (!primaryFiles) {
    return llvm::None;
  }

  FrontendInputsAndOutputs result;
  std::set<llvm::StringRef> unusedPrimaryFiles;
  std::tie(result, unusedPrimaryFiles) =
      CreateInputFilesConsumingPrimaries(*primaryFiles);

  if (DiagnoseUnusedPrimaryFiles(unusedPrimaryFiles)) {
    return llvm::None;
  }

  // Must be set before iterating over inputs needing outputs.
  result.SetBypassBatchModeChecks(args.hasArg(opts::BypassBatchModeChecks));

  return std::move(result);
}

bool FrontendInputsConverter::EnforceFilelistExclusion() {
  if (args.hasArg(opts::INPUT) && fileListPathArg) {
    de.PrintD(SrcLoc(), diag::err_cannot_have_input_files_with_file_list);
    return true;
  }
  // The following is not strictly necessary, but the restriction makes
  // it easier to understand a given command line:
  if (args.hasArg(opts::PrimaryFile) && primaryFileListPathArg) {
    de.PrintD(SrcLoc(),
              diag::err_cannot_have_primary_files_with_primary_file_list);
    return true;
  }
  return false;
}

bool FrontendInputsConverter::ReadInputFilesFromCommandLine() {
  bool hadDuplicates = false;
  for (const Arg *A : args.filtered(opts::INPUT, opts::PrimaryFile)) {
    hadDuplicates = AddFile(A->getValue()) || hadDuplicates;
  }
  return false; // FIXME: Don't bail out for duplicates, too many tests depend
  // on it.
}

bool FrontendInputsConverter::ReadInputFilesFromFilelist() {
  bool hadDuplicates = false;
  bool hadError =
      ForAllFilesInFileList(fileListPathArg, [&](llvm::StringRef file) -> void {
        hadDuplicates = AddFile(file) || hadDuplicates;
      });
  if (hadError) {
    return true;
  }
  return false; // FIXME: Don't bail out for duplicates, too many tests depend
                // on it.
}

bool FrontendInputsConverter::ForAllFilesInFileList(
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
    de.PrintD(SrcLoc(), diag::err_invalid_arg_value,
              diag::LLVMStr(badFileDescriptorRetryCountArg->getAsString(args)),
              diag::LLVMStr(badFileDescriptorRetryCountArg->getValue()));
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
    de.PrintD(SrcLoc(), diag::err_cannot_open_file, diag::LLVMStr(path),
              diag::LLVMStr(filelistBufferOrError.getError().message()));
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

bool FrontendInputsConverter::AddFile(StringRef file) {
  if (files.insert(file)) {
    return false;
  }
  de.PrintD(SrcLoc(), diag::err_duplicate_input_file, diag::LLVMStr(file));
  return true;
}

Optional<std::set<StringRef>> FrontendInputsConverter::ReadPrimaryFiles() {
  std::set<StringRef> primaryFiles;
  for (const Arg *A : args.filtered(opts::PrimaryFile)) {
    primaryFiles.insert(A->getValue());
  }
  if (ForAllFilesInFileList(
          primaryFileListPathArg,
          [&](StringRef file) -> void { primaryFiles.insert(file); })) {
    return None;
  }
  return primaryFiles;
}

std::pair<FrontendInputsAndOutputs, std::set<llvm::StringRef>>
FrontendInputsConverter::CreateInputFilesConsumingPrimaries(
    std::set<llvm::StringRef> primaryFiles) {

  bool hasAnyPrimaryFiles = !primaryFiles.empty();
  FrontendInputsAndOutputs result;
  for (auto &file : files) {
    bool isPrimary = primaryFiles.count(file) > 0;
    result.AddInput(FrontendInputFile(file, isPrimary));
    if (isPrimary) {
      primaryFiles.erase(file);
    }
  }

  if (!files.empty() && !hasAnyPrimaryFiles) {
    llvm::Optional<std::vector<std::string>> userSuppliedNamesOrErr =
        FrontendOutputFilesComputer::
            GetOutputFilenamesFromCommandLineOrFileList(args, de, opts::o,
                                                        opts::OutputFileList);
    if (userSuppliedNamesOrErr && userSuppliedNamesOrErr->size() == 1) {
      result.SetIsSingleThreadedWMO(true);
    }
  }

  return {std::move(result), std::move(primaryFiles)};
}

bool FrontendInputsConverter::DiagnoseUnusedPrimaryFiles(
    std::set<StringRef> primaryFiles) {
  for (auto &file : primaryFiles) {
    // Catch "stone-compile  -c -filelist foo -primary-file
    // some-file-not-in-foo".
    assert(fileListPathArg && "Unused primary with no filelist");
    de.PrintD(SrcLoc(), diag::err_primary_file_not_found, diag::LLVMStr(file),
              diag::LLVMStr(fileListPathArg->getValue()));
  }
  return !primaryFiles.empty();
}
