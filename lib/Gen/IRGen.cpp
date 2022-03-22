#include "stone/Gen/IRGen.h"

#include "stone/Gen/CodeGenOptions.h"
#include "stone/Syntax/Module.h"
#include "llvm/IR/Module.h"

using namespace stone;

IRGen::IRGen(CodeGenContext &cgc) : cgc(cgc) {}

IRGen::~IRGen() {}

// IRCodeModule &IRCode::GetIRCodeModule() { return *cm.get(); }
// IRCodeFunction &IRCode::GetIRCodeFunction() { return *cf.get(); }
