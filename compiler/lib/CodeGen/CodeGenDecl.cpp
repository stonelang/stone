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

  for (auto *decl : sourceFile.GetTopLevelDecls()) {
    EmitGlobalDecl(decl);
  }
}

void CodeGenModule::EmitGlobalDecl(Decl *topLevelDecl) {

  assert(topLevelDecl->IsTopLevel() && "Not a top-level declaration");

  switch (topLevelDecl->GetKind()) {
  case DeclKind::Fun:
    return EmitFunDecl(topLevelDecl->GetAsFunDecl());
  case DeclKind::Struct:
    return EmitStructDecl(topLevelDecl->GetAsStructDecl());
  }

  llvm_unreachable("bad decl kind!");
}

void CodeGenModule::EmitFunDecl(FunDecl *funDecl) {

  assert(funDecl && "Null FundDecl");
  auto funType = GetFunctionType(funDecl);
}

void CodeGenModule::EmitInterfaceDecl(InterfaceDecl *d) {}

void CodeGenModule::EmitStructDecl(StructDecl *d) {}

void CodeGenModule::EmitEnumDecl(EnumDecl *d) {}

void CodeGenModule::EmitVarDecl(VarDecl *d) {}

void CodeGenModule::EmitAutoDecl(AutoDecl *d) {}

void CodeGenModule::EmitConstructorDecl(ConstructorDecl *d) {}

void CodeGenModule::EmitDestructorDecl(DestructorDecl *dd) {}
