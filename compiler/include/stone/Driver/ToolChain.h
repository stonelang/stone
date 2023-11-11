#ifndef STONE_DRIVER_TOOLCHAIN_H
#define STONE_DRIVER_TOOLCHAIN_H

#include <cassert>
#include <climits>
#include <memory>
#include <string>
#include <utility>

#include "stone/Basic/File.h"
#include "stone/Basic/LLVM.h"
#include "stone/Basic/List.h"
#include "stone/Basic/Mem.h"
#include "stone/Driver/DriverOptions.h"
#include "stone/Driver/Job.h"
#include "stone/Driver/Phase.h"

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/Triple.h"
#include "llvm/MC/MCTargetOptions.h"
#include "llvm/Option/Option.h"
#include "llvm/Support/VersionTuple.h"
#include "llvm/Target/TargetOptions.h"

namespace llvm {
namespace opt {
class Arg;
class ArgList;
class DerivedArgList;

} // namespace opt
namespace vfs {
class FileSystem;
} // namespace vfs
} // namespace llvm

namespace stone {

class ToolChain;
class Driver;
class Compilation;

struct ToolName final {
  constexpr static const char *SC = "stone-compile";
  constexpr static const char *LD = "ld";
  constexpr static const char *LLD = "lld";
  constexpr static const char *Clang = "clang++";
  constexpr static const char *GCC = "g++";
  constexpr static const char *Git = "git";
};

/// The tool types that are supported
enum class ToolKind {
  None,
  Assemble,
  Clang,
  GCC,
  Git,
  LD,
  LLD,
  SC,
};

class Tool final {
  ToolKind kind;
  /// The tool name (for debugging).
  const char *fullName;
  /// The human readable name for the tool, for use in diagnostics.
  const char *shortName;
  /// The tool chain this tool is a part of.
  const ToolChain &toolChain;

  bool isDefault = false;
  bool isObsolete = false;

public:
  Tool(ToolKind kind, const char *fullName, const char *shortName,
       const ToolChain &toolChain, bool isDefault = false)
      : kind(kind), fullName(fullName), shortName(shortName),
        toolChain(toolChain), isDefault(isDefault) {}

public:
  bool OnSystem() { return file::Exists(fullName); }
  bool CanExec() { return file::CanExec(fullName); }
  bool IsObsolete() { return isObsolete; }

  const char *GetFullName() const { return fullName; }
  const char *GetShortName() const { return shortName; }
  ToolKind GetKind() { return kind; }
  bool IsDefault() { return isDefault; }
};

class OutputOptions;
/// A map for caching Jobs for a given Phase/ToolChain pair
// using ToolGroupCache = llvm::DenseMap<ToolKind, std::unique_ptr<ToolGroup>>;
enum class ToolChainKind { None, Darwin, Unix, Win };

class ToolChain {
  ToolChainKind kind;

protected:
  Driver &driver;
  const llvm::Triple triple;

public:
  using Paths = llvm::SmallVector<std::string, 16>;
  using Tools = llvm::SmallVector<Tool, 16>;

  // using ToolPaths = llvm::SmallVector<std::pair<llvm::StringRef,
  // llvm::StringRef, 16>
protected:
  ToolChain(ToolChainKind kind, Driver &driver, const llvm::Triple &triple);

protected:
  /// The list of toolchain specific path prefixes to search for libraries.
  Paths libPaths;

  /// The list of toolchain specific path prefixes to search for files.
  Paths filePaths;

  /// The list of toolchain specific path prefixes to search for programs.
  Paths programPaths;

  // Tne compilaiton tools for this tool chain
  // Tools tools;
  SafeList<Tool> tools;

public:
  virtual ~ToolChain() = default;

protected:
  // Build tools and add to the tools
  virtual std::unique_ptr<Tool> BuildSC();
  virtual std::unique_ptr<Tool> BuildLD() = 0;
  virtual std::unique_ptr<Tool> BuildLLD() = 0;
  virtual std::unique_ptr<Tool> BuildClang() = 0;
  virtual std::unique_ptr<Tool> BuildGCC() = 0;
  virtual std::unique_ptr<Tool> BuildGit() = 0;

  std::unique_ptr<Tool> BuildTool(ToolKind kind, const char *fullName,
                                  const char *shortName, bool isDefault);
  Tool *FindTool(ToolKind tk) const;

public:
  virtual void Initialize();

  Tool *GetSC() { return FindTool(ToolKind::SC); }
  Tool *GetLD() { return FindTool(ToolKind::LD); }
  Tool *GetLLD() { return FindTool(ToolKind::LLD); }
  Tool *GetClang() { return FindTool(ToolKind::Clang); }
  Tool *GetGCC() { return FindTool(ToolKind::GCC); }
  Tool *GetGit() { return FindTool(ToolKind::Git); }

public:
  Driver &GetDriver() { return driver; }
  const llvm::Triple &GetTriple() const { return triple; }

  Paths &GetLibraryPaths() { return libPaths; }
  const Paths &GetLibraryPaths() const { return libPaths; }

  Paths &GetFilePaths() { return filePaths; }
  const Paths &GetFilePaths() const { return filePaths; }

  Paths &GetProgramPaths() { return programPaths; }
  const Paths &GetProgramPaths() const { return programPaths; }

  llvm::Triple::ArchType GetArchType() { return triple.getArch(); }
  const llvm::Triple::ArchType GetArchType() const { return triple.getArch(); }

  ToolChainKind GetKind() { return kind; }

  /// Searches for the given executable in appropriate paths relative to the
  /// Stone binary.
  ///
  /// This method caches its results.
  ///
  /// \sa PerformFindProgramRelativeToStone
  std::string FindProgramRelativeToStone(llvm::StringRef executableName);

  /// An override point for platform-specific subclasses to customize how to
  /// do relative searches for programs.
  ///
  /// This method is invoked by FindProgramRelativeToStone().
  virtual std::string
  FindProgramRelativeToStoneImpl(llvm::StringRef executableName);

protected:
  virtual JobDetail ConstructDetail(const CompilePhase &action);
  virtual JobDetail ConstructDetail(const DynamicLinkPhase &action) = 0;
  virtual JobDetail ConstructDetail(const StaticLinkPhase &action) = 0;
  virtual JobDetail ConstructDetail(const ExecutableLinkPhase &action) = 0;

public:
  std::unique_ptr<Job> ConstructJob(const Phase &action,
                                    Compilation &compilation,
                                    std::unique_ptr<CommandOutput> output,
                                    const OutputOptions &outputOptions);
};

} // namespace stone
#endif
