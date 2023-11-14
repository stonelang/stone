#include "stone/Basic/Defer.h"
#include "stone/Basic/Mem.h"
#include "stone/Basic/SrcMgr.h"
#include "stone/Compile/CompilerOptions.h"
#include "stone/Compile/CompilerOptionsConverter.h"
#include "stone/Diag/CompilerDiagnostic.h"
#include "stone/Public.h"

#include "clang/Basic/Stack.h"
#include "clang/Basic/TargetOptions.h"
#include "clang/CodeGen/ObjectFilePCHContainerOperations.h"
#include "clang/Config/config.h"
#include "clang/Driver/DriverDiagnostic.h"
#include "clang/Driver/Options.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/CompilerInvocation.h"
#include "clang/Frontend/FrontendDiagnostic.h"
#include "clang/Frontend/TextDiagnosticBuffer.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"
#include "clang/Frontend/Utils.h"

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

// CompilerConfiguration::CompilerConfiguration()
//     : clangContext(new ClangContext()), langContext(langOpts) {
//   SetTargetTriple(llvm::sys::getDefaultTargetTriple());
// }

// CompilerConfiguration::~CompilerConfiguration() {}

// llvm::Optional<unsigned> CompilerConfiguration::CreateCodeCompletionBuffer()
// {
//   llvm::Optional<unsigned> codeCompletionBufferID;
//   // auto codeCompletePoint = GetCodeCompletionPoint();
//   // if (codeCompletePoint.first) {
//   //   auto memBuf = codeCompletePoint.first;
//   //   // CompilerConfiguration doesn't own the buffers, copy to a new
//   buffer.
//   //   codeCompletionBufferID = SourceMgr.addMemBufferCopy(memBuf);
//   //   InputSourceCodeBufferIDs.push_back(*codeCompletionBufferID);
//   //   SourceMgr.setCodeCompletionPoint(*codeCompletionBufferID,
//   //                                    codeCompletePoint.second);
//   // }
//   return codeCompletionBufferID;
// }

// void CompilerConfiguration::SetMainExecutable(const char *arg0,
//                                               void *mainAddr) {

//   // TODO:
//   //
//   GetCompilerOptions().MainExecutablePath(llvm::sys::fs::getMainExecutable(arg0,
//   //  mainAddr)); GetCompilerOptions().MainExecutableName =
//   //      file::GetStem(GetCompilerOptions().MainExecutablePath);
// }

// void CompilerConfiguration::SetupWorkingDirectory() {
//   llvm::sys::fs::current_path(GetCompilerOptions().workDirectory);
// }

// Status CompilerConfiguration::CreateSourceBuffers() {

//   // Adds to InputSourceCodeBufferIDs, so may need to happen before the
//   // per-input setup.
//   // const llvm::Optional<unsigned> codeCompletionBufferID =
//   //     CreateCodeCompletionBuffer();

//   // const auto &inputs = GetCompilerOptions().inputsAndOutputs.GetInputs();
//   // const bool shouldRecover =
//   //     GetCompilerOptions().inputsAndOutputs.ShouldRecoverMissingInputs();

//   // bool hasFailed = false;
//   // for (const CompilerInputFile &input : inputs) {
//   //   bool failed = false;
//   //   llvm::Optional<unsigned> bufferID =
//   //       GetRecordedBufferID(input, shouldRecover, failed);
//   //   hasFailed |= failed;

//   //   if (!bufferID.hasValue() || !input.IsPrimary()) {
//   //     continue;
//   //   }
//   //   RecordPrimarySourceID(*bufferID);
//   // }
//   // if (hasFailed) {
//   //   return Status::Error();
//   // }

//   // return stone::Status();
//   return Status();
// }

// llvm::Optional<unsigned> CompilerConfiguration::GetRecordedBufferID(
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

//   // FIXME: The fact that this test happens twice, for some cases,
//   // suggests that setupInputs could use another round of refactoring.
//   // TODO:
//   // if (serialization::isSerializedAST(buffers->ModuleBuffer->getBuffer()))
//   {
//   //   PartialModules.push_back(std::move(*buffers));
//   //   return None;
//   // }

//   // TODO
//   // assert(buffers->moduleDocBuffer.get() == nullptr);
//   // assert(buffers->moduleSourceInfoBuffer.get() == nullptr);

//   // Transfer ownership of the MemoryBuffer to the SourceMgr.
//   unsigned bufferID = GetLangContext().GetSrcMgr().addNewSourceBuffer(
//       std::move(buffers->moduleBuffer));

//   sourceBufferIDs.push_back(bufferID);
//   return bufferID;
// }

// // TODO:
// llvm::Optional<ModuleBuffers>
// CompilerConfiguration::GetInputBuffersIfPresent(
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
//   // assert((srcID > 0) && "Input file buffer ID must be greater than
//   zero.");
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

// void CompilerConfiguration::SetTargetTriple(StringRef Triple) {
//   SetTargetTriple(llvm::Triple(Triple));
// }
// void CompilerConfiguration::SetTargetTriple(const llvm::Triple &triple) {
//   GetLangContext().GetLangOptions().SetTarget(triple);
//   // TODO? UpdateRuntimeLibraryPaths(SearchPathOpts, LangOpts.Target);
// }

// unsigned
// CompilerConfiguration::CreateSourceBuffer(const CompilerInputFile &input) {
//   // TODO: Remove
//   //  auto fb =
//   //  GetLangContext().GetFileMgr().getBufferForFile(input.GetFileName()); if
//   //  (!fb) {
//   //    GetLangContext().GetDiags().PrintD(SrcLoc(),
//   // diag::err_unable_to_open_buffer_for_file,
//   // diag::LLVMStr(input.GetFileName()));
//   //  }
//   //  auto srcID =
//   //  GetLangContext().GetSrcMgr().addNewSourceBuffer(std::move(*fb));
//   //  assert((srcID > 0) && "Input file buffer ID must be greater than
//   zero.");
//   //  return srcID;
// }

// void CompilerConfiguration::RecordPrimarySourceID(unsigned primarySourceID) {
//   primarySourceIDs.insert(primarySourceID);
// }

// // std::unique_ptr<OutputFile>
// // CompilerConfiguration::ComputeOutputFile(CompilerUnit &source) {
// //   stone::Panic("ComputeSourceOutputFile not implemented");
// // }

// Status CompilerConfiguration::SetupClang(llvm::ArrayRef<const char *> argv,
//                                          const char *arg0) {
//   // Setup the clang diagnostics
//   clang::IntrusiveRefCntPtr<clang::DiagnosticIDs> DiagID(
//       new clang::DiagnosticIDs());
//   IntrusiveRefCntPtr<clang::DiagnosticOptions> DiagOpts =
//       new clang::DiagnosticOptions();
//   clang::TextDiagnosticBuffer *DiagsBuffer = new clang::TextDiagnosticBuffer;
//   clang::DiagnosticsEngine Diags(DiagID, &*DiagOpts, DiagsBuffer);

//   bool Success = clang::CompilerInvocation::CreateFromArgs(
//       GetClangContext().GetInstance().getInvocation(), argv, Diags, arg0);
//   if (!Success) {
//     return Status::Error();
//   }

//   // Create the actual diagnostics engine.
//   GetClangContext().GetInstance().createDiagnostics();
//   if (!GetClangContext().GetInstance().hasDiagnostics()) {
//     return Status::Error();
//   }

//   DiagsBuffer->FlushDiagnostics(
//       GetClangContext().GetInstance().getDiagnostics());
//   if (!Success) {
//     GetClangContext().GetInstance().getDiagnosticClient().finish();
//     return Status::Error();
//   }
//   // If there were errors in processing arguments, don't do anything else.
//   if (GetClangContext().GetInstance().getDiagnostics().hasErrorOccurred()) {
//     return Status::Error();
//   }

//   // Set up the file and source managers, if needed.
//   if (!GetClangContext().GetInstance().hasFileManager()) {
//     assert(GetClangContext().GetInstance().createFileManager());
//   }
//   if (!GetClangContext().GetInstance().hasSourceManager()) {
//     GetClangContext().GetInstance().createSourceManager(
//         GetClangContext().GetInstance().getFileManager());
//   }

//   assert(GetClangContext().GetInstance().createTarget());

//   return Status();
// }

// // TODO:
// //  static void SetPointerAuthOptions(
// //      PointerAuthOptions &pointerAuthOptions,
// //      const clang::PointerAuthOptions &clangPointerAuthOptions) {

// //   // Intentionally do a slice-assignment to copy over the clang options.
// //   static_cast<clang::PointerAuthOptions &>(pointerAuthOptions) =
// //       clangPointerAuthOptions;
// // }

// // TODO: See clang
// static void InitLLVMTargetOptions(llvm::TargetOptions &llvmTargetOpts,
//                                   const CodeGenOptions &codeGenOpts,
//                                   const LangOptions &langOpts) {

//   // Explicitly request debugger tuning for LLDB which is the default
//   // on Darwin platforms but not on others.
//   llvmTargetOpts.DebuggerTuning = llvm::DebuggerKind::LLDB;
//   llvmTargetOpts.FunctionSections = codeGenOpts.functionSections;

//   switch (langOpts.threadModelKind) {
//   case LangOptions::ThreadModelKind::POSIX:
//     llvmTargetOpts.ThreadModel = llvm::ThreadModel::POSIX;
//     break;
//   case LangOptions::ThreadModelKind::Single:
//     llvmTargetOpts.ThreadModel = llvm::ThreadModel::Single;
//     break;
//   }
//   // TODO:
//   // Set float ABI type.
//   // assert((CodeGenOpts.FloatABI == "soft" || CodeGenOpts.FloatABI ==
//   "softfp"
//   // ||
//   //         CodeGenOpts.FloatABI == "hard" || CodeGenOpts.FloatABI.empty())
//   &&
//   //        "Invalid Floating Point ABI!");
//   // Options.FloatABIType =
//   //     llvm::StringSwitch<llvm::FloatABI::ABIType>(CodeGenOpts.FloatABI)
//   //         .Case("soft", llvm::FloatABI::Soft)
//   //         .Case("softfp", llvm::FloatABI::Soft)
//   //         .Case("hard", llvm::FloatABI::Hard)
//   //         .Default(llvm::FloatABI::Default);
// }
// IRTargetOptions stone::GetIRTargetOptions(const CodeGenOptions &codeGenOpts,
//                                           const LangOptions &langOpts,
//                                           ClangContext &cc) {
//   llvm::TargetOptions llvmTargetOpts;
//   InitLLVMTargetOptions(llvmTargetOpts, codeGenOpts, langOpts);

//   clang::TargetOptions &clangTargetOpts =
//       cc.GetInstance().getTarget().getTargetOpts();
//   return std::make_tuple(llvmTargetOpts, clangTargetOpts.CPU,
//                          clangTargetOpts.Features, clangTargetOpts.Triple);
// }

// Status
// CompilerConfiguration::ParseCommandLine(llvm::ArrayRef<const char *> args,
//                                         const char *arg0) {

//   unsigned includedFlagsBitmask = 0;
//   unsigned excludedFlagsBitmask;
//   unsigned missingArgIndex;
//   unsigned missingArgCount;

//   /// May want to do this last?
//   if (SetupClang(args, arg0).IsError()) {
//     return Status::Error();
//   }

//   auto compilerOptionTable = opts::CreateOptTable();
//   auto compilerInputArgList =
//       std::make_unique<llvm::opt::InputArgList>(compilerOptionTable->ParseArgs(
//           args, missingArgIndex, missingArgCount, includedFlagsBitmask,
//           excludedFlagsBitmask));

//   assert(compilerInputArgList && "No input argument list.");

//   if (missingArgCount) {
//     GetLangContext().GetDiags().PrintD(
//         SrcLoc(), diag::err_missing_arg_value,
//         diag::LLVMStr(compilerInputArgList->getArgString(missingArgIndex)),
//         diag::UInt(missingArgCount));
//     return Status::Error();
//   }
//   // Check for unknown arguments.
//   for (const llvm::opt::Arg *arg :
//        compilerInputArgList->filtered(opts::UNKNOWN)) {
//     GetLangContext().GetDiags().PrintD(
//         SrcLoc(), diag::err_unknown_arg,
//         diag::LLVMStr(arg->getAsString(*compilerInputArgList)));
//   }
//   // Ok for now.
//   if (GetLangContext().GetDiags().HasError()) {
//     return Status::Error();
//   }
//   if (ParseCompilerAction(*compilerInputArgList).IsError()) {
//     return Status::Error();
//   }

//   if (ParseCompilerOptions(*compilerInputArgList,
//                            nullptr /* pass null for now*/)
//           .IsError()) {

//     return Status::Error();
//   }

//   if (ParseTypeCheckerOptions(*compilerInputArgList).IsError()) {
//     return Status::Error();
//   }

//   if (ParseSearchPathOptions(*compilerInputArgList).IsError()) {

//     return Status::Error();
//   }

//   if (ParseCodeGenOptions(*compilerInputArgList).IsError()) {
//     return Status::Error();
//   }

//   if (ParseTargetOptions(*compilerInputArgList).IsError()) {
//     return Status::Error();
//   }

//   // CreateSourceBuffers();

//   return Status();
// }

// Status
// CompilerConfiguration::ParseCompilerAction(llvm::opt::InputArgList &args) {
//   auto actionArg = args.getLastArg(opts::ModeGroup);
//   if (actionArg) {
//     auto actionKind =
//     opts::GetActionKindByOptionID(opts::GetArgID(actionArg));
//     GetAction().SetKind(actionKind);
//     if (GetAction().IsAlien()) {
//       return Status::Error();
//     }
//     GetAction().SetName(opts::GetArgName(actionArg));
//   }
//   return Status();
// }

// Status
// CompilerConfiguration::ParseCompilerOptions(llvm::opt::InputArgList &args,
//                                             MemoryBuffers *buffers) {

//   CompilerOptionsConverter converter(GetLangContext().GetDiags(), args,
//                                      GetLangContext().GetLangOptions(),
//                                      GetCompilerOptions(),
//                                      GetModuleOptions());
//   return converter.Convert(buffers);
// }
// Status CompilerConfiguration::ParseLangOptions(llvm::opt::InputArgList &args)
// {
//   return Status();
// }

// Status
// CompilerConfiguration::ParseTypeCheckerOptions(llvm::opt::InputArgList &args)
// {
//   return Status();
// }
// Status
// CompilerConfiguration::ParseSearchPathOptions(llvm::opt::InputArgList &args)
// {
//   return Status();
// }

// static void ComputeCodeCodeGenOutputKind(const CompilerOptions &compilerOpts,
//                                          CodeGenOptions &codeGenOpts) {

//   // TODO: You are missing a few -- OK for now
//   switch (compilerOpts.GetAction().GetKind()) {
//   case ActionKind::EmitModule:
//     codeGenOpts.codeGenOutputKind = CodeGenOutputKind::LLVMModule;
//   case ActionKind::EmitIRBefore:
//     codeGenOpts.codeGenOutputKind = CodeGenOutputKind::LLVMIRPreOptimization;
//   case ActionKind::EmitIRAfter:
//     codeGenOpts.codeGenOutputKind =
//     CodeGenOutputKind::LLVMIRPostOptimization;
//   case ActionKind::EmitBC:
//     codeGenOpts.codeGenOutputKind = CodeGenOutputKind::LLVMBitCode;
//     break;
//   case ActionKind::EmitAssembly:
//     codeGenOpts.codeGenOutputKind = CodeGenOutputKind::NativeAssembly;
//     break;
//   default:
//     codeGenOpts.codeGenOutputKind = CodeGenOutputKind::ObjectFile;
//     break;
//   }
// }
// Status
// CompilerConfiguration::ParseCodeGenOptions(llvm::opt::InputArgList &args) {

//   ComputeCodeCodeGenOutputKind(GetCompilerOptions(), GetCodeGenOptions());

//   return Status();
// }
// Status
// CompilerConfiguration::ParseTargetOptions(llvm::opt::InputArgList &args) {

//   std::tie(GetCodeGenOptions().llvmTargetOpts, GetCodeGenOptions().targetCPU,
//            GetCodeGenOptions().targetFeatures,
//            GetCodeGenOptions().effectiveClangTriple) =
//       stone::GetIRTargetOptions(GetCodeGenOptions(),
//                                 GetLangContext().GetLangOptions(),
//                                 *clangContext);

//   // if (clangContext.GetInstance().getLangOpts().PointerAuthCalls) {
//   //   SetPointerAuthOptions(const_cast<CodeGenOptions
//   //   &>(GetCodeGenOptions).pointerAuth,
//   // clangContext.GetInstance().getCodeGenOpts().PointerAuth);
//   // }
//   return Status();
// }

// // TODO: Look at SetupWorkingDirectory
// llvm::StringRef
// CompilerConfiguration::ParseWorkDirectory(const llvm::opt::InputArgList
// &args) {
//   if (auto *arg = args.getLastArg(opts::WorkDir)) {
//     llvm::SmallString<128> smallStr;
//     smallStr = arg->getValue();
//     llvm::sys::fs::make_absolute(smallStr);
//     return smallStr.str();
//   }
//   return llvm::StringRef();
// }

// // TODO: PrintHelpTask
// void CompilerConfiguration::PrintHelp() {

//   if (GetAction().IsPrintHelp() || GetAction().IsPrintHelpHidden()) {
//     unsigned IncludedFlagsBitmask = opts::CompilerOption;
//     unsigned ExcludedFlagsBitmask =
//         GetAction().IsPrintHelpHidden() ? 0 : llvm::opt::HelpHidden;

//     std::unique_ptr<llvm::opt::OptTable> optTable(opts::CreateOptTable());
//     optTable->printHelp(llvm::outs(),
//                         GetCompilerOptions().MainExecutableName.data(),
//                         "stone-compile", IncludedFlagsBitmask,
//                         ExcludedFlagsBitmask, /*ShowAllAliases*/ false);
//   }
// }
