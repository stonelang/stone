#ifndef STONE_DRIVER_DRIVER_JOB_CONSTRUCTION_H
#define STONE_DRIVER_DRIVER_JOB_CONSTRUCTION_H

#include "stone/Basic/File.h"
#include "stone/Basic/OptionSet.h"
#include "stone/Basic/STDAlias.h"
#include "stone/Driver/CompilationEntity.h"
#include "stone/Driver/DriverOptions.h"

#include "llvm/ADT/PointerUnion.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Option/Arg.h"
#include "llvm/Option/ArgList.h"

namespace stone {

class JobConstruction : public TopLevelCompilationEntity {

protected:
  JobConstruction(CompilationEntityKind kind, CompilationEntityList inputs,
                  file::FileType fileType)
      : TopLevelCompilationEntity(kind, inputs, fileType) {
    AddAllowFileType();
  }

public:
  virtual llvm::ArrayRef<const Job *> ConstructJobs(const Driver &driver);

public:
  static bool classof(const CompilationEntity *entity) {
    return (
        entity->GetKind() >= CompilationEntityKind::CompileJobConstruction &&
        entity->GetKind() <= CompilationEntityKind::StaticLinkJobConstruction);
  }
};

class IncrementatlJobConstruction : public JobConstruction {
public:
  IncrementatlJobConstruction(CompilationEntityKind kind,
                              CompilationEntityList inputs,
                              file::FileType fileType)
      : JobConstruction(kind, inputs, fileType) {}

public:
  llvm::ArrayRef<const Job *> ConstructJobs(const Driver &driver) override;

public:
  static bool classof(const CompilationEntity *entity) {
    return entity->GetKind() == CompilationEntityKind::CompileJobConstruction ||
           entity->GetKind() ==
               CompilationEntityKind::MergeModuleJobConstruction;
  }
};

class CompileJobConstruction final : public IncrementatlJobConstruction {

public:
  /// In this scenario, we are creating one compile job with all inputs to be
  /// added.
  CompileJobConstruction(file::FileType outputFileType);

public:
  /// In this scenario, one compile job for eache input.
  CompileJobConstruction(const CompilationEntity *input,
                         file::FileType outputFileType);

public:
  llvm::ArrayRef<const Job *> ConstructJobs(const Driver &driver) override;

public:
  static bool classof(const CompilationEntity *entity) {
    return entity->GetKind() == CompilationEntityKind::CompileJobConstruction;
  }
};
class MergeModuleJobConstruction final : public IncrementatlJobConstruction {
public:
  MergeModuleJobConstruction(CompilationEntityList inputs)
      : IncrementatlJobConstruction(
            CompilationEntityKind::MergeModuleJobConstruction, inputs,
            file::FileType::StoneModule) {}

public:
  llvm::ArrayRef<const Job *> ConstructJobs(const Driver &driver) override;

public:
  static bool classof(const CompilationEntity *entity) {
    return entity->GetKind() ==
           CompilationEntityKind::MergeModuleJobConstruction;
  }
};

class LinkJobConstruction : public JobConstruction {
  LinkMode linkMode;

public:
  LinkJobConstruction(CompilationEntityKind kind, CompilationEntityList inputs,
                      file::FileType outputFileType, LinkMode linkMode)
      : JobConstruction(kind, inputs, outputFileType), linkMode(linkMode) {}

public:
  LinkMode GetLinkMode() const { return linkMode; }
  llvm::ArrayRef<const Job *> ConstructJobs(const Driver &driver) override;

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

public:
  DynamicLinkJobConstruction(CompilationEntityList inputs, LinkMode linkMode,
                             bool withLTO = false);

public:
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

public:
  StaticLinkJobConstruction(CompilationEntityList inputs, LinkMode linkMode);

public:
  static bool classof(const CompilationEntity *entity) {
    return entity->GetKind() ==
           CompilationEntityKind::StaticLinkJobConstruction;
  }

public:
  static StaticLinkJobConstruction *
  Create(Driver &driver, CompilationEntityList inputs, LinkMode linkMode);
};

class BackendJobConstruction final : public JobConstruction {
  size_t inputIndex;

public:
  BackendJobConstruction(const CompilationEntity *input,
                         file::FileType outputFileType, size_t inputIndex);

public:
  virtual size_t GetInputIndex() const override { return inputIndex; }

  llvm::ArrayRef<const Job *> ConstructJobs(const Driver &driver) override;

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

  llvm::ArrayRef<const Job *> ConstructJobs(const Driver &driver) override;

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

} // namespace stone

#endif
