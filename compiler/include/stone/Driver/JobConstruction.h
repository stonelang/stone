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
  JobConstruction(CompilationEntityKind kind, CompilationEntityList inputs,
                  file::FileType fileType);

public:
  static bool classof(const CompilationEntity *entity) {
    return entity->IsJobConstruction();
  }
};

class IncrementalJobConstruction : public JobConstruction {
  // CompileStyle compileStyle;

public:
  IncrementalJobConstruction(CompilationEntityKind kind,
                             CompilationEntityList inputs,
                             file::FileType fileType)
      : JobConstruction(kind, inputs, fileType) {}

public:
  static bool classof(const CompilationEntity *entity) {
    return entity->IsCompileJobConstruction() ||
           entity->IsMergeModuleJobConstruction();
  }
};

class CompileJobConstruction final : public IncrementalJobConstruction {

public:
  /// In this scenario, we are creating one compile job with all inputs to be
  /// added.
  CompileJobConstruction(file::FileType outputFileType);

public:
  /// In this scenario, one compile job for eache input.
  CompileJobConstruction(const CompilationEntity *input,
                         file::FileType outputFileType);

public:
  static bool classof(const CompilationEntity *entity) {
    return entity->IsCompileJobConstruction();
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
  static bool classof(const CompilationEntity *entity) {
    return entity->IsMergeModuleJobConstruction();
  }

public:
  static MergeModuleJobConstruction *Create(const Driver &driver,
                                            CompilationEntityList inputs);
};

class ModuleWrapJobConstruction final : public JobConstruction {

public:
  ModuleWrapJobConstruction(CompilationEntityList inputs);

public:
  static bool classof(const CompilationEntity *entity) {
    return entity->IsModuleWrapJobConstruction();
  }

public:
  static ModuleWrapJobConstruction *Create(const Driver &driver,
                                           CompilationEntityList inputs);
};

class LinkJobConstruction : public JobConstruction {
  LinkMode linkMode;

public:
  LinkJobConstruction(CompilationEntityKind kind, CompilationEntityList inputs,
                      file::FileType outputFileType, LinkMode linkMode)
      : JobConstruction(kind, inputs, outputFileType), linkMode(linkMode) {}

public:
  LinkMode GetLinkMode() const { return linkMode; }

public:
  static bool classof(const CompilationEntity *entity) {
    return entity->IsLinkJobConstruction();
  }
};

class DynamicLinkJobConstruction final : public LinkJobConstruction {
  bool withLTO;

public:
  DynamicLinkJobConstruction(CompilationEntityList inputs, LinkMode linkMode,
                             bool withLTO = false);

public:
  // Job *Apply(const Driver &driver) override;
  bool WithLTO() const { return withLTO; }

public:
  static bool classof(const CompilationEntity *entity) {
    return entity->IsDynamicLinkJobConstruction();
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
    return entity->IsStaticLinkJobConstruction();
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
    return entity->IsBackendJobConstruction();
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

public:
  static bool classof(const CompilationEntity *entity) {
    return entity->IsGeneratePCHJobConstruction();
  }

public:
  static GeneratePCHJobConstruction *Create(Driver &driver,
                                            const CompilationEntity *input,
                                            llvm::StringRef persistentPCHDir);
};

class InterpretJobConstruction : public JobConstruction {
public:
  explicit InterpretJobConstruction()
      : JobConstruction(CompilationEntityKind::InterpretJobConstruction,
                        std::nullopt, file::FileType::None) {}

public:
  static bool classof(const CompilationEntity *entity) {
    return entity->IsInterpretJobConstruction();
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
    return entity->IsAutolinkExtractJobConstruction();
  }

  static AutolinkExtractJobConstruction *Create(const Driver &driver,
                                                CompilationEntityList inputs);
};

} // namespace stone

#endif
