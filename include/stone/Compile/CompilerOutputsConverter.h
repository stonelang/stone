#ifndef STONE_COMPILE_COMPILEROUTPUTSCONVERTER_H
#define STONE_COMPILE_COMPILEROUTPUTSCONVERTER_H

#include "stone/AST/Diagnostics.h"
#include "stone/Basic/LLVM.h"
#include "stone/Basic/SupplementaryOutputPaths.h"
#include "stone/Compile/CompilerInputsAndOutputs.h"
#include "stone/Compile/CompilerOptions.h"
#include "stone/Support/Options.h"
#include "llvm/Option/ArgList.h"

#include <vector>

namespace stone {

class OutputFileMap;
/// Given the command line arguments and information about the inputs,
/// Fill in all the information in CompilerInputsAndOutputs.

class CompilerOutputsConverter {
  const llvm::opt::ArgList &args;
  llvm::StringRef moduleName;
  CompilerInputsAndOutputs &inputsAndOutputs;
  DiagnosticEngine &de;

public:
  CompilerOutputsConverter(const llvm::opt::ArgList &args, StringRef moduleName,
                           CompilerInputsAndOutputs &inputsAndOutputs,
                           DiagnosticEngine &de)
      : args(args), moduleName(moduleName), inputsAndOutputs(inputsAndOutputs),
        de(de) {}

  bool Convert(std::vector<std::string> &mainOutputs,
               std::vector<std::string> &mainOutputsForIndexUnits,
               std::vector<SupplementaryOutputPaths> &supplementaryOutputs,
               CompilerActionKind actionKind);

  /// Try to read an output file list file.
  /// \returns `None` if it could not open the filelist.
  static std::optional<std::vector<std::string>>
  ReadOutputFileList(StringRef filelistPath, DiagnosticEngine &de);
};

struct CompilerOutputOptInfo {
  StringRef PrettyName;
  opts::OptID SingleID;
  opts::OptID FilelistID;
  StringRef SingleOptSpelling;
};

class CompilerOutputFilesComputer {
  DiagnosticEngine &de;
  const CompilerInputsAndOutputs &inputsAndOutputs;
  const std::vector<std::string> OutputFileArguments;
  const std::string OutputDirectoryArgument;
  const std::string FirstInput;
  CompilerActionKind actionKind;
  const llvm::opt::Arg *const moduleNameArg;
  const StringRef Suffix;
  const bool HasTextualOutput;
  const CompilerOutputOptInfo OutputInfo;

  CompilerOutputFilesComputer(
      DiagnosticEngine &de, const CompilerInputsAndOutputs &inputsAndOutputs,
      std::vector<std::string> outputFileArguments,
      StringRef outputDirectoryArgument, StringRef firstInput,
      CompilerActionKind actionKind, const llvm::opt::Arg *moduleNameArg,
      StringRef suffix, bool hasTextualOutput, CompilerOutputOptInfo optInfo);

public:
  static std::optional<CompilerOutputFilesComputer>
  Create(const llvm::opt::ArgList &args, DiagnosticEngine &de,
         const CompilerInputsAndOutputs &inputsAndOutputs,
         CompilerOutputOptInfo optInfo, CompilerActionKind actionKind);

  /// \return the output filenames on the command line or in the output
  /// filelist. If there
  /// were neither -o's nor an output filelist, returns an empty vector.
  static std::optional<std::vector<std::string>>
  GetOutputFilenamesFromCommandLineOrFileList(const llvm::opt::ArgList &args,
                                              DiagnosticEngine &de,
                                              opts::OptID singleOpt,
                                              opts::OptID filelistOpt);

  std::optional<std::vector<std::string>> ComputeOutputFiles() const;

private:
  std::optional<std::string>
  ComputeOutputFile(StringRef outputArg, const CompilerInputFile &input) const;

  /// \return the correct output filename when none was specified.
  ///
  /// Such an absence should only occur when invoking the invocation
  /// without the driver,
  /// because the driver will always pass -o with an appropriate filename
  /// if output is required for the requested action.
  std::optional<std::string>
  DeriveOutputFileFromInput(const CompilerInputFile &input) const;

  /// \return the correct output filename when a directory was specified.
  ///
  /// Such a specification should only occur when invoking the invocation
  /// directly, because the driver will always pass -o with an appropriate
  /// filename if output is required for the requested action.
  std::optional<std::string>
  DeriveOutputFileForDirectory(const CompilerInputFile &input) const;

  std::string DetermineBaseNameOfOutput(const CompilerInputFile &input) const;

  std::string DeriveOutputFileFromParts(StringRef dir, StringRef base) const;
};

class SupplementaryOutputPathsComputer {
  const llvm::opt::ArgList &args;
  DiagnosticEngine &de;
  const CompilerInputsAndOutputs &inputsAndOutputs;
  ArrayRef<std::string> OutputFiles;
  StringRef moduleName;
  CompilerActionKind actionKind;

public:
  SupplementaryOutputPathsComputer(
      const llvm::opt::ArgList &args, DiagnosticEngine &de,
      const CompilerInputsAndOutputs &inputsAndOutputs,
      ArrayRef<std::string> outputFiles, StringRef moduleName,
      CompilerActionKind actionKind);

  std::optional<std::vector<SupplementaryOutputPaths>>
  ComputeOutputPaths() const;

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
  std::optional<std::vector<SupplementaryOutputPaths>>
  GetSupplementaryOutputPathsFromArguments() const;

  /// Read a supplementary output file map file.
  /// \returns `None` if it could not open the file map.
  std::optional<std::vector<SupplementaryOutputPaths>>
  ReadSupplementaryOutputFileMap() const;

  /// Given an ID corresponding to supplementary output argument
  /// (e.g. -emit-module-path), collect all such paths, and ensure
  /// there are the right number of them.
  std::optional<std::vector<std::string>>
  GetSupplementaryFilenamesFromArguments(opts::OptID pathID) const;

  std::optional<SupplementaryOutputPaths> ComputeOutputPathsForOneInput(
      StringRef outputFilename,
      const SupplementaryOutputPaths &pathsFromFilelists,
      const CompilerInputFile &) const;

  llvm::StringRef DeriveDefaultSupplementaryOutputPathExcludingExtension(
      StringRef outputFilename, const CompilerInputFile &) const;

  /// \return empty string if no output file.
  std::string DetermineSupplementaryOutputFilename(
      opts::OptID emitOpt, std::string pathFromArgumentsOrFilelists,
      stone::FileType type, StringRef mainOutputIfUsable,
      StringRef defaultSupplementaryOutputPathExcludingExtension) const;

  void DeriveModulePathParameters(StringRef mainOutputFile,
                                  opts::OptID &emitOption,
                                  std::string &extension,
                                  std::string &mainOutputIfUsable) const;
};

} // namespace stone

#endif
