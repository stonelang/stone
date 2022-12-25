#ifndef STONE_BASIC_CODEGENOPTIONS_H
#define STONE_BASIC_CODEGENOPTIONS_H

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/CodeGen.h"
#include "llvm/Support/Regex.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Transforms/Instrumentation/AddressSanitizerOptions.h"

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace stone {

enum class InlineMode {
  Default = 0, // Use the standard function inlining pass.
  Hint,        // Inline only (implicitly) hinted functions.
  Always       // Only run the always inlining pass.
};

enum class LTOKind {
  None,
  ///
  Full,
  ///
  Thin
};

// The optimization mode specified on the command line or with function
// attributes.
enum class OptimizationLevel : uint8_t {
  None = 0,
  Less = 1,
  Default = 2,
  Aggressive = 3,
};

enum class NativeModeKind : uint8_t {
  None = 0,
  /// Generate an LLVM module and write it out as LLVM assembly.
  EmitAssemblyPreOptimization,

  /// Generate an LLVM module and write it out as LLVM assembly.
  EmitAssemblyPostOptimization,

  /// Generate an LLVM module and write it out as LLVM bitcode.
  EmitBC,

  /// Generate an LLVM module and compile it to assembly.
  EmitAssembly,

  /// Generate an LLVM module, compile it, and assemble into an object file.
  EmitObject
};

enum class LibraryKind { Library = 0, Framework };

class LinkLibrary final {
private:
  std::string name;
  unsigned kind : 1;
  unsigned forceLoad : 1;

public:
  LinkLibrary(llvm::StringRef name, LibraryKind kind, bool forceLoad = false)
      : name(name), kind(static_cast<unsigned>(kind)), forceLoad(forceLoad) {
    assert(GetKind() == kind && "not enough bits for the kind");
  }

  LibraryKind GetKind() const { return static_cast<LibraryKind>(kind); }
  llvm::StringRef GetName() const { return name; }
  bool ShouldForceLoad() const { return forceLoad; }
};

class CodeGenOptions final {
public:
  bool skipOptimization = false;

  /// The code model to use (-mcmodel).
  std::string codeModel;

  NativeModeKind nativeModeKind = NativeModeKind::None;

  OptimizationLevel optimizationLevel = OptimizationLevel::None;

  /// The libraries and frameworks specified on the command line.
  llvm::SmallVector<LinkLibrary, 4> linkLibraries;

  /// The public dependent libraries specified on the command line.
  std::vector<std::string> publicLinkLibraries;

  /// The name of the relocation model to use.
  llvm::Reloc::Model relocationModel;

  InlineMode inlineMode = InlineMode::Default;

public:
  bool CanOptimize() { return optimizationLevel > OptimizationLevel::Default; }
  bool OptimizeForSpeed() {
    return optimizationLevel == OptimizationLevel::Default;
  }
  bool OptimizeForSize() {
    return optimizationLevel == OptimizationLevel::Aggressive;
  }
};

} // namespace stone

#endif
