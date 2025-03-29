#ifndef STONE_DRIVER_TOOLCHAIN_H
#define STONE_DRIVER_TOOLCHAIN_H

#include "stone/Driver/DriverOptions.h"
#include "clang/Basic/DarwinSDKInfo.h"

#include "llvm/Option/ArgList.h"
#include "llvm/Support/Compiler.h"

namespace stone {

class Driver;

class ToolChain {
  const Driver &driver;

protected:
  ToolChain(const Driver &driver);

public:
  virtual ~ToolChain();

public:
  const Driver &GetDriver() const { return driver; }
  ToolChainKind GetKind() const;

public:
};

class DarwinToolChain final : public ToolChain {

public:
  DarwinToolChain(const Driver &driver);
  ~DarwinToolChain() = default;

public:
  static bool classof(const ToolChain *toolChain) {
    return toolChain->GetKind() == ToolChainKind::Darwin;
  }
};

} // namespace stone
#endif