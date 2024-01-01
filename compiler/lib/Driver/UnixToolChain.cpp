#include "stone/Driver/ToolChain.h"

using namespace stone;
using namespace stone::file;

std::string UnixToolChain::GetDefaultLinker() const {
  // if (driverOpts.GetTriple().isAndroid()){
  //   return "lld";
  // }

  // switch (driverOpts.GetTriple().getArch()) {
  // case llvm::Triple::arm:
  // case llvm::Triple::aarch64:
  // case llvm::Triple::aarch64_32:
  // case llvm::Triple::armeb:
  // case llvm::Triple::thumb:
  // case llvm::Triple::thumbeb:
  //   // BFD linker has issues wrt relocation of the protocol conformance
  //   // section on these targets, it also generates COPY relocations for
  //   // final executables, as such, unless specified, we default to gold
  //   // linker.
  //   return "gold";
  // case llvm::Triple::x86:
  // case llvm::Triple::x86_64:
  // case llvm::Triple::ppc64:
  // case llvm::Triple::ppc64le:
  // case llvm::Triple::systemz:
  //   // BFD linker has issues wrt relocations against protected symbols.
  //   return "gold";
  // default:
  //   // Otherwise, use the default BFD linker.
  //   return "";
  // }
  return "";
}
