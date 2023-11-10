#ifndef STONE_AST_TYPECOLLECTOR_H
#define STONE_AST_TYPECOLLECTOR_H

#include "stone/AST/Specifier.h"
#include "stone/AST/TypeAlignment.h"
#include "stone/AST/TypeOperator.h"
#include "stone/AST/TypeQualifier.h"
#include "stone/AST/TypeSlab.h"

namespace stone {

class Type;
class TypeCollector final {
  Type ty;
  TypeSpecifierCollector typeSpecifierCollector;
  TypeQualifierCollector typeQualifierCollector;
  TypeSlabCollector typeChunkCollector;
  TypeOperatorCollector typeOperatorCollector;

public:
  TypeCollector() {}

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

  TypeSlabCollector &GetTypeSlabCollector() { return typeChunkCollector; }
  const TypeSlabCollector &GetTypeSlabCollector() const {
    return typeChunkCollector;
  }
  TypeOperatorCollector &GetTypeOperatorCollector() {
    return typeOperatorCollector;
  }
  const TypeOperatorCollector &GetTypeOperatorCollector() const {
    return typeOperatorCollector;
  }

public:
  void SetType(Type ty);
  Type GetType();

public:
  Type Apply(const stone::ASTContext &astContext, Type ty) const;
};

} // namespace stone

#endif