#include "stone/Compile/CompilerInstance.h"

using namespace stone;

Status CompilerInstance::CompileForEmitCode() {

  if (Compile(ModeKind::TypeCheck).IsError()) {
    return Status::Error();
  }

  return Status();
}