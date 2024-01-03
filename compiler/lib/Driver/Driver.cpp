#include "stone/Driver/Driver.h"
#include "stone/Diag/CoreDiagnostic.h"
#include "stone/Diag/DriverDiagnostic.h"
#include "stone/Driver/DriverAllocation.h"
#include "stone/Driver/Job.h"
#include "stone/Strings.h"

#include "llvm/Support/Host.h"
#include "llvm/Support/Path.h"

using namespace stone;

using namespace llvm::opt;

Driver::Driver()
    : optTable(stone::CreateOptTable()),
      compilationEntities(new CompilationEntities()) {}

Driver::~Driver() {}

llvm::opt::InputArgList *
Driver::ParseArgStrings(llvm::ArrayRef<const char *> args) {

  unsigned includedFlagsBitmask = 0;
  unsigned excludedFlagsBitmask = opts::NoDriverOption;
  unsigned missingArgIndex;
  unsigned missingArgCount;

  inputArgList = std::make_unique<InputArgList>(
      GetOptTable().ParseArgs(args, missingArgIndex, missingArgCount,
                              includedFlagsBitmask, excludedFlagsBitmask));

  assert(inputArgList && "No input argument list.");
  if (missingArgCount) {
    diags.PrintD(SrcLoc(), diag::err_missing_arg_value,
                 diag::LLVMStr(inputArgList->getArgString(missingArgIndex)),
                 diag::UInt(missingArgCount));
    return nullptr;
  }
  // Check for unknown arguments.
  for (const llvm::opt::Arg *arg : inputArgList->filtered(opts::UNKNOWN)) {
    diags.PrintD(SrcLoc(), diag::err_unknown_arg,
                 diag::LLVMStr(arg->getAsString(*inputArgList)));
    return nullptr;
  }
  return inputArgList.get();
}

Status Driver::ConvertArgStrings(const llvm::opt::InputArgList &args) {
  return DriverOptionsConverter(args, driverOpts, *this).Convert();
}

void Driver::PrintHelp(bool showHidden) const {

  unsigned IncludedFlagsBitmask = 0;
  unsigned ExcludedFlagsBitmask = opts::NoDriverOption;

  if (!showHidden) {
    ExcludedFlagsBitmask |= HelpHidden;
  }
  GetOptTable().printHelp(
      llvm::outs(), GetDriverOptions().GetMainExecutableName().data(),
      "Stone is a compiler tool for compiling Stone source code.",
      IncludedFlagsBitmask, ExcludedFlagsBitmask,
      /*ShowAllAliases*/ false);
}

ToolChain *Driver::BuildToolChain(ToolChainKind toolChainKind) {
  switch (toolChainKind) {
  case ToolChainKind::Darwin:
    toolChain = std::make_unique<DarwinToolChain>(*this);
    break;
  case ToolChainKind::Linux:
    toolChain = std::make_unique<LinuxToolChain>(*this);
    break;
  case ToolChainKind::Windows:
    toolChain = std::make_unique<WindowsToolChain>(*this);
    break;
  default:
    llvm_unreachable("Unsupported OS -- cannot proceed with compilation!");
  }
  if (!toolChain) {
    return nullptr;
  }
  return toolChain.get();
}

std::unique_ptr<CompileStyle> Driver::BuildCompileStyle() {
  // Just return normal for now
  return std::make_unique<NormalCompileStyle>(*this);
}

Status CompileStyle::BuildCompilationEntities(CompilationEntities &entities) {
  llvm_unreachable("Illegal for for the base class CompileStyle");
}

Status
NormalCompileStyle::BuildCompilationEntities(CompilationEntities &entities) {

  driver.GetDriverOptions().GetInputsAndOutputs().ForEachInput(
      [&](const DriverInputFile *input) {
        // auto fileTypeExecution =
        // driver.GetFileTypeExection(input.GetFileType());
        // fileTypeExecution.BuildCompilationEntities();
      });

  return Status();
}

Status
SingleCompileStyle::BuildCompilationEntities(CompilationEntities &entities) {

  if (driver.GetDriverOptions().GetInputsAndOutputs().HasNoInputs()) {
    return Status::MakeHasCompletionAndIsError();
  }

  auto jobConstruction = CompileJobConstruction::Create(
      driver,
      driver.GetDriverOptions().GetDriverOutputInfo().GetOutputFileType());

  driver.GetDriverOptions().GetInputsAndOutputs().ForEachInput(
      [&](const DriverInputFile *input) { jobConstruction->AddInput(input); });

  // Because this is a single you may be able to do this -- but, since you are
  // linking, maybe, the linker may be the top level job
  auto jobs = jobConstruction->ConstructJobs(driver);

  return Status();
}

Compilation *Driver::BuildCompilation(const ToolChain &toolChain) {

  return nullptr;
}

void *stone::AllocateInDriver(size_t bytes, const stone::Driver &driver,
                              unsigned alignment) {
  return driver.Allocate(bytes, alignment);
}
