#include "stone/Compile/CompilerInstance.h"

using namespace stone;

/// Handles semantics
Status CompilerInstance::CompileForGenerateIR(
    std::function<Status(CodeGenContext &)> notifiy) {

  return Status();
}
Status CompilerInstance::CompileForEmitNative(CodeGenContext& codeGenContext) {

  // if (CompileForTypeCheck().IsError()) {
  //   return Status::Error();
  // }

  // // We are performing some low level code generation
  // CodeGenContext codeGenContext(
  //     GetInvocation().GetCodeGenOptions(),
  //     GetInvocation().GetModuleOptions(), GetInvocation().GetTargetOptions(),
  //     GetInvocation().GetLangContext(), GetInvocation().GetClangContext());

  // // Must generate IR
  // if (GenerateIR(*this, codeGenContext).IsError()) {
  //   return Status::Error();
  // }

  return Status();
}