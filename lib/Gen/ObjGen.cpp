#include "stone/Gen/ObjGen.h"
#include "llvm/IR/Module.h"

using namespace stone;

ObjGen::ObjGen(CodeGenContext &codeGenContext, llvm::Module *llvmMod,
               syn::TreeContext &tc)
    : codeGenContext(codeGenContext), llvmMod(llvmMod), tc(tc) {}

ObjGen::~ObjGen() {}

// TODO: You should think about using Clang for this
void ObjGen::Emit() {}
