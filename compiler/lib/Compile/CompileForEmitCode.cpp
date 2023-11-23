#include "stone/Compile/CompilerInstance.h"

using namespace stone;


static Status GenerateIR(CompilerInstance& compiler, CodeGenContext& codeGenContext){
	return Status();
}
Status CompilerInstance::CompileForEmitCode() {

  if (CompileForTypeCheck().IsError()) {
    return Status::Error();
  }

  // We are performing some low level code generation
  CodeGenContext codeGenContext(
      GetInvocation().GetCodeGenOptions(), GetInvocation().GetModuleOptions(),
      GetInvocation().GetTargetOptions(), GetInvocation().GetLangContext(),
      GetInvocation().GetClangContext());

  // Must generate IR
  if(GenerateIR(*this, codeGenContext).IsError()){
  	return Status::Error();
  }

  return Status();
}