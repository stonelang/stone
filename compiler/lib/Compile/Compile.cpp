#include "stone/Basic/Defer.h"
#include "stone/Basic/LLVMInit.h"
#include "stone/Basic/MainExecutablePath.h"
#include "stone/Compile/Compiler.h"
#include "stone/Compile/CompilerTask.h"
#include "stone/Compile/Compiling.h"
#include "stone/Diag/CompilerDiagnostic.h"
#include "stone/Diag/TextDiagnosticConsumer.h"
#include "stone/Diag/TextDiagnosticFormatter.h"
#include "stone/Gen/CodeGenContext.h"
#include "stone/Option/Action.h"
#include "stone/Public.h"
#include "stone/Syntax/Module.h"
#include "stone/Syntax/SyntaxDiagnosticArgument.h"

#include "clang/Basic/TargetInfo.h"

#include "llvm/IR/Module.h"
#include "llvm/Support/Casting.h"

using namespace stone;
using namespace stone::syn;

int stone::Compile(llvm::ArrayRef<const char *> args, const char *arg0,
                   void *mainAddr, CompilerListener *listener) {

  llvm::PrettyStackTraceString crashInfo("Compile construction...");
  FINISH_LLVM_INIT();

  auto Finish = [&](Status status = Status::Success()) -> int {
    return status.GetFlag();
  };

  SyntaxDiagnosticFormatter formatter;
  SyntaxDiagnosticEmitter emitter(formatter);
  TextDiagnosticConsumer consumer(emitter);

  Compiler compiler;
  compiler.AddDiagnosticConsumer(consumer);

  // If the args are empty, it is pointless to move forward. 
  if (args.empty()) {
    compiler.GetDiags().PrintD(diag::err_no_compile_args);
    return Finish(Status::Error());
  }

  compiler.SetMainExecutable(arg0, mainAddr);

  CompilerCommandLine commandLine(compiler);
  auto status = commandLine.Parse(args);

  if (status.IsError()) {
    return Finish(Status::Error());
  }

  if (listener) {
    listener->CompletedCommandLineParsing(compiler);
  }
  status = compiler.Configure();
  if (status.IsError()) {
    return Finish(Status::Error());
  }
  if (listener) {
    listener->CompletedConfiguration(compiler);
  }

  compiler.BuildTasks();
  if (compiler.HasError()) {
    return Finish(Status::Error());
  }
  if (listener) {
    listener->CompletedBuildingTasks(compiler);
  }

  // Run compiler tasks
  compiler.RunTasks();
  if (compiler.HasError()) {
    return Finish(Status::Error());
  }
  if (listener) {
    listener->CompletedRunningTasks(compiler);
  }

  // ERROR(error_no_compile_args, none, "no arguments provided to
  // ConfigurationFileBuffers configurationFileBuffers;
  // // Configure the compiler
  // if (compiler.GetConfig().ParseCommandLine(args, arg0).IsError()) {
  //   return Finish(Status::Error());
  // }

  // if (listener) {
  //   listener->CompletedCommandLineParsing(compiler.GetConfig());
  // }

  // // The user has entered an action that the compiler does not know about.
  // if (compiler.GetAction().IsAlien()) {
  //   compiler.GetDiags().PrintD(diag::err_alien_mode);
  //   return Finish(Status::Error());
  // }

  // // // Now, we are ready to setup the compiler.
  // compiler.Setup();
  // compiler.SetListener(listener);

  // // Notifity that the compiler is configured
  // if (listener) {
  //   listener->CompletedConfiguration(compiler);
  // }

  // Now that we are ready to do real work, call defer Finish
  STONE_DEFER { compiler.Finish(); };

  // // Build all the compiler tasks
  // compiler.BuildTasks();
  // if (compiler.HasError()) {
  //   return Finish(Status::Error());
  // }

  // // Run compiler tasks
  // compiler.RunTasks();

  // if (compiler.GetAction().IsAlien()) {
  //   compiler.Report(diag::err_alien_mode);
  //   return Finish(Status::Error());
  // }
  // if (compiler.GetAction().IsPrintHelp() ||
  //     compiler.GetAction().IsPrintHelpHidden()) {
  //   // compiler.PrintHelp();
  //   return Finish();
  // }
  // if (compiler.GetAction().IsPrintVersion()) {
  //   compiler.PrintVersion();
  //   return Finish();
  // }
  // if (!compiler.GetAction().CanCompile()) {
  //   /// compiler.GetDiags().PrintD()
  //   return Finish();
  // }
  // if (listener) {
  //   listener->OnCompileConfigured(compiler);
  // }

  /// This should be called internally
  // if (compiler.CreateSourceBuffers().IsError()) {
  //   return Finish(Status::Error());
  // }

  // // Now, we are ready to setup the compiler.
  // compiler.Setup();
  // compiler.SetListener(listener);

  // if (compiling::Compile(compiler).IsError()) {
  //   return Finish(Status::Error());
  // }
  return Finish();
}

void Compiler::BuildTasks() { AddTask(GetAction().GetKind()); }

void Compiler::AddTask(ActionKind kind) {

  // switch (GetAction().GetKind()) {
  // case ActionKind::PrintHelp: {
  // case ActionKind::PrintHelpHidden:
  //   GetQueue().AddTask(PrintHelpTask::Create(*this));
  //   break;
  // }
  // case ActionKind::PrintVersion: {
  //   GetQueue().AddTask(PrintVersionTask::Create(*this));
  //   break;
  // }
  // case ActionKind::Parse: {
  //   GetQueue().AddTask(ParseTask::Create(*this));
  //   break;
  // }
  // case ActionKind::ResolveImports: {
  //   AddTask(ActionKind::Parse);
  //   GetQueue().AddTask(ResolveImportsTask::Create(*this));
  //   break;
  // }
  // case ActionKind::DumpSyntax: {
  //   AddTask(ActionKind::Parse);
  //   GetQueue().AddTask(DumpSyntaxTask::Create(*this));
  //   break;
  // }
  // case ActionKind::TypeCheck: {
  //   AddTask(ActionKind::ResolveImports);
  //   GetQueue().AddTask(TypeCheckTask::Create(*this));
  //   break;
  // }
  // case ActionKind::PrintSyntax: {
  //   AddTask(ActionKind::TypeCheck);
  //   GetQueue().AddTask(PrintSyntaxTask::Create(*this));
  //   break;
  // }
  // case ActionKind::PrintSyntax: {
  //   AddTask(ActionKind::TypeCheck);
  //   GetQueue().AddTask(PrintSyntaxTask::Create(*this));
  //   break;
  // }
  // case ActionKind::EmitModule: {
  //   AddTask(ActionKind::TypeCheck);
  //   GetQueue().AddTask(EmitModuleTask::Create(*this));
  //   break;
  // }
  // case ActionKind::MergeModules: {
  //   AddTask(ActionKind::TypeCheck);
  //   GetQueue().AddTask(MergeModulesTask::Create(*this));
  //   break;
  // }
  // case ActionKind::EmitIRBefore: {
  //   AddTask(ActionKind::TypeCheck);
  //   GetQueue().AddTask(EmitIRBeforeTask::Create(*this));
  //   break;
  // }
  // case ActionKind::EmitIRAfter: {
  //   AddTask(ActionKind::TypeCheck);
  //   GetQueue().AddTask(EmitIRAfterTask::Create(*this));
  //   break;
  // }
  // case ActionKind::EmitBC: {
  //   AddTask(ActionKind::EmitIRAfter);
  //   GetQueue().AddTask(EmitBCTask::Create(*this));
  //   break;
  // }
  // case ActionKind::EmitObject: {
  //   AddTask(ActionKind::EmitIRAfter);
  //   GetQueue().AddTask(EmitObjectTask::Create(*this));
  //   break;
  // }
  // case ActionKind::DumpTypeInfo: {
  //   AddTask(ActionKind::EmitIRAfter);
  //   GetQueue().AddTask(DumpTypeInfoTask::Create(*this));
  //   break;
  // }
  // default: {
  // } break;
  // }
}

void Compiler::RunTasks() { GetQueue().RunTasks(); }

Status PrintHelpTask::Execute(Compiler &compiler) { return Status(); }

// Status compiling::Compile(Compiler &compiler) {

//   Status status;

//   // At this point, everything requires syntax analysis.
//   compiling::VerifyCompilerInputFileTypes(compiler);

//   if (compiler.GetAction().IsParse()) {
//     if (compiling::Parse(compiler).IsError()) {
//       return Status::Error();
//     }
//   }

//   // Otherwise, default to performing syntax analysis with import
//   resoltuion. if (compiling::ParseAndImportResolution(compiler).IsError())
//   {
//     status.SetIsError();
//     return status;
//   }

//   if (compiler.GetAction().IsResolveImports()) {
//     status.SetHasCompletion();
//     return status;
//   }

//   if (compiler.GetAction().IsDumpAST()) {
//     compiling::DumpAST(compiler);
//     status.SetHasCompletion();
//     return status;
//   }

//   // At this point, everything requires type-checking
//   if (compiling::TypeCheck(compiler).IsError()) {
//     status.SetIsError();
//     return status;
//   }

//   if (compiler.GetAction().IsTypeCheck()) {
//     status.SetHasCompletion();
//     return status;
//   }
//   // Are we trying to print the AST?
//   if (compiler.GetAction().IsPrintSyntax()) {
//     compiling::PrintSyntax(compiler);
//     status.SetHasCompletion();
//     return status;
//   }

//   if (compiler.GetAction().IsDumpTypeInfo()) {
//     compiling::DumpTypeInfo(compiler);
//     status.SetHasCompletion();
//     return status;
//   }

//   // Everyting from now on requires type checking
//   if (compiler.IsActionPostTypeChecking()) {
//     if (compiling::CompileAfterTypeChecking(compiler).IsError()) {
//       status.SetHasCompletion();
//       return status;
//     }
//   }
//   return Status();
// }

// void compiling::VerifyCompilerInputFileTypes(Compiler &compiler) {

//   assert([&]() -> bool {
//     if (compiler.GetAction().IsParse()) {
//       // Parsing gets triggered lazily, but let's make sure we have the
//       right
//       // input kind.
//       return llvm::all_of(
//           compiler.GetCompilerOptions().inputsAndOutputs.GetInputs(),
//           [](const CompilerInputFile &cif) {
//             const auto fileType = cif.GetType();
//             return fileType == file::Type::Stone ||
//                    fileType == file::Type::StoneModuleInterface;
//           });
//     }
//     return true;
//   }() && "Only supports parsing .stone files");
// }

// Status compiling::Parse(Compiler &compiler) {

//   SyntaxListener *syntaxListener = nullptr;
//   LexerListener *lexerListener = nullptr;

//   if (compiler.GetListener()) {
//     syntaxListener = compiler.GetListener()->GetSyntaxListener();
//     lexerListener = compiler.GetListener()->GetLexerListener();
//   }
//   for (auto moduleFile :
//        compiler.GetModuleSystem().GetMainModule()->GetFiles()) {
//     if (auto *syntaxFile = llvm::dyn_cast<syn::SyntaxFile>(moduleFile)) {
//       stone::ParseSyntaxFile(*syntaxFile, compiler.GetSyntaxContext(),
//                              syntaxListener, lexerListener);
//       syntaxFile->stage = SyntaxFileStage::Parsed;
//     }
//   }
//   if (compiler.GetListener()) {
//     compiler.GetListener()->OnSyntaxAnalysisCompleted(compiler);
//   }
//   return Status();
// }

// Status compiling::ParseAndImportResolution(Compiler &compiler) {

//   if (compiling::Parse(compiler).IsError()) {
//     return Status::Error();
//   }
//   if (!compiler.GetAction().IsParse()) {
//     compiler.ResolveImports();
//   }
//   return Status();
// }

// Status compiling::DumpSyntax(Compiler &compiler, syn::SyntaxFile
// &syntaxFile)
// {
//   return Status();
// }

// Status compiling::TypeCheck(Compiler &compiler) {

//   TypeCheckerListener *listener = nullptr;
//   if (compiler.GetListener()) {
//     listener = compiler.GetListener()->GetTypeCheckerListener();
//   }

//   compiler.ForEachSyntaxFileToTypeCheck(
//       [&](SyntaxFile &syntaxFile, TypeCheckerOptions &typeCheckerOpts,
//           stone::TypeCheckerListener *listener) {
//         stone::TypeCheckSyntaxFile(syntaxFile, typeCheckerOpts, listener);
//         // TODO: Check for errors
//         syntaxFile.stage = SyntaxFileStage::TypeChecked;
//       });

//   compiling::FinishTypeCheck(compiler);

//   if (compiler.GetListener()) {
//     compiler.GetListener()->OnSemanticAnalysisCompleted(compiler);
//   }

//   return Status();
// }

// Status compiling::FinishTypeCheck(Compiler &compiler) { return Status(); }
// Status compiling::PrintSyntax(Compiler &compiler) { return Status(); }

// Status compiling::CompileAfterTypeChecking(Compiler &compiler) {

//   Status status;
//   // Create the CodeGenContext
//   CodeGenContext codeGenContext(
//       compiler.GetCodeGenOptions(), compiler.GetModuleOptions(),
//       compiler.GetTargetOptions(), compiler.GetLangContext(),
//       compiler.GetClangContext());

//   // If we are here, we need to GenIR
//   if (compiling::GenIR(compiler, codeGenContext).IsError()) {
//     status.SetHasCompletion();
//   }
//   if (compiler.GetAction().IsEmitIRAfter() ||
//       compiler.GetAction().IsEmitIRBefore()) {
//     status.SetHasCompletion();
//     return status;
//   }

//   assert(compiler.CanCodeGen() &&
//          "The current action does not suport generating code.");

//   compiling::GenCode(compiler, codeGenContext);
// }

// Status compiling::GenIR(Compiler &compiler, CodeGenContext &codeGenContext)
// {

//   return Status();
// }

// void compiling::DumpIR(Compiler &compiler, CodeGenContext &codeGenContext)
// {}

// void compiling::PrintIR(Compiler &compiler, CodeGenContext &codeGenContext)
// {}

/// Code generation
// Status compiling::GenCode(Compiler &compiler, CodeGenContext
// &codeGenContext)
// {

//   if(GetInvocation().GetCodeGenOptions().codeGenOutputKind ==

//   switch (GetInvocation().GetCodeGenOptions().codeGenOutputKind) {
//   case CodeGenOutputKind::LLVMModule:
//     return CompileWithGenIR(
//         cgc, [&](Compiler &compiler, CodeGenContext &cgc) {
//           return GenModule(*this, cgc);
//         });
//   case CodeGenOutputKind::LLVMIRPreOptimization:
//   case CodeGenOutputKind::LLVMIRPostOptimization:
//     return CompileWithGenIR(
//         cgc, [&](Compiler &compiler, CodeGenContext &cgc) {
//           return DumpIR(*this, cgc);
//         });
//   // case CodeGenOutputKind::PrintIR:
//   //   return CompileWithGenIR(
//   //       cgc, [&](Compiler &compiler, CodeGenContext &cgc) {
//   //         return PrintIR(*this, cgc);
//   //       });
//   default:
//     return CompileWithGenIR(
//         cgc, [&](Compiler &compiler, CodeGenContext &cgc) {
//           return CompileWithGenNative(cgc);
//         });
// }

// CodeGenListener *codeGenListener = nullptr;
// if (compiler.GetListener()) {
//   codeGenListener = compiler.GetListener()->GetCodeGenListener();
// }
// auto result = stone::GenNative(codeGenContext, compiler.GetSyntaxContext(),
//                                llvm::StringRef(), codeGenListener);

// return Status();
//}

// static Status DumpIR(Compiler &compiler, CodeGenContext &cgc) {
//   Status::Success();
// }

// static Status PrintIR(Compiler &compiler, CodeGenContext &cgc) {
//   Status::Success();
// }

// Status Compiler::CompileWithGenIR(CodeGenContext &cgc,
//                                           IRCodeGenCompletedCallback
//                                           notifiy)
//                                           {
//   const auto &compiler = GetInvocation();
//   const CompilerOptions &compilerOpts = compiler.GetCompilerOptions();

//   if (IsWholeModuleCodeGen()) {
//     auto *mainModule = GetModuleSystem().GetMainModule();
//     const PrimaryFileSpecificPaths primaryFileSpecificPaths =
//         GetPrimaryFileSpecificPathsForWholeModuleOptimizationMode();

//     stone::GenModuleIR(cgc, primaryFileSpecificPaths.outputFilename,
//     mainModule,
//                        primaryFileSpecificPaths);
//   } else if (IsSyntaxFileCodeGen()) {
//     for (auto *primarySyntaxFile : GetPrimarySyntaxFiles()) {
//       const PrimaryFileSpecificPaths primaryFileSpecificPaths =
//           GetPrimaryFileSpecificPathsForSyntaxFile(*primarySyntaxFile);
//       stone::GenSyntaxFileIR(cgc, primaryFileSpecificPaths.outputFilename,
//                              primarySyntaxFile, primaryFileSpecificPaths);
//     }
//   }

//   if (notifiy) {
//     notifiy(*this, cgc);
//   }
//   Status::Error();
// }
// static Status GenModule(Compiler &compiler, CodeGenContext &cgc) {
//   return Status::Success();
// }

// Status Compiler::CompileWithGenNative(CodeGenContext &cgc) {

//   auto result = stone::GenNative(cgc, GetSyntaxContext(),
//   llvm::StringRef(),
//                                  GetInvocation().GetListener());
//   return Status::Success();
// }

// Status Compiler::CompileWithCodeGen() {

//   assert(CanCodeGen() && "Mode does not support code gen");

//   // We are performing some low level code generation
//   CodeGenContext cgc(
//       GetInvocation().GetCodeGenOptions(),
//       GetInvocation().GetModuleOptions(),
//       GetInvocation().GetTargetOptions(), GetInvocation().GetLangContext(),
//       GetInvocation().GetClangContext());

// auto *Module = IGM.getModule();
// assert(Module && "Expected llvm:Module for IR generation!");

// Module->setTargetTriple(IGM.Triple.str());

// // Set the module's string representation.
// Module->setDataLayout(IGM.DataLayout.getStringRepresentation());

// clang::TargetInfo &targetInfo =
//     GetInvocation().GetClangContext().GetInstance().getTarget();

// // Setup the empty module
// cgc.GetLLVMModule().setTargetTriple(targetInfo.getTriple().getTriple());
// cgc.GetLLVMModule().setDataLayout(targetInfo.getDataLayoutString());

// const auto &sdkVersion = targetInfo.getSDKVersion();

// if (!sdkVersion.empty()) {
//   cgc.GetLLVMModule().setSDKVersion(sdkVersion);
// }

// if (const auto *tvt = targetInfo.getDarwinTargetVariantTriple()) {
//   cgc.GetModule().setDarwinTargetVariantTriple(tvt->getTriple());
// }

// if (auto TVSDKVersion = targetInfo.getDarwinTargetVariantSDKVersion()) {
//   cgc.GetModule().setDarwinTargetVariantSDKVersion(*TVSDKVersion);
// }

// switch (GetInvocation().GetCodeGenOptions().codeGenOutputKind) {
// case CodeGenOutputKind::LLVMModule:
//   return CompileWithGenIR(
//       cgc, [&](Compiler &compiler, CodeGenContext &cgc) {
//         return GenModule(*this, cgc);
//       });
// case CodeGenOutputKind::LLVMIRPreOptimization:
// case CodeGenOutputKind::LLVMIRPostOptimization:
//   return CompileWithGenIR(
//       cgc, [&](Compiler &compiler, CodeGenContext &cgc) {
//         return DumpIR(*this, cgc);
//       });
// // case CodeGenOutputKind::PrintIR:
// //   return CompileWithGenIR(
// //       cgc, [&](Compiler &compiler, CodeGenContext &cgc) {
// //         return PrintIR(*this, cgc);
// //       });
// default:
//   return CompileWithGenIR(
//       cgc, [&](Compiler &compiler, CodeGenContext &cgc) {
//         return CompileWithGenNative(cgc);
//       });
// }
//}

// static Status DumpSyntax(Compiler &compiler, syn::SyntaxFile &sf) {
//   return Status::Success();
// }

// static Status PrintSyntax(Compiler &compiler) {
//   return Status::Success();
// }

// Status Compiler::CompileWithParsing() {
//   return CompileWithParsing(
//       [&](syn::SyntaxFile &) { return Status::Success(); });
// }

// Status Compiler::CompileWithParsing(ParsingCompletedCallback notifiy)
// {

//   SyntaxListener *syntaxListener = nullptr;
//   LexerListener *lexerListener = nullptr;

//   if (compiler.GetListener()) {
//     syntaxListener = compiler.GetListener()->GetSyntaxListener();
//     lexerListener = compiler.GetListener()->GetLexerListener();
//   }

//   for (auto moduleFile : GetModuleSystem().GetMainModule()->GetFiles()) {
//     if (auto *syntaxFile = llvm::dyn_cast<syn::SyntaxFile>(moduleFile)) {
//       stone::ParseSyntaxFile(*syntaxFile, GetSyntaxContext(),
//       syntaxListener,
//                              lexerListener);
//       if (notifiy) {
//         notifiy(*syntaxFile);
//       }
//     }
//   }

//   if (!GetAction().IsParseOnly()) {
//     ResolveImports();
//   }
//   if (compiler.GetListener()) {
//     compiler.GetListener()->OnSyntaxAnalysisCompleted(*this);
//   }
//   return Status::Success();
// }

// Status Compiler::CompileWithTypeChecking() {
//   return CompileWithTypeChecking(
//       [&](Compiler &) { return Status::Success(); });
// }

// Status Compiler::CompileWithTypeChecking(
//     TypeCheckingCompletedCallback notifiy) {

//   auto status = CompileWithParsing();
//   if (status.IsError()) {
//     return status;
//   }

//   // TypeCheckerListener *typeCheckerListener = nullptr;
//   // if (compiler.GetListener()) {
//   //   compiler.GetListener()->GetTypeCheckerListener();
//   // }

//   ForEachSyntaxFile([&](SyntaxFile &syntaxFile,
//                         TypeCheckerOptions &typeCheckerOpts,
//                         stone::TypeCheckerListener *listener) {
//     stone::TypeCheckSyntaxFile(syntaxFile, typeCheckerOpts, listener);
//   });

//   // TODO: FinishTypeCheck();
//   if (compiler.GetListener()) {
//     compiler.GetListener()->OnSemanticAnalysisCompleted(*this);
//   }
//   return notifiy(*this);
// }
// Status Compiler::Compile() {

//   assert(CanCompile() && "Unknown mode -- cannot continue with compile!");
//   llvm::TimeTraceScope compileTimeScope("Compile");

//   if (GetListener()) {
//     GetListener()->OnCompileStarted(*this);
//   }
//   Status status;

//   BuildTasks();
//   RunTasks();

// switch (GetAction().GetKind()) {
// case ActionKind::Parse:
//   status = CompileWithParsing();
//   break;
// case ActionKind::DumpSyntax:
//   status = CompileWithParsing(
//       [&](syn::SyntaxFile &sf) { return DumpSyntax(*this, sf); });
//   break;
// case ActionKind::TypeCheck:
//   status = CompileWithTypeChecking();
//   break;
// case ActionKind::PrintSyntax:
//   status = CompileWithTypeChecking(
//       [&](Compiler &compiler) { return PrintSyntax(*this); });
//   break;
// default:
//   status = CompileWithTypeChecking(
//       [&](Compiler &compiler) { return CompileWithCodeGen(); });
//   break;
// }

//   return status;
// }
// Status Compiler::BuildTasks() { GetQueue().Add(PrintTask()); }

// void Compiler::RunTasks() {}
