#include "stone/Parse/Lexer.h"
#include "stone/Basic/Basic.h"
#include "stone/Basic/LangOptions.h"
#include "stone/Basic/SrcMgr.h"

#include "gtest/gtest.h"

using namespace stone;
using namespace stone::syn;

class LexerTest : public ::testing::Test {
protected:
  DiagnosticEngine de;
  DiagnosticOptions diagOpts;
  FileSystemOptions fmOpts;
  LangOptions langOpts;
  FileMgr fm;
  SrcMgr sm;
  Basic basic;

protected:
  LexerTest() : de(diagOpts, &sm), fm(fmOpts) {}

protected:
  std::unique_ptr<Lexer> CreateLexer(llvm::StringRef source) {

    auto srcID = sm.addMemBufferCopy(source);
    return std::make_unique<Lexer>(srcID, sm, basic);
  }
  std::vector<syn::Token> Lex(llvm::StringRef source) {

    auto lexer = CreateLexer(source);
    std::vector<syn::Token> tokens;
    while (true) {
      syn::Token token;
      lexer->Lex(token);
      tokens.push_back(token);
      break;
      // if(token.GetKind() == tk::Type::eof) {
      //	break;
      //}
    }
    return tokens;
  }
};

TEST_F(LexerTest, GetNextToken) {

  llvm::StringRef source = "fun\n";
  auto tokens = Lex(source);

  ASSERT_EQ(tk::Type::kw_fun, tokens[0].GetType());
}
