#include "stone/Compile/Compiler.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/Mem.h"
#include "stone/Basic/SrcMgr.h"
#include "stone/Compile/CompilerInputFile.h"
#include "stone/Diag/CompilerDiagnostic.h"
#include "stone/Option/ActionKind.h"
#include "stone/Parse/Lexer.h" // TODO: do better
#include "stone/Syntax/ClangContext.h"
#include "stone/Syntax/Module.h"

using namespace stone;

Compiler::Compiler()
    : invocation(*this), fileMgr(invocation.GetFileSystemOptions()) {}

Status Compiler::Setup() {

  assert(invocation.HasAction() && "Compiler does not have a valid action!");
  if (SetupCompilerInputFiles().IsError()) {
    Status::Error();
  }
  if (ShouldSetupASTContext()) {
    if (SetupASTContext().IsError()) {
      return Status::Error();
    }
    // For the time being, we ti MemoryContext to AST
    memContext = std::make_unique<MemoryContext>(invocation.GetLangOptions());
  }
  SetUpIsWholeModuleCompile();

  SetupStatsReporter();

  return Status();
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

void Compiler::SetMainModule(ModuleDecl *inputMainModule) {

  // TODO: This defaults to true for now
  assert(inputMainModule->IsMainModule());
  mainModule = inputMainModule;
  astContext->AddLoadedModule(mainModule);
}

Status Compiler::CreateSourceFilesForMainModule(
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

SourceFile *Compiler::ComputeMainSourceFileForModule(ModuleDecl *mod) const {
  // TODO:
  if (invocation.GetCompilerOptions().parsingInputMode ==
      CompilerOptions::ParsingInputMode::StoneLibrary) {
    return nullptr;
  }
  return nullptr;
}

// Sources
Status Compiler::SetupCompilerInputFiles() {

  // Adds to InputSourceCodeBufferIDs, so may need to happen before the
  // per-input setup.
  const llvm::Optional<unsigned> codeCompletionBufferID =
      CreateCodeCompletionBuffer();

  const auto &inputs =
      invocation.GetCompilerOptions().inputsAndOutputs.GetInputs();
  const bool shouldRecover = invocation.GetCompilerOptions()
                                 .inputsAndOutputs.ShouldRecoverMissingInputs();

  bool hasFailed = false;
  for (const CompilerInputFile &input : inputs) {
    bool failed = false;
    llvm::Optional<unsigned> bufferID =
        GetRecordedBufferID(input, shouldRecover, failed);
    hasFailed |= failed;

    if (!bufferID.hasValue() || !input.IsPrimary()) {
      continue;
    }
    RecordPrimarySourceID(*bufferID);
  }
  if (hasFailed) {
    return Status::Error();
  }
  return Status();
}

llvm::Optional<unsigned>
Compiler::GetRecordedBufferID(const CompilerInputFile &input,
                              const bool shouldRecover, bool &failed) {
  if (!input.GetBuffer()) {
    if (llvm::Optional<unsigned> existingBufferID =
            GetSrcMgr().getIDForBufferIdentifier(input.GetFileName())) {
      return existingBufferID;
    }
  }
  auto buffers = GetInputBuffersIfPresent(input);

  // Recover by dummy buffer if requested.
  if (!buffers.hasValue() && shouldRecover &&
      input.GetType() == file::Type::Stone) {
    buffers = ModuleBuffers(llvm::MemoryBuffer::getMemBuffer(
        "// missing file\n", input.GetFileName()));
  }

  if (!buffers.hasValue()) {
    failed = true;
    return llvm::None;
  }

  // FIXME: The fact that this test happens twice, for some cases,
  // suggests that setupInputs could use another round of refactoring.
  // TODO:
  // if (serialization::isSerializedAST(buffers->ModuleBuffer->getBuffer())) {
  //   PartialModules.push_back(std::move(*buffers));
  //   return None;
  // }

  // TODO
  // assert(buffers->moduleDocBuffer.get() == nullptr);
  // assert(buffers->moduleSourceInfoBuffer.get() == nullptr);

  // Transfer ownership of the MemoryBuffer to the SourceMgr.
  unsigned bufferID =
      GetSrcMgr().addNewSourceBuffer(std::move(buffers->moduleBuffer));

  inputSourceBufferIDList.push_back(bufferID);
  return bufferID;
}

// unsigned Compiler::CreateBufferIDForCompilerInputFile(const CompilerInputFile
// &input) {
//   auto fb = GetFileMgr().getBufferForFile(input.GetFileName());
//   if (!fb) {
//     GetDiags().PrintD(SrcLoc(), diag::err_unable_to_open_buffer_for_file,
//                       diag::LLVMStr(input.GetFileName()));
//   }
//   auto srcID = astContext->GetSrcMgr().addNewSourceBuffer(std::move(*fb));
//   assert((srcID > 0) && "Input file buffer ID must be greater than zero.");
//   return srcID;
// }

void Compiler::RecordPrimarySourceID(unsigned primarySourceID) {
  primarySourceBufferIDList.insert(primarySourceID);
}

// TODO:
llvm::Optional<ModuleBuffers>
Compiler::GetInputBuffersIfPresent(const CompilerInputFile &input) {

  if (auto b = input.GetBuffer()) {
    return ModuleBuffers(llvm::MemoryBuffer::getMemBufferCopy(
        b->getBuffer(), b->getBufferIdentifier()));
  }

  // FIXME: Working with filenames is fragile, maybe use the real path
  // or have some kind of FileManager.

  using InputFileOrError = llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>>;
  InputFileOrError inputFileOrError =
      GetFileMgr().getBufferForFile(input.GetFileName());

  if (!inputFileOrError) {
    GetDiags().PrintD(SrcLoc(), diag::err_unable_to_open_buffer_for_file,
                      diag::LLVMStr(input.GetFileName()));
    return llvm::None;
  }

  // Just return the file buffer for now
  return ModuleBuffers(std::move(*inputFileOrError));
  // if (!fb) {
  //   ctx.GetDiagUnit().PrintD(SrcLoc(),
  //   diag::err_unable_to_open_buffer_for_file,
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
  //   return None;
  // }
  // if (!serialization::isSerializedAST((*inputFileOrErr)->getBuffer()))
  //   return ModuleBuffers(std::move(*inputFileOrErr));

  // auto swiftdoc = openModuleDoc(input);
  // auto sourceinfo = openModuleSourceInfo(input);
  // return ModuleBuffers(std::move(*inputFileOrErr),
  //                      swiftdoc.hasValue() ? std::move(swiftdoc.getValue())
  //                      : nullptr, sourceinfo.hasValue() ?
  //                      std::move(sourceinfo.getValue()) : nullptr);

  // return llvm::None;
}

llvm::Optional<unsigned> Compiler::CreateCodeCompletionBuffer() {
  llvm::Optional<unsigned> codeCompletionBufferID;
  return codeCompletionBufferID;
}

SourceFile *Compiler::CreateSourceFileForMainModule(ModuleDecl *mainModule,
                                                    SourceFileKind kind,
                                                    unsigned bufferID,
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
Compiler::GetSourceFileParsingOptions(bool forPrimary) const {

  auto parsingOpts =
      SourceFile::GetDefaultParsingOptions(invocation.GetLangOptions());
  return parsingOpts;
}

// TODO: return true for now
bool Compiler::ShouldSetupASTContext() { return true; }

Status Compiler::SetupASTContext() {

  astContext = std::make_unique<ASTContext>(
      invocation.GetLangOptions(), invocation.GetSearchPathOptions(),
      invocation.GetClangContext(), GetDiags(), GetStats());
}
void Compiler::TryFreeASTContext() {

  // Just free for now
  FreeASTContext();
}
void Compiler::FreeASTContext() {

  if (astContext) {
    astContext.reset();
  }
  mainModule = nullptr;
  primarySourceBufferIDList.clear();
}

void Compiler::FreeMemoryContext() {
  if (memContext) {
    memContext.reset();
  }
}

void Compiler::SetupStatsReporter() {

  const std::string &statsOutputDir =
      invocation.GetCompilerOptions().statsOutputDir;
  if (statsOutputDir.empty())
    return;

  const std::string &outputFile =
      invocation.GetCompilerOptions()
          .inputsAndOutputs.LastInputProducingOutput()
          .OutputFilename();

  statsReporter = std::make_unique<CompilerStatsReporter>(
      invocation.GetCompilerOptions().moduleOpts.moduleName,
      invocation.GetCompilerOptions()
          .inputsAndOutputs.GetStatsFileMangledInputName(),
      invocation.GetLangOptions().Target.normalize(),
      llvm::sys::path::extension(outputFile), "O", statsOutputDir, &GetSrcMgr(),
      &invocation.GetClangContext().GetInstance().getSourceManager(),
      invocation.GetCompilerOptions().traceStats,
      invocation.GetCompilerOptions().profileEvents,
      invocation.GetCompilerOptions().profileEntities);
}

Status Compiler::ForEachSourceFileInMainModule(
    std::function<Status(SourceFile &sourceFile)> notify) {
  for (auto moduleFile : GetMainModule()->GetFiles()) {
    if (auto *sourceFile = llvm::dyn_cast<SourceFile>(moduleFile)) {
      if (notify(*sourceFile).IsError()) {
        return Status::Error();
      }
    } else {
      return Status::Error();
    }
  }
  return Status();
}

Status Compiler::ForEachPrimarySourceFile(
    std::function<Status(SourceFile &sourceFile)> notify) {
  for (auto moduleFile : GetPrimarySourceFiles()) {
    if (auto *sourceFile = llvm::dyn_cast<SourceFile>(moduleFile)) {
      if (notify(*sourceFile).IsError()) {
        return Status::Error();
      }
    } else {
      return Status::Error();
    }
  }
  return Status();
}

Status Compiler::ForEachSourceFileToTypeCheck(
    std::function<Status(SourceFile &sourceFile)> notify) {
  if (IsCompileForWholeModule()) {
    ForEachSourceFileInMainModule([&](SourceFile &sourceFile) {
      if (notify(sourceFile).IsError()) {
        return Status::Error();
      }
    });
  } else {
    ForEachPrimarySourceFile([&](SourceFile &sourceFile) {
      if (notify(sourceFile).IsError()) {
        return Status::Error();
      }
    });
  }
  return Status();
}

Status Compiler::IsValidModuleName(const llvm::StringRef moduleName) {
  if (!Lexer::isIdentifier(moduleName)) {
    return Status::Error();
  }
  return Status();
}

void CompilerStatsReporter::CountASTStats(Compiler &compiler) {}

void CompilerStatsReporter::CountDeclStats(Compiler &compiler) {}

void CompilerStatsReporter::CountExprStats(Compiler &compiler) {}

void CompilerStatsReporter::CountTypeStats(Compiler &compiler) {}

void CompilerStatsReporter::CountSourceFileStats(Compiler &compiler) {}
