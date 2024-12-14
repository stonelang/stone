#ifndef STONE_CODEGEN_CODEGENMODULE_H
#define STONE_CODEGEN_CODEGENMODULE_H

#include "stone/AST/Diagnostics.h"
#include "stone/AST/Module.h"
#include "stone/CodeGen/CodeGen.h"
#include "stone/CodeGen/CodeGenPassManager.h"

#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
class QualType;
class FunctionType;
class IntegerType;
class PointerType;
} // namespace llvm

namespace stone {
class QualType;
class SourceFile;
class AutoDecl;
class Decl;
class ConstructorDecl;
class DestructorDecl;
class InterfaceDecl;
class VarDecl;
class CodeGenModule;
class CodeGenFunction;
class FunctionDecl;


class CodeGenConstant final {
public:
  CodeGenModule &cgm;
  CodeGenFunction *cgf;

public:
  CodeGenConstant(const CodeGenConstant &other) = delete;
  CodeGenConstant &operator=(const CodeGenConstant &other) = delete;

public:
  CodeGenConstant(CodeGenModule &cgm, CodeGenFunction *cgf = nullptr);

  ~CodeGenConstant();
};

class CodeGenModule final {
public:
  llvm::Type *VoidTy;

  llvm::IntegerType *Int8Ty;  /// i8
  llvm::IntegerType *Int16Ty; /// i16
  llvm::IntegerType *Int32Ty; /// i32
  llvm::IntegerType *Int64Ty; /// i64
  llvm::IntegerType *IntTy;   /// int
  llvm::IntegerType *CharTy;  /// char

  llvm::PointerType *Int8PtrTy;  /// i16*
  llvm::PointerType *Int16PtrTy; /// i16*
  llvm::PointerType *Int32PtrTy; /// i32*
  llvm::PointerType *Int64PtrTy; /// i64*
  llvm::PointerType *IntPtrTy;   ///  int*

  // LLVM Address types
  llvm::IntegerType *RelativeAddressTy;
  llvm::PointerType *RelativeAddressPtrTy;

  // llvm::CallingConv::ID CCC;          /// standard C calling convention
  // llvm::CallingConv::ID DefaultCC;     /// default calling convention

private:
  CodeGen &codeGen;
  SourceFile *sourceFile;
  CodeGenPassManager codeGenPassMgr;
  ModuleNameAndOuptFileName moduleNameAndOuptFileName;
  std::unique_ptr<clang::CodeGenerator> clangCodeGen;
  const llvm::DataLayout dataLayout;
  std::unique_ptr<clang::CodeGenerator> CreateClangCodeGen();

public:
  /// The \p sourceFile is used when the llvm module is generated in a
  /// doing multi-threaded and whole-module compilation -- null otherwise.
  CodeGenModule(CodeGen &codeGen, SourceFile *sourceFile,
                ModuleNameAndOuptFileName moduleNameAndOuptFileName);

public:
  CodeGen &GetCodeGen() { return codeGen; }
  SourceFile *GetSourceFile() { return sourceFile; }
  bool HasSourceFile() { return sourceFile != nullptr; }

  ModuleNameAndOuptFileName GetModuleNameAndOuptFileName() {
    return moduleNameAndOuptFileName;
  }
  CodeGenPassManager &GetCodeGenPassManager() { return codeGenPassMgr; }
  clang::CodeGenerator &GetClangCodeGen() { return *clangCodeGen; }
  const llvm::DataLayout &GetDataLayout() { return dataLayout; }

public:
  void EmitSourceFile(SourceFile &sf);
  void EmitGlobalDecl(Decl *d);
  void EmitFunDecl(FunDecl *d);
  void EmitInterfaceDecl(InterfaceDecl *d);
  void EmitStructDecl(StructDecl *d);
  void EmitEnumDecl(EnumDecl *d);
  void EmitVarDecl(VarDecl *d);
  void EmitAutoDecl(AutoDecl *d);

  void EmitConstructorDecl(ConstructorDecl *d);
  void EmitDestructorDecl(DestructorDecl *dd);

public:
  llvm::Type *GetType(const QualType ty);
  llvm::FunctionType *GetFunctionType(const FunctionDecl *functionDecl);
};

} // namespace stone

#endif
