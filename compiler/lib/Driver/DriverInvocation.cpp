#include "stone/Driver/DriverInvocation.h"
#include "stone/Driver/Driver.h"
#include "stone/Driver/Job.h"

using namespace stone;

DriverInvocation::DriverInvocation(Driver &driver)
    : driver(driver), optTable(stone::CreateOptTable()) {}

Status DriverInvocation::ParseCommandLine(llvm::ArrayRef<const char *> args) {

  unsigned includedFlagsBitmask = 0;
  unsigned excludedFlagsBitmask = opts::NoDriverOption;
  unsigned missingArgIndex;
  unsigned missingArgCount;

  inputArgList = std::make_unique<InputArgList>(
      GetOptTable().ParseArgs(args, missingArgIndex, missingArgCount,
                              includedFlagsBitmask, excludedFlagsBitmask));

  assert(inputArgList && "No input argument list.");
  if (missingArgCount) {
    driver.GetDiags().PrintD(
        SrcLoc(), diag::err_missing_arg_value,
        diag::LLVMStr(inputArgList->getArgString(missingArgIndex)),
        diag::UInt(missingArgCount));
    return Status::Error();
  }
  // Check for unknown arguments.
  for (const llvm::opt::Arg *arg : inputArgList->filtered(opts::UNKNOWN)) {
    driver.GetDiags().PrintD(SrcLoc(), diag::err_unknown_arg,
                             diag::LLVMStr(arg->getAsString(*inputArgList)));
    return Status::Error();
  }
  if (driver.GetDiags().HasError()) {
    return Status::Error();
  }

  if (TranslateInputArgList(GetInputArgList()).IsError()) {
    return Status::Error();
  }
  if (ParseDriverOptions(GetDerivedArgList()).IsError()) {
    return Status::Error();
  }
  return Status();
}

Status
DriverInvocation::TranslateInputArgList(const InputArgList &inputArgList) {
  // Just return for now.
  derivedArgList = std::make_unique<llvm::opt::DerivedArgList>(inputArgList);
  return Status();
}

Status DriverInvocation::ParseDriverOptions(const llvm::opt::ArgList &argList) {

  driverOpts.mainAction = opts::ParseAction(argList);
  if (GetAction().IsAlien()) {
    return Status::Error();
  }
  return Status();
}
Status DriverInvocation::ParseCompilationOptions(
    const const llvm::opt::ArgList &argList) {

  return Status();
}

Status DriverInvocation::ComputeLinkMode(const llvm::opt::ArgList &argList) {
  return Status();
}

Status
DriverInvocation::ComputeCompilationKind(const llvm::opt::ArgList &argList) {
  return Status();
}

Status
DriverInvocation::ComputeToolChainKind(const llvm::opt::ArgList &argList) {
  return Status();
}

Status DriverInvocation::ComputeInputFiles(const llvm::opt::ArgList &argList) {}

Status DriverInvocation::MightHaveExplicitPrimaryInputs(
    const JobOutput &jobOutput) const {

  return Status();
}

void DriverInvocation::ForEachInputFile(
    std::function<void(InputFile &inputFile)> callback) {
  for (auto inputFile : driverOpts.inputFiles) {
    callback(inputFile);
  }
}

Driver &DriverInvocation::GetDriver() { return driver; }