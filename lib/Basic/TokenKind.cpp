
#include "stone/Basic/Token.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/ErrorHandling.h"

using namespace stone;

static const char *const TokenNames[] = {
#define TOK(X) #X,
#define KEYWORD(X) #X,
#include "stone/Basic/TokenKind.def"
    nullptr};

const char *Token::GetName() { return stone::GetTokenNameByKind(kind); }

bool stone::IsTokenTextDetermined(tok kind) { return false; }

const char *stone::GetTokenNameByKind(tok kind) {
  if (static_cast<uint8_t>(kind) < static_cast<uint8_t>(tok::LAST))
    return TokenNames[static_cast<uint8_t>(kind)];

  llvm_unreachable("Unknown TokenType");
}

void stone::DumpTokenKind(llvm::raw_ostream &os, tok ty) {}
