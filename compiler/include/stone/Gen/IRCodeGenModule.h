#ifndef STONE_GEN_IRCODEGENMODULE_H
#define STONE_GEN_IRCODEGENMODULE_H

#include "stone/Basic/OutputFile.h"
#include "stone/Basic/STDAlias.h"
#include "stone/Gen/IRCodeGenTypeCache.h"
#include "stone/Gen/IRCodeGenTypeResolver.h"
#include "stone/Syntax/Module.h"
#include "stone/Syntax/SyntaxVisitor.h"

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
namespace syn {
class SyntaxFile;
class Decl;
class GlobalDecl;
class FunDecl;
class InterfaceDecl;
class StructDecl;
class EnumDecl;
class VarDecl;
class AutoDecl;
class SyntaxFile;
class NominalTypeDecl;
} // namespace syn

class IRCodeGen;
class CodeGenListener;

// class IRCodeGenLoop {};
// class IRCodeGenCall {};
class IRCodeGenBlocks final {
public:
};

class IRCodeGenModule final : public SyntaxVisitor<IRCodeGenModule> {

  IRCodeGen &irCodeGen;
  IRCodeGenTypeCache typeCache;
  IRCodeGenTypeResolver typeResolver;

  llvm::StringRef moduleName;
  llvm::StringRef outputFilename;

  SyntaxFile *curSyntaxFile = nullptr;

  // llvm::SetVector<CanType> builtinTypes;
  //  /// Opaque but fixed-size types for which we also emit builtin type
  //  /// descriptors, allowing the reflection library to layout these types
  //  /// without knowledge of their contents. This includes imported structs
  //  /// and fixed-size multi-payload enums.
  //  llvm::SetVector<const NominalTypeDecl *> opaqueTypes;
  //  // /// Imported protocols referenced by types in this module when emitting
  //  // /// reflection metadata.
  //  llvm::SetVector<const InterfaceDecl *> importedInterfaces;
  //  // /// Imported structs referenced by types in this module when emitting
  //  // /// reflection metadata.
  //  llvm::SetVector<const StructDecl *> importedStructs;

private:
  IRCodeGenModule(const IRCodeGenModule &) = delete;
  void operator=(const IRCodeGenModule &) = delete;

public:
  IRCodeGenModule(IRCodeGen &irCodeGen, llvm::StringRef moduleName,
                  llvm::StringRef outputFilename);
  ~IRCodeGenModule();

public:
  // Globals

  // llvm::DenseMap<LinkEntity, llvm::Constant*> globalVars;
  // llvm::DenseMap<LinkEntity, llvm::Constant*> globalGOTEquivalents;
  // llvm::DenseMap<LinkEntity, llvm::Function*> globalFunctions;
  // llvm::DenseSet<const clang::Decl *> globalClangDecls;
  // llvm::StringMap<std::pair<llvm::GlobalVariable*, llvm::Constant*>>
  //   globalStrings;

public:
  IRCodeGen &GetIRCodeGen() { return irCodeGen; }
  IRCodeGenTypeCache &GetIRCodeGenTypeCache() { return typeCache; }
  IRCodeGenTypeResolver &GetIRCodeGenTypeResolver() { return typeResolver; }

public:
  void EmitSyntaxFile(SyntaxFile &sf);
  void EmitGlobalDecl(Decl *d);
  // void EmitGlobalDecl(syn::GlobalDecl *gd);
  void EmitFunDecl(FunDecl *d);
  void EmitInterfaceDecl(InterfaceDecl *d);
  void EmitStructDecl(StructDecl *d);
  void EmitEnumDecl(EnumDecl *d);
  void EmitVarDecl(EnumDecl *d);
  void EmitAutoDecl(AutoDecl *d);
  void EmitConstructorDecl(ConstructorDecl *d);
  void EmitDestructorDecl(DestructorDecl *d);

public:
  // llvm::Constant *getBuiltinLibFunction(const FunctionDecl *FD,
  //                                       unsigned BuiltinID);

private:
  void Emit();

public:
  static Int64 Clamp(Int64 val, Int64 low, Int64 high) {
    return std::min(high, std::max(low, val));
  }
};
} // namespace stone

#endif
