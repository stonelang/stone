#ifndef STONE_AST_DECL_H
#define STONE_AST_DECL_H

#include "stone/AST/ASTAllocation.h"
#include "stone/AST/Attribute.h"
#include "stone/AST/DeclContext.h"
#include "stone/AST/DeclKind.h"
#include "stone/AST/DeclName.h"
#include "stone/AST/Generics.h"
#include "stone/AST/Identifier.h"
#include "stone/AST/IfConfig.h"
#include "stone/AST/Import.h"
#include "stone/AST/InlineBitfield.h"
#include "stone/AST/Modifier.h"
#include "stone/AST/Storage.h"
#include "stone/AST/TypeAlignment.h"
#include "stone/AST/Visibility.h"
#include "stone/Basic/AddressSpace.h"
#include "stone/Basic/LLVM.h"
#include "stone/Basic/Memory.h"
#include "stone/Basic/SrcLoc.h"

// #include "stone/AST/Redeclarable.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/PointerUnion.h"
#include "llvm/ADT/iterator.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/PrettyStackTrace.h"
#include "llvm/Support/TrailingObjects.h"
#include "llvm/Support/VersionTuple.h"

#include <algorithm>
#include <iterator>
#include <string>
#include <type_traits>
#include <utility>

namespace stone {
class Decl;
class Stmt;
class FunDecl;
class StructDecl;
class ClassDecl;
class InterfaceDecl;
class ModuleDecl;
class BraceStmt;
class DeclContext;
class ASTContext;
class ValueDecl;
class VarDecl;
struct ASTNode;
class Type;
class Expr;
class ConstructorDecl;
class DestructorDecl;
class AliasDecl;
class ArchetypeKind;
class ASTPrinter;
class ASTWalker;
class GenericParamList;
class TrailingWhereClause;
class DiagnosticEngine;

using UnifiedContext = llvm::PointerUnion<DeclContext *, ASTContext *>;

enum : unsigned {
  NumDeclKindBits = stone::CountBitsUsed(static_cast<unsigned>(DeclKind::Count))
};

// Kinds of pointer types.
enum PointerTypeKind : unsigned {
  Raw,
};

// Introduces a name and associates it with a type such as:
// int x where x is the declaration, int is the type.
class alignas(1 << DeclAlignInBits) Decl : public ASTAllocation<Decl> {

protected:
  DeclKind kind;
  SrcLoc kindLoc;
  DeclName name;
  UnifiedContext context;

protected:
  union {
    uint64_t OpaqueBits;
    STONE_INLINE_BITFIELD_BASE(
        Decl, BitMax(NumDeclKindBits, 8) + 1 + 1 + 1 + 1 + 1 + 1, Kind
        : BitMax(NumDeclKindBits, 8),

          /// Whether this declaration is invalid.
          IsValid : 1,

          /// Whether this declaration was implicitly created, e.g.,
          /// an implicit constructor in a struct.
          IsImplicit : 1,

          /// Whether this declaration was mapped directly from a Clang AST.
          ///
          /// Use GetClangNode() to retrieve the corresponding Clang AST.
          IsFromClang : 1,

          /// Whether this declaration was added to the surrounding
          /// DeclContext of an active #if config clause.
          IsEscapedFromIfConfig : 1,

          /// Whether this declaration is syntactically scoped inside of
          /// a local context, but should behave like a top-level
          /// declaration for name lookup purposes. This is used by
          /// lldb.
          IsHoisted : 1,

          /// Wether this is a top level decl
          IsTopLevel : 1);

    STONE_INLINE_BITFIELD(
        ValueDecl, Decl, 1 + 1 + 1 + 1,

        IsInLookupTable : 1,

        /// Whether we have already checked whether this declaration is a
        /// redeclaration.
        CheckedRedeclaration : 1,

        /// Whether the decl can be accessed by swift users; for instance,
        /// a.storage for lazy var a is a decl that cannot be accessed.
        IsUserVisible : 1,

        /// Whether this member was synthesized as part of a derived
        /// protocol conformance.
        IsSynthesized : 1);

    STONE_INLINE_BITFIELD(StorageDecl, ValueDecl, 1,
                          /// Whether this property is a type property
                          /// (currently unfortunately called 'static').
                          IsStatic : 1);

    STONE_INLINE_BITFIELD(VarDecl, StorageDecl, 1 + 1, IsBuiltin : 1,
                          /// Whether this is a lazily top-level global variable
                          /// from the main file.
                          IsTopLevelGlobal : 1);

    STONE_INLINE_BITFIELD(
        FunctionDecl, ValueDecl, 1,
        /// \see AbstractFunctionDecl::BodyKind
        // BodyKind : 3,

        /// \see AbstractFunctionDecl::SILSynthesizeKind
        // SILSynthesizeKind : 2,

        /// Import as member status.
        // IAMStatus : 8,

        /// Whether the function has an implicit 'self' parameter.
        // HasImplicitSelfDecl : 1,

        /// Whether we are overridden later.
        // Overridden : 1,

        IsMember : 1

        /// Whether this member's body consists of a single expression.
        // HasSingleExpressionBody : 1,

        /// Whether peeking into this function detected nested type
        /// declarations. This is set when skipping over the decl at parsing.
        // HasNestedTypeDeclarations : 1
    );

    STONE_INLINE_BITFIELD(
        FunDecl, FunctionDecl, 1 + 1,
        /// Whether we've computed the 'static' flag yet.
        // IsStaticComputed : 1,

        /// Whether this function is a 'static' method.
        IsStatic : 1,

        IsMain : 1

        /// Whether 'static' or 'class' was used.
        // StaticSpelling : 2,

        /// Whether we are statically dispatched even if overridable
        // ForcedStaticDispatch : 1,

        /// Whether we've computed the 'self' access kind yet.
        // ThisfAccessComputed : 1,

        /// Backing bits for 'self' access kind.
        // ThisAccess : 2,

        /// Whether this is a top-level function which should be treated
        /// as if it were in local context for the purposes of capture
        /// analysis.
        // HasTopLevelLocalContextCaptures : 1
    );

    STONE_INLINE_BITFIELD_EMPTY(TypeDecl, ValueDecl);

    STONE_INLINE_BITFIELD(
        ModuleDecl, TypeDecl, 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1,
        /// If the module is compiled as static library.
        IsStaticLibrary : 1,

        /// If the module was or is being compiled with `-enable-testing`.
        IsTestingEnabled : 1,

        /// If the module failed to load
        FailedToLoad : 1,

        /// Whether the module is resilient.
        ///
        /// \sa ResilienceStrategy
        RawResilienceStrategy : 1,

        /// Whether all imports have been resolved. Used to detect circular
        /// imports.
        HasResolvedImports : 1,

        /// If the module was or is being compiled with
        /// `-enable-private-imports`.
        PrivateImportsEnabled : 1,

        /// If the module is compiled with `-enable-implicit-dynamic`.
        ImplicitDynamicEnabled : 1,

        /// Whether the module is a system module.
        IsSystemModule : 1,

        /// Whether the module was imported from Clang (or, someday, maybe
        /// another language).
        IsNonStoneModule : 1,

        /// Whether this module is the main module.
        IsMainModule : 1,

        /// Whether this module has incremental dependency information
        /// available.
        HasIncrementalInfo : 1,

        /// Whether this module was built with
        /// -experimental-hermetic-seal-at-link.
        HasHermeticSealAtLink : 1,

        /// Whether this module has been compiled with comprehensive checking
        /// for concurrency, e.g., Sendable checking.
        IsConcurrencyChecked : 1);
    // STONE_INLINE_BITFIELD_EMPTY(TypeDecl, ValueDecl);

  } Bits;

  // Storage for the declaration attributes.
  DeclAttributeList Attributes;

  // Storage for the declaration attributes.
  DeclModifierList Modifiers;

  /// The next declaration in the list of declarations within this
  /// member context.
  Decl *nextDecl = nullptr;

public:
  Decl() = delete;
  Decl(const Decl &) = delete;
  Decl(Decl &&) = delete;
  Decl &operator=(const Decl &) = delete;
  Decl &operator=(Decl &&) = delete;

protected:
  Decl(DeclKind kind, DeclName name, UnifiedContext context)
      : kind(kind), name(name), context(context) {}

public:
  DeclKind GetKind() const { return kind; }
  DeclName GetName() const { return name; }
  Identifier GetIdentifier() const {
    return GetName().GetDeclNameBase().GetIdentifier();
  }
  SrcLoc GetLoc() const { return kindLoc; }

public:
  DeclContext *GetDeclContextForModule() const;
  DeclContext *GetDeclContext() const {
    if (auto dc = context.dyn_cast<DeclContext *>()) {
      return dc;
    }
    return GetDeclContextForModule();
  }

  ModuleDecl *GetModuleContext() const;
  ASTContext &GetASTContext() const {
    auto dc = context.dyn_cast<DeclContext *>();
    if (dc) {
      return dc->GetASTContext();
    }
    return *context.get<ASTContext *>();
  }

  const DeclAttributeList &GetAttributes() const { return Attributes; }
  DeclAttributeList &GetAttributes() { return Attributes; }

  const DeclModifierList &GetModifiers() const { return Modifiers; }
  DeclModifierList &GetModifiers() { return Modifiers; }

public:
  bool IsTopLevel() { return Bits.Decl.IsTopLevel; }
  void SetIsTopLevel(bool isTopLevel = true) {
    Bits.Decl.IsTopLevel = isTopLevel;
  }

protected:
  template <typename DeclTy> friend class Redeclarable;

public:
  bool Walk(ASTWalker &walker);

public:
  template <typename DeclTy, typename AllocatorTy>
  static void *AllocateMemory(AllocatorTy &allocatorTy, size_t baseSize,
                              bool extraSace = false);
};

class ValueDecl : public Decl {

  TypeState *typeState;
  VisibilityLevel visibilityKind;

public:
  ValueDecl(DeclKind kind, DeclName name, TypeState *typeState,
            UnifiedContext context)
      : Decl(kind, name, context), typeState(typeState) {}

public:
  void SetType(TypeState *ts) { typeState = ts; }
  TypeState *GetTypeState() { return typeState; }

public:
  /// IsInstanceMember - Determine whether this value is an instance member
  /// of an enum, struct or interface.
  bool IsInstanceMember() const;

  bool HasVisibilityLevel() { return visibilityKind != VisibilityLevel::None; }
  VisibilityLevel GetVisibilityLevel() const { return visibilityKind; }

  void SetVisibilityLevel(VisibilityLevel visibilityKind) {
    assert(!HasVisibilityLevel() && "access already set");
    OverwriteVisibility(visibilityKind);
  }
  // Overwrite the access of this declaration.
  // This is needed in the LLDB REPL.
  void OverwriteVisibility(VisibilityLevel inputLevel) {
    visibilityKind = inputLevel;
  }

  /// Return whether this declaration has been determined invalid.
  bool IsInvalid() const;

  /// Mark this declaration invalid.
  void SetInvalid();

  /// Determine whether this declaration was implicitly generated by the
  /// compiler (rather than explicitly written in source code).
  // bool IsImplicit() const { return Bits.Decl.IsImplicit; }

  // /// Mark this declaration as implicit.
  // void SetImplicit(bool isImplicit = true) { Bits.Decl.IsImplicit =
  // isImplicit; }

public:
  static bool classof(const Decl *d) {
    return d->GetKind() >= DeclKind::FirstValueDecl &&
           d->GetKind() <= DeclKind::LastValueDecl;
  }
};

class TypeDecl : public ValueDecl /*TODO: AnyDecl, ForwardDecl*/ {

  friend class ASTContext;
  /// This indicates the Type object that represents
  /// this TypeDecl.  It is a cache maintained by
  /// ASTContext::getTypedefType, ASTContext::getTagDeclKind, and
  /// ASTContext::getTemplateTypeParmType, and TemplateTypeParmDecl.
  mutable const TypeState *typeForDecl = nullptr;

  /// The start of the source range for this declaration.
  // SrcLoc startLoc;
  // DeclName name;
  // SrcLoc nameLoc;

protected:
  TypeDecl(DeclKind kind, DeclName name, TypeState *typeState,
           UnifiedContext context)
      : ValueDecl(kind, name, typeState, context) {}

public:
  // Low-level accessor. If you just want the type defined by this node,
  // check out ASTContext::getTypeDeclType or one of
  // ASTContext::getTypedefType, ASTContext::getRecordType, etc. if you
  // already know the specific kind of node this is.
  const TypeState *GetTypeForDecl() const { return typeForDecl; }
  void SetTypeForDecl(const TypeState *TS) { typeForDecl = TS; }

  // SrcLoc GetBeginSrcLoc() const LLVM_READONLY { return LocStart; }
  // void SetStartSrcLoc(startSrcLoc L) { LocStart = L; }
};

class alignas(8) GenericContextBase {
  // // Not really public. See GenericContext.
public:
  /// The state of the generic parameters.
  enum class GenericParamsState : uint8_t {
    /// The stored generic parameters represent parsed generic parameters,
    /// written in the source.
    Parsed = 0,
    /// The stored generic parameters represent generic parameters that are
    /// synthesized by the type checker but were not written in the source.
    TypeChecked = 1,
    /// The stored generic parameters represent both the parsed and
    /// type-checked generic parameters.
    ParsedAndTypeChecked = 2,
  };

  llvm::PointerIntPair<GenericParamList *, 2, GenericParamsState>
      genericParamsAndState;

  /// The trailing where clause.
  ///
  /// Note that this is not currently serialized, because semantic analysis
  /// moves the trailing where clause into the generic parameter list.
  TrailingWhereClause *trailingWhereClause = nullptr;

  //   /// The generic signature of this declaration.
  //   llvm::PointerIntPair<GenericSignature, 1, bool> GenericSigAndBit;
};

class GenericContext : private GenericContextBase, public DeclContext {
public:
  GenericContext(DeclContextKind kind, DeclContext *parent,
                 GenericParamList *params = nullptr);
};

class GenericTypeDecl : public GenericContext, public TypeDecl {
public:
  GenericTypeDecl(DeclKind K, DeclContext *DC, DeclName name,
                  TypeState *typeState,
                  /*llvm::ArrayRef<InheritedEntry> inherited,*/
                  GenericParamList *genericParams = nullptr);
};

class GenericTypeParamDecl final
    : public TypeDecl,
      private llvm::TrailingObjects<GenericTypeParamDecl, TypeState *, SrcLoc> {
  friend TrailingObjects;
};

class AliasDecl : public GenericTypeDecl {

  /// The location of the 'alias' keyword // seems that this location should be
  /// in TypeDecl
  SrcLoc aliasLoc;

  /// The location of the equal '=' token
  SrcLoc equalLoc;

  /// The end of the type, valid even when the type cannot be parsed
  SrcLoc typeEndLoc;

  /// The location of the right-hand side of the typealias binding
  // TypeLoc underlyingTy;

public:
};

class LabelDecl : public Decl {
public:
};

class SpaceDecl final : public Decl, public DeclContext {

  SrcLoc lBraceStartLoc;
  SrcLoc rBraceEndLoc;
  SpaceDecl *parent;

public:
  SpaceDecl(DeclName name, DeclContext *parentDC, SpaceDecl *parent = nullptr)
      : Decl(DeclKind::Space, name, context),
        DeclContext(DeclContextKind::SpaceDecl, parentDC) {}

public:
};

class TypeParamDecl : public TypeDecl {};

// This is really your function prototye
class FunctionDecl : public GenericContext,
                     public ValueDecl
/*, public Redeclarable<FunctionDecl>*/ {

  // TypeLoc returnType;

  /// This enum member is active if GetBodyKind() is BodyKind::Parsed or
  /// BodyKind::TypeChecked.
  BraceStmt *body;

  StorageSpecKind storageSpecKind;

  /// Info - Further source/type location info for special kinds of names.
  // TODO: DeclNameLoc specialNameLoc;

public:
  enum class BodyStatus {
    None,

    /// Function body is delayed, to be parsed later.
    Unparsed,

    /// Function body is parsed and available as an AST subtree.
    Parsed,

    /// Function body is not available, although it was written in the source.
    Skipped,

    /// Function body will be synthesized on demand.
    Synthesize,

    /// Function body is present and type-checked.
    TypeChecked,

    /// Function body text was deserialized from a .swiftmodule.
    Deserialized
  };

public:
  FunctionDecl(DeclKind kind, DeclName name, TypeState *resultType,
               DeclContext *parent)
      : GenericContext(DeclContextKind::FunctionDecl, parent),
        ValueDecl(kind, name, resultType, parent) {}

public:
  /// TODO:
  void SetBodyStatus(BodyStatus status) {
    // Bits.AbstractFunctionDecl.BodyKind = unsigned(K);
  }

  BraceStmt *GetBody(bool canSynthesize = true) const;
  /// Set a new body for the function.
  void SetBody(BraceStmt *body, BodyStatus bodyStatus);

  void SetStorageSpecKind(StorageSpecKind ssk) { storageSpecKind = ssk; }
  StorageSpecKind GetStorageSpecKind() { return storageSpecKind; }

  bool IsMember() { return Bits.FunctionDecl.IsMember; }

  // DeclNameLoc GetSpecialNameLoc() { return specialNameLoc; }
  //  void SetReturnType(TypeDecl* tyDecl);

public:
  static bool classof(const Decl *d) {
    return d->GetKind() >= DeclKind::FirstFunctionDecl &&
           d->GetKind() <= DeclKind::LastFunctionDecl;
  }

  static bool classof(const DeclContext *dc) {
    if (auto d = dc->ToDecl())
      return classof(d);
    return false;
  }

public:
  using DeclContext::operator new;
  using DeclContext::operator delete;
  using Decl::GetASTContext;
};

/// Standalone function: fun F0() -> void {}
class FunDecl : public FunctionDecl {

  SrcLoc staticLoc;

  // TODO: You should aonly pass ASTContext and DeclContext
  SrcLoc funLoc;
  bool hasLBrace;

  /// fun GetObject() -> Object* { return obj; } where obj is the returnType.
  /// and Oject* is the Type which is the resultType
  // TypeLoc result;

  // TODO: We are removing SpecialNameLoc for now.
public:
  FunDecl(DeclKind kind, SrcLoc staticLoc, DeclName name, TypeState *result,
          DeclContext *parent)
      : FunctionDecl(kind, name, result, parent), staticLoc(staticLoc),
        funLoc(funLoc) {}

public:
  bool IsMain() const;

  /// True if the function is a defer body.
  bool IsDeferBody() const;

  bool IsStatic() const;

  bool IsMember() const;

  // TODO: Think about
  bool IsForward() const;

  bool HasReturn() const;

  // TypeLoc GetReturnType() const;

  //  bool IsMain() { return Bits.FunDecl.IsTopLevel; }
  // void SetIsTopLevel(bool isTopLevel = true) {
  //   Bits.Decl.IsTopLevel = isTopLevel;
  // }

public:
  // void SetReturnType(TypeDecl* returnTy);
  // SrcLoc GetStaticLoc() const { return staticLoc; }
  // SrcLoc GetFunLoc() const { return funcLoc; }

  static bool classof(const Decl *d) { return d->GetKind() == DeclKind::Fun; }

  static bool classof(const FunctionDecl *d) {
    return classof(static_cast<const Decl *>(d));
  }
  static bool classof(const DeclContext *dc) {
    if (auto d = dc->ToDecl())
      return classof(d);
    return false;
  }

public:
  static FunDecl *Create(ASTContext &AC, SrcLoc staticLoc, SrcLoc funLoc,
                         DeclName name, TypeState *result, DeclContext *parent);

  // static FunDecl *Create(DeclSpecifierCollector &collector,
  //                        ASTContext &astContext, DeclContext *parent);

  // /// Implitictly create function
  // static FunDecl *CreateImplicit(DeclSpecifierCollector &collector,
  //                                ASTContext &sc, DeclContext *parent);
};

class ConstructorDecl : public FunctionDecl {
public:
};

class DestructorDecl : public FunctionDecl {
public:
};

class NominalTypeDecl : public GenericTypeDecl {
public:
  static bool classof(const Decl *d) { return true; }
};

class StructDecl final : public NominalTypeDecl {
public:
  static bool classof(const Decl *d) {
    return d->GetKind() == DeclKind::Struct;
  }

public:
  void AddMember(Decl *d);

public:
  static StructDecl *Create(DeclName name, SrcLoc loc, ASTContext &astContext,
                            DeclContext *parent = nullptr);
};

class ClassDecl final : public NominalTypeDecl {
public:
  static bool classof(const Decl *D) { return D->GetKind() == DeclKind::Class; }
};

class InterfaceDecl final : public NominalTypeDecl {
public:
  static bool classof(const Decl *d) {
    return d->GetKind() == DeclKind::Interface;
  }

public:
  static InterfaceDecl *Create(DeclName name, SrcLoc loc, ASTContext &sc,
                               DeclContext *parent = nullptr);
};

class EnumDecl final : public NominalTypeDecl {
public:
  static bool classof(const Decl *d) { return d->GetKind() == DeclKind::Enum; }

public:
  static EnumDecl *Create(DeclName name, SrcLoc loc, ASTContext &sc,
                          DeclContext *parent = nullptr);
};

// THINK: struct S { } join S { S() {} ~S() {} fun Print() -> void {} ...}
class JoinDecl {};

// Declarators and the like
class StorageDecl : public ValueDecl {
public:
};

class VarDecl : public StorageDecl {
public:
  /// Get the type of the variable within its context. If the context is
  /// generic, this will use archetypes.
  // Type GetType() const;

  // VarDecl(Type ty);

public:
  static VarDecl *Create(ASTContext &sc);
};

class ParamDecl : public VarDecl {
public:
};

class ImportDecl final : public Decl {
  SrcLoc importLoc;
  ImportKind importKind;

public:
  // Module *mod = nullptr;
};

/// THINK: join a module -- this is also defined lower
// class JoinDecl final : public Decl {
//   SrcLoc joinLoc;

// public:
// };

class TrustDecl final
    : public Decl,
      public llvm::TrailingObjects<TrustDecl, GenericParamList *> {

public:
};

class UsingDecl final
    : public Decl,
      public llvm::TrailingObjects<TrustDecl, GenericParamList *> {

public:
};
/// IfConfigDecl - This class represents #if/#else/#endif blocks.
/// Active and inactive block members are stored separately, with the
/// actionion being that active members will be handed back to the enclosing
/// context.
class IfConfigDecl : public Decl {

  friend Decl;

  SrcLoc endLoc;

  /// An array of clauses controlling each of the #if/#elseif/#else
  /// conditions. The array is ASTContext allocated.
  llvm::ArrayRef<IfConfigClause> clauses;

  SrcLoc GetLocFromSource() const {
    assert(clauses.size() > 0);
    return clauses[0].loc;
  }

public:
  // IfConfigDecl(DeclContext *parent, llvm::ArrayRef<IfConfigClause> clauses,
  //              SrcLoc endLoc, bool hadMissingEnd)
  //   : Decl(DeclKind::IfConfig, Parent), Clauses(Clauses), EndLoc(EndLoc)
  // {
  //   Bits.IfConfigDecl.HadMissingEnd = HadMissingEnd;
  // }

  // ArrayRef<IfConfigClause> getClauses() const { return Clauses; }

  // /// Return the active clause, or null if there is no active one.
  // const IfConfigClause *getActiveClause() const {
  //   for (auto &Clause : Clauses)
  //     if (Clause.isActive) return &Clause;
  //   return nullptr;
  // }

  // const ArrayRef<ASTNode> getActiveClauseElements() const {
  //   if (auto *Clause = getActiveClause())
  //     return Clause->Elements;
  //   return {};
  // }

  // SrcLoc getEndLoc() const { return EndLoc; }

  // bool hadMissingEnd() const { return Bits.IfConfigDecl.HadMissingEnd; }

  // SourceRange getSourceRange() const;

  // static bool classof(const Decl *D) {
  //   return D->getKind() == DeclKind::IfConfig;
  // }
};

class OperatorDecl : public Decl {};

/// Top level expressions, and statements
// class TopLevelCodeDecl final :, public Decl, public DeclContext {
// public:
// };

} // namespace stone
#endif
