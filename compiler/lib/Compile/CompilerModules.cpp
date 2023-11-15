#include "stone/Compile/Compiler.h"

Status Compiler::SetupModules() {}

syn::ModuleDecl *Compiler::GetMainModule() const {
  if (!mainModule) {

    // assert(invocation.GetModuleOptions().HasModuleName());

    // // TODO: Check to make sure that we have the correct Identifier
    // Identifier moduleIdentifier =
    //     GetSyntaxContext().GetIdentifier(invocation.GetModuleOptions().moduleName);

    // mainModule =
    //     ModuleDecl::Create(moduleIdentifier, syntaxContext, true);

    // // Register the main module with the AST context.
    // GetSyntaxContext().AddLoadedModule(mainModule);

    // // Create and add the module's files.
    // llvm::SmallVector<syn::ModuleFile *, 16> moduleFiles;

    // if (CreateSyntaxFilesForMainModule(mainModule, moduleFiles).IsError()) {
    //   // If we failed to load a partial module, mark the main module as
    //   having
    //   // "failed to load", as it will contain no files. Note that we don'ttry
    //   // to add any of the successfully loaded partial modules. This ensures
    //   // that we don't encounter cases where we try to resolve a
    //   cross-reference
    //   // into a partial module that failed to load.
    //   // mainModule->SetFailedToLoad();
    // } else {
    //   for (auto *moduleFile : moduleFiles) {
    //     mainModule->AddFile(*moduleFile);
    //   }
    // }
  }
  return mainModule;
}

// // TODO:
// Status Compiler::CreateSyntaxFilesForMainModule(
//     syn::ModuleDecl *mod,
//     llvm::SmallVectorImpl<syn::ModuleFile *> &resultFiles) const {
//   // Try to pull out the main source file, if any. This ensures that it
//   // is at the start of the list of files.
//   llvm::Optional<unsigned> mainBufferID = llvm::None;
//   if (syn::SyntaxFile *mainSyntaxFile = ComputeMainSyntaxFileForModule(mod))
//   {
//     mainBufferID = mainSyntaxFile->GetSrcID();
//     resultFiles.push_back(mainSyntaxFile);
//   }

//   // If we have partial modules to load, do so now, bailing if any failed to
//   // load.
//   // TODO:
//   // if (!partialModules.empty()) {
//   //   if (LoadPartialModulesAndImplicitImports(mod, files))
//   //     return true;
//   // }

//   // Finally add the library files.
//   // FIXME: This is the only demand point for InputSourceCodeBufferIDs. We
//   // should compute this list of source files lazily.

//   for (auto bufferID : compiler.GetInputSourceBufferIDs()) {
//     // Skip the main buffer, we've already handled it.
//     if (bufferID == mainBufferID) {
//       continue;
//     }

//     auto *libraryFile = CreateSyntaxFileForMainModule(
//         mod, syn::SyntaxFileKind::Library, bufferID);
//     resultFiles.push_back(libraryFile);
//   }
//   return Status();
// }

// syn::SyntaxFile *
// Compiler::ComputeMainSyntaxFileForModule(ModuleDecl *mod) const {

//   if (compiler.GetCompilerOptions().parsingInputMode ==
//       CompilerOptions::ParsingInputMode::StoneLibrary) {
//     return nullptr;
//   }

//   return nullptr;
// }

// syn::SyntaxFile *Compiler::CreateSyntaxFileForMainModule(
//     ModuleDecl *mod, syn::SyntaxFileKind syntaxFileKind, unsigned bufferID,
//     bool isMainBuffer) const {

//   auto isPrimary = bufferID && compiler.IsPrimarySourceID(bufferID);
//   auto parsingOpts = GetSyntaxFileParsingOptions(isPrimary);

//   auto syntaxFile =
//       SyntaxFile::Make(syntaxFileKind, bufferID, *mod, syntaxContext);

//   // if (isMainBuffer)
//   //   inputFile->SyntaxParsingCache =
//   //   Invocation.getMainFileSyntaxParsingCache();

//   // return inputFile;

//   return syntaxFile;
// }

// syn::SyntaxFile::ParsingOptions
// Compiler::GetSyntaxFileParsingOptions(bool forPrimary) const {

//   auto parsingOpts = SyntaxFile::GetDefaultParsingOptions(
//       syntaxContext.GetLangContext().GetLangOptions());
//   return parsingOpts;
// }

// void Compiler::SetMainModule(ModuleDecl *mod) {

//   // TODO: This defaults to true for now
//   assert(mod->IsMainModule());
//   mainModule = mod;
//   syntaxContext.AddLoadedModule(mod);
// }

// Status Compiler::IsValidModuleName(const llvm::StringRef moduleName) {

//   // llvm::SmallVector<llvm::StringRef, 4> results;
//   // moduleName.split(results, ".");
//   // for (auto identifier : results) {
//   //   if (!Lexer::isIdentifier(identifier)) {
//   //     return stone::Error(true);
//   //   }
//   // }
//   // return Error();
//   if (!Lexer::isIdentifier(moduleName)) {
//     return Status::Error();
//   }
//   return Status();
// }

// bool stone::EmitImportedModules(syn::SyntaxContext &context,
//                                 ModuleDecl *mainModule,
//                                 const CompilerOptions &opts) {
//   return false;
// }
