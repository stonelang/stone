#include "stone/Compile/Compiler.h"

Status Compiler::SetupSources() {

  //   // Adds to InputSourceCodeBufferIDs, so may need to happen before the
  //   // per-input setup.
  //   const llvm::Optional<unsigned> codeCompletionBufferID =
  //       CreateCodeCompletionBuffer();

  //   const auto &inputs = GetCompilerOptions().inputsAndOutputs.GetInputs();
  //   const bool shouldRecover =
  //       GetCompilerOptions().inputsAndOutputs.ShouldRecoverMissingInputs();

  //   bool hasFailed = false;
  //   for (const CompilerInputFile &input : inputs) {
  //     bool failed = false;
  //     llvm::Optional<unsigned> bufferID =
  //         GetRecordedBufferID(input, shouldRecover, failed);
  //     hasFailed |= failed;

  //     if (!bufferID.hasValue() || !input.IsPrimary()) {
  //       continue;
  //     }
  //     RecordPrimarySourceID(*bufferID);
  //   }
  //   if (hasFailed) {
  //     return Status::Error();
  //   }
  return Status();
}

// llvm::Optional<unsigned> Compiler::GetRecordedBufferID(
//     const CompilerInputFile &input, const bool shouldRecover, bool &failed) {
//   if (!input.GetBuffer()) {
//     if (llvm::Optional<unsigned> existingBufferID =
//             GetLangContext().GetSrcMgr().getIDForBufferIdentifier(
//                 input.GetFileName())) {
//       return existingBufferID;
//     }
//   }
//   auto buffers = GetInputBuffersIfPresent(input);

//   // Recover by dummy buffer if requested.
//   if (!buffers.hasValue() && shouldRecover &&
//       input.GetType() == file::Type::Stone) {
//     buffers = ModuleBuffers(llvm::MemoryBuffer::getMemBuffer(
//         "// missing file\n", input.GetFileName()));
//   }

//   if (!buffers.hasValue()) {
//     failed = true;
//     return llvm::None;
//   }

// FIXME: The fact that this test happens twice, for some cases,
// suggests that setupInputs could use another round of refactoring.
// TODO:
// if (serialization::isSerializedAST(buffers->ModuleBuffer->getBuffer())){
//   PartialModules.push_back(std::move(*buffers));
//   return None;
// }

// TODO
// assert(buffers->moduleDocBuffer.get() == nullptr);
// assert(buffers->moduleSourceInfoBuffer.get() == nullptr);

// Transfer ownership of the MemoryBuffer to the SourceMgr.
//   unsigned bufferID = GetLangContext().GetSrcMgr().addNewSourceBuffer(
//       std::move(buffers->moduleBuffer));

//   sourceBufferIDs.push_back(bufferID);
//   return bufferID;
// }

// // TODO:
// llvm::Optional<ModuleBuffers>
// Compiler::GetInputBuffersIfPresent(
//     const CompilerInputFile &input) {

//   // if (auto b = input.GetBuffer()) {
//   //   return ModuleBuffers(llvm::MemoryBuffer::getMemBufferCopy(
//   //       b->getBuffer(), b->getBufferIdentifier()));
//   // }

//   // // FIXME: Working with filenames is fragile, maybe use the real path
//   // // or have some kind of FileManager.

//   // using InputFileOrError =
//   // llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>>; InputFileOrError
//   // inputFileOrError =
//   //     GetLangContext().GetFileMgr().getBufferForFile(input.GetFileName());

//   // if (!inputFileOrError) {
//   //   GetLangContext().GetDiags().PrintD(SrcLoc(),
//   // diag::err_unable_to_open_buffer_for_file,
//   // diag::LLVMStr(input.GetFileName()));
//   //   return llvm::None;
//   // }

//   // // Just return the file buffer for now
//   // return ModuleBuffers(std::move(*inputFileOrError));
//   // if (!fb) {
//   //   GetLangContext().GetDiags().PrintD(SrcLoc(),
//   //   diag::err_unable_to_open_buffer_for_file,
//   //                            diag::LLVMStr(input.GetFileName()));
//   // }
//   // auto srcID =
//   // GetLangContext().GetSrcMgr().addNewSourceBuffer(std::move(*fb));
//   // assert((srcID > 0) && "Input file buffer ID must be greater thanzero.");
//   // return srcID;

//   // FileOrError inputFileOrErr =
//   //   swift::vfs::getFileOrSTDIN(getFileSystem(), input.getFileName(),
//   //                             /*FileSize*/-1,
//   //                             /*RequiresNullTerminator*/true,
//   //                             /*IsVolatile*/false,
//   //     /*Bad File Descriptor Retry*/getInvocation().getCompilerOptions()
//   //                              .BadFileDescriptorRetryCount);
//   // if (!inputFileOrErr) {
//   //   Diagnostics.diagnose(SourceLoc(), diag::error_open_input_file,
//   //                        input.getFileName(),
//   //                        inputFileOrErr.getError().message());
//   //   return None;
//   // }
//   // if (!serialization::isSerializedAST((*inputFileOrErr)->getBuffer()))
//   //   return ModuleBuffers(std::move(*inputFileOrErr));

//   // auto swiftdoc = openModuleDoc(input);
//   // auto sourceinfo = openModuleSourceInfo(input);
//   // return ModuleBuffers(std::move(*inputFileOrErr),
//   //                      swiftdoc.hasValue() ?
//   std::move(swiftdoc.getValue()) :
//   //                      nullptr, sourceinfo.hasValue() ?
//   //                      std::move(sourceinfo.getValue()) : nullptr);

//   return llvm::None;
// }
