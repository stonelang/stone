#ifndef STONE_COMPILE_COMPILE_H
#define STONE_COMPILE_COMPILE_H

namespace stone {

class Compiler;
class CompilerTask;
class CompilerInvocation;

class CompilerListener {
public:
  CompilerListener() = default;
  virtual ~CompilerListener() = default;

public:
  virtual void CompletedCommandLineParsing(CompilerInvocation &invocation) {}
  virtual void CompletedInitialization(Compiler &compiler) {}
  virtual void CompletedBuildingTasks(Compiler &compiler) {}

  virtual void ExecutingCompilerTasks(Compiler &compiler, CompilerTask *task) {}
  virtual void FinishedCompilerTasks(Compiler &compiler, CompilerTask *task) {}

  virtual void CompletedSyntaxAnalysis(Compiler &compiler) {}
  virtual void CompletedSemanticAnalysis(Compiler &compiler) {}
  virtual void CompletedCodeGeneration(Compiler &compiler) {}
  virtual void CompletedRunningTasks(Compiler &compiler) {}
};

int Compile(llvm::ArrayRef<const char *> args, const char *arg0, void *mainAddr,
            CompilerListener *listener = nullptr);

} // namespace stone

#endif
