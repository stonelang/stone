#ifndef STONE_COMPILE_LANGOPTIONSBUILDER_H
#define STONE_COMPILE_LANGOPTIONSBUILDER_H

#include "stone/Compile/LangOptions.h"

namespace stone {

class LangOptionsConverter {

  DiagnosticEngine &de;
  const llvm::opt::ArgList;
  &args LangOptions &langOpts;

public:
  LangOptionsConverter(DiagnosticEngine &de, const llvm::opt::ArgList &args,
                       Mode &mode, LangOptions &langOpts)
      : de(de), args(args), langOpts(langOpts) {}

public:
  void Convert();
};
} // namespace stone
#endif
