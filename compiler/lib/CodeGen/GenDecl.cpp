#include "stone/CodeGen/CodeGen.h"
#include "stone/CodeGen/CodeGenFunction.h"
#include "stone/CodeGen/CodeGenModule.h"
#include "stone/AST/Decl.h"
#include "stone/AST/Global.h"

using namespace stone;
using namespace stone::ast;

void CodeGenModule::EmitGlobalDecl(Decl *d) {

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

// void CodeGenModule::EmitGlobalDecl(ast::GlobalDecl *gd) {}

namespace {
class PrettyASTFileEmission : public llvm::PrettyStackTraceEntry {
  const ast::ASTFile &sf;

public:
  explicit PrettyASTFileEmission(const ASTFile &sf) : sf(sf) {}
  void print(raw_ostream &os) const override {
    // os << "While emitting IR for asttax file " << SF.GetFilename() << '\n';
  }
};
} // end anonymous namespace

void CodeGenModule::EmitASTFile(ast::ASTFile &sf) {

  PrettyASTFileEmission stackEntry(sf);
  llvm::SaveAndRestore<ast::ASTFile *> setCurASTFile(curASTFile, &sf);
  // Walk through the asttax file and call emit
  // Emit types and other global decls.
  for (auto d : sf.Decls) {
    EmitGlobalDecl(d);
  }
}

void CodeGenModule::EmitFunDecl(FunDecl *funDecl,
                                  llvm::GlobalValue *globalValue) {

  assert(funDecl && "Null FundDecl");

  auto funDeclType = GetCodeGenTypeResolver().GetFunctionType(funDecl);

  EmitFunctionOptions emitFunctionOpts;
  emitFunctionOpts |= EmitFunctionFlags::IsForDefinition;
  emitFunctionOpts |= EmitFunctionFlags::DontDefer;

  if (!globalValue) {
    globalValue = llvm::cast<llvm::GlobalValue>(
        GetFunctionAddress(funDecl, funDeclType, emitFunctionOpts));
  }

  auto *llvmFunction = cast<llvm::Function>(globalValue);

  SetFunctionLinkage(funDecl, llvmFunction);
  CodeGenFunction(*this, llvmFunction).EmitFunction(funDecl);
}

void CodeGenModule::EmitStructDecl(StructDecl *structDecl) {}

void CodeGenModule::EmitInterfaceDecl(InterfaceDecl *interfaceDecl) {}

void CodeGenModule::EmitEnumDecl(EnumDecl *enumDecl) {}

void CodeGenModule::EmitConstructorDecl(ConstructorDecl *d) {}

void CodeGenModule::EmitDestructorDecl(DestructorDecl *d) {}
