#ifndef STONE_COMPILE_COMPILING_H
#define STONE_COMPILE_COMPILING_H

namespace stone {
class Compiler;
namespace syn {
class SyntaxFile;
}

class CompilerQueue final {};

class CompilerTask : public CompilerAllocation {
public:
  virtual Status Execute(Compiler &compiler);
  virtual void Report();
};

void PrintHelp(ColorStream &out, const llvm::opt::OptTable &opts);
void PrintVersion();
void PrintTimers();
void PrintDiagnostics();
void PrintStatistics();

class CompilerStatsTask : public CompilerTask {};

class VerifyInputFileTypesTask : public CompilerTask {};

class SyntaxTask : public CompilerTask {
  virtual Status Execute(Compiler &compiler)
};

class ParseTask : public SyntaxTask {
  virtual Status Execute(Compiler &compiler)
};

class ParseAndImportResolutionTask : public SyntaxTask {
  virtual Status Execute(Compiler &compiler)

      public : static ParseAndImportResolutionTask *Create();
};

class TypeCheckTask : public SyntaxTask {
  virtual Status Execute(Compiler &compiler)

      public : static ParseAndImportResolutionTask *Create();
};

// class EmitIRTask : public SyntaxTask {
//   virtual Status Execute(Compiler &compiler)
// };

class CodeGenTask : public SyntaxTask {
  virtual Status Execute(Compiler &compiler)
};

class IRCodeGenTask : public CodeGenTask {
  virtual Status Execute(Compiler &compiler)
};
class MachineCodeGenTask : public CodeGenTask {
  virtual Status Execute(Compiler &compiler)
};

compiler.AddTask(ParseTask::Create(compiler));
compiler.AddTask(ParseAndImportResolutionTask::Create(compiler));

compiler.AddTask(TypeCheckTask());
compiler.AddTask(EmitIRTask());
compiler.AddTask(EmitObjectTask());

compiler.GetQueue().Run();

namespace compiling {

Status Compile(Compiler &compiler);

// Syntax parsing
void VerifyCompilerInputFileTypes(Compiler &compiler);
Status Parse(Compiler &compiler);
Status ParseAndImportResolution(Compiler &compiler);
Status DumpSyntax(Compiler &compiler, syn::SyntaxFile &syntaxFile);

// Type checking
Status TypeCheck(Compiler &compiler);

using TypeCheckingCompletedCallback = llvm::function_ref<Status(Compiler &)>;

Status WithTypeChecking(Compiler &Instance,
                        TypeCheckingCompletedCallback notify,
                        bool typeCheckDespiteErrors = false);

Status FinishTypeCheck(Compiler &compiler);

// Replace with void
Status PrintSyntax(Compiler &compiler);
Status CompileAfterTypeChecking(Compiler &compiler);

/// Code generation
Status GenCode(Compiler &compiler, CodeGenContext &codeGenContext);
Status GenIR(Compiler &compiler, CodeGenContext &codeGenContext);
void DumpIR(Compiler &compiler, CodeGenContext &codeGenContext);
void PrintIR(Compiler &compiler, CodeGenContext &codeGenContext);

} // namespace compiling

class CompilerRequest {};

class SyntaxParingCompilerRequest() {}
class TypeCheckingCompilerRequest {};

} // namespace stone
#endif
