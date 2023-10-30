#include "stone/CodeGen/CodeGenMachine.h"

#include "llvm/IR/Module.h"

using namespace stone;

CodeGenMachine::CodeGenMachine(CodeGenContext &cgc, syn::ASTContext &sc)
    : cgc(cgc), sc(sc) {}

CodeGenMachine::~CodeGenMachine() {}
