#include "stone/Compile/ModuleSystem.h"
#include "stone/Compile/Compiler.h"
#include "stone/Parse/Lexer.h"
#include "stone/Syntax/DeclFactory.h"

using namespace stone;

ModuleSystem::ModuleSystem(Compiler &compiler, ASTContext &syntaxContext)
    : compiler(compiler), syntaxContext(syntaxContext) {}

ModuleSystem::~ModuleSystem() {}

ModuleDecl *ModuleSystem::GetMainModule() const {
  if (!mainModule) {

    assert(compiler.GetModuleOptions().HasModuleName());

    // TODO: Check to make sure that we have the correct Identifier
    Identifier moduleIdentifier =
        syntaxContext.GetIdentifier(compiler.GetModuleOptions().moduleName);

    mainModule =
        DeclFactory::MakeModuleDecl(moduleIdentifier, syntaxContext, true);

    // Register the main module with the AST context.
    syntaxContext.AddLoadedModule(mainModule);

    // Create and add the module's files.
    llvm::SmallVector<ModuleFile *, 16> moduleFiles;

    if (CreateSourceFilesForMainModule(mainModule, moduleFiles).IsError()) {
      // If we failed to load a partial module, mark the main module as having
      // "failed to load", as it will contain no files. Note that we don'ttry
      // to add any of the successfully loaded partial modules. This ensures
      // that we don't encounter cases where we try to resolve a cross-reference
      // into a partial module that failed to load.
      // mainModule->SetFailedToLoad();
    } else {
      for (auto *moduleFile : moduleFiles) {
        mainModule->AddFile(*moduleFile);
      }
    }
  }
  return mainModule;
}

// TODO:
Status ModuleSystem::CreateSourceFilesForMainModule(
    ModuleDecl *mod, llvm::SmallVectorImpl<ModuleFile *> &resultFiles) const {
  // Try to pull out the main source file, if any. This ensures that it
  // is at the start of the list of files.
  llvm::Optional<unsigned> mainBufferID = llvm::None;
  if (SourceFile *mainSourceFile = ComputeMainSourceFileForModule(mod)) {
    mainBufferID = mainSourceFile->GetSrcID();
    resultFiles.push_back(mainSourceFile);
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

  for (auto bufferID : compiler.GetInputSourceBufferIDs()) {
    // Skip the main buffer, we've already handled it.
    if (bufferID == mainBufferID) {
      continue;
    }

    auto *libraryFile =
        CreateSourceFileForMainModule(mod, SourceFileKind::Library, bufferID);
    resultFiles.push_back(libraryFile);
  }
  return Status();
}

SourceFile *ModuleSystem::ComputeMainSourceFileForModule(ModuleDecl *mod) const {

  if (compiler.GetCompilerOptions().parsingInputMode ==
      CompilerOptions::ParsingInputMode::StoneLibrary) {
    return nullptr;
  }

  return nullptr;
}

SourceFile *ModuleSystem::CreateSourceFileForMainModule(ModuleDecl *mod,
                                                  SourceFileKind sourceFileKind,
                                                  unsigned bufferID,
                                                  bool isMainBuffer) const {

  auto isPrimary = bufferID && compiler.IsPrimarySourceID(bufferID);
  auto parsingOpts = GetSourceFileParsingOptions(isPrimary);

  auto sourceFile = SourceFile::Make(sourceFileKind, bufferID, *mod, syntaxContext);

  // if (isMainBuffer)
  //   inputFile->SyntaxParsingCache =
  //   Invocation.getMainFileSyntaxParsingCache();

  // return inputFile;

  return sourceFile;
}

SourceFile::ParsingOptions
ModuleSystem::GetSourceFileParsingOptions(bool forPrimary) const {

  auto parsingOpts = SourceFile::GetDefaultParsingOptions(
      syntaxContext.GetLangContext().GetLangOptions());
  return parsingOpts;
}

void ModuleSystem::SetMainModule(ModuleDecl *mod) {

  // TODO: This defaults to true for now
  assert(mod->IsMainModule());
  mainModule = mod;
  syntaxContext.AddLoadedModule(mod);
}

Status ModuleSystem::IsValidModuleName(const llvm::StringRef moduleName) {

  // llvm::SmallVector<llvm::StringRef, 4> results;
  // moduleName.split(results, ".");
  // for (auto identifier : results) {
  //   if (!Lexer::isIdentifier(identifier)) {
  //     return stone::Error(true);
  //   }
  // }
  // return Error();
  if (!Lexer::isIdentifier(moduleName)) {
    return Status::Error();
  }
  return Status();
}

bool stone::EmitImportedModules(ASTContext &context, ModuleDecl *mainModule,
                                const CompilerOptions &opts) {
  return false;
}
