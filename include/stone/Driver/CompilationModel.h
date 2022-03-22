#ifndef STONE_DRIVER_COMPILATIONMODEL_H
#define STONE_DRIVER_COMPILATIONMODEl_H

#include "stone/Core/LLVM.h"
#include "stone/Core/List.h"
#include "stone/Driver/OutputOptions.h"

namespace stone {

class CompilationModel {
  CompilationMode mode;

public:
  CompilationModel(CompilationMode mode) : mode(mode) {}
};
class QuadraticCompilationModel final : public CompilationModel {
public:
  QuadraticCompilationModel() : CompilationModel(CompilationMode::Quadratic) {}
};

class FlatCompilationModel final : public CompilationModel {
public:
  FlatCompilationModel() : CompilationModel(CompilationMode::Flat) {}
};

class CPUCompilationModel final : public CompilationModel {

public:
  CPUCompilationModel() : CompilationModel(CompilationMode::CPU) {}
};

class SingleCompilationModel final : public CompilationModel {
public:
  SingleCompilationModel() : CompilationModel(CompilationMode::Single) {}
};

} // namespace stone

#endif
