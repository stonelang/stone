#include "stone/Gen/IRCodeGen.h"
#include "stone/Basic/CodeGenOptions.h"
#include "stone/Syntax/Module.h"

#include "llvm/IR/Module.h"

using namespace stone;

IRCodeGen::IRCodeGen(CodeGenContext &cgc, CodeGenListener *listener)
    : cgc(cgc), listener(listener), cgb(cgc, typeCache) {}

IRCodeGen::~IRCodeGen() {}

Safe<llvm::TargetMachine> IRCodeGen::CreateTargetMachine() {}