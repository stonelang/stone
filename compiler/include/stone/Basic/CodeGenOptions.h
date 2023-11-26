#ifndef STONE_BASIC_CODEGENOPTIONS_H
#define STONE_BASIC_CODEGENOPTIONS_H

// #include "clang/Basic/PointerAuthOptions.h"
// #include "clang/Basic/PointerAuthOptions.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/CodeGen.h"
#include "llvm/Support/Regex.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Transforms/Instrumentation/AddressSanitizerOptions.h"

#include <string>

namespace llvm {
class TargetOptions;
}
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

enum class CodeGenOutputKind : uint8_t {
  /// Generate an LLVM module and return it.
  LLVMModule = 0,

  /// Generate an LLVM module and write it out as LLVM assembly.
  LLVMIRPreOptimization,

  /// Generate an LLVM module and write it out as LLVM assembly.
  LLVMIRPostOptimization,

  /// Generate an LLVM module and write it out as LLVM bitcode.
  LLVMBitCode,

  /// Generate an LLVM module and compile it to assembly.
  NativeAssembly,

  /// Generate an LLVM module, compile it, and assemble into an object file.
  ObjectFile
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

using IRTargetOptions = std::tuple<llvm::TargetOptions, std::string,
                                   std::vector<std::string>, std::string>;

// using clang::PointerAuthSchema;
struct PointerAuthOptions final /*: clang::PointerAuthOptions*/ {
public:
};

class CodeGenOptions final {
public:
  bool skipOptimization = false;

  /// Should we use the legacy pass manager.
  unsigned useLegacyPassManager : 1;

  /// The code model to use (-mcmodel).
  std::string codeModel;

  CodeGenOutputKind codeGenOutputKind = CodeGenOutputKind::None;

  OptimizationLevel optimizationLevel = OptimizationLevel::None;

  /// The libraries and frameworks specified on the command line.
  llvm::SmallVector<LinkLibrary, 4> linkLibraries;

  /// The public dependent libraries specified on the command line.
  std::vector<std::string> publicLinkLibraries;

  /// The name of the relocation model to use -- it default to llvm::Reloc::PIC_
  llvm::Reloc::Model relocationModel = llvm::Reloc::PIC_;

  InlineMode inlineMode = InlineMode::Default;

  /// Should we spend time verifying that the IR we produce is
  /// well-formed?
  unsigned verifyWellFormedIR : 1;

  IRTargetOptions irTargetOptions;

  /// Pointer authentication.
  // PointerAuthOptions pointerAuthOptions;

  /// Emit functions to separate sections.
  unsigned functionSections : 1;

  /// The LLVM target options.
  llvm::TargetOptions llvmTargetOpts;

  /// The target CPU
  std::string targetCPU;

  /// The triple generated from clang.
  std::string effectiveClangTriple;

  /// The features the target supports.
  std::vector<std::string> targetFeatures;

  bool isWholeModuleCompile = false;

public:
  bool ShouldOptimize() const {
    return optimizationLevel > OptimizationLevel::Default;
  }

  bool OptimizeForSpeed() const {
    return optimizationLevel == OptimizationLevel::Default;
  }
  bool OptimizeForSize() const {
    return optimizationLevel == OptimizationLevel::Aggressive;
  }

  // TODO: OK for now
  llvm::CodeGenFileType GetLLVMCodeGenFileType() const {
    if (codeGenOutputKind == CodeGenOutputKind::NativeAssembly) {
      return llvm::CGFT_AssemblyFile;
    }
    return llvm::CGFT_ObjectFile;
  }
};

} // namespace stone

#endif
