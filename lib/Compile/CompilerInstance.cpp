#include "stone/Compile/CompilerInstance.h"
#include "stone/AST/DiagnosticsCompile.h"
#include "stone/AST/Module.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/Memory.h"
#include "stone/Basic/SrcMgr.h"
#include "stone/Compile/Compile.h"
#include "stone/Compile/CompilerAction.h"
#include "stone/Compile/CompilerInputFile.h"
#include "stone/Compile/CompilerInvocation.h"
#include "stone/Compile/CompilerObservation.h"

// TODO: do better
#include "stone/Parse/Lexer.h"

using namespace stone;

CompilerInstance::CompilerInstance(CompilerInvocation &invocation)
    : invocation(invocation) {}

bool CompilerInstance::Setup() {

  assert(invocation.GetCompilerOptions().HasPrimaryAction() &&
         "CompilerInstance does not have a valid action!");

  if (SetupCompilerInputFiles().IsError()) {
    return false;
  }
  if (ShouldSetupASTContext()) {
    if (!SetupASTContext()) {
      return false;
    }
  }
  SetupStats();
  return true;
}

CompilerActionKind CompilerAction::GetPrimaryActionKind() {
  return instance.GetPrimaryActionKind();
}

bool CompilerAction::SetupAction() {
  assert(HasSelfActionKind());
  if (HasDepActionKind()) {
    auto dep = instance.ConstructAction(GetDepActionKind());
    assert(dep);
    dep->SetConsumer(this);
    if (!instance.ExecuteAction(*dep)) {
      return false;
    }
  }
  return true;
}

bool CompilerInstance::ExecuteAction() {
  return ExecuteAction(GetPrimaryActionKind());
}

bool CompilerInstance::ExecuteAction(CompilerActionKind kind) {
  auto action = ConstructAction(kind);
  return ExecuteAction(*action);
}
bool CompilerInstance::ExecuteAction(CompilerAction &action) {
  if (!action.SetupAction()) {
    return false;
  }
  if (!action.ExecuteAction()) {
    return false;
  }
  return action.FinishAction();
}

std::unique_ptr<CompilerAction>
CompilerInstance::ConstructAction(CompilerActionKind kind) {
  switch (kind) {
  case CompilerActionKind::PrintHelp:
    return std::make_unique<PrintHelpAction>(*this);
    // case CompilerActionKind::PrintHelpHidden:
    //   return
    //   std::make_unique<CompilerInstance::PrintHelpHiddenAction>(*this);
    // case CompilerActionKind::PrintVersion:
    //   return std::make_unique<CompilerInstance::PrintVersionAction>(*this);
    // case CompilerActionKind::PrintFeature:
    //   return std::make_unique<CompilerInstance::PrintFeatureAction>(*this);
    // case CompilerActionKind::Parse:
    //   return std::make_unique<CompilerInstance::ParseAction>(*this);
    // case CompilerActionKind::EmitParse:
    //   return std::make_unique<CompilerInstance::EmitParseAction>(*this);
    // case CompilerActionKind::ResolveImports:
    //   return std::make_unique<CompilerInstance::ResolveImportsAction>(*this);
    // case CompilerActionKind::TypeCheck:
    //   return std::make_unique<CompilerInstance::TypeCheckAction>(*this);
    // case CompilerActionKind::EmitAST:
    //   return std::make_unique<CompilerInstance::EmitASTAction>(*this);
    // case CompilerActionKind::EmitIR:
    //   return std::make_unique<CompilerInstance::EmitIRAction>(*this);
    // case CompilerActionKind::EmitBC:
    //   return std::make_unique<CompilerInstance::EmitBCAction>(*this);
    // case CompilerActionKind::EmitObject:
    //   return std::make_unique<CompilerInstance::EmitObjectAction>(*this);
    // case CompilerActionKind::EmitModule:
    //   return std::make_unique<EmitModuleAction>(*this);
    // case CompilerActionKind::MergeModules:
    //   return std::make_unique<MergeModulesAction>(*this);
    // case CompilerActionKind::EmitAssembly:
    //   return std::make_unique<EmitAssemblyAction>(*this);
  }
  llvm_unreachable("Unable to create CompilerAction -- unknon action!");
}

ModuleDecl *CompilerInstance::GetMainModule() const {

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
    astContext->SetMainModule(mainModule);

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

void CompilerInstance::SetMainModule(ModuleDecl *inputMainModule) {

  // TODO: This defaults to true for now
  assert(inputMainModule->IsMainModule());
  mainModule = inputMainModule;
  astContext->AddLoadedModule(mainModule);
}

Status CompilerInstance::CreateSourceFilesForMainModule(
    ModuleDecl *mod, llvm::SmallVectorImpl<ModuleFile *> &resultFiles) const {

  // Try to pull out the main source file, if any. This ensures that it
  // is at the start of the list of files.
  std::optional<unsigned> mainBufferID = std::nullopt;
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

  for (auto bufferID : inputSourceBufferIDList) {
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

SourceFile *
CompilerInstance::ComputeMainSourceFileForModule(ModuleDecl *mod) const {
  // TODO:
  if (invocation.GetCompilerOptions().parsingInputMode ==
      CompilerOptions::ParsingInputMode::StoneLibrary) {
    return nullptr;
  }
  return nullptr;
}

// Sources
Status CompilerInstance::SetupCompilerInputFiles() {

  // Adds to InputSourceCodeBufferIDs, so may need to happen before the
  // per-input setup.
  const std::optional<unsigned> codeCompletionBufferID =
      CreateCodeCompletionBuffer();

  assert([&]() -> bool {
    if (invocation.GetCompilerOptions().ShouldActionOnlyParse()) {
      // Parsing gets triggered lazily, but let's make sure we have the right
      // input kind.
      return llvm::all_of(
          invocation.GetCompilerOptions().GetInputsAndOutputs().GetInputs(),
          [](const CompilerInputFile &input) {
            const auto fileType = input.GetType();
            return fileType == FileType::Stone ||
                   fileType == FileType::StoneModuleInterface;
          });
    }
    return true;
  }() && "Only supports parsing .stone files");

  const auto &inputs =
      invocation.GetCompilerOptions().GetInputsAndOutputs().GetInputs();
  const bool shouldRecover = invocation.GetCompilerOptions()
                                 .inputsAndOutputs.ShouldRecoverMissingInputs();

  bool hasFailed = false;
  for (const CompilerInputFile &input : inputs) {
    bool failed = false;
    std::optional<unsigned> bufferID =
        GetRecordedBufferID(input, shouldRecover, failed);
    hasFailed |= failed;

    if (!bufferID.has_value() || !input.IsPrimary()) {
      continue;
    }
    RecordPrimarySourceID(*bufferID);
  }
  if (hasFailed) {
    return Status::Error();
  }
  return Status();
}

std::optional<unsigned>
CompilerInstance::GetRecordedBufferID(const CompilerInputFile &input,
                                      const bool shouldRecover, bool &failed) {
  if (!input.GetBuffer()) {
    if (std::optional<unsigned> existingBufferID =
            invocation.GetSrcMgr().getIDForBufferIdentifier(
                input.GetFileName())) {
      return existingBufferID;
    }
  }
  auto buffers = GetInputBuffersIfPresent(input);

  // Recover by dummy buffer if requested.
  if (!buffers.has_value() && shouldRecover &&
      input.GetType() == FileType::Stone) {
    buffers = ModuleBuffers(llvm::MemoryBuffer::getMemBuffer(
        "// missing file\n", input.GetFileName()));
  }

  if (!buffers.has_value()) {
    failed = true;
    return std::nullopt;
  }

  // FIXME: The fact that this test happens twice, for some cases,
  // suggests that setupInputs could use another round of refactoring.
  // TODO:
  // if (serialization::isSerializedAST(buffers->ModuleBuffer->getBuffer())) {
  //   PartialModules.push_back(std::move(*buffers));
  //   return std::nullopt;
  // }

  // TODO
  // assert(buffers->moduleDocBuffer.get() == nullptr);
  // assert(buffers->moduleSourceInfoBuffer.get() == nullptr);

  // Transfer ownership of the MemoryBuffer to the SourceMgr.
  unsigned bufferID = invocation.GetSrcMgr().addNewSourceBuffer(
      std::move(buffers->moduleBuffer));

  inputSourceBufferIDList.push_back(bufferID);
  return bufferID;
}

// unsigned CompilerInstance::CreateBufferIDForCompilerInputFile(const
// CompilerInputFile &input) {
//   auto fb = GetFileMgr().getBufferForFile(input.GetFileName());
//   if (!fb) {
//     GetDiags().diagnose(SrcLoc(), diag::error_unable_to_open_buffer_for_file,
//                       diag::LLVMStr(input.GetFileName()));
//   }
//   auto srcID = astContext->GetSrcMgr().addNewSourceBuffer(std::move(*fb));
//   assert((srcID > 0) && "Input file buffer ID must be greater than zero.");
//   return srcID;
// }

void CompilerInstance::RecordPrimarySourceID(unsigned primarySourceID) {
  primarySourceBufferIDList.insert(primarySourceID);
}

// TODO:
std::optional<ModuleBuffers>
CompilerInstance::GetInputBuffersIfPresent(const CompilerInputFile &input) {

  if (auto b = input.GetBuffer()) {
    return ModuleBuffers(llvm::MemoryBuffer::getMemBufferCopy(
        b->getBuffer(), b->getBufferIdentifier()));
  }

  // FIXME: Working with filenames is fragile, maybe use the real path
  // or have some kind of FileManager.

  using InputFileOrError = llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>>;
  InputFileOrError inputFileOrError =
      invocation.GetClangImporter().GetFileMgr().getBufferForFile(
          input.GetFileName());

  if (!inputFileOrError) {
    invocation.GetDiags().diagnose(SrcLoc(),
                                   diag::error_unable_to_open_buffer_for_file,
                                   input.GetFileName());
    return std::nullopt;
  }

  // Just return the file buffer for now
  return ModuleBuffers(std::move(*inputFileOrError));
  // if (!fb) {
  //   ctx.GetDiagUnit().diagnose(SrcLoc(),
  //   diag::error_unable_to_open_buffer_for_file,
  //                            diag::LLVMStr(input.GetFileName()));
  // }
  // auto srcID = ctx.GetSrcMgr().addNewSourceBuffer(std::move(*fb));
  // assert((srcID > 0) && "Input file buffer ID must be greater than zero.");
  // return srcID;

  // FileOrError inputFileOrErr =
  //   swift::vfs::getFileOrSTDIN(getFileSystem(), input.getFileName(),
  //                             /*FileSize*/-1,
  //                             /*RequiresNullTerminator*/true,
  //                             /*IsVolatile*/false,
  //     /*Bad File Descriptor Retry*/getInvocation().getCompilerOptions()
  //                              .BadFileDescriptorRetryCount);
  // if (!inputFileOrErr) {
  //   Diagnostics.diagnose(SourceLoc(), diag::error_open_input_file,
  //                        input.getFileName(),
  //                        inputFileOrErr.getError().message());
  //   return std::nullopt;
  // }
  // if (!serialization::isSerializedAST((*inputFileOrErr)->getBuffer()))
  //   return ModuleBuffers(std::move(*inputFileOrErr));

  // auto swiftdoc = openModuleDoc(input);
  // auto sourceinfo = openModuleSourceInfo(input);
  // return ModuleBuffers(std::move(*inputFileOrErr),
  //                      swiftdoc.hasValue() ? std::move(swiftdoc.getValue())
  //                      : nullptr, sourceinfo.hasValue() ?
  //                      std::move(sourceinfo.getValue()) : nullptr);

  // return std::nullopt;
}

std::optional<unsigned> CompilerInstance::CreateCodeCompletionBuffer() {
  std::optional<unsigned> codeCompletionBufferID;
  return codeCompletionBufferID;
}

SourceFile *CompilerInstance::CreateSourceFileForMainModule(
    ModuleDecl *mainModule, SourceFileKind kind, unsigned bufferID,
    bool isMainBuffer) const {

  auto isPrimary = bufferID && IsPrimarySourceID(bufferID);
  auto parsingOpts = GetSourceFileParsingOptions(isPrimary);
  auto sourceFile =
      SourceFile::Create(kind, bufferID, *mainModule, *astContext);

  // if (isMainBuffer)
  //   inputFile->SyntaxParsingCache =
  //   Invocation.getMainFileSyntaxParsingCache();

  // return inputFile;
  return sourceFile;
}

SourceFile::ParsingOptions
CompilerInstance::GetSourceFileParsingOptions(bool forPrimary) const {

  auto parsingOpts =
      SourceFile::GetDefaultParsingOptions(invocation.GetLangOptions());
  return parsingOpts;
}

/// TODO: returning true for now.
bool CompilerInstance::ShouldSetupASTContext() {
  return invocation.GetCompilerOptions().IsAnyAction(
      CompilerActionKind::Parse, CompilerActionKind::TypeCheck);
}

bool CompilerInstance::SetupASTContext() {

  astContext = std::make_unique<ASTContext>(
      invocation.GetLangOptions(), invocation.GetSearchPathOptions(),
      invocation.GetClangImporter(), invocation.GetDiags(), GetStats());

  return true;
}
void CompilerInstance::TryFreeASTContext() {

  // Just free for now
  FreeASTContext();
}
void CompilerInstance::FreeASTContext() {

  if (astContext) {
    astContext.reset();
  }
  mainModule = nullptr;
  primarySourceBufferIDList.clear();
}

void CompilerInstance::SetupStats() {

  const std::string &statsOutputDir =
      invocation.GetCompilerOptions().statsOutputDir;
  if (statsOutputDir.empty())
    return;

  const std::string &outputFile =
      invocation.GetCompilerOptions()
          .inputsAndOutputs.LastInputProducingOutput()
          .OutputFilename();

  stats = std::make_unique<StatsReporter>(
      "stone-compile", invocation.GetCompilerOptions().moduleOpts.moduleName,
      invocation.GetCompilerOptions()
          .inputsAndOutputs.GetStatsFileMangledInputName(),
      invocation.GetLangOptions().DefaultTargetTriple.normalize(),
      llvm::sys::path::extension(outputFile), "O", statsOutputDir,
      &invocation.GetSrcMgr(),
      &invocation.GetClangImporter().GetClangInstance().getSourceManager(),
      invocation.GetCompilerOptions().traceStats,
      invocation.GetCompilerOptions().profileEvents,
      invocation.GetCompilerOptions().profileEntities);
}

void CompilerInstance::ForEachSourceFileInMainModule(
    std::function<void(SourceFile &sourceFile)> notify) {
  for (auto moduleFile : GetMainModule()->GetFiles()) {
    if (auto *sourceFile = llvm::dyn_cast<SourceFile>(moduleFile)) {
      notify(*sourceFile);
    }
  }
}
void CompilerInstance::ForEachPrimarySourceFile(
    std::function<void(SourceFile &sourceFile)> notify) {
  for (auto moduleFile : GetPrimarySourceFiles()) {
    if (auto *sourceFile = llvm::dyn_cast<SourceFile>(moduleFile)) {
      notify(*sourceFile);
    }
  }
}

// Status CompilerInstance::ForEachSourceFileToTypeCheck(
//     std::function<Status(SourceFile &sourceFile)> notify) {
//   if (IsCompileForWholeModule()) {
//     ForEachSourceFileInMainModule([&](SourceFile &sourceFile) {
//       if (notify(sourceFile).IsError()) {
//         return Status::Error();
//       }
//     });
//   } else {
//     ForEachPrimarySourceFile([&](SourceFile &sourceFile) {
//       if (notify(sourceFile).IsError()) {
//         return Status::Error();
//       }
//     });
//   }
//   return Status();
// }
std::error_code CompilerInstance::CreateDirectory(std::string name) {
  return llvm::sys::fs::create_directories(name);
}