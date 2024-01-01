#include "stone/Driver/Driver.h"
#include "stone/Driver/DriverOptions.h"
#include "stone/Driver/CompilationEntity.h"

#include "llvm/Support/Host.h"
#include "llvm/Support/Path.h"

using namespace stone;
using namespace llvm::opt;

///< DriverOptions
DriverOptions::DriverOptions()
    : defaultTargetTriple(llvm::sys::getDefaultTargetTriple()) {}

bool DriverOptions::IsSupportAction() const { return action.IsSupport(); }
bool DriverOptions::IsCompilableAction() const { return action.CanCompile(); }
bool DriverOptions::IsCompileOnlyAction() const {
  return (IsCompilableAction() && !IsLinkableAction());
}
bool DriverOptions::IsLinkableAction() const {
  return (driverOutputInfo.HasLinkMode());
}
bool DriverOptions::IsLinkOnlyAction() const {
  return (IsLinkableAction() && !IsCompilableAction());
}

/// < DriverInputsAndOutputs
DriverInputsAndOutputs::DriverInputsAndOutputs(
    const DriverInputsAndOutputs &other) {
  for (const DriverInputFile *input : other.inputs) {
    AddInput(input);
  }
}

DriverInputsAndOutputs &
DriverInputsAndOutputs::operator=(const DriverInputsAndOutputs &other) {
  ClearInputs();
  for (const DriverInputFile *input : other.inputs) {
    AddInput(input);
  }
  return *this;
}

void DriverInputsAndOutputs::AddInput(const DriverInputFile *input) {
  inputs.push_back(input);
}
void DriverInputsAndOutputs::ClearInputs() { inputs.clear(); }

void DriverInputsAndOutputs::ForEachInput(
    std::function<void(const DriverInputFile *)> callback) const {
  for (const DriverInputFile *input : inputs) {
    callback(input);
  }
}

/// < DriverInputsConverter
DriverInputsConverter::DriverInputsConverter(const llvm::opt::ArgList &args,
                                             DriverOptions &driverOpts,
                                             Driver &driver)
    : args(args), driverOpts(driverOpts), driver(driver) {}

llvm::Optional<DriverInputsAndOutputs> DriverInputsConverter::Convert() {

  // Just read for now
  if (ReadFilesFromCommandLine().IsError()) {
    return llvm::None;
  }
  auto inputsAndOutputs = CreateInputFiles();
  return std::move(inputsAndOutputs);
}

Status DriverInputsConverter::ReadFilesFromCommandLine() {

  Status duplicateInputFileStatus;
  for (const Arg *arg : args.filtered(opts::INPUT)) {
    duplicateInputFileStatus = AddFile(arg->getValue());
    if (duplicateInputFileStatus.IsError() &&
        !driverOpts.shouldProcessDuplicateInputFile) {
      return Status::MakeHasCompletionAndIsError();
    }
  }
  return Status();
}
Status DriverInputsConverter::AddFile(llvm::StringRef file) {
  if (files.insert(file)) {
    return Status();
  }
  driver.GetDiags().PrintD(SrcLoc(), diag::err_duplicate_input_file,
                           diag::LLVMStr(file));
  return Status::MakeHasCompletionAndIsError();
}

llvm::Optional<DriverInputsAndOutputs>
DriverInputsConverter::CreateInputFiles() {

  DriverInputsAndOutputs inputsAndOutputs;
  for (auto &file : files) {
    inputsAndOutputs.AddInput(DriverInputFile::Create(driver, file));
  }
  return std::move(inputsAndOutputs);
}
