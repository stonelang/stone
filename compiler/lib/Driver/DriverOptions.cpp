#include "stone/Driver/DriverOptions.h"
#include "stone/Driver/CompilationEntity.h"
#include "stone/Driver/Driver.h"

#include "llvm/Support/Host.h"
#include "llvm/Support/Path.h"

using namespace stone;
using namespace stone::file;
using namespace llvm::opt;

///< DriverOptions
DriverOptions::DriverOptions()
    : defaultTargetTriple(llvm::sys::getDefaultTargetTriple()) {}

bool DriverOptions::IsHelpAction() const { return mainAction.IsPrintHelp(); }
bool DriverOptions::IsHelpHiddenAction() const {
  return mainAction.IsPrintHelpHidden();
}
bool DriverOptions::IsPrintVersionAction() const {
  return mainAction.IsPrintVersion();
}

bool DriverOptions::IsCompilableAction() const {
  return mainAction.ShouldCompile();
}
bool DriverOptions::IsCompileOnlyAction() const {
  return (IsCompilableAction() && !IsLinkableAction());
}
bool DriverOptions::IsLinkableAction() const {
  return (driverOutputInfo.ShouldLink());
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

DriverArgList::DriverArgList(const llvm::opt::ArgList &args) : args(args) {}

const Arg *DriverArgList::GetLTO() const { return args.getLastArg(opts::LTO); }

const Arg *DriverArgList::GetLTOLibrary() const {
  return args.getLastArg(opts::LTOLibrary);
}

/// < DriverInputsConverter
DriverInputsConverter::DriverInputsConverter(const llvm::opt::ArgList &args,
                                             DriverOptions &driverOpts,
                                             Driver &driver)
    : args(args), driverOpts(driverOpts), driver(driver) {}

std::optional<DriverInputsAndOutputs> DriverInputsConverter::Convert() {

  // Just read for now
  if (ReadFilesFromCommandLine().IsError()) {
    return std::nullopt;
  }
  auto inputsAndOutputs = CreateInputFiles();
  return inputsAndOutputs;
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
                           StringRef(file));
  return Status::MakeHasCompletionAndIsError();
}

std::optional<DriverInputsAndOutputs>
DriverInputsConverter::CreateInputFiles() {

  DriverInputsAndOutputs inputsAndOutputs;
  for (auto &file : files) {
    inputsAndOutputs.AddInput(driver.CreateInput(file));
  }
  return inputsAndOutputs;
}

//< DriverOptionsConverter
DriverOptionsConverter::DriverOptionsConverter(const llvm::opt::ArgList &args,
                                               DriverOptions &driverOpts,
                                               Driver &driver)
    : args(args), driverOpts(driverOpts), driver(driver) {}

Status DriverOptionsConverter::Convert() {

  std::optional<DriverInputsAndOutputs> inputsAndOutputs =
      DriverInputsConverter(args, driverOpts, driver).Convert();

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
    driverOpts.inputsAndOutputs = std::move(inputsAndOutputs).getValue();
    if (driverOpts.allowModuleWithCompilerErrors) {
      driverOpts.GetInputsAndOutputs().SetShouldRecoverMissingInputs();
    }
  }
  /// TODO: should not have to call this again -- the above code should work
  if (!driverOpts.GetInputsAndOutputs().HasInputs()) {
    return Status::MakeHasCompletionAndIsError();
  }
  driverOpts.inputFileType =
      driverOpts.GetInputsAndOutputs().FirstInput()->GetFileType();

  driverOpts.mainAction = Action::Create(args);
  if (!driverOpts.HasMainAction()) {
    return Status::MakeHasCompletionAndIsError();
  }
  driverOpts.workingDirectory = ComputeWorkingDirectory();
  // if (!driverOpts.HasWorkingDirectory()) {
  //   return Status::MakeHasCompletionAndIsError();
  // }
  driverOpts.driverOutputInfo.compileInvocationMode =
      ComputeCompileInvocationMode();

  driverOpts.toolChainKind = ComputeToolChainKind();
  if (!driverOpts.HasToolChainKind()) {
    return Status::MakeHasCompletionAndIsError();
  }

  driverOpts.driverOutputInfo.linkMode = ComputeLinkMode();
  if (!driverOpts.GetDriverOutputInfo().ShouldLink()) {
    return Status::MakeHasCompletionAndIsError();
  }

  // TODO: Ok for now
  ComputeOutputInfo();

  return Status();
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

// TODO: Just return for now
CompileInvocationMode DriverOptionsConverter::ComputeCompileInvocationMode() {
  if (args.hasArg(opts::SingleCompileInvocation)) {
    return CompileInvocationMode::Single;
  }
  return CompileInvocationMode::Multiple;
}

ToolChainKind DriverOptionsConverter::ComputeToolChainKind() {

  if (const Arg *A = args.getLastArg(opts::Target)) {
    driverOpts.defaultTargetTriple = llvm::Triple::normalize(A->getValue());
  }
  llvm::Triple target(driverOpts.defaultTargetTriple);

  driverOpts.triple = target;

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
    driver.GetDiags().PrintD(
        SrcLoc(), diag::err_unknown_target,
        StringRef(args.getLastArg(opts::Target)->getValue()));
    return ToolChainKind::None;
  }
  }
  // TODO: Reset the triple
  return ToolChainKind::None;
}

LinkMode DriverOptionsConverter::ComputeLinkMode() {

  assert(driverOpts.HasMainAction());
  // Simple for now
  if (driverOpts.GetMainAction().IsNone()) {
    return LinkMode::Executable;
  } else if (driverOpts.GetMainAction().IsEmitLibrary()) {
    if (args.hasArg(opts::Static)) {
      return LinkMode::StaticLibrary;
    } else {
      return LinkMode::DynamicLibrary;
    }
  }
  return LinkMode::None;
}

void DriverOptionsConverter::SetTriple(llvm::Triple inputTriple) {
  driverOpts.triple = inputTriple;
}

void DriverOptionsConverter::ComputeOutputInfo() {

  auto modeOption = args.getLastArg(opts::ModeGroup);

  /// Just Object for now
  auto outputFileType =
      (driverOpts.driverOutputInfo.ltoVariant != LTOKind::None)
          ? FileType::BC
          : FileType::Object;

  driverOpts.driverOutputInfo.outputFileType = outputFileType;
}

void DriverOptionsConverter::ComputeGeneratePCH() {}

void DriverOptionsConverter::ComputeNumberOfParallelTasks() {}

// // llvm::Triple DriverOptionsConverter::ComputeTarget() {
// // }

// LTOKind DriverOptionsConverter::ComputeLTO() { return LTOKind::None; }
