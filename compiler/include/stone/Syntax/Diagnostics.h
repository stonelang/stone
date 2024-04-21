#ifndef STONE_SYNTAX_DIAGNOSTICS_H
#define STONE_SYNTAX_DIAGNOSTICS_H

#include "stone/Syntax/Identifier.h"
#include "stone/Syntax/TypeLoc.h"

#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/StringSet.h"
#include "llvm/Support/Allocator.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/SaveAndRestore.h"
#include "llvm/Support/VersionTuple.h"

namespace clang {
class NamedDecl;
}

namespace stone {
class Decl;
class ConstructorDecl;
class FuncDecl;
class AliasDecl;
class ValueDecl;
class DeclAttribute;
class DiagnosticEngine;
class Identifier;
class GeneratedSourceInfo;
class SrcMgr;
class SourceFile;

namespace diags {

enum class DescriptivePatternKind : uint8_t;
enum class SelfAccessKind : uint8_t;
enum class ReferenceOwnership : uint8_t;
enum class StaticSpellingKind : uint8_t;
enum class DescriptiveDeclKind : uint8_t;
enum class DeclAttrKind : unsigned;
enum class StmtKind;

// Enumeration describing all of possible diagnostics.
///
/// Each of the diagnostics described in Diagnostics.def has an entry in
/// this enumeration type that uniquely identifies it.
enum class DiagID : uint32_t;

/// Describes a diagnostic along with its argument types.
///
/// The diagnostics header introduces instances of this type for each
/// diagnostic, which provide both the set of argument types (used to
/// check/convert the arguments at each call site) and the diagnostic ID
/// (for other information about the diagnostic).
template <typename... ArgTypes> struct Diag {
  /// The diagnostic ID corresponding to this diagnostic.
  DiagID ID;
};

namespace detail {
/// Describes how to pass a diagnostic argument of the given type.
///
/// By default, diagnostic arguments are passed by value, because they
/// tend to be small. Larger diagnostic arguments
/// need to specialize this class template to pass by reference.
template <typename T> struct PassArgument {
  typedef T type;
};
} // namespace detail

template <class... ArgTypes>
using DiagArgTuple =
    std::tuple<typename detail::PassArgument<ArgTypes>::type...>;

/// A family of wrapper types for compiler data types that forces its
/// underlying data to be formatted with full qualification.
///
/// So far, this is only useful for \c Type, hence the SFINAE'ing.
template <typename T, typename = void> struct FullyQualified {};

template <typename T>
struct FullyQualified<
    T,
    typename std::enable_if<std::is_convertible<T, stone::Type>::value>::type> {
  stone::Type t;

public:
  FullyQualified(T t) : t(t){};
  stone::Type getType() const { return t; }
};

enum class DiagnosticArgumentKind {
  String,
  Integer,
  Unsigned,
  Identifier,
  Decl,
  Type,
  VersionTuple,
  Diagnostic,
  ClangDecl
};
enum class RequirementKind : uint8_t;

/// Variant type that holds a single diagnostic argument of a known
/// type.
///
/// All diagnostic arguments are converted to an instance of this class.
class DiagnosticArgument final {
  DiagnosticArgumentKind Kind;
  union {
    int IntegerVal;
    unsigned UnsignedVal;
    StringRef StringVal;
    Identifier IdentifierVal;
    const Decl *TheDecl;
    Type TypeVal;
  };

public:
  DiagnosticArgument(StringRef S)
      : Kind(DiagnosticArgumentKind::String), StringVal(S) {}

  DiagnosticArgument(int I)
      : Kind(DiagnosticArgumentKind::Integer), IntegerVal(I) {}

  DiagnosticArgument(unsigned I)
      : Kind(DiagnosticArgumentKind::Unsigned), UnsignedVal(I) {}

  DiagnosticArgument(Identifier I)
      : Kind(DiagnosticArgumentKind::Identifier), IdentifierVal(Identifier(I)) {
  }

  DiagnosticArgument(const Decl *VD)
      : Kind(DiagnosticArgumentKind::Decl), TheDecl(VD) {}

  DiagnosticArgument(Type T) : Kind(DiagnosticArgumentKind::Type), TypeVal(T) {}

  /// Initializes a diagnostic argument using the underlying type of the
  /// given enum.
  template <
      typename EnumType,
      typename std::enable_if<std::is_enum<EnumType>::value>::type * = nullptr>
  DiagnosticArgument(EnumType value)
      : DiagnosticArgument(
            static_cast<typename std::underlying_type<EnumType>::type>(value)) {
  }

  DiagnosticArgumentKind getKind() const { return Kind; }

  StringRef getAsString() const {
    assert(Kind == DiagnosticArgumentKind::String);
    return StringVal;
  }

  int getAsInteger() const {
    assert(Kind == DiagnosticArgumentKind::Integer);
    return IntegerVal;
  }

  unsigned getAsUnsigned() const {
    assert(Kind == DiagnosticArgumentKind::Unsigned);
    return UnsignedVal;
  }

  Identifier getAsIdentifier() const {
    assert(Kind == DiagnosticArgumentKind::Identifier);
    return IdentifierVal;
  }

  const Decl *getAsDecl() const {
    assert(Kind == DiagnosticArgumentKind::Decl);
    return TheDecl;
  }

  Type getAsType() const {
    assert(Kind == DiagnosticArgumentKind::Type);
    return TypeVal;
  }
};

/// Describes the current behavior to take with a diagnostic.
/// Ordered from most severe to least.
enum class DiagnosticBehavior : uint8_t {
  Unspecified = 0,
  Fatal,
  Error,
  Warning,
  Remark,
  Note,
  Ignore,
};

} // namespace diags

} // namespace stone

#endif