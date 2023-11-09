#ifndef STONE_AST_TYPECOLLECTOR_H
#define STONE_AST_TYPECOLLECTOR_H

#include "stone/AST/Specifier.h"
#include "stone/AST/TypeAlignment.h"
#include "stone/AST/TypeOperator.h"
#include "stone/AST/TypeQualifier.h"
#include "stone/AST/TypeSlab.h"

namespace stone {

class QualType;
class TypeCollector final {
  QualType qualType;
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
  void SetType(QualType qualType);
  QualType GetType();

public:
  QualType Apply(const stone::ASTContext &astContext, QualType qualType) const;
};

} // namespace stone

#endif