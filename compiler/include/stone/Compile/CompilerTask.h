#ifndef STONE_COMPILE_COMPILERTASK_H
#define STONE_COMPILE_COMPILERTASK_H

#include "stone/Basic/Status.h"
#include "stone/Compile/CompilerAllocation.h"

namespace stone {

class Compiler;
constexpr size_t CompilerTaskAlignInBits = 3;

enum class CompilerTaskKind {
  ///< Print help
  PrintHelp = 0,
  ///< Print hidden help
  PrintHelpHidden,
  //< Print language version
  PrintVersion,
  //< Print the timers
  PrintTimers,
  // < Print the statistics
  PrintStatistics,
  ///< Parse only
  ParseOnly,
  ///< Parse and resolve use(s) only
  ResolveImports,
  ///< Parse and dump syntax tree
  DumpSyntax,
  ///< Parse and type-check only
  TypeCheck,
  ///< TODO
  DumpTypeInfo,
  ///< Parse, type-check, and  pretty print syntax tree
  PrintSyntax,
  //</ Parse, type-check, and pretty print llvm-ir
  PrintIR,
  // </ Complete any work before emitting.
  BeforeEmit,
  //</ Parse, type-check, and emit LLVM IR pre optimization
  EmitIRBefore,
  //</ Parse, type-check, and emit LLVM IR post optimization
  EmitIRAfter,
  ///< Parse, type-check, and emit a library.
  ///< Default => platform specific. But, with -static => 'any.a'
  EmitLibrary,
  ///< Create a module fule
  InitModule,
  //< Parse, type-check, and emit a module. Ex: 'any.stonemod'
  EmitModule,
  //< Parse, type-check, and emit LLVM BC
  EmitBC,
  //< Parse, type-check, and emit assembly
  EmitAssembly,
  ///< Parse, type-check, and emit native object code
  EmitObject,
  ///< Merge all modules
  MergeModules
};
class alignas(1 << CompilerTaskAlignInBits) CompilerTask
    : public CompilerAllocation<std::aligned_storage<8, 8>::type> {
public:
  // TODO: used these in the future -- this allows you to add more tasks.
  enum ID : unsigned {
    ///< Print help
    PrintHelp = 0,
    ///< Print hidden help
    PrintHelpHidden,
    //< Print language version
    PrintVersion,
    //< Print the timers
    PrintTimers,
    // < Print the statistics
    PrintStatistics,
    ///< Parse only
    ParseOnly,
    ///< Parse and resolve use(s) only
    ResolveImports,
    ///< Parse and dump syntax tree
    DumpSyntax,
    ///< Parse and type-check only
    TypeCheck,
    ///< TODO
    DumpTypeInfo,
    ///< Parse, type-check, and  pretty print syntax tree
    PrintSyntax,
    //</ Parse, type-check, and pretty print llvm-ir
    PrintIR,
    // </ Complete any work before emitting.
    BeforeEmit,
    //</ Parse, type-check, and emit LLVM IR pre optimization
    EmitIRBefore,
    //</ Parse, type-check, and emit LLVM IR post optimization
    EmitIRAfter,
    ///< Parse, type-check, and emit a library.
    ///< Default => platform specific. But, with -static => 'any.a'
    EmitLibrary,
    ///< Create a module fule
    InitModule,
    //< Parse, type-check, and emit a module. Ex: 'any.stonemod'
    EmitModule,
    //< Parse, type-check, and emit LLVM BC
    EmitBC,
    //< Parse, type-check, and emit assembly
    EmitAssembly,
    ///< Parse, type-check, and emit native object code
    EmitObject,
    ///< Merge all modules
    MergeModules
  };
  using CompilerTaskDescription = CompilerTask::ID;
  CompilerTaskDescription description;

public:
  CompilerTask(CompilerTaskDescription description)
      : description(description) {}

public:
  virtual Status Execute(Compiler &compiler) {}
};

class PrintHelpTask final : public CompilerTask {

public:
  PrintHelpTask() : CompilerTask(CompilerTaskDescription::PrintHelp) {}

public:
  virtual Status Execute(Compiler &compiler) override;

public:
  static PrintHelpTask *Create(const Compiler &compiler);
};

class PrintVersionTask final : public CompilerTask {

public:
  PrintVersionTask() : CompilerTask(CompilerTaskDescription::PrintVersion) {}

public:
  virtual Status Execute(Compiler &compiler) override;

public:
  static PrintVersionTask *Create(const Compiler &compiler);
};

class VerifyInputFileTypesTask : public CompilerTask {
public:
  virtual Status Execute(Compiler &compiler) override;

public:
  static VerifyInputFileTypesTask *Create(const Compiler &compiler);
};

class SyntaxTask : public CompilerTask {

public:
  virtual Status Execute(Compiler &compiler) override;
};

class ParseTask : public SyntaxTask {
public:
  virtual Status Execute(Compiler &compiler) override;

public:
  static ParseTask *Create();
};

class ResolveImportsTask : public SyntaxTask {
public:
  virtual Status Execute(Compiler &compiler) override;

public:
  static ResolveImportsTask *Create();
};

class TypeCheckTask : public SyntaxTask {
public:
  virtual Status Execute(Compiler &compiler);

public:
  static TypeCheckTask *Create();
};

// class EmittingTask : public SyntaxTask {
//   virtual Status Execute(Compiler &compiler)
// };
// class CodeGenTask : public SyntaxTask {
//   virtual Status Execute(Compiler &compiler)
// };

// class IRCodeGenTask : public CodeGenTask {
//   virtual Status Execute(Compiler &compiler)
// };
// class MachineCodeGenTask : public CodeGenTask {
//   virtual Status Execute(Compiler &compiler)
// };

} // namespace stone
#endif
