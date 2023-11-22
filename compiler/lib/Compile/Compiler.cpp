#include "stone/Compile/Compiler.h"
#include "stone/Compile/CompilerInputFile.h"
#include "stone/Option/ActionKind.h"
#include "stone/Parse/Lexer.h" // TODO: do better
#include "stone/Syntax/Module.h"

using namespace stone;

Compiler::Compiler() : invocation(*this) {}

void Compiler::Setup() { assert(invocation.HasAction()); }

std::unique_ptr<CompilerExecution>
Compiler::GetExecutionForAction(ActionKind action) {
  switch (action) {
  case ActionKind::PrintHelp:
  case ActionKind::PrintHelpHidden:
  case ActionKind::PrintVersion:
    return std::make_unique<SupportExecution>(*this);
  case ActionKind::Parse:
  case ActionKind::ResolveImports:
  case ActionKind::DumpSyntax:
    return std::make_unique<SyntaxAnalysisExecution>(*this);
  case ActionKind::TypeCheck:
  case ActionKind::PrintSyntax:
  case ActionKind::DumpTypeInfo:
    return std::make_unique<SemanticAnalysisExecution>(*this);
  case ActionKind::EmitIRBefore:
  case ActionKind::EmitIRAfter:
  case ActionKind::EmitBC:
  case ActionKind::EmitAssembly:
  case ActionKind::EmitObject:
    return std::make_unique<CodeGenExecution>(*this);
  default: {
    return std::make_unique<FallbackExecution>(*this);
  }
  }
}

Status Compiler::ExecuteAction(ActionKind kind) {
  auto execution = GetExecutionForAction(kind);
  execution->Setup();
  return execution->Execute();
}

ModuleDecl *Compiler::GetMainModule() const {

  assert(astContext);
  if (!mainModule) {
    assert(invocation.GetCompilerOptions().moduleOpts.HasModuleName());

    // TODO: Check to make sure that we have the correct Identifier
    Identifier moduleIdentifier = astContext->GetIdentifier(
        invocation.GetCompilerOptions().moduleOpts.moduleName);

    mainModule = ModuleDecl::CreateMainModule(moduleIdentifier, *astContext);
    assert(mainModule);
    // Register the main module with the AST context.
    astContext->AddLoadedModule(mainModule);

    // Create and add the module's files.
    llvm::SmallVector<ModuleFile *, 16> moduleFiles;
    if (!CreateSourceFilesForMainModule(mainModule, moduleFiles).IsError()) {
      for (auto *moduleFile : moduleFiles)
        mainModule->AddFile(*moduleFile);
    } else {
      // If we failed to load a partial module, mark the main module as having
      // "failed to load", as it will contain no files. Note that we don'ttry
      // to add any of the successfully loaded partial modules. This ensures
      // that we don't encounter cases where we try to resolve a cross-reference
      // into a partial module that failed to load.
      // mainModule->SetFailedToLoad();
    }
  }
  return mainModule;
}

void Compiler::SetMainModule(ModuleDecl *inputMainModule) {

  // TODO: This defaults to true for now
  assert(inputMainModule->IsMainModule());
  mainModule = inputMainModule;
  astContext->AddLoadedModule(mainModule);
}

Status Compiler::CreateSourceFilesForMainModule(
    ModuleDecl *mod, llvm::SmallVectorImpl<ModuleFile *> &files) const {

  return Status();
}

SourceFile *Compiler::CreateSourceFileForMainModule(ModuleDecl *mainModule,
                                                    SourceFileKind fileKind,
                                                    unsigned bufferID,
                                                    bool isMainBuffer) const {

  return nullptr;
}

SourceFile *Compiler::ComputeMainSourceFileForModule(ModuleDecl *mod) const {
  return nullptr;
}

Status Compiler::IsValidModuleName(const llvm::StringRef moduleName) {
  if (!Lexer::isIdentifier(moduleName)) {
    return Status::Error();
  }
  return Status();
}
