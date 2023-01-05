#ifndef STONE_GEN_IRCODEGEN_H
#define STONE_GEN_IRCODEGEN_H

#include "stone/Basic/LLVM.h"
#include "stone/Gen/CodeGenContext.h"
#include "stone/Gen/IRCodeGenBuilder.h"
#include "stone/Gen/IRCodeGenTypeCache.h"
#include "stone/Syntax/Module.h"

#include "llvm/IR/PassManager.h"

namespace llvm {
class raw_pwrite_stream;
class GlobalVariable;
class MemoryBuffer;
class Module;
class TargetOptions;
class TargetMachine;
} // namespace llvm

namespace stone {

class CodeGenListener;
class IRCodeGen final {
  CodeGenContext &cgc;
  CodeGenListener *listener;

public:
  IRCodeGen(CodeGenContext &cgc, CodeGenListener *listener = nullptr);
  ~IRCodeGen();
  // IRCodeGenFunction &GetIRCodeGenFunction();

  CodeGenContext &GetCodeGenContext() { return cgc; }
  CodeGenListener *GetCodeGenListener() { return listener; }

public:
  /// Given a global declaration, return a mangled name for this declaration
  /// which has been added to this code generator via a Handle method.
  // llvm::StringRef GetMangledNameForGlobalDecl(Decl *d);

  /// Return the LLVM address of the given global entity.
  ///
  /// \param isForDefinition If true, the caller intends to define the
  ///   entity; the object returned will be an llvm::GlobalValue of
  ///   some sort.  If false, the caller just intends to use the entity;
  ///   the object returned may be any sort of constant value, and the
  ///   code generator will schedule the entity for emission if a
  ///   definition has been registered with this code generator.
  // llvm::Constant *GetAddressForGlobalDecl(Decl *d, bool isForDefinition);
};
} // namespace stone

#endif