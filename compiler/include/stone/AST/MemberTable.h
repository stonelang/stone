#ifndef STONE_AST_MEMBERTABLE_H
#define STONE_AST_MEMBERTABLE_H

#include "stone/Foreign/DeclName.h"

// #include "stone/Foreign/ClangImporter.h"
// #include "stone/AST/NameLookup.h"
// #include "stone/AST/ASTContext.h"
// #include "stone/AST/Scope.h"

// #include "stone/AST/ASTVisitor.h"
// #include "stone/AST/DebuggerClient.h"
// #include "stone/AST/ExistentialLayout.h"
// #include "stone/AST/LazyResolver.h"
// #include "stone/AST/Initializer.h"
// #include "stone/AST/ReferencedNameTracker.h"

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

} // namespace stone
#endif
