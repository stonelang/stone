#ifndef STONE_SYNTAX_DECLCOLLECTOR_H
#define STONE_SYNTAX_DECLCOLLECTOR_H

#include "stone/Basic/OptionSet.h"
#include "stone/Syntax/Attribute.h"
#include "stone/Syntax/DeclName.h"
#include "stone/Syntax/Generics.h"
#include "stone/Syntax/TypeQualifier.h"
#include "stone/Syntax/TypeThunk.h"
#include "stone/Syntax/Types.h"

#include "llvm/ADT/ArrayRef.h"

namespace stone {
namespace syn {

/// Describes the kind of unqualified-id parsed.
enum class UnqualifiedIdKind {
  /// An identifier, e.g., int, Particle
  Identifier,
  /// An overloaded operator name, e.g., operator+.
  OperatorFunction,
  /// A conversion function name, e.g., operator int.
  ConversionFunction,
  /// A user-defined literal name, e.g., operator "" _i.
  LiteralOperator,
  /// A constructor name.
  ConstructorName,
  /// A constructor named via a template-id.
  ConstructorTemplate,
  /// A destructor name.
  DestructorName,
  /// A template-id, e.g., f<int>.
  Template,

  // TODO: Think about
  /// An implicit 'this' parameter
  ImplicitThisParam,
  /// A deduction-guide name (a template-name)
  DeductionGuideName
};

class UnqualifiedId {
private:
  UnqualifiedId(const UnqualifiedId &other) = delete;
  const UnqualifiedId &operator=(const UnqualifiedId &) = delete;

public:
  /// Describes the kind of unqualified-id parsed.
  UnqualifiedIdKind Kind;

  union {
    /// When Kind == Identifier, the parsed identifier, or when
    /// Kind == UserLiteralId, the identifier suffix.
    Identifier *identifier;
  };
};

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
  SrcLoc arrowLoc;
  SrcLoc doubleColonLoc;

  enum Flags : unsigned {
    None = 1 << 0,
    Fun = 1 << 1,
    Inline = 1 << 2,
    ForcedInline = 1 << 3,
    Virtual = 1 << 4,
    NoReturn = 1 << 5,
    IsMember = 1 << 6,
    IsStatic = 1 << 7,
  };

private:
  unsigned flags;

public:
  void AddFun(SrcLoc loc) {
    flags |= Fun;
    funLoc = loc;
  }
  bool HasFun() { return ((flags & Fun) && funLoc.isValid()); }
  SrcLoc GetFunLoc() { return funLoc; }

  void AddInline(SrcLoc loc) {
    flags |= Inline;
    inlineLoc = loc;
  }
  bool HasInline() { return ((flags & Inline) && inlineLoc.isValid()); }
  SrcLoc GetInlineLoc() { return inlineLoc; }

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

  void AddArrowLoc(SrcLoc loc) { arrowLoc = loc; }
  SrcLoc GetArrowLoc() { return arrowLoc; }

  void AddIsMember(SrcLoc inputLoc) {
    flags |= IsMember;
    doubleColonLoc = inputLoc;
  }
  bool HasIsMember() { return (flags & IsMember) && doubleColonLoc.isValid(); }
  SrcLoc GetDoubleColonLoc() { return doubleColonLoc; }

public:
  void Apply();
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
  bool HasAny() {
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
  void Apply();
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

  bool HasAny() { return (HasPublic() || HasPrivate() || HasInternal()); }
  SrcLoc GetLoc() { return loc; }
  AccessLevel GetAccessLevel() { return level; }

  void Apply();
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

class TypeCollector final {
  Type type;
  TypeSpecifierCollector typeSpecifierCollector;
  TypeQualifierCollector typeQualifierCollector;
  TypeThunkCollector typeChunkCollector;

public:
  TypeCollector() {}

  TypeQualifierCollector &GetTypeQualifierCollector() {
    return typeQualifierCollector;
  }
  const TypeQualifierCollector &GetTypeQualifierCollector() const {
    return typeQualifierCollector;
  }
  TypeSpecifierCollector &GetTypeSpecifierCollector() {
    return typeSpecifierCollector;
  }
  const TypeSpecifierCollector &GetTypeSpecifierCollector() const {
    return typeSpecifierCollector;
  }

  TypeThunkCollector &GetTypeThunkCollector() { return typeChunkCollector; }
  const TypeThunkCollector &GetTypeThunkCollector() const {
    return typeChunkCollector;
  }
  void Apply();

public:
  void SetType(Type inputType) { type = inputType; }
  Type GetType() { return type; }
};

class DeclCollector {

  AttributeCollector attributeCollector;
  TypeCollector typeCollector;
  StorageSpecifierCollector storageSpecifierCollector;
  FunctionSpecifierCollector functionSpecifierCollector;
  UsingDeclarationCollector usingDeclarationCollector;
  AccessLevelCollector accessLevelCollector;

  // DeclNameLoc
  DeclName name;
  SrcLoc nameLoc;

private:
  DeclCollector(const DeclCollector &) = delete;
  void operator=(const DeclCollector &) = delete;

public:
  DeclCollector();

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

  FunctionSpecifierCollector &GetFunctionSpecifierCollector() {
    return functionSpecifierCollector;
  }
  const FunctionSpecifierCollector &GetFunctionSpecifierCollector() const {
    return functionSpecifierCollector;
  }

  TypeCollector &GetTypeCollector() { return typeCollector; }
  const TypeCollector &GetTypeCollector() const { return typeCollector; }

  AccessLevelCollector &GetAccessLevelCollector() {
    return accessLevelCollector;
  }

  AttributeCollector &GetAttributeCollector() { return attributeCollector; }
  const AttributeCollector &GetAttributeCollector() const {
    return attributeCollector;
  }

  void SetDeclName(DeclName inputName) { name = inputName; }
  DeclName GetDeclName() { return name; }

  void SetDeclNameLoc(SrcLoc inputLoc) { nameLoc = inputLoc; }
  SrcLoc GetDeclNameLoc() { return nameLoc; }

public:
  void Apply();
};

} // namespace syn
} // namespace stone
#endif
