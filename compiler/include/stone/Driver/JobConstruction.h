#ifndef STONE_DRIVER_DRIVER_JOBCONSTRUCTION_H
#define STONE_DRIVER_DRIVER_JOBCONSTRUCTION_H

#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/TinyPtrVector.h"
#include "llvm/Option/Arg.h"
#include "llvm/Option/ArgList.h"

namespace stone {

enum class JobConstructionKind : uint8_t {
  Compile = 0,
  Backend,
  MergeModule,
  ModuleWrap,
  DynamicLink,
  StaticLink,
  Assemble,
  First = Compile,
  Last = Assemble
};

/// A job construction input can be a file or another job construction
/// (dependency)
using JobConstructionInput =
    llvm::PointerUnion<stone::file::File *, JobConstruction *>;

/// A list of all job construction inputs
using JobConstructionInputList = llvm::ArrayRef<JobConstructionInput>;

class JobConstruction {

protected:
  JobConstruction kind;
  file::Type outputFileType = file::Type::None;
  llvm::TinyPtrVector<JobConstructionInput> inputs;

public:
};

class CompileJobConstruction final : public JobConstruction {
public:
};

class DynamicLinkJobConstruction final : public JobConstruction {
public:
};

class StaticLinkJobConstruction final : public JobConstruction {
public:
};

class BackendJobConstruction final : public JobConstruction {
public:
};

} // namespace stone

#endif
