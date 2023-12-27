#include "stone/Basic/OutputFileMap.h"
#include "stone/Diag/CompilerDiagnostic.h"
#include "stone/Strings.h"
// #include "stone/Basic/Platform.h"
#include "stone/Compile/CompilerInputsConverter.h"
#include "stone/Compile/CompilerOptionsConverter.h"
#include "stone/Compile/CompilerOutputsConverter.h"

#include "stone/Option/Options.h"
// #include "stone/Option/SanitizerOptions.h"

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

bool CompilerOutputsConverter::Convert(
    std::vector<std::string> &mainOutputs,
    std::vector<std::string> &mainOutputsForIndexUnits,
    std::vector<SupplementaryOutputPaths> &supplementaryOutputs,
    const Action &action) {

  Optional<CompilerOutputFilesComputer> ofc =
      CompilerOutputFilesComputer::Create(
          args, de, inputsAndOutputs,
          {"output", opts::o, opts::OutputFileList, "-o"}, action);

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

    llvm::Optional<CompilerOutputFilesComputer> iuofc =
        CompilerOutputFilesComputer::Create(
            args, de, inputsAndOutputs,
            {"index unit output path", opts::IndexUnitOutputPath,
             opts::IndexUnitOutputPathFileList, "-index-unit-output-path"},
            action);

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
                                       moduleName, action)
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
CompilerOutputsConverter::ReadOutputFileList(const llvm::StringRef fileListPath,
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

llvm::Optional<CompilerOutputFilesComputer> CompilerOutputFilesComputer::Create(
    const llvm::opt::ArgList &args, DiagnosticEngine &de,
    const CompilerInputsAndOutputs &inputsAndOutputs,
    CompilerOutputOptInfo optInfo, const Action &action) {
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

  auto outputType = action.GetOutputFileType();

  return CompilerOutputFilesComputer(
      de, inputsAndOutputs, std::move(outputFileArguments),
      outputDirectoryArgument, firstInput, action,
      args.getLastArg(opts::ModuleName), file::GetTypeExt(outputType),
      action.CanOutput(), optInfo);
}

CompilerOutputFilesComputer::CompilerOutputFilesComputer(
    DiagnosticEngine &de, const CompilerInputsAndOutputs &inputsAndOutputs,
    std::vector<std::string> outputFileArguments,
    const StringRef outputDirectoryArgument, const StringRef firstInput,
    const Action &action, const llvm::opt::Arg *moduleNameArg,
    const StringRef suffix, const bool hasTextualOutput,
    CompilerOutputOptInfo optInfo)
    : de(de), inputsAndOutputs(inputsAndOutputs),
      OutputFileArguments(outputFileArguments),
      OutputDirectoryArgument(outputDirectoryArgument), FirstInput(firstInput),
      action(action), moduleNameArg(moduleNameArg), Suffix(suffix),
      HasTextualOutput(hasTextualOutput), OutputInfo(optInfo) {}

llvm::Optional<std::vector<std::string>>
CompilerOutputFilesComputer::ComputeOutputFiles() const {
  std::vector<std::string> outputFiles;
  unsigned i = 0;
  bool hadError = inputsAndOutputs.ForEachInputProducingAMainOutputFile(
      [&](const CompilerInputFile &input) -> bool {
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

llvm::Optional<std::string> CompilerOutputFilesComputer::ComputeOutputFile(
    StringRef outputArg, const CompilerInputFile &input) const {
  // Return an empty string to signify no output.
  // The invocation does not currently produce a diagnostic
  // if a -o argument is present for such an action
  // for instance stonec -invocation -o foo -interpret foo.stone
  if (!action.CanOutput()) {
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

llvm::Optional<std::string>
CompilerOutputFilesComputer::DeriveOutputFileFromInput(
    const CompilerInputFile &input) const {

  if (input.GetFileName() == strings::Dash || HasTextualOutput) {
    return std::string(strings::Dash);
  }

  std::string baseName = DetermineBaseNameOfOutput(input);
  if (baseName.empty()) {
    // Assuming CompilerOptions::doesJobActionProduceOutput(RequestedJobAction)
    de.PrintD(SrcLoc(), diag::err_no_output_filename_specified,
              diag::LLVMStr(OutputInfo.PrettyName));
    return llvm::None;
  }
  return DeriveOutputFileFromParts("", baseName);
}

Optional<std::string> CompilerOutputFilesComputer::DeriveOutputFileForDirectory(
    const CompilerInputFile &input) const {
  std::string baseName = DetermineBaseNameOfOutput(input);
  if (baseName.empty()) {
    de.PrintD(SrcLoc(), diag::err_implicit_output_file_is_directory,
              diag::LLVMStr(OutputDirectoryArgument),
              diag::LLVMStr(OutputInfo.SingleOptSpelling));
    return None;
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
    const Action &action)
    : args(args), de(de), inputsAndOutputs(inputsAndOutputs),
      OutputFiles(outputFiles), moduleName(moduleName), action(action) {}

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
    return None;
  return outputPaths;
}

Optional<std::vector<SupplementaryOutputPaths>>
SupplementaryOutputPathsComputer::GetSupplementaryOutputPathsFromArguments()
    const {

  assert(false && "Not implemented");

  // auto moduleOutput =
  //     GetSupplementaryFilenamesFromArguments(opts::EmitModulePath);

  // auto moduleDocOutput =
  //     GetSupplementaryFilenamesFromArguments(opts::EmitModuleDocPath);

  // auto dependenciesFile =
  //     GetSupplementaryFilenamesFromArguments(opts::EmitDependenciesPath);

  // auto referenceDependenciesFile = GetSupplementaryFilenamesFromArguments(
  //     opts::EmitReferenceDependenciesPath);

  // auto serializedDiagnostics =
  //     GetSupplementaryFilenamesFromArguments(opts::SerializeDiagnosticsPath);

  // auto fixItsOutput =
  //     GetSupplementaryFilenamesFromArguments(opts::EmitFixitsPath);

  // auto loadedModuleTrace =
  //     GetSupplementaryFilenamesFromArguments(opts::EmitLoadedModuleTracePath);

  // auto TBD = GetSupplementaryFilenamesFromArguments(opts::EmitTBDPath);

  // auto moduleInterfaceOutput =
  //     GetSupplementaryFilenamesFromArguments(opts::EmitModuleInterfacePath);

  // auto privateModuleInterfaceOutput = GetSupplementaryFilenamesFromArguments(
  //     opts::EmitPrivateModuleInterfacePath);

  // auto moduleSourceInfoOutput =
  //     GetSupplementaryFilenamesFromArguments(opts::EmitModuleSourceInfoPath);

  // auto moduleSummaryOutput =
  //     GetSupplementaryFilenamesFromArguments(opts::EmitModuleSummaryPath);

  // auto abiDescriptorOutput =
  //     GetSupplementaryFilenamesFromArguments(opts::EmitABIDescriptorPath);

  // auto moduleSemanticInfoOutput =
  //     GetSupplementaryFilenamesFromArguments(opts::EmitModuleSemanticInfoPath);

  // auto optRecordOutput =
  //     GetSupplementaryFilenamesFromArguments(opts::SaveOptimizationRecordPath);

  // if (!moduleOutput || !moduleDocOutput || !dependenciesFile ||
  //     !referenceDependenciesFile || !serializedDiagnostics || !fixItsOutput
  //     || !loadedModuleTrace || !TBD || !moduleInterfaceOutput ||
  //     !privateModuleInterfaceOutput || !moduleSourceInfoOutput ||
  //     !moduleSummaryOutput || !abiDescriptorOutput ||
  //     !moduleSemanticInfoOutput || !optRecordOutput) {
  //   return None;
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

llvm::Optional<SupplementaryOutputPaths>
SupplementaryOutputPathsComputer::ComputeOutputPathsForOneInput(
    StringRef outputFile, const SupplementaryOutputPaths &pathsFromArguments,
    const CompilerInputFile &input) const {
  StringRef defaultSupplementaryOutputPathExcludingExtension =
      DeriveDefaultSupplementaryOutputPathExcludingExtension(outputFile, input);

  auto dependenciesFilePath = DetermineSupplementaryOutputFilename(
      opts::EmitDependenciesPath, pathsFromArguments.dependenciesFilePath,
      file::Type::Dependencies, "",
      defaultSupplementaryOutputPathExcludingExtension);

  auto referenceDependenciesFilePath = DetermineSupplementaryOutputFilename(
      opts::EmitReferenceDependencies,
      pathsFromArguments.referenceDependenciesFilePath, file::Type::StoneDeps,
      "", defaultSupplementaryOutputPathExcludingExtension);

  auto serializedDiagnosticsPath = DetermineSupplementaryOutputFilename(
      opts::SerializeDiagnosticsPath,
      pathsFromArguments.serializedDiagnosticsPath,
      file::Type::SerializedDiagnostics, "",
      defaultSupplementaryOutputPathExcludingExtension);

  // There is no non-path form of -emit-fixits-path
  auto fixItsOutputPath = pathsFromArguments.fixItsOutputPath;

  // auto objcHeaderOutputPath = DetermineSupplementaryOutputFilename(
  //     OPT_emit_objc_header, pathsFromArguments.ObjCHeaderOutputPath,
  //     file::Type::ObjCHeader, "",
  //     defaultSupplementaryOutputPathExcludingExtension);

  // auto loadedModuleTracePath = DetermineSupplementaryOutputFilename(
  //     opts::EmitLoadedModuleTrace, pathsFromArguments.loadedModuleTracePath,
  //     file::Type::ModuleTrace, "",
  //     defaultSupplementaryOutputPathExcludingExtension);

  auto tbdPath = DetermineSupplementaryOutputFilename(
      opts::EmitTBD, pathsFromArguments.tbdPath, file::Type::TBD, "",
      defaultSupplementaryOutputPathExcludingExtension);

  auto moduleDocOutputPath = DetermineSupplementaryOutputFilename(
      opts::EmitModuleDoc, pathsFromArguments.moduleDocOutputPath,
      file::Type::StoneModuleDoc, "",
      defaultSupplementaryOutputPathExcludingExtension);

  auto moduleSourceInfoOutputPath = DetermineSupplementaryOutputFilename(
      opts::EmitModuleSourceInfo, pathsFromArguments.moduleSourceInfoOutputPath,
      file::Type::StoneSourceInfo, "",
      defaultSupplementaryOutputPathExcludingExtension);
  auto moduleSummaryOutputPath = DetermineSupplementaryOutputFilename(
      opts::EmitModuleSummary, pathsFromArguments.moduleSummaryOutputPath,
      file::Type::StoneModuleSummary, "",
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
      file::Type::StoneModule, mainOutputIfUsableForModule,
      defaultSupplementaryOutputPathExcludingExtension);

  // auto yamlOptRecordPath = DetermineSupplementaryOutputFilename(
  //     opts::SaveOptimizationRecordPath, pathsFromArguments.YAMLOptRecordPath,
  //     file::Type::yamlOptRecord, "",
  //     defaultSupplementaryOutputPathExcludingExtension);

  // auto bitstreamOptRecordPath = DetermineSupplementaryOutputFilename(
  //     opts::SaveOptimizationRecordPath,
  //     pathsFromArguments.BitstreamOptRecordPath,
  //     file::Type::BitstreamOptRecord,
  //     "", defaultSupplementaryOutputPathExcludingExtension);

  SupplementaryOutputPaths sop;
  // sop.ObjCHeaderOutputPath = objcHeaderOutputPath;
  sop.moduleOutputPath = moduleOutputPath;
  sop.moduleDocOutputPath = moduleDocOutputPath;
  sop.dependenciesFilePath = dependenciesFilePath;
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
    opts::OptID emitOpt, std::string pathFromArguments, file::Type type,
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
  llvm::sys::path::replace_extension(path, file::GetTypeExt(type));
  return path.str().str();
}

void SupplementaryOutputPathsComputer::DeriveModulePathParameters(
    StringRef mainOutputFile, opts::OptID &emitOption, std::string &extension,
    std::string &mainOutputIfUsable) const {

  emitOption = opts::EmitModule;

  bool canUseMainOutputForModule =
      action.GetKind() == ActionKind::MergeModules ||
      action.GetKind() == ActionKind::EmitModule;

  extension = file::GetTypeExt(file::Type::StoneModule).str();

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
  const std::pair<file::Type, std::string &> typesAndStrings[] = {
      {file::Type::StoneModule, paths.moduleOutputPath},
      // {file::Type::StoneModuleDoc, paths.ModuleDocOutputPath},
      // {file::Type::StoneSourceInfoFile, paths.ModuleSourceInfoOutputPath},
      // {file::Type::Dependencies, paths.dependenciesFilePath},
      // {file::Type::StoneDeps, paths.referenceDependenciesFilePath},
      // {file::Type::SerializeDiagnostics,
      // paths.SerializeDiagnosticsPath}, {file::Type::ModuleTrace,
      // paths.LoadedModuleTracePath}, {file::Type::TBD, paths.TBDPath},
      // {file::Type::StoneModuleInterfaceFile,
      //  paths.ModuleInterfaceOutputPath},
      // {file::Type::StoneModuleSummaryFile, paths.ModuleSummaryOutputPath},
      // {file::Type::PrivateStoneModuleInterfaceFile,
      //  paths.PrivateModuleInterfaceOutputPath},
      // {file::Type::YAMLOptRecord, paths.YAMLOptRecordPath},
      // {file::Type::BitstreamOptRecord, paths.BitstreamOptRecordPath},
      // {file::Type::StoneABIDescriptor, paths.ABIDescriptorOutputPath},
  };
  for (const std::pair<file::Type, std::string &> &typeAndString :
       typesAndStrings) {
    auto const out = map->find(typeAndString.first);
    typeAndString.second = out == map->end() ? "" : out->second;
  }
  return paths;
}

Optional<std::vector<SupplementaryOutputPaths>>
SupplementaryOutputPathsComputer::ReadSupplementaryOutputFileMap() const {

  // TODO:
  //  if (Arg *A = args.getLastArg(
  //        opts::emit_objc_header_path,
  //        opts::emit_module_path,
  //        opts::emit_module_doc_path,
  //        opts::emit_dependencies_path,
  //        opts::emit_reference_dependencies_path,
  //        opts::serialize_diagnostics_path,
  //        opts::emit_loaded_module_trace_path,
  //        opts::emit_module_interface_path,
  //        opts::emit_private_module_interface_path,
  //        opts::emit_module_source_info_path,
  //        opts::emit_tbd_path)) {
  //    de.PrintD(SrcLoc(),
  //                   diag::error_cannot_have_supplementary_outputs,
  //                   A->getSpelling(), "-supplementary-output-file-map");
  //    return llvm::None;
  //  }

  const StringRef supplementaryFileMapPath =
      args.getLastArgValue(opts::SupplementaryOutputFileMap);

  unsigned BadFileDescriptorRetryCount = 0;
  if (const Arg *A = args.getLastArg(opts::BadFileDescriptorRetryCount)) {
    if (StringRef(A->getValue())
            .getAsInteger(10, BadFileDescriptorRetryCount)) {
      de.PrintD(SrcLoc(), diag::err_invalid_arg_value,
                diag::LLVMStr(A->getAsString(args)),
                diag::LLVMStr(A->getValue()));
      return llvm::None;
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
    de.PrintD(SrcLoc(), diag::err_cannot_open_file,
              diag::LLVMStr(supplementaryFileMapPath),
              diag::LLVMStr(buffer.getError().message()));
    return llvm::None;
  }
  llvm::Expected<OutputFileMap> outputFileMap =
      OutputFileMap::LoadFromBuffer(std::move(buffer.get()), "");
  if (auto Err = outputFileMap.takeError()) {
    de.PrintD(SrcLoc(), diag::err_unable_to_load_supplementary_output_file_map,
              diag::LLVMStr(supplementaryFileMapPath),
              diag::LLVMStr(llvm::toString(std::move(Err))));
    return llvm::None;
  }

  std::vector<SupplementaryOutputPaths> outputPaths;
  bool hadError = false;
  inputsAndOutputs.ForEachInputProducingSupplementaryOutput(
      [&](const CompilerInputFile &input) -> bool {
        const TypeToPathMap *mapForInput =
            outputFileMap->GetOutputMapForInput(input.GetFileName());
        if (!mapForInput) {
          de.PrintD(SrcLoc(),
                    diag::err_missing_entry_in_supplementary_output_file_map,
                    diag::LLVMStr(supplementaryFileMapPath),
                    diag::LLVMStr(input.GetFileName()));
          hadError = true;
        }
        outputPaths.push_back(CreateFromTypeToPathMap(mapForInput));
        return false;
      });
  if (hadError) {
    return llvm::None;
  }
  return outputPaths;
}
