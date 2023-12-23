#include "stone/Gen/IRGenFunction.h"
#include "stone/Gen/IRGenModule.h"
#include "stone/Syntax/Decl.h"
#include "stone/Syntax/Global.h"

using namespace stone;

void IRGenModule::EmitGlobalDecl(Decl *d) {

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
// void IRGenModule::EmitGlobalDecl(GlobalDecl *gd) {}

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

void IRGenModule::EmitSourceFile(SourceFile &sourceFile) {

  PrettySourceFileEmission stackEntry(sourceFile);
  //  TODO: llvm::SaveAndRestore<SourceFile *> setCurSourceFile(curSourceFile,
  //  &sf);
  // Walk through the syntax file and call emit
  // Emit types and other global decls.
  for (auto topLevelDecl : sourceFile.topLevelDecls) {
    EmitGlobalDecl(topLevelDecl);
  }
}

void IRGenModule::EmitInterfaceDecl(InterfaceDecl *interfaceDecl) {}

void IRGenModule::EmitEnumDecl(EnumDecl *enumDecl) {}

void IRGenModule::EmitConstructorDecl(ConstructorDecl *d) {}

void IRGenModule::EmitDestructorDecl(DestructorDecl *d) {}
