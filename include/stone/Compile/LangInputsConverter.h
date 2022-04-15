#ifndef STONE_COMPILE_LANINPUTSSBUILDER_H
#define STONE_COMPILE_LANINPUTSSBUILDER_H

namespace stone {

class LangInputsConverter {
  DiagnosticEngine &de;
  const llvm::opt::ArgList;

public:
  LangOptionsConverter(DiagnosticEngine &de, const llvm::opt::ArgList &args)
      : de(de), args(args) {}

public:
  void Convert();
};
} // namespace stone
#endif
