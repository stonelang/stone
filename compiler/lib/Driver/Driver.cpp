#include "stone/Driver/Driver.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/FileType.h"
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
    : optTable(stone::CreateOptTable()) {
}

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

void TopLevelCompilationEntities::ForEachTopLevelJobConstruction(
    std::function<void(const CompilationEntity *entity)> callback) {
  for (auto topLevelJobConstruction : topLevelJobConstructions) {
    callback(topLevelJobConstruction);
  }
}

/// Get each top level job
void TopLevelCompilationEntities::ForEachTopLevelJob(
    std::function<void(const CompilationEntity *entity)> callback) {
  for (auto topLevelJob : topLevelJobs) {
    callback(topLevelJob);
  }
}

/// Get each top level job
void TopLevelCompilationEntities::ForEachTopLevelExternalJob(
    std::function<void(const CompilationEntity *entity)> callback) {
  for (auto topLevelExternalJob : topLevelExternalJobs) {
    callback(topLevelExternalJob);
  }
}

BuildingCompilationEntitiesConsumer::BuildingCompilationEntitiesConsumer(
    Driver &driver)
    : driver(driver) {}

void BuildingCompilationEntitiesConsumer::CompletedCompilationEntity(
    const CompilationEntity *entity) {
  llvm_unreachable("Only sub-classes can make this call");
}

void BuildingCompilationEntitiesConsumer::Finish() {
  llvm_unreachable("Only sub-classes can make this call");
}

BuildingJobConstructionEntitiesConsumer::
    BuildingJobConstructionEntitiesConsumer(Driver &driver)
    : BuildingCompilationEntitiesConsumer(driver) {

  // assert(driver.GetDriverOptions().GetDriverOutputInfo().ShouldLink());
}

BuildingJobConstructionEntitiesConsumer *
BuildingJobConstructionEntitiesConsumer::Create(Driver &driver) {
  return new (driver) BuildingJobConstructionEntitiesConsumer(driver);
}

void BuildingJobConstructionEntitiesConsumer::CompletedCompilationEntity(
    const CompilationEntity *entity) {

  /// Note, this may not be a top level -- could just be an object file
  // asert(entity->IsTopLevel());

  /// TODO: Some checks
  AddCompilationEntity(entity);

  // why not just construct the job here
}

void BuildingJobConstructionEntitiesConsumer::Finish() {

  // if (HasTopLevelCompilationEntities()) {
  //   auto const outputInfo = driver.GetDriverOptions().GetDriverOutputInfo();
  //   if (outputInfo.IsStaticLibraryLink()) {
  //     auto topLevelJC = StaticLinkJobConstruction::Create(
  //         driver, entities, outputInfo.GetLinkMode());
  //     // topLevelJC->AddIsTopLevel();

  //     driver.GetTopLevelCompilationEntities().AddTopLevelJobConstruction(
  //         StaticLinkJobConstruction::Create(driver, entities,
  //                                           outputInfo.GetLinkMode()));
  //   } else {
  //     driver.GetTopLevelCompilationEntities().AddTopLevelJobConstruction(
  //         DynamicLinkJobConstruction::Create(
  //             driver, entities, outputInfo.GetLinkMode(),
  //             outputInfo.HasLTO()));
  //   }
  // }
}

// MergeModuleJobConstructionEntitiesConsumer::
//     MergeModuleJobConstructionEntitiesConsumer(Driver &driver)
//     : BuildingCompilationEntitiesConsumer(driver) {

//   assert(!driver.IsSingleCompileInvocation());
//   assert(driver.ShouldGenerateModule());
// }

// MergeModuleJobConstructionEntitiesConsumer *
// MergeModuleJobConstructionEntitiesConsumer::Create(Driver &driver) {

//   if (driver.IsSingleCompileInvocation()) {
//     return nullptr;
//   }

//   if (!driver.ShouldGenerateModule()) {
//     return nullptr;
//   }

//   return new (driver) MergeModuleJobConstructionEntitiesConsumer(driver);
// }

// void MergeModuleJobConstructionEntitiesConsumer::CompletedCompilationEntity(
//     const CompilationEntity *entity) {
//   // Add to the special Module

//   if (auto incrementalJobEntity =
//           llvm::dyn_cast<IncrementalJobConstruction>(entity)) {
//     // Take the upper bound of the status of any incremental inputs to
//     // ensure that the merge-modules job gets run if *any* input job is run.
//     // const auto conservativeStatus =
//     //     std::max(StatusBound.status, IJA->getInputInfo().status);
//     // // The modification time here is not important to the rest of the
//     // // incremental build. We take the upper bound in case an attempt to
//     // // compare the swiftmodule output's mod time and any input files is
//     // // made. If the compilation has been correctly scheduled, the
//     // // swiftmodule's mod time will always strictly exceed the mod time of
//     // // any of its inputs when we are able to skip it.
//     // const auto conservativeModTime = std::max(
//     //     StatusBound.previousModTime, IJA->getInputInfo().previousModTime);
//     // StatusBound = InputInfo{conservativeStatus, conservativeModTime};
//   }
// }

// void MergeModuleJobConstructionEntitiesConsumer::Finish() {

//   // Ok, now we can try to create the link job
//   // But, you have to create a special M
// }

// BuildingJobConstructionEntities::BuildingJobConstructionEntities(Driver
// &driver)
//     : driver(driver) {}

// void BuildingJobConstructionEntities::AddConsumer(
//     BuildingCompilationEntitiesConsumer *consumer) {
//   if (consumer) {
//     consumers.push_back(consumer);
//   }
// }

// Status BuildingJobConstructionEntities::BuildForCompileInvocation(
//     CompileInvocationMode compileStyle) {
//   switch (compileStyle) {
//   case CompileInvocationMode::Multiple:
//     return BuildForMultipleCompileInvocation();
//   case CompileInvocationMode::Single:
//     return BuildForSingleCompileInvocation();
//   case CompileInvocationMode::Batch:
//     return BuildForBatchCompileInvocation();
//   default:
//     llvm_unreachable("Invalid compile invocation kind");
//   }
// }

// CompileJobConstruction *
// BuildingJobConstructionEntities::CreateCompileJobConstruction(
//     const DriverInputFile *input) {

// // if (args.hasArg(opts::::EmbedBitCode)) {
// // }
// /// Check that it requires a PCH
// // CompileJobConstruction *compileJobConstruction = nullptr;
// if (input) {
//   return CompileJobConstruction::Create(
//       driver, input,
//       driver.GetDriverOptions().GetDriverOutputInfo().GetOutputFileType());
// }

// return CompileJobConstruction::Create(
//     driver,
//     driver.GetDriverOptions().GetDriverOutputInfo().GetOutputFileType());

// //   if (driver.IsSingleCompileInvocation()) {
// //   }
// // }

// if (IsTopLevelJobConstruction()) {
//   CompletedCompilationEntity(compileJobConstruction);

// } else {
//   driver.GetTopLevelCompilationEntities().AddTopLevelJobConstruction(
//       compileJobConstruction);
//   return compileJobConstruction;
// }
//}

// void BuildingJobConstructionEntities::CompletedCompilationEntity(
//     const CompilationEntity *entity) {

//   ForEachConsumer([&](BuildingCompilationEntitiesConsumer *consumer) {
//     consumer->CompletedCompilationEntity(entity);
//   });
// }

// Status BuildingJobConstructionEntities::BuildForMultipleCompileInvocation() {
//   assert(driver.IsMultipleCompileInvocation());

// driver.GetDriverOptions().GetInputsAndOutputs().ForEachInput(
//     [&](const DriverInputFile *input) {
//       switch (input->GetFileType()) {
//       case FileType::Stone: {
//         assert(input->IsPartOfStoneCompilation());
//         CompletedCompilationEntity(CreateCompileJobConstruction(input));
//         break;
//       }
//       case FileType::Object: {
//         CompletedCompilationEntity(input);
//         break;
//       }
//       default:
//         llvm_unreachable(" Invalid file type");
//       }
//     });

//   return Status();
// }
// Status BuildingJobConstructionEntities::BuildForSingleCompileInvocation() {
//   assert(driver.IsSingleCompileInvocation());

// auto compileJobConstruction = CreateCompileJobConstruction();
// assert(compileJobConstruction);

// driver.GetDriverOptions().GetInputsAndOutputs().ForEachInput(
//     [&](const DriverInputFile *input) {
//       assert(input->IsPartOfStoneCompilation());
//       auto currentInput = driver.CastToJobConstruction(input);
//       compileJobConstruction->AddInput(currentInput);
//     });
// CompletedCompilationEntity(compileJobConstruction);

// return Status();
//}
// Status BuildingJobConstructionEntities::BuildForBatchCompileInvocation() {
//   assert(driver.IsBatchCompileInvocation());

//   return Status();
// }

// void BuildingJobConstructionEntities::ForEachConsumer(
//     std::function<void(BuildingCompilationEntitiesConsumer *consumer)> fn) {
//   for (auto consumer : consumers) {
//     fn(consumer);
//   }
// }

// void BuildingJobConstructionEntities::Finish() {

//   ///
// }

// TopLevelJobEntitiesBuilder::TopLevelJobEntitiesBuilder(Driver &driver)
//     : driver(driver) {}

// Status TopLevelJobEntitiesBuilder::BuildTopLevelJobEntities(
//     TopLevelCompilationEntities &entities) {

//   entities.ForEachTopLevelJobConstruction([&](const CompilationEntity
//   *entity) {
//     if (auto *jc = llvm::dyn_cast<JobConstruction>(entity)) {
//       entities.AddTopLevelJob(BuildTopLevelJob(jc));
//     }
//     // auto jb =
//     //
//     jobEntitiesBuilder.BuildTopLevelJob(llvm::dyn_cast<JobConstruction>(entity));

//     // auto jc = llvm::dyn_cast<JobConstruction>(entity);
//     // auto jb = const_cast<JobConstruction*>(jc)->ConstructJob(*this);
//     // entities.AddTopLevelJob(jb);
//   });
//   return Status();
// }
// Job *TopLevelJobEntitiesBuilder::BuildTopLevelJob(const JobConstruction *jc)
// {

//   return nullptr;
// }

// void TopLevelJobEntitiesBuilder::Finish() {}

Status Driver::BuildTopLevelJobConstructionEntities(
    TopLevelCompilationEntities &entities) {

  auto consumer = BuildingJobConstructionEntitiesConsumer::Create(*this);

  STONE_DEFER {
    [&]() {
      // Do something here
      consumer->Finish();
    }();
  };

  auto buildCompileInvocation =
      [&](BuildingJobConstructionEntitiesConsumer *consumer) -> Status {
    switch (GetCompileInvocationMode()) {
    case CompileInvocationMode::Multiple:
      return BuildMultipleCompileInvocation(consumer);
    case CompileInvocationMode::Single:
      return BuildSingleCompileInvocation(consumer);
    case CompileInvocationMode::Batch:
      return BuildBatchCompileInvocation(consumer);
    }
    // Work around MSVC warning: not all control paths return a value
    llvm_unreachable("All switch cases were covered");
  }(consumer);

  if (buildCompileInvocation.IsErrorOrHasCompletion()) {
    return Status::MakeHasCompletionAndIsError();
  }
  // STONE_DEFER { jobConstructionEntitiesBuilder.Finish(); };

  // jobConstructionEntitiesBuilder.AddConsumer(
  //     BuildingJobConstructionEntitiesConsumer::Create(*this));

  // // jobConstructionEntitiesBuilder.AddConsumer(
  // //     MergeModuleJobConstructionEntitiesConsumer::Create(*this));

  // return jobConstructionEntitiesBuilder.BuildForCompileInvocation(
  //     GetCompileInvocationMode());
}

Status Driver::BuildMultipleCompileInvocation(
    BuildingCompilationEntitiesConsumer *consumer) {

  assert(IsMultipleCompileInvocation());
  GetDriverOptions().GetInputsAndOutputs().ForEachInput(
      [&](const DriverInputFile *input) {
        switch (input->GetFileType()) {
        case FileType::Stone: {
          assert(input->IsPartOfStoneCompilation());
          consumer->CompletedCompilationEntity(
              CreateCompileJobConstruction(input));
          break;
        }
        case FileType::Autolink:
        case FileType::Object: {
          consumer->CompletedCompilationEntity(input);
          break;
        }
        default:
          llvm_unreachable(" Invalid file type");
        }
      });
  return Status();
}

Status Driver::BuildSingleCompileInvocation(
    BuildingCompilationEntitiesConsumer *consumer) {

  assert(IsSingleCompileInvocation());
  auto compileJobConstruction = CreateCompileJobConstruction();

  assert(compileJobConstruction);

  GetDriverOptions().GetInputsAndOutputs().ForEachInput(
      [&](const DriverInputFile *input) {
        assert(input->IsPartOfStoneCompilation());
        auto currentInput = llvm::dyn_cast<JobConstruction>(input);
        compileJobConstruction->AddInput(currentInput);
      });
  consumer->CompletedCompilationEntity(compileJobConstruction);
}

Status Driver::BuildBatchCompileInvocation(
    BuildingCompilationEntitiesConsumer *consumer) {}

Status Driver::BuildTopLevelJobEntities(TopLevelCompilationEntities &entities) {

  // STONE_DEFER { jobEntitiesBuilder.Finish(); };
  // if (!entities.HasTopLevelJobConstructions()) {
  //   return Status::MakeHasCompletionAndIsError();
  // }
  // return jobEntitiesBuilder.BuildTopLevelJobEntities(entities);
}

CompileJobConstruction *
Driver::CreateCompileJobConstruction(const DriverInputFile *input) {

  // TODO: args.hasArg(opts::::EmbedBitCode))
  // Check that it requires a PCH

  if (input) {
    return CompileJobConstruction::Create(
        *this, input,
        GetDriverOptions().GetDriverOutputInfo().GetOutputFileType());
  }
  return CompileJobConstruction::Create(
      *this, GetDriverOptions().GetDriverOutputInfo().GetOutputFileType());
}

Status Driver::BuildTopLevelCompilationEntities(
    TopLevelCompilationEntities &entities) {

  auto status = BuildTopLevelJobConstructionEntities(entities);
  if (status.IsErrorOrHasCompletion()) {
    return Status::MakeHasCompletionAndIsError();
  }
  // status = BuildTopLevelJobEntities(entities);
  // if (status.IsErrorOrHasCompletion()) {
  //   return Status::MakeHasCompletionAndIsError();
  // }
  return Status();
}

Compilation *Driver::BuildCompilation(const ToolChain &toolChain) {

  auto status =
      BuildTopLevelCompilationEntities(GetTopLevelCompilationEntities());

  return nullptr;
}

void *stone::AllocateInDriver(size_t bytes, const stone::Driver &driver,
                              unsigned alignment) {
  return driver.Allocate(bytes, alignment);
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
