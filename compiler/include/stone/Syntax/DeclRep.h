#ifndef STONE_SYNTAX_DECLREP_H
#define STONE_SYNTAX_DECLREP_H

#include "stone/Basic/OptionSet.h"
#include "stone/Syntax/Attribute.h"
#include "stone/Syntax/DeclName.h"
#include "stone/Syntax/Pattern.h"
#include "stone/Syntax/Template.h"
#include "stone/Syntax/TypeRep.h"

#include "llvm/ADT/ArrayRef.h"

namespace stone {
namespace syn {

class DeclRep {
private:
  QualifierTypeRep *qualifierTypeRep = nullptr;
  SpecifierTypeRep *functionTypeRep = nullptr;
  FunctionTypeRep *functionTypeRep = nullptr;
  // public:
  // 	AccessLevelLoc accessLevelLoc;

public:
  FunctionTypeRep *GetFunctionTypeRep() {
    assert(functionTypeRep);
    return functionTypeRep;
  }
};
} // namespace syn
} // namespace stone