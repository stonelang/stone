#include "stone/Gen/IRModule.h"

#include "stone/Gen/IRGen.h"
#include "stone/Syntax/Decl.h"

using namespace stone;
using namespace stone::syn;

// llvm::Module *IRModule::GetLLVMModule() { return llvmModule.get(); }
// llvm::Module *IRModule::ReleaseLLVMModule() { return llvmModule.release(); }

IRModuleEmitter::IRModuleEmitter(IRGen &irGen) : irGen(irGen) {}

void IRModuleEmitter::EmitSyntaxFile(const syn::SyntaxFile &sf) {
  // Walk through the syntax file and call emit
  // Emit types and other global decls.
  for (auto &topLevelDecl : sf.topLevelDecls) {
    EmitTopLevelDecl(&topLevelDecl);
  }
}
void IRModuleEmitter::EmitTopLevelDecl(Decl *topLevelDecl) {
  switch (topLevelDecl->GetKind()) {
  case DeclKind::Interface:
    return EmitInterfaceDecl(cast<InterfaceDecl>(topLevelDecl));
  case DeclKind::Fun:
    return EmitFunDecl(cast<FunDecl>(topLevelDecl));
  // case DeclKind::Enum:
  //   return EmitEnumDecl(cast<EnumDecl>(topLvelDecl));
  // case DeclKind::Struct:
  //   return EmitStructDecl(cast<StructDecl>(topLvelDecl));
  default:
    return;
  }
}

void IRModuleEmitter::EmitFunDecl(FunDecl *topLevelDecl) {}
void IRModuleEmitter::EmitStructDecl(StructDecl *topLevelDecl) {}

void IRModuleEmitter::EmitInterfaceDecl(InterfaceDecl *topLevelDecl) {}
void IRModuleEmitter::EmitEnumDecl(EnumDecl *topLevelDecl) {}
