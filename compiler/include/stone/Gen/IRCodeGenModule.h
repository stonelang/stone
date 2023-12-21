#ifndef STONE_GEN_IRCODEGENMODULE_H
#define STONE_GEN_IRCODEGENMODULE_H

#include "stone/Basic/Mem.h"
#include "stone/Basic/OutputFile.h"
#include "stone/Basic/STDAlias.h"
#include "stone/Gen/IRCodeGenMetadata.h"
#include "stone/Gen/IRCodeGenTypeCache.h"
#include "stone/Gen/IRCodeGenTypeResolver.h"
#include "stone/Syntax/ASTAllocation.h"
#include "stone/Syntax/ASTVisitor.h"
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

class SourceFile;
class IRGenModule;
class Decl;
class GlobalDecl;
class FunDecl;
class InterfaceDecl;
class StructDecl;
class EnumDecl;
class VarDecl;
class AutoDecl;
class SourceFile;
class NominalTypeDecl;
class CodeGenContext;
class CodeGenListener;

// class IRCodeGenLoop {};
// class IRCodeGenCall {};
class IRCodeGenBlocks final {
public:
};

struct EmitFunctionFlags final {
  EmitFunctionFlags() = delete;
  /// Flags that control the parsing of declarations.
  enum ID : UInt32 {
    None = 1 << 0,
    ForVirtualTable = 1 << 1,
    DontDefer = 1 << 2,
    IsThunk = 1 << 3,
    AllowEnumElement = 1 << 4,
    IsForDefinition = 1 << 5,
  };
};
/// Options that control the parsing of declarations.
using EmitFunctionOptions = stone::OptionSet<EmitFunctionFlags::ID>;

// TODO: Use ASTAllocation in the future?
class IRCodeGenResult final : public MemoryAllocation<IRCodeGenResult> {

  std::unique_ptr<llvm::LLVMContext> llvmContext;
  std::unique_ptr<llvm::Module> llvmModule;
  std::unique_ptr<llvm::TargetMachine> llvmTargetMachine;

  IRCodeGenResult()
      : llvmContext(nullptr), llvmModule(nullptr), llvmTargetMachine(nullptr) {}

public:
  IRCodeGenResult(IRCodeGenResult const &) = delete;
  IRCodeGenResult &operator=(IRCodeGenResult const &) = delete;
  IRCodeGenResult(IRCodeGenResult &&) = default;
  IRCodeGenResult &operator=(IRCodeGenResult &&) = default;

public:
  /// Construct a \c IRCodeGenResult  that owns a given module and context.
  ///
  /// The given pointers must not be null. If a null \c IRCodeGenResult  is
  /// needed, use \c IRCodeGenResult ::null() instead.
  explicit IRCodeGenResult(std::unique_ptr<llvm::LLVMContext> &&llvmContext,
                           std::unique_ptr<llvm::Module> &&llvmModule,
                           std::unique_ptr<llvm::TargetMachine> &&Target)
      : llvmContext(std::move(llvmContext)), llvmModule(std::move(llvmModule)),
        llvmTargetMachine(std::move(llvmTargetMachine)) {}

public:
  explicit operator bool() const {
    return llvmModule != nullptr && llvmContext != nullptr;
  }

public:
  const llvm::Module *GetLLVMModule() const { return llvmModule.get(); }
  llvm::Module *GetLLVMModule() { return llvmModule.get(); }

  const llvm::LLVMContext *GetLLVMContext() const { return llvmContext.get(); }
  llvm::LLVMContext *GetLLVMContext() { return llvmContext.get(); }

  const llvm::TargetMachine *GetTargetMachine() const {
    return llvmTargetMachine.get();
  }
  llvm::TargetMachine *GetTargetMachine() { return llvmTargetMachine.get(); }

public:
  /// Release ownership of the context and module to the caller, consuming
  /// this value in the process.
  ///
  /// The REPL is the only caller that needs this. New uses of this function
  /// should be avoided at all costs.
  std::pair<llvm::LLVMContext *, llvm::Module *> release() && {
    return {llvmContext.release(), llvmModule.release()};
  }

public:
  static IRCodeGenResult *
  Create(MemoryContext &memContext,
         std::unique_ptr<llvm::LLVMContext> &&llvmContext,
         std::unique_ptr<llvm::Module> &&llvmModule,
         std::unique_ptr<llvm::TargetMachine> &&llvmTargetMachine);

public:
  /// Transfers ownership of the underlying module and context to an
  /// ORC-compatible context.
  // llvm::orc::ThreadSafeModule IntoThreadSafeContext() &&;
};

class IRCodeGen final {

  const CodeGenOptions &codeGenOpts;
  ASTContext &astContext;

  llvm::DenseMap<SourceFile *, IRCodeGenModule *> irCodeGenModules;

  llvm::SmallVector<IRCodeGenModule *, 8> queue;

  // The IGM of the first source file.
  IRCodeGenModule *primaryCodeGenModule = nullptr;

  // The current IGM for which IR is generated.
  IRCodeGenModule *currentCodeGenModule = nullptr;

public:
  std::unique_ptr<llvm::LLVMContext> llvmContext;
  std::unique_ptr<llvm::TargetMachine> llvmTargetMachine;

private:
  IRCodeGen(const IRCodeGen &) = delete;
  void operator=(const IRCodeGen &) = delete;

public:
  explicit IRCodeGen(const CodeGenOptions &codeGenOpts, ASTContext &astContext);

public:
  /// Add an IRCodeGenModule for a source file.
  /// Should only be called from IRCodeGenModule's constructor.
  void AddIRCodeGenModule(SourceFile *sourceFile,
                          IRCodeGenModule *codeGenModule);

  /// Get an IRGenModule for a declaration context.
  /// Returns the IRCodeGenModule of the containing source file, or if this
  /// cannot be determined, returns the primary IRGenModule.
  IRCodeGenModule *GetIRCodeGenModule(DeclContext *ctxt);

public:
  ASTContext &GetASTContext() { return astContext; }
  llvm::LLVMContext &GetLLVMContext() { return *llvmContext; }
  llvm::TargetMachine &GetTargetMachine() { return *llvmTargetMachine; }
  const CodeGenOptions &GetCodeGenOptions() const { return codeGenOpts; }

public:
  // IRCodeGenResule* GenCode();
public:
  /// Return the effective triple used by clang.
  llvm::Triple GetEffectiveClangTriple();
  const llvm::StringRef GetClangDataLayoutString();
};

class IRCodeGenModule final : public ASTVisitor<IRCodeGenModule> {

  IRCodeGen &irCodeGen;
  IRCodeGenTypeCache typeCache;
  IRCodeGenTypeResolver typeResolver;
  IRCodeGenMetadata metadata;
  // IRCodeGenDebug debug;
  llvm::StringRef outputFilename;

  const llvm::DataLayout dataLayout;
  const llvm::Triple triple;

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

  // std::unique_ptr<llvm::Module> llvmModule;
  std::unique_ptr<clang::CodeGenerator> clangCodeGen;

private:
  IRCodeGenModule(const IRCodeGenModule &) = delete;
  void operator=(const IRCodeGenModule &) = delete;

public:
  IRCodeGenModule(IRCodeGen &irCodeGen, SourceFile *sourceFile,
                  llvm::StringRef moduleName, llvm::StringRef outputFilename);
  ~IRCodeGenModule();

public:
  void Setup();
  void SetupLLVMModule();

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
  IRCodeGen &GetIRCodeGen() { return irCodeGen; }
  IRCodeGenTypeCache &GetIRCodeGenTypeCache() { return typeCache; }
  IRCodeGenTypeResolver &GetIRCodeGenTypeResolver() { return typeResolver; }
  IRCodeGenMetadata &GetIRCodeGenMetadata() { return metadata; }
  // IRCodeGenDebug &GetIRCodeGenDebug() { return debug; }

  const llvm::DataLayout &GetDataLayout() { return dataLayout; }
  const llvm::Triple &GetTriple() { return triple; }

public:
  void EmitSourceFile(SourceFile &sf);
  void EmitGlobalDecl(Decl *d);
  /// Emit functions, variables and tables which are needed anyway, e.g. because
  /// they are externally visible.
  // void EmitGlobalTopLevel(const std::vector<std::string> &linkerDirectives);

  // void EmitGlobalDecl(GlobalDecl *gd);
  void EmitInterfaceDecl(InterfaceDecl *d);
  void EmitStructDecl(StructDecl *d);
  void EmitEnumDecl(EnumDecl *d);
  void EmitVarDecl(EnumDecl *d);
  void EmitAutoDecl(AutoDecl *d);

  void EmitConstructorDecl(ConstructorDecl *d);
  void EmitDestructorDecl(DestructorDecl *dd);

private:
  llvm::Constant *
  CreateFunction(llvm::StringRef mangledName, FunctionDecl *fd,
                 llvm::Type *fnTy, const EmitFunctionOptions emitFunctionOpts,
                 llvm::AttributeList extraAttrs = llvm::AttributeList());
  llvm::Constant *
  GetOrCreateFunction(llvm::StringRef mangledName, FunctionDecl *fd,
                      llvm::Type *fnTy,
                      const EmitFunctionOptions emitFunctionOpts,
                      llvm::AttributeList extraAttrs = llvm::AttributeList());

public:
  void EmitFunDecl(FunDecl *fd, llvm::GlobalValue *globalValue = nullptr);

  llvm::Constant *
  GetFunctionAddress(FunctionDecl *fd, llvm::Type *functionTy,
                     const EmitFunctionOptions emitFunctionOpts);

  /// Emits the function definition for a given SILDeclRef.
  // void EmitFunctionDefinition(FunDecl *fd);

  // See Clang CodeGenModule
  void SetFunctionLinkage(FunctionDecl *fd, llvm::Function *fn);
  llvm::GlobalValue::LinkageTypes GetFunctionLinkage(FunctionDecl *fd);

public:
  // llvm::Constant *GetBuiltinLibFunction(const FunctionDecl *FD,
  //                                       unsigned BuiltinID);

  // llvm::Module &GetLLVMModule() { return *llvmModule; }
  clang::CodeGenerator &GetClangCodeGen() { return *clangCodeGen; }

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

class IRCodeGenModuleOptimizer {};

} // namespace stone

#endif
