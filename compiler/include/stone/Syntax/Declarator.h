#ifndef STONE_SYNTAX_DECLARATOR_H
#define STONE_SYNTAX_DECLARATOR_H

#include "stone/Syntax/DeclSpecifier.h"
namespace stone {
namespace syn {

constexpr size_t DeclaratorAlignInBits = 3;

class alignas(1 << DeclaratorAlignInBits) Declarator {
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
};

} // namespace syn
} // namespace stone
#endif
