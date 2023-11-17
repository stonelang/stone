#ifndef STONE_COMPILE_COMPILERTASK_H
#define STONE_COMPILE_COMPILERTASK_H

#include "stone/Basic/Color.h"
#include "stone/Basic/Status.h"
#include "stone/Option/ActionKind.h"
#include "stone/Compile/CompilerAllocation.h"

#include "llvm/Support/Chrono.h"

namespace stone {

class Compiler;
class CompilerQueue;
constexpr size_t CompilerTaskAlignInBits = 3;

class alignas(1 << CompilerTaskAlignInBits) CompilerTask
    : public CompilerAllocation<std::aligned_storage<8, 8>::type> {

  friend CompilerQueue;
  ActionKind source;

protected:
  bool hasDependency = false;
  bool isCompleted = false;
  bool skippingTask = false;

  llvm::sys::TimePoint<> startTime;
  llvm::sys::TimePoint<> endTime = llvm::sys::TimePoint<>::min();

public:
  CompilerTask(ActionKind source) : source(source) {}

public:
  bool HasDependency() { return hasDependency; }
  bool IsCompleted() { return isCompleted; }
  bool SkippingTask() { return skippingTask; }
  ActionKind GetSource() { return source; }
  bool IsCompilable() const;

public:
  virtual void Setup(Compiler &compiler) = 0;
  virtual Status Execute(Compiler &compiler, CompilerTask *dep = nullptr) = 0;
  virtual void Print(ColorStream &stream) = 0;
};

class PrintHelpTask final : public CompilerTask {

public:
  PrintHelpTask() : CompilerTask(ActionKind::PrintHelp) {}

public:
  virtual void Setup(Compiler &compiler) override {}
  virtual Status Execute(Compiler &compiler,
                         CompilerTask *dep = nullptr) override;

  virtual void Print(ColorStream &stream) override {}

public:
  static PrintHelpTask *Create(const Compiler &compiler);
};

class PrintVersionTask final : public CompilerTask {

public:
  PrintVersionTask() : CompilerTask(ActionKind::PrintVersion) {}

public:
  virtual void Setup(Compiler &compiler) override {}
  virtual Status Execute(Compiler &compiler, CompilerTask *dep) override;
  void Print(ColorStream &stream) override {}

public:
  static PrintVersionTask *Create(const Compiler &compiler);
};

class AbstractParseTask : public CompilerTask {

public:
  AbstractParseTask(ActionKind source) : CompilerTask(source) {}

public:
  virtual void Setup(Compiler &compiler) override {}
  virtual Status Execute(Compiler &compiler, CompilerTask *dep) override;
  virtual void Print(ColorStream &stream) override {}
};

class ParseTask final : public AbstractParseTask {

public:
  ParseTask() : AbstractParseTask(ActionKind::Parse) { hasDependency = true; }

public:
  virtual void Setup(Compiler &compiler) override {}
  virtual Status Execute(Compiler &compiler, CompilerTask *dep) override;
  virtual void Print(ColorStream &stream) override;

public:
  static ParseTask *Create(const Compiler &compiler);
};

class ResolveImportsTask : public CompilerTask {

public:
  ResolveImportsTask() : CompilerTask(ActionKind::ResolveImports) {
    hasDependency = true;
  }

public:
  virtual void Setup(Compiler &compiler) override {}
  virtual Status Execute(Compiler &compiler, CompilerTask *dep) override;
  virtual void Print(ColorStream &stream) override;

public:
  static ResolveImportsTask *Create(const Compiler &compiler);
};

class TypeCheckTask : public CompilerTask {

public:
  TypeCheckTask() : CompilerTask(ActionKind::TypeCheck) {
    hasDependency = true;
  }

public:
  virtual void Setup(Compiler &compiler) override {}
  virtual Status Execute(Compiler &compiler, CompilerTask *dep) override;
  virtual void Print(ColorStream &stream) override;

public:
  static TypeCheckTask *Create(const Compiler &compiler);
};

class AbstractEmitTask : public CompilerTask {

public:
  AbstractEmitTask(ActionKind source) : CompilerTask(source) {
    hasDependency = true;
  }

public:
  virtual void Setup(Compiler &compiler) override {}
  virtual Status Execute(Compiler &compiler, CompilerTask *dep) override;
  virtual void Print(ColorStream &stream) override;
};

class EmitIRBeforeTask : public AbstractEmitTask {

public:
  EmitIRBeforeTask() : AbstractEmitTask(ActionKind::EmitIRBefore) {
    hasDependency = true;
  }

public:
  virtual void Setup(Compiler &compiler) override {}
  virtual Status Execute(Compiler &compiler, CompilerTask *dep) override;
  virtual void Print(ColorStream &stream) override;

public:
  static EmitIRBeforeTask *Create(const Compiler &compiler);
};

class EmitIRAfterTask : public AbstractEmitTask {

public:
  EmitIRAfterTask() : AbstractEmitTask(ActionKind::EmitIRAfter) {
    hasDependency = true;
  }

public:
  virtual void Setup(Compiler &compiler) override {}
  virtual Status Execute(Compiler &compiler, CompilerTask *dep) override;
  virtual void Print(ColorStream &stream) override;

public:
  static EmitIRAfterTask *Create(const Compiler &compiler);
};

class EmitBitCodeTask : public AbstractEmitTask {

public:
  EmitBitCodeTask() : AbstractEmitTask(ActionKind::EmitBC) {
    hasDependency = true;
  }

public:
  virtual void Setup(Compiler &compiler) override {}
  virtual Status Execute(Compiler &compiler, CompilerTask *dep) override;
  virtual void Print(ColorStream &stream) override;

public:
  static EmitBitCodeTask *Create(const Compiler &compiler);
};

class EmitObjectTask : public AbstractEmitTask {

public:
  EmitObjectTask() : AbstractEmitTask(ActionKind::EmitObject) {
    hasDependency = true;
  }

public:
  virtual void Setup(Compiler &compiler) override {}
  virtual Status Execute(Compiler &compiler, CompilerTask *dep) override;
  virtual void Print(ColorStream &stream) override;

public:
  static EmitObjectTask *Create(const Compiler &compiler);
};

} // namespace stone
#endif
