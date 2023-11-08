#ifndef STONE_BASIC_CODECOMPLETIONLISTENER_H
#define STONE_BASIC_CODECOMPLETIONLISTENER_H

#include "stone/Basic/DiagnosticEngine.h"
#include "stone/Basic/FileMgr.h"
#include "stone/Basic/FileSystemOptions.h"
#include "stone/Basic/LangOptions.h"
#include "stone/Basic/SrcMgr.h"
#include "stone/Basic/StatisticEngine.h"
#include "stone/Basic/Status.h"

namespace llvm {
class Module;
}

namespace stone {

class Decl;
class Stmt;
class Expr;
class ModuleDecl;
class Token;
class ASTFile;


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
  virtual void OnToken(const stone::Token *token) {}
};

class ASTListener : public CodeCompletionListener {
public:
  ASTListener() = default;
  virtual ~ASTListener() = default;

public:
  virtual void OnDone() {}
  virtual void OnError() {}

public:
  virtual void OnDecl(const stone::Decl *decl, bool isTopLevel = false);
  virtual void OnStmt(const stone::Stmt *stmt);
  virtual void OnExpr(const stone::Expr *expr);
  virtual void OnToken(const stone::Token *token);

public:
  virtual void OnParseError() {}
  virtual void OnParseStarted() {}
  virtual void OnParseASTFile(stone::ASTFile *sf) {}
  virtual void OnParseCompleted() {}
};

class TypeCheckerListener : public CodeCompletionListener {
public:
  TypeCheckerListener() = default;
  virtual ~TypeCheckerListener() = default;

public:
  virtual void OnDeclTypeChecked(stone::Decl *decl, bool isTopLvel = false);
  virtual void OnStmtTypeChecked(stone::Stmt *stmt) {}
  virtual void OnExprTypeChecked(stone::Expr *expr) {}

public:
  virtual void OnTypeCheckError();
  virtual void OnASTFileTypeChecked(stone::ASTFile *asttaxFile) {}
  virtual void OnModuleTypeChecked(stone::ModuleDecl *mod) {}

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
