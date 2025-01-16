#ifndef STONE_AST_DECLNAME_H
#define STONE_AST_DECLNAME_H

#include "stone/AST/Identifier.h"
#include "stone/AST/Type.h"
#include "stone/Basic/LLVM.h"
#include "stone/Basic/OperatorKind.h"

#include "llvm/ADT/FoldingSet.h"

namespace stone {
class PrintingPolicy;
class ASTContext;
class DeclName;
class DeclNameTable;

class DeclIdentifier final {
  Identifier name;
  SrcLoc loc;

public:
  DeclIdentifier() = default;
  DeclIdentifier(Identifier name, SrcLoc loc) : name(name), loc(loc) {}

public:
  bool IsValid() const { return !name.empty(); }
};

enum class DeclNameKind : uint8_t {
  Identifier = 0,
  Constructor,
  Destructor,
  Operator,
  LiteralOperator,
  UsingDirective,
  Compound // Represents names with arguments
};

class DeclName final {
  DeclNameKind kind;
  DeclIdentifier baseIdentifier;         // Name and location
  //std::vector<Identifier> argumentNames; // For compound names

  // Private constructor for special kinds
  DeclName(DeclNameKind kind, DeclIdentifier identifier)
      : kind(kind), baseIdentifier(identifier) {}

public:
  // Constructors
  DeclName() : nameKind(DeclNameKind::Identifier), baseIdentifier() {}

  explicit DeclName(DeclIdentifier identifier)
      : kind(DeclNameKind::Identifier), baseIdentifier(sidentifier) {}

public:
  // Getters
  Kind GetKind() const { return nameKind; }
  const DeclIdentifier &GetIdentifier() const { return baseIdentifier; }

  // const std::vector<Identifier> &getArgumentNames() const {
  //   return argumentNames;
  // }
  const SrcLoc &GetLoc() const { return baseIdentifier.location; }

  // Utility
  bool IsIdentifier() const { return nameKind == DeclNameKind::Identifier; }
  bool IsSpecial() const { return nameKind != DeclNameKind::Identifier; }
  bool SsValid() const { return baseIdentifier.isValid(); }

  // Comparisons
  bool operator==(const DeclName &other) const {
    return nameKind == other.nameKind &&
           baseIdentifier == other.baseIdentifier &&
           argumentNames == other.argumentNames;
  }

  bool operator!=(const DeclName &other) const { return !(*this == other); }

public:
  static DeclName CreateConstructor(SrcLoc loc) {
    return DeclName(DeclNameKind::Constructor, {"constructor", loc});
  }

  static DeclName CreateDestructor(SrcLoc loc) {
    return DeclName(DeclNameKind::Destructor, {"destructor", loc});
  }

  static DeclName CreateOperator(SrcLoc loc) {
    return DeclName(DeclNameKind::Operator, {"operator", loc});
  }

  // static DeclName CreateCompound(DeclIdentifier base,
  //                                std::vector<Identifier> args) {
  //   DeclName name(DeclNameKind::Compound, std::move(base));
  //   name.argumentNames = std::move(args);
  //   return name;
  // }
};

} // namespace stone
#endif
