#ifndef STONE_DRIVER_DRIVER_COMPILATION_ENTITY_H
#define STONE_DRIVER_DRIVER_COMPILATION_ENTITY_H

#include "stone/Basic/File.h"
#include "stone/Basic/OptionSet.h"
#include "stone/Driver/DriverAllocation.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/TinyPtrVector.h"

namespace stone {
class Job;
class JobConstruct;
class Driver;
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
  BatchJob,
  First = Input,
  Last = Job
};

/// A list of all job construction inputs
using CompilationEntityList = llvm::ArrayRef<const CompilationEntity *>;

constexpr size_t CompilationEntityAlignInBits = 3;
class alignas(1 << CompilationEntityAlignInBits) CompilationEntity
    : public DriverAllocation<CompilationEntity> {

  CompilationEntityKind kind;
  file::FileType fileType;

public:
  enum class CompilationEntityFlags : uint8_t {
    None = 1 << 0,
    AllowTopLevel = 1 << 1,
    AllowFileType = 1 << 2,
  };

  using size_type = llvm::ArrayRef<const CompilationEntity *>::size_type;
  using iterator = llvm::ArrayRef<const CompilationEntity *>::iterator;
  using const_iterator =
      llvm::ArrayRef<const CompilationEntity *>::const_iterator;

  using CompilationEntityOptions = stone::OptionSet<CompilationEntityFlags>;
  CompilationEntityOptions compilationEntityOpts;

  using DriverAllocation<CompilationEntity>::operator new;
  using DriverAllocation<CompilationEntity>::operator delete;

protected:
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

  bool IsBatchJob() const {
    return GetKind() == CompilationEntityKind::BatchJob;
  }

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

class TopLevelCompilationEntity : public CompilationEntity {
  llvm::TinyPtrVector<const CompilationEntity *> inputs;

protected:
  TopLevelCompilationEntity(CompilationEntityKind kind,
                            CompilationEntityList inputs,
                            file::FileType fileType)
      : CompilationEntity(kind, fileType), inputs(inputs) {}

public:
  size_type size() const { return inputs.size(); }
  iterator begin() { return inputs.begin(); }
  iterator end() { return inputs.end(); }
  const_iterator begin() const { return inputs.begin(); }
  const_iterator end() const { return inputs.end(); }

  // Returns the index of the Input action's output file which is used as
  // (single) input to this action. Most actions produce only a single output
  // file, so we return 0 by default.
  virtual size_t GetInputIndex() const { return 0; }

public:
  void AddInput(const CompilationEntity *input) { inputs.push_back(input); }

public:
  static bool classof(const CompilationEntity *entity) {
    return (entity->GetKind() >=
                CompilationEntityKind::CompileJobConstruction &&
            entity->GetKind() <= CompilationEntityKind::Job);
  }
};

} // namespace stone

#endif