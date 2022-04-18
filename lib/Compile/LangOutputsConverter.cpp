
#include "stone/Basic/CompileDiagnostic.h"
#include "stone/Basic/OutputFileMap.h"

//#include "stone/Basic/Platform.h"
#include "stone/Compile/LangInputsConverter.h"
#include "stone/Compile/LangOptionsConverter.h"
#include "stone/Compile/LangOutputsConverter.h"

#include "stone/Session/Options.h"
//#include "stone/Session/SanitizerOptions.h"

#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Option/Arg.h"
#include "llvm/Option/ArgList.h"
#include "llvm/Option/Option.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/LineIterator.h"
#include "llvm/Support/Path.h"

using namespace stone;
using namespace llvm::opt;

bool LangOutputsConverter::Convert(
    std::vector<std::string> &mainOutputs,
    std::vector<std::string> &mainOutputsForIndexUnits,
    std::vector<SupplementaryOutputPaths> &supplementaryOutputs,
    const Mode &mode) {

  Optional<LangOutputFilesComputer> ofc = LangOutputFilesComputer::Create(
      args, de, inputsAndOutputs,
      {"output", opts::o, opts::OutputFileList, "-o"}, mode);

  if (!ofc) {
    return true;
  }
  llvm::Optional<std::vector<std::string>> mains = ofc->ComputeOutputFiles();
  if (!mains) {
    return true;
  }

  llvm::Optional<std::vector<std::string>> indexMains;
  if (args.hasArg(opts::IndexUnitOutputPath,
                  opts::IndexUnitOutputPathFileList)) {

    llvm::Optional<LangOutputFilesComputer> iuofc =
        LangOutputFilesComputer::Create(
            args, de, inputsAndOutputs,
            {"index unit output path", opts::IndexUnitOutputPath,
             opts::IndexUnitOutputPathFileList, "-index-unit-output-path"},
            mode);

    if (!iuofc) {
      return true;
    }
    indexMains = iuofc->ComputeOutputFiles();
    if (!indexMains) {
      return true;
    }

    assert(mains->size() == indexMains->size() && "checks not equivalent?");
  }

  llvm::Optional<std::vector<SupplementaryOutputPaths>> supplementaries =
      SupplementaryOutputPathsComputer(args, de, inputsAndOutputs, *mains,
                                       moduleName, mode)
          .ComputeOutputPaths();

  if (!supplementaries) {
    return true;
  }

  mainOutputs = std::move(*mains);
  if (indexMains) {
    mainOutputsForIndexUnits = std::move(*indexMains);
  }
  supplementaryOutputs = std::move(*supplementaries);
  return false;
}

llvm::Optional<std::vector<std::string>>
LangOutputsConverter::ReadOutputFileList(const llvm::StringRef fileListPath,
                                         DiagnosticEngine &de) {
  llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> buffer =
      llvm::MemoryBuffer::getFile(fileListPath);
  if (!buffer) {
    de.PrintD(SrcLoc(), diag::err_cannot_open_file, diag::LLVMStr(fileListPath),
              diag::LLVMStr(buffer.getError().message()));
    return llvm::None;
  }
  std::vector<std::string> outputFiles;
  for (llvm::StringRef line :
       make_range(llvm::line_iterator(*buffer.get()), {})) {
    outputFiles.push_back(line.str());
  }
  return outputFiles;
}

llvm::Optional<std::vector<std::string>>
LangOutputFilesComputer::GetOutputFilenamesFromCommandLineOrFileList(
    const ArgList &args, DiagnosticEngine &de, opts::OptID singleOpt,
    opts::OptID fileListOpt) {
  if (const Arg *A = args.getLastArg(fileListOpt)) {

    assert(
        !args.hasArg(singleOpt) &&
        "don't use -o with -output-filelist or -index-unit-output-path with" &&
        " " && "-index-unit-output-filelist");

    return LangOutputsConverter::ReadOutputFileList(A->getValue(), de);
  }
  return args.getAllArgValues(singleOpt);
}

llvm::Optional<LangOutputFilesComputer>
LangOutputFilesComputer::Create(const llvm::opt::ArgList &args,
                                DiagnosticEngine &de,
                                const LangInputsAndOutputs &inputsAndOutputs,
                                LangOutputOptInfo optInfo, const Mode &mode) {
  Optional<std::vector<std::string>> outputArguments =
      GetOutputFilenamesFromCommandLineOrFileList(args, de, optInfo.SingleID,
                                                  optInfo.FilelistID);
  if (!outputArguments)
    return None;
  const StringRef outputDirectoryArgument =
      outputArguments->size() == 1 &&
              llvm::sys::fs::is_directory(outputArguments->front())
          ? llvm::StringRef(outputArguments->front())
          : llvm::StringRef();
  ArrayRef<std::string> outputFileArguments =
      outputDirectoryArgument.empty()
          ? llvm::ArrayRef<std::string>(*outputArguments)
          : llvm::ArrayRef<std::string>();
  const StringRef firstInput =
      inputsAndOutputs.HasSingleInput()
          ? llvm::StringRef(inputsAndOutputs.GetFilenameOfFirstInput())
          : llvm::StringRef();

  if (!outputFileArguments.empty() &&
      outputFileArguments.size() !=
          inputsAndOutputs.CountOfInputsProducingMainOutputs()) {
    de.PrintD(SrcLoc(),
              diag::err_if_any_output_files_are_specified_they_all_must_be,
              diag::LLVMStr(optInfo.PrettyName));
    return llvm::None;
  }

  const file::Type outputType =
      LangOptions::GetFileTypeByModeKind(mode.GetKind());

  return LangOutputFilesComputer(
      de, inputsAndOutputs, std::move(outputFileArguments),
      outputDirectoryArgument, firstInput, mode,
      args.getLastArg(opts::ModuleName), file::GetTypeExt(outputType),
      mode.CanOutput(), optInfo);
}

LangOutputFilesComputer::LangOutputFilesComputer(
    DiagnosticEngine &de, const LangInputsAndOutputs &inputsAndOutputs,
    std::vector<std::string> outputFileArguments,
    const StringRef outputDirectoryArgument, const StringRef firstInput,
    const Mode &mode, const llvm::opt::Arg *moduleNameArg,
    const StringRef suffix, const bool hasTextualOutput,
    LangOutputOptInfo optInfo)
    : de(de), inputsAndOutputs(inputsAndOutputs),
      OutputFileArguments(outputFileArguments),
      OutputDirectoryArgument(outputDirectoryArgument), FirstInput(firstInput),
      mode(mode), moduleNameArg(moduleNameArg), Suffix(suffix),
      HasTextualOutput(hasTextualOutput), OutputInfo(optInfo) {}

llvm::Optional<std::vector<std::string>>
LangOutputFilesComputer::ComputeOutputFiles() const {
  std::vector<std::string> outputFiles;
  unsigned i = 0;
  bool hadError = inputsAndOutputs.ForEachInputProducingAMainOutputFile(
      [&](const LangInputFile &input) -> bool {
        StringRef outputArg = OutputFileArguments.empty()
                                  ? llvm::StringRef()
                                  : llvm::StringRef(OutputFileArguments[i++]);

        llvm::Optional<std::string> outputFile =
            ComputeOutputFile(outputArg, input);
        if (!outputFile)
          return true;
        outputFiles.push_back(*outputFile);
        return false;
      });
  return hadError ? llvm::None
                  : llvm::Optional<std::vector<std::string>>(outputFiles);
}

llvm::Optional<std::string>
LangOutputFilesComputer::ComputeOutputFile(StringRef outputArg,
                                           const LangInputFile &input) const {
  // Return an empty string to signify no output.
  // The frontend does not currently produce a diagnostic
  // if a -o argument is present for such an action
  // for instance stonec -frontend -o foo -interpret foo.stone
  if (!mode.CanOutput()) {
    return std::string();
  }
  if (!OutputDirectoryArgument.empty()) {
    return DeriveOutputFileForDirectory(input);
  }
  if (!outputArg.empty()) {
    return outputArg.str();
  }

  return DeriveOutputFileFromInput(input);
}

llvm::Optional<std::string> LangOutputFilesComputer::DeriveOutputFileFromInput(
    const LangInputFile &input) const {

  if (input.GetFileName() == LangOptions::dash || HasTextualOutput) {
    return std::string(LangOptions::dash);
  }

  std::string baseName = DetermineBaseNameOfOutput(input);
  if (baseName.empty()) {
    // Assuming FrontendOptions::doesActionProduceOutput(RequestedAction)
    de.PrintD(SrcLoc(), diag::err_no_output_filename_specified,
              diag::LLVMStr(OutputInfo.PrettyName));
    return llvm::None;
  }
  return DeriveOutputFileFromParts("", baseName);
}

Optional<std::string> LangOutputFilesComputer::DeriveOutputFileForDirectory(
    const LangInputFile &input) const {
  std::string baseName = DetermineBaseNameOfOutput(input);
  if (baseName.empty()) {
    de.PrintD(SrcLoc(), diag::err_implicit_output_file_is_directory,
              diag::LLVMStr(OutputDirectoryArgument),
              diag::LLVMStr(OutputInfo.SingleOptSpelling));
    return None;
  }
  return DeriveOutputFileFromParts(OutputDirectoryArgument, baseName);
}

std::string LangOutputFilesComputer::DetermineBaseNameOfOutput(
    const LangInputFile &input) const {
  std::string nameToStem = input.IsPrimary() ? input.GetFileName()
                           : moduleNameArg   ? moduleNameArg->getValue()
                                             : FirstInput;
  return llvm::sys::path::stem(nameToStem).str();
}

std::string
LangOutputFilesComputer::DeriveOutputFileFromParts(StringRef dir,
                                                   StringRef base) const {
  assert(!base.empty());
  llvm::SmallString<128> path(dir);
  llvm::sys::path::append(path, base);
  llvm::sys::path::replace_extension(path, Suffix);
  return std::string(path.str());
}

SupplementaryOutputPathsComputer::SupplementaryOutputPathsComputer(
    const ArgList &args, DiagnosticEngine &de,
    const LangInputsAndOutputs &inputsAndOutputs,
    ArrayRef<std::string> outputFiles, StringRef moduleName, const Mode &mode)
    : args(args), de(de), inputsAndOutputs(inputsAndOutputs),
      OutputFiles(outputFiles), moduleName(moduleName), mode(mode) {}

llvm::Optional<std::vector<SupplementaryOutputPaths>>
SupplementaryOutputPathsComputer::ComputeOutputPaths() const {
  Optional<std::vector<SupplementaryOutputPaths>> pathsFromUser =
      args.hasArg(opts::SupplementaryOutputFileMap)
          ? ReadSupplementaryOutputFileMap()
          : GetSupplementaryOutputPathsFromArguments();
  if (!pathsFromUser)
    return None;

  if (inputsAndOutputs.HasPrimaryInputs()) {
    assert(OutputFiles.size() == pathsFromUser->size());
  } else if (inputsAndOutputs.IsSingleThreadedWMO()) {
    assert(OutputFiles.size() == pathsFromUser->size() &&
           pathsFromUser->size() == 1);
  } else {
    // Multi-threaded WMO is the exception
    assert(OutputFiles.size() == inputsAndOutputs.InputCount() &&
                   pathsFromUser->size() == inputsAndOutputs.HasInputs()
               ? 1
               : 0);
  }

  std::vector<SupplementaryOutputPaths> outputPaths;
  unsigned i = 0;
  bool hadError = inputsAndOutputs.ForEachInputProducingSupplementaryOutput(
      [&](const LangInputFile &input) -> bool {
        if (auto suppPaths = ComputeOutputPathsForOneInput(
                OutputFiles[i], (*pathsFromUser)[i], input)) {
          ++i;
          outputPaths.push_back(*suppPaths);
          return false;
        }
        return true;
      });
  if (hadError)
    return None;
  return outputPaths;
}

// Optional<std::vector<SupplementaryOutputPaths>>
// SupplementaryOutputPathsComputer::GetSupplementaryOutputPathsFromArguments()
//     const {

//   auto objCHeaderOutput = getSupplementaryFilenamesFromArguments(
//       opts::emit_objc_header_path);
//   auto moduleOutput =
//       getSupplementaryFilenamesFromArguments(opts::emit_module_path);
//   auto moduleDocOutput =
//       getSupplementaryFilenamesFromArguments(opts::emit_module_doc_path);
//   auto dependenciesFile = getSupplementaryFilenamesFromArguments(
//       opts::emit_dependencies_path);
//   auto referenceDependenciesFile = getSupplementaryFilenamesFromArguments(
//       opts::emit_reference_dependencies_path);
//   auto serializedDiagnostics = getSupplementaryFilenamesFromArguments(
//       opts::serialize_diagnostics_path);
//   auto fixItsOutput = getSupplementaryFilenamesFromArguments(
//       opts::emit_fixits_path);
//   auto loadedModuleTrace = getSupplementaryFilenamesFromArguments(
//       opts::emit_loaded_module_trace_path);
//   auto TBD = getSupplementaryFilenamesFromArguments(opts::emit_tbd_path);
//   auto moduleInterfaceOutput = getSupplementaryFilenamesFromArguments(
//       opts::emit_module_interface_path);
//   auto privateModuleInterfaceOutput = getSupplementaryFilenamesFromArguments(
//       opts::emit_private_module_interface_path);
//   auto moduleSourceInfoOutput = getSupplementaryFilenamesFromArguments(
//       opts::emit_module_source_info_path);
//   auto moduleSummaryOutput = getSupplementaryFilenamesFromArguments(
//       opts::emit_module_summary_path);
//   auto abiDescriptorOutput = getSupplementaryFilenamesFromArguments(
//       opts::emit_abi_descriptor_path);
//   auto moduleSemanticInfoOutput = getSupplementaryFilenamesFromArguments(
//       opts::emit_module_semantic_info_path);
//   auto optRecordOutput = getSupplementaryFilenamesFromArguments(
//       opts::save_optimization_record_path);
//   if (!objCHeaderOutput || !moduleOutput || !moduleDocOutput ||
//       !dependenciesFile || !referenceDependenciesFile ||
//       !serializedDiagnostics || !fixItsOutput || !loadedModuleTrace || !TBD
//       || !moduleInterfaceOutput || !privateModuleInterfaceOutput ||
//       !moduleSourceInfoOutput || !moduleSummaryOutput || !abiDescriptorOutput
//       || !moduleSemanticInfoOutput || !optRecordOutput) {
//     return None;
//   }
//   std::vector<SupplementaryOutputPaths> result;

//   const unsigned N =
//       inputsAndOutputs.countOfFilesProducingSupplementaryOutput();
//   for (unsigned i = 0; i < N; ++i) {
//     SupplementaryOutputPaths sop;
//     sop.ObjCHeaderOutputPath = (*objCHeaderOutput)[i];
//     sop.ModuleOutputPath = (*moduleOutput)[i];
//     sop.ModuleDocOutputPath = (*moduleDocOutput)[i];
//     sop.DependenciesFilePath = (*dependenciesFile)[i];
//     sop.ReferenceDependenciesFilePath = (*referenceDependenciesFile)[i];
//     sop.SerializedDiagnosticsPath = (*serializedDiagnostics)[i];
//     sop.FixItsOutputPath = (*fixItsOutput)[i];
//     sop.LoadedModuleTracePath = (*loadedModuleTrace)[i];
//     sop.TBDPath = (*TBD)[i];
//     sop.ModuleInterfaceOutputPath = (*moduleInterfaceOutput)[i];
//     sop.PrivateModuleInterfaceOutputPath =
//     (*privateModuleInterfaceOutput)[i]; sop.ModuleSourceInfoOutputPath =
//     (*moduleSourceInfoOutput)[i]; sop.ModuleSummaryOutputPath =
//     (*moduleSummaryOutput)[i]; sop.ABIDescriptorOutputPath =
//     (*abiDescriptorOutput)[i]; sop.ModuleSemanticInfoOutputPath =
//     (*moduleSemanticInfoOutput)[i]; sop.YAMLOptRecordPath =
//     (*optRecordOutput)[i]; sop.BitstreamOptRecordPath =
//     (*optRecordOutput)[i]; result.push_back(sop);
//   }
//   return result;
// }

// // Extend this routine for filelists if/when we have them.

Optional<std::vector<std::string>>
SupplementaryOutputPathsComputer::GetSupplementaryFilenamesFromArguments(
    opts::OptID pathID) const {
  std::vector<std::string> paths = args.getAllArgValues(pathID);

  const unsigned N =
      inputsAndOutputs.CountOfFilesProducingSupplementaryOutput();

  if (paths.size() == N) {
    return paths;
  } else if (pathID == opts::EmitLoadedModuleTracePath && paths.size() < N) {
    // We only need one file to output the module trace file because they
    // are all equivalent. Add supplementary empty output paths for moduletrace
    // to make sure the compiler won't panic for
    // diag::err_wrong_number_of_arguments.

    for (unsigned I = paths.size(); I != N; I++) {
      paths.emplace_back();
    }
    return paths;
  }

  if (paths.empty()) {
    return std::vector<std::string>(N, std::string());
  }

  de.PrintD(
      SrcLoc(), diag::err_wrong_number_of_arguments,
      diag::LLVMStr(args.getLastArg(pathID)->getOption().getPrefixedName()),
      diag::Int(N), diag::Int(paths.size()));
  return llvm::None;
}

// Optional<SupplementaryOutputPaths>
// SupplementaryOutputPathsComputer::computeOutputPathsForOneInput(
//     StringRef outputFile, const SupplementaryOutputPaths &pathsFromArguments,
//     const LangInputFile &input) const {
//   StringRef defaultSupplementaryOutputPathExcludingExtension =
//       deriveDefaultSupplementaryOutputPathExcludingExtension(outputFile,
//       input);

//   using namespace options;

//   auto dependenciesFilePath = determineSupplementaryOutputFilename(
//       OPT_emit_dependencies, pathsFromArguments.DependenciesFilePath,
//       file_types::TY_Dependencies, "",
//       defaultSupplementaryOutputPathExcludingExtension);

//   auto referenceDependenciesFilePath = determineSupplementaryOutputFilename(
//       OPT_emit_reference_dependencies,
//       pathsFromArguments.ReferenceDependenciesFilePath,
//       file_types::TY_SwiftDeps, "",
//       defaultSupplementaryOutputPathExcludingExtension);

//   auto serializedDiagnosticsPath = determineSupplementaryOutputFilename(
//       OPT_serialize_diagnostics,
//       pathsFromArguments.SerializedDiagnosticsPath,
//       file_types::TY_SerializedDiagnostics, "",
//       defaultSupplementaryOutputPathExcludingExtension);

//   // There is no non-path form of -emit-fixits-path
//   auto fixItsOutputPath = pathsFromArguments.FixItsOutputPath;

//   auto objcHeaderOutputPath = determineSupplementaryOutputFilename(
//       OPT_emit_objc_header, pathsFromArguments.ObjCHeaderOutputPath,
//       file_types::TY_ObjCHeader, "",
//       defaultSupplementaryOutputPathExcludingExtension);

//   auto loadedModuleTracePath = determineSupplementaryOutputFilename(
//       OPT_emit_loaded_module_trace, pathsFromArguments.LoadedModuleTracePath,
//       file_types::TY_ModuleTrace, "",
//       defaultSupplementaryOutputPathExcludingExtension);

//   auto tbdPath = determineSupplementaryOutputFilename(
//       OPT_emit_tbd, pathsFromArguments.TBDPath, file_types::TY_TBD, "",
//       defaultSupplementaryOutputPathExcludingExtension);

//   auto moduleDocOutputPath = determineSupplementaryOutputFilename(
//       OPT_emit_module_doc, pathsFromArguments.ModuleDocOutputPath,
//       file_types::TY_SwiftModuleDocFile, "",
//       defaultSupplementaryOutputPathExcludingExtension);

//   auto moduleSourceInfoOutputPath = determineSupplementaryOutputFilename(
//       OPT_emit_module_source_info,
//       pathsFromArguments.ModuleSourceInfoOutputPath,
//       file_types::TY_SwiftSourceInfoFile, "",
//       defaultSupplementaryOutputPathExcludingExtension);
//   auto moduleSummaryOutputPath = determineSupplementaryOutputFilename(
//       OPT_emit_module_summary, pathsFromArguments.ModuleSummaryOutputPath,
//       file_types::TY_SwiftModuleSummaryFile, "",
//       defaultSupplementaryOutputPathExcludingExtension);

//   // There is no non-path form of -emit-interface-path
//   auto ModuleInterfaceOutputPath =
//       pathsFromArguments.ModuleInterfaceOutputPath;
//   auto PrivateModuleInterfaceOutputPath =
//       pathsFromArguments.PrivateModuleInterfaceOutputPath;

//   // There is no non-path form of -emit-abi-descriptor-path
//   auto ABIDescriptorOutputPath = pathsFromArguments.ABIDescriptorOutputPath;
//   auto ModuleSemanticInfoOutputPath =
//   pathsFromArguments.ModuleSemanticInfoOutputPath; ID emitModuleOption;
//   std::string moduleExtension;
//   std::string mainOutputIfUsableForModule;
//   deriveModulePathParameters(outputFile, emitModuleOption, moduleExtension,
//                              mainOutputIfUsableForModule);

//   auto moduleOutputPath = determineSupplementaryOutputFilename(
//       emitModuleOption, pathsFromArguments.ModuleOutputPath,
//       file_types::TY_SwiftModuleFile, mainOutputIfUsableForModule,
//       defaultSupplementaryOutputPathExcludingExtension);

//   auto YAMLOptRecordPath = determineSupplementaryOutputFilename(
//       OPT_save_optimization_record_path,
//       pathsFromArguments.YAMLOptRecordPath, file_types::TY_YAMLOptRecord, "",
//       defaultSupplementaryOutputPathExcludingExtension);
//   auto bitstreamOptRecordPath = determineSupplementaryOutputFilename(
//       OPT_save_optimization_record_path,
//       pathsFromArguments.BitstreamOptRecordPath,
//       file_types::TY_BitstreamOptRecord, "",
//       defaultSupplementaryOutputPathExcludingExtension);

//   SupplementaryOutputPaths sop;
//   sop.ObjCHeaderOutputPath = objcHeaderOutputPath;
//   sop.ModuleOutputPath = moduleOutputPath;
//   sop.ModuleDocOutputPath = moduleDocOutputPath;
//   sop.DependenciesFilePath = dependenciesFilePath;
//   sop.ReferenceDependenciesFilePath = referenceDependenciesFilePath;
//   sop.SerializedDiagnosticsPath = serializedDiagnosticsPath;
//   sop.FixItsOutputPath = fixItsOutputPath;
//   sop.LoadedModuleTracePath = loadedModuleTracePath;
//   sop.TBDPath = tbdPath;
//   sop.ModuleInterfaceOutputPath = ModuleInterfaceOutputPath;
//   sop.PrivateModuleInterfaceOutputPath = PrivateModuleInterfaceOutputPath;
//   sop.ModuleSourceInfoOutputPath = moduleSourceInfoOutputPath;
//   sop.ModuleSummaryOutputPath = moduleSummaryOutputPath;
//   sop.ABIDescriptorOutputPath = ABIDescriptorOutputPath;
//   sop.ModuleSemanticInfoOutputPath = ModuleSemanticInfoOutputPath;
//   sop.YAMLOptRecordPath = YAMLOptRecordPath;
//   sop.BitstreamOptRecordPath = bitstreamOptRecordPath;
//   return sop;
// }

llvm::StringRef SupplementaryOutputPathsComputer::
    DeriveDefaultSupplementaryOutputPathExcludingExtension(
        llvm::StringRef outputFilename, const LangInputFile &input) const {

  // Put the supplementary output file next to the output file if possible.
  if (!outputFilename.empty() && outputFilename != LangOptions::dash) {
    return outputFilename;
  }
  if (input.IsPrimary() && input.GetFileName() != LangOptions::dash) {
    return llvm::sys::path::filename(input.GetFileName());
  }
  return moduleName;
}

// std::string
// SupplementaryOutputPathsComputer::determineSupplementaryOutputFilename(
//     opts::OptID emitOpt, std::string pathFromArguments, file::Type type,
//     StringRef mainOutputIfUsable,
//     StringRef defaultSupplementaryOutputPathExcludingExtension) const {

//   if (!pathFromArguments.empty())
//     return pathFromArguments;

//   if (!args.hasArg(emitOpt))
//     return std::string();

//   if (!mainOutputIfUsable.empty()) {
//     return mainOutputIfUsable.str();
//   }

//   llvm::SmallString<128>
//   path(defaultSupplementaryOutputPathExcludingExtension);
//   llvm::sys::path::replace_extension(path, file_types::getExtension(type));
//   return path.str().str();
// }

// void SupplementaryOutputPathsComputer::deriveModulePathParameters(
//     StringRef mainOutputFile, opts::OptID &emitOption, std::string
//     &extension, std::string &mainOutputIfUsable) const {

//   bool isSIB = RequestedAction == FrontendOptions::ActionType::EmitSIB ||
//                RequestedAction == FrontendOptions::ActionType::EmitSIBGen;

//   emitOption = !isSIB ? opts::emit_module
//                       : RequestedAction ==
//                       FrontendOptions::ActionType::EmitSIB
//                             ? opts::emit_sib
//                             : opts::emit_sibgen;

//   bool canUseMainOutputForModule =
//       RequestedAction == FrontendOptions::ActionType::MergeModules ||
//       RequestedAction == FrontendOptions::ActionType::EmitModuleOnly ||
//       isSIB;

//   extension = file_types::getExtension(isSIB ? file_types::TY_SIB
//                                              :
//                                              file_types::TY_SwiftModuleFile)
//                   .str();

//   mainOutputIfUsable = canUseMainOutputForModule && !OutputFiles.empty()
//                            ? mainOutputFile.str()
//                            : "";
// }

// static SupplementaryOutputPaths
// createFromTypeToPathMap(const TypeToPathMap *map) {
//   SupplementaryOutputPaths paths;
//   if (!map)
//     return paths;
//   const std::pair<file::Type, std::string &> typesAndStrings[] = {
//       {file_types::TY_ObjCHeader, paths.ObjCHeaderOutputPath},
//       {file_types::TY_SwiftModuleFile, paths.ModuleOutputPath},
//       {file_types::TY_SwiftModuleDocFile, paths.ModuleDocOutputPath},
//       {file_types::TY_SwiftSourceInfoFile, paths.ModuleSourceInfoOutputPath},
//       {file_types::TY_Dependencies, paths.DependenciesFilePath},
//       {file_types::TY_SwiftDeps, paths.ReferenceDependenciesFilePath},
//       {file_types::TY_SerializedDiagnostics,
//       paths.SerializedDiagnosticsPath}, {file_types::TY_ModuleTrace,
//       paths.LoadedModuleTracePath}, {file_types::TY_TBD, paths.TBDPath},
//       {file_types::TY_SwiftModuleInterfaceFile,
//        paths.ModuleInterfaceOutputPath},
//       {file_types::TY_SwiftModuleSummaryFile, paths.ModuleSummaryOutputPath},
//       {file_types::TY_PrivateSwiftModuleInterfaceFile,
//        paths.PrivateModuleInterfaceOutputPath},
//       {file_types::TY_YAMLOptRecord, paths.YAMLOptRecordPath},
//       {file_types::TY_BitstreamOptRecord, paths.BitstreamOptRecordPath},
//       {file_types::TY_SwiftABIDescriptor, paths.ABIDescriptorOutputPath},
//   };
//   for (const std::pair<file::Type, std::string &> &typeAndString :
//        typesAndStrings) {
//     auto const out = map->find(typeAndString.first);
//     typeAndString.second = out == map->end() ? "" : out->second;
//   }
//   return paths;
// }

// Optional<std::vector<SupplementaryOutputPaths>>
// SupplementaryOutputPathsComputer::readSupplementaryOutputFileMap() const {
//   if (Arg *A = args.getLastArg(
//         opts::emit_objc_header_path,
//         opts::emit_module_path,
//         opts::emit_module_doc_path,
//         opts::emit_dependencies_path,
//         opts::emit_reference_dependencies_path,
//         opts::serialize_diagnostics_path,
//         opts::emit_loaded_module_trace_path,
//         opts::emit_module_interface_path,
//         opts::emit_private_module_interface_path,
//         opts::emit_module_source_info_path,
//         opts::emit_tbd_path)) {
//     de.PrintD(SrcLoc(),
//                    diag::error_cannot_have_supplementary_outputs,
//                    A->getSpelling(), "-supplementary-output-file-map");
//     return None;
//   }
//   const StringRef supplementaryFileMapPath =
//       args.getLastArgValue(opts::supplementary_output_file_map);

//   unsigned BadFileDescriptorRetryCount = 0;
//   if (const Arg *A = args.getLastArg(opts::bad_file_descriptor_retry_count))
//   {
//     if (StringRef(A->getValue()).getAsInteger(10,
//     BadFileDescriptorRetryCount)) {
//       de.PrintD(SrcLoc(), diag::error_invalid_arg_value,
//                      A->getAsString(args), A->getValue());
//       return None;
//     }
//   }

//   llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> buffer = nullptr;
//   for (unsigned I = 0; I < BadFileDescriptorRetryCount + 1; ++I) {
//     buffer = llvm::MemoryBuffer::getFile(supplementaryFileMapPath);
//     if (buffer)
//       break;
//     if (buffer.getError().value() != EBADF)
//       break;
//   }
//   if (!buffer) {
//     de.PrintD(SrcLoc(), diag::cannot_open_file,
//                    supplementaryFileMapPath, buffer.getError().message());
//     return None;
//   }
//   llvm::Expected<OutputFileMap> outputFileMap =
//       OutputFileMap::loadFromBuffer(std::move(buffer.get()), "");
//   if (auto Err = outputFileMap.takeError()) {
//     de.PrintD(SrcLoc(),
//                    diag::error_unable_to_load_supplementary_output_file_map,
//                    supplementaryFileMapPath, llvm::toString(std::move(Err)));
//     return None;
//   }

//   std::vector<SupplementaryOutputPaths> outputPaths;
//   bool hadError = false;
//   inputsAndOutputs.forEachInputProducingSupplementaryOutput(
//       [&](const LangInputFile &input) -> bool {
//         const TypeToPathMap *mapForInput =
//             outputFileMap->getOutputMapForInput(input.getFileName());
//         if (!mapForInput) {
//           de.PrintD(
//               SrcLoc(),
//               diag::error_missing_entry_in_supplementary_output_file_map,
//               supplementaryFileMapPath, input.getFileName());
//           hadError = true;
//         }
//         outputPaths.push_back(createFromTypeToPathMap(mapForInput));
//         return false;
//       });
//   if (hadError)
//     return None;

//   return outputPaths;
// }
