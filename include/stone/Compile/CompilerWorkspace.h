#ifndef STONE_COMPILE_COMPILERWORKSPACE_H
#define STONE_COMPILE_COMPILERWORKSPACE_H

#include "stone/Basic/List.h"
#include "stone/Compile/InFlightInputFile.h"

namespace stone {

class CompilerWorkspace final {
public:
  // BasicList<InFlightInputFile> inFlightInputs;

public:
  CompilerWorkspace() = default;
  CompilerWorkspace(const CompilerWorkspace &other);
  CompilerWorkspace &operator=(const CompilerWorkspace &other);

public:
  void AddInFlightInputFile(const InFlightInputFile &input);
};
} // namespace stone

#endif