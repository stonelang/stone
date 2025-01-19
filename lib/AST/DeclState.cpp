#include "stone/AST/DeclState.h"

using namespace stone;

DeclStateID stone::GetDeclStateKindFor(tok kind) {
  switch (kind) {
#undef DECL_KEYWORD
#define DECL_KEYWORD(name, pretty)                                             \
  case tok::name:                                                              \
    return DeclStateID::pretty;
#include "stone/Basic/TokenKind.def"
  }
  assert(false && "Unable to match the token-kind with a pretty token-kind" !);
}