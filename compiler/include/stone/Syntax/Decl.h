#ifndef STONE_SYNTAX_DECL_H
#define STONE_SYNTAX_DECL_H

#include "stone/Basic/AddressSpace.h"
#include "stone/Basic/LLVM.h"
#include "stone/Basic/SrcLoc.h"
#include "stone/Diag/DiagnosticArgument.h"
#include "stone/Syntax/Access.h"
#include "stone/Syntax/DeclContext.h"
#include "stone/Syntax/DeclKind.h"
#include "stone/Syntax/DeclName.h"
#include "stone/Syntax/Generics.h"
#include "stone/Syntax/Identifier.h"
#include "stone/Syntax/IfConfig.h"
#include "stone/Syntax/Import.h"
#include "stone/Syntax/InlineBitfield.h"
#include "stone/Syntax/Specifier.h"
#include "stone/Syntax/SyntaxAllocation.h"
#include "stone/Syntax/TypeAlignment.h"
#include "stone/Syntax/TypeLoc.h"
#include "stone/Syntax/Types.h"

// #include "stone/Syntax/Redeclarable.h"

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
class DiagnosticEngine;
namespace syn {

class Decl;
class Stmt;
class ModuleDecl;
class BraceStmt;
class DeclContext;
class SyntaxContext;
class ValueDecl;
class VarDecl;
struct SyntaxNode;
class Type;
class Expr;
class ConstructorDecl;
class DestructorDecl;
class AliasDecl;
class ArchetypeKind;
class SyntaxPrinter;
class SyntaxWalker;
class GenericParamList;
class TrailingWhereClause;

class DeclStats final : public Stats {
  const Decl &declaration;

public:
  DeclStats(const Decl &declaration)
      : Stats("ast-declaration stats:"), declaration(declaration) {}
  void Print(ColorfulStream &stream) override;
};

using UnifiedContext = llvm::PointerUnion<DeclContext *, SyntaxContext *>;

enum : unsigned {
  NumDeclKindBits = stone::CountBitsUsed(static_cast<unsigned>(DeclKind::Count))
};

// Kinds of pointer types.
enum PointerTypeKind : unsigned {
  Raw,
};

class alignas(1 << DeclAlignInBits) Decl : public syn::SyntaxAllocation<Decl> {
  friend DeclStats;

  DeclKind kind;
  /// The location of the decl
  SrcLoc loc;
  DeclContext *dc;

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
          IsTopLevel : 1

    );

    STONE_INLINE_BITFIELD(
        ValueDecl, Decl, 1 + 1 + 1 + 1,

        IsInLookupTable : 1,

        /// Whether we have already checked whether this declaration is a
        /// redeclaration.
        CheckedRedeclaration : 1,

        /// Whether the decl can be accessed by swift users; for instance,
        /// a.storage for lazy var a is a decl that cannot be accessed.
        IsUserAccessible : 1,

        /// Whether this member was synthesized as part of a derived
        /// protocol conformance.
        IsSynthesized : 1);

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
        FunDecl, FunctionDecl, 1,
        /// Whether we've computed the 'static' flag yet.
        // IsStaticComputed : 1,

        /// Whether this function is a 'static' method.
        IsStatic : 1

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

  /// The next declaration in the list of declarations within this
  /// member context.
  Decl *nextDecl = nullptr;

public:
  Decl() = delete;
  Decl(const Decl &) = delete;
  Decl(Decl &&) = delete;
  Decl &operator=(const Decl &) = delete;
  Decl &operator=(Decl &&) = delete;

public:
  friend class DeclContext;
  UnifiedContext context;

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

  PrettyDeclKind GetPrettyKind() const;
  static llvm::StringRef GetPrettyKindName(PrettyDeclKind kind);

  SrcLoc GetLoc() const { return loc; }
  DeclContext *GetDeclContextForModule() const;

  DeclContext *GetDeclContext() const {
    if (auto dc = context.dyn_cast<DeclContext *>()) {
      return dc;
    }
    return GetDeclContextForModule();
  }

  syn::Module *GetModuleContext() const;

  SyntaxContext &GetSyntaxContext() const {
    auto dc = context.dyn_cast<DeclContext *>();
    if (dc) {
      return dc->GetSyntaxContext();
    }
    return *context.get<SyntaxContext *>();
  }

protected:
  Decl(DeclKind kind, UnifiedContext context) : kind(kind), context(context) {}

protected:
  template <typename DeclTy> friend class Redeclarable;

public:
  bool Walk(SyntaxWalker &walker);
};

class NameableDecl : public Decl {
  /// The name of this declaration, which is typically a normal
  /// identifier but may also be a special ty of name (C++
  /// constructor, etc.)
  DeclName name;
  SrcLoc nameLoc;

protected:
  NameableDecl(DeclKind kind, DeclName name, SrcLoc nameLoc,
               UnifiedContext context)
      : Decl(kind, context), name(name), nameLoc(nameLoc) {}

public:
  /// Get the identifier that names this declaration, if there is one.
  ///
  /// This will return NULL if this declaration has no name (e.g., for
  /// an unnamed class) or if the name is a special name such ast a C++
  /// constructor.
  Identifier GetBasicName() const { return name.GetDeclNameBaseIdentifier(); }

  /// Get the name of identifier for this declaration as a StringRef.
  ///
  /// This requires that the declaration have a name and that it be a simple
  /// identifier.
  llvm::StringRef GetBasicNameText() const {
    return GetBasicName().GetString();
  }

  void SetDeclName(DeclName name) { this->name = name; }
  DeclName GetDeclName() { return name; }

  void SetDeclNameLoc(SrcLoc nameLoc) { this->nameLoc = nameLoc; }
  SrcLoc GetDeclNameLoc() { return nameLoc; }
};

class ValueDecl : public NameableDecl {

  Type type;
  AccessLevel level;

public:
  ValueDecl(DeclKind kind, DeclName name, SrcLoc nameLoc, Type type,
            UnifiedContext context)
      : NameableDecl(kind, name, nameLoc, context), type(type) {}

public:
  void SetType(Type inputType) { type = inputType; }
  Type GetType() { return type; }

public:
  /// IsInstanceMember - Determine whether this value is an instance member
  /// of an enum, struct or interface.
  bool IsInstanceMember() const;

  bool HasAccessLevel() { return level != AccessLevel::None; }
  AccessLevel GetAccessLevel() const { return level; }

  void SetAccessLevel(AccessLevel level) {
    assert(!HasAccessLevel() && "access already set");
    OverwriteAccess(level);
  }
  // Overwrite the access of this declaration.
  // This is needed in the LLDB REPL.
  void OverwriteAccess(AccessLevel inputLevel) { level = inputLevel; }

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

  friend class SyntaxContext;
  /// This indicates the Type object that represents
  /// this TypeDecl.  It is a cache maintained by
  /// ASTContext::getTypedefType, ASTContext::getTagDeclKind, and
  /// ASTContext::getTemplateTypeParmType, and TemplateTypeParmDecl.
  mutable const Type *typeForDecl = nullptr;

  /// The start of the source range for this declaration.
  // SrcLoc startLoc;
  // DeclName name;
  // SrcLoc nameLoc;

protected:
  TypeDecl(DeclKind kind, Identifier name, SrcLoc nameLoc, Type type,
           UnifiedContext context)
      : ValueDecl(kind, name, nameLoc, type, context) {}

public:
  // Low-level accessor. If you just want the type defined by this node,
  // check out ASTContext::getTypeDeclType or one of
  // ASTContext::getTypedefType, ASTContext::getRecordType, etc. if you
  // already know the specific kind of node this is.
  const Type *GetTypeForDecl() const { return typeForDecl; }
  void SetTypeForDecl(const Type *TD) { typeForDecl = TD; }

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
  GenericTypeDecl(DeclKind K, DeclContext *DC, Identifier name, SrcLoc nameLoc,
                  Type type,
                  /*llvm::ArrayRef<InheritedEntry> inherited,*/
                  GenericParamList *genericParams = nullptr);
};

class GenericTypeParamDecl final
    : public TypeDecl,
      private llvm::TrailingObjects<GenericTypeParamDecl, Type *, SrcLoc> {
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
  TypeLoc underlyingTy;

public:
};

class LabelDecl : public NameableDecl {
public:
};

class SpaceDecl final : public NameableDecl, public DeclContext {

  SrcLoc lBraceStartLoc;
  SrcLoc rBraceEndLoc;
  SpaceDecl *parent;

public:
  SpaceDecl(Identifier name, SrcLoc nameLoc, DeclContext *parentDC,
            SpaceDecl *parent = nullptr)
      : NameableDecl(DeclKind::Space, name, nameLoc, context),
        DeclContext(DeclContextKind::SpaceDecl, parentDC) {}

public:
};

class TypeParamDecl : public TypeDecl {};

// This is really your function prototye
class FunctionDecl : public GenericContext,
                     public ValueDecl
/*, public syn::Redeclarable<FunctionDecl>*/ {

  // TypeLoc returnType;

  /// This enum member is active if GetBodyKind() is BodyKind::Parsed or
  /// BodyKind::TypeChecked.
  BraceStmt *body;

  StorageSpecifierKind storageSpecifierKind;

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
  FunctionDecl(DeclKind kind, DeclName name, SrcLoc nameLoc, Type retType,
               DeclContext *parent)
      : GenericContext(DeclContextKind::FunctionDecl, parent),
        ValueDecl(kind, name, nameLoc, retType, parent) {}

public:
  /// TODO:
  void SetBodyStatus(BodyStatus status) {
    // Bits.AbstractFunctionDecl.BodyKind = unsigned(K);
  }

  BraceStmt *GetBody(bool canSynthesize = true) const;
  /// Set a new body for the function.
  void SetBody(BraceStmt *body, BodyStatus bodyStatus);

  void SetStorageSpecifierKind(StorageSpecifierKind ssk) {
    this->storageSpecifierKind = ssk;
  }
  StorageSpecifierKind GetStorageSpecifierKind() {
    return storageSpecifierKind;
  }

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
  using Decl::GetSyntaxContext;
};

/// Standalone function: fun F0() -> void {}
class FunDecl : public FunctionDecl {
  // TODO: You should aonly pass SyntaxContext and DeclContext
  SrcLoc funLoc;
  bool hasLBrace;

  /// fun GetObject() -> Object* { return obj; } where obj is the returnType.
  /// and Oject* is the QualType which is the resultType
  TypeLoc result;

  // TODO: We are removing SpecialNameLoc for now.
public:
  FunDecl(DeclKind kind, SrcLoc funLoc, DeclName name, SrcLoc nameLoc,
          Type result, DeclContext *parent)
      : FunctionDecl(kind, name, nameLoc, result, parent) {}

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
};

/// Member functions: fun Particle::Fire() -> bool ...
class MethodDecl : public FunDecl {
  /// TODO: pass , NominalTypeDecl* owner,
public:
  MethodDecl(DeclKind kind, SrcLoc funLoc, DeclName name, SrcLoc nameLoc,
             Type retType, DeclContext *parent)
      : FunDecl(kind, funLoc, name, nameLoc, retType, parent) {}
};

class ConstructorDecl : public MethodDecl {
public:
};

class DestructorDecl : public MethodDecl {
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
};

class InterfaceDecl final : public NominalTypeDecl {
public:
  static bool classof(const Decl *d) {
    return d->GetKind() == DeclKind::Interface;
  }
};

class EnumDecl final : public NominalTypeDecl {
public:
  static bool classof(const Decl *d) { return d->GetKind() == DeclKind::Enum; }
};

// Declarators and the like
class StorageDecl : public ValueDecl {
public:
};

class VarDecl : public StorageDecl {
public:
  /// Get the type of the variable within its context. If the context is
  /// generic, this will use archetypes.
  // QualType GetQualType() const;
};

class ParamDecl : public VarDecl {
public:
};

class ImportDecl final : public NameableDecl {
  SrcLoc importLoc;
  ImportKind importKind;

public:
  // syn::Module *mod = nullptr;
};

class TrustDecl final
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
  /// conditions. The array is SyntaxContext allocated.
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

class TopLevelDecl final : public DeclContext, public Decl {
public:
};

} // namespace syn
} // namespace stone
#endif
