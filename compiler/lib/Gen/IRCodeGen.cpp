#include "stone/Basic/CodeGenOptions.h"
#include "stone/Gen/IRCodeGen.h"
#include "stone/Gen/IRCodeGenResult.h"
#include "stone/Syntax/Module.h"

#include "llvm/IR/Module.h"

using namespace stone;

IRCodeGen::IRCodeGen(CodeGenContext &cgc) : cgc(cgc), irCodeGenBuilder(cgc) {}

IRCodeGen::~IRCodeGen() {}

// IRCodeModule &IRCode::GetIRCodeModule() { return *cm.get(); }
// IRCodeFunction &IRCode::GetIRCodeFunction() { return *cf.get(); }


