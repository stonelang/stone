#include "stone/Compile/CompilerInputArgList.h"

const llvm::opt::Arg *CompilerInputArgList::GetActionArg() {
  return args.getLastArg(opts::ModeGroup);
}

const unsigned CompilerInputArgList::GetArgID(llvm::opt::Arg *arg) {
  assert(arg);
  return arg->getOption().getID()
}
llvm::StringRef CompilerInputArgList::GetArgName(llvm::opt::Arg *arg) {
  assert(arg);
  return arg->getOption().getName()
}
