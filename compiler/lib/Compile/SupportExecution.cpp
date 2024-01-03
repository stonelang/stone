#include "stone/Compile/CompilerExecution.h"

using namespace stone;

PrintHelpExecution::PrintHelpExecution(Compiler &compiler)
    : CompilerExecution(compiler) {}

Status PrintHelpExecution::ExecuteAction() { return Status(); }

PrintHelpHiddenExecution::PrintHelpHiddenExecution(Compiler &compiler)
    : CompilerExecution(compiler) {}

Status PrintHelpHiddenExecution::ExecuteAction() { return Status(); }

PrintVersionExecution::PrintVersionExecution(Compiler &compiler)
    : CompilerExecution(compiler) {}

Status PrintVersionExecution::ExecuteAction() { return Status(); }

PrintFeatureExecution::PrintFeatureExecution(Compiler &compiler)
    : CompilerExecution(compiler) {}

Status PrintFeatureExecution::ExecuteAction() { return Status(); }
