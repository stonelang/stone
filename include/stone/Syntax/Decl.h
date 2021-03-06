#ifndef STONE_SYNTAX_DECL_H
#define STONE_SYNTAX_DECL_H

#include "stone/Basic/AddressSpace.h"
#include "stone/Basic/DiagnosticArgument.h"
#include "stone/Basic/LLVM.h"
#include "stone/Basic/SrcLoc.h"
#include "stone/Syntax/DeclContext.h"
#include "stone/Syntax/DeclKind.h"
#include "stone/Syntax/DeclName.h"
#include "stone/Syntax/Identifier.h"
#include "stone/Syntax/Specifier.h"
#include "stone/Syntax/SyntaxNode.h"
#include "stone/Syntax/Type.h"
#include "stone/Syntax/TypeAlignment.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/PointerUnion.h"
#include "llvm/ADT/iterator.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/PrettyStackTrace.h"
#include "llvm/Support/VersionTuple.h"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iterator>
#include <string>
#include <type_traits>
#include <utility>

namespace stone {
namespace syn {

class Decl;
class Stmt;
class Module;
class BraceStmt;
class DeclContext;
class TreeContext;
class ValueDecl;
class VarDecl;
class SyntaxNode;
class Type;
class Expr;
class ConstructorDecl;
class DestructorDecl;
class DiagnosticEngine;
class TypeAliasDecl;
class ArchetypeKind;
class SyntaxPrinter;

class DeclStats final : public Stats {
  const Decl &declaration;

public:
  DeclStats(const Decl &declaration, Basic &basic)
      : Stats("ast-declaration stats:", basic), declaration(declaration) {}
  void Print() override;
};

class alignas(1 << DeclAlignInBits) Decl : public SyntaxNode {

  friend DeclStats;

  DeclKind kind;
  /// The location of the decl
  SrcLoc loc;
  DeclContext *dc;

public:
  // Make vanilla new/delete illegal for Decls.
  void *operator new(size_t bytes) = delete;
  void operator delete(void *data) = delete;

  // Only allow allocation of Decls using the allocator in ASTContext
  // or by doing a placement new.
  void *operator new(size_t bytes, const TreeContext &tc,
                     unsigned alignment = alignof(Decl));

  void *operator new(size_t bytes, void *mem) {
    assert(mem);
    return mem;
  }

  // TODO: UB
  // void *operator new(std::size_t size, const TreeContext &ctx,
  //                    DeclContext *parent, std::size_t extra = 0);

public:
  Decl() = delete;
  Decl(const Decl &) = delete;
  Decl(Decl &&) = delete;
  Decl &operator=(const Decl &) = delete;
  Decl &operator=(Decl &&) = delete;

public:
  friend class DeclContext;

  // struct MultipleDeclContext final {
  //   DeclContext *semaDeclContext;
  //   DeclContext *lexicalDeclContext;
  // };

  llvm::PointerUnion<DeclContext *, TreeContext *> context;

  // llvm::PointerUnion<DeclContext *, MultipleDeclContext *> context;

  // bool IsInSemaDeclContext() const { return context.is<DeclContext *>(); }
  // bool IsOutOfSemaDeclContext() const {
  //   return context.is<MultipleDeclContext *>();
  // }

  // MultipleDeclContext *GetMultipleDeclContext() const {
  //   return context.get<MultipleDeclContext *>();
  // }

  // DeclContext *GetSemaDeclContext() const {
  //   return context.get<DeclContext *>();
  // }

  /// DeclKind - This indicates which class this is.
  // unsigned declType : 7;

  /// InvalidDecl - This indicates a semantic error occurred.
  // unsigned invalidDecl :  1;

  /// HasAttrs - This indicates whether the decl has attributes or not.
  // unsigned hasAttrs : 1;

  /// Implicit - Whether this declaration was implicitly generated by
  /// the implementation rather than explicitly written by the user.
  // unsigned implicit : 1;

  /// Whether this declaration was "used", meaning that a definition is
  /// required.
  // unsigned used : 1;

public:
  DeclKind GetKind() const { return kind; }
  SrcLoc GetLoc() const { return loc; }

  TreeContext &GetTreeContext() const {
    auto dc = context.dyn_cast<DeclContext *>();
    if (dc) {
      return dc->GetTreeContext();
    }
    return *context.get<TreeContext *>();
  }

protected:
  Decl(DeclKind kind, SrcLoc loc,
       llvm::PointerUnion<DeclContext *, TreeContext *> context)
      : kind(kind), loc(loc), context(context) {}
};

class NamedDecl : public Decl {
  /// The name of this declaration, which is typically a normal
  /// identifier but may also be a special ty of name (C++
  /// constructor, etc.)
  DeclName name;
  SrcLoc nameLoc;

protected:
  NamedDecl(DeclKind kind, SrcLoc loc, DeclContext *dc)
      : Decl(kind, loc, dc), name(nullptr) {}

public:
  /// Get the identifier that names this declaration, if there is one.
  ///
  /// This will return NULL if this declaration has no name (e.g., for
  /// an unnamed class) or if the name is a special name such ast a C++
  /// constructor.
  Identifier *GetIdentifier() const { return name.GetAsIdentifier(); }

  /// Get the name of identifier for this declaration as a StringRef.
  ///
  /// This requires that the declaration have a name and that it be a simple
  /// identifier.
  llvm::StringRef GetNameText() const {
    // TODO: assert(name.IsIdentifier() && "Name is not a simple identifier");
    return GetIdentifier() ? GetIdentifier()->GetName() : "";
  }
  void SetDeclName(DeclName name) { this->name = name; }
  DeclName GetDeclName() { return name; }

  void SetDeclNameLoc(SrcLoc nameLoc) { this->nameLoc = nameLoc; }
  SrcLoc GetDeclNameLoc() { return nameLoc; }
};

class AnyDecl : public NamedDecl {
public:
};

class TypeDecl : public NamedDecl /*TODO: AnyDecl*/ {

  friend class TreeContext;
  /// This indicates the Type object that represents
  /// this TypeDecl.  It is a cache maintained by
  /// ASTContext::getTypedefType, ASTContext::getTagDeclKind, and
  /// ASTContext::getTemplateTypeParmType, and TemplateTypeParmDecl.

  // mutable const Type *typeForDecl = nullptr;

  /// The start of the source range for this declaration.
  SrcLoc startLoc;

protected:
  TypeDecl(DeclKind kind, SrcLoc loc, DeclContext *dc)
      : NamedDecl(kind, loc, dc) {}

public:
  void SetIdentifier(Identifier *identifier) { SetDeclName(identifier); }
};

// TODO: May use this instead of using NamedDecl
class ValueDecl : public NamedDecl {
  QualType qTy;

public:
  ValueDecl(DeclKind ty, SrcLoc loc, DeclContext *dc)
      : NamedDecl(ty, loc, dc) {}

public:
  void SetQualType(QualType qTy) { this->qTy = qTy; }
  QualType GetQualType() { return qTy; }
};

// class LabelDecl : public NamedDecl {
// public:
// };

// class SpaceDecl : public NamedDecl {
// public:
//   SpaceDecl(DeclContext *dc, SrcLoc loc, DeclName name)
//       : NamedDecl(DeclKind::Space, dc, loc, name) {}
// };

class DeclaratorDecl : public ValueDecl {
public:
  DeclaratorDecl(DeclKind kind, SrcLoc loc, DeclContext *dc)
      : ValueDecl(kind, loc, dc) {}
};

class AccessControl {
  // This also belongs to struct
  AccessLevel level;

public:
  void SetAccessLevel(AccessLevel level) { this->level = level; }
  AccessLevel GetAccessLevel() { return level; }
};

// This is really your function prototye
class FunctionDecl
    : public DeclaratorDecl,
      public DeclContext,
      public AccessControl /*, syn::Redeclarable<FunctionDecl> */ {

  StorageKind storageKind;

public:
  FunctionDecl(DeclKind kind, SrcLoc loc, TreeContext &tc, DeclContext *dc);

public:
  /// BraceStmt
  Stmt *GetBody();
  // void SetBody(Stmt body) {}

  void SetStorageKind(StorageKind storageTy) {
    this->storageKind = storageKind;
  }
  StorageKind GetStorageKind() { return storageKind; }

  // void SetReturnType(TypeDecl* tyDecl);

public:
};

/// Standalone function: fun F0() -> void {}
class FunDecl : public FunctionDecl {

  // TODO: You should aonly pass TreeContext and DeclContext
public:
  FunDecl(SrcLoc loc, TreeContext &tc, DeclContext *dc)
      : FunctionDecl(DeclKind::Fun, loc, tc, dc) {}

public:
  bool IsMain() const;

  /// True if the function is a defer body.
  bool IsDeferBody() const;

  bool IsStatic() const;

public:
  // void SetReturnType(TypeDecl* returnTy);

  // SrcLoc GetStaticLoc() const { return staticLoc; }
  // SrcLoc GetFunLoc() const { return funcLoc; }

  static bool classof(const Decl *d) { return d->GetKind() == DeclKind::Fun; }
  static bool classof(const FunctionDecl *d) {
    return classof(static_cast<const Decl *>(d));
  }
  static bool classof(const DeclContext *dc) {
    if (auto d = dc->GetAsDecl())
      return classof(d);
    return false;
  }
};

// Member functions: fun Particle::Fire() -> bool ...
// class MethodDecl : public FunctionDecl {
// public:
//   // MethodDecl(TreeContext &tc, DeclContext *dc, SrcLoc funLoc,
//   //            const DeclName &dn, SrcLoc dnLoc, StorageType st)
//   //     : FunctionDecl(DeclKind::Fun, tc, dc, dn, dnLoc, st) {}

// public:
//   bool IsStatic() const;
//   bool IsInstance() const { return !IsStatic(); }
// };

class NominalTypeDecl : public TypeDecl,
                        public DeclContext,
                        public AccessControl {

public:
};

class StructDecl final : public NominalTypeDecl {
public:
};

class InterfaceDecl final : public NominalTypeDecl {
public:
};

class EnumDecl final : public NominalTypeDecl {
public:
};

class BlockDecl : public Decl, public DeclContext {};

class ConstructorInitializer final {
public:
};

// class ConstructorDecl : public MethodDecl {
// public:
// };

// class DestructorDecl : public MethodDecl {
// public:
// };

// TODO: Maybe Enum, Struct, ... can be replaced with Member
enum class UseDeclKind : uint8_t {
  /// use STD.IO;
  Module = 0,

  //// use STD.Time.Month;
  Enum,

  /// use STD.IO.OutputStream;
  Struct,

  /// use STD.IO.Stream;
  Interface,

  /// fun Main() -> int { use STD.Math.Min;  auto min = Min<AnyType>(first,
  /// second); }
  Fun,

  // use Stream = STD.IO.Stream;
  // use Min = STD.Main.Min(first, second);  // TODO: Think
  Alias,
};

class UseDeclBase : public NamedDecl {
public:
};
class UseDecl : public UseDeclBase {
  SrcLoc useLoc;

public:
  // Module *mod = nullptr;
};

} // namespace syn
} // namespace stone
#endif
