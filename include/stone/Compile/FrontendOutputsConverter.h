#ifndef STONE_COMPILE_LANGOUTPUTSCONVERTER_H
#define STONE_COMPILE_LANGOUTPUTSCONVERTER_H

#include "stone/Diag/DiagnosticEngine.h"
#include "stone/Diag/DiagnosticListener.h"
#include "stone/Basic/LLVM.h"
#include "stone/Compile/FrontendOptions.h"
#include "stone/Compile/SupplementaryOutputPaths.h"

#include "stone/Session/Mode.h"
#include "stone/Session/Options.h"
#include "llvm/Option/ArgList.h"

#include <vector>

namespace stone {

class Mode;
class OutputFileMap;
/// Given the command line arguments and information about the inputs,
/// Fill in all the information in FrontendInputsAndOutputs.

class FrontendOutputsConverter {
  const llvm::opt::ArgList &args;
  llvm::StringRef moduleName;
  FrontendInputsAndOutputs &inputsAndOutputs;
  DiagnosticEngine &de;

public:
  FrontendOutputsConverter(const llvm::opt::ArgList &args, StringRef moduleName,
                           FrontendInputsAndOutputs &inputsAndOutputs,
                           DiagnosticEngine &de)
      : args(args), moduleName(moduleName), inputsAndOutputs(inputsAndOutputs),
        de(de) {}

  bool Convert(std::vector<std::string> &mainOutputs,
               std::vector<std::string> &mainOutputsForIndexUnits,
               std::vector<SupplementaryOutputPaths> &supplementaryOutputs,
               const Mode &mode);

  /// Try to read an output file list file.
  /// \returns `None` if it could not open the filelist.
  static Optional<std::vector<std::string>>
  ReadOutputFileList(StringRef filelistPath, DiagnosticEngine &de);
};

struct FrontendOutputOptInfo {
  StringRef PrettyName;
  opts::OptID SingleID;
  opts::OptID FilelistID;
  StringRef SingleOptSpelling;
};

class FrontendOutputFilesComputer {
  DiagnosticEngine &de;
  const FrontendInputsAndOutputs &inputsAndOutputs;
  const std::vector<std::string> OutputFileArguments;
  const std::string OutputDirectoryArgument;
  const std::string FirstInput;
  const Mode &mode;
  const llvm::opt::Arg *const moduleNameArg;
  const StringRef Suffix;
  const bool HasTextualOutput;
  const FrontendOutputOptInfo OutputInfo;

  FrontendOutputFilesComputer(DiagnosticEngine &de,
                              const FrontendInputsAndOutputs &inputsAndOutputs,
                              std::vector<std::string> outputFileArguments,
                              StringRef outputDirectoryArgument,
                              StringRef firstInput, const Mode &mode,
                              const llvm::opt::Arg *moduleNameArg,
                              StringRef suffix, bool hasTextualOutput,
                              FrontendOutputOptInfo optInfo);

public:
  static Optional<FrontendOutputFilesComputer>
  Create(const llvm::opt::ArgList &args, DiagnosticEngine &de,
         const FrontendInputsAndOutputs &inputsAndOutputs,
         FrontendOutputOptInfo optInfo, const Mode &mode);

  /// \return the output filenames on the command line or in the output
  /// filelist. If there
  /// were neither -o's nor an output filelist, returns an empty vector.
  static Optional<std::vector<std::string>>
  GetOutputFilenamesFromCommandLineOrFileList(const llvm::opt::ArgList &args,
                                              DiagnosticEngine &de,
                                              opts::OptID singleOpt,
                                              opts::OptID filelistOpt);

  Optional<std::vector<std::string>> ComputeOutputFiles() const;

private:
  Optional<std::string> ComputeOutputFile(StringRef outputArg,
                                          const FrontendInputFile &input) const;

  /// \return the correct output filename when none was specified.
  ///
  /// Such an absence should only occur when invoking the frontend
  /// without the driver,
  /// because the driver will always pass -o with an appropriate filename
  /// if output is required for the requested action.
  Optional<std::string>
  DeriveOutputFileFromInput(const FrontendInputFile &input) const;

  /// \return the correct output filename when a directory was specified.
  ///
  /// Such a specification should only occur when invoking the frontend
  /// directly, because the driver will always pass -o with an appropriate
  /// filename if output is required for the requested action.
  Optional<std::string>
  DeriveOutputFileForDirectory(const FrontendInputFile &input) const;

  std::string DetermineBaseNameOfOutput(const FrontendInputFile &input) const;

  std::string DeriveOutputFileFromParts(StringRef dir, StringRef base) const;
};

class SupplementaryOutputPathsComputer {
  const llvm::opt::ArgList &args;
  DiagnosticEngine &de;
  const FrontendInputsAndOutputs &inputsAndOutputs;
  ArrayRef<std::string> OutputFiles;
  StringRef moduleName;
  const Mode &mode;

public:
  SupplementaryOutputPathsComputer(
      const llvm::opt::ArgList &args, DiagnosticEngine &de,
      const FrontendInputsAndOutputs &inputsAndOutputs,
      ArrayRef<std::string> outputFiles, StringRef moduleName,
      const Mode &mode);

  Optional<std::vector<SupplementaryOutputPaths>> ComputeOutputPaths() const;

private:
  /// \Return a set of supplementary output paths for each input that might
  /// produce supplementary outputs, or None to signal an error.
  /// \note
  /// Batch-mode supports multiple primary inputs.
  /// \par
  /// The paths are derived from arguments
  /// such as -emit-module-path. These are not the final computed paths,
  /// merely the ones passed in via the command line.
  /// \note
  /// In the future, these will also include those passed in via whatever
  /// filelist scheme gets implemented to handle cases where the command line
  /// arguments become burdensome.
  Optional<std::vector<SupplementaryOutputPaths>>
  GetSupplementaryOutputPathsFromArguments() const;

  /// Read a supplementary output file map file.
  /// \returns `None` if it could not open the file map.
  Optional<std::vector<SupplementaryOutputPaths>>
  ReadSupplementaryOutputFileMap() const;

  /// Given an ID corresponding to supplementary output argument
  /// (e.g. -emit-module-path), collect all such paths, and ensure
  /// there are the right number of them.
  Optional<std::vector<std::string>>
  GetSupplementaryFilenamesFromArguments(opts::OptID pathID) const;

  llvm::Optional<SupplementaryOutputPaths> ComputeOutputPathsForOneInput(
      StringRef outputFilename,
      const SupplementaryOutputPaths &pathsFromFilelists,
      const FrontendInputFile &) const;

  llvm::StringRef DeriveDefaultSupplementaryOutputPathExcludingExtension(
      StringRef outputFilename, const FrontendInputFile &) const;

  /// \return empty string if no output file.
  std::string DetermineSupplementaryOutputFilename(
      opts::OptID emitOpt, std::string pathFromArgumentsOrFilelists,
      file::Type type, StringRef mainOutputIfUsable,
      StringRef defaultSupplementaryOutputPathExcludingExtension) const;

  void DeriveModulePathParameters(StringRef mainOutputFile,
                                  opts::OptID &emitOption,
                                  std::string &extension,
                                  std::string &mainOutputIfUsable) const;
};

} // namespace stone

#endif
