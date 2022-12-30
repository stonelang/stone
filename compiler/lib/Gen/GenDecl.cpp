#include "stone/Gen/IRCodeGen.h"
#include "stone/Gen/IRCodeGenModule.h"
#include "stone/Syntax/Decl.h"
#include "stone/Syntax/Global.h"

using namespace stone;
using namespace stone::syn;

void IRCodeGenModule::EmitTopLevelDecl(Decl *d) {

  switch (d->GetKind()) {
  case DeclKind::Interface:
    return EmitInterfaceDecl(cast<InterfaceDecl>(d));
  case DeclKind::Fun:
    return EmitFunDecl(cast<FunDecl>(d));
  // case DeclKind::Enum:
  //   return EmitEnumDecl(cast<EnumDecl>(d));
  // case DeclKind::Struct:
  //   return EmitStructDecl(cast<StructDecl>(d));
  default:
    return;
  }
}

void IRCodeGenModule::EmitGlobalDecl(syn::GlobalDecl *gd) {}

void IRCodeGenModule::EmitSyntaxFile(syn::SyntaxFile &sf) {
  // Walk through the syntax file and call emit
  // Emit types and other global decls.
  for (auto d : sf.Decls) {
    EmitTopLevelDecl(d);
  }
}

void IRCodeGenModule::EmitFunDecl(FunDecl *funDecl) {}

void IRCodeGenModule::EmitStructDecl(StructDecl *structDecl) {}

void IRCodeGenModule::EmitInterfaceDecl(InterfaceDecl *interfaceDecl) {}

void IRCodeGenModule::EmitEnumDecl(EnumDecl *enumDecl) {}

void IRCodeGenModule::EmitConstructorDecl(ConstructorDecl *d) {}

void IRCodeGenModule::EmitDestructorDecl(DestructorDecl *d) {}
