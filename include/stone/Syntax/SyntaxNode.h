#ifndef STONE_SYNTAX_NODE_H
#define STONE_SYNTAX_NODE_H

namespace stone {
namespace syn {

enum class SyntaxKind { Decl, Smt, Expr };
class SyntaxNode {
  SyntaxKind kind;

public:
};

} // namespace syn
} // namespace stone
#endif
