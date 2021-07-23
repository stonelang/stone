#include "stone/Compile/Compiler.h"
#include "stone/Compile/CompilableItem.h"

using namespace stone;

bool CompilableItem::CanOutput() { return compiler.GetMode().CanOutput(); }

void CompilableItem::CreateOutputFile() {}

void CompilableItem::PurgeOutputFile() {}