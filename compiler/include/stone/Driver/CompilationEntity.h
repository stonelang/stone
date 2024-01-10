#ifndef STONE_DRIVER_DRIVER_COMPILATION_ENTITY_H
#define STONE_DRIVER_DRIVER_COMPILATION_ENTITY_H

#include "stone/Basic/Color.h"
#include "stone/Basic/FileType.h"
#include "stone/Basic/OptionSet.h"
#include "stone/Driver/DriverAllocation.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/TinyPtrVector.h"
#include "llvm/Support/TrailingObjects.h"

namespace stone {
class Job;
class Driver;
class JobConstruction;
class Compilation;
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
  InterpretJobConstruction,
  AutolinkExtractJobConstruction,
  Job,
  BatchJob,
  First = Input,
  Last = BatchJob
};

/// A list of all job construction inputs
using CompilationEntityList = llvm::ArrayRef<const CompilationEntity *>;

constexpr size_t CompilationEntityAlignInBits = 8;
class alignas(1 << CompilationEntityAlignInBits) CompilationEntity
    : public DriverAllocation<CompilationEntity> {

  CompilationEntityKind kind;
  file::FileType fileType;

public:
  enum class CompilationEntityFlags : uint8_t {
    None = 1 << 0,
    AllowTopLevel = 1 << 1,
    IsTopLevel = 1 << 2,
    AllowFileType = 1 << 3,
    AllowOutput = 1 << 4,
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
  file::FileType GetFileType() const { return fileType; }

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
  bool IsLinkJobConstruction() const {
    return IsStaticLinkJobConstruction() || IsDynamicLinkJobConstruction();
  }

  bool IsJobConstruction() const {
    return CompilationEntity::IsJobConstruction(GetKind());
  }

  bool IsInterpretJobConstruction() const {
    return GetKind() == CompilationEntityKind::InterpretJobConstruction;
  }

  bool IsAutolinkExtractJobConstruction() const {
    return GetKind() == CompilationEntityKind::AutolinkExtractJobConstruction;
  }

  bool IsJob() const { return GetKind() == CompilationEntityKind::Job; }

  bool IsBatchJob() const {
    return GetKind() == CompilationEntityKind::BatchJob;
  }

  bool IsAnyJob() const { return CompilationEntity::IsJob(GetKind()); }

  bool HasFileType() { return fileType != file::FileType::None; }

public:
  virtual void Print(ColorStream &colorStream) const;

public:
  bool HasAllowTopLevel() const {
    return compilationEntityOpts.contains(
        CompilationEntityFlags::AllowTopLevel);
  }
  void AddAllowTopLevel() {
    assert(!IsInput());
    compilationEntityOpts |= CompilationEntityFlags::AllowTopLevel;
  }
  void ClearAllowTopLevel() {}

  bool HasIsTopLevel() const {
    return compilationEntityOpts.contains(CompilationEntityFlags::IsTopLevel);
  }
  void AddIsTopLevel() {
    assert(!IsInput());
    compilationEntityOpts |= CompilationEntityFlags::IsTopLevel;
  }
  void ClearIsTopLevel() {}

  bool HasAllowFileType() const {
    return compilationEntityOpts.contains(
        CompilationEntityFlags::AllowTopLevel);
  }

  void AddAllowFileType() {
    assert(!IsJob());
    compilationEntityOpts |= CompilationEntityFlags::AllowFileType;
  }
  void ClearAllowFileType() {}

  bool HasAllowOutput() const {
    return compilationEntityOpts.contains(CompilationEntityFlags::AllowOutput);
  }
  void AddAllowOutput() {
    assert(!IsInput());
    compilationEntityOpts |= CompilationEntityFlags::AllowOutput;
  }
  void ClearAllowOutput() {}
  void ClearAllFlags();

public:
  // JobConstruction *CastToJobConstruction() {
  //   return llvm::dyn_cast<JobConstruction>(this);
  // }
  // Job *CastToJob() { return llvm::dyn_cast<Job>(this); }

private:
  void ComputeAllFlags(CompilationEntityKind kind);

public:
  static bool classof(const CompilationEntity *entity) {
    return (entity->GetKind() >= CompilationEntityKind::First &&
            entity->GetKind() <= CompilationEntityKind::Last);
  }

public:
  static bool IsAny(CompilationEntityKind kind);
  static bool IsJobConstruction(CompilationEntityKind kind);
  static bool IsJob(CompilationEntityKind kind);
};

class TopLevelCompilationEntity : public CompilationEntity {
  llvm::TinyPtrVector<const CompilationEntity *> inputs;

protected:
  TopLevelCompilationEntity(CompilationEntityKind kind,
                            CompilationEntityList inputs,
                            file::FileType fileType);

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
  bool HasInputs() { return !inputs.empty() && inputs.size() > 0; }

  const CompilationEntity *FirstInput() {
    if (!HasInputs()) {
      return nullptr;
    }
    return inputs.front();
  }

  bool ForEachInput(
      std::function<void(const CompilationEntity *input)> callback) const {
    for (const CompilationEntity *entity : *this) {
      callback(entity);
    }
  }

public:
  static bool classof(const CompilationEntity *entity) {
    return (entity->GetKind() >=
                CompilationEntityKind::CompileJobConstruction &&
            entity->GetKind() <= CompilationEntityKind::BatchJob);
  }
};

} // namespace stone

#endif