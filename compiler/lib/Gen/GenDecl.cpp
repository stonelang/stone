#include "stone/Gen/IRCodeGenFunction.h"
#include "stone/Gen/IRCodeGenModule.h"
#include "stone/Syntax/Decl.h"
#include "stone/Syntax/Global.h"

using namespace stone;

void IRCodeGenModule::EmitGlobalDecl(Decl *d) {

  assert(d->IsTopLevel() && "Not a top-level declaration");

  switch (d->GetKind()) {
  case DeclKind::Interface:
    return EmitInterfaceDecl(llvm::cast<InterfaceDecl>(d));
  case DeclKind::Fun:
    return EmitFunDecl(llvm::cast<FunDecl>(d));
  // case DeclKind::Enum:
  //   return EmitEnumDecl(cast<EnumDecl>(d));
  // case DeclKind::Struct:
  //   return EmitStructDecl(cast<StructDecl>(d));
  default:
    return;
  }
}
// void IRCodeGenModule::EmitGlobalDecl(GlobalDecl *gd) {}

namespace {
class PrettySourceFileEmission : public llvm::PrettyStackTraceEntry {
  const SourceFile &sf;

public:
  explicit PrettySourceFileEmission(const SourceFile &sf) : sf(sf) {}
  void print(raw_ostream &os) const override {
    // os << "While emitting IR for syntax file " << SF.GetFilename() << '\n';
  }
};
} // end anonymous namespace

void IRCodeGenModule::EmitSourceFile(SourceFile &sf) {

  PrettySourceFileEmission stackEntry(sf);
  //  TODO: llvm::SaveAndRestore<SourceFile *> setCurSourceFile(curSourceFile,
  //  &sf);
  // Walk through the syntax file and call emit
  // Emit types and other global decls.
  for (auto d : sf.Decls) {
    EmitGlobalDecl(d);
  }
}

void IRCodeGenModule::EmitInterfaceDecl(InterfaceDecl *interfaceDecl) {}

void IRCodeGenModule::EmitEnumDecl(EnumDecl *enumDecl) {}

void IRCodeGenModule::EmitConstructorDecl(ConstructorDecl *d) {}

void IRCodeGenModule::EmitDestructorDecl(DestructorDecl *d) {}
