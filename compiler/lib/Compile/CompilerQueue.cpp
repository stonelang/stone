#include "stone/Compile/Compiler.h"
#include "stone/Compile/CompilerAllocation.h"
#include "stone/Compile/CompilerTask.h"

using namespace stone;

CompilerQueue::CompilerQueue(Compiler &compiler) : compiler(compiler) {}

void CompilerQueue::AddTask(CompilerTask *task) { runQueue.push_back(task); }

void CompilerQueue::RunTasks() {
  ColorStream stream;
  CompilerTask *depTask = nullptr;
  for (auto curTask : runQueue) {
    curTask->Setup();
    curTask->Execute(GetCompiler(), depTask);
    curTask->Print(stream);
    depTask = curTask;
  }
}
