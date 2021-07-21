#include "stone/Basic/TokenType.h"
#include "llvm/ADT/StringRef.h"

using namespace stone;

bool tk::IsTokenTextDetermined(tk::Type ty) { return false; }

llvm::StringRef tk::GetTokenText(tk::Type ty) { return ""; }

void tk::DumpTokenKind(llvm::raw_ostream &os, tk::Type ty) {}
