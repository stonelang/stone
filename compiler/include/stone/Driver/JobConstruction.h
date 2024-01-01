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

} // namespace stone

#endif
