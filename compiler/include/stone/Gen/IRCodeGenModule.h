#ifndef STONE_GEN_IRCODEGENMODULE_H
#define STONE_GEN_IRCODEGENMODULE_H

#include "stone/Basic/OutputFile.h"
#include "stone/Syntax/Module.h"

#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/Hashing.h"
#include "llvm/ADT/MapVector.h"
#include "llvm/ADT/SetVector.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/IR/Attributes.h"
#include "llvm/IR/CallingConv.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/ValueHandle.h"
#include "llvm/Target/TargetMachine.h"

#include <atomic>
#include <memory>

using namespace stone::syn;

namespace llvm {
class Constant;
class ConstantInt;
class DataLayout;
class Function;
class FunctionType;
class GlobalVariable;
class InlineAsm;
class IntegerType;
class LLVMContext;
class MDNode;
class Metadata;
class Module;
class PointerType;
class StructType;
class StringRef;
class Type;
class AttributeList;
} // namespace llvm

namespace llvm {
class raw_pwrite_stream;
class GlobalVariable;
class MemoryBuffer;
class Module;
class TargetOptions;
class TargetMachine;
} // namespace llvm

namespace stone {

class IRCodeGen;

struct IRCodeGenTypes final {

  /// void (usually {})
  llvm::Type *VoidTy;

  // LLVM Address types
  llvm::IntegerType *RelativeAddressTy;
  llvm::PointerType *RelativeAddressPtrTy;

  /// LLVM basic types

  llvm::IntegerType *Int1Ty;     /// i1
  llvm::IntegerType *Int8Ty;     /// i8
  llvm::IntegerType *Int16Ty;    /// i16
  llvm::IntegerType *Int32Ty;    /// i32
  llvm::PointerType *Int32PtrTy; /// i32 *
  llvm::IntegerType *Int64Ty;    /// i64
};

// I do not think that you need this -- just use IRCodeGen
class IRCodeGenModule final {
  IRCodeGen &irCodeGen;
  syn::SyntaxFile *sf;
  const OutputFile *outputFile;

  IRCodeGenTypes irCodeGenTypes;

private:
  IRCodeGenModule(const IRCodeGenModule &) = delete;
  void operator=(const IRCodeGenModule &) = delete;

public:
  IRCodeGenModule(IRCodeGen &irCodeGen, syn::SyntaxFile *sf,
                  const OutputFile *outputFile);

  ~IRCodeGenModule();

public:
  void EmitSyntaxFile(const SyntaxFile &sf);
  void EmitDecl(Decl *d);
  void EmitFunDecl(FunDecl *d);
  void EmitInterfaceDecl(InterfaceDecl *d);
  void EmitStructDecl(StructDecl *d);
  void EmitEnumDecl(EnumDecl *d);
};
} // namespace stone

#endif
