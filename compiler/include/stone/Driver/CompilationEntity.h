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
  Last = StaticLinkJobConstruction
};

/// A list of all job construction inputs
using CompilationEntityList = llvm::ArrayRef<const CompilationEntity *>;



} // namespace stone

#endif