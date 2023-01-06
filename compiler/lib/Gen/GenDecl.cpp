#include "stone/Gen/IRCodeGen.h"
#include "stone/Gen/IRCodeGenModule.h"
#include "stone/Syntax/Decl.h"
#include "stone/Syntax/Global.h"

using namespace stone;
using namespace stone::syn;

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

// void IRCodeGenModule::EmitGlobalDecl(syn::GlobalDecl *gd) {}

namespace {
class PrettySyntaxFileEmission : public llvm::PrettyStackTraceEntry {
  const syn::SyntaxFile &sf;

public:
  explicit PrettySyntaxFileEmission(const SyntaxFile &sf) : sf(sf) {}
  void print(raw_ostream &os) const override {
    // os << "While emitting IR for syntax file " << SF.GetFilename() << '\n';
  }
};
} // end anonymous namespace

void IRCodeGenModule::EmitSyntaxFile(syn::SyntaxFile &sf) {

  PrettySyntaxFileEmission stackEntry(sf);
  llvm::SaveAndRestore<syn::SyntaxFile *> setCurSyntaxFile(curSyntaxFile, &sf);
  // Walk through the syntax file and call emit
  // Emit types and other global decls.
  for (auto d : sf.Decls) {
    EmitGlobalDecl(d);
  }
}

void IRCodeGenModule::EmitFunDecl(FunDecl *funDecl,
                                  llvm::GlobalValue *globalValue) {

  auto llvmFunctionType = GetIRCodeGenTypeResolver().GetFunctionType(funDecl);

  // globalValue = llvm::cast<llvm::GlobalValue>(
  //     GetFunctionAddress(GD, Ty, /*forVTable=*/false,
  //                        /*dontDefer=*/true, ForDefinition));

  // IRCodeGenFunction(*this, nullptr).EmitFunction(funDecl);
}

void IRCodeGenModule::EmitStructDecl(StructDecl *structDecl) {}

void IRCodeGenModule::EmitInterfaceDecl(InterfaceDecl *interfaceDecl) {}

void IRCodeGenModule::EmitEnumDecl(EnumDecl *enumDecl) {}

void IRCodeGenModule::EmitConstructorDecl(ConstructorDecl *d) {}

void IRCodeGenModule::EmitDestructorDecl(DestructorDecl *d) {}
