#ifndef STONE_DRIVER_DRIVER_JOB_CONSTRUCTION_H
#define STONE_DRIVER_DRIVER_JOB_CONSTRUCTION_H

#include "stone/Basic/FileType.h"
#include "stone/Basic/OptionSet.h"
#include "stone/Basic/STDAlias.h"
#include "stone/Driver/CompilationEntity.h"
#include "stone/Driver/DriverAllocation.h"
#include "stone/Driver/DriverOptions.h"

#include "llvm/ADT/PointerUnion.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Option/Arg.h"
#include "llvm/Option/ArgList.h"

namespace stone {
class Job;

class JobConstruction : public TopLevelCompilationEntity {

protected:
  class ApplyScope final : public DriverAllocation<ApplyScope> {
    JobConstruction *current;

  public:
    llvm::SmallVector<const CompilationEntity *, 4> inputEntities;
    llvm::SmallVector<const CompilationEntity *, 8> jobEntities;

  public:
    explicit ApplyScope(JobConstruction *current);
    ~ApplyScope();

  public:
    JobConstruction *GetCurrent() { return current; }
    void AddJob(const CompilationEntity *entity) {
      jobEntities.push_back(entity);
    }
    bool HasJobs() { return !jobEntities.empty() && jobEntities.size() > 0; }

    void AddInput(const CompilationEntity *entity) {
      inputEntities.push_back(entity);
    }
    bool HasInputs() {
      return !inputEntities.empty() && inputEntities.size() > 0;
    }
  };

  static ApplyScope *CreateApplyScope(Driver &driver, JobConstruction *current);

protected:
  JobConstruction(CompilationEntityKind kind, CompilationEntityList inputs,
                  file::FileType fileType);

protected:
  void ComputeJobMainOutput();
  virtual Job *Apply(Driver &driver, JobConstruction *current);

public:
  virtual Job *Apply(Driver &driver);

public:
  static bool classof(const CompilationEntity *entity) {
    return (entity->GetKind() >=
                CompilationEntityKind::CompileJobConstruction &&
            entity->GetKind() <=
                CompilationEntityKind::AutolinkExtractJobConstruction);
  }
};

class IncrementalJobConstruction : public JobConstruction {
  // CompileStyle compileStyle;

public:
  IncrementalJobConstruction(CompilationEntityKind kind,
                             CompilationEntityList inputs,
                             file::FileType fileType)
      : JobConstruction(kind, inputs, fileType) {}

protected:
  // Job *ConstructSelfJob(const Driver &driver) override {}
  //  virtual Job *ConstructInputJob(const Driver &driver){}

public:
  // Job *Apply(const Driver &driver) override {}

public:
  static bool classof(const CompilationEntity *entity) {
    return entity->GetKind() == CompilationEntityKind::CompileJobConstruction ||
           entity->GetKind() ==
               CompilationEntityKind::MergeModuleJobConstruction;
  }
};

class CompileJobConstruction final : public IncrementalJobConstruction {

protected:
  // Job *Apply(const Driver &driver, JobContruction* parent) override;

public:
  /// In this scenario, we are creating one compile job with all inputs to be
  /// added.
  CompileJobConstruction(file::FileType outputFileType);

public:
  /// In this scenario, one compile job for eache input.
  CompileJobConstruction(const CompilationEntity *input,
                         file::FileType outputFileType);

public:
  // Job *Apply(const Driver &driver) override;

public:
  static bool classof(const CompilationEntity *entity) {
    return entity->GetKind() == CompilationEntityKind::CompileJobConstruction;
  }

public:
  static CompileJobConstruction *Create(const Driver &driver,
                                        file::FileType outputFileType);
  static CompileJobConstruction *Create(const Driver &driver,
                                        const CompilationEntity *input,
                                        file::FileType outputFileType);
};
class MergeModuleJobConstruction final : public IncrementalJobConstruction {
public:
  MergeModuleJobConstruction(CompilationEntityList inputs);

public:
  // Job *Apply(const Driver &driver) override {}

public:
  static bool classof(const CompilationEntity *entity) {
    return entity->GetKind() ==
           CompilationEntityKind::MergeModuleJobConstruction;
  }

public:
  static MergeModuleJobConstruction *Create(const Driver &driver,
                                            CompilationEntityList inputs);
};

class LinkJobConstruction : public JobConstruction {
  LinkMode linkMode;

protected:
  // Job *ConstructSelfJob(const Driver &driver) override {}

public:
  LinkJobConstruction(CompilationEntityKind kind, CompilationEntityList inputs,
                      file::FileType outputFileType, LinkMode linkMode)
      : JobConstruction(kind, inputs, outputFileType), linkMode(linkMode) {}

public:
  LinkMode GetLinkMode() const { return linkMode; }
  // Job *Apply(const Driver &driver) override {}

public:
  static bool classof(const CompilationEntity *entity) {
    return (entity->GetKind() ==
            CompilationEntityKind::StaticLinkJobConstruction) ||
           (entity->GetKind() ==
            CompilationEntityKind::DynamicLinkJobConstruction);
  }
};

class DynamicLinkJobConstruction final : public LinkJobConstruction {
  bool withLTO;

protected:
  // Job *ConstructSelfJob(const Driver &driver) override;

public:
  DynamicLinkJobConstruction(CompilationEntityList inputs, LinkMode linkMode,
                             bool withLTO = false);

public:
  // Job *Apply(const Driver &driver) override;
  bool WithLTO() const { return withLTO; }

public:
  static bool classof(const CompilationEntity *entity) {
    return entity->GetKind() ==
           CompilationEntityKind::DynamicLinkJobConstruction;
  }

public:
  static DynamicLinkJobConstruction *Create(Driver &driver,
                                            CompilationEntityList inputs,
                                            LinkMode linkMode,
                                            bool wthLTO = false);
};

class StaticLinkJobConstruction final : public LinkJobConstruction {

protected:
  // Job *Apply(const Driver &driver, JobConstructin* parent) override;

public:
  StaticLinkJobConstruction(CompilationEntityList inputs, LinkMode linkMode);

public:
  // Job *Apply(const Driver &driver) override;

public:
  static bool classof(const CompilationEntity *entity) {
    return entity->GetKind() ==
           CompilationEntityKind::StaticLinkJobConstruction;
  }

public:
  static StaticLinkJobConstruction *
  Create(Driver &driver, CompilationEntityList inputs, LinkMode linkMode);
};

/// This is for BitCode process
class BackendJobConstruction final : public JobConstruction {
  size_t inputIndex;

public:
  BackendJobConstruction(const CompilationEntity *input,
                         file::FileType outputFileType, size_t inputIndex);

public:
  virtual size_t GetInputIndex() const override { return inputIndex; }

  // Job *Apply(const Driver &driver) override {}

public:
  static bool classof(const CompilationEntity *entity) {
    return entity->GetKind() == CompilationEntityKind::BackendJobConstruction;
  }

public:
  static BackendJobConstruction *Create(Driver &driver, CompilationEntity input,
                                        file::FileType outputFileType,
                                        size_t inputIndex);
};

class GeneratePCHJobConstruction final : public JobConstruction {
  std::string persistentPCHDir;

public:
  GeneratePCHJobConstruction(const CompilationEntity *input,
                             llvm::StringRef persistentPCHDir);

public:
  bool IsPersistentPCH() const { return !persistentPCHDir.empty(); }
  StringRef GetPersistentPCHDir() const { return persistentPCHDir; }

  // Job *Apply(const Driver &driver) override {}

public:
  static bool classof(const JobConstruction *construction) {
    return construction->GetKind() ==
           CompilationEntityKind::GeneratePCHJobConstruction;
  }

public:
  static GeneratePCHJobConstruction *Create(Driver &driver,
                                            CompilationEntity input,
                                            llvm::StringRef persistentPCHDir);
};

class InterpretJobConstruction : public JobConstruction {
public:
  explicit InterpretJobConstruction()
      : JobConstruction(CompilationEntityKind::InterpretJobConstruction,
                        llvm::None, file::FileType::None) {}

public:
  static bool classof(const CompilationEntity *entity) {
    return entity->GetKind() == CompilationEntityKind::InterpretJobConstruction;
  }
  static InterpretJobConstruction *Create(const Driver &driver);
};

class AutolinkExtractJobConstruction : public JobConstruction {

public:
  AutolinkExtractJobConstruction(CompilationEntityList inputs)
      : JobConstruction(CompilationEntityKind::AutolinkExtractJobConstruction,
                        inputs, file::FileType::Autolink) {}

public:
  static bool classof(const CompilationEntity *entity) {
    return entity->GetKind() ==
           CompilationEntityKind::AutolinkExtractJobConstruction;
  }

  static AutolinkExtractJobConstruction *Create(const Driver &driver,
                                                CompilationEntityList inputs);
};

} // namespace stone

#endif
