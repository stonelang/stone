#ifndef STONE_BASIC_CODECOMPLETIONLISTENER_H
#define STONE_BASIC_CODECOMPLETIONLISTENER_H

#include "stone/Basic/Error.h"
#include "stone/Basic/FileMgr.h"
#include "stone/Basic/FileSystemOptions.h"
#include "stone/Basic/LangOptions.h"
#include "stone/Basic/Result.h"
#include "stone/Basic/SrcMgr.h"
#include "stone/Basic/StatisticEngine.h"
#include "stone/Diag/DiagUnit.h"

namespace llvm {
class Module;
}

namespace stone {

namespace syn {
class Decl;
class Stmt;
class Expr;
class Module;
class Token;
class ASTFile;

} // namespace syn

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
  virtual void OnToken(const syn::Token *token) {}
};

class ASTListener : public CodeCompletionListener {
public:
  ASTListener() = default;
  virtual ~ASTListener() = default;

public:
  virtual void OnDone() {}
  virtual void OnError() {}

public:
  virtual void OnDecl(const syn::Decl *decl, bool isTopLevel = false);
  virtual void OnStmt(const syn::Stmt *stmt);
  virtual void OnExpr(const syn::Expr *expr);
  virtual void OnToken(const syn::Token *token);

public:
  virtual void OnParseError() {}
  virtual void OnParseStarted() {}
  virtual void OnParseASTFile(syn::ASTFile *sf) {}
  virtual void OnParseCompleted() {}
};

class TypeCheckerListener : public CodeCompletionListener {
public:
  TypeCheckerListener() = default;
  virtual ~TypeCheckerListener() = default;

public:
  virtual void OnDeclTypeChecked(syn::Decl *decl, bool isTopLvel = false);
  virtual void OnStmtTypeChecked(syn::Stmt *stmt) {}
  virtual void OnExprTypeChecked(syn::Expr *expr) {}

public:
  virtual void OnTypeCheckError();
  virtual void OnASTFileTypeChecked(syn::ASTFile *syntaxFile) {}
  virtual void OnModuleTypeChecked(syn::Module *mod) {}

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
