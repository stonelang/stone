#include "stone/Compile/Lexer.h"
#include "stone/Core/Context.h"
#include "stone/Core/SystemOptions.h"
#include "stone/Core/SrcMgr.h"

#include "gtest/gtest.h"

using namespace stone;
using namespace stone::syn;

class LexerTest : public ::testing::Test {
protected:
  Context ctx;

protected:
  LexerTest() {}

protected:
  std::unique_ptr<Lexer> CreateLexer(llvm::StringRef source) {

    auto srcID = ctx.GetSrcMgr().addMemBufferCopy(source);
    return std::make_unique<Lexer>(srcID, ctx.GetSrcMgr(), &ctx.GetDiagEngine(), &ctx.GetStatEngine());
  }
  std::vector<syn::Token> Lex(llvm::StringRef source) {

    auto lexer = CreateLexer(source);
    std::vector<syn::Token> tokens;
    while (true) {
      syn::Token token;
      lexer->Lex(token);
      tokens.push_back(token);
      if(token.GetKind() == tok::eof) {
      	break;
      }
    }
    return tokens;
  }
};

TEST_F(LexerTest, GetNextToken) {

  llvm::StringRef source = "Main fun() -> \n";
  auto tokens = Lex(source);

  ASSERT_EQ(tok::identifier, tokens[0].GetKind());
  ASSERT_EQ(tok::kw_fun, tokens[1].GetKind());
  ASSERT_EQ(tok::l_paren, tokens[2].GetKind());
  ASSERT_EQ(tok::r_paren, tokens[3].GetKind());
  //ASSERT_EQ(tok::arrow, tokens[4].GetKind());
}
