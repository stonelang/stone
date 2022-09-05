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
public:
  DeclaratorChunk(){};
  DeclaratorChunkKind kind;

public:
  struct PointerTypeInfo {};
  struct FunctionTypeInfo {};

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

class FunctionSpecifierContext final {
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
  bool HasFun() { return flags & FunctionSpecifierContext::Fun; }

  void AddInline(SrcLoc loc) { flags |= FunctionSpecifierContext::Inline; }
  bool HasInline() { return flags & FunctionSpecifierContext::Inline; }

  void AddForcedInline(SrcLoc loc) {
    flags |= FunctionSpecifierContext::ForcedInline;
  }
  bool HasForcedInline() {
    return flags & FunctionSpecifierContext::ForcedInline;
  }

  void AddVirtual(SrcLoc loc) { flags |= FunctionSpecifierContext::Virtual; }
  bool HasVirtual() { return flags & FunctionSpecifierContext::Virtual; }

  void AddNoReturn(SrcLoc loc) { flags |= FunctionSpecifierContext::NoReturn; }
  bool HasNoReturn() { return flags & FunctionSpecifierContext::NoReturn; }

  void AddIsMember() { flags |= FunctionSpecifierContext::IsMember; }
  bool HasIsMember() { return flags & FunctionSpecifierContext::IsMember; }

  SrcLoc GetFunLoc() { return funLoc; }
};

class StorageSpecifierContext final {
  StorageSpecifierKind kind;

public:
  StorageSpecifierContext() : kind(StorageSpecifierKind::None) {}

public:
  void SetKind(StorageSpecifierKind anyKind) { kind = anyKind; }

  StorageSpecifierKind GetKind() { return kind; }
};

// enum class DescriptiveDeclSpecifier {
//   None,
//   FuncitonDefinition,
//   NominalType,
//   BasicType
// }

class AccessLevelContext final {
  SrcLoc loc;
  AccessLevel level = AccessLevel::None;

private:
  void AddAccessLevel(AccessLevel inputLevel, SrcLoc inputLoc) {
    level = inputLevel;
    loc = inputLoc;
  }

public:
  void AddPublic(SrcLoc inputLoc) {
    AddAccessLevel(AccessLevel::Public, inputLoc);
  }
  void AddPrivate(SrcLoc inputLoc) {
    AddAccessLevel(AccessLevel::Private, inputLoc);
  }
  void AddInternal(SrcLoc inputLoc) {
    AddAccessLevel(AccessLevel::Internal, inputLoc);
  }
  bool HasAccessLevel() { return level != AccessLevel::None; }
  SrcLoc GetLoc() { return loc; }
};

class DeclSpecifier {

  AttributeFactory &attributeFactory;
  TypeSpecifierContext typeSpecifierContext;
  TypeQualifierCollector typeQualifierCollector;
  StorageSpecifierContext storageSpecifierContext;
  FunctionSpecifierContext functionSpecifierContext;
  AccessLevelContext accessLevelContext;

  // DescriptiveDeclSpecifier descriptiveDeclSpecifier =
  // DescriptiveDeclSpecifier::None;

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
  TypeQualifierCollector &GetTypeQualifierCollector() {
    return typeQualifierCollector;
  }
  AccessLevelContext &GetAccessLevelContext() { return accessLevelContext; }

  // void SetDescriptiveDeclSpecifier(DescriptiveDeclSpecifier descriptive){
  //   descriptiveDeclSpecifier = descriptive;

  // }
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

public:
  /// AddTypeInfo - Add a chunk to this declarator. Also extend the range to
  /// EndLoc, which should be the last token of the chunk.
  void AddTypeInfo(const DeclaratorChunk &chunk, SrcLoc endLoc) {
    declTypeInfo.push_back(chunk);
    // if (!EndLoc.isInvalid())
    //   SetRangeEnd(EndLoc);
  }
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
