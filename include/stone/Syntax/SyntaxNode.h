#ifndef STONE_SYNTAX_SYNTAXNODE_H
#define STONE_SYNTAX_SYNTAXNODE_H

namespace stone {
namespace syn {

enum class SyntaxNodeKind : uint8_t { Decl, Smt, Expr };

class SyntaxNode {
  SyntaxNodeKind kind;

public:
};

} // namespace syn
} // namespace stone
#endif
