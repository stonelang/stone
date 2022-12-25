#include "stone/Gen/IRCodeGenModule.h"
#include "stone/Gen/IRCodeGen.h"
#include "stone/Syntax/Decl.h"

using namespace stone;
using namespace stone::syn;

IRCodeGenModule::IRCodeGenModule(IRCodeGen &irCodeGen, syn::SyntaxFile *sf,
                                 llvm::StringRef outputFileName)
    : irCodeGen(irCodeGen), sf(sf), outputFileName(outputFileName) {}

IRCodeGenModule::~IRCodeGenModule() {}

void IRCodeGenModule::EmitSyntaxFile(const syn::SyntaxFile &sf) {
  // Walk through the syntax file and call emit
  // Emit types and other global decls.
  for (auto d : sf.Decls) {
    EmitDecl(d);
  }
}
