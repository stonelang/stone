#ifndef STONE_PARSE_PARSING_DECL_SPEC_H
#define STONE_PARSE_PARSING_DECL_SPEC_H

#include "stone/Parse/Parser.h"
#include "stone/Parse/ParsingTypeSpec.h"

#include "llvm/ADT/TinyPtrVector.h"
#include "llvm/Support/Timer.h"

#include <memory>

namespace stone {

/// To assist debugging parser crashes, tell us the location of the
/// current curTok.
class ParsingPrettyStackTrace final : public llvm::PrettyStackTraceEntry {
  Parser &parser;

public:
  explicit ParsingPrettyStackTrace(Parser &parser);
  void print(llvm::raw_ostream &out) const override;
};

class ParsingDeclOptions final {
  unsigned storage;

public:
  /// Flags that control the parsing of declarations.
  enum Flags {
    Default = 0,
    AllowTopLevel = 1 << 1,
    HasContainerType = 1 << 2,
    AllowDestructor = 1 << 3,
    AllowEnumElement = 1 << 4,
    InInterface = 1 << 5,
    InStruct = 1 << 6,
    InEnum = 1 << 7,
    IsBuiltinType = 1 << 8,
    IsIdentifierType = 1 << 9,
  };

public:
  ParsingDeclOptions() {}

public:
  void AddAllowTopLevel() { storage |= Flags::AllowTopLevel; }
  bool HasAllowTopLevel() const { return storage & Flags::AllowTopLevel; }
  ///\Has only AllowTopLevel
  bool IsAllowTopLevelOnly() const { return storage == Flags::AllowTopLevel; }
  ///\Clear AllowTopLevel
  void ClearAllowTopLevel() { storage &= ~Flags::AllowTopLevel; }

public:
  void AddIsBuiltinType() { storage |= Flags::IsBuiltinType; }
  bool HasIsBuiltinType() const { return storage & Flags::IsBuiltinType; }
  ///\Has only AllowTopLevel
  bool IsBuiltinTypeOnly() const { return storage == Flags::IsBuiltinType; }
  ///\Clear AllowTopLevel
  void ClearIsBuiltinType() { storage &= ~Flags::IsBuiltinType; }
};

class ParsingASTScope final : public ASTScope {
  Parser &currentParser;

public:
  ParsingASTScope(const ParsingASTScope &) = delete;
  void operator=(const ParsingASTScope &) = delete;

public:
  ParsingASTScope(Parser &currentParser, ASTScopeKind kind);
  ~ParsingASTScope();

public:
  Parser &GetCurrentParser() { return currentParser; }
};

/// Describes a parsed declaration name.
struct ParsingDeclName final {
  /// The name of the context of which the corresponding entity should
  /// become a member.
  StringRef ContextName;

  /// The base name of the declaration.
  StringRef BaseName;

  /// The argument labels for a function declaration.
  SmallVector<StringRef, 4> ArgumentLabels;

  /// Whether this is a function name (vs. a value name).
  bool IsFunctionName = false;

  bool IsSubscript = false;

  /// For a declaration name that makes the declaration into an
  /// instance member, the index of the "Self" parameter.
  std::optional<unsigned> ThisIndex;

  /// Determine whether this is a valid name.
  explicit operator bool() const { return !BaseName.empty(); }

  /// Whether this declaration name turns the declaration into a
  /// member of some named context.
  bool isMember() const { return !ContextName.empty(); }

  /// Whether the result is translated into an instance member.
  bool isInstanceMember() const {
    return isMember() && static_cast<bool>(ThisIndex);
  }

  /// Whether the result is translated into a static/class member.
  bool isClassMember() const {
    return isMember() && !static_cast<bool>(ThisIndex);
  }

  /// Whether this is an operator.
  bool isOperator() const { return Lexer::isOperator(BaseName); }

  /// Form a declaration name from this parsed declaration name.
  DeclName FormDeclName(ASTContext &ctx, bool isSubscript = false,
                        bool isCxxClassTemplateSpec = false) const;
};

class ParsingDeclSpecLoc final {
  SrcLoc Loc;
  llvm::SmallVector<SrcLoc, 5> overflow;

  void AddLoc(SrcLoc loc) {
    if (overflow.size() == 0) {
      Loc = loc;
    } else {
      overflow.push_back(loc);
    }
  }

public:
  ParsingDeclSpecLoc() : Loc(SrcLoc()) {}

public:
  bool HasLoc() const { return Loc.isValid(); }
  void ClearLoc() { Loc = SrcLoc(); }
  void SetLoc(SrcLoc loc) { AddLoc(loc); }
  SrcLoc GetLoc() { return Loc; }
  bool HasOverflow() const { return (overflow.size() > 0); }
  llvm::ArrayRef<SrcLoc> GetOverflow() { return overflow; }
};

class ParsingVisibilitySpecLoc {};

class ParsingDeclSpec final {
  Parser &currentParser;

  /// The location of the import specifier
  ParsingDeclSpecLoc importLoc;

  /// The location of the const qualifier
  ParsingDeclSpecLoc constLoc;

  /// The location of the restrict qualifier
  ParsingDeclSpecLoc restrictLoc;

  //// The location of the volatiile qualifier
  ParsingDeclSpecLoc volatileLoc;

  /// The location of the stone qualifier
  ParsingDeclSpecLoc stoneLoc;

  /// The location of the stone qualifier
  ParsingDeclSpecLoc mutableLoc;

  /// The location of the public visibility
  ParsingDeclSpecLoc publicVisibilityLoc;

  /// The location of the internal visibility
  ParsingDeclSpecLoc internalVisibilityLoc;

  /// The location of the private visibility
  ParsingDeclSpecLoc privateVisibilityLoc;

  /// The type that we are parsing
  ParsingTypeSpec *parsingTypeSpec = nullptr;

  /// The parsing decl options
  ParsingDeclOptions parsingDeclOpts;

public:
  Identifier basicName;

  ParsingDeclSpecLoc basicNameLoc;

  /// The name of the decl that ware parsing
  DeclName declName;
  ///
  ParsingDeclSpecLoc declNameLoc;

public:
  ParsingDeclSpec(const ParsingDeclSpec &) = delete;
  void operator=(const ParsingDeclSpec &) = delete;

public:
  ParsingDeclSpec(Parser &currentParser) : currentParser(currentParser) {}
  ~ParsingDeclSpec();

public:
  bool HasParsingTypeSpec() const { return parsingTypeSpec != nullptr; }
  ParsingTypeSpec *GetParsingTypeSpec() { return parsingTypeSpec; }
  void SetParsingTypeSpec(ParsingTypeSpec *typeSpec) {
    parsingTypeSpec = typeSpec;
  }

  ParsingFunTypeSpec *GetParsingFunTypeSpec() {
    assert(HasParsingTypeSpec());
    return llvm::cast<ParsingFunTypeSpec>(GetParsingTypeSpec());
  }

  ParsingBuiltinTypeSpec *GetParsingBuiltinTypeSpec() {
    assert(HasParsingTypeSpec());
    return llvm::cast<ParsingBuiltinTypeSpec>(GetParsingTypeSpec());
  }

  Parser &GetCurrentParser() { return currentParser; }
  ParsingDeclOptions &GetParsingDeclOptions() { return parsingDeclOpts; }

  void SetDeclName(DeclName dn) { declName = dn; }
  DeclName GetDeclName() { return declName; }
  ParsingDeclSpecLoc GetDeclNameLoc() { return declNameLoc; }

public:
  ParsingDeclSpecLoc GetConstQualifier() { return constLoc; }
  ParsingDeclSpecLoc GetRestrictQualifier() { return restrictLoc; }
  ParsingDeclSpecLoc GetVolatileQualifier() { return volatileLoc; }
  ParsingDeclSpecLoc GetStoneQualifier() { return stoneLoc; }
  ParsingDeclSpecLoc GetMutableQualifier() { return mutableLoc; }

public:
  ParsingDeclSpecLoc GetPublicVisibility() { return publicVisibilityLoc; }
  ParsingDeclSpecLoc GetInternalVisibility() { return internalVisibilityLoc; }
  ParsingDeclSpecLoc GetPrivateVisibility() { return privateVisibilityLoc; }

  ParsingDeclSpecLoc GetImport() { return importLoc; }

public:
  void StripQualSpecs() {
    constLoc.ClearLoc();
    restrictLoc.ClearLoc();
    volatileLoc.ClearLoc();
    stoneLoc.ClearLoc();
    mutableLoc.ClearLoc();
  }

public:
  unsigned GetFastTypeQualifierFlags();

  /// Apply the collected qualifiers to the given type.
  QualType ApplyQualSpecs(Type *typePtr);

  /// Apply the collected qualifiers to the given type.
  void ApplyQualSpecs(QualType &ty);

  QualSpecs GetQualSpecs() const;

  bool HasQualifierOverflow() {
    if (GetConstQualifier().HasOverflow() ||
        GetRestrictQualifier().HasOverflow() ||
        GetVolatileQualifier().HasOverflow() ||
        GetStoneQualifier().HasOverflow() ||
        GetMutableQualifier().HasOverflow()) {
      return true;
    }
    return false;
  }
  bool HasVisibilityOverflow() {

    if (GetPublicVisibility().HasOverflow() ||
        GetRestrictQualifier().HasOverflow() ||
        GetInternalVisibility().HasOverflow() ||
        GetPrivateVisibility().HasOverflow()) {
      return true;
    }
    return false;
  }
};

} // namespace stone

#endif