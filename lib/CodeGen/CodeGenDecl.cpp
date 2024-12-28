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

  for (auto *topLevelDecl : sourceFile.GetTopLevelDecls()) {
    EmitTopLevelDecl(topLevelDecl);
  }
}

void CodeGenModule::EmitTopLevelDecl(Decl *topLevelDecl) {

  assert(topLevelDecl->IsTopLevel() && "Not a top-level declaration");
  EmitDecl(topLevelDecl);
}

void CodeGenModule::EmitDecl(Decl *D) {

  switch (D->GetKind()) {
  case DeclKind::Fun:
    return EmitFunDecl(D->GetAsFunDecl());
  case DeclKind::Struct:
    return EmitStructDecl(D->GetAsStructDecl());
  case DeclKind::Class:
    return EmitClassDecl(D->GetAsClassDecl());
  case DeclKind::Interface:
    return EmitInterfaceDecl(D->GetAsInterfaceDecl());
  }
  llvm_unreachable("bad decl kind!");
}

void CodeGenModule::EmitFunDecl(FunDecl *funDecl) {

  assert(funDecl && "Null FundDecl");
  auto funType = GetFunctionType(funDecl);

  
}

void CodeGenModule::EmitInterfaceDecl(InterfaceDecl *d) {}

void CodeGenModule::EmitStructDecl(StructDecl *d) {}

void CodeGenModule::EmitClassDecl(ClassDecl *d) {}

void CodeGenModule::EmitEnumDecl(EnumDecl *d) {}

void CodeGenModule::EmitVarDecl(VarDecl *d) {}

void CodeGenModule::EmitAutoDecl(AutoDecl *d) {}

void CodeGenModule::EmitConstructorDecl(ConstructorDecl *d) {}

void CodeGenModule::EmitDestructorDecl(DestructorDecl *dd) {}
