
#include "stone/Core/Token.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/ErrorHandling.h"

using namespace stone;

static const char *const TokenNames[] = {
#define TOK(X) #X,
#define KEYWORD(X, Y) #X,
#include "stone/Core/TokenKind.def"
    nullptr};

const char *syn::Token::GetName() { return tk::GetName(kind); }

bool tk::IsTokenTextDetermined(tk::Kind kind) { return false; }

const char *tk::GetName(tk::Kind kind) {

  if (static_cast<uint8_t>(kind) < static_cast<uint8_t>(tk::Kind::MAX))
    return TokenNames[static_cast<uint8_t>(kind)];

  llvm_unreachable("Unknown TokenType");
}

void tk::DumpTokenKind(llvm::raw_ostream &os, tk::Kind ty) {}
