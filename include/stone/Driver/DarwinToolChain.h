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
namespace darwin {
class DarwinToolChain final : public ToolChain {
  const llvm::Optional<llvm::Triple> &targetVariant;

public:
  DarwinToolChain(Driver &driver, const llvm::Triple &triple,
                  const llvm::Optional<llvm::Triple> &targetVariant);
  ~DarwinToolChain() = default;

public:
  bool Initialize() override;

public:
  /// Consruct the compile job
  Job *ConstructCompileJob(const file::File &input,
                           const OutputOptions &outputOpts);

  /// Construct the static-link job
  Job *ConstructStaticLinkJob(job::InputList inputs,
                              const OutputOptions &outputOpts);

  Job *ConstructDynamicLinkJob(job::InputList inputs,
                               const OutputOptions &outputOpts) override;

  Job *ConstructExecLinkJob(job::InputList inputs,
                            const OutputOptions &outputOpts) override;

public:
  std::unique_ptr<TaskDetail>
  ConstructTaskDetail(const CompileJob &job) override;

  std::unique_ptr<TaskDetail>
  ConstructTaskDetail(const DynamicLinkJob &job) override;

  std::unique_ptr<TaskDetail>
  ConstructTaskDetail(const StaticLinkJob &job) override;

  std::unique_ptr<TaskDetail>
  ConstructTaskDetail(const ExecutableLinkJob &job) override;

protected:
  std::unique_ptr<Tool> BuildSC() override;
  std::unique_ptr<Tool> BuildLD() override;
  std::unique_ptr<Tool> BuildLLD() override;
  std::unique_ptr<Tool> BuildClang() override;
  std::unique_ptr<Tool> BuildGCC() override;
};
} // namespace darwin
} // namespace stone
#endif
