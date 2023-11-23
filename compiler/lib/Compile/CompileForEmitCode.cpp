#include "stone/Compile/CompilerInstance.h"

using namespace stone;

Status CompilerInstance::CompileForEmitCode() {

  if (CompileForTypeCheck().IsError()) {
    return Status::Error();
  }

  // We are performing some low level code generation
  CodeGenContext cgc(
      GetInvocation().GetCodeGenOptions(), GetInvocation().GetModuleOptions(),
      GetInvocation().GetTargetOptions(), GetInvocation().GetLangContext(),
      GetInvocation().GetClangContext());

  return Status();
}