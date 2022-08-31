#ifndef STONE_SYNTAX_DECLSPECIFIER_H
#define STONE_SYNTAX_DECLSPECIFIER_H

#include "stone/Basic/OptionSet.h"
#include "stone/Syntax/Attribute.h"
#include "stone/Syntax/Specifier.h"
#include "stone/Syntax/Template.h"
#include "stone/Syntax/Type.h"

#include "llvm/ADT/ArrayRef.h"

namespace stone {
namespace syn {

enum class DeclaratorChunkKind {
  Pointer,
  Reference,
  Array,
  Function,
  BlockPointer,
  MemberPointer,
  Paren,
  Pipe,
};

enum class DeclaratorContextKind {
  SyntaxFile,          // File scope declaration.
  FunctionSignature,   // Within a function prototype.
  TypeName,            // Abstract declarator for types.
  FunctionalCast,      // Type in a C++ functional cast expression.
  Member,              // Struct/Union field.
  Block,               // Declaration within a block in a function.
  ForInit,             // Declaration within first part of a for loop.
  SelectionInit,       // Declaration within optional init stmt of if/switch.
  Condition,           // Condition declaration in a C++ if/switch/while/for.
  TemplateParam,       // Within a template parameter list.
  New,                 // new-expression.
  BlockLiteral,        // Block literal declarator.
  LambdaExpr,          // Lambda-expression declarator.
  LambdaExprParameter, // Lambda-expression parameter declarator.
  ConversionId,        // Conversion-type-id.
  TrailingReturn,      // Trailing-type-specifier.
  TrailingReturnVar,   // Trailing-type-specifier for variable.
  TemplateArg,         // Any template argument (in template argument list).
  TemplateTypeArg,     // Template type argument (in default argument).
  AliasDecl,           // Alias-declaration.
  AliasTemplate,       // Alias-declaration template.
  RequiresExpr         // Requires-expression.
};

struct DeclaratorChunk final {
public:
  DeclaratorChunk(){};
  DeclaratorChunkKind kind;

public:
  static DeclaratorChunk CreatePointer();
  static DeclaratorChunk CreateReference();
  static DeclaratorChunk CreateFunction();
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

class TypeSpecifierContext final {
  TypeQualifierKind typeQualifierKind;
  TypeSpecifierKind typeSpecifierKind;

public:
  TypeSpecifierContext()
      : typeQualifierKind(TypeQualifierKind::None),
        typeSpecifierKind(TypeSpecifierKind::None) {}

public:
  // bool SetTypeSpeciferKind(TypeSpecifierKind anyKind, SrcLoc loc,
  //                          const char *&prevTypeSpecifier, Diag<> diagID,
  //                          Decl *rep, bool owned);

  bool SetTypeSpecifierKind(TypeSpecifierKind kind, SrcLoc loc);
  bool HasTypeSpecifierKind() const {
    return typeSpecifierKind != TypeSpecifierKind::None;
  }

  bool SetTypeQualifierKind(TypeQualifierKind kind, SrcLoc loc);
  bool HasTypeQualifierKind() const {
    return typeQualifierKind != TypeQualifierKind::None;
  }

public:
  TypeSpecifierKind GetTypeSpecifierKind() { return typeSpecifierKind; }
  TypeQualifierKind GetTypeQualifierKind() { return typeQualifierKind; }

  bool IsBasicType();
  bool IsNominalType();
};

class FunctionSpecifierContext final {
public:
  enum Flags {
    None = 0,
    Inline = 1 << 1,
    ForcedInline = 1 << 2,
    Virtual = 1 << 3,
    NoReturn = 1 << 4
  };
  /// Options that control the parsing of declarations.
  using FunctionSpecifierOptions =
      stone::OptionSet<FunctionSpecifierContext::Flags>;

private:
  FunctionSpecifierOptions flags;

public:
  void SetFlags(FunctionSpecifierOptions inputFlags) { flags = inputFlags; }
  bool HasInline() { return flags.contains(FunctionSpecifierContext::Inline); }
};

class StorageSpecifierContext final {
  StorageSpecifierKind kind;

public:
  StorageSpecifierContext() : kind(StorageSpecifierKind::None) {}

public:
  void SetKind(StorageSpecifierKind anyKind) { kind = anyKind; }

  StorageSpecifierKind GetKind() { return kind; }
};

class DeclSpecifier {

  AttributeFactory &attributeFactory;
  TypeSpecifierContext typeSpecifierContext;
  StorageSpecifierContext storageSpecifierContext;
  FunctionSpecifierContext functionSpecifierContext;

  DeclSpecifier(const DeclSpecifier &) = delete;
  void operator=(const DeclSpecifier &) = delete;

public:
  DeclSpecifier(AttributeFactory &attributeFactory)
      : attributeFactory(attributeFactory) {}

public:
  StorageSpecifierContext &GetStorageSpeciferContext() {
    return storageSpecifierContext;
  }
  TypeSpecifierContext &GetTypeSpecifierContext() {
    return typeSpecifierContext;
  }
  FunctionSpecifierContext &GetFunctionSpecifierContext() {
    return functionSpecifierContext;
  }
};

class Declarator {

  const DeclSpecifier &declSpecifier;
  ScopeSpecifier scopeSpecifier;
  /// Where we are parsing this declarator.
  DeclaratorContextKind contextKind;

  // /// The C++17 structured binding, if any. This is an alternative to a Name.
  // DecompositionDeclarator bindingGroup;

  // /// DeclTypeInfo - This holds each type that the declarator includes as it
  // is
  /// parsed.  This is pushed from the identifier out, which means that element
  /// #0 will be the most closely bound to the identifier, and
  /// DeclTypeInfo.back() will be the least closely bound.
  llvm::SmallVector<DeclaratorChunk, 8> declTypeInfo;

  /// If this declarator declares a template, its template parameter lists.
  llvm::ArrayRef<TemplateParameterList *> templateParameterLists;

public:
  Declarator(const DeclSpecifier &declSpecifier,
             DeclaratorContextKind scopeKind)
      : declSpecifier(declSpecifier), contextKind(contextKind) {}

public:
  /// getDeclSpec - Return the declaration-specifier that this declarator was
  /// declared with.
  const DeclSpecifier &GetDeclSpecifier() const { return declSpecifier; }
  DeclaratorContextKind GetContextKind() { return contextKind; }
};

// /// A context for parsing declaration specifiers.  TODO: flesh this
// /// out, there are other significant restrictions on specifiers than
// /// would be best implemented in the parser.
// enum class DeclSpecifierContextKind {
//   Normal,         // normal context
//   Fun,
//   Struct,          // struct context, enables 'friend'
//   Type, // C++ type-specifier-seq or C specifier-qualifier-list
//   Trailing, // C++11 trailing-type-specifier in a trailing return type
//   AliasDeclaration,  // C++11 type-specifier-seq in an alias-declaration
//   TopLevel,          // top-level/namespace declaration context
//   TemplateParam,     // template parameter context
//   TemplateTypeArg,  // template type argument context
//   Condition           // condition declaration context
// };

} // namespace syn
} // namespace stone
#endif
