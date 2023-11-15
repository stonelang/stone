#ifndef STONE_COMPILE_COMPILERTASK_H
#define STONE_COMPILE_COMPILERTASK_H

#include "stone/Basic/Color.h"
#include "stone/Basic/Status.h"
#include "stone/Compile/CompilerAllocation.h"

#include "llvm/Support/Chrono.h"

namespace stone {

class Compiler;
class CompilerQueue;
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
  ///< Check to make sure we are compiling .stone, .stonemoduleinterface files
  PreParse,
  ///< Parse only
  Parse,
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
  // </ Parse, type-check, and emit LLVM IR pre optimization
  GenIR,
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
  MergeModules,

  ///< Compiling has ended, perform final task
  Final
};
class alignas(1 << CompilerTaskAlignInBits) CompilerTask
    : public CompilerAllocation<std::aligned_storage<8, 8>::type> {

  friend CompilerQueue;

  CompilerTaskKind kind;

protected:
  bool hasDependency = false;
  bool isCompleted = false;
  bool skippingTask = false;

  llvm::sys::TimePoint<> startTime;
  llvm::sys::TimePoint<> endTime = llvm::sys::TimePoint<>::min();

public:
  CompilerTask(CompilerTaskKind kind) : kind(kind) {}

public:
  bool HasDependency() { return hasDependency; }
  bool IsCompleted() { return isCompleted; }
  bool SkippingTask() { return skippingTask; }
  CompilerTaskKind GetKind() { return kind; }
  bool IsCompilable() const;

public:
  virtual Status Execute(Compiler &compiler, CompilerTask *dep = nullptr) = 0;
  virtual void Print(ColorStream &stream) = 0;
};

class PrintHelpTask final : public CompilerTask {

public:
  PrintHelpTask() : CompilerTask(CompilerTaskKind::PrintHelp) {}

public:
  virtual Status Execute(Compiler &compiler,
                         CompilerTask *dep = nullptr) override;

  virtual void Print(ColorStream &stream) override {}

public:
  static PrintHelpTask *Create(const Compiler &compiler);
};

class PrintVersionTask final : public CompilerTask {

public:
  PrintVersionTask() : CompilerTask(CompilerTaskKind::PrintVersion) {}

public:
  virtual Status Execute(Compiler &compiler, CompilerTask *dep) override;
  void Print(ColorStream &stream) override {}

public:
  static PrintVersionTask *Create(const Compiler &compiler);
};

class PreParseTask : public CompilerTask {

public:
  PreParseTask() : CompilerTask(CompilerTaskKind::PreParse) {}

public:
  virtual Status Execute(Compiler &compiler, CompilerTask *dep) override;
  virtual void Print(ColorStream &stream) override;

public:
  static PreParseTask *Create(const Compiler &compiler);
};

class ParseTask : public CompilerTask {

public:
  ParseTask() : CompilerTask(CompilerTaskKind::Parse) { hasDependency = true; }

public:
  virtual Status Execute(Compiler &compiler, CompilerTask *dep) override;
  virtual void Print(ColorStream &stream) override;

public:
  static ParseTask *Create(const Compiler &compiler);
};

class ResolveImportsTask : public CompilerTask {

public:
  ResolveImportsTask() : CompilerTask(CompilerTaskKind::ResolveImports) {
    hasDependency = true;
  }

public:
  virtual Status Execute(Compiler &compiler, CompilerTask *dep) override;
  virtual void Print(ColorStream &stream) override;

public:
  static ResolveImportsTask *Create(const Compiler &compiler);
};

class TypeCheckTask : public CompilerTask {

public:
  TypeCheckTask() : CompilerTask(CompilerTaskKind::TypeCheck) {
    hasDependency = true;
  }

public:
  virtual Status Execute(Compiler &compiler, CompilerTask *dep) override;
  virtual void Print(ColorStream &stream) override;

public:
  static TypeCheckTask *Create(const Compiler &compiler);
};

class GenIRTask : public CompilerTask {

public:
  GenIRTask() : CompilerTask(CompilerTaskKind::GenIR) { hasDependency = true; }

public:
  virtual Status Execute(Compiler &compiler, CompilerTask *dep) override;
  virtual void Print(ColorStream &stream) override;

public:
  static PreEmitTask *Create(const Compiler &compiler);
};

class EmitIRBeforeTask : public CompilerTask {

public:
  EmitIRBeforeTask() : CompilerTask(CompilerTaskKind::EmitIRBefore) {
    hasDependency = true;
  }

public:
  virtual Status Execute(Compiler &compiler, CompilerTask *dep) override;
  virtual void Print(ColorStream &stream) override;

public:
  static EmitIRBeforeTask *Create(const Compiler &compiler);
};

class EmitIRAfterTask : public CompilerTask {

public:
  EmitIRAfterTask() : CompilerTask(CompilerTaskKind::EmitIRAfter) {
    hasDependency = true;
  }

public:
  virtual Status Execute(Compiler &compiler, CompilerTask *dep) override;
  virtual void Print(ColorStream &stream) override;

public:
  static EmitIRAfterTask *Create(const Compiler &compiler);
};

class EmitObjectTask : public CompilerTask {

public:
  EmitObjectTask() : CompilerTask(CompilerTaskKind::EmitObject) {
    hasDependency = true;
  }

public:
  virtual Status Execute(Compiler &compiler, CompilerTask *dep) override;
  virtual void Print(ColorStream &stream) override;

public:
  static EmitObjectTask *Create(const Compiler &compiler);
};

class FinalTask final : public CompilerTask {

public:
  FinalTask() : CompilerTask(CompilerTaskKind::Final) {}

public:
  virtual Status Execute(Compiler &compiler, CompilerTask *dep) override;
  virtual void Print(ColorStream &stream) override;

public:
  static FinalTask *Create(const Compiler &compiler);
};

} // namespace stone
#endif
