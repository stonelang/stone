#ifndef STONE_OPTION_OPTIONS_H
#define STONE_OPTION_OPTIONS_H

#include "stone/Basic/ColorStream.h"
#include "stone/Basic/FileType.h"
#include "stone/Support/ActionKind.h"
#include "stone/Support/Mode.h"

#include "llvm/Option/Arg.h"
#include "llvm/Option/ArgList.h"
#include "llvm/Option/Option.h"

#include <memory>

namespace llvm {
namespace opt {
class OptTable;
}
} // namespace llvm

namespace stone {

namespace opts {
enum OptFlag {
  CompilerOption = (1 << 4),
  NoCompilerOption = (1 << 5),
  DriverOption = (1 << 6),
  NoDriverOption = (1 << 7),
  DebugOption = (1 << 8),
  ArgumentIsPath = (1 << 9),
  ModuleInterfaceOption = (1 << 10),
  SupplementaryOutput = (1 << 11),
  StoneAPIExtractOption = (1 << 12),
  StoneSymbolGraphExtractOption = (1 << 13),
  StoneAPIDigesterOption = (1 << 14),
  NoBatchOption = (1 << 15),
  DoesNotAffectIncrementalBuild = (1 << 16),
  NoInteractiveOption = (1 << 17),
  CacheInvariant = (1 << 18),

};

enum OptID {
  OPT_INVALID = 0, // This is not an option ID.
#define OPTION(...) LLVM_MAKE_OPT_ID(__VA_ARGS__),
#include "stone/Support/Options.inc"
  OPT_LAST
#undef OPTION
};

ActionKind GetActionKindByOptID(const unsigned optID);
llvm::StringRef GetEqualValueByOptID(const opts::OptID optID,
                                     const llvm::opt::InputArgList &args);
unsigned GetArgID(const llvm::opt::Arg *arg);
llvm::StringRef GetArgName(const llvm::opt::Arg *arg);

void PrintArg(ColorStream &outStream, const char *arg, llvm::StringRef tempDir);

} // namespace opts

class Options {
  /// Stone options
  std::unique_ptr<llvm::opt::OptTable> optTable;

protected:
  /// \The path the executing program
  llvm::StringRef mainExecutablePath;

  /// \The name of the executing program
  llvm::StringRef mainExecutableName;

public:
  Options();

public:
  /// The main options table
  const llvm::opt::OptTable &GetOptTable() const { return *optTable; }

  /// \return the main executable path
  llvm::StringRef GetMainExecutablePath() const { return mainExecutablePath; }

  /// \check that there exist the main executable path
  bool HasMainExecutablePath() const {
    return !mainExecutablePath.empty() && mainExecutablePath.size() > 0;
  }
  /// \return the main executable name
  llvm::StringRef GetMainExecutableName() const { return mainExecutableName; }

  /// \check that there exist the main executable path
  bool HasMainExecutableName() const {
    return !mainExecutableName.empty() && mainExecutableName.size() > 0;
  }

public:
  /// \The primary action arg that starts a compilation
  llvm::opt::Arg *primaryActionArg = nullptr;

  /// \The primary action that starts a compilation
  // PrimaryAction primaryAction = PrimaryAction::None;

  /// \return true if this is the None action
  // bool IsNoneAction() const;

  // /// \return true if this is the PrintHelp action
  // bool IsPrintHelpAction() const;

  // /// \return true if this is the PrintHelpHidden action.
  // bool IsPrintHelpHiddenAction() const;

  // /// \return true if this is the PrintVersion action
  // bool IsPrintVersionAction() const;

  // /// \return true if this is the PrintFeature action
  // bool IsPrintFeatureAction() const;

  // /// \return true if this is the Parse action
  // bool IsParseAction() const;

  // /// \return true if this is the ResolveImports action
  // bool IsResolveImportsAction() const;

  // /// \return true if this is the PrintASTBefore action
  // bool IsPrintASTBeforeAction() const;

  // /// \return true if this is the TypeCheck action
  // bool IsTypeCheckAction() const;

  // /// \return true if this is the PrintASTAfter action
  // bool IsPrintASTAfterAction() const;

  // /// \return true if this is the EmitIRAfter action
  // bool IsEmitIRAfterAction() const;

  // /// \return true if this is the EmitIRBefore action
  // bool IsEmitIRBeforeAction() const;

  // /// \return true if this is the EmitModule action
  // bool IsEmitModuleAction() const;

  // /// \return true if this is the EmitBC action
  // bool IsEmitBCAction() const;

  // /// \return true if this is the EmitObject action
  // bool IsEmitObjectAction() const;

  // /// \return true if this is the EmitAssembly action
  // bool IsEmitAssemblyAction() const;

  // /// \return true if the given action requires input files to be provided.
  // bool DoesActionRequireInputs() const {
  //   return DoesActionRequireInputs(MainAction);
  // }

  // /// \return true if the given action produces output
  // bool DoesActionProduceOutputFile() const {
  //   return DoesActionProduceOutputFile(MainAction);
  // }
  // /// \return the FileType for the action
  // file::FileType GetActionInputFileType() const {
  //   return GetActionInputFileType(MainAction);
  // }
  // /// \return the FileType for the action
  // file::FileType GetActionOutputFileType() const {
  //   return GetActionOutputFileType(MainAction);
  // }
  // /// \return the string name of the action
  // llvm::StringRef GetActionString() const {
  //   return GetActionString(MainAction);
  // }

  //   bool Is(PrimaryAction k) const { return kind == k; }
  //   bool IsAny(PrimaryAction K1) const { return Is(K1); }
  //   template <typename... T>
  //   bool IsAny(PrimaryAction K1, PrimaryAction K2, T... K) const {
  //     if (Is(K1)) {
  //       return true;
  //     }
  //     return IsAny(K2, K...);
  //   }
  //   template <typename... T> bool IsNot(PrimaryAction K1, T... K) const {
  //     return !IsAny(K1, K...);
  //   }

  // public:
  //   /// \return true if the given action requires input files to be provided.
  //   static bool DoesActionRequireInputs(PrimaryAction action);
  //   /// \return true if the given action produces output
  //   static bool DoesActionProduceOutputFile(PrimaryAction action);
  //   /// \return the FileType for the action
  //   static file::FileType GetActionInputFileType(PrimaryAction action);
  //   /// \return the FileType for the action
  //   static file::FileType GetActionOutputFileType(PrimaryAction action);
  //   /// \return the string name of the action
  //   static llvm::StringRef GetActionString(PrimaryAction action);
  //   /// \return the string name of the action
  //   static llvm::StringRef GetActionString(const llvm::opt::Arg *arg) const;
  // }
};

} // namespace stone

#endif
