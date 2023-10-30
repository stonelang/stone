#include "stone/Sem/TypeCheckAccess.h"
#include "stone/Sem/TypeChecker.h"
#include "stone/AST/ASTVisitor.h"

using namespace stone;
using namespace stone::syn;
using namespace stone::sem;

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

protected:
  TypeChecker &checker;

public:
  AccessLevelCheckingBase(TypeChecker &checker) : checker(checker) {}

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

void AccessLevelCheckingBase::CheckTypeAccessLevelImpl(
    Type type, AccessScope contextAccessScope, const DeclContext *useDC,
    bool mayBeInferred,
    llvm::function_ref<CheckTypeAccessLevelCallback> diagnose) {}

void AccessLevelCheckingBase::CheckTypeAccessLevel(
    Type ty, const ValueDecl *context, bool mayBeInferred,
    llvm::function_ref<CheckTypeAccessLevelCallback> diagnose) {}

class AccessLevelChecking : public AccessLevelCheckingBase,
                            public DeclVisitor<AccessLevelChecking> {

public:
  AccessLevelChecking(TypeChecker &checker)
      : AccessLevelCheckingBase(checker) {}

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
};

void TypeChecker::CheckAccessLevel(Decl *d) {

  // if (llvm::isa<syn::ValueDecl>(d)) {
  //   sem::CheckAccessLevel(d)
  // }
  AccessLevelChecking(*this).Visit(d);
}
