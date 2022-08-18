#include "stone/Gen/IRCodeGenModule.h"
#include "stone/Gen/IRCodeGen.h"
#include "stone/Syntax/Decl.h"

using namespace stone;
using namespace stone::syn;

// llvm::Module *IRModule::GetLLVMModule() { return llvmModule.get(); }
// llvm::Module *IRModule::ReleaseLLVMModule() { return llvmModule.release(); }

IRCodeGenModule::IRCodeGenModule(IRCodeGen &irCodeGen) : irCodeGen(irCodeGen) {}

void IRCodeGenModule::EmitSyntaxFile(const syn::SyntaxFile &sf) {
  // Walk through the syntax file and call emit
  // Emit types and other global decls.
  for (auto &topLevelDecl : *sf.allDecls) {
    EmitTopLevelDecl(topLevelDecl);
  }
}
