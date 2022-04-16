#ifndef STONE_COMPILE_LANGOPTIONSBUILDER_H
#define STONE_COMPILE_LANGOPTIONSBUILDER_H

#include "stone/Compile/LangOptions.h"

namespace stone {

class LangOptionsConverter {

  DiagnosticEngine &de;
  const llvm::opt::ArgList &args;
  const Mode &mode;
  LangOptions &langOpts;

public:
  LangOptionsConverter(DiagnosticEngine &de, const llvm::opt::ArgList &args,
                       const Mode &mode, LangOptions &langOpts)
      : de(de), args(args), mode(mode), langOpts(langOpts) {}

public:
  void Convert();
};
} // namespace stone
#endif
