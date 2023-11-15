

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
//       stone::ParseSyntaxFile(*syntaxFile, compiler.GetASTContext(),
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
// auto result = stone::GenNative(codeGenContext, compiler.GetASTContext(),
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

//   auto result = stone::GenNative(cgc, GetASTContext(),
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
//       stone::ParseSyntaxFile(*syntaxFile, GetASTContext(),
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
