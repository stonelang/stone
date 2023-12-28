#ifndef STONE_DRIVER_DRIVER_COMPILATION_H
#define STONE_DRIVER_DRIVER_COMPILATION_H

#include "stone/Driver/CompilationKind.h"
#include "stone/Driver/DriverOptions.h"

namespace stone {

class Compilation {
  CompilationKind kind;

public:
  Compilation(CompilationKind kind) : kind(kind) {}

public:
  CompilationKind GetKind() { return kind; }
};

class QuadraticCompilation final : public Compilation {

public:
  QuadraticCompilation();
};

class FlatCompilation : public Compilation {

public:
  FlatCompilation();
};

class CPUCountCompilation : public Compilation {

public:
  CPUCountCompilation();
};

class SingleCompilation : public Compilation {

public:
  SingleCompilation();
};

} // namespace stone

#endif
