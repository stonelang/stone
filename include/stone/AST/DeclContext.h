#ifndef STONE_AST_DECLCONTEXT_H
#define STONE_AST_DECLCONTEXT_H

#include <type_traits>

#include "stone/AST/DeclKind.h"
#include "stone/AST/Identifier.h"
#include "stone/AST/Type.h"
#include "stone/AST/TypeAlignment.h"
#include "stone/Basic/LLVM.h"
#include "stone/Basic/Memory.h"
#include "stone/Basic/STDAlias.h"
#include "stone/Basic/SrcLoc.h"

#include "llvm/ADT/PointerEmbeddedInt.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/PointerUnion.h"
#include "llvm/Support/raw_ostream.h"

namespace llvm {
class raw_ostream;
}

namespace stone {

class ASTContext;
class CanType;
class Decl;
class DeclContext;
class EnumDecl;
class ExtensionDecl;
class Expr;
class InterfaceDecl;
class SourceFile;
class Type;
class Module;
class ModuleFile;
class NominalTypeDecl;
class ValueDecl;
class StructDecl;

enum class DeclContextKind : UInt8 {
  None = 0,
  SpaceDecl,
  ModuleDecl,
  ModuleFile,
  FunctionDecl,
  EnumElementDecl,
  GenericTypeDecl,
  ClosureExpr,
  SerializedLocal,
  Initializer,
};

/// Used in diagnostic %selects.
struct FragileFunction final {

  enum Kind : unsigned {
    Transparent,
    Inlinable,
    AlwaysEmitIntoClient,
    DefaultArgument,
    PropertyInitializer,
    BackDeploy,
    None
  };

  Kind kind = None;
  bool allowUsableFromInline = false;

  friend bool operator==(FragileFunction lhs, FragileFunction rhs) {
    return (lhs.kind == rhs.kind &&
            lhs.allowUsableFromInline == rhs.allowUsableFromInline);
  }

  /// Casts to `unsigned` for diagnostic %selects.
  unsigned GetSelector() { return static_cast<unsigned>(kind); }
};

class alignas(1 << DeclContextAlignInBits) DeclContext
    : public MemoryAllocation<DeclContext> {

  DeclContext *parent = nullptr;
  DeclContextKind declContextKind = DeclContextKind::None;

  void SetParent(DeclContext *inputParent) { parent = inputParent; }

  // See stone/AST/Decl.h
  static DeclContext *CastDeclToDeclContext(const Decl *d);

protected:
  /// FirstDecl - The first declaration stored within this declaration
  /// context.
  mutable Decl *firstDecl = nullptr;

  /// LastDecl - The last declaration stored within this declaration
  /// context. FIXME: We could probably cache this value somewhere
  /// outside of the DeclContext, to reduce the size of DeclContext by
  /// another pointer.
  mutable Decl *lastDecl = nullptr;

  /// Build up a chain of declarations.
  ///
  /// \returns the first/last pair of declarations.
  static std::pair<Decl *, Decl *> BuildDeclChain(llvm::ArrayRef<Decl *> decls,
                                                  bool fieldsAlreadyLoaded);

public:
  DeclContext(DeclContextKind kind, DeclContext *parent = nullptr);

public:
  DeclContextKind GetDeclContextKind() const;

  bool Is(DeclContextKind k) const { return declContextKind == k; }
  bool IsNot(DeclContextKind k) const { return declContextKind != k; }
  bool IsAny(DeclContextKind K1) const { return Is(K1); }

  template <typename... T>
  bool IsAny(DeclContextKind K1, DeclContextKind K2, T... K) const {
    if (Is(K1)) {
      return true;
    }
    return IsAny(K2, K...);
  }
  // Predicates to check to see if the token is not the same as any of a list.
  template <typename... T> bool IsNot(DeclContextKind K1, T... K) const {
    return !IsAny(K1, K...);
  }
  bool IsDecl() {
    return IsAny(DeclContextKind::SpaceDecl, DeclContextKind::ModuleDecl,
                 DeclContextKind::FunctionDecl,
                 DeclContextKind::GenericTypeDecl,
                 DeclContextKind::EnumElementDecl);
  }
  Decl *ToDecl() {
    if (IsDecl()) {
      return reinterpret_cast<Decl *>(this + 1);
    }
    return nullptr;
  }
  const Decl *ToDecl() const {
    return const_cast<DeclContext *>(this)->ToDecl();
  }

  // Return the ASTContext for a specified DeclContext by
  /// walking up to the enclosing module and returning its ASTContext.
  ASTContext &GetASTContext() const;

  /// Returns the semantic parent of this context.  A context has a
  /// parent if and only if it is not a module context.
  DeclContext *GetParent() const { return parent; }
  bool HasParent() { return parent != nullptr; }

  bool IsModuleContext() const;
  bool IsModuleFileContext() const;

  ModuleDecl *GetParentModule() const;
  SourceFile *GetParentSourceFile() const;

  bool IsTypeContext() const;

  /// If this DeclContext is an enum, or an extension on an enum, return the
  /// EnumDecl, otherwise return null.
  // EnumDecl *GetThisEnumDecl() const;

  /// If this DeclContext is a struct, or an extension on a struct, return the
  /// StructDecl, otherwise return null.
  // StructDecl *GetThisStructDecl() const;

  /// If this DeclContext is a protocol, or an extension on a
  /// protocol, return the ProtocolDecl, otherwise return null.
  // InterfaceDecl *GetThisInterfaceDecl() const;
};
} // namespace stone
#endif
