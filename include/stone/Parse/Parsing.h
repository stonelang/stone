#ifndef STONE_PARSE_PARSING_H
#define STONE_PARSE_PARSING_H

#include "stone/AST/ASTScope.h"
#include "stone/AST/DeclState.h"
#include "stone/AST/TypeState.h"
#include "stone/Parse/Parser.h"

#include "llvm/ADT/TinyPtrVector.h"
#include "llvm/Support/Timer.h"

namespace stone {
class ParsingDeclState;
class ParsingTypeState;

/// To assist debugging parser crashes, tell us the location of the
/// current curTok.
class ParsingPrettyStackTrace final : public llvm::PrettyStackTraceEntry {
  Parser &parser;

public:
  explicit ParsingPrettyStackTrace(Parser &parser);
  void print(llvm::raw_ostream &out) const override;
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

class ParsingDeclOptions final {
  unsigned storage;

public:
  /// Flags that control the parsing of declarations.
  enum Flags {
    None = 0,
    AllowTopLevel = 1 << 1,
    InTopLevel = 1 << 2,
    HasContainerType = 1 << 3,
    AllowDestructor = 1 << 4,
    AllowEnumElement = 1 << 5,
    InInterface = 1 << 6,
    InStruct = 1 << 7,
    InEnum = 1 << 8,
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
};

class ParsingDeclState final {
  Parser &parser;

  /// The DeclState passed to the Decl
  DeclState *declState;

  /// The parsing decl options
  ParsingDeclOptions parsingDeclOpts;

  // The identifier we are parsing.
  Identifier declIdentifier;

  // Direct comparison is disabled for states
  void operator==(ParsingDeclState PDS) const = delete;
  void operator!=(ParsingDeclState PDS) const = delete;

public:
  explicit ParsingDeclState(Parser &parser)
      : parser(parser), declState(new(parser.GetASTContext())
                                      DeclState(parser.GetASTContext())) {}

public:
  ParsingDeclOptions &GetParsingDeclOptions() { return parsingDeclOpts; }
  DeclState *GetDeclState() { return declState; }
  void SetDeclIdentifier(Identifier identifier) { declIdentifier = identifier; }
  Identifier GetDeclIdentifier() { return declIdentifier; }
  Parser &GetParser() { return parser; }

  TypeInfluencerList &GetTypeInfluencerList() {
    return declState->GetTypeInfluencerList();
  }
  DeclInfluencerList &GetDeclInfluencerList() {
    return declState->GetDeclInfluencerList();
  }
};

} // namespace stone
#endif
