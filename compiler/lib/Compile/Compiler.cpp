#include "stone/Compile/Compiler.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/Mem.h"
#include "stone/Basic/SrcMgr.h"
#include "stone/Compile/CompilerInputFile.h"
#include "stone/Diag/CompilerDiagnostic.h"
#include "stone/Option/ActionKind.h"
#include "stone/Parse/Lexer.h" // TODO: do better
#include "stone/Public.h"
#include "stone/Syntax/ClangContext.h"
#include "stone/Syntax/Module.h"

using namespace stone;

Compiler::Compiler()
    : invocation(*this), fileMgr(invocation.GetFileSystemOptions()),
      clangContext(new ClangContext()) {}

void Compiler::Setup() { assert(invocation.HasAction()); }

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

// Sources
Status Compiler::CreateSourceBuffers() {

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
  unsigned bufferID = astContext->GetSrcMgr().addNewSourceBuffer(
      std::move(buffers->moduleBuffer));

  sourceBufferIDs.push_back(bufferID);
  return bufferID;
}

unsigned Compiler::CreateSourceBuffer(const CompilerInputFile &input) {
  auto fb = GetFileMgr().getBufferForFile(input.GetFileName());
  if (!fb) {
    GetDiags().PrintD(SrcLoc(), diag::err_unable_to_open_buffer_for_file,
                      diag::LLVMStr(input.GetFileName()));
  }
  auto srcID = astContext->GetSrcMgr().addNewSourceBuffer(std::move(*fb));
  assert((srcID > 0) && "Input file buffer ID must be greater than zero.");
  return srcID;
}

void Compiler::RecordPrimarySourceID(unsigned primarySourceID) {
  primarySourceIDs.insert(primarySourceID);
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
  //                      swiftdoc.hasValue() ? std::move(swiftdoc.getValue()) :
  //                      nullptr, sourceinfo.hasValue() ?
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

SourceFile *Compiler::ComputeMainSourceFileForModule(ModuleDecl *mod) const {
  return nullptr;
}

SourceFile::ParsingOptions
Compiler::GetSourceFileParsingOptions(bool forPrimary) const {

  auto parsingOpts =
      SourceFile::GetDefaultParsingOptions(invocation.GetLangOptions());
  return parsingOpts;
}

Status Compiler::IsValidModuleName(const llvm::StringRef moduleName) {
  if (!Lexer::isIdentifier(moduleName)) {
    return Status::Error();
  }
  return Status();
}

bool Compiler::ShouldSetupClang() {
  return invocation.GetAction().IsAny(
      ActionKind::EmitIRBefore, ActionKind::EmitIRAfter, ActionKind::EmitBC,
      ActionKind::EmitAssembly, ActionKind::EmitObject);
}