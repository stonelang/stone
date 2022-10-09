#ifndef STONE_SYNTAX_PATTERN_H
#define STONE_SYNTAX_PATTERN_H

#include "stone/Syntax/Specifier.h"
#include "stone/Syntax/SyntaxAllocation.h"

namespace stone {
namespace syn {
constexpr size_t TypePatternAlignInBits = 3;

// /// TODO: I think you can replace this with ScopeKind
// enum class DeclaratorScopeKind {
//   None = 0,
//   SyntaxFile,          // File scope declaration.
//   FunctionSignature,   // Within a function prototype.
//   TypeName,            // Abstract VariableName for types.
//   FunctionalCast,      // Type in a C++ functional cast expression.
//   Member,              // Struct/Union field.
//   Block,               // Declaration within a block in a function.
//   ForInit,             // Declaration within first part of a for loop.
//   SelectionInit,       // Declaration within optional init stmt of if/switch.
//   Condition,           // Condition declaration in a C++ if/switch/while/for.
//   TemplateParam,       // Within a template parameter list.
//   New,                 // new-expression.
//   BlockLiteral,        // Block literal VariableName.
//   LambdaExpr,          // Lambda-expression VariableName.
//   LambdaExprParameter, // Lambda-expression parameter VariableName.
//   ConversionId,        // Conversion-type-id.
//   TrailingReturn,      // Trailing-type-specifier.
//   TrailingReturnVar,   // Trailing-type-specifier for VariableName.
//   TemplateArg,         // Any template argument (in template argument list).
//   TemplateTypeArg,     // Template type argument (in default argument).
//   AliasDecl,           // Alias-declaration.
//   AliasTemplate,       // Alias-declaration template.
//   RequiresExpr         // Requires-expression.
// };

enum class TypePatternKind {
  None,
  Direct,
  Pointer,
  Reference,
  Array,
  BlockPointer,
  MemberPointer,
  Paren,
  Pipe,
};

class alignas(8) TypePattern : syn::SyntaxAllocation<TypePattern> {
  SrcLoc loc;
  TypePatternKind kind;

public:
  TypePattern(TypePatternKind kind, SrcLoc inputLoc)
      : kind(kind), loc(inputLoc) {}

public:
  TypePatternKind GetKind() const { return kind; }
  SrcLoc GetLoc() { return loc; }
};

class DirectTypePattern final : public TypePattern {
public:
  DirectTypePattern() : TypePattern(TypePatternKind::Direct, SrcLoc()) {}

public:
  static DirectTypePattern Create();
};

class PointerTypePattern final : public TypePattern {
public:
  PointerTypePattern(SrcLoc loc) : TypePattern(TypePatternKind::Pointer, loc) {}

public:
  static PointerTypePattern Create(SrcLoc loc);
};

class MemberPointerTypePattern final : public TypePattern {
public:
  MemberPointerTypePattern(SrcLoc loc)
      : TypePattern(TypePatternKind::MemberPointer, loc) {}

public:
  static MemberPointerTypePattern Create(SrcLoc loc);
};

class ReferenceTypePattern final : public TypePattern {
public:
  ReferenceTypePattern(SrcLoc loc)
      : TypePattern(TypePatternKind::Reference, loc) {}

public:
  static ReferenceTypePattern Create(SrcLoc loc);
};

class ArrayTypePattern final : public TypePattern {
public:
  ArrayTypePattern(SrcLoc loc) : TypePattern(TypePatternKind::Array, loc) {}

public:
  static ArrayTypePattern Create(SrcLoc loc);
};

class ParenTypePattern final : public TypePattern {
public:
  ParenTypePattern(SrcLoc loc) : TypePattern(TypePatternKind::Paren, loc) {}

public:
  static ParenTypePattern Create(SrcLoc loc);
};

// class FunctionTypePattern final : public TypePattern {
// public:
//   FunctionTypePattern() :
//   TypePattern(TypePatternKind::Function) {}

// public:
//   static FunctionTypePattern Create();
// };

class TypePatternCollector {

  const TypeSpecifierCollector &specifierCollector;

  /// This holds each type-patter that the type-specifer includes as it is
  /// parsed.  This is pushed from the type out, which means that element
  /// #0 will be the most closely bound to the type, and
  /// patterns.back() will be the least closely bound to the type.
  llvm::SmallVector<TypePattern, 8> patterns;

  /// If this Declarator declares a template, its template parameter lists.
  // llvm::ArrayRef<TemplateParameterList *> templateParameterLists;

public:
  TypePatternCollector(const TypeSpecifierCollector &specifierCollector)
      : specifierCollector(specifierCollector) {}

public:
  const TypeSpecifierCollector &GetTypeSpecifierCollector() const {
    return specifierCollector;
  }
  bool HasTypePatterns() { return patterns.size() > 0; }

private:
  /// Add a chunk to this Declarator. Also extend the range to
  /// EndLoc, which should be the last token of the chunk.
  void AddTypePattern(const TypePattern pattern) {
    patterns.push_back(pattern);
    // TODO:
    //  if (!EndLoc.isInvalid())
    //    SetRangeEnd(EndLoc);
  }

public:
  // Direct has no source loc
  void AddDirect();
  void AddPointer(SrcLoc loc);
  void AddReference(SrcLoc loc);
  void AddArray(SrcLoc loc);
  void AddBlockPointer(SrcLoc loc);
  void AddParen(SrcLoc loc);
  void AddPipe(SrcLoc loc);

public:
  /// int** -- the '*' toucing int
  const TypePattern *GetInnermostNonParenPattern() const {
    for (unsigned i = patterns.size(), i_end = 0; i != i_end; --i) {
      if (patterns[i - 1].GetKind() != TypePatternKind::Paren) {
        return &patterns[i - 1];
      }
    }
    return nullptr;
  }

  /// int** -- the '*' farthest from int
  const TypePattern *GetOutermostNonParenPattern() const {
    for (unsigned i = 0, i_end = patterns.size(); i < i_end; ++i) {
      if (patterns[i].GetKind() != TypePatternKind::Paren) {
        return &patterns[i];
      }
    }
    return nullptr;
  }

public:
  void Verify();
};

} // namespace syn
} // namespace stone
#endif
