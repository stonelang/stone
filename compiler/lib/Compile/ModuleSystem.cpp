#include "stone/Compile/ModuleSystem.h"
#include "stone/AST/DeclFactory.h"
#include "stone/Compile/CompilerInvocation.h"
#include "stone/Compile/Lexer.h"

using namespace stone;
using namespace stone::ast;

ModuleSystem::ModuleSystem(CompilerInvocation &invocation, ast::ASTContext &sc)
    : invocation(invocation), sc(sc) {}

ModuleSystem::~ModuleSystem() {}

ast::ModuleDecl *ModuleSystem::GetMainModule() const {
  if (!mainModule) {

    assert(invocation.GetModuleOptions().HasModuleName());

    // TODO: Check to make sure that we have the correct Identifier
    Identifier moduleIdentifier =
        sc.GetIdentifier(invocation.GetModuleOptions().moduleName);

    mainModule = DeclFactory::MakeModuleDecl(moduleIdentifier, sc, true);

    // Register the main module with the AST context.
    sc.AddLoadedModule(mainModule);

    // Create and add the module's files.
    llvm::SmallVector<ast::ModuleFile *, 16> moduleFiles;

    if (!CreateASTFilesForMainModule(mainModule, moduleFiles).Has()) {
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

Error ModuleSystem::CreateASTFilesForMainModule(
    ast::ModuleDecl *mod,
    llvm::SmallVectorImpl<ast::ModuleFile *> &resultFiles) const {
  // Try to pull out the main source file, if any. This ensures that it
  // is at the start of the list of files.
  llvm::Optional<unsigned> mainBufferID = llvm::None;
  if (ast::ASTFile *mainASTFile = ComputeMainASTFileForModule(mod)) {
    mainBufferID = mainASTFile->GetSrcID();
    resultFiles.push_back(mainASTFile);
  }

  // If we have partial modules to load, do so now, bailing if any failed to
  // load.
  // TODO:
  // if (!partialModules.empty()) {
  //   if (LoadPartialModulesAndImplicitImports(mod, files))
  //     return true;
  // }

  // Finally add the library files.
  // FIXME: This is the only demand point for InputSourceCodeBufferIDs. We
  // should compute this list of source files lazily.

  for (auto bufferID : invocation.GetSourceBufferIDs()) {
    // Skip the main buffer, we've already handled it.
    if (bufferID == mainBufferID) {
      continue;
    }

    auto *libraryFile =
        CreateASTFileForMainModule(mod, ast::ASTFileKind::Library, bufferID);
    resultFiles.push_back(libraryFile);
  }
  return Error();
}

ast::ASTFile *ModuleSystem::ComputeMainASTFileForModule(ModuleDecl *mod) const {

  if (invocation.GetCompilerOptions().parsingInputMode ==
      CompilerOptions::ParsingInputMode::StoneLibrary) {
    return nullptr;
  }

  return nullptr;
}

ast::ASTFile *ModuleSystem::CreateASTFileForMainModule(
    ModuleDecl *mod, ast::ASTFileKind asttaxFileKind, unsigned bufferID,
    bool isMainBuffer) const {

  auto isPrimary = bufferID && invocation.IsPrimarySourceID(bufferID);
  auto parsingOpts = GetASTFileParsingOptions(isPrimary);

  auto asttaxFile = ASTFile::Make(asttaxFileKind, bufferID, *mod, sc);

  // if (isMainBuffer)
  //   inputFile->ASTParsingCache =
  //   Invocation.getMainFileASTParsingCache();

  // return inputFile;

  return asttaxFile;
}

ast::ASTFile::ParsingOptions
ModuleSystem::GetASTFileParsingOptions(bool forPrimary) const {

  auto parsingOpts =
      ASTFile::GetDefaultParsingOptions(sc.GetLang().GetLangOptions());
  return parsingOpts;
}

void ModuleSystem::SetMainModule(ModuleDecl *mod) {

  // TODO: This defaults to true for now
  assert(mod->IsMainModule());
  mainModule = mod;
  sc.AddLoadedModule(mod);
}

Error ModuleSystem::IsValidModuleName(const llvm::StringRef moduleName) {

  llvm::SmallVector<llvm::StringRef, 4> results;
  moduleName.split(results, ".");
  for (auto identifier : results) {
    if (!Lexer::isIdentifier(identifier)) {
      return stone::Error(true);
    }
  }
  return Error();
}

bool stone::EmitImportedModules(ast::ASTContext &context,
                                ModuleDecl *mainModule,
                                const CompilerOptions &opts) {
  return false;
}
