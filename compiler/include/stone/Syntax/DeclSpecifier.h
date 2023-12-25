#ifndef STONE_SYNTAX_DECL_SPECIFIER_H
#define STONE_SYNTAX_DECL_SPECIFIER_H

#include "stone/Basic/OptionSet.h"
#include "stone/Syntax/Attribute.h"
#include "stone/Syntax/DeclName.h"
#include "stone/Syntax/FunctionSpecifier.h"
#include "stone/Syntax/Generics.h"
#include "stone/Syntax/StorageSpecifier.h"
#include "stone/Syntax/TypeOperator.h"
#include "stone/Syntax/TypeQualifier.h"
#include "stone/Syntax/TypeSpecifier.h"
#include "stone/Syntax/TypeThunk.h"
#include "stone/Syntax/Types.h"

#include "llvm/ADT/ArrayRef.h"

namespace stone {

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

class StorageSpecifierCollector final {

  SrcLoc loc;
  StorageSpecifierKind storageKind;
  StorageDurationKind durationKind;

private:
  void AddStorageSpecifier(StorageSpecifierKind inputKind, SrcLoc inputLoc) {
    storageKind = inputKind;
    loc = inputLoc;
  }
  void AddStorageDurationKind(StorageDurationKind inputKind) {
    durationKind = inputKind;
  }

public:
  StorageSpecifierCollector()
      : storageKind(StorageSpecifierKind::None),
        durationKind(StorageDurationKind::None) {}

public:
  StorageSpecifierKind GetStorageKind() { return storageKind; }
  StorageDurationKind GetDurationKind() { return durationKind; }

  void AddExtern(SrcLoc loc) {
    AddStorageSpecifier(StorageSpecifierKind::Extern, loc);
  }
  bool HasExtern() {
    return (storageKind == StorageSpecifierKind::Extern && loc.isValid());
  }

  void AddStatic(SrcLoc loc) {
    AddStorageSpecifier(StorageSpecifierKind::Static, loc);
  }
  bool HasStatic() {
    return (storageKind == StorageSpecifierKind::Static && loc.isValid());
  }

  void AddRegister(SrcLoc loc) {
    AddStorageSpecifier(StorageSpecifierKind::Register, loc);
  }
  bool HasRegister() {
    return (storageKind == StorageSpecifierKind::Register && loc.isValid());
  }
  bool HasAny() {
    /// TODO: Consider auto
    return (HasExtern() || HasStatic() || HasRegister());
  }

  SrcLoc GetLoc() { return loc; }

  void AddFullExpressionStorageDuration() {
    AddStorageDurationKind(StorageDurationKind::FullExpression);
  }
  void AddAutomaticStorageDuration() {
    AddStorageDurationKind(StorageDurationKind::Automatic);
  }
  void AddThreadStorageDuration() {
    AddStorageDurationKind(StorageDurationKind::Thread);
  }
  void AddStaticStorageDuration() {
    AddStorageDurationKind(StorageDurationKind::Static);
  }
  void AddDynamicStorageDuration() {
    AddStorageDurationKind(StorageDurationKind::Dynamic);
  }
  void Apply();
};

class AccessSpecifierCollector final {
  SrcLoc loc;
  AccessLevel level;

private:
  void AddAccessLevel(AccessLevel inputLevel, SrcLoc inputLoc) {
    assert(level == AccessLevel::None);
    level = inputLevel;
    loc = inputLoc;
  }

public:
  AccessSpecifierCollector() : level(AccessLevel::None) {}

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
class UsingSpecifierCollector final {
  SrcLoc loc;

public:
  UsingSpecifierCollector() : loc(SrcLoc()) {}

public:
  void AddUsing(SrcLoc inputLoc) { loc = inputLoc; }
  bool HasUsing() { return loc.isValid(); }
  SrcLoc GetLoc() { return loc; }
};

class DeclSpecifierCollector {

  AttributeCollector attributeCollector;

  StorageSpecifierCollector storageSpecifierCollector;
  FunctionSpecifierCollector functionSpecifierCollector;
  AccessSpecifierCollector accessSpecifierCollector;

  TypeSpecifierCollector typeSpecifierCollector;
  TypeQualifierCollector typeQualifierCollector;
  TypeThunkCollector typeChunkCollector;
  TypeOperatorCollector typeOperatorCollector;

  UsingSpecifierCollector usingSpecifierCollector;

  // DeclNameLoc
  DeclName name;
  SrcLoc nameLoc;

private:
  DeclSpecifierCollector(const DeclSpecifierCollector &) = delete;
  void operator=(const DeclSpecifierCollector &) = delete;

public:
  DeclSpecifierCollector();

public:
  UsingSpecifierCollector &GetUsingSpecifierCollector() {
    return usingSpecifierCollector;
  }
  const UsingSpecifierCollector &GetUsingSpecifierCollector() const {
    return usingSpecifierCollector;
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

  AccessSpecifierCollector &GetAccessSpecifierCollector() {
    return accessSpecifierCollector;
  }

  const AccessSpecifierCollector &GetAccessSpecifierCollector() const {
    return accessSpecifierCollector;
  }

  AttributeCollector &GetAttributeCollector() { return attributeCollector; }
  const AttributeCollector &GetAttributeCollector() const {
    return attributeCollector;
  }

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
  TypeOperatorCollector &GetTypeOperatorCollector() {
    return typeOperatorCollector;
  }
  const TypeOperatorCollector &GetTypeOperatorCollector() const {
    return typeOperatorCollector;
  }

  void SetDeclName(DeclName inputName) { name = inputName; }
  DeclName GetDeclName() { return name; }

  void SetDeclNameLoc(SrcLoc inputLoc) { nameLoc = inputLoc; }
  SrcLoc GetDeclNameLoc() { return nameLoc; }

public:
  void Apply(Decl *d);
};

} // namespace stone
#endif
