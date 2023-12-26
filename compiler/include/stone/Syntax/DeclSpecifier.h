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
class ImportSpecifierCollector final {
  SrcLoc loc;

public:
  ImportSpecifierCollector() : loc(SrcLoc()) {}

public:
  void AddImport(SrcLoc inputLoc) { loc = inputLoc; }
  bool HasImport() { return loc.isValid(); }
  SrcLoc GetLoc() { return loc; }
};

class DeclSpecifierCollector {

  AttributeSpecifierCollector attributeCollector;

  StorageSpecifierCollector storageSpecifierCollector;
  FunctionSpecifierCollector functionSpecifierCollector;
  AccessSpecifierCollector accessSpecifierCollector;

  TypeSpecifierCollector typeSpecifierCollector;
  TypeQualifierCollector typeQualifierCollector;
  TypeThunkCollector typeChunkCollector;
  TypeOperatorCollector typeOperatorCollector;

  ImportSpecifierCollector importSpecifierCollector;

  // DeclNameLoc
  DeclName name;
  SrcLoc nameLoc;

private:
  DeclSpecifierCollector(const DeclSpecifierCollector &) = delete;
  void operator=(const DeclSpecifierCollector &) = delete;

public:
  DeclSpecifierCollector();

public:
  ImportSpecifierCollector &GetImportSpecifierCollector() {
    return importSpecifierCollector;
  }
  const ImportSpecifierCollector &GetImportSpecifierCollector() const {
    return importSpecifierCollector;
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

  AttributeSpecifierCollector &GetAttributeSpecifierCollector() {
    return attributeCollector;
  }
  const AttributeSpecifierCollector &GetAttributeSpecifierCollector() const {
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
