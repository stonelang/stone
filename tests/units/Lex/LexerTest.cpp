#include "stone/Parse/Lexer.h"
#include "stone/Basic/LangOptions.h"
#include "stone/Basic/SrcMgr.h"
#include "stone/LangContext.h"

#include "gtest/gtest.h"

using namespace stone;
using namespace stone::syn;

class LexerTest : public ::testing::Test {
protected:
  LangContext ctx;

protected:
  LexerTest() {}

protected:
  std::unique_ptr<Lexer> CreateLexer(llvm::StringRef source) {

    auto srcID = ctx.GetSrcMgr().addMemBufferCopy(source);
    return std::make_unique<Lexer>(srcID, ctx.GetSrcMgr(),
                                   &ctx.GetDiagUnit().GetDiagEngine(),
                                   &ctx.GetStatEngine());
  }
  std::vector<syn::Token> Lex(llvm::StringRef source) {

    auto lexer = CreateLexer(source);
    std::vector<syn::Token> tokens;
    while (true) {
      syn::Token token;
      lexer->Lex(token);
      tokens.push_back(token);
      if (token.GetKind() == tok::eof) {
        break;
      }
    }
    return tokens;
  }
};

TEST_F(LexerTest, GetNextToken) {

  llvm::StringRef source = "fun Main() -> int  { return 0;}\n";
  auto tokens = Lex(source);

  ASSERT_EQ(tok::kw_fun, tokens[0].GetKind());
  ASSERT_EQ(tok::identifier, tokens[1].GetKind());
  ASSERT_EQ(tok::l_paren, tokens[2].GetKind());
  ASSERT_EQ(tok::r_paren, tokens[3].GetKind());
  ASSERT_EQ(tok::arrow, tokens[4].GetKind());
  ASSERT_EQ(tok::kw_int, tokens[5].GetKind());
  ASSERT_EQ(tok::l_brace, tokens[6].GetKind());
  ASSERT_EQ(tok::kw_return, tokens[7].GetKind());
  ASSERT_EQ(tok::integer_literal, tokens[8].GetKind());
  ASSERT_EQ(tok::semi, tokens[9].GetKind());
  ASSERT_EQ(tok::r_brace, tokens[10].GetKind());
}
