#include "stone/Driver/CompilationJob.h"

using namespace stone;

void CompileJob::Run() { cmd::ExecuteAsync(*ToCommand()); }

const Command *CompileJob::ToCommand() const {
  // TODO: Build out command

  return llvm::dyn_cast<Command>(this);
}

void DynamicLinkJob::Run() { cmd::ExecuteAsync(*ToCommand()); }

const Command *DynamicLinkJob::ToCommand() const {
  // TODO: Build out command

  return llvm::dyn_cast<Command>(this);
}
