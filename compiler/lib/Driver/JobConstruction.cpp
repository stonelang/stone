#include "stone/Driver/JobConstruction.h"
#include "stone/Basic/FileType.h"
#include "stone/Driver/Driver.h"
#include "stone/Driver/DriverPrettyStackTrace.h"

using namespace stone;
using namespace stone::file;

JobConstruction::JobConstruction(CompilationEntityKind kind,
                                 CompilationEntityList inputs,
                                 file::FileType fileType)
    : TopLevelCompilationEntity(kind, inputs, fileType) {

  /// create the Job
  // You have the inputs, so you can add them : job->AddInput()
}

Job *JobConstruction::ConstructJob(Driver &driver, JobConstruction *jc) {

  CompilationEntityPrettyStackTrace entityTraceCrashInfo("building job", jc);

  // You are better of doing this in the driver
  // auto buildingTopLevelJobEntities =
  //     BuildingTopLevelJobEntities::Create(driver);

  // for (const CompilationEntity *entity : *jc) {

  //   if (auto jc = llvm::dyn_cast<JobConstruction>(entity)) {
  //     // this->ConstructJob(driver, *jc);
  //   } else {
  //     // Just check that this is indeed an input
  //     if (entity->IsInput()) {
  //       buildingTopLevelJobEntities->inputEntities.push_back(entity);
  //     }
  //   }
  // }

  // driver.GetToolChain().ConstructJob(jc, driver.GetCompilation(),
  //   buildingTopLevelJobEntities.jobEntities,
  //   buildingTopLevelJobEntities.inputEntities);

  //  jc.ForEachInput([&](const CompilationEntity *entity) {
  //   auto jc = llvm::dyn_cast<JobConstruction>(entity);

  //   jc->ConstructJob(driver);

  //   // auto topLevelJob =
  //   driver.CastToJobConstruction(entity)->ConstructJob();
  //   // auto topLevelJob = llmv::dyn_cast<Job>
  // });

  // if (construction->FirstInput()->IsInput()) {
  // }
  // switch(construction->FirstInput())
  /// If you override, then you do not have to perform this action
  // if (driver.ShouldGenerateModule() &&
  //     (llvm::isa<CompileJobConstruction>(parent) ||
  //      llvm::isa<MergeModuleJobConstruction>(parent))) {
  // }

  // assert(driver.HasToolChain());

  // driver.ComputeJobMainOutput();
  // return driver.GetToolChain().ConstructJob(this,
  // driver.GetCompilation(),
  // ....);

  return nullptr;
}
void JobConstruction::ComputeJobMainOutput() {}

Job *JobConstruction::ConstructJob(Driver &driver) {
  assert(HasIsTopLevel());
  return ConstructJob(driver, this);
}

CompileJobConstruction::CompileJobConstruction(FileType outputFileType)
    : IncrementalJobConstruction(CompilationEntityKind::CompileJobConstruction,
                                 llvm::None, outputFileType) {

  assert(file::IsOutputFileType(outputFileType));
}

CompileJobConstruction::CompileJobConstruction(const CompilationEntity *input,
                                               FileType outputFileType)
    : IncrementalJobConstruction(CompilationEntityKind::CompileJobConstruction,
                                 input, outputFileType) {

  assert(file::IsOutputFileType(outputFileType));
}

// Job *CompileJobConstruction::ConstructJob(const Driver &driver) {

//   assert(HasInputs());

//   // if (FirstInput()->IsInput()) {
//   //   // You have nothing to do but to build the job
//   // }

//   /// If ipu

//   /// if first input is an input file, then you just create the job
//   /// else, if the first input is a JobConstruction, then you must create a
//   job
//   /// for it like wise
//   // but, it would be nice to pass the parent just so that it can add itself
//   as
//   // an input into the parent job
//   return nullptr;
// }

CompileJobConstruction *
CompileJobConstruction::Create(const Driver &driver,
                               const CompilationEntity *input,
                               FileType outputFileType) {
  return new (driver) CompileJobConstruction(input, outputFileType);
}

CompileJobConstruction *
CompileJobConstruction::Create(const Driver &driver, FileType outputFileType) {
  return new (driver) CompileJobConstruction(outputFileType);
}

MergeModuleJobConstruction::MergeModuleJobConstruction(
    CompilationEntityList inputs)
    : IncrementalJobConstruction(
          CompilationEntityKind::MergeModuleJobConstruction, inputs,
          file::FileType::StoneModule) {}

MergeModuleJobConstruction *
MergeModuleJobConstruction::Create(const Driver &driver,
                                   CompilationEntityList inputs) {
  return new (driver) MergeModuleJobConstruction(inputs);
}

DynamicLinkJobConstruction::DynamicLinkJobConstruction(
    CompilationEntityList inputs, LinkMode linkMode, bool withLTO)
    : LinkJobConstruction(CompilationEntityKind::DynamicLinkJobConstruction,
                          inputs, FileType::Image, linkMode),
      withLTO(withLTO) {

  assert((linkMode != LinkMode::None) && (linkMode != LinkMode::StaticLibrary));
}

DynamicLinkJobConstruction *
DynamicLinkJobConstruction::Create(Driver &driver, CompilationEntityList inputs,
                                   LinkMode linkMode, bool withLTO) {

  return new (driver) DynamicLinkJobConstruction(inputs, linkMode, withLTO);
}

// Job *DynamicLinkJobConstruction::ConstructSelfJob(const Driver &driver) {

//   return nullptr;
// }
// Job *DynamicLinkJobConstruction::ConstructJob(const Driver &driver) {

//   return nullptr;
// }

StaticLinkJobConstruction::StaticLinkJobConstruction(
    CompilationEntityList inputs, LinkMode linkMode)
    : LinkJobConstruction(CompilationEntityKind::StaticLinkJobConstruction,
                          inputs, FileType::Image, linkMode) {
  assert(linkMode == LinkMode::StaticLibrary);
}

StaticLinkJobConstruction *
StaticLinkJobConstruction::Create(Driver &driver, CompilationEntityList inputs,
                                  LinkMode linkMode) {
  return new (driver) StaticLinkJobConstruction(inputs, linkMode);
}

// Job *StaticLinkJobConstruction::ConstructSelfJob(const Driver &driver) {
//   return nullptr;
// }

// Job *StaticLinkJobConstruction::ConstructJob(const Driver &driver) {
//   return nullptr;
// }

BackendJobConstruction::BackendJobConstruction(const CompilationEntity *input,
                                               FileType outputFileType,
                                               size_t inputIndex)
    : JobConstruction(CompilationEntityKind::BackendJobConstruction, input,
                      outputFileType),
      inputIndex(inputIndex) {

  assert(file::IsOutputFileType(outputFileType));
}

GeneratePCHJobConstruction::GeneratePCHJobConstruction(
    const CompilationEntity *input, llvm::StringRef persistentPCHDir)
    :

      JobConstruction(CompilationEntityKind::GeneratePCHJobConstruction, input,
                      persistentPCHDir.empty() ? FileType::PCH
                                               : FileType::None),
      persistentPCHDir(persistentPCHDir) {}