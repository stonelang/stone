#include "stone/Compile/Compiler.h"
#include "stone/Compile/CompilerTask.h"
#include "stone/Compile/CompilerAllocation.h"

using namespace stone;

CompilerQueue::CompilerQueue(Compiler &compiler) : compiler(compiler) {}

void CompilerQueue::AddTask(CompilerTask* task) {
	runQueue.push(task);
}

Status PrintHelpTask::Execute(Compiler &compiler) {
	return Status();
}

PrintHelpTask* PrintHelpTask::Create(const Compiler& compiler){
	return new (compiler) PrintHelpTask();
}