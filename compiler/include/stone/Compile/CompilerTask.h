#ifndef STONE_COMPILE_COMPILERTASK_H
#define STONE_COMPILE_COMPILERTASK_H

#include "stone/Compile/CompilerAllocation.h"

namespace stone {

class Compiler;
constexpr size_t CompilerTaskAlignInBits = 3;

class alignas(1 << CompilerTaskAlignInBits) CompilerTask
    : public CompilerAllocation<std::aligned_storage<8, 8>::type> {
public:
  virtual Status Execute(Compiler &compiler) {}
};

// class CompilerStatsTask : public CompilerTask {
// public:
//   virtual Status Execute(Compiler &compiler) override;
// };

// class VerifyInputFileTypesTask : public CompilerTask {
// public:
//   virtual Status Execute(Compiler &compiler) override;
// };

// class SyntaxTask : public CompilerTask {

// public:
//   virtual Status Execute(Compiler &compiler) override;
// };

// class ParseTask : public SyntaxTask {
// public:
//   virtual Status Execute(Compiler &compiler) override;
// };

// class ParseAndImportResolutionTask : public SyntaxTask {
// public:
//   virtual Status Execute(Compiler &compiler) override;

// public:
//   static ParseAndImportResolutionTask *Create();
// };

// class TypeCheckTask : public SyntaxTask {
// public:
//   virtual Status Execute(Compiler &compiler);

// public:
//   static ParseAndImportResolutionTask *Create();
// };

// class EmitIRTask : public SyntaxTask {
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
