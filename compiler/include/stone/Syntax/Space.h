#ifndef STONE_SYNTAX_SPACE_H
#define STONE_SYNTAX_SPACE_H

#include "stone/Syntax/Decl.h"

namespace stone {
namespace syn {

class Space final : public NamedDecl, public DeclContext {
public:
  Space(SyntaxContext &sc, DeclContext *dc, bool inline, SrcLoc startLoc,
        SrcLoc idLoc, Identifier *identifier);
};

}
#endif
