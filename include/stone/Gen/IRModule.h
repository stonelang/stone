#ifndef STONE_GEN_IRMODULE_H
#define STONE_GEN_IRMODULE_H

#include <memory>

#include "stone/Syntax/Module.h"
#include "llvm/IR/Module.h"

namespace llvm {
class raw_pwrite_stream;
class GlobalVariable;
class MemoryBuffer;
class Module;
class TargetOptions;
class TargetMachine;
} // namespace llvm

namespace stone {

class IRGen;
class IRModule {
  std::unique_ptr<llvm::Module> llvmModule;

public:
  /// Return the module that this code generator is building into.
  ///
  /// This may return null after HandleTranslationUnit is called;
  /// this signifies that there was an error generating code.  A
  /// diagnostic will have been generated in this case, and the module
  /// will be deleted.
  ///
  /// It will also return null if the module is released.
  llvm::Module *GetLLVMModule();
  llvm::Module *ReleaseLLVMModule();
};

class IRModuleEmitter final {
  IRGen &irGen;
  std::unique_ptr<IRModule> irModule;

private:
  IRModuleEmitter(const IRModuleEmitter &) = delete;
  void operator=(const IRModuleEmitter &) = delete;

public:
  IRModuleEmitter(IRGen &irGen);
  ~IRModuleEmitter();

public:
  void EmitSyntaxFile(const syn::SyntaxFile &sf);
  void EmitTopLevelDecl(syn::Decl *topLvelDecl);
  void EmitFunDecl(syn::FunDecl *topLvelDecl);
  void EmitInterfaceDecl(syn::InterfaceDecl *topLevelDecl);
  void EmitStructDecl(syn::StructDecl *topLvelDecl);
  void EmitEnumDecl(syn::EnumDecl *topLvelDecl);

  /// It will also return null if the module is released.
  // IRModule *GetIRModule();
};
} // namespace stone

#endif
