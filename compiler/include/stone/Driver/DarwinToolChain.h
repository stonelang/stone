#ifndef STONE_DRIVER_DARWIN_H
#define STONE_DRIVER_DARWIN_H

#include <cassert>
#include <climits>
#include <memory>
#include <string>
#include <utility>

#include "stone/Basic/File.h"
#include "stone/Basic/LLVM.h"
#include "stone/Basic/List.h"
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
  void Initialize() override;

protected:
  std::unique_ptr<Tool> BuildSC() override;
  std::unique_ptr<Tool> BuildLD() override;
  std::unique_ptr<Tool> BuildLLD() override;
  std::unique_ptr<Tool> BuildClang() override;
  std::unique_ptr<Tool> BuildGCC() override;
  std::unique_ptr<Tool> BuildGit() override;

public:
  JobDetail ConstructDetail(const CompileIntent &intent) override;
  JobDetail ConstructDetail(const DynamicLinkIntent &intent) override;
  JobDetail ConstructDetail(const StaticLinkIntent &intent) override;
  JobDetail ConstructDetail(const ExecutableLinkIntent &intent) override;
};
} // namespace darwin
} // namespace stone
#endif
