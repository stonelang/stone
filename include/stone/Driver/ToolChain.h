#ifndef STONE_DRIVER_TOOLCHAIN_H
#define STONE_DRIVER_TOOLCHAIN_H

#include <cassert>
#include <climits>
#include <memory>
#include <string>
#include <utility>

#include "stone/Core/File.h"
#include "stone/Core/LLVM.h"
#include "stone/Core/List.h"
#include "stone/Core/Mem.h"
#include "stone/Driver/DriverOptions.h"
#include "stone/Driver/Job.h"
#include "stone/Driver/Request.h"
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

/// The tool types that are supported
enum class ToolKind { None, Assemble, Clang, GCC, LD, LLD, SC };

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
/// A map for caching Jobs for a given Action/ToolChain pair
// using ToolGroupCache = llvm::DenseMap<ToolKind, std::unique_ptr<ToolGroup>>;
enum class ToolChainKind { None, Darwin, Unix, Win };

class ToolChain {
  ToolChainKind kind;

protected:
  Driver &driver;
  const llvm::Triple triple;

  // All the jobs the tool chain created --- lifetime management.
  llvm::SmallVector<std::unique_ptr<const Job>, 32> jobs;

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

  const Driver &GetDriver() { return driver; }
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

protected:
  // TODO: Now that you are using the construction method, you may not need
  // this.
  virtual std::unique_ptr<Tool> BuildSCTool();
  virtual std::unique_ptr<Tool> BuildLDTool() = 0;
  virtual std::unique_ptr<Tool> BuildLLDTool() = 0;
  virtual std::unique_ptr<Tool> BuildClangTool() = 0;
  virtual std::unique_ptr<Tool> BuildGCCTool() = 0;

protected:
  std::unique_ptr<Tool> BuildTool(ToolKind kind, const char *fullName,
                                  const char *shortName, bool isDefault);

public:
  virtual bool Initialize();
  Tool *FindTool(ToolKind tk) const;

public:
  virtual Job *ConstructCompileJob(Compilation &compilation,
                                   const file::File &input,
                                   const OutputOptions &outputOpts);

  // TODO: You only need ConstructLinkJob
  virtual Job *ConstructStaticLinkJob(job::InputList inputs,
                                      const OutputOptions &outputOpts) = 0;

  virtual Job *ConstructExecLinkJob(job::InputList inputs,
                                    const OutputOptions &outputOpts) = 0;

  virtual Job *ConstructDynamicLinkJob(job::InputList inputs,
                                       const OutputOptions &outputOpts) = 0;

public:
  // virtual TaskDetail ConstructTaskDetail(const CompileJob &job,
  //                                        Compilation &compilation);
  // virtual TaskDetail ConstructTaskDetail(const StaticLinkJob &job,
  //                                        Compilation &compilation);
  // virtual TaskDetail ConstructTaskDetail(const DynamicLinkJob &job,
  //                                        Compilation &compilation);
  // virtual TaskDetail ConstructTaskDetail(const ExecutableLinkJob &job,
  //                                        Compilation &compilation);

protected:
  template <typename JobTy, typename... Args> JobTy *MakeJob(Args &&...args) {
    auto job = new JobTy(std::forward<Args>(args)...);
    jobs.emplace_back(job);
    return job;
  }
};

} // namespace stone
#endif
