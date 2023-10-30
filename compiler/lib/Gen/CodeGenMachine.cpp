#include "stone/Gen/CodeGenMachine.h"

#include "llvm/IR/Module.h"

using namespace stone;

CodeGenMachine::CodeGenMachine(CodeGenContext &cgc, syn::SyntaxContext &sc)
    : cgc(cgc), sc(sc) {}

CodeGenMachine::~CodeGenMachine() {}
