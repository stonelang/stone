#ifndef STONE_SYNTAX_SPECIFIER_H
#define STONE_SYNTAX_SPECIFIER_H

#include "stone/Syntax/Decl.h"

namespace stone {
namespace syn {

enum class SpaceUnitKind : uint8_t { Syntax, Builtin };

class SpaceUnit {};

class SyntaxUnit : public SpaceUnit {};
class Space final : public DeclContext, public TypeDecl {
public:
};
}
#endif
