#ifndef STONE_SYNTAX_MEMBERTABLE_H
#define STONE_SYNTAX_MEMBERTABLE_H

#include "stone/Foreign/DeclName.h"

// #include "stone/Foreign/ClangImporter.h"
// #include "stone/Syntax/NameLookup.h"
// #include "stone/Syntax/ASTContext.h"
// #include "stone/Syntax/Scope.h"

// #include "stone/Syntax/ASTVisitor.h"
// #include "stone/Syntax/DebuggerClient.h"
// #include "stone/Syntax/ExistentialLayout.h"
// #include "stone/Syntax/LazyResolver.h"
// #include "stone/Syntax/Initializer.h"
// #include "stone/Syntax/ReferencedNameTracker.h"

// #include "stone/Basic/SourceManager.h"
// #include "stone/Basic/Statistic.h"
// #include "stone/Basic/STLExtras.h"

#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/TinyPtrVector.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iterator>
#include <string>
#include <type_traits>
#include <utility>

namespace stone {
namespace syn {

class MemberTable final {

  using Entries = llvm::DenseMap<DeclName, llvm::TinyPtrVector<ValueDecl *>>;
  Entries entries;

public:
  MemberTable(const Expr &) = delete;
  MemberTable(MemberTable &&) = delete;

  MemberTable &operator=(const MemberTable &) = delete;
  MemberTable &operator=(MemberTable &&) = delete;

public:
  MemberTable() {}

public:
  void AddMember(Decl *member);
};

} // namespace syn
} // namespace stone
#endif
