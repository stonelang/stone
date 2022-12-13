#include "stone/Compile/ModuleSystem.h"
#include "stone/Compile/CompilerInvocation.h"
#include "stone/Parse/Lexer.h"

#include "stone/Syntax/SyntaxFactory.h"

using namespace stone;
using namespace stone::syn;

ModuleSystem::ModuleSystem(LangContext &lc, SyntaxContext &sc,
                           CompilerOptions &compilerOpts)
    : lc(lc), sc(sc), compilerOpts(compilerOpts) {}

ModuleSystem::~ModuleSystem() {}

syn::ModuleDecl *ModuleSystem::GetMainModule() const {
  if (!mainModule) {
    // TODO: Check to make sure that we have the correct Identifier
    Identifier moduleIdentifier = sc.GetIdentifier(GetModuleOptions().moduleName);
    mainModule = ModuleDeclFactory::Create(moduleIdentifier, sc, true);

    // Register the main module with the AST context.
    sc.AddLoadedModule(mainModule);

    // Create and add the module's files.
    llvm::SmallVector<ModuleFile *, 16> moduleFiles;

    if (!CreateSyntaxFilesForMainModule(mainModule, moduleFiles).Has()) {
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

Error ModuleSystem::CreateSyntaxFilesForMainModule(
    syn::ModuleDecl *mod, llvm::SmallVectorImpl<syn::ModuleFile *> &resultFiles) const {
  // Try to pull out the main source file, if any. This ensures that it
  // is at the start of the list of files.
  llvm::Optional<unsigned> mainBufferID = llvm::None;
  if (syn::SyntaxFile *mainSyntaxFile = ComputeMainSyntaxFileForModule(mod)) {
    mainBufferID = mainSyntaxFile->GetSrcID();
    resultFiles.push_back(mainSyntaxFile);
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

  // for (auto bufferID : invocation.GetSourceBufferIDs()) {
  //   // Skip the main buffer, we've already handled it.
  //   if (bufferID == mainBufferID){
  //     continue;
  //   }

  //   auto *libraryFile =
  //       CreateSyntaxFilesForMainModule(mod, ModuleFileKind::Library,
  //       bufferID);
  //   files.push_back(libraryFile);
  // }
  return Error();
}

SyntaxFile *
ModuleSystem::ComputeMainSyntaxFileForModule(ModuleDecl *mod) const {

  if (GetCompilerOptions().parsingInputMode ==
      CompilerOptions::ParsingInputMode::StoneLibrary) {
    return nullptr;
  }

  return nullptr;
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