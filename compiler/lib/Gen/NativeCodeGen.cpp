#include "stone/Gen/NativeCodeGen.h"

#include "llvm/IR/Module.h"

using namespace stone;

NativeCodeGen::NativeCodeGen(CodeGenContext &cgc, syn::ASTContext &sc)
    : cgc(cgc), sc(sc) {}

NativeCodeGen::~NativeCodeGen() {}
