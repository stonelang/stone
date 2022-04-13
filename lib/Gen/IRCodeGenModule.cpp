#include "stone/Gen/IRCodeGenModule.h"
#include "stone/Gen/IRCodeGen.h"
#include "stone/Syntax/Decl.h"

using namespace stone;
using namespace stone::syn;

// llvm::Module *IRModule::GetLLVMModule() { return llvmModule.get(); }
// llvm::Module *IRModule::ReleaseLLVMModule() { return llvmModule.release(); }

IRCodeGenModule::IRCodeGenModule(IRCodeGen &irCodeGen) : irCodeGen(irCodeGen) {}

void IRCodeGenModule::EmitSyntaxFile(const syn::SyntaxFile &sf) {
  // Walk through the syntax file and call emit
  // Emit types and other global decls.
  for (auto &topLevelDecl : sf.topLevelDecls) {
    EmitTopLevelDecl(&topLevelDecl);
  }
}
void IRCodeGenModule::EmitTopLevelDecl(Decl *topLevelDecl) {
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

void IRCodeGenModule::EmitFunDecl(FunDecl *topLevelDecl) {}
void IRCodeGenModule::EmitStructDecl(StructDecl *topLevelDecl) {}

void IRCodeGenModule::EmitInterfaceDecl(InterfaceDecl *topLevelDecl) {}
void IRCodeGenModule::EmitEnumDecl(EnumDecl *topLevelDecl) {}
