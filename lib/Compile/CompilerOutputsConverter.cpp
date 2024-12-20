#include "stone/AST/DiagnosticsCompile.h"
#include "stone/Basic/OutputFileMap.h"
#include "stone/Strings.h"
// #include "stone/Basic/Platform.h"
#include "stone/Compile/CompilerInputsConverter.h"
#include "stone/Compile/CompilerOptionsConverter.h"
#include "stone/Compile/CompilerOutputsConverter.h"

#include "stone/Support/Options.h"
// #include "stone/Option/SanitizerOptions.h"

#include "llvm/ADT/STLExtras.h"
#include "llvm/Option/Arg.h"
#include "llvm/Option/ArgList.h"
#include "llvm/Option/Option.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/LineIterator.h"
#include "llvm/Support/Path.h"
#include "llvm/TargetParser/Triple.h"

using namespace stone;
using namespace llvm::opt;

bool CompilerOutputsConverter::Convert(
    std::vector<std::string> &mainOutputs,
    std::vector<std::string> &mainOutputsForIndexUnits,
    std::vector<SupplementaryOutputPaths> &supplementaryOutputs,
    CompilerActionKind actionKind) {

  std::optional<CompilerOutputFilesComputer> ofc =
      CompilerOutputFilesComputer::Create(
          args, de, inputsAndOutputs,
          {"output", opts::OPT_o, opts::OPT_OutputFileList, "-o"}, actionKind);

  if (!ofc) {
    return true;
  }
  std::optional<std::vector<std::string>> mains = ofc->ComputeOutputFiles();
  if (!mains) {
    return true;
  }

  std::optional<std::vector<std::string>> indexMains;
  if (args.hasArg(opts::OPT_IndexUnitOutputPath,
                  opts::OPT_IndexUnitOutputPathFileList)) {

    std::optional<CompilerOutputFilesComputer> iuofc =
        CompilerOutputFilesComputer::Create(
            args, de, inputsAndOutputs,
            {"index unit output path", opts::OPT_IndexUnitOutputPath,
             opts::OPT_IndexUnitOutputPathFileList, "-index-unit-output-path"},
            actionKind);

    if (!iuofc) {
      return true;
    }
    indexMains = iuofc->ComputeOutputFiles();
    if (!indexMains) {
      return true;
    }

    assert(mains->size() == indexMains->size() && "checks not equivalent?");
  }

  std::optional<std::vector<SupplementaryOutputPaths>> supplementaries =
      SupplementaryOutputPathsComputer(args, de, inputsAndOutputs, *mains,
                                       moduleName, actionKind)
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

std::optional<std::vector<std::string>>
CompilerOutputsConverter::ReadOutputFileList(const llvm::StringRef fileListPath,
                                             DiagnosticEngine &de) {
  llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> buffer =
      llvm::MemoryBuffer::getFile(fileListPath);
  if (!buffer) {
    de.diagnose(SrcLoc(), diag::error_cannot_open_file, fileListPath,
                buffer.getError().message());
    return std::nullopt;
  }
  std::vector<std::string> outputFiles;
  for (llvm::StringRef line :
       make_range(llvm::line_iterator(*buffer.get()), {})) {
    outputFiles.push_back(line.str());
  }
  return outputFiles;
}

std::optional<std::vector<std::string>>
CompilerOutputFilesComputer::GetOutputFilenamesFromCommandLineOrFileList(
    const ArgList &args, DiagnosticEngine &de, opts::OptID singleOpt,
    opts::OptID fileListOpt) {
  if (const Arg *A = args.getLastArg(fileListOpt)) {

    assert(
        !args.hasArg(singleOpt) &&
        "don't use -o with -output-filelist or -index-unit-output-path with" &&
        " " && "-index-unit-output-filelist");

    return CompilerOutputsConverter::ReadOutputFileList(A->getValue(), de);
  }
  return args.getAllArgValues(singleOpt);
}

std::optional<CompilerOutputFilesComputer> CompilerOutputFilesComputer::Create(
    const llvm::opt::ArgList &args, DiagnosticEngine &de,
    const CompilerInputsAndOutputs &inputsAndOutputs,
    CompilerOutputOptInfo optInfo, CompilerActionKind actionKind) {
  std::optional<std::vector<std::string>> outputArguments =
      GetOutputFilenamesFromCommandLineOrFileList(args, de, optInfo.SingleID,
                                                  optInfo.FilelistID);
  if (!outputArguments)
    return std::nullopt;
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
    de.diagnose(SrcLoc(),
                diag::error_if_any_output_files_are_specified_they_all_must_be,
                optInfo.PrettyName);
    return std::nullopt;
  }

  auto actionOutputFileType =
      CompilerOptions::GetActionOutputFileType(actionKind);

  return CompilerOutputFilesComputer(
      de, inputsAndOutputs, std::move(outputFileArguments),
      outputDirectoryArgument, firstInput, actionKind,
      args.getLastArg(opts::OPT_ModuleName),
      stone::GetFileTypeExt(actionOutputFileType),
      CompilerOptions::DoesActionProduceOutput(actionKind), optInfo);
}

CompilerOutputFilesComputer::CompilerOutputFilesComputer(
    DiagnosticEngine &de, const CompilerInputsAndOutputs &inputsAndOutputs,
    std::vector<std::string> outputFileArguments,
    const StringRef outputDirectoryArgument, const StringRef firstInput,
    CompilerActionKind actionKind, const llvm::opt::Arg *moduleNameArg,
    const StringRef suffix, const bool hasTextualOutput,
    CompilerOutputOptInfo optInfo)
    : de(de), inputsAndOutputs(inputsAndOutputs),
      OutputFileArguments(outputFileArguments),
      OutputDirectoryArgument(outputDirectoryArgument), FirstInput(firstInput),
      actionKind(actionKind), moduleNameArg(moduleNameArg), Suffix(suffix),
      HasTextualOutput(hasTextualOutput), OutputInfo(optInfo) {}

std::optional<std::vector<std::string>>
CompilerOutputFilesComputer::ComputeOutputFiles() const {
  std::vector<std::string> outputFiles;
  unsigned i = 0;
  bool hadError = inputsAndOutputs.ForEachInputProducingAMainOutputFile(
      [&](const CompilerInputFile &input) -> bool {
        StringRef outputArg = OutputFileArguments.empty()
                                  ? llvm::StringRef()
                                  : llvm::StringRef(OutputFileArguments[i++]);

        std::optional<std::string> outputFile =
            ComputeOutputFile(outputArg, input);
        if (!outputFile)
          return true;
        outputFiles.push_back(*outputFile);
        return false;
      });
  return hadError ? std::nullopt
                  : std::optional<std::vector<std::string>>(outputFiles);
}

std::optional<std::string> CompilerOutputFilesComputer::ComputeOutputFile(
    StringRef outputArg, const CompilerInputFile &input) const {
  // Return an empty string to signify no output.
  // The invocation does not currently produce a diagnostic
  // if a -o argument is present for such an action
  // for instance stonec -invocation -o foo -interpret foo.stone

  if (!CompilerOptions::DoesActionProduceOutput(actionKind)) {
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

std::optional<std::string>
CompilerOutputFilesComputer::DeriveOutputFileFromInput(
    const CompilerInputFile &input) const {

  if (input.GetFileName() == strings::Dash || HasTextualOutput) {
    return std::string(strings::Dash);
  }

  std::string baseName = DetermineBaseNameOfOutput(input);
  if (baseName.empty()) {
    // Assuming CompilerOptions::doesJobActionProduceOutput(RequestedJobAction)
    de.diagnose(SrcLoc(), diag::error_no_output_filename_specified,
                OutputInfo.PrettyName);
    return std::nullopt;
  }
  return DeriveOutputFileFromParts("", baseName);
}

std::optional<std::string>
CompilerOutputFilesComputer::DeriveOutputFileForDirectory(
    const CompilerInputFile &input) const {
  std::string baseName = DetermineBaseNameOfOutput(input);
  if (baseName.empty()) {
    de.diagnose(SrcLoc(), diag::error_implicit_output_file_is_directory,
                OutputDirectoryArgument, OutputInfo.SingleOptSpelling);
    return std::nullopt;
  }
  return DeriveOutputFileFromParts(OutputDirectoryArgument, baseName);
}

std::string CompilerOutputFilesComputer::DetermineBaseNameOfOutput(
    const CompilerInputFile &input) const {
  std::string nameToStem = input.IsPrimary() ? input.GetFileName()
                           : moduleNameArg   ? moduleNameArg->getValue()
                                             : FirstInput;
  return llvm::sys::path::stem(nameToStem).str();
}

std::string
CompilerOutputFilesComputer::DeriveOutputFileFromParts(StringRef dir,
                                                       StringRef base) const {
  assert(!base.empty());
  llvm::SmallString<128> path(dir);
  llvm::sys::path::append(path, base);
  llvm::sys::path::replace_extension(path, Suffix);
  return std::string(path.str());
}

SupplementaryOutputPathsComputer::SupplementaryOutputPathsComputer(
    const ArgList &args, DiagnosticEngine &de,
    const CompilerInputsAndOutputs &inputsAndOutputs,
    ArrayRef<std::string> outputFiles, StringRef moduleName,
    CompilerActionKind actionKind)
    : args(args), de(de), inputsAndOutputs(inputsAndOutputs),
      OutputFiles(outputFiles), moduleName(moduleName), actionKind(actionKind) {
}

std::optional<std::vector<SupplementaryOutputPaths>>
SupplementaryOutputPathsComputer::ComputeOutputPaths() const {
  std::optional<std::vector<SupplementaryOutputPaths>> pathsFromUser =
      args.hasArg(opts::OPT_SupplementaryOutputFileMap)
          ? ReadSupplementaryOutputFileMap()
          : GetSupplementaryOutputPathsFromArguments();
  if (!pathsFromUser)
    return std::nullopt;

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
      [&](const CompilerInputFile &input) -> bool {
        if (auto suppPaths = ComputeOutputPathsForOneInput(
                OutputFiles[i], (*pathsFromUser)[i], input)) {
          ++i;
          outputPaths.push_back(*suppPaths);
          return false;
        }
        return true;
      });
  if (hadError)
    return std::nullopt;
  return outputPaths;
}

std::optional<std::vector<SupplementaryOutputPaths>>
SupplementaryOutputPathsComputer::GetSupplementaryOutputPathsFromArguments()
    const {

  llvm_unreachable(
      "TODO: GetSupplementaryOutputPathsFromArguments  not implemented");

  // auto moduleOutput =
  //     GetSupplementaryFilenamesFromArguments(opts::OPT_EmitModulePath);

  // auto moduleDocOutput =
  //     GetSupplementaryFilenamesFromArguments(opts::OPT_EmitModuleDocPath);

  // auto dependenciesFile =
  //     GetSupplementaryFilenamesFromArguments(opts::OPT_EmitDependenciesPath);

  // auto referenceDependenciesFile = GetSupplementaryFilenamesFromArguments(
  //     opts::OPT_EmitReferenceDependenciesPath);

  // auto serializedDiagnostics =
  //     GetSupplementaryFilenamesFromArguments(opts::OPT_SerializeDiagnosticsPath);

  // auto fixItsOutput =
  //     GetSupplementaryFilenamesFromArguments(opts::OPT_EmitFixitsPath);

  // auto loadedModuleTrace =
  //     GetSupplementaryFilenamesFromArguments(opts::OPT_EmitLoadedModuleTracePath);

  // auto TBD = GetSupplementaryFilenamesFromArguments(opts::OPT_EmitTBDPath);

  // auto moduleInterfaceOutput =
  //     GetSupplementaryFilenamesFromArguments(opts::OPT_EmitModuleInterfacePath);

  // auto privateModuleInterfaceOutput = GetSupplementaryFilenamesFromArguments(
  //     opts::OPT_EmitPrivateModuleInterfacePath);

  // auto moduleSourceInfoOutput =
  //     GetSupplementaryFilenamesFromArguments(opts::OPT_EmitModuleSourceInfoPath);

  // auto moduleSummaryOutput =
  //     GetSupplementaryFilenamesFromArguments(opts::OPT_EmitModuleSummaryPath);

  // auto abiDescriptorOutput =
  //     GetSupplementaryFilenamesFromArguments(opts::OPT_EmitABIDescriptorPath);

  // auto moduleSemanticInfoOutput =
  //     GetSupplementaryFilenamesFromArguments(opts::OPT_EmitModuleSemanticInfoPath);

  // auto optRecordOutput =
  //     GetSupplementaryFilenamesFromArguments(opts::OPT_SaveOptimizationRecordPath);

  // if (!moduleOutput || !moduleDocOutput || !dependenciesFile ||
  //     !referenceDependenciesFile || !serializedDiagnostics || !fixItsOutput
  //     || !loadedModuleTrace || !TBD || !moduleInterfaceOutput ||
  //     !privateModuleInterfaceOutput || !moduleSourceInfoOutput ||
  //     !moduleSummaryOutput || !abiDescriptorOutput ||
  //     !moduleSemanticInfoOutput || !optRecordOutput) {
  //   return std::nullopt;
  // }
  std::vector<SupplementaryOutputPaths> result;

  // const unsigned N =
  //     inputsAndOutputs.countOfFilesProducingSupplementaryOutput();

  // for (unsigned i = 0; i < N; ++i) {
  //   SupplementaryOutputPaths sop;
  //   sop.moduleOutputPath = (*moduleOutput)[i];
  //   sop.moduleDocOutputPath = (*moduleDocOutput)[i];
  //   sop.dependenciesFilePath = (*dependenciesFile)[i];
  //   sop.referenceDependenciesFilePath = (*referenceDependenciesFile)[i];
  //   sop.serializedDiagnosticsPath = (*serializedDiagnostics)[i];
  //   sop.fixItsOutputPath = (*fixItsOutput)[i];
  //   sop.loadedModuleTracePath = (*loadedModuleTrace)[i];
  //   sop.tbdPath = (*TBD)[i];
  //   sop.moduleInterfaceOutputPath = (*moduleInterfaceOutput)[i];
  //   sop.privateModuleInterfaceOutputPath =
  //   (*privateModuleInterfaceOutput)[i]; sop.moduleSourceInfoOutputPath =
  //   (*moduleSourceInfoOutput)[i]; sop.moduleSummaryOutputPath =
  //   (*moduleSummaryOutput)[i]; sop.abiDescriptorOutputPath =
  //   (*abiDescriptorOutput)[i]; sop.moduleSemanticInfoOutputPath =
  //   (*moduleSemanticInfoOutput)[i]; sop.yamlOptRecordPath =
  //   (*optRecordOutput)[i]; sop.bitstreamOptRecordPath =
  //   (*optRecordOutput)[i]; result.push_back(sop);
  // }
  return result;
}

// // Extend this routine for filelists if/when we have them.

std::optional<std::vector<std::string>>
SupplementaryOutputPathsComputer::GetSupplementaryFilenamesFromArguments(
    opts::OptID pathID) const {
  std::vector<std::string> paths = args.getAllArgValues(pathID);

  const unsigned N =
      inputsAndOutputs.CountOfFilesProducingSupplementaryOutput();

  if (paths.size() == N) {
    return paths;
  } else if (pathID == opts::OPT_EmitLoadedModuleTracePath &&
             paths.size() < N) {
    // We only need one file to output the module trace file because they
    // are all equivalent. Add supplementary empty output paths for moduletrace
    // to make sure the compiler won't panic for
    // diag::error_wrong_number_of_arguments.

    for (unsigned I = paths.size(); I != N; I++) {
      paths.emplace_back();
    }
    return paths;
  }

  if (paths.empty()) {
    return std::vector<std::string>(N, std::string());
  }

  de.diagnose(SrcLoc(), diag::error_wrong_number_of_arguments,
              args.getLastArg(pathID)->getOption().getPrefixedName(), N,
              paths.size());
  return std::nullopt;
}

std::optional<SupplementaryOutputPaths>
SupplementaryOutputPathsComputer::ComputeOutputPathsForOneInput(
    StringRef outputFile, const SupplementaryOutputPaths &pathsFromArguments,
    const CompilerInputFile &input) const {
  StringRef defaultSupplementaryOutputPathExcludingExtension =
      DeriveDefaultSupplementaryOutputPathExcludingExtension(outputFile, input);

  // TODO:
  //  auto dependenciesFilePath = DetermineSupplementaryOutputFilename(
  //      opts::OPT_EmitDependenciesPath,
  //      pathsFromArguments.dependenciesFilePath, FileType::Dependencies, "",
  //      defaultSupplementaryOutputPathExcludingExtension);

  auto referenceDependenciesFilePath = DetermineSupplementaryOutputFilename(
      opts::OPT_EmitReferenceDependencies,
      pathsFromArguments.referenceDependenciesFilePath, FileType::StoneDeps, "",
      defaultSupplementaryOutputPathExcludingExtension);

  auto serializedDiagnosticsPath = DetermineSupplementaryOutputFilename(
      opts::OPT_SerializeDiagnosticsPath,
      pathsFromArguments.serializedDiagnosticsPath,
      FileType::SerializedDiagnostics, "",
      defaultSupplementaryOutputPathExcludingExtension);

  // There is no non-path form of -emit-fixits-path
  auto fixItsOutputPath = pathsFromArguments.fixItsOutputPath;

  // auto objcHeaderOutputPath = DetermineSupplementaryOutputFilename(
  //     opts::OPT_emit_objc_header, pathsFromArguments.ObjCHeaderOutputPath,
  //     FileType::ObjCHeader, "",
  //     defaultSupplementaryOutputPathExcludingExtension);

  // auto loadedModuleTracePath = DetermineSupplementaryOutputFilename(
  //     opts::OPT_EmitLoadedModuleTrace,
  //     pathsFromArguments.loadedModuleTracePath, FileType::ModuleTrace, "",
  //     defaultSupplementaryOutputPathExcludingExtension);

  auto tbdPath = DetermineSupplementaryOutputFilename(
      opts::OPT_EmitTBD, pathsFromArguments.tbdPath, FileType::TBD, "",
      defaultSupplementaryOutputPathExcludingExtension);

  auto moduleDocOutputPath = DetermineSupplementaryOutputFilename(
      opts::OPT_EmitModuleDoc, pathsFromArguments.moduleDocOutputPath,
      FileType::StoneModuleDoc, "",
      defaultSupplementaryOutputPathExcludingExtension);

  auto moduleSourceInfoOutputPath = DetermineSupplementaryOutputFilename(
      opts::OPT_EmitModuleSourceInfo,
      pathsFromArguments.moduleSourceInfoOutputPath, FileType::StoneSourceInfo,
      "", defaultSupplementaryOutputPathExcludingExtension);
  auto moduleSummaryOutputPath = DetermineSupplementaryOutputFilename(
      opts::OPT_EmitModuleSummary, pathsFromArguments.moduleSummaryOutputPath,
      FileType::StoneModuleSummary, "",
      defaultSupplementaryOutputPathExcludingExtension);

  // There is no non-path form of -emit-interface-path
  auto moduleInterfaceOutputPath = pathsFromArguments.moduleInterfaceOutputPath;
  auto privateModuleInterfaceOutputPath =
      pathsFromArguments.privateModuleInterfaceOutputPath;

  // There is no non-path form of -emit-abi-descriptor-path
  auto abiDescriptorOutputPath = pathsFromArguments.abiDescriptorOutputPath;
  auto moduleSemanticInfoOutputPath =
      pathsFromArguments.moduleSemanticInfoOutputPath;

  opts::OptID emitModuleOption;
  std::string moduleExtension;
  std::string mainOutputIfUsableForModule;
  DeriveModulePathParameters(outputFile, emitModuleOption, moduleExtension,
                             mainOutputIfUsableForModule);

  auto moduleOutputPath = DetermineSupplementaryOutputFilename(
      emitModuleOption, pathsFromArguments.moduleOutputPath,
      FileType::StoneModuleFile, mainOutputIfUsableForModule,
      defaultSupplementaryOutputPathExcludingExtension);

  // auto yamlOptRecordPath = DetermineSupplementaryOutputFilename(
  //     opts::OPT_SaveOptimizationRecordPath,
  //     pathsFromArguments.YAMLOptRecordPath, FileType::yamlOptRecord, "",
  //     defaultSupplementaryOutputPathExcludingExtension);

  // auto bitstreamOptRecordPath = DetermineSupplementaryOutputFilename(
  //     opts::OPT_SaveOptimizationRecordPath,
  //     pathsFromArguments.BitstreamOptRecordPath,
  //     FileType::BitstreamOptRecord,
  //     "", defaultSupplementaryOutputPathExcludingExtension);

  SupplementaryOutputPaths sop;
  // sop.ObjCHeaderOutputPath = objcHeaderOutputPath;
  sop.moduleOutputPath = moduleOutputPath;
  sop.moduleDocOutputPath = moduleDocOutputPath;
  // sop.dependenciesFilePath = dependenciesFilePath;
  sop.referenceDependenciesFilePath = referenceDependenciesFilePath;
  sop.serializedDiagnosticsPath = serializedDiagnosticsPath;
  sop.fixItsOutputPath = fixItsOutputPath;
  // sop.loadedModuleTracePath = loadedModuleTracePath;
  sop.tbdPath = tbdPath;
  sop.moduleInterfaceOutputPath = moduleInterfaceOutputPath;
  sop.privateModuleInterfaceOutputPath = privateModuleInterfaceOutputPath;
  sop.moduleSourceInfoOutputPath = moduleSourceInfoOutputPath;
  sop.moduleSummaryOutputPath = moduleSummaryOutputPath;
  sop.abiDescriptorOutputPath = abiDescriptorOutputPath;
  sop.moduleSemanticInfoOutputPath = moduleSemanticInfoOutputPath;
  // sop.yamlOptRecordPath = yamlOptRecordPath;
  // sop.bitstreamOptRecordPath = bitstreamOptRecordPath;
  return sop;
}

llvm::StringRef SupplementaryOutputPathsComputer::
    DeriveDefaultSupplementaryOutputPathExcludingExtension(
        llvm::StringRef outputFilename, const CompilerInputFile &input) const {

  // Put the supplementary output file next to the output file if possible.
  if (!outputFilename.empty() && outputFilename != strings::Dash) {
    return outputFilename;
  }
  if (input.IsPrimary() && input.GetFileName() != strings::Dash) {
    return llvm::sys::path::filename(input.GetFileName());
  }
  return moduleName;
}

std::string
SupplementaryOutputPathsComputer::DetermineSupplementaryOutputFilename(
    opts::OptID emitOpt, std::string pathFromArguments, FileType type,
    StringRef mainOutputIfUsable,
    StringRef defaultSupplementaryOutputPathExcludingExtension) const {

  if (!pathFromArguments.empty()) {
    return pathFromArguments;
  }

  if (!args.hasArg(emitOpt)) {
    return std::string();
  }

  if (!mainOutputIfUsable.empty()) {
    return mainOutputIfUsable.str();
  }
  llvm::SmallString<128> path(defaultSupplementaryOutputPathExcludingExtension);
  llvm::sys::path::replace_extension(path, stone::GetFileTypeExt(type));
  return path.str().str();
}

void SupplementaryOutputPathsComputer::DeriveModulePathParameters(
    StringRef mainOutputFile, opts::OptID &emitOption, std::string &extension,
    std::string &mainOutputIfUsable) const {

  emitOption = opts::OPT_EmitModule;

  bool canUseMainOutputForModule =
      actionKind == CompilerActionKind::MergeModules ||
      actionKind == CompilerActionKind::EmitModule;

  extension = stone::GetFileTypeExt(FileType::StoneModuleFile).str();

  mainOutputIfUsable = canUseMainOutputForModule && !OutputFiles.empty()
                           ? mainOutputFile.str()
                           : "";
}

static SupplementaryOutputPaths
CreateFromTypeToPathMap(const TypeToPathMap *map) {
  SupplementaryOutputPaths paths;
  if (!map) {
    return paths;
  }
  // TODO:
  const std::pair<FileType, std::string &> typesAndStrings[] = {
      {FileType::StoneModuleFile, paths.moduleOutputPath},
      // {FileType::StoneModuleDoc, paths.ModuleDocOutputPath},
      // {FileType::StoneSourceInfoFile, paths.ModuleSourceInfoOutputPath},
      // {FileType::Dependencies, paths.dependenciesFilePath},
      // {FileType::StoneDeps, paths.referenceDependenciesFilePath},
      // {FileType::SerializeDiagnostics,
      // paths.SerializeDiagnosticsPath}, {FileType::ModuleTrace,
      // paths.LoadedModuleTracePath}, {FileType::TBD, paths.TBDPath},
      // {FileType::StoneModuleInterfaceFile,
      //  paths.ModuleInterfaceOutputPath},
      // {FileType::StoneModuleSummaryFile, paths.ModuleSummaryOutputPath},
      // {FileType::PrivateStoneModuleInterfaceFile,
      //  paths.PrivateModuleInterfaceOutputPath},
      // {FileType::YAMLOptRecord, paths.YAMLOptRecordPath},
      // {FileType::BitstreamOptRecord, paths.BitstreamOptRecordPath},
      // {FileType::StoneABIDescriptor, paths.ABIDescriptorOutputPath},
  };
  for (const std::pair<FileType, std::string &> &typeAndString :
       typesAndStrings) {
    auto const out = map->find(typeAndString.first);
    typeAndString.second = out == map->end() ? "" : out->second;
  }
  return paths;
}

std::optional<std::vector<SupplementaryOutputPaths>>
SupplementaryOutputPathsComputer::ReadSupplementaryOutputFileMap() const {

  // TODO:
  //  if (Arg *A = args.getLastArg(
  //        opts::OPT_emit_objc_header_path,
  //        opts::OPT_emit_module_path,
  //        opts::OPT_emit_module_doc_path,
  //        opts::OPT_emit_dependencies_path,
  //        opts::OPT_emit_reference_dependencies_path,
  //        opts::OPT_serialize_diagnostics_path,
  //        opts::OPT_emit_loaded_module_trace_path,
  //        opts::OPT_emit_module_interface_path,
  //        opts::OPT_emit_private_module_interface_path,
  //        opts::OPT_emit_module_source_info_path,
  //        opts::OPT_emit_tbd_path)) {
  //    de.diagnose(SrcLoc(),
  //                   diag::error_cannot_have_supplementary_outputs,
  //                   A->getSpelling(), "-supplementary-output-file-map");
  //    return std::nullopt;
  //  }

  const StringRef supplementaryFileMapPath =
      args.getLastArgValue(opts::OPT_SupplementaryOutputFileMap);

  unsigned BadFileDescriptorRetryCount = 0;
  if (const Arg *A = args.getLastArg(opts::OPT_BadFileDescriptorRetryCount)) {
    if (StringRef(A->getValue())
            .getAsInteger(10, BadFileDescriptorRetryCount)) {
      de.diagnose(SrcLoc(), diag::error_invalid_arg_value, A->getAsString(args),
                  A->getValue());
      return std::nullopt;
    }
  }
  llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> buffer = nullptr;
  for (unsigned I = 0; I < BadFileDescriptorRetryCount + 1; ++I) {
    buffer = llvm::MemoryBuffer::getFile(supplementaryFileMapPath);
    if (buffer) {
      break;
    }
    if (buffer.getError().value() != EBADF) {
      break;
    }
  }
  if (!buffer) {
    de.diagnose(SrcLoc(), diag::error_cannot_open_file,
                supplementaryFileMapPath, buffer.getError().message());
    return std::nullopt;
  }
  llvm::Expected<OutputFileMap> outputFileMap =
      OutputFileMap::LoadFromBuffer(std::move(buffer.get()), "");
  if (auto Err = outputFileMap.takeError()) {
    de.diagnose(SrcLoc(),
                diag::error_unable_to_load_supplementary_output_file_map,
                supplementaryFileMapPath, llvm::toString(std::move(Err)));
    return std::nullopt;
  }

  std::vector<SupplementaryOutputPaths> outputPaths;
  bool hadError = false;
  inputsAndOutputs.ForEachInputProducingSupplementaryOutput(
      [&](const CompilerInputFile &input) -> bool {
        const TypeToPathMap *mapForInput =
            outputFileMap->GetOutputMapForInput(input.GetFileName());
        if (!mapForInput) {
          de.diagnose(
              SrcLoc(),
              diag::error_missing_entry_in_supplementary_output_file_map,
              supplementaryFileMapPath, input.GetFileName());
          hadError = true;
        }
        outputPaths.push_back(CreateFromTypeToPathMap(mapForInput));
        return false;
      });
  if (hadError) {
    return std::nullopt;
  }
  return outputPaths;
}
