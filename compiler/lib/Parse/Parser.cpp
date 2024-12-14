#include "stone/Parse/Parser.h"
#include "stone/AST/ASTContext.h"
#include "stone/Basic/SrcLoc.h"
#include "stone/Basic/SrcMgr.h"

using namespace stone;

Parser::Parser(SourceFile &sourceFile, ASTContext &astContext)
    : Parser(sourceFile, astContext,
             std::unique_ptr<Lexer>(
                 new Lexer(sourceFile.GetSrcID(), astContext.GetSrcMgr(),
                           &astContext.GetDiags(), astContext.GetStats()))) {}

Parser::Parser(SourceFile &sourceFile, ASTContext &astContext,
               std::unique_ptr<Lexer> lx)
    : sourceFile(sourceFile), astContext(astContext), lexer(lx.release()),
      curDC(&sourceFile) {}

Parser::~Parser() {}

SrcLoc Parser::ConsumeToken(ParsingNotification notification) {
  SetPrevTok(curTok);
  auto loc = curTok.GetLoc();
  assert(curTok.IsNot(tok::eof) && "Lexing past eof!");

  if (notification == ParsingNotification::TokenConsumed) {
    if (HasCodeCompletionCallbacks()) {
      GetCodeCompletionCallbacks()->CompletedToken(&curTok);
    }
  }
  Lex(curTok, leadingTrivia, trailingTrivia);
  prevTokLoc = loc;
  return loc;
}

// This is there because you may want to strip certain things from the
// identifier name -- something to think about.
Identifier Parser::GetIdentifier(llvm::StringRef text) {
  return astContext.GetIdentifier(text);
}

ParserStatus Parser::ParseIdentifier(Identifier &result, SrcLoc &resultLoc) {
  ParserStatus status;

  assert(GetCurTok().IsIdentifierOrUnderscore());
  resultLoc = ConsumeIdentifier(result);

  return status;
}
SrcLoc Parser::ConsumeIdentifier(Identifier &result) {
  assert(curTok.IsAny(tok::identifier, tok::kw_this));

  result = GetIdentifier(GetCurTok().GetText());
  return ConsumeToken();
}

void Parser::RecordTokenHash(llvm::StringRef tokText) {
  assert(!tokText.empty());
  if (currentTokenHash) {
    currentTokenHash->combine(tokText);
    // Add null byte to separate tokens.
    currentTokenHash->combine(UInt8{0});
  }
}
SrcLoc Parser::ConsumeStartingLess() {
  assert(StartsWithLess(curTok) && "Token does not start with '<'");
  return ConsumeStartingCharOfCurToken(tok::l_angle);
}

SrcLoc Parser::ConsumeStartingGreater() {
  assert(StartsWithGreater(curTok) && "Token does not start with '>'");
  return ConsumeStartingCharOfCurToken(tok::r_angle);
}

// TODO:
SrcLoc Parser::ConsumeStartingCharOfCurToken(tok kind, size_t len) {
  return SrcLoc();
}

// ParsingScope::ParsingScope(Parser &self, ASTScopeKind kind,
//                            llvm::StringRef description)
//     : self(self), description(description) {
//   EnterScope(kind);
// }

/// Enter a new scope
// void ParsingScope::EnterScope(ASTScopeKind kind) { self.EnterScope(kind); }

// // Exit - Exit the scope associated with this object now, rather
// // than waiting until the object is destroyed.
// void ParsingScope::ExitScope() { self.ExitScope(); }

// ParsingScope::~ParsingScope() { ExitScope(); }

// void Parser::EnterScope(ASTScopeKind kind) {

//   if (!GetCurScope()) {
//     assert(kind == ASTScopeKind::TopLevelDecl);
//   }
//   // Create the new scope
//   auto curScope = CreateScope(kind, GetCurScope());

//   // Make sure we have a scope
//   assert(curScope);

//   // Cache the scope
//   PushCurScope(curScope);
// }

// ASTScope *Parser::GetCurScope() const {
//   if (HasCurScope()) {
//     return parsingScopeCache.back();
//   }
//   return nullptr;
// }

// ASTScope *Parser::CreateScope(ASTScopeKind kind, ASTScope *parentScope) {
//   return new (GetASTContext()) ASTScope(kind, GetDiags(), parentScope);
// }
// /// ExitScope - pop a scope off the scope stack.
// void Parser::ExitScope() {
//   // Ensure we have a current scope.
//   if (GetCurScope()) {
//     // Remove the scope
//     PopCurScope();
//   }
// }
