#include "stone/Gen/NativeCodeGen.h"

#include "llvm/IR/Module.h"

using namespace stone;

NativeCodeGen::NativeCodeGen(CodeGenContext &cgc, IRCodeGenResult &result,
                             syn::SyntaxContext &sc)
    : cgc(cgc), result(result), sc(sc) {}

NativeCodeGen::~NativeCodeGen() {}

// TODO: You should think about using Clang for this
void NativeCodeGen::Emit() {}
