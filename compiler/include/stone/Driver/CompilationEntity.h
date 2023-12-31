#ifndef STONE_DRIVER_DRIVER_COMPILATION_ENTITY_H
#define STONE_DRIVER_DRIVER_COMPILATION_ENTITY_H

#include "stone/Driver/DriverAllocation.h"

namespace stone {

enum class CompilationEntityKind {
  Input = 0,
};

class CompilationEntity : public DriverAllocation<CompilationEntity> {
public:
  using DriverAllocation<CompilationEntity>::operator new;
  using DriverAllocation<CompilationEntity>::operator delete;
};

class CompilationEntities final {
public:
};
} // namespace stone

#endif