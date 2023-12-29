#ifndef STONE_DRIVER_DRIVER_JOB_CONSTRUCTION_H
#define STONE_DRIVER_DRIVER_JOB_CONSTRUCTION_H

#include "stone/Basic/File.h"
#include "stone/Basic/STDAlias.h"
#include "stone/Driver/DriverAllocation.h"
#include "stone/Driver/DriverInvocation.h"

#include "llvm/ADT/PointerUnion.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/TinyPtrVector.h"
#include "llvm/Option/Arg.h"
#include "llvm/Option/ArgList.h"

namespace stone {
class JobConstruction;

enum class JobConstructionKind : UInt8 {
  None = 0,
  Compile,
  Backend,
  MergeModule,
  ModuleWrap,
  DynamicLink,
  StaticLink,
  First = Compile,
  Last = StaticLink
};

/// A job construction input can be a file or another job construction
/// (dependency)
using JobConstructionInput = llvm::PointerUnion<InputFile *, JobConstruction *>;

/// A list of all job construction inputs
using JobConstructionInputList = llvm::ArrayRef<JobConstructionInput>;

class JobConstruction : public DriverAllocation<JobConstruction> {
protected:
  JobConstructionKind kind = JobConstructionKind::None;
  file::Type fileType = file::Type::None;
  llvm::TinyPtrVector<JobConstructionInput> inputs;

public:
  JobConstruction(JobConstructionKind kind, JobConstructionInputList inputs,
                  file::Type fileType)
      : kind(kind), inputs(inputs), fileType(fileType) {}

public:
  using size_type = llvm::ArrayRef<JobConstructionInput>::size_type;
  using iterator = llvm::ArrayRef<JobConstructionInput>::iterator;
  using const_iterator = llvm::ArrayRef<JobConstructionInput>::const_iterator;

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
  JobConstructionKind GetKind() const { return kind; }
  file::Type GetFileType() { return fileType; }

public:
  static bool classof(const JobConstruction *construction) {
    return (construction->GetKind() >= JobConstructionKind::First &&
            construction->GetKind() <= JobConstructionKind::Last);
  }
};

class CompileJobConstruction final : public JobConstruction {

public:
  CompileJobConstruction(file::Type outputFileType);
  CompileJobConstruction(JobConstructionInput input, file::Type outputFileType);

public:
  static bool classof(const JobConstruction *construction) {
    return construction->GetKind() == JobConstructionKind::Compile;
  }
};

class DynamicLinkJobConstruction final : public JobConstruction {

  LinkMode linkMode;
  bool withLTO;

public:
  DynamicLinkJobConstruction(JobConstructionInputList inputs, LinkMode linkMode,
                             bool withLTO = false);

public:
  LinkMode GetLinkMode() const { return linkMode; }
  bool WithLTO() const { return withLTO; }

public:
  static bool classof(const JobConstruction *construction) {
    return construction->GetKind() == JobConstructionKind::DynamicLink;
  }
};

class StaticLinkJobConstruction final : public JobConstruction {
  LinkMode linkMode;

public:
  StaticLinkJobConstruction(JobConstructionInputList inputs, LinkMode linkMode);

public:
  LinkMode GetLinkMode() const { return linkMode; }

public:
  static bool classof(const JobConstruction *construction) {
    return construction->GetKind() == JobConstructionKind::StaticLink;
  }
};

class BackendJobConstruction final : public JobConstruction {
  size_t inputIndex;

public:
  BackendJobConstruction(JobConstructionInput input, file::Type outputFileType,
                         size_t inputIndex);

public:
  virtual size_t GetInputIndex() const override { return inputIndex; }

public:
  static bool classof(const JobConstruction *construction) {
    return construction->GetKind() == JobConstructionKind::Backend;
  }
};

} // namespace stone

#endif
