#include "stone/Driver/DriverOptions.h"
#include "stone/Diag/DiagnosticEngine.h"
#include "stone/Diag/DriverDiagnostic.h"
#include "stone/Driver/DriverInputFile.h"

#include "llvm/Support/Host.h"
#include "llvm/Support/Path.h"

using namespace stone;
using namespace llvm::opt;

// Driver options
DriverOptions::DriverOptions()
    : defaultTargetTriple(llvm::sys::getDefaultTargetTriple()) {}

bool DriverOptions::IsSupportAction() const { return action.IsSupport(); }
bool DriverOptions::IsCompilableAction() const { return action.CanCompile(); }
bool DriverOptions::IsCompileOnlyAction() const {
  return (IsCompilableAction() && !IsLinkableAction());
}
bool DriverOptions::IsLinkableAction() const {
  return (linkMode != LinkMode::None);
}
bool DriverOptions::IsLinkOnlyAction() const {
  return (IsLinkableAction() && !IsCompilableAction());
}

// Iinputs and outputs
DriverInputsAndOutputs::DriverInputsAndOutputs(
    const DriverInputsAndOutputs &other) {
  for (DriverInputFile input : other.inputs) {
    AddInput(input);
  }
}

DriverInputsAndOutputs &
DriverInputsAndOutputs::operator=(const DriverInputsAndOutputs &other) {
  ClearInputs();
  for (DriverInputFile input : other.inputs) {
    AddInput(input);
  }
  return *this;
}

void DriverInputsAndOutputs::AddInput(const DriverInputFile &input) {
  inputs.push_back(input);
}
void DriverInputsAndOutputs::ClearInputs() { inputs.clear(); }

Status DriverInputsAndOutputs::ForEachInput(
    std::function<Status(const DriverInputFile &)> fn) const {
  for (const DriverInputFile &input : inputs) {
    if (fn(input).IsErrorOrHasCompletion()) {
      return Status::MakeHasCompletionAndIsError();
    }
  }
  return Status();
}

// Inputs converter
DriverInputsConverter::DriverInputsConverter(const llvm::opt::ArgList &args,
                                             DriverOptions &driverOpts,
                                             DiagnosticEngine &diags)
    : args(args), driverOpts(driverOpts), diags(diags) {}

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
  diags.PrintD(SrcLoc(), diag::err_duplicate_input_file, diag::LLVMStr(file));
  return Status::MakeHasCompletionAndIsError();
}

llvm::Optional<DriverInputsAndOutputs>
DriverInputsConverter::CreateInputFiles() {

  DriverInputsAndOutputs inputsAndOutputs;
  for (auto &file : files) {
    inputsAndOutputs.AddInput(DriverInputFile(file));
  }
  return std::move(inputsAndOutputs);
}

// Options converter
DriverOptionsConverter::DriverOptionsConverter(const llvm::opt::ArgList &args,
                                               DriverOptions &driverOpts,
                                               DiagnosticEngine &diags)
    : args(args), driverOpts(driverOpts), diags(diags) {}

Status DriverOptionsConverter::Convert() {

  llvm::Optional<DriverInputsAndOutputs> inputsAndOutputs =
      DriverInputsConverter(args, driverOpts, diags).Convert();

  if (!inputsAndOutputs) {
    return Status::MakeHasCompletionAndIsError();
  }
  if (!inputsAndOutputs->HasInputs()) {
    return Status::MakeHasCompletionAndIsError();
  }

  // Make sure that it is empty,
  bool haveNewInputsAndOutputs = false;
  if (driverOpts.GetInputsAndOutputs().HasInputs()) {
    assert(!inputsAndOutputs->HasInputs());
  } else {
    haveNewInputsAndOutputs = true;
    driverOpts.GetInputsAndOutputs() = std::move(inputsAndOutputs).getValue();
    // if (driverOpts.allowModuleWithCompilerErrors) {
    //   driverOpts.GetInputsAndOutputs().SetShouldRecoverMissingInputs();
    // }
  }

  /// TODO: should not have to call this again -- the above code should work
  if (!driverOpts.GetInputsAndOutputs().HasInputs()) {
    return Status::MakeHasCompletionAndIsError();
  }
  driverOpts.inputFileType =
      driverOpts.GetInputsAndOutputs().FirstInput().GetFileType();

  driverOpts.action = stone::ComputeAction(args);
  if (!driverOpts.HasAction()) {
    return Status::MakeHasCompletionAndIsError();
  }
  driverOpts.workingDirectory = ComputeWorkingDirectory();
  if (!driverOpts.HasWorkingDirectory()) {
    return Status::MakeHasCompletionAndIsError();
  }

  driverOpts.toolChainKind = ComputeToolChainKind();
  if (driverOpts.HasToolChainKind()) {
    return Status::MakeHasCompletionAndIsError();
  }
  driverOpts.compileInvocationMode = ComputeCompileInvocationMode();

  return Status();
}

ToolChainKind DriverOptionsConverter::ComputeToolChainKind() {

  if (const Arg *A = args.getLastArg(opts::Target)) {
    driverOpts.defaultTargetTriple = llvm::Triple::normalize(A->getValue());
  }
  llvm::Triple target(driverOpts.defaultTargetTriple);
  switch (target.getOS()) {
  case llvm::Triple::Darwin:
  case llvm::Triple::MacOSX: {
    if (const Arg *A = args.getLastArg(opts::TargetVariant)) {
      driverOpts.targetVariant =
          llvm::Triple(llvm::Triple::normalize(A->getValue()));
    }
    return ToolChainKind::Darwin;
  }
  case llvm::Triple::Linux: {
    if (target.isAndroid()) {
      return ToolChainKind::Android;
    }
    return ToolChainKind::Linux;
  }
  case llvm::Triple::FreeBSD: {
    return ToolChainKind::FreeBSD;
  }
  case llvm::Triple::OpenBSD: {
    return ToolChainKind::OpenBSD;
  }
  case llvm::Triple::Win32: {
    return ToolChainKind::Windows;
  }
  case llvm::Triple::UnknownOS: {
    return ToolChainKind::Unix;
  }
  default: {
    diags.PrintD(SrcLoc(), diag::err_unknown_target,
                 diag::LLVMStr(args.getLastArg(opts::Target)->getValue()));
    ToolChainKind::None;
  }
  }
  ToolChainKind::None;
}

llvm::StringRef DriverOptionsConverter::ComputeWorkingDirectory() {
  if (auto *arg = args.getLastArg(opts::WorkingDirectory)) {
    llvm::SmallString<128> workingDirectory;
    workingDirectory = arg->getValue();
    llvm::sys::fs::make_absolute(workingDirectory);
    return workingDirectory.str();
  }
  return llvm::StringRef();
}

CompileInvocationMode DriverOptionsConverter::ComputeCompileInvocationMode() {
  return CompileInvocationMode::Normal;
}

LTOKind DriverOptionsConverter::ComputeLTO() { return LTOKind::None; }

LinkMode DriverOptionsConverter::ComputeLinkMode() {

  assert(driverOpts.HasAction());

  // if (driverOpts.IsNone()) {
  //   driverOpts.linkMode = LinkMode::Executable;
  // } else if (GetAction().IsEmitLibrary()) {
  //   if (args.hasArg(opts::Static)) {
  //     driverOpts.linkMode = LinkMode::StaticLibrary;
  //   } else {
  //     driverOpts.linkMode = LinkMode::DynamicLibrary;
  //   }
  // }
}

// llvm::Triple DriverOptionsConverter::ComputeTarget() {

// }
