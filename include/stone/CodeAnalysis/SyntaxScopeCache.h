#ifndef STONE_CODEANALYSIS_SYNTAXSCOPECACHE_H
#define STONE_CODEANALYSIS_SYNTAXSCOPECACHE_H

#include "stone/Syntax/SyntaxScope.h"
namespace stone {
namespace syn {

class SyntaxScopeCache final {
  enum { MaxSize = 16 };
  unsigned curSize;

public:
  SyntaxScopeCache() {}
  ~SyntaxScopeCache() {}

public:
  void AddScope(SyntaxScope *scope);
  SyntaxScope *GetScope();
};
} // namespace syn
} // namespace stone
#endif