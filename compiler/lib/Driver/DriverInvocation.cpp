#include "stone/Driver/DriverInvocation.h"
#include "stone/Diag/CoreDiagnostic.h"
#include "stone/Driver/Driver.h"
#include "stone/Driver/Job.h"
#include "stone/Strings.h"

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
  if (BuildInputFiles(argList, driverOpts.inputFiles).IsError()) {
    return Status::Error();
  }
  return Status();
}
Status DriverInvocation::ParseCompilationOptions(
    const const llvm::opt::ArgList &argList) {

  return Status();
}

Status DriverInvocation::ComputeLinkMode(const llvm::opt::ArgList &args) {

  assert(HasAction());
  if (GetAction().IsNone()) {
    driverOpts.linkMode = LinkMode::Executable;
  } else if (GetAction().IsEmitLibrary()) {
    if (args.hasArg(opts::Static)) {
      driverOpts.linkMode = LinkMode::StaticLibrary;
    } else {
      driverOpts.linkMode = LinkMode::DynamicLibrary;
    }
  }
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

Status DriverInvocation::MightHaveExplicitPrimaryInputs(
    const JobOutput &jobOutput) const {

  return Status();
}

Status DriverInvocation::BuildInputFiles(const llvm::opt::ArgList &args,
                                         InputFileList &inputFiles) const {

  llvm::DenseMap<llvm::StringRef, llvm::StringRef> sourceFileNames;
  auto CheckInputFileExistence = [&](llvm::StringRef inputFile) -> Status {
    if (!driverOpts.checkInputFileExistence) {
      return Status::Success();
    }
    // stdin always exists.
    if (inputFile == strings::Dash) {
      return Status::Success();
    }
    if (file::Exists(inputFile)) {
      return Status::Success();
    }
    return Status::Error();
  };
  for (Arg *inputArg : args) {
    if (inputArg->getOption().getKind() == Option::InputClass) {
      llvm::StringRef inputValue = inputArg->getValue();
      file::Type fileType = file::Type::None;
      // stdin must be handled specially.
      if (inputValue.equals(strings::Dash)) {
        // By default, treat stdin as Swift input.
        fileType = file::Type::Stone;
      } else {
        // Otherwise lookup by extension.
        fileType = file::GetTypeByExt(inputValue);
        if (fileType == file::Type::None) {
          // By default, treat inputs with no extension, or with an
          // extension that isn't recognized, as object files.
          fileType = file::Type::Object;
        }
      }
      if (CheckInputFileExistence(inputValue).IsSuccess()) {
        inputFiles.push_back(InputFile(fileType, inputArg));
      }
      if (fileType == file::Type::Stone) {
        auto basename = file::GetBase(inputValue);
        if (!sourceFileNames.insert({basename, inputValue}).second) {
          driver.GetDiags().PrintD(SrcLoc(), diag::err_two_files_same_name,
          						   diag::LLVMStr(basename),
                                   diag::LLVMStr(sourceFileNames[basename]),
                                   diag::LLVMStr(inputValue));
          driver.GetDiags().PrintD(SrcLoc(),
                                   diag::note_explain_two_files_same_name);
          return Status::Error();
        }
      }
    }
  }
}

void DriverInvocation::ForEachInputFile(
    std::function<void(InputFile &inputFile)> callback) {
  for (auto inputFile : driverOpts.inputFiles) {
    callback(inputFile);
  }
}

Driver &DriverInvocation::GetDriver() { return driver; }