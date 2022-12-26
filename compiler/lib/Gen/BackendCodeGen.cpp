#include "stone/Gen/BackendCodeGen.h"

#include "llvm/IR/Module.h"

using namespace stone;

BackendCodeGen::BackendCodeGen(CodeGenContext &cgc, syn::SyntaxContext &sc)
    : cgc(cgc), sc(sc) {}

BackendCodeGen::~BackendCodeGen() {}
