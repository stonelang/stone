#ifndef STONE_DRIVER_DRIVER_JOB_CONSTRUCTION_H
#define STONE_DRIVER_DRIVER_JOB_CONSTRUCTION_H

#include "stone/Basic/File.h"
#include "stone/Basic/OptionSet.h"
#include "stone/Basic/STDAlias.h"
#include "stone/Driver/CompilationEntity.h"
#include "stone/Driver/DriverOptions.h"

#include "llvm/ADT/PointerUnion.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/TinyPtrVector.h"
#include "llvm/Option/Arg.h"
#include "llvm/Option/ArgList.h"

namespace stone {
class Job;
class JobConstruction;

enum class JobConstructionKind : UInt8 {
  // TODO: Remove None
  None = 0,
  Compile,
  Backend,
  GeneratePCH,
  MergeModule,
  ModuleWrap,
  DynamicLink,
  StaticLink,
  First = Compile,
  Last = StaticLink
};

/// A job construction input can be a file or another job construction
/// (dependency)
using JobConstructionInput =
    llvm::PointerUnion<DriverInputFile *, JobConstruction *>;

/// A list of all job construction inputs
using JobConstructionInputList = llvm::ArrayRef<JobConstructionInput>;

class JobConstruction : public CompilationEntity {
public:
  enum class JobConstructionFlags : uint8_t {
    None = 1 << 0,
    TopLevel = 1 << 1,
  };
  /// Options that control the JobConstruction
  using JobConstructionOptions = stone::OptionSet<JobConstructionFlags>;

protected:
  JobConstructionKind kind = JobConstructionKind::None;
  file::FileType fileType = file::FileType::None;
  llvm::TinyPtrVector<JobConstructionInput> inputs;

protected:
  JobConstruction(JobConstructionKind kind, JobConstructionInputList inputs,
                  file::FileType fileType)
      : kind(kind), inputs(inputs), fileType(fileType) {}

public:
  JobConstructionOptions jobConstructionOpts;

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

  // DriverInputFile* GetASDriverInputFile();
  // JobConstruction* GetASJobConstruction();

public:
  JobConstructionKind GetKind() const { return kind; }
  file::FileType GetFileType() { return fileType; }
  void AddInput(JobConstructionInput input) { inputs.push_back(input); }

public:
  bool HasTopLevel() const {
    return jobConstructionOpts.contains(JobConstructionFlags::TopLevel);
  }
  void AddTopLevel() { jobConstructionOpts |= JobConstructionFlags::TopLevel; }
  void ClearTopLevel() {}

public:
  static bool classof(const JobConstruction *construction) {
    return (construction->GetKind() >= JobConstructionKind::First &&
            construction->GetKind() <= JobConstructionKind::Last);
  }

public:
  // virtual llvm::ArrayRef<const Job*> ConstructJobs() {}

public:
  // static JobConstructionInput *CreateInput(DriverInputFile& input);
};

class IncrementatlJobConstruction : public JobConstruction {
public:
  IncrementatlJobConstruction(JobConstructionKind Kind,
                              JobConstructionInputList inputs,
                              file::FileType fileType);

public:
  static bool classof(const JobConstruction *construction) {
    return construction->GetKind() == JobConstructionKind::Compile ||
           construction->GetKind() == JobConstructionKind::MergeModule;
  }
};

class CompileJobConstruction final : public IncrementatlJobConstruction {

public:
  /// In this scenario, we are creating one compile job with all inputs to be
  /// added.
  CompileJobConstruction(file::FileType outputFileType);

public:
  /// In this scenario, one compile job for eache input.
  CompileJobConstruction(JobConstructionInput input,
                         file::FileType outputFileType);

public:
  // llvm::ArrayRef<const Job *> ConstructJob(Compilation& compilation) override
  // {}

public:
  static bool classof(const JobConstruction *construction) {
    return construction->GetKind() == JobConstructionKind::Compile;
  }

public:
  static CompileJobConstruction *Create(Driver &driver,
                                        file::FileType outputFileType);
  static CompileJobConstruction *Create(Driver &driver,
                                        JobConstructionInput input,
                                        file::FileType outputFileType);
};

class MergeModuleJobConstruction final : public IncrementatlJobConstruction {
public:
  MergeModuleJobConstruction(JobConstructionInputList inputs);

public:
  static bool classof(const JobConstruction *construction) {
    return construction->GetKind() == JobConstructionKind::MergeModule;
  }
};

class LinkJobConstruction : public JobConstruction {
  LinkMode linkMode;

public:
  LinkJobConstruction(JobConstructionKind kind, JobConstructionInputList inputs,
                      file::FileType outputFileType, LinkMode linkMode);

public:
  LinkMode GetLinkMode() const { return linkMode; }

public:
  static bool classof(const JobConstruction *construction) {
    return (construction->GetKind() == JobConstructionKind::StaticLink) ||
           (construction->GetKind() == JobConstructionKind::DynamicLink);
  }
};

class DynamicLinkJobConstruction final : public LinkJobConstruction {

  bool withLTO;

public:
  DynamicLinkJobConstruction(JobConstructionInputList inputs, LinkMode linkMode,
                             bool withLTO = false);

public:
  bool WithLTO() const { return withLTO; }

public:
  static bool classof(const JobConstruction *construction) {
    return construction->GetKind() == JobConstructionKind::DynamicLink;
  }

public:
  static DynamicLinkJobConstruction *Create(Driver &driver,
                                            JobConstructionInputList inputs,
                                            LinkMode linkMode,
                                            bool wthLTO = false);
};

class StaticLinkJobConstruction final : public LinkJobConstruction {

public:
  StaticLinkJobConstruction(JobConstructionInputList inputs, LinkMode linkMode);

public:
  static bool classof(const JobConstruction *construction) {
    return construction->GetKind() == JobConstructionKind::StaticLink;
  }

public:
  static StaticLinkJobConstruction *
  Create(Driver &driver, JobConstructionInputList inputs, LinkMode linkMode);
};

class BackendJobConstruction final : public JobConstruction {
  size_t inputIndex;

public:
  BackendJobConstruction(JobConstructionInput input,
                         file::FileType outputFileType, size_t inputIndex);

public:
  virtual size_t GetInputIndex() const override { return inputIndex; }

public:
  static bool classof(const JobConstruction *construction) {
    return construction->GetKind() == JobConstructionKind::Backend;
  }

public:
  static BackendJobConstruction *Create(Driver &driver,
                                        JobConstructionInput input,
                                        file::FileType outputFileType,
                                        size_t inputIndex);
};

class GeneratePCHJobConstruction final : public JobConstruction {
  std::string persistentPCHDir;

public:
  GeneratePCHJobConstruction(JobConstructionInput input,
                             llvm::StringRef persistentPCHDir);

public:
  bool IsPersistentPCH() const { return !persistentPCHDir.empty(); }
  StringRef GetPersistentPCHDir() const { return persistentPCHDir; }

public:
  static bool classof(const JobConstruction *construction) {
    return construction->GetKind() == JobConstructionKind::GeneratePCH;
  }

public:
  static BackendJobConstruction *Create(Driver &driver,
                                        JobConstructionInput input,
                                        llvm::StringRef persistentPCHDir);
};

} // namespace stone

#endif
