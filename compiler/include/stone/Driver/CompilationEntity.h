#ifndef STONE_DRIVER_DRIVER_COMPILATION_ENTITY_H
#define STONE_DRIVER_DRIVER_COMPILATION_ENTITY_H

#include "stone/Driver/DriverAllocation.h"

namespace stone {

class Driver;
class Compilation;

enum class CompilationEntityKind {
  Input = 0,
};

class CompilationEntity : public DriverAllocation<CompilationEntity> {
public:
  using DriverAllocation<CompilationEntity>::operator new;
  using DriverAllocation<CompilationEntity>::operator delete;

public:
};

class CompilationEntities final {
  friend Driver;
  friend Compilation;

public:
};
} // namespace stone

#endif