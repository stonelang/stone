#ifndef STONE_AST_DECL_SPECIFIER_H
#define STONE_AST_DECL_SPECIFIER_H

#include "stone/AST/Attribute.h"
#include "stone/AST/DeclName.h"
#include "stone/AST/FunctionSpecifier.h"
#include "stone/AST/Generics.h"
#include "stone/AST/StorageSpecifier.h"
#include "stone/AST/TypeChunk.h"
#include "stone/AST/TypeOperator.h"
#include "stone/AST/TypeQualifier.h"
#include "stone/AST/TypeSpecifier.h"
#include "stone/AST/Types.h"
#include "stone/Basic/OptionSet.h"

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

private:
  ImportSpecifierCollector(const ImportSpecifierCollector &) = delete;
  void operator=(const ImportSpecifierCollector &) = delete;

public:
  ImportSpecifierCollector() : loc(SrcLoc()) {}

public:
  void AddImport(SrcLoc inputLoc) { loc = inputLoc; }
  bool HasImport() { return loc.isValid(); }
  SrcLoc GetLoc() { return loc; }
};

class DeclNameCollector final {

  DeclName name;
  SrcLoc loc;

private:
  DeclNameCollector(const DeclNameCollector &) = delete;
  void operator=(const DeclNameCollector &) = delete;

public:
  DeclNameCollector() {}

public:
  void SetName(DeclName inputName) { name = inputName; }
  DeclName GetName() { return name; }

  void SetLoc(SrcLoc inputLoc) { loc = inputLoc; }
  SrcLoc GetLoc() { return loc; }
};

enum class DeclSpecifierKind {
  None = 0,
#define DECL_SPEC(DS) DS,
#include "stone/AST/DeclSpecifierKind.def"

};

class DeclSpecifierCollector {

  /// Import specifier collection
  ImportSpecifierCollector importSpecifierCollector;

  /// Attribute info collection
  AttributeSpecifierCollector attributeCollector;

  /// Storage specifier collection
  StorageSpecifierCollector storageSpecifierCollector;

  /// Function specifier collection
  FunctionSpecifierCollector functionSpecifierCollector;

  /// Access specifer collection
  AccessSpecifierCollector accessSpecifierCollector;

  /// Type specifier collection
  TypeSpecifierCollector typeSpecifierCollector;

  /// Type qualifier collection
  TypeQualifierCollector typeQualifierCollector;

  /// Type thunk collection
  TypeChunkCollector typeChunkCollector;

  /// Type operator collection
  TypeOperatorCollector typeOperatorCollector;

  /// Decl name collection
  DeclNameCollector declNameCollector;

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

  TypeChunkCollector &GetTypeChunkCollector() { return typeChunkCollector; }
  const TypeChunkCollector &GetTypeChunkCollector() const {
    return typeChunkCollector;
  }
  TypeOperatorCollector &GetTypeOperatorCollector() {
    return typeOperatorCollector;
  }
  const TypeOperatorCollector &GetTypeOperatorCollector() const {
    return typeOperatorCollector;
  }

  DeclNameCollector &GetDeclNameCollector() { return declNameCollector; }
  const DeclNameCollector &GetDeclNameCollector() const {
    return declNameCollector;
  }

public:
  void Apply(Decl *d);
};

} // namespace stone
#endif
