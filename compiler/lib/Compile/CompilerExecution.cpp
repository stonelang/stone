#include "stone/Compile/CompilerExecution.h"

using namespace stone;

CompilerExecution::CompilerExecution(Compiler &compiler) : compiler(compiler) {}

Status CompilerExecution::Setup() { return Status(); }