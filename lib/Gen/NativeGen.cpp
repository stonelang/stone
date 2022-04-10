#include "stone/Gen/NativeGen.h"
#include "llvm/IR/Module.h"

using namespace stone;

NativeGen::NativeGen(CodeGenContext &cgc, llvm::Module *llvmMod,
                     syn::SyntaxContext &tc)
    : cgc(cgc), llvmMod(llvmMod), tc(tc) {}

NativeGen::~NativeGen() {}

// TODO: You should think about using Clang for this
void NativeGen::Emit() {}
