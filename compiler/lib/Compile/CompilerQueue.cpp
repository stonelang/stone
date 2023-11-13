#include "stone/Compile/Compiler.h"
#include "stone/Compile/CompilerAllocation.h"
#include "stone/Compile/CompilerTask.h"

using namespace stone;

CompilerQueue::CompilerQueue(Compiler &compiler) : compiler(compiler) {}

void CompilerQueue::AddTask(CompilerTask *task) { runQueue.push_back(task); }

PrintHelpTask *PrintHelpTask::Create(const Compiler &compiler) {
  return new (compiler) PrintHelpTask();
}

void CompilerQueue::RunTasks() {
	for(auto task : runQueue){
		task->Execute(GetCompiler());
	}
}
