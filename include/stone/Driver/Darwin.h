#ifndef STONE_DRIVER_DARWIN_H
#define STONE_DRIVER_DARWIN_H

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
#include "stone/Driver/ToolChain.h"

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

class DarwinToolChain final : public ToolChain {
  const llvm::Optional<llvm::Triple> &targetVariant;

public:
  DarwinToolChain(const Driver &driver, const llvm::Triple &triple,
                  const llvm::Optional<llvm::Triple> &targetVariant);
  ~DarwinToolChain() = default;

public:
  bool Initialize() override;

public:
  // JobInvocation
  // ConstructInvocation(const CompileJobRequest &request) const override;

  // JobInvocation
  // ConstructInvocation(const LinkJobRequest &request) const override;

  // CommandInvocation
  // ConstructInvocation(const StaticLinkIntent &intent) const override;

  // CommandInvocation
  // ConstructInvocation(const DynamicLinkIntent &intent) const override;

  // Command* ConstructCommand(const CompileJob& job) const override;

  std::unique_ptr<Job> ConstructCompileJob(const Tool &tool,
                                           const file::File &input);
  std::unique_ptr<Job> ConstructStaticLinkJob(const Tool &tool,
                                              InputList inputs,
                                              file::Type outputFileType);

  std::unique_ptr<Job> ConstructStaticLinkJob(const Tool &tool, DepList deps,
                                              file::Type outputFileType);

  std::unique_ptr<Job> ConstructDynamicLinkJob(const Tool &tool,
                                               InputList inputs,
                                               file::Type outputFileType,
                                               bool withLTO) override;
  std::unique_ptr<Job> ConstructDynamicLinkJob(const Tool &tool, DepList deps,
                                               file::Type outputFileType,
                                               bool withLTO) override;

  std::unique_ptr<Job> ConstructExecLinkJob(const Tool &tool, InputList inputs,
                                            file::Type outputFileType) override;
  std::unique_ptr<Job> ConstructExecLinkJob(const Tool &tool, DepList deps,
                                            file::Type outputFileType) override;

protected:
  std::unique_ptr<Tool> BuildSCTool() override;
  std::unique_ptr<Tool> BuildLDTool() override;
  std::unique_ptr<Tool> BuildLLDTool() override;
  std::unique_ptr<Tool> BuildClangTool() override;
  std::unique_ptr<Tool> BuildGCCTool() override;
};

} // namespace stone
#endif
