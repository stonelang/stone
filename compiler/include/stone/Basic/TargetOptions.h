#ifndef STONE_BASIC_TARGETOPTIONS_H
#define STONE_BASIC_TARGETOPTIONS_H

#include "llvm/ADT/StringMap.h"
#include "llvm/Support/VersionTuple.h"
#include "llvm/Target/TargetOptions.h"

#include <string>
#include <vector>

namespace stone {
/// Options for controlling the target.
class TargetOptions final {
public:
  /// The name of the target triple to compile for.
  std::string triple;

  /// When compiling for the device side, contains the triple used to compile
  /// for the host.
  std::string hostTriple;

  /// If given, the name of the target CPU to generate code for.
  std::string cpu;

  /// If given, the name of the target CPU to tune code for.
  std::string tuneCPU;

  /// If given, the unit to use for floating point math.
  std::string fpMath;

  /// If given, the name of the target ABI to use.
  std::string abi;

  /// The EABI version to use
  llvm::EABI eABIVersion;

  /// If given, the version string of the linker in use.
  std::string linkerVersion;

  /// The list of target specific features to enable or disable, as written on
  /// the command line.
  std::vector<std::string> featuresAsWritten;

  /// The list of target specific features to enable or disable -- this should
  /// be a list of strings starting with by '+' or '-'.
  std::vector<std::string> features;

  /// The map of which features have been enabled disabled based on the command
  /// line.
  llvm::StringMap<bool> featureMap;

  /// If given, enables support for __int128_t and __uint128_t types.
  bool forceEnableInt128 = false;

  /// \brief If enabled, use 32-bit pointers for accessing const/local/shared
  /// address space.
  bool NVPTXUseShortPointers = false;

  /// \brief If enabled, allow AMDGPU unsafe floating point atomics.
  bool allowAMDGPUUnsafeFPAtomics = false;

  // The code model to be used as specified by the user. Corresponds to
  // CodeModel::Model enum defined in include/llvm/Support/CodeGen.h, plus
  // "default" for the case when the user has not explicitly specified a
  // code model.
  std::string codeModel;

  /// The version of the SDK which was used during the compilation.
  /// The option is used for two different purposes:
  /// * on darwin the version is propagated to LLVM where it's used
  ///   to support SDK Version metadata (See D55673).
  /// * CUDA compilation uses it to control parts of CUDA compilation
  ///   in clang that depend on specific version of the CUDA SDK.
  llvm::VersionTuple sdkVersion;
};
} // namespace stone

#endif