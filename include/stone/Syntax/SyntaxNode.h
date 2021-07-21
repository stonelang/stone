#ifndef STONE_SYNTAX_NODE_H
#define STONE_SYNTAX_NODE_H

namespace stone {
namespace syn {

enum class SyntaxNodeKind { Decl, Smt, Expr };
class SyntaxNode {
  SyntaxNodeKind kind;

public:
};

} // namespace syn
} // namespace stone
#endif
