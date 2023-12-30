#include "stone/Basic/Defer.h"
#include "stone/Driver/Driver.h"

using namespace stone;

class BuildJobConstructionsImpl final {
public:
  const Driver &driver;
  llvm::SmallVector<JobConstructionInput, 2> moduleInputs;
  llvm::SmallVector<JobConstructionInput, 2> linkerInputs;

public:
  BuildJobConstructionsImpl(const Driver &driver) : driver(driver) {}
  ~BuildJobConstructionsImpl() = default;

public:
  Status BuildForNormalCompileInvocation();
  Status BuildForSingleCompileInvocation();
  Status BuildForFlatCompileIvocation();

public:
  Status BuildCompileJobConstruction();
  Status BuildMergeModuleJobConstruction();
  Status BuildLinkJobConstruction();
  Status BuildBackendJobConstruction();

public:
  Status FinishBuildJobConstructions();
};

Status BuildJobConstructionsImpl::BuildForNormalCompileInvocation() {

  return Status();
}

Status BuildJobConstructionsImpl::BuildForSingleCompileInvocation() {

  return Status();
}

Status BuildJobConstructionsImpl::BuildForFlatCompileIvocation() {
  return Status();
}

Status BuildJobConstructionsImpl::BuildCompileJobConstruction() {

  if (driver.GetDriverOptions().IsCompilableAction()) {
  }

  return Status();
}
Status BuildJobConstructionsImpl::BuildMergeModuleJobConstruction() {

  return Status();
}

Status BuildJobConstructionsImpl::BuildLinkJobConstruction() {

  /// driver.AddTopLevelJobConstruction();

  return Status();
}

Status BuildJobConstructionsImpl::BuildBackendJobConstruction() {

  return Status();
}

Status BuildJobConstructionsImpl::FinishBuildJobConstructions() {

  if (driver.GetDriverOptions().IsLinkableAction()) {
    // BuildLinkJobConstruction();
  }
}

/// Build the job-constructions
Status Driver::BuildJobConstructions() {

  BuildJobConstructionsImpl buildJobConstructionsImpl(*this);
  STONE_DEFER { buildJobConstructionsImpl.FinishBuildJobConstructions(); };

  auto status = [&]() -> Status {
    switch (GetDriverOptions().GetCompileInvocationMode()) {
    case CompileInvocationMode::Normal:
      return buildJobConstructionsImpl.BuildForNormalCompileInvocation();
    case CompileInvocationMode::Single:
      return buildJobConstructionsImpl.BuildForSingleCompileInvocation();
    case CompileInvocationMode::Flat:
      return buildJobConstructionsImpl.BuildForFlatCompileIvocation();
    default:
      llvm_unreachable("Invalid compilation kind");
    }
  }();
}
