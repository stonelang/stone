#include "stone/Basic/Status.h"
#include "stone/Compile/Compiler.h"
#include "stone/Compile/CompilerExecution.h"

using namespace stone;

SupportExecution::SupportExecution(Compiler &compiler)
    : CompilerExecution(compiler) {}

Status SupportExecution::Setup() {}

Status SupportExecution::Execute() {}

// Status SupportExecution::ExecutePrintHelp() { return Status(); }
// Status SupportExecution::ExecutePrintVersion() { return Status(); }
// Status SupportExecution::ExecutePrintFeature() { return Status(); }
