#ifndef STONE_BASIC_CODECOMPLETIONLISTENER_H
#define STONE_BASIC_CODECOMPLETIONLISTENER_H

#include "stone/Basic/FileMgr.h"
#include "stone/Basic/FileSystemOptions.h"
#include "stone/Basic/LangOptions.h"
#include "stone/Basic/Status.h"
#include "stone/Basic/SrcMgr.h"
#include "stone/Basic/StatisticEngine.h"
#include "stone/Diag/DiagUnit.h"

namespace llvm {
class Module;
}

namespace stone {

namespace ast {
class Decl;
class Stmt;
class Expr;
class Module;
class Token;
class ASTFile;

} // namespace ast

class CodeCompletionListener {
public:
  CodeCompletionListener() = default;
  virtual ~CodeCompletionListener() = default;
};

class LexerListener : public CodeCompletionListener {
public:
  LexerListener() = default;
  virtual ~LexerListener() = default;

public:
  virtual void OnToken(const ast::Token *token) {}
};

class ASTListener : public CodeCompletionListener {
public:
  ASTListener() = default;
  virtual ~ASTListener() = default;

public:
  virtual void OnDone() {}
  virtual void OnError() {}

public:
  virtual void OnDecl(const ast::Decl *decl, bool isTopLevel = false);
  virtual void OnStmt(const ast::Stmt *stmt);
  virtual void OnExpr(const ast::Expr *expr);
  virtual void OnToken(const ast::Token *token);

public:
  virtual void OnParseError() {}
  virtual void OnParseStarted() {}
  virtual void OnParseASTFile(ast::ASTFile *sf) {}
  virtual void OnParseCompleted() {}
};

class TypeCheckerListener : public CodeCompletionListener {
public:
  TypeCheckerListener() = default;
  virtual ~TypeCheckerListener() = default;

public:
  virtual void OnDeclTypeChecked(ast::Decl *decl, bool isTopLvel = false);
  virtual void OnStmtTypeChecked(ast::Stmt *stmt) {}
  virtual void OnExprTypeChecked(ast::Expr *expr) {}

public:
  virtual void OnTypeCheckError();
  virtual void OnASTFileTypeChecked(ast::ASTFile *asttaxFile) {}
  virtual void OnModuleTypeChecked(ast::Module *mod) {}

public:
};

class CodeGenListener : public CodeCompletionListener {
public:
  CodeGenListener() = default;
  virtual ~CodeGenListener() = default;

public:
  virtual void OnEmitIRError() {}
  virtual void OnEmitIRCompleted(llvm::Module *m) {}
  virtual void OnEmitObjectError() {}
  virtual void OnEmitObject() {}
  virtual void OnEmitObjectCompleted() {}
  virtual void OnEmitBitCodeError() {}
  virtual void OnEmitBitCode() {}
  virtual void OnEmitBitCodeompleted() {}
  virtual void OnEmitModuleError() {}
  virtual void OnEmitModule() {}
  virtual void OnEmitModuleCompleted() {}
  virtual void OnEmitLibraryError() {}
  virtual void OnEmitLibrary() {}
  virtual void OnEmitLibraryCompleted() {}
};

class CompilerInstance;
class CompilerInvocation;
class CompilerListener : public LexerListener,
                         public ASTListener,
                         public TypeCheckerListener,
                         public CodeGenListener {
public:
  CompilerListener() = default;
  virtual ~CompilerListener() = default;

public:
  virtual void OnCompileConfigured(CompilerInvocation &invocation) {}
  virtual void OnCompileStarted(CompilerInstance &instance) {}
  virtual void OnASTAnalysisCompleted(CompilerInstance &instance) {}
  virtual void OnSemanticAnalysisCompleted(CompilerInstance &instance) {}
  virtual void OnCodeGenCompleted(CompilerInstance &instance) {}
  virtual void OnCompileCompleted(CompilerInstance &instance) {}
};
} // namespace stone

#endif
