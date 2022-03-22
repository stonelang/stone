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
};

class FlatCompilationModel final : public CompilationModel {};

class CPUCompilationModel final : public CompilationModel {

} class SingleCompilationModel final : public CompilationModel {
};

} // namespace stone

#endif
