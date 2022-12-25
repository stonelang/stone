#ifndef STONE_GEN_IRCODEGENBUILDER_H
#define STONE_GEN_IRCODEGENBUILDER_H

#include "stone/Basic/LLVM.h"
#include "stone/Gen/CodeGenContext.h"
#include "stone/Syntax/Module.h"

#include "llvm/IR/DataLayout.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Type.h"

#include <memory>

namespace llvm {
class raw_pwrite_stream;
class GlobalVariable;
class MemoryBuffer;
class Module;
class TargetOptions;
class TargetMachine;
} // namespace llvm

namespace stone {

class IRCodeGenFunction;
class IRCodeGenTypeCache;

class IRCodeGenBuilderInserter : public llvm::IRBuilderDefaultInserter {
private:
  IRCodeGenFunction *cgf = nullptr;

public:
  IRCodeGenBuilderInserter() = default;
  explicit IRCodeGenBuilderInserter(IRCodeGenFunction *cgf) : cgf(cgf) {}

  /// This forwards to CodeGenFunction::InsertHelper.
  void InsertHelper(llvm::Instruction *instruction, const llvm::Twine &name,
                    llvm::BasicBlock *basicBlock,
                    llvm::BasicBlock::iterator insertPt) const override;
};

using IRCodeGenBuilderBase =
    llvm::IRBuilder<llvm::ConstantFolder, IRCodeGenBuilderInserter>;

class IRCodeGenBuilder final : public IRCodeGenBuilderBase {
  CodeGenContext &cgc;
  const IRCodeGenTypeCache &typeCache;

public:
  IRCodeGenBuilder(CodeGenContext &cgc, const IRCodeGenTypeCache &typeCache);

  IRCodeGenBuilder(CodeGenContext &cgc, const IRCodeGenTypeCache &typeCache,
                   const llvm::ConstantFolder &constFoler,
                   const IRCodeGenBuilderInserter &inserter);

  IRCodeGenBuilder(CodeGenContext &cgc, const IRCodeGenTypeCache &typeCache,
                   llvm::Instruction *instruction);

  IRCodeGenBuilder(CodeGenContext &cgc, const IRCodeGenTypeCache &typeCache,
                   llvm::BasicBlock *basicBlock);

  ~IRCodeGenBuilder();

public:
  // llvm::ConstantInt *GetSize(clang::CharUnits N) {
  //   return llvm::ConstantInt::get(TypeCache.SizeTy, N.getQuantity());
  // }
  // llvm::ConstantInt *GetSize(UInt64 N) {
  //   return llvm::ConstantInt::get(TypeCache.SizeTy, N);
  // }

  // Note that we intentionally hide the CreateLoad APIs that don't
  // take an alignment.
  // llvm::LoadInst *CreateLoad(Address Addr, const llvm::Twine &Name = "") {
  //   return CreateAlignedLoad(Addr.getElementType(), Addr.getPointer(),
  //                            Addr.getAlignment().getAsAlign(), Name);
  // }
  // llvm::LoadInst *CreateLoad(Address Addr, const char *Name) {
  //   // This overload is required to prevent string literals from
  //   // ending up in the IsVolatile overload.
  //   return CreateAlignedLoad(Addr.getElementType(), Addr.getPointer(),
  //                            Addr.getAlignment().getAsAlign(), Name);
  // }
  // llvm::LoadInst *CreateLoad(Address Addr, bool IsVolatile,
  //                            const llvm::Twine &Name = "") {
  //   return CreateAlignedLoad(Addr.getElementType(), Addr.getPointer(),
  //                            Addr.getAlignment().getAsAlign(), IsVolatile,
  //                            Name);
  // }
};
} // namespace stone

#endif