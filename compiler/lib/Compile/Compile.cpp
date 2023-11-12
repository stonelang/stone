#include "stone/Basic/Defer.h"
#include "stone/Basic/LLVMInit.h"
#include "stone/Basic/MainExecutablePath.h"
#include "stone/Compile/CompilerInstance.h"
#include "stone/Compile/CompilerInvocation.h"
#include "stone/Compile/Compiling.h"
#include "stone/Diag/CompilerDiagnostic.h"
#include "stone/Diag/TextDiagnosticFormatter.h"
#include "stone/Diag/TextDiagnosticListener.h"
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

  auto programPath = llvm::sys::fs::getMainExecutable(arg0, mainAddr);
  auto programName = file::GetStem(programPath);

  CompilerInvocation invocation;
  // TODO: SetMainExecutingPath
  // invocation.GetCompilerOptions().ExecutingProgramName = programName;
  // invocation.GetCompilerOptions().ExecutingProgramPath = programPath;

  // TODO: Set by default in CompilerInvocation
  llvm::sys::fs::current_path(invocation.GetCompilerOptions().workDirectory);

  STONE_DEFER { invocation.Finish(); };

  if (args.empty()) {
    invocation.GetLangContext().GetDiags().PrintD(SrcLoc(),
                                                  diag::err_no_input_files);
    return Finish(Status::Error());
  }
  // We setup clang now -- this just loads the instance.
  if (invocation.SetupClang(args, arg0).IsError()) {

    return Finish(Status::Error());
  }

  // Setup the custom formatting to be able to handle syntax diagnostics
  SyntaxDiagnosticFormatter diagFormatter;
  SyntaxDiagnosticEmitter diagEmitter(diagFormatter);
  TextDiagnosticListener diagListener(diagEmitter);

  invocation.GetDiags().AddListener(diagListener);

  ConfigurationFileBuffers configurationFileBuffers;

  // Parse arguments.
  if (invocation.ParseOptions(args).IsError()) {
    return Finish(Status::Error());
  }
  if (invocation.HasError()) {
    return Finish(Status::Error());
  }

  if (invocation.GetAction().IsAlien()) {
    invocation.GetDiags().PrintD(SrcLoc(), diag::err_alien_mode);
    return Finish(Status::Error());
  }
  if (invocation.GetAction().IsPrintHelp() ||
      invocation.GetAction().IsPrintHelpHidden()) {
    invocation.PrintHelp();
    return Finish();
  }
  if (invocation.GetAction().IsPrintVersion()) {
    invocation.PrintVersion();
    return Finish();
  }
  if (!invocation.GetAction().CanCompile()) {
    /// invocation.PrintD()
    return Finish();
  }
  if (listener) {
    listener->OnCompileConfigured(invocation);
  }
  if (invocation.CreateSourceBuffers().IsError()) {
    return Finish(Status::Error());
  }

  CompilerInstance compiler(listener);
  compiler.Initialize(invocation);

  if (compiling::Compile(compiler).IsError()) {
    return Finish(Status::Error());
  }
  return Finish();
}

Status compiling::Compile(CompilerInstance &compiler) {

  Status status;

  // At this point, everything requires syntax analysis.
  compiling::VerifyCompilerInputFileTypes(compiler);

  if (compiler.GetAction().IsParse()) {
    if (compiling::Parse(compiler).IsError()) {
      return Status::Error();
    }
  }

  // Otherwise, default to performing syntax analysis with import resoltuion.
  if (compiling::ParseAndImportResolution(compiler).IsError()) {
    status.SetIsError();
    return status;
  }

  if (compiler.GetAction().IsResolveImports()) {
    status.SetHasCompletion();
    return status;
  }

  if (compiler.GetAction().IsDumpAST()) {
    compiling::DumpAST(compiler);
    status.SetHasCompletion();
    return status;
  }

  // At this point, everything requires type-checking
  if (compiling::TypeCheck(compiler).IsError()) {
    status.SetIsError();
    return status;
  }

  if (compiler.GetAction().IsTypeCheck()) {
    status.SetHasCompletion();
    return status;
  }
  // Are we trying to print the AST?
  if (compiler.GetAction().IsPrintSyntax()) {
    compiling::PrintSyntax(compiler);
    status.SetHasCompletion();
    return status;
  }

  if (compiler.GetAction().IsDumpTypeInfo()) {
    compiling::DumpTypeInfo(compiler);
    status.SetHasCompletion();
    return status;
  }

  // Everyting from now on requires type checking
  if (compiler.IsActionPostTypeChecking()) {
    if (compiling::CompileAfterTypeChecking(compiler).IsError()) {
      status.SetHasCompletion();
      return status;
    }
  }
  return Status();
}

void compiling::VerifyCompilerInputFileTypes(CompilerInstance &compiler) {

  assert([&]() -> bool {
    if (compiler.GetAction().IsParse()) {
      // Parsing gets triggered lazily, but let's make sure we have the right
      // input kind.
      return llvm::all_of(
          compiler.GetCompilerOptions().InputsAndOutputs.getAllInputs(),
          [](const CompilerInputFile &cif) {
            const auto fileType = cif.GetType();
            return fileType == file::Type::Stone ||
                   fileType == file::Type::StoneModuleInterface;
          });
    }
    return true;
  }() && "Only supports parsing .stone files");
}

Status compiling::Parse(CompilerInstance &compiler) {

  SyntaxListener *syntaxListener = nullptr;
  LexerListener *lexerListener = nullptr;

  if (compiler.GetListener()) {
    syntaxListener = compiler.GetListener()->GetSyntaxListener();
    lexerListener = compiler.GetListener()->GetLexerListener();
  }
  for (auto moduleFile :
       compiler.GetModuleSystem().GetMainModule()->GetFiles()) {
    if (auto *syntaxFile = llvm::dyn_cast<syn::SyntaxFile>(moduleFile)) {
      stone::ParseSyntaxFile(*syntaxFile, compiler.GetSyntaxContext(),
                             syntaxListener, lexerListener);
      *syntaxFile.stage = SyntaxFileStage::Parsed;
    }
  }
  if (compiler.GetListener()) {
    compiler.GetListener()->OnSyntaxAnalysisCompleted(compiler);
  }
  return Status();
}

Status compiling::ParseAndImportResolution(CompilerInstance &compiler) {

  if (compiling::Parse(compiler).IsError()) {
    return Status::Error();
  }
  if (!compiler.GetAction().IsParse()) {
    compiler.ResolveImports();
  }
  return Status();
}

Status compiling::DumpSyntax(CompilerInstance &compiler,
                             syn::SyntaxFile &syntaxFile) {
  return Status();
}

Status compiling::TypeCheck(CompilerInstance &compiler) {

  TypeCheckerListener *listener = nullptr;
  if (compiler.GetListener()) {
    listener = compiler.GetListener()->GetTypeCheckerListener();
  }

  compiler.ForEachSyntaxFileToTypeCheck(
      [&](SyntaxFile &syntaxFile, TypeCheckerOptions &typeCheckerOpts,
          stone::TypeCheckerListener *listener) {
        stone::TypeCheckSyntaxFile(syntaxFile, typeCheckerOpts, listener);
        // TODO: Check for errors
        syntaxFile.stage = SyntaxFileStage::TypeChecked;
      });

  compiling::FinishTypeCheck(compiler);

  if (compiler.GetListener()) {
    compiler.GetListener()->OnSemanticAnalysisCompleted(compiler);
  }

  return Status();
}

Status compiling::FinishTypeCheck(CompilerInstance &compiler) {
  return Status();
}
Status compiling::PrintSyntax(CompilerInstance &compiler) { return Status(); }

Status compiling::CompileAfterTypeChecking(CompilerInstance &compiler) {

  Status status;
  // Create the CodeGenContext
  CodeGenContext codeGenContext(compiler.GetInvocation().GetCodeGenOptions(),
                                compiler.GetInvocation().GetModuleOptions(),
                                compiler.GetInvocation().GetTargetOptions(),
                                compiler.GetInvocation().GetLangContext(),
                                compiler.GetInvocation().GetClangContext());

  // If we are here, we need to GenIR
  if (compiling::GenIR(compiler, codeGenContext).IsError()) {
    status.SetHasCompletion();
  }
  if (compiler.GetAction().IsEmitIRAfter() ||
      compiler.GetAction().IsEmitIRBefore()) {
    status.SetHasCompletion();
    return status;
  }

  assert(compiler.CanCodeGen() &&
         "The current action does not suport generating code.");

  compiling::GenCode(compiler, codeGenContext);
}

Status compiling::GenIR(CompilerInstance &compiler,
                        CodeGenContext &codeGenContext) {

  return Status();
}

void compiling::DumpIR(CompilerInstance &compiler,
                       CodeGenContext &codeGenContext) {}

void compiling::PrintIR(CompilerInstance &compiler,
                        CodeGenContext &codeGenContext) {}

/// Code generation
Status compiling::GenCode(CompilerInstance &compiler,
                          CodeGenContext &codeGenContext) {

  //   if(GetInvocation().GetCodeGenOptions().codeGenOutputKind ==

  //   switch (GetInvocation().GetCodeGenOptions().codeGenOutputKind) {
  //   case CodeGenOutputKind::LLVMModule:
  //     return CompileWithGenIR(
  //         cgc, [&](CompilerInstance &compiler, CodeGenContext &cgc) {
  //           return GenModule(*this, cgc);
  //         });
  //   case CodeGenOutputKind::LLVMIRPreOptimization:
  //   case CodeGenOutputKind::LLVMIRPostOptimization:
  //     return CompileWithGenIR(
  //         cgc, [&](CompilerInstance &compiler, CodeGenContext &cgc) {
  //           return DumpIR(*this, cgc);
  //         });
  //   // case CodeGenOutputKind::PrintIR:
  //   //   return CompileWithGenIR(
  //   //       cgc, [&](CompilerInstance &compiler, CodeGenContext &cgc) {
  //   //         return PrintIR(*this, cgc);
  //   //       });
  //   default:
  //     return CompileWithGenIR(
  //         cgc, [&](CompilerInstance &compiler, CodeGenContext &cgc) {
  //           return CompileWithGenNative(cgc);
  //         });
  // }

  CodeGenListener *codeGenListener = nullptr;
  if (compiler.GetListener()) {
    codeGenListener = compiler.GetListener();
  }
  auto result = stone::GenNative(codeGenContext, compiler.GetSyntaxContext(),
                                 llvm::StringRef(), codeGenListener);

  return Status();
}

// static Status DumpIR(CompilerInstance &compiler, CodeGenContext &cgc) {
//   Status::Success();
// }

// static Status PrintIR(CompilerInstance &compiler, CodeGenContext &cgc) {
//   Status::Success();
// }

// Status CompilerInstance::CompileWithGenIR(CodeGenContext &cgc,
//                                           IRCodeGenCompletedCallback notifiy)
//                                           {
//   const auto &invocation = GetInvocation();
//   const CompilerOptions &compilerOpts = invocation.GetCompilerOptions();

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
// static Status GenModule(CompilerInstance &compiler, CodeGenContext &cgc) {
//   return Status::Success();
// }

// Status CompilerInstance::CompileWithGenNative(CodeGenContext &cgc) {

//   auto result = stone::GenNative(cgc, GetSyntaxContext(), llvm::StringRef(),
//                                  GetInvocation().GetListener());
//   return Status::Success();
// }

// Status CompilerInstance::CompileWithCodeGen() {

//   assert(CanCodeGen() && "Mode does not support code gen");

//   // We are performing some low level code generation
//   CodeGenContext cgc(
//       GetInvocation().GetCodeGenOptions(),
//       GetInvocation().GetModuleOptions(), GetInvocation().GetTargetOptions(),
//       GetInvocation().GetLangContext(), GetInvocation().GetClangContext());

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
//       cgc, [&](CompilerInstance &compiler, CodeGenContext &cgc) {
//         return GenModule(*this, cgc);
//       });
// case CodeGenOutputKind::LLVMIRPreOptimization:
// case CodeGenOutputKind::LLVMIRPostOptimization:
//   return CompileWithGenIR(
//       cgc, [&](CompilerInstance &compiler, CodeGenContext &cgc) {
//         return DumpIR(*this, cgc);
//       });
// // case CodeGenOutputKind::PrintIR:
// //   return CompileWithGenIR(
// //       cgc, [&](CompilerInstance &compiler, CodeGenContext &cgc) {
// //         return PrintIR(*this, cgc);
// //       });
// default:
//   return CompileWithGenIR(
//       cgc, [&](CompilerInstance &compiler, CodeGenContext &cgc) {
//         return CompileWithGenNative(cgc);
//       });
// }
//}

// static Status DumpSyntax(CompilerInstance &compiler, syn::SyntaxFile &sf) {
//   return Status::Success();
// }

// static Status PrintSyntax(CompilerInstance &compiler) {
//   return Status::Success();
// }

// Status CompilerInstance::CompileWithParsing() {
//   return CompileWithParsing(
//       [&](syn::SyntaxFile &) { return Status::Success(); });
// }

// Status CompilerInstance::CompileWithParsing(ParsingCompletedCallback notifiy)
// {

//   SyntaxListener *syntaxListener = nullptr;
//   LexerListener *lexerListener = nullptr;

//   if (compiler.GetListener()) {
//     syntaxListener = compiler.GetListener()->GetSyntaxListener();
//     lexerListener = compiler.GetListener()->GetLexerListener();
//   }

//   for (auto moduleFile : GetModuleSystem().GetMainModule()->GetFiles()) {
//     if (auto *syntaxFile = llvm::dyn_cast<syn::SyntaxFile>(moduleFile)) {
//       stone::ParseSyntaxFile(*syntaxFile, GetSyntaxContext(), syntaxListener,
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

// Status CompilerInstance::CompileWithTypeChecking() {
//   return CompileWithTypeChecking(
//       [&](CompilerInstance &) { return Status::Success(); });
// }

// Status CompilerInstance::CompileWithTypeChecking(
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
Status CompilerInstance::Compile() {

  assert(CanCompile() && "Unknown mode -- cannot continue with compile!");
  llvm::TimeTraceScope compileTimeScope("Compile");

  if (GetInvocation().GetListener()) {
    GetInvocation().GetListener()->OnCompileStarted(*this);
  }
  Status status;

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
  //       [&](CompilerInstance &compiler) { return PrintSyntax(*this); });
  //   break;
  // default:
  //   status = CompileWithTypeChecking(
  //       [&](CompilerInstance &compiler) { return CompileWithCodeGen(); });
  //   break;
  // }
  return status;
}