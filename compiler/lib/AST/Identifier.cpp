#include "stone/AST/Identifier.h"

#include <cassert>
#include <cstdio>
#include <cstring>
#include <string>

#include "stone/Basic/LangOptions.h"
#include "stone/Basic/TokenKind.h"
#include "clang/Basic/CharInfo.h"
#include "llvm/ADT/DenseMapInfo.h"
#include "llvm/ADT/FoldingSet.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Allocator.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
// #include "stone/AST/OperatorKind.h"
// #include "stone/AST/Specifiers.h"

using namespace stone;

// /// Returns true if the identifier is a keyword
// bool Identifier::IsKeyword(const LangOptions &systemOpts) const {
//   switch (ty) {
// #define KEYWORD(NAME, FLAG) \
//   case tok::kw_##NAME: \
//     return GetKeywordStatus(systemOpts, FLAG) == KeywordStatus::On;
// #include "stone/Basic/TokenKind.def"
//   default:
//     return false;
//   }
// }

// // TODO:
// bool Identifier::IsIdentifier(llvm::StringRef identifier) {
//   assert(false && "TODO");
//   return false;
// }

// IdentifierTable::IdentifierTable(const LangOptions &systemOpts)
//     : systemOpts(systemOpts) {
//   AddKeywords(systemOpts);
// }

// static void AddKeyword(llvm::StringRef keyword, tok kind, unsigned flag,
//                        const LangOptions &systemOpts, IdentifierTable &table)
//                        {
//   auto status = GetKeywordStatus(systemOpts, flag);
//   if (status == KeywordStatus::Off) {
//     return;
//   }
//   auto &identifier = table.Get(
//       keyword, status == KeywordStatus::Reserved ? tok::identifier : kind);

//   identifier.SetIsKeywordReserved(status == KeywordStatus::Reserved);
// }

// void IdentifierTable::AddKeywords(const LangOptions &LangOpts) {
//   // Add keywords and tokens for the current language.
// #define KEYWORD(NAME, FLAG) \
//   AddKeyword(llvm::StringRef(#NAME), tok::kw_##NAME, FLAG, systemOpts,
//   *this);
// #include "stone/Basic/TokenKind.def"
// }

IdentifierTable::IdentifierTable(llvm::BumpPtrAllocator &allocator)
    : entries(allocator) {
  // AddKeywords(langOpts);
}

Identifier IdentifierTable::GetIdentifier(llvm::StringRef identifierStr) const {

  if ((identifierStr.data() != nullptr) && !identifierStr.empty() &&
      identifierStr.size() > 0) {
    auto pair = std::make_pair(identifierStr, Identifier::Aligner());
    auto first = entries.insert(pair).first;
    return Identifier(first->getKeyData());
  }
  return Identifier(nullptr);
}

//===----------------------------------------------------------------------===//
// Stats
//===----------------------------------------------------------------------===//
/// PrintStats - Print statistics about how well the identifier table is doing
/// at hashing identifiers.
// void IdentifierTableStats::Print(ColorStream &stream) {
//  unsigned numBuckets = table.symbols.getNumBuckets();
//  unsigned numIdentifiers = table.symbols.getNumItems();
//  unsigned numEmptyBuckets = numBuckets - numIdentifiers;
//  unsigned averageIdentifierSize = 0;
//  unsigned maxIdentifierLength = 0;

// // TODO: Figure out maximum times an identifier had to probe for -stats.
// for (llvm::StringMap<Identifier *, llvm::BumpPtrAllocator>::const_iterator
//          I = table.symbols.begin(),
//          E = table.symbols.end();
//      I != E; ++I) {
//   unsigned idLen = I->getKeyLength();
//   averageIdentifierSize += idLen;
//   if (maxIdentifierLength < idLen)
//     maxIdentifierLength = idLen;
// }

// stream << GetName() << '\n';

// fprintf(stderr, "# Identifiers:   %d\n", numIdentifiers);
// fprintf(stderr, "# Empty Buckets: %d\n", numEmptyBuckets);
// fprintf(stderr, "Hash density (#identifiers per bucket): %f\n",
//         numIdentifiers / (double)numBuckets);
// fprintf(stderr, "Ave identifier length: %f\n",
//         (averageIdentifierSize / (double)numIdentifiers));
// fprintf(stderr, "Max identifier length: %d\n", maxIdentifierLength);

// // Compute statistics about the memory allocated for identifiers.
// table.symbols.getAllocator().PrintStats();
//}
