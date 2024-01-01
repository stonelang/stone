#ifndef STONE_DRIVER_DRIVER_COMPILATION_ENTITY_H
#define STONE_DRIVER_DRIVER_COMPILATION_ENTITY_H

#include "stone/Driver/DriverAllocation.h"

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/TinyPtrVector.h"

namespace stone {
class CompilationEntity;

enum class CompilationEntityKind : uint8_t {
  Input = 0,
  CompileJobConstruction,
  BackendJobConstruction,
  GeneratePCHJobConstruction,
  MergeModuleJobConstruction,
  ModuleWrapJobConstruction,
  DynamicLinkJobConstruction,
  StaticLinkJobConstruction,
  Job,
  First = Input,
  Last = Job
};

/// A list of all job construction inputs
using CompilationEntityList = llvm::ArrayRef<const CompilationEntity *>;

class CompilationEntity : public DriverAllocation<CompilationEntity> {
  CompilationEntityKind kind;

public:
  using DriverAllocation<CompilationEntity>::operator new;
  using DriverAllocation<CompilationEntity>::operator delete;

public:
  CompilationEntity(CompilationEntityKind kind);

public:
  CompilationEntityKind GetKind() const { return kind; }

public:
  bool IsInput() const { return GetKind() == CompilationEntityKind::Input; }

  bool IsCompileJobConstruction() const {
    return GetKind() == CompilationEntityKind::CompileJobConstruction;
  }
  bool IsBackendJobConstruction() const {
    return GetKind() == CompilationEntityKind::BackendJobConstruction;
  }
  bool IsGeneratePCHJobConstruction() const {
    return GetKind() == CompilationEntityKind::GeneratePCHJobConstruction;
  }
  bool IsMergeModuleJobConstruction() const {
    return GetKind() == CompilationEntityKind::MergeModuleJobConstruction;
  }
  bool IsModuleWrapJobConstruction() const {
    return GetKind() == CompilationEntityKind::ModuleWrapJobConstruction;
  }
  bool IsDynamicLinkJobConstruction() const {
    return GetKind() == CompilationEntityKind::DynamicLinkJobConstruction;
  }
  bool IsStaticLinkJobConstruction() const {
    return GetKind() == CompilationEntityKind::StaticLinkJobConstruction;
  }
  bool IsJob() const { return GetKind() == CompilationEntityKind::Job; }

public:
  static bool classof(const CompilationEntity *entity) {
    return (entity->GetKind() >= CompilationEntityKind::First &&
            entity->GetKind() <= CompilationEntityKind::Last);
  }
};

} // namespace stone

#endif