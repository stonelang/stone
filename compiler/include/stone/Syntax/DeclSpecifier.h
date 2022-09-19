#ifndef STONE_SYNTAX_DECLSPECIFIER_H
#define STONE_SYNTAX_DECLSPECIFIER_H

#include "stone/Basic/OptionSet.h"
#include "stone/Syntax/Attribute.h"
#include "stone/Syntax/Specifier.h"
#include "stone/Syntax/Template.h"
#include "stone/Syntax/Types.h"

#include "llvm/ADT/ArrayRef.h"

namespace stone {
namespace syn {

class ScopeSpecifier {
  SrcRange range;
  // NestedNameSpecifierLocBuilder builder;

public:
  SrcRange GetRange() const { return range; }
  void SetRange(SrcRange R) { range = R; }
  void SetBeginLoc(SrcLoc loc) { range.Start = loc; }
  void SetEndLoc(SrcLoc loc) { range.End = loc; }
  SrcLoc GetBeginLoc() const { return range.Start; }
  SrcLoc GetEndLoc() const { return range.End; }

  // /// Retrieve the representation of the nested-name-specifier.
  // NestedNameSpecifier *GetScopeRepresentation() const {
  //   return builder.GetRepresentation();
  // }
};

class FunctionSpecifierCollector final {
  SrcLoc inlineLoc;
  SrcLoc forcedInlineLoc;
  SrcLoc virtualLoc;
  SrcLoc funLoc;

  enum Flags : unsigned {
    None = 1 << 0,
    Fun = 1 << 1,
    Inline = 1 << 2,
    ForcedInline = 1 << 3,
    Virtual = 1 << 4,
    NoReturn = 1 << 5,
    IsMember = 1 << 6
  };

private:
  unsigned flags;

public:
  void AddFun(SrcLoc loc) {
    flags |= Fun;
    funLoc = loc;
  }
  bool HasFun() { return ((flags & Fun) && funLoc.isValid()); }

  void AddInline(SrcLoc loc) {
    flags |= Inline;
    inlineLoc = loc;
  }
  bool HasInline() { return ((flags & Inline) && inlineLoc.isValid()); }

  void AddForcedInline(SrcLoc loc) {
    flags |= ForcedInline;
    forcedInlineLoc = loc;
  }
  bool HasForcedInline() {
    return ((flags & ForcedInline) && forcedInlineLoc.isValid());
  }

  void AddVirtual(SrcLoc loc) { flags |= Virtual; }
  bool HasVirtual() { return ((flags & Virtual) && virtualLoc.isValid()); }

  void AddNoReturn(SrcLoc loc) { flags |= NoReturn; }
  bool HasNoReturn() { return (flags & NoReturn); }

  void AddIsMember() { flags |= IsMember; }
  bool HasIsMember() { return (flags & IsMember); }

  SrcLoc GetFunLoc() { return funLoc; }
};

class StorageSpecifierCollector final {

  SrcLoc loc;
  StorageSpecifierKind kind;
  StorageDuration duration;

private:
  void AddStorageSpecifier(StorageSpecifierKind inputKind, SrcLoc inputLoc) {
    kind = inputKind;
    loc = inputLoc;
  }
  void AddStorageDuration(StorageDuration inputDuration) {
    duration = inputDuration;
  }

public:
  StorageSpecifierCollector() : kind(StorageSpecifierKind::None) {}

public:
  StorageSpecifierKind GetKind() { return kind; }
  StorageDuration GetDuration() { return duration; }

  void AddExtern(SrcLoc loc) {
    AddStorageSpecifier(StorageSpecifierKind::Extern, loc);
  }
  bool HasExtern() {
    return (kind == StorageSpecifierKind::Extern && loc.isValid());
  }

  void AddStatic(SrcLoc loc) {
    AddStorageSpecifier(StorageSpecifierKind::Static, loc);
  }
  bool HasStatic() {
    return (kind == StorageSpecifierKind::Static && loc.isValid());
  }

  void AddRegister(SrcLoc loc) {
    AddStorageSpecifier(StorageSpecifierKind::Register, loc);
  }
  bool HasRegister() {
    return (kind == StorageSpecifierKind::Register && loc.isValid());
  }
  bool HasAnyStorageSpecifier() {
    /// TODO: Consider auto
    return (HasExtern() || HasStatic() || HasRegister());
  }

  SrcLoc GetLoc() { return loc; }

  void AddFullExpressionStorageDuration() {
    AddStorageDuration(StorageDuration::FullExpression);
  }
  void AddAutomaticStorageDuration() {
    AddStorageDuration(StorageDuration::Automatic);
  }
  void AddThreadStorageDuration() {
    AddStorageDuration(StorageDuration::Thread);
  }
  void AddStaticStorageDuration() {
    AddStorageDuration(StorageDuration::Static);
  }
  void AddDynamicStorageDuration() {
    AddStorageDuration(StorageDuration::Dynamic);
  }
};

class AccessLevelCollector final {
  SrcLoc loc;
  AccessLevel level;

private:
  void AddAccessLevel(AccessLevel inputLevel, SrcLoc inputLoc) {
    assert(level == AccessLevel::None);
    level = inputLevel;
    loc = inputLoc;
  }

public:
  AccessLevelCollector() : level(AccessLevel::None) {}

public:
  void AddPublic(SrcLoc inputLoc) {
    AddAccessLevel(AccessLevel::Public, inputLoc);
  }
  bool HasPublic() { return (level == AccessLevel::Public && loc.isValid()); }

  void AddPrivate(SrcLoc inputLoc) {
    AddAccessLevel(AccessLevel::Private, inputLoc);
  }
  bool HasPrivate() { return (level == AccessLevel::Private && loc.isValid()); }

  void AddInternal(SrcLoc inputLoc) {
    AddAccessLevel(AccessLevel::Internal, inputLoc);
  }
  bool HasInternal() {
    return (level == AccessLevel::Internal && loc.isValid());
  }

  bool HasAnyAccessLevel() {
    return (HasPublic() || HasPrivate() || HasInternal());
  }
  SrcLoc GetLoc() { return loc; }
};

// Light weight value type for now
class UsingDeclarationCollector final {
  SrcLoc loc;

public:
  UsingDeclarationCollector() : loc(SrcLoc()) {}

public:
  void AddUsing(SrcLoc inputLoc) { loc = inputLoc; }
  bool HasUsing() { return loc.isValid(); }
  SrcLoc GetLoc() { return loc; }
};

class DeclSpecifier {

  AttributeFactory &attributeFactory;
  TypeSpecifierCollector typeSpecifierCollector;
  TypeQualifierCollector typeQualifierCollector;
  StorageSpecifierCollector storageSpecifierCollector;
  FunctionSpecifierCollector functionSpecifierCollector;
  UsingDeclarationCollector usingDeclarationCollector;
  AccessLevelCollector accessLevelCollector;

  DeclSpecifier(const DeclSpecifier &) = delete;
  void operator=(const DeclSpecifier &) = delete;

public:
  DeclSpecifier(AttributeFactory &attributeFactory)
      : attributeFactory(attributeFactory) {}

public:
  UsingDeclarationCollector &GetUsingDeclarationCollector() {
    return usingDeclarationCollector;
  }
  const UsingDeclarationCollector &GetUsingDeclarationCollector() const {
    return usingDeclarationCollector;
  }
  StorageSpecifierCollector &GetStorageSpecifierCollector() {
    return storageSpecifierCollector;
  }
  const StorageSpecifierCollector &GetStorageSpecifierCollector() const {
    return storageSpecifierCollector;
  }
  TypeSpecifierCollector &GetTypeSpecifierCollector() {
    return typeSpecifierCollector;
  }
  const TypeSpecifierCollector &GetTypeSpecifierCollector() const {
    return typeSpecifierCollector;
  }
  FunctionSpecifierCollector &GetFunctionSpecifierCollector() {
    return functionSpecifierCollector;
  }
  const FunctionSpecifierCollector &GetFunctionSpecifierCollector() const {
    return functionSpecifierCollector;
  }
  TypeQualifierCollector &GetTypeQualifierCollector() {
    return typeQualifierCollector;
  }
  const TypeQualifierCollector &GetTypeQualifierCollector() const {
    return typeQualifierCollector;
  }
  AccessLevelCollector &GetAccessLevelCollector() {
    return accessLevelCollector;
  }
};

/// TODO: I think you can replace this with ScopeKind
enum class DeclaratorScopeKind {
  None = 0,
  SyntaxFile,          // File scope declaration.
  FunctionSignature,   // Within a function prototype.
  TypeName,            // Abstract VariableName for types.
  FunctionalCast,      // Type in a C++ functional cast expression.
  Member,              // Struct/Union field.
  Block,               // Declaration within a block in a function.
  ForInit,             // Declaration within first part of a for loop.
  SelectionInit,       // Declaration within optional init stmt of if/switch.
  Condition,           // Condition declaration in a C++ if/switch/while/for.
  TemplateParam,       // Within a template parameter list.
  New,                 // new-expression.
  BlockLiteral,        // Block literal VariableName.
  LambdaExpr,          // Lambda-expression VariableName.
  LambdaExprParameter, // Lambda-expression parameter VariableName.
  ConversionId,        // Conversion-type-id.
  TrailingReturn,      // Trailing-type-specifier.
  TrailingReturnVar,   // Trailing-type-specifier for VariableName.
  TemplateArg,         // Any template argument (in template argument list).
  TemplateTypeArg,     // Template type argument (in default argument).
  AliasDecl,           // Alias-declaration.
  AliasTemplate,       // Alias-declaration template.
  RequiresExpr         // Requires-expression.
};

enum class DeclaratorKind {
  Pointer,
  Reference,
  Array,
  Function,
  BlockPointer,
  MemberPointer,
  Paren,
  Pipe,
};

class alignas(8) Declarator {
  DeclaratorKind kind;

public:
  Declarator(DeclaratorKind kind) : kind(kind) {}
};

class PointerDeclarator final : public Declarator {
  UInt8 starCount;

public:
  PointerDeclarator() : Declarator(DeclaratorKind::Pointer), starCount(0) {}
  void AddPointer();

public:
  static PointerDeclarator Create();

public:
  // UInt8 GetStarCount() const { return starCount; }
  // void AddStar() { ++starCount; }
};

class MemberPointerDeclarator final : public Declarator {
public:
  MemberPointerDeclarator() : Declarator(DeclaratorKind::MemberPointer) {}

public:
  static MemberPointerDeclarator Create();
};

class ReferenceDeclarator final : public Declarator {
public:
  ReferenceDeclarator() : Declarator(DeclaratorKind::Reference) {}

public:
  void AddReference();

public:
  static ReferenceDeclarator Create();
};

class ArrayDeclarator final : public Declarator {
public:
  ArrayDeclarator() : Declarator(DeclaratorKind::Array) {}

public:
  static ArrayDeclarator Create();
};

class ParenDeclarator final : public Declarator {
public:
  ParenDeclarator() : Declarator(DeclaratorKind::Paren) {}

public:
  static ParenDeclarator Create();
};

class FunctionDeclarator final : public Declarator {
public:
  FunctionDeclarator() : Declarator(DeclaratorKind::Function) {}

public:
  static FunctionDeclarator Create();
};

class DeclaratorCollector {

  const DeclSpecifier &declSpecifier;
  ScopeSpecifier scopeSpecifier;
  /// Where we are parsing this Declarator.
  DeclaratorScopeKind declaratorScopeKind;

  // /// The C++17 structured binding, if any. This is an alternative to a Name.
  // DecompositionDeclarator bindingGroup;

  /// chunks - This holds each type that the Declarator includes as it is
  /// parsed.  This is pushed from the identifier out, which means that element
  /// #0 will be the most closely bound to the identifier, and
  /// chunks.back() will be the least closely bound.
  llvm::SmallVector<Declarator, 8> declarators;

  /// If this Declarator declares a template, its template parameter lists.
  // llvm::ArrayRef<TemplateParameterList *> templateParameterLists;

public:
  DeclaratorCollector(const DeclSpecifier &declSpecifier,
                      DeclaratorScopeKind scopeKind)
      : declSpecifier(declSpecifier), declaratorScopeKind(declaratorScopeKind) {
  }

public:
  /// getDeclSpec - Return the declaration-specifier that this Declarator was
  /// declared with.
  const DeclSpecifier &GetDeclSpecifier() const { return declSpecifier; }
  DeclaratorScopeKind GetDeclaratorScopeKind() { return declaratorScopeKind; }

public:
  /// Add a chunk to this Declarator. Also extend the range to
  /// EndLoc, which should be the last token of the chunk.
  void AddDeclarator(const Declarator &declarator, SrcLoc endLoc) {
    declarators.push_back(declarator);
    // TODO:
    //  if (!EndLoc.isInvalid())
    //    SetRangeEnd(EndLoc);
  }

public:
  void Collect();
};

} // namespace syn
} // namespace stone
#endif
