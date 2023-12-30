#ifndef STONE_DRIVER_DRIVER_OPTIONS_H
#define STONE_DRIVER_DRIVER_OPTIONS_H

#include "stone/Basic/STDAlias.h"
#include "stone/Option/Action.h"
#include "stone/Option/Options.h"

using namespace llvm::opt;

namespace stone {

enum class ToolChainKind {
  None = 0,
  /// Darwin tool-chain
  Darwin,
  /// Linux tool-chain
  Linux,
  /// Windows tool-chain
  Windows,
  /// FreeBSD tool-chain
  FreeBSD,
  /// OpenBSD tool-chain
  OpenBSD,
  /// Android tool-chain
  Android,
  /// Any unix tool-chain
  Unix
};

using ToolChainOSType = llvm::Triple::OSType;

class DriverOptions final {
public:
  /// The main action requested or computed.
  Action mainAction;

  /// Default target triple.
  std::string defaultTargetTriple;

  /// Default target triple.
  llvm::Optional<llvm::Triple> targetVariant;

  /// The path the executing program
  llvm::StringRef mainExecutablePath;

  /// The name of the executing program
  llvm::StringRef mainExecutableName;

   /// Indicates whether the driver should check that the input file exist.
  bool checkInputFileExistence = false;


  /// Extra args to pass to the driver executable
  llvm::SmallVector<std::string, 2> extraMainExecutableArgs;

  /// The file input type
  file::Type inputFileType = file::Type::None;

  InputFileList inputFiles;

  std::string toolsDirectory;

  ToolChainKind toolChainKind = ToolChainKind::None;

  ToolChainOSType toolChainOSType = ToolChainOSType::UnknownOS;

public:
  DriverOptions();
};

} // namespace stone

#endif