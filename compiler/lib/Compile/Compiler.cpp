#include "stone/Compile/Compiler.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/Memory.h"
#include "stone/Basic/SrcMgr.h"
#include "stone/Compile/CompilerExecution.h"
#include "stone/Compile/CompilerInputFile.h"
#include "stone/Compile/CompilerInvocation.h"
#include "stone/Compile/CompilerObservation.h"
#include "stone/Parse/Lexer.h" // TODO: do better
#include "stone/Support/CompilerDiagnostic.h"
#include "stone/Syntax/ClangContext.h"
#include "stone/Syntax/Module.h"

using namespace stone;
using namespace stone::file;

Compiler::Compiler(CompilerInvocation &invocation) : invocation(invocation) {}

Status Compiler::Setup() {

  assert(invocation.GetCompilerOptions().HasMainAction() &&
         "Compiler does not have a valid action!");

  if (SetupCompilerInputFiles().IsError()) {
    Status::Error();
  }
  if (ShouldSetupASTContext()) {
    if (SetupASTContext().IsError()) {
      return Status::Error();
    }
  }
  SetupStats();
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
  std::optional<unsigned> mainBufferID = std::nullopt;
  if (SourceFile *mainSourceFile = ComputeMainSourceFileForModule(mod)) {
    mainBufferID = mainSourceFile->GetBufferID();
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
Compiler::GetRecordedBufferID(const CompilerInputFile &input,
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

// unsigned Compiler::CreateBufferIDForCompilerInputFile(const CompilerInputFile
// &input) {
//   auto fb = GetFileMgr().getBufferForFile(input.GetFileName());
//   if (!fb) {
//     GetDiags().PrintD(SrcLoc(), diag::error_unable_to_open_buffer_for_file,
//                       diag::LLVMStr(input.GetFileName()));
//   }
//   auto bufferID = astContext->GetSrcMgr().addNewSourceBuffer(std::move(*fb));
//   assert((bufferID > 0) && "Input file buffer ID must be greater than
//   zero."); return bufferID;
// }

void Compiler::RecordPrimarySourceID(unsigned primarySourceID) {
  primarySourceBufferIDList.insert(primarySourceID);
}

// TODO:
std::optional<ModuleBuffers>
Compiler::GetInputBuffersIfPresent(const CompilerInputFile &input) {

  if (auto b = input.GetBuffer()) {
    return ModuleBuffers(llvm::MemoryBuffer::getMemBufferCopy(
        b->getBuffer(), b->getBufferIdentifier()));
  }

  // FIXME: Working with filenames is fragile, maybe use the real path
  // or have some kind of FileManager.

  using InputFileOrError = llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>>;
  InputFileOrError inputFileOrError =
      invocation.GetFileMgr().getBufferForFile(input.GetFileName());

  if (!inputFileOrError) {
    invocation.GetDiags().PrintD(SrcLoc(),
                                 diag::error_unable_to_open_buffer_for_file,
                                 diag::LLVMStr(input.GetFileName()));
    return std::nullopt;
  }

  // Just return the file buffer for now
  return ModuleBuffers(std::move(*inputFileOrError));
  // if (!fb) {
  //   ctx.GetDiagUnit().PrintD(SrcLoc(),
  //   diag::error_unable_to_open_buffer_for_file,
  //                            diag::LLVMStr(input.GetFileName()));
  // }
  // auto bufferID = ctx.GetSrcMgr().addNewSourceBuffer(std::move(*fb));
  // assert((bufferID > 0) && "Input file buffer ID must be greater than
  // zero."); return bufferID;

  // FileOrError inputFileOrErr =
  //   swift::vfs::getFileOrSTDIN(getFileSystem(), input.getFileName(),
  //                             /*FileSize*/-1,
  //                             /*RequiresNullTerminator*/true,
  //                             /*IsVolatile*/false,
  //     /*Bad File Descriptor Retry*/getInvocation().getCompilerOptions()
  //                              .BadFileDescriptorRetryCount);
  // if (!inputFileOrErr) {
  //   Diagnostics.diagnose(SourceLoc(), diag::erroror_open_input_file,
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

std::optional<unsigned> Compiler::CreateCodeCompletionBuffer() {
  std::optional<unsigned> codeCompletionBufferID;
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
      invocation.GetClangContext(), invocation.GetDiags(), GetStats());
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

void Compiler::SetupStats() {

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

Status Compiler::ExecuteAction(CompilerExecution &execution) {

  if (execution.SetupAction().IsError()) {
    return Status::MakeHasCompletionAndIsError();
  }
  if (execution.ExecuteAction().IsError()) {
    return Status::MakeHasCompletionAndIsError();
  }
  return execution.FinishAction();
}

Status Compiler::ExecuteAction(CompilerAction action) {
  auto execution = CreateExectution(action);
  return ExecuteAction(*execution);
}

std::unique_ptr<CompilerExecution>
Compiler::CreateExectution(CompilerAction action) {
  switch (action) {
  case CompilerAction::PrintHelp:
    return std::make_unique<PrintHelpExecution>(*this);
  case CompilerAction::PrintHelpHidden:
    return std::make_unique<PrintHelpHiddenExecution>(*this);
  case CompilerAction::PrintVersion:
    return std::make_unique<PrintVersionExecution>(*this);
  case CompilerAction::PrintFeature:
    return std::make_unique<PrintFeatureExecution>(*this);
  case CompilerAction::Parse:
    return std::make_unique<ParseExecution>(*this);
  case CompilerAction::PrintASTBefore:
    return std::make_unique<PrintASTBeforeExecution>(*this);
  case CompilerAction::ResolveImports:
    return std::make_unique<ResolveImportsExecution>(*this);
  case CompilerAction::TypeCheck:
    return std::make_unique<TypeCheckExecution>(*this);
  case CompilerAction::PrintASTAfter:
    return std::make_unique<PrintASTAfterExecution>(*this);
  case CompilerAction::EmitIRBefore:
    return std::make_unique<EmitIRBeforeExecution>(*this);
  case CompilerAction::EmitIRAfter:
    return std::make_unique<EmitIRAfterExecution>(*this);
  case CompilerAction::EmitModule:
    return std::make_unique<EmitModuleExecution>(*this);
  case CompilerAction::MergeModules:
    return std::make_unique<MergeModulesExecution>(*this);
  case CompilerAction::EmitBC:
    return std::make_unique<EmitBitCodeExecution>(*this);
  case CompilerAction::EmitAssembly:
    return std::make_unique<EmitAssemblyExecution>(*this);
  case CompilerAction::EmitObject:
    return std::make_unique<EmitObjectExecution>(*this);
  }
  llvm_unreachable("Unable to create CompilerExecution -- unknon action!");
}

void Compiler::PrintHelp(bool showHidden) const {

  unsigned IncludedFlagsBitmask = 0;
  unsigned ExcludedFlagsBitmask = stone::opts::ExcludeCompilerOption;

  if (!showHidden) {
    ExcludedFlagsBitmask |= llvm::opt::HelpHidden;
  }
  invocation.GetOptTable().printHelp(
      llvm::outs(),
      invocation.GetCompilerOptions().GetMainExecutableName().data(),
      "Stone is a compiler tool for compiling Stone source code.",
      IncludedFlagsBitmask, ExcludedFlagsBitmask,
      /*ShowAllAliases*/ false);
}