#ifndef STONE_COMPILE_COMPILERINPUTARGLIST_H
#define STONE_COMPILE_COMPILERINPUTARGLIST_H

#include "llvm/ADT/ArrayRef.h"

namespace stone {

class CompilerInputArgList final {
  llvm::opt::InputArgList &args;

public:
  CompilerInputArgList(llvm::opt::InputArgList args) : args(args) {}

public:
  const llvm::opt::Arg *GetActionArg();
  const unsigned GetOptionID(llvm::opt::Arg *arg);
  llvm::StringRef GetArgName(llvm::opt::Arg *arg);
};

#endif