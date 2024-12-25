#include "stone/AST/ASTContext.h"
#include "stone/Basic/LangOptions.h"

#include "clang/Basic/CharInfo.h"
#include "llvm/ADT/DenseMapInfo.h"
#include "llvm/ADT/FoldingSet.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Allocator.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

#include <cassert>
#include <cstdio>
#include <cstring>
#include <string>

using namespace stone;

Identifier ASTContext::GetIdentifier(llvm::StringRef identifierText) const {

  if ((identifierText.data() != nullptr) && !identifierText.empty() &&
      identifierText.size() > 0) {
    auto pair = std::make_pair(identifierText, Identifier::Aligner());
    auto first = identifierTable.insert(pair).first;
    return Identifier(first->getKeyData());
  }
  return Identifier(nullptr);
}
