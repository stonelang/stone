#include "stone/Syntax/Decl.h"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <string>
#include <tuple>
#include <utility>

#include "stone/Basic/LLVM.h"
#include "stone/Basic/LangOptions.h"
#include "stone/Basic/SrcLoc.h"
#include "stone/Syntax/Identifier.h"
#include "stone/Syntax/Module.h"
#include "stone/Syntax/Stmt.h"
#include "stone/Syntax/Template.h" //DeclTemplate
#include "stone/Syntax/Types.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/Support/VersionTuple.h"
#include "llvm/Support/raw_ostream.h"

using namespace stone;
using namespace stone::syn;

// // Only allow allocation of Decls using the allocator in ASTContext.
// void *syn::Decl::operator new(std::size_t bytes, const SyntaxContext &tc,
//                               unsigned alignment) {
//   return tc.Allocate(bytes, alignment);
// }

// // Only allow allocation of Modules using the allocator in ASTContext.
// void *syn::Module::operator new(std::size_t bytes, const SyntaxContext &tc,
//                                 unsigned alignment) {
//   return tc.Allocate(bytes, alignment);
// }

bool ValueDecl::IsInstanceMember() const {
  DeclContext *dc = GetDeclContext();

  if (!dc->IsTypeContext()) {
    return false;
  }

  switch (GetKind()) {
    // case DeclKind::Import:
    // case DeclKind::Extension:
    // case DeclKind::PatternBinding:
    // case DeclKind::EnumCase:
    // case DeclKind::TopLevelCode:
    // case DeclKind::InfixOperator:
    // case DeclKind::PrefixOperator:
    // case DeclKind::PostfixOperator:
    // case DeclKind::IfConfig:
    // case DeclKind::PoundDiagnostic:
    // case DeclKind::PrecedenceGroup:
    // case DeclKind::MissingMember:
    //   llvm_unreachable("Not a ValueDecl");

  case DeclKind::Struct:
  case DeclKind::Enum:
  case DeclKind::Interface:
    // case DeclKind::Alias:
    //  Types are not instance members.
    return false;

    // case DeclKind::Constructor:
    //   // Constructors are not instance members.
    //   return false;

    // case DeclKind::Destructor:
    //   // Destructors are technically instance members, although they
    //   // can't actually be referenced as such.
    //   return true;

  case DeclKind::Fun:
    // Non-static methods are instance members.
    // return !cast<FunDecl>(this)->IsStatic(); // TODO:
    return true;

    // case DeclKind::Param:
    //   // enum elements and function parameters are not instance members.
    //   return false;

    // case DeclKind::Subscript:
    // case DeclKind::Var:
    //   // Non-static variables and subscripts are instance members.
    //   return !cast<AbstractStorageDecl>(this)->isStatic();

    // case DeclKind::Module:
    //   // Modules are never instance members.
    //   return false;
  }
  llvm_unreachable("bad DeclKind");
}

template <std::size_t len>
static bool IsMainImpl(const NamedDecl *named, const char (&str)[len]) {
  const Identifier *identifier = named->GetIdentifier();
  return identifier && identifier->isStr(str);
}

// Keeping this very simple for now
bool FunDecl::IsMain() const {
  return (!IsInstanceMember() && IsMainImpl(this, "Main"));
}

/// True if the function is a defer body.
bool FunDecl::IsDeferBody() const {}

bool FunDecl::IsStatic() const { return false; }

// TODO: Remove
bool FunDecl::IsMember() const { return false; }

// TODO: Think about
bool FunDecl::IsForward() const { return false; }

bool FunDecl::HasReturn() const { return false; }

void FunDecl::SetFunLoc(SrcLoc loc) { funLoc = loc; }

void DeclStats::Print(ColorfulStream &stream) {}
