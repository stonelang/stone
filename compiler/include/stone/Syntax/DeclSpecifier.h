#ifndef STONE_SYNTAX_DECLSPECIFIER_H
#define STONE_SYNTAX_DECLSPECIFIER_H

#include "stone/Syntax/Attribute.h"
#include "stone/Syntax/Specifier.h"
#include "stone/Syntax/Template.h"

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
struct DeclaratorChunk final {

  DeclaratorChunkKind kind;

  DeclaratorChunk(){};
};

enum class DeclaratorScopeKind {
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
  TypeSpecifierKind kind;

public:
  TypeSpecifierContext() : kind(TypeSpecifierKind::None) {}

public:
  bool SetTypeSpeciferKind(TypeSpecifierKind anyKind, SrcLoc loc,
                           const char *&prevTypeSpecifier, Diag<> diagID,
                           Decl *rep, bool owned);

  // bool SetFunctionSpecifierInline(SourceLocation Loc, const char *&prevSpec,
  //                            unsigned &DiagID);

  // bool setFunctionSpecifierForceInline(SourceLocation Loc, const char
  // *&PrevSpec,
  //                                 unsigned &DiagID);

public:
  TypeSpecifierKind GetKind() { return kind; }

  bool IsBasicType();
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
};

class Declarator {

  const DeclSpecifier &declSpecifier;
  ScopeSpecifier scopeSpecifier;

  /// Where we are parsing this declarator.
  DeclaratorScopeKind declaratorScopeKind;

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
  Declarator(const DeclSpecifier &declSpecifier)
      : declSpecifier(declSpecifier) {}

public:
  /// getDeclSpec - Return the declaration-specifier that this declarator was
  /// declared with.
  const DeclSpecifier &GetDeclSpecifier() const { return declSpecifier; }
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
