#include "stone/Gen/NativeCodeGen.h"
#include "llvm/IR/Module.h"

using namespace stone;

NativeCodeGen::NativeCodeGen(CodeGenContext &cgc, llvm::Module *llvmMod,
                             syn::SyntaxContext &tc)
    : cgc(cgc), llvmMod(llvmMod), tc(tc) {}

NativeCodeGen::~NativeCodeGen() {}

// TODO: You should think about using Clang for this
void NativeCodeGen::Emit() {}
