#ifndef STONE_DRIVER_DRIVER_COMPILATION_ENTITY_H
#define STONE_DRIVER_DRIVER_COMPILATION_ENTITY_H

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

};


} // namespace stone

#endif