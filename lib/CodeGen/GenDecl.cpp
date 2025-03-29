#include "stone/AST/Decl.h"
#include "stone/AST/Global.h"
#include "stone/AST/Module.h"
#include "stone/CodeGen/CodeGenFunction.h"
#include "stone/CodeGen/CodeGenModule.h"

#include "llvm/Support/PrettyStackTrace.h"

using namespace stone;

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

void CodeGenModule::EmitSourceFile(SourceFile &sourceFile) {

  for (auto *GD : sourceFile.GetTopLevelDecls()) {
    EmitGlobalDecl(GD);
  }
}

void CodeGenModule::EmitGlobalDecl(Decl *GD) {

  assert(GD->IsTopLevel() && "Not a top-level declaration");
  EmitDecl(GD);
}

void CodeGenModule::EmitDecl(Decl *D) {

  switch (D->GetKind()) {
  case DeclKind::Fun: {
    break; // not here
    // return EmitFunDecl(D->GetAsFunDecl());
  }
  case DeclKind::Struct:
    return EmitStructDecl(llvm::cast<StructDecl>(D));
  case DeclKind::Class:
    return EmitClassDecl(llvm::cast<ClassDecl>(D));
  case DeclKind::Interface:
    return EmitInterfaceDecl(llvm::cast<InterfaceDecl>(D));
  }
  llvm_unreachable("bad decl kind!");
}

void CodeGenModule::EmitEnumDecl(EnumDecl *ED) {}

void CodeGenModule::EmitVarDecl(VarDecl *VD) {}

void CodeGenModule::EmitAutoDecl(AutoDecl *AD) {}

void CodeGenModule::EmitConstructorDecl(ConstructorDecl *CD) {}

void CodeGenModule::EmitDestructorDecl(DestructorDecl *DD) {}
