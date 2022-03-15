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
#include "stone/Core/Malloc.h"
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
  CommandInvocation
  ConstructInvocation(const CompileIntent &intent) const override;

  CommandInvocation
  ConstructInvocation(const StaticLinkIntent &intent) const override;

  CommandInvocation
  ConstructInvocation(const DynamicLinkIntent &intent) const override;

protected:
  std::unique_ptr<Tool> BuildSCTool() override;
  std::unique_ptr<Tool> BuildLDTool() override;
  std::unique_ptr<Tool> BuildLLDTool() override;
  std::unique_ptr<Tool> BuildClangTool() override;
  std::unique_ptr<Tool> BuildGCCTool() override;
};

} // namespace stone
#endif
