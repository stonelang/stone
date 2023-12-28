#ifndef STONE_DRIVER_DRIVER_JOB_CONSTRUCTION_H
#define STONE_DRIVER_DRIVER_JOB_CONSTRUCTION_H

#include "stone/Basic/File.h"
#include "stone/Basic/Mem.h"
#include "stone/Basic/STDAlias.h"

#include "llvm/ADT/PointerUnion.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/TinyPtrVector.h"
#include "llvm/Option/Arg.h"
#include "llvm/Option/ArgList.h"

namespace stone {
class JobConstruction;

enum class JobConstructionKind : UInt8 {
  Compile = 0,
  Backend,
  MergeModule,
  ModuleWrap,
  DynamicLink,
  StaticLink,
  Assemble,
  Executable,
  First = Compile,
  Last = Assemble
};

/// A job construction input can be a file or another job construction
/// (dependency)
using JobConstructionInput =
    llvm::PointerUnion<stone::file::File *, JobConstruction *>;

/// A list of all job construction inputs
using JobConstructionInputList = llvm::ArrayRef<JobConstructionInput>;

class JobConstruction : public MemoryAllocation<JobConstruction> {
protected:
  JobConstructionKind kind;
  file::Type outputFileType = file::Type::None;
  // llvm::TinyPtrVector<JobConstructionInput> inputs;

public:
  JobConstruction(JobConstructionKind kind) : kind(kind) {}

public:
  JobConstructionKind GetKind() { return kind; }

public:
};

class CompileJobConstruction final : public JobConstruction {
public:
  CompileJobConstruction() : JobConstruction(JobConstructionKind::Compile) {}
};

class DynamicLinkJobConstruction final : public JobConstruction {
public:
  DynamicLinkJobConstruction()
      : JobConstruction(JobConstructionKind::DynamicLink) {}
};

class StaticLinkJobConstruction final : public JobConstruction {
public:
  StaticLinkJobConstruction()
      : JobConstruction(JobConstructionKind::StaticLink) {}
};

class BackendJobConstruction final : public JobConstruction {
public:
  BackendJobConstruction() : JobConstruction(JobConstructionKind::Backend) {}
};

class ExecutableJobConstruction final : public JobConstruction {
public:
  ExecutableJobConstruction()
      : JobConstruction(JobConstructionKind::Executable) {}
};

} // namespace stone

#endif
