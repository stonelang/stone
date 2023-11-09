#ifndef STONE_GEN_CODEGENMODULE_H
#define STONE_GEN_CODEGENMODULE_H

#include "stone/AST/ASTVisitor.h"
#include "stone/AST/Module.h"
#include "stone/Basic/OutputFile.h"
#include "stone/Basic/STDAlias.h"
#include "stone/CodeGen/CodeGenContext.h"
#include "stone/CodeGen/CodeGenMetadata.h"
#include "stone/CodeGen/CodeGenTypeCache.h"
#include "stone/CodeGen/CodeGenTypeResolver.h"

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

using namespace stone;

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

class ASTFile;
class Decl;
class GlobalDecl;
class FunDecl;
class InterfaceDecl;
class StructDecl;
class EnumDecl;
class VarDecl;
class AutoDecl;
class ASTFile;
class NominalTypeDecl;
class CodeGenListener;

// class CodeGenLoop {};
// class CodeGenCall {};
class CodeGenBlocks final {
public:
};

struct EmitFunctionFlags final {
  EmitFunctionFlags() = delete;
  /// Flags that control the parsing of declarations.
  enum ID : UInt32 {
    None = 1 << 0,
    ForVirtualTable = 1 << 1,
    DontDefer = 1 << 2,
    IsSlab = 1 << 3,
    AllowEnumElement = 1 << 4,
    IsForDefinition = 1 << 5,
  };
};
/// Options that control the parsing of declarations.
using EmitFunctionOptions = stone::OptionSet<EmitFunctionFlags::ID>;

class CodeGenModule final : public stone::ASTVisitor<CodeGenModule> {

  CodeGenContext &cgc;
  CodeGenTypeCache typeCache;
  CodeGenTypeResolver typeResolver;
  CodeGenMetadata metadata;
  // CodeGenDebug debug;

  llvm::StringRef moduleName;
  llvm::StringRef outputFilename;

  ASTFile *curASTFile = nullptr;

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
  CodeGenModule(const CodeGenModule &) = delete;
  void operator=(const CodeGenModule &) = delete;

public:
  CodeGenModule(CodeGenContext &cgc, llvm::StringRef moduleName,
                llvm::StringRef outputFilename);
  ~CodeGenModule();

public:
  enum IsForFunctionDefintion : bool {
    NotForDefinition = false,
    ForDefinition = true
  };

public:
  // Globals

  // llvm::DenseMap<LinkEntity, llvm::Constant*> globalVars;
  // llvm::DenseMap<LinkEntity, llvm::Constant*> globalGOTEquivalents;
  // llvm::DenseMap<LinkEntity, llvm::Function*> globalFunctions;
  // llvm::DenseSet<const clang::Decl *> globalClangDecls;
  // llvm::StringMap<std::pair<llvm::GlobalVariable*, llvm::Constant*>>
  //   globalStrings;

  llvm::SmallVector<InterfaceDecl *, 4> interfaces;

public:
  CodeGenContext &GetCodeGenContext() { return cgc; }
  CodeGenTypeCache &GetCodeGenTypeCache() { return typeCache; }
  CodeGenTypeResolver &GetCodeGenTypeResolver() { return typeResolver; }
  CodeGenMetadata &GetCodeGenMetadata() { return metadata; }
  // CodeGenDebug &GetCodeGenDebug() { return debug; }

public:
  void EmitASTFile(ASTFile &sf);

  void EmitGlobalDecl(Decl *d);

  /// Emit functions, variables and tables which are needed anyway, e.g. because
  /// they are externally visible.
  // void EmitGlobalTopLevel(const std::vector<std::string> &linkerDirectives);

  // void EmitGlobalDecl(stone::GlobalDecl *gd);
  void EmitInterfaceDecl(InterfaceDecl *d);
  void EmitStructDecl(StructDecl *d);
  void EmitEnumDecl(EnumDecl *d);
  void EmitVarDecl(EnumDecl *d);
  void EmitAutoDecl(AutoDecl *d);

  void EmitConstructorDecl(ConstructorDecl *d);
  void EmitDestructorDecl(DestructorDecl *dd);

private:
  llvm::Constant *
  CreateFunction(llvm::StringRef mangledName, stone::FunctionDecl *fd,
                 llvm::Type *fnTy, const EmitFunctionOptions emitFunctionOpts,
                 llvm::AttributeList extraAttrs = llvm::AttributeList());
  llvm::Constant *
  GetOrCreateFunction(llvm::StringRef mangledName, stone::FunctionDecl *fd,
                      llvm::Type *fnTy,
                      const EmitFunctionOptions emitFunctionOpts,
                      llvm::AttributeList extraAttrs = llvm::AttributeList());

public:
  void EmitFunDecl(FunDecl *fd, llvm::GlobalValue *globalValue = nullptr);

  llvm::Constant *
  GetFunctionAddress(stone::FunctionDecl *fd, llvm::Type *functionTy,
                     const EmitFunctionOptions emitFunctionOpts);

  /// Emits the function definition for a given SILDeclRef.
  // void EmitFunctionDefinition(FunDecl *fd);

  // See Clang CodeGenModule
  void SetFunctionLinkage(stone::FunctionDecl *fd, llvm::Function *fn);
  llvm::GlobalValue::LinkageTypes GetFunctionLinkage(stone::FunctionDecl *fd);

public:
  // llvm::Constant *GetBuiltinLibFunction(const FunctionDecl *FD,
  //                                       unsigned BuiltinID);

private:
  void Emit();

public:
  llvm::StringRef GetMangledName(Decl &d);

public:
  // clang::CanQual<clang::Type> GetClangType(CanType type);

  llvm::GlobalValue *GetGlobalValue(llvm::StringRef name);

  /// Given a global declaration, return a mangled name for this declaration
  /// which has been added to this code generator via a Handle method.
  llvm::StringRef GetMangledNameOfGlobalDecl(Decl *d);

  /// Return the LLVM address of the given global entity.
  ///
  /// \param isForDefinition If true, the caller intends to define the
  ///   entity; the object returned will be an llvm::GlobalValue of
  ///   some sort.  If false, the caller just intends to use the entity;
  ///   the object returned may be any sort of constant value, and the
  ///   code generator will schedule the entity for emission if a
  ///   definition has been registered with this code generator.
  llvm::Constant *GetAddressOfGlobalDecl(Decl *d, bool isForDefinition);

public:
  static Int64 Clamp(Int64 val, Int64 low, Int64 high) {
    return std::min(high, std::max(low, val));
  }
};
} // namespace stone

#endif
