#ifndef STONE_COMPILE_LANGOPTIONSBUILDER_H
#define STONE_COMPILE_LANGOPTIONSBUILDER_H

#include "stone/Basic/Context.h"
#include "stone/Basic/DiagnosticEngine.h"
#include "stone/Basic/DiagnosticListener.h"
#include "stone/Compile/LangOptions.h"
#include "stone/Session/Options.h"
#include "llvm/Option/ArgList.h"

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
  stone::Error
  Convert(llvm::SmallVectorImpl<std::unique_ptr<llvm::MemoryBuffer>> *buffers);
};
} // namespace stone
#endif
