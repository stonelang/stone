#ifndef STONE_COMPILE_LANINPUTSSBUILDER_H
#define STONE_COMPILE_LANINPUTSSBUILDER_H

#include "stone/Basic/Context.h"
#include "stone/Compile/LangOptions.h"

#include "llvm/ADT/SetVector.h"
#include "llvm/Option/ArgList.h"
#include <set>

namespace stone {

class DiagnosticEngine;

class LangInputsConverter {
  DiagnosticEngine &de;
  const llvm::opt::ArgList &args;

public:
  LangInputsConverter(DiagnosticEngine &de, const llvm::opt::ArgList &args)
      : de(de), args(args) {}

public:
  void Convert();
};
} // namespace stone
#endif
