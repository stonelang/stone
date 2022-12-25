#include "stone/Gen/IRCodeGen.h"
#include "stone/Gen/IRCodeGenModule.h"
#include "stone/Syntax/Decl.h"

using namespace stone;
using namespace stone::syn;

void IRCodeGenModule::EmitDecl(Decl *d) {
  switch (d->GetKind()) {
  case DeclKind::Interface:
    return EmitInterfaceDecl(cast<InterfaceDecl>(d));
  case DeclKind::Fun:
    return EmitFunDecl(cast<FunDecl>(d));
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
