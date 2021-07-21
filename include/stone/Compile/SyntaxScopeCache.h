#ifndef STONE_COMPILE_SYNTAX_SCOPE_CACHE_H
#define STONE_COMPILE_SYNTAX_SCOPE_CACHE_H

#include "stone/Syntax/SyntaxScope.h"
namespace stone {
namespace syn {
class SyntaxScopeCache final {
  enum { MaxSize = 16 };

public:
  SyntaxScopeCache() {}
  ~SyntaxScopeCache() {}
};
} // namespace syn
} // namespace stone
#endif