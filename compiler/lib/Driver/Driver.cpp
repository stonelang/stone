#include "stone/Driver/Driver.h"
#include "stone/Diag/CoreDiagnostic.h"
#include "stone/Diag/DriverDiagnostic.h"
#include "stone/Driver/DriverAllocation.h"
#include "stone/Driver/Job.h"
#include "stone/Strings.h"

#include "llvm/Support/Host.h"
#include "llvm/Support/Path.h"

using namespace stone;
using namespace stone::file;

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
  return DriverOptionsConverter(args, driverOpts, GetDiags()).Convert();
}

Status Driver::Setup(const llvm::opt::InputArgList &argList) {

  // return converter.Convert(buffers);

  auto workingDirectory = ComputeWorkingDirectory(argList);
  if (workingDirectory.empty()) {
    return Status::MakeHasCompletionAndIsError();
  }
  driverOpts.workingDirectory = workingDirectory;

  auto derivedArgList = TranslateInputArgList(argList, workingDirectory);
  if (!derivedArgList) {
    return Status::MakeHasCompletionAndIsError();
  }

  if (ComputeAction(*derivedArgList).IsError()) {
    return Status::MakeHasCompletionAndIsError();
  }

  if (GetDriverOptions().GetAction().IsSupport()) {
    PrintSupport();
    return Status::MakeHasCompletion();
  }

  if (BuildInputFiles(*derivedArgList, driverOpts.inputFiles).IsError()) {
    return Status::MakeHasCompletionAndIsError();
  }
  if (!GetDriverOptions().HasInputFiles()) {
    return Status::MakeHasCompletionAndIsError();
  }

  // auto toolChain = BuildToolChain(argList);
  // if (!toolChain) {
  //   return Status::MakeHasCompletionAndIsError();
  // }

  // driverOpts.compileInvocationMode =
  //     ComputeCompileInvocationMode(*derivedArgList);

  // BuildOutputs(*derivedArgList);

  // assert(driverOpts.HasOutputFileType() &&
  //        "Did not find a valid output file-type!");

  // Determine the OutputInfo for the driver.
  // OutputInfo OI;
  // bool BatchMode = false;
  // OI.CompilerMode = computeCompilerMode(*TranslatedArgList, Inputs,
  // BatchMode); buildOutputInfo(TC, *TranslatedArgList, BatchMode, Inputs, OI);

  // compilation = BuildCompilation(invocation.GetCompilationKind());

  // assert(HasCompilation());

  // if (compilation->BuildTopLevelJobConstructions().IsError()) {
  //   return Status::Error();
  // }

  // if (compilation->BuildJobs().IsError()) {
  //   return Status::Error();
  // }

  return Status();
}

llvm::StringRef
Driver::ComputeWorkingDirectory(const llvm::opt::InputArgList &argList) {
  if (auto *arg = argList.getLastArg(opts::WorkingDirectory)) {
    llvm::SmallString<128> workingDirectory;
    workingDirectory = arg->getValue();
    llvm::sys::fs::make_absolute(workingDirectory);
    return workingDirectory.str();
  }
  return llvm::StringRef();
}

llvm::opt::DerivedArgList *
Driver::TranslateInputArgList(const llvm::opt::InputArgList &argList,
                              llvm::StringRef workingDirectory) {
  // TODO:
  derivedArgList = std::make_unique<llvm::opt::DerivedArgList>(argList);
  return derivedArgList.get();
}

Status Driver::ComputeAction(const llvm::opt::DerivedArgList &argList) {

  driverOpts.action = stone::ComputeAction(argList);
  if (!GetDriverOptions().HasAction()) {
    return Status::Error();
  }
}

Status Driver::BuildInputFiles(const DerivedArgList &args,
                               InputFileList &inputFiles) {

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
      FileType fileType = FileType::None;
      // stdin must be handled specially.
      if (inputValue.equals(strings::Dash)) {
        // By default, treat stdin as Swift input.
        fileType = FileType::Stone;
      } else {
        // Otherwise lookup by extension.
        fileType = file::GetTypeByExt(inputValue);
        if (fileType == FileType::None) {
          // By default, treat inputs with no extension, or with an
          // extension that isn't recognized, as object files.
          fileType = FileType::Object;
        }
      }
      if (CheckInputFileExistence(inputValue).IsSuccess()) {
        inputFiles.push_back(InputFile(fileType, inputArg));
      }
      if (fileType == FileType::Stone) {
        auto basename = file::GetBase(inputValue);
        if (!sourceFileNames.insert({basename, inputValue}).second) {

          // diags.PrintD(SrcLoc(), diag::err_two_files_same_name,
          //                   diag::LLVMStr(basename),
          //                   diag::LLVMStr(sourceFileNames[basename]),
          //                   diag::LLVMStr(inputValue));
          // diags.PrintD(SrcLoc(), diag::note_explain_two_files_same_name);

          return Status::Error();
        }
      }
    }
  }
  return Status();
}

// ToolChainKind
// Driver::ComputeToolChainKind(const llvm::opt::InputArgList &argList) {

//   if (const Arg *A = argList.getLastArg(opts::Target)) {
//     driverOpts.defaultTargetTriple = llvm::Triple::normalize(A->getValue());
//   }
//   llvm::Triple target(driverOpts.defaultTargetTriple);
//   switch (target.getOS()) {
//   case llvm::Triple::Darwin:
//   case llvm::Triple::MacOSX: {
//     if (const Arg *A = argList.getLastArg(opts::TargetVariant)) {
//       driverOpts.targetVariant =
//           llvm::Triple(llvm::Triple::normalize(A->getValue()));
//     }
//     return ToolChainKind::Darwin;
//   }
//   case llvm::Triple::Linux: {
//     if (target.isAndroid()) {
//       return ToolChainKind::Android;
//     }
//     return ToolChainKind::Linux;
//   }
//   case llvm::Triple::FreeBSD: {
//     return ToolChainKind::FreeBSD;
//   }
//   case llvm::Triple::OpenBSD: {
//     return ToolChainKind::OpenBSD;
//   }
//   case llvm::Triple::Win32: {
//     return ToolChainKind::Windows;
//   }
//   case llvm::Triple::UnknownOS: {
//     return ToolChainKind::Unix;
//   }
//   default: {
//     diags.PrintD(SrcLoc(), diag::err_unknown_target,
//                  diag::LLVMStr(argList.getLastArg(opts::Target)->getValue()));
//     ToolChainKind::None;
//   }
//   }
//   ToolChainKind::None;
// }

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

CompileInvocationMode
Driver::ComputeCompileInvocationMode(const DerivedArgList &args) {

  // TODO: Just use normal for now
  //  bool useWMO = args.hasFlag(opts::WholeModuleOptimization,
  //                             opts::WithoutWholeModuleOptimization, false);

  // const arg *argRequiringSingleCompile = Args.getLastArg(
  //     options::opts::IndexFile,
  //     useWMO ? opts::WholeModuleOptimization : llvm::opt::OptSpecifier());

  return CompileInvocationMode::Normal;
}

Status Driver::BuildOutputs(const DerivedArgList &args) {

  if (const Arg *arg = args.getLastArg(opts::LTO)) {
    auto ltoVariant =
        llvm::StringSwitch<llvm::Optional<LTOKind>>(arg->getValue())
            .Case("llvm-thin", LTOKind::LLVMThin)
            .Case("llvm-full", LTOKind::LLVMFull)
            .Default(llvm::None);
    if (ltoVariant) {
      driverOpts.ltoVariant = ltoVariant.value();
    } else {
      // diags.PrintD(SrcLoc(), diag::err_invalid_arg_value,
      //                diag::LLVMStr(arg->getAsString(Args)),
      //                diag::LLVMStr(arg->getValue()));
    }
  }

  return Status();
}

// Status Driver::ComputeAction(const llvm::opt::DerivedArgList &argList) {}

// // std::unique_ptr<ToolChain> Driver::BuildToolChain(ToolChainKind kind) {
// //   switch (kind) {
// //   case ToolChainKind::Darwin:
// //     return std::make_unique<DarwinToolChain>(*this);
// //   // case ToolChainKind::Linux:
// //   //   return std::make_unique<LinuxToolChain>(*this);
// //   // case ToolChainKind::Windows:
// //   //   return std::make_unique<WindowsToolChain>(*this);
// //   // case ToolChainKind::FreeBSD:
// //   //   return std::make_unique<FreeBSDToolChain>(*this);
// //   // case ToolChainKind::OpenBSD:
// //   //   return std::make_unique<OpenBSDToolChain>(*this);
// //   // case ToolChainKind::Android:
// //   //   return std::make_unique<AndroidToolChain>(*this);
// //   // case ToolChainKind::Unix:
// //   //   return std::make_unique<UnixToolChain>(*this);
// //   default: {
// //     llvm_unreachable("Unsupported tool-chain");
// //   }
// //   }
// // }

// std::unique_ptr<Compilation> Driver::BuildCompilation(CompilationKind kind)
// {

//   assert(HasToolChain());
//   BuildingCompilationRAII buildingCompilation(*this);
//   switch (kind) {
//   case CompilationKind::Normal:
//     return BuildNormalCompilation(buildingCompilation);
//   case CompilationKind::Flat:
//     return BuildFlatCompilation(buildingCompilation);
//   case CompilationKind::CPUCount:
//     return BuildCPUCountCompilation(buildingCompilation);
//   case CompilationKind::Single:
//     return BuildSingleCompilation(buildingCompilation);
//   default:
//     llvm_unreachable(" Invalid compilation kind");
//   }
// }

// std::unique_ptr<Compilation>
// Driver::BuildNormalCompilation(BuildingCompilationRAII
// &buildingCompilation)
// {

//   /// BuildingCompilationRAII buildingCompilation(*this);

//   invocation.ForEachInputFile([&](InputFile &input) {
//     JobConstructionInput currentInput = static_cast<InputFile *>(&input);

//     switch (input.GetType()) {
//     case FileType::Stone: {

//       assert(file::IsPartOfCompilation(input.GetType()));

//       currentInput = CompileJobConstruction::Create(
//           *this, currentInput,
//           invocation.GetDriverOptions().outputFileType);

//       buildingCompilation.AddModuleInput(currentInput);

//       // Basic for now
//       buildingCompilation.AddLinkerInput(currentInput);
//       break;
//     }
//     case FileType::Object: {
//       if (invocation.ShouldLink() && invocation.IsLinkOnly()) {
//         buildingCompilation.AddLinkerInput(currentInput);
//       }
//       break;
//     }
//     default:
//       llvm_unreachable(" Invalid file type");
//     }
//   });

//   return nullptr;
// }
// std::unique_ptr<Compilation>
// Driver::BuildFlatCompilation(BuildingCompilationRAII &buildingCompilation)
// {

//   invocation.ForEachInputFile([&](InputFile &input) {
//     JobConstructionInput currentInput = const_cast<InputFile *>(&input);
//     currentInput = CompileJobConstruction::Create(
//         *this, currentInput,
//         invocation.GetDriverOptions().outputFileType);

//     buildingCompilation.AddModuleInput(currentInput);
//     buildingCompilation.AddLinkerInput(currentInput);
//   });

//   return nullptr;
// }

// std::unique_ptr<Compilation>
// Driver::BuildSingleCompilation(BuildingCompilationRAII
// &buildingCompilation)
// {

//   auto compileJobConstruction = CompileJobConstruction::Create(
//       *this, invocation.GetDriverOptions().outputFileType);

//   invocation.ForEachInputFile([&](InputFile &input) {
//     JobConstructionInput currentInput = const_cast<InputFile *>(&input);
//     compileJobConstruction->AddInput(currentInput);
//   });

//   buildingCompilation.AddModuleInput(compileJobConstruction);
//   buildingCompilation.AddLinkerInput(compileJobConstruction);

//   return nullptr;
// }

// std::unique_ptr<Compilation>
// Driver::BuildCPUCountCompilation(BuildingCompilationRAII
// &buildingCompilation) {

//   invocation.ForEachInputFile([&](InputFile &input) {

//   });

//   return nullptr;
// }

// Driver::BuildingCompilationRAII::~BuildingCompilationRAII() {

//   if (driver.GetInvocation().ShouldLink() && HasLinkerInputs()) {
//     JobConstruction *linkJobConstruction = nullptr;

//     // Add the linker inputs
//     switch (driver.GetInvocation().GetLinkMode()) {
//     case LinkMode::StaticLibrary: {
//       linkJobConstruction = StaticLinkJobConstruction::Create(
//           driver, linkerInputs, driver.GetInvocation().GetLinkMode());
//     }
//     default: {
//       // FIXME: WithLTO
//       linkJobConstruction = DynamicLinkJobConstruction::Create(
//           driver, linkerInputs, driver.GetInvocation().GetLinkMode(),
//           driver.GetInvocation().GetDriverOptions().WithLTO());
//     }
//       AddTopLevelJobConstruction(linkJobConstruction);
//     }
//     // TODO:
//     // On ELF platforms there's no built in autolinking mechanism, so we
//     // pull the info we need from the .o files directly and pass them as an
//     // argument input file to the linker.

//   } else {
//     // We can't rely on the merge module action being the only top-level
//     // action that needs to run. There may be other actions (e.g.
//     // BackendJobActions) that are not merge-module inputs but should be
//     run
//     // anyway.
//     // if (MergeModuleAction){
//     //   AddTopLevelJobConstruction(MergeModuleAction);
//     // }
//     // topLevelActions.append(AllLinkerInputs.begin(),
//     AllLinkerInputs.end());
//   }
// }

// std::unique_ptr<stone::TaskQueue>
// Driver::BuildTaskQueue(const Compilation &compilation) {

//   return nullptr;
// }

// Status Driver::BuildTopLevelJobConstructions(Compilation &compilation) {

//   return Status();
// }

// Status Driver::BuildTopLevelJobConstruction() {}

// Status Driver::BuildTopLevelJobs() {}

// /// Print the driver version.
// void Driver::PrintVersion(const ToolChain &toolChain, raw_ostream &os)
// const
// {}

void Driver::PrintHelp(bool showHidden) const {

  unsigned IncludedFlagsBitmask = 0;
  unsigned ExcludedFlagsBitmask = opts::NoDriverOption;

  if (!showHidden) {
    ExcludedFlagsBitmask |= HelpHidden;
  }
  GetOptTable().printHelp(
      llvm::outs(), GetDriverOptions().GetMainExecutableName().data(),
      "Stone compiler", IncludedFlagsBitmask, ExcludedFlagsBitmask,
      /*ShowAllAliases*/ false);
}

void Driver::PrintSupport() const {
  switch (GetDriverOptions().GetAction().GetKind()) {
  case ActionKind::PrintHelp:
    return PrintHelp();
  case ActionKind::PrintHelpHidden:
    return PrintHelp(true);
  case ActionKind::PrintVersion:
    // return PrintVersion(); // TODO:
    break;
  default:
    llvm_unreachable("Invalid support action!");
  }
}

void *stone::AllocateInDriver(size_t bytes, const stone::Driver &driver,
                              unsigned alignment) {
  return driver.Allocate(bytes, alignment);
}
