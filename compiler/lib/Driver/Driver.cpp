#include "stone/Driver/Driver.h"
#include "stone/Diag/DriverDiagnostic.h"
#include "stone/Option/Options.h"

using namespace stone;

using namespace llvm::opt;

Driver::Driver()
    : fileMgr(GetFileSystemOptions()), optTable(stone::CreateOptTable()) {}

std::unique_ptr<InputArgList>
Driver::ParseCommandLine(llvm::ArrayRef<const char *> args) {

  unsigned includedFlagsBitmask = 0;
  unsigned excludedFlagsBitmask = opts::NoDriverOption;
  unsigned missingArgIndex;
  unsigned missingArgCount;

  auto inputArgList = std::make_unique<InputArgList>(
      GetOptTable().ParseArgs(args, missingArgIndex, missingArgCount,
                              includedFlagsBitmask, excludedFlagsBitmask));

  assert(inputArgList && "No input argument list.");

  if (missingArgCount) {
    GetDiags().PrintD(
        SrcLoc(), diag::err_missing_arg_value,
        diag::LLVMStr(inputArgList->getArgString(missingArgIndex)),
        diag::UInt(missingArgCount));
    return nullptr;
  }
  // Check for unknown arguments.
  for (const llvm::opt::Arg *arg : inputArgList->filtered(opts::UNKNOWN)) {
    GetDiags().PrintD(SrcLoc(), diag::err_unknown_arg,
                      diag::LLVMStr(arg->getAsString(*inputArgList)));
    return nullptr;
  }
  if (GetDiags().HasError()) {
    return nullptr;
  }
  return inputArgList;
}

static Status ComputeLinkMode(Driver &driver) { return Status(); }
static Status ComputeCompilationKind(Driver &driver) { return Status(); }

std::unique_ptr<llvm::opt::DerivedArgList>
Driver::TranslateInputArgList(const InputArgList &inputArgList) {
  // Just return for now.
  auto derivedArgList =
      std::make_unique<llvm::opt::DerivedArgList>(inputArgList);

  derivedArgList;
}

Status Driver::ParseDriverOptions(const ArgList &args) {

  // First, get the action for the driver
  driverOpts.mainAction = opts::GetAction(args);

  return Status();
}
