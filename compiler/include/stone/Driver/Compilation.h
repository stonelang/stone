#ifndef STONE_DRIVER_DRIVER_COMPILATION_H
#define STONE_DRIVER_DRIVER_COMPILATION_H

#include "stone/Driver/Driver.h"

namespace stone {

class Compilation {
  CompilationKind kind;
  Driver &driver;

public:
  Compilation(CompilationKind kind, Driver &driver);
  virtual ~Compilation();

public:
  CompilationKind GetKind() { return kind; }

public:
  virtual Status Execute() = 0;
};

class QuadraticCompilation final : public Compilation {

public:
  QuadraticCompilation(Driver &driver);

public:
  Status Execute() override;
};

class FlatCompilation : public Compilation {

public:
  FlatCompilation(Driver &driver);

public:
  Status Execute() override;
};

class CPUCountCompilation : public Compilation {

public:
  CPUCountCompilation(Driver &driver);

public:
  Status Execute() override;
};

class SingleCompilation : public Compilation {

public:
  SingleCompilation(Driver &driver);

public:
  Status Execute() override;
};

} // namespace stone

#endif
