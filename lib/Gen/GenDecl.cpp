#include "stone/Gen/IRCodeGen.h"
#include "stone/Gen/IRCodeGenModule.h"
#include "stone/Syntax/Decl.h"

using namespace stone;
using namespace stone::syn;

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
