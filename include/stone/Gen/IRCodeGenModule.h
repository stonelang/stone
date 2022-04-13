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

class IRCodeGen;
class IRCodeGenModule final {
  IRCodeGen &irCodeGen;

private:
  IRCodeGenModule(const IRCodeGenModule &) = delete;
  void operator=(const IRCodeGenModule &) = delete;

public:
  IRCodeGenModule(IRCodeGen &irCodeGen);
  ~IRCodeGenModule();

public:
  void EmitSyntaxFile(const syn::SyntaxFile &sf);
  void EmitTopLevelDecl(syn::Decl *topLvelDecl);
  void EmitFunDecl(syn::FunDecl *topLvelDecl);
  void EmitInterfaceDecl(syn::InterfaceDecl *topLevelDecl);
  void EmitStructDecl(syn::StructDecl *topLvelDecl);
  void EmitEnumDecl(syn::EnumDecl *topLvelDecl);
};
} // namespace stone

#endif
