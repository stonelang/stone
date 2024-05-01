#ifndef STONE_GEN_IRCODEGENBUILDER_H
#define STONE_GEN_IRCODEGENBUILDER_H

#include "stone/AST/Module.h"
#include "stone/Basic/LLVM.h"
#include "stone/Gen/IRGenInvocation.h"


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

class IRGenFunction;
struct IRGenTypeCache;

class IRGenBuilderInserter : public llvm::IRBuilderDefaultInserter {
private:
  IRGenFunction *cgf = nullptr;

public:
  IRGenBuilderInserter() = default;
  explicit IRGenBuilderInserter(IRGenFunction *cgf) : cgf(cgf) {}

  /// This forwards to CodeGenFunction::InsertHelper.
  void InsertHelper(llvm::Instruction *instruction, const llvm::Twine &name,
                    llvm::BasicBlock *basicBlock,
                    llvm::BasicBlock::iterator insertPt) const override;
};

using IRGenBuilderBase =
    llvm::IRBuilder<llvm::ConstantFolder, IRGenBuilderInserter>;

class IRGenBuilder final : public IRGenBuilderBase {
  IRGenInvocation &invocation;
  const IRGenTypeCache &typeCache;

public:
  IRGenBuilder(IRGenInvocation &invocation, const IRGenTypeCache &typeCache);

  IRGenBuilder(IRGenInvocation &invocation, const IRGenTypeCache &typeCache,
               const llvm::ConstantFolder &constFoler,
               const IRGenBuilderInserter &inserter);

  IRGenBuilder(IRGenInvocation &invocation, const IRGenTypeCache &typeCache,
               llvm::Instruction *instruction);

  IRGenBuilder(IRGenInvocation &invocation, const IRGenTypeCache &typeCache,
               llvm::BasicBlock *basicBlock);

  ~IRGenBuilder();

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