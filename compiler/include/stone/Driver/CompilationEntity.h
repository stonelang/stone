#ifndef STONE_DRIVER_DRIVER_COMPILATION_ENTITY_H
#define STONE_DRIVER_DRIVER_COMPILATION_ENTITY_H

#include "stone/Basic/File.h"
#include "stone/Basic/OptionSet.h"
#include "stone/Driver/DriverAllocation.h"

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/TinyPtrVector.h"
#include "llvm/ADT/ArrayRef.h"

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
  file::FileType fileType;

public:
  enum class CompilationEntityFlags : uint8_t {
    None = 1 << 0,
    AllowTopLevel = 1 << 1,
    AllowFileType = 1 << 2,
  };
  /// Options that control the JobConstruction
  using CompilationEntityOptions = stone::OptionSet<CompilationEntityFlags>;
  CompilationEntityOptions compilationEntityOpts;

public:
  using DriverAllocation<CompilationEntity>::operator new;
  using DriverAllocation<CompilationEntity>::operator delete;

protected:
  CompilationEntity(CompilationEntityKind kind);
  CompilationEntity(CompilationEntityKind kind, file::FileType fileType);

public:
  CompilationEntityKind GetKind() const { return kind; }
  file::FileType GetFileType() { return fileType; }

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

  bool HasFileType() { return fileType != file::FileType::None; }

  bool HasAllowTopLevel() const {
    return compilationEntityOpts.contains(
        CompilationEntityFlags::AllowTopLevel);
  }
  void AddAllowTopLevel() {
    compilationEntityOpts |= CompilationEntityFlags::AllowTopLevel;
  }
  void ClearAllowTopLevel() {}

  bool HasAllowFileType() const {
    return compilationEntityOpts.contains(
        CompilationEntityFlags::AllowTopLevel);
  }

  void AddAllowFileType() {
    assert(!IsJob());
    compilationEntityOpts |= CompilationEntityFlags::AllowFileType;
  }
  void ClearAllowFileType() {}

public:
  static bool classof(const CompilationEntity *entity) {
    return (entity->GetKind() >= CompilationEntityKind::First &&
            entity->GetKind() <= CompilationEntityKind::Last);
  }
};

// class TopLevelCompilationEntity : public CompilationEntity {
//   llvm::TinyPtrVector<CompilationEntity> inputs;

// protected:

//   TopLevelCompilationEntity(CompilationEntityKind kind,
//                             CompilationEntityList inputs,
//                             file::FileType fileType);
// };

} // namespace stone

#endif