#ifndef STONE_AST_DECLSTATE_H
#define STONE_AST_DECLSTATE_H

#include "stone/AST/ASTAllocation.h"
#include "stone/AST/Decl.h"
#include "stone/AST/DeclState.h"
#include "stone/AST/Property.h"
#include "stone/AST/TypeAlignment.h"

namespace stone {

class alignas(1 << DeclAlignInBits) DeclState final : ASTAllocation<DeclState> {
  friend class ASTContext;

public:
  Decl *D = nullptr;
  SrcLoc kindLoc;
  // DeclProperties

  // DeclAttributeList attributes;
  // DeclModifierList modifiers;
  // DeclTemplateList templates;

public:
  DeclState() = default;

  explicit DeclState(Decl *decl) : D(decl) {}
  // Overload the bool operator
  explicit operator bool() const { return D != nullptr; }

public:
  void SetDecl(Decl *decl) { D = decl; }
  Decl *GetDecl() { return D; }

private:
  // Direct comparison is disabled for states
  void operator==(DeclState D) const = delete;
  void operator!=(DeclState D) const = delete;
};

} // namespace stone
#endif
