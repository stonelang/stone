#include "stone/Compile/Frontend.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/Mem.h"
#include "stone/Basic/SrcMgr.h"
#include "stone/Compile/FrontendListener.h"
#include "stone/Diag/FrontendDiagnostic.h"

#include "llvm/Support/BuryPointer.h"
#include "llvm/Support/CrashRecoveryContext.h"
#include "llvm/Support/Errc.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/LockFileManager.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/Program.h"
#include "llvm/Support/Signals.h"
#include "llvm/Support/TimeProfiler.h"
#include "llvm/Support/Timer.h"
#include "llvm/Support/raw_ostream.h"

using namespace stone;
using namespace stone::syn;
using namespace stone::opts;

Frontend::Frontend(llvm::StringRef programName, llvm::StringRef programPath,
                   FrontendListener *listener)
    : Session(programName, programPath), listener(listener) {
  excludedFlagsBitmask = opts::NoFrontendOption;

  stats = std::make_unique<FrontendStats>(*this);

  GetContext().GetStatEngine().Register(stats.get());
}
Frontend::~Frontend() {}

llvm::Optional<unsigned> Frontend::CreateCodeCompletionBuffer() {
  llvm::Optional<unsigned> codeCompletionBufferID;
  // auto codeCompletePoint = GetCodeCompletionPoint();
  // if (codeCompletePoint.first) {
  //   auto memBuf = codeCompletePoint.first;
  //   // CompilerInvocation doesn't own the buffers, copy to a new buffer.
  //   codeCompletionBufferID = SourceMgr.addMemBufferCopy(memBuf);
  //   InputSourceCodeBufferIDs.push_back(*codeCompletionBufferID);
  //   SourceMgr.setCodeCompletionPoint(*codeCompletionBufferID,
  //                                    codeCompletePoint.second);
  // }
  return codeCompletionBufferID;
}

Error Frontend::CreateSourceBuffers() {

  // Adds to InputSourceCodeBufferIDs, so may need to happen before the
  // per-input setup.
  const llvm::Optional<unsigned> codeCompletionBufferID =
      CreateCodeCompletionBuffer();

  const auto &inputs = GetFrontendOptions().inputsAndOutputs.GetInputs();
  const bool shouldRecover =
      GetFrontendOptions().inputsAndOutputs.ShouldRecoverMissingInputs();

  bool hasFailed = false;
  for (const FrontendInputFile &input : inputs) {
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
    return stone::Error(true);
  }

  return stone::Error();
}

llvm::Optional<unsigned>
Frontend::GetRecordedBufferID(const FrontendInputFile &input,
                              const bool shouldRecover, bool &failed) {
  if (!input.GetBuffer()) {
    if (llvm::Optional<unsigned> existingBufferID =
            ctx.GetSrcMgr().getIDForBufferIdentifier(input.GetFileName())) {
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
      ctx.GetSrcMgr().addNewSourceBuffer(std::move(buffers->moduleBuffer));

  sourceBufferIDs.push_back(bufferID);
  return bufferID;
}

// TODO:
llvm::Optional<ModuleBuffers>
Frontend::GetInputBuffersIfPresent(const FrontendInputFile &input) {

  if (auto b = input.GetBuffer()) {
    return ModuleBuffers(llvm::MemoryBuffer::getMemBufferCopy(
        b->getBuffer(), b->getBufferIdentifier()));
  }

  // FIXME: Working with filenames is fragile, maybe use the real path
  // or have some kind of FileManager.

  using InputFileOrError = llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>>;
  InputFileOrError inputFileOrError =
      ctx.GetFileMgr().getBufferForFile(input.GetFileName());

  if (!inputFileOrError) {
    ctx.GetDiagUnit().PrintD(SrcLoc(), diag::err_unable_to_open_buffer_for_file,
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
  //     /*Bad File Descriptor Retry*/getInvocation().getFrontendOptions()
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

unsigned Frontend::CreateSourceBuffer(const FrontendInputFile &input) {
  auto fb = ctx.GetFileMgr().getBufferForFile(input.GetFileName());
  if (!fb) {
    ctx.GetDiagUnit().PrintD(SrcLoc(), diag::err_unable_to_open_buffer_for_file,
                             diag::LLVMStr(input.GetFileName()));
  }
  auto srcID = ctx.GetSrcMgr().addNewSourceBuffer(std::move(*fb));
  assert((srcID > 0) && "Input file buffer ID must be greater than zero.");
  return srcID;
}

void Frontend::RecordPrimarySourceID(unsigned primarySourceID) {
  primarySourceIDs.insert(primarySourceID);
}

// std::unique_ptr<OutputFile>
// Frontend::ComputeOutputFile(FrontendUnit &source) {
//   stone::Panic("ComputeSourceOutputFile not implemented");
// }

void Frontend::Finish() {
  if (listener) {
    listener->OnCompileCompleted(*this);
  }
  // TODO: Print stats here.
}

void FrontendStats::Print(ColorfulStream &stream) {
  // if (sc.GetFrontendOpts().printStats) {
  //   // GetContext().Out() << GetName() << '\n';
  //   return;
  // }
}
