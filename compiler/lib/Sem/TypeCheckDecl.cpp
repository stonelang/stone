#include "stone/Basic/OptionSet.h"
#include "stone/Sem/TypeChecker.h"
#include "stone/Syntax/SyntaxVisitor.h"

using stone::sem::TypeChecker;

/// \see checkTypeAccess
using CheckTypeAccessLevelCallback = void(AccessScope, const Type *);

struct AccessLevelCheckingFlags final {
  AccessLevelCheckingFlags() = delete;
  enum ID {
    None = 0 << 1,
    Infer = 1 << 1,
    WarnOnlyDiagnotic = 1 << 2,
  };
};
/// Options that control the parsing of declarations.
using AccessLevelCheckingOptions =
    stone::OptionSet<AccessLevelCheckingFlags::ID>;

class AccessLevelCheckingBase {
public:
  AccessLevelCheckingOptions flags;

public:
  void CheckTypeAccessLevelImpl(
      Type type, AccessScope contextAccessScope, const DeclContext *useDC,
      bool mayBeInferred,
      llvm::function_ref<CheckTypeAccessLevelCallback> diagnose);

  void CheckTypeAccessLevel(
      Type ty, const ValueDecl *context, bool mayBeInferred,
      llvm::function_ref<CheckTypeAccessLevelCallback> diagnose);

  void CheckTypeAccessLevel(
      const TypeLoc &TL, const ValueDecl *context, bool mayBeInferred,
      llvm::function_ref<CheckTypeAccessLevelCallback> diagnose) {

    return CheckTypeAccessLevel(TL.GetType(), context, mayBeInferred, diagnose);
  }
};

class AccessLevelChecking : public AccessLevelCheckingBase,
                            public DeclVisitor<AccessLevelChecking> {

  TypeChecker &checker;

public:
  AccessLevelChecking(TypeChecker &checker) : checker(checker) {}

public:
  void Visit(Decl *d) {
    // if (d->IsInvalid() || d->IsImplicit()) {
    //   return;
    // }
    DeclVisitor<AccessLevelChecking>::Visit(d);
  }

  void VisitValueDecl(ValueDecl *D) {}
  void VisitIfConfigDecl(IfConfigDecl *ifConfigDecl) {}

  void VisitFunDecl(FunDecl *fd) {}

  // #define UNREACHABLE(KIND, REASON) \
  // void visit##KIND##Decl(KIND##Decl *D) { \
  //   llvm_unreachable(REASON); \
  // }
  // #undef UNREACHABLE
};

class DeclChecking final : public DeclVisitor<DeclChecking> {
  TypeChecker &checker;
  SyntaxContext &sc;
  SyntaxFile *sf;

public:
  DeclChecking(TypeChecker &checker, SyntaxContext &sc, SyntaxFile *sf)
      : checker(checker), sc(sc), sf(sf) {}

public:
  void Visit(Decl *d) {
    //
    DeclVisitor<DeclChecking>::Visit(d);

    // checker.CheckTypes(d);
  }
  void VisitDecl(Decl *D) {}

public:
  void VisitFunDecl(FunDecl *funDecl) { checker.CheckAccessLevel(funDecl); }

  void VisitImportDecl(ImportDecl *importDecl) {}
  void VisitIfConfigDecl(IfConfigDecl *ifConfigDecl) {}
};

void TypeChecker::CheckDecl(Decl *d) {

  auto *sf = d->GetDeclContext()->GetParentSyntaxFile();
  DeclChecking(*this, d->GetSyntaxContext(), sf).Visit(d);
}

void TypeChecker::CheckAccessLevel(Decl *d) {

  // sem::CheckAccessLevel();

  if (llvm::isa<syn::ValueDecl>(d)) {
    AccessLevelChecking(*this).Visit(d);
  }
}
