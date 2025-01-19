#ifndef STONE_AST_DECLSTATE_H
#define STONE_AST_DECLSTATE_H

#include "stone/AST/ASTAllocation.h"
#include "stone/AST/DeclState.h"
#include "stone/AST/Property.h"
#include "stone/AST/TypeAlignment.h"

#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/Support/Allocator.h"
#include <cassert>

namespace stone {
class ASTContext;

enum class DeclStateKind : uint8_t {
#undef DECL_KEYWORD
#define DECL_KEYWORD(name, pretty) pretty,
#include "stone/Basic/TokenKind.def"
  Invalid // Default for unknown or unsupported tokens
};

DeclStateKind GetDeclStateKindFor(tok kind);

class alignas(1 << DeclAlignInBits) DeclState final : ASTAllocation<DeclState> {
  friend ASTContext;

  DeclStateKind kind;

  // Identifier declIdentifier;

  PropertyCollector<DeclProperty> properties;

  // The ASTContext associated with this DeclSate
  ASTContext *AC = nullptr;

  // Direct comparison is disabled for states
  void operator==(DeclState D) const = delete;
  void operator!=(DeclState D) const = delete;

public:
  DeclState(ASTContext *AC) : AC(AC) {}

public:
  DeclStateKind GetDeclStateKind() { return kind; }
  ASTContext *GetASTContext() { return AC; }
  bool HasASTContext() { return AC != nullptr; }
  PropertyCollector<DeclProperty> &GetProperties() { return properties; }

  bool HasProperty(PropertyKind kind) const {
    return properties.HasProperty(kind);
  }
  void AddProperty(PropertyKind kind, DeclProperty *property) {
    properties.AddProperty(kind, property);
  }
};

} // namespace stone
#endif
