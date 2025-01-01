#ifndef STONE_BASIC_CODEGEN_OPTIONS_H
#define STONE_BASIC_CODEGEN_OPTIONS_H

// #include "clang/Basic/PointerAuthOptions.h"
// #include "clang/Basic/PointerAuthOptions.h"

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Passes/OptimizationLevel.h"
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
  LLVMFull,
  ///
  LLVMThin
};

// The optimization mode specified on the command line or with function
// attributes.
enum class OptimizationMode : unsigned {
  None = 0,
  Less = 1,
  Default = 2,
  Aggressive = 3,
};

// The optimization mode specified on the command line or with function
// attributes.
// enum class OptimizationMode : uint8_t {
//   NotSet = 0,
//   NoOptimization = 1,  // -Onone
//   ForSpeed = 2,        // -Ospeed == -O
//   ForSize = 3,         // -Osize
//   LastMode = ForSize
// };

// enum : unsigned { NumOptimizationModeBits =
// stone::CountBitsUsed(static_cast<unsigned>(OptimizationMode::LastMode)) };

enum class CodeGenOutputKind : unsigned {
  None = 0,
  /// Generate an LLVM module and return it.
  LLVMModule,

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

enum class CodeGenDebugInfoLevel : unsigned {
  None,             ///< No debug info.
  LineTables,       ///< Line tables only.
  ASTTypes,         ///< Line tables + AST type references.
  DwarfTypes,       ///< Line tables + AST type references + DWARF types.
  Normal = ASTTypes ///< The setting LLDB prefers.
};

enum class CodeGenDebugInfoFormat : unsigned { None, DWARF, CodeView };

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

class CodeGenOptions final {
public:
  bool noIROptimization = false;

  /// Should we use the legacy pass manager.
  unsigned useLegacyPassManager : 1;

  /// The code model to use (-mcmodel).
  std::string codeModel;

  CodeGenOutputKind codeGenOutputKind = CodeGenOutputKind::None;
  bool HasCodeGenOutputKind() {
    return codeGenOutputKind != CodeGenOutputKind::None;
  }

  /// The command line string that is to be stored in the debug info.
  std::string DebugFlags;

  /// List of -Xcc -D macro definitions.
  std::vector<std::string> ClangDefines;

  // TODO: vs llvm::CodeGenOpt::Level
  OptimizationMode optimizationMode = OptimizationMode::None;

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
    return optimizationMode > OptimizationMode::Default;
  }

  bool OptimizeForSpeed() const {
    return optimizationMode == OptimizationMode::Default;
  }
  bool OptimizeForSize() const {
    return optimizationMode == OptimizationMode::Aggressive;
  }

  llvm::CodeGenFileType GetCodeGenFileType() const {
    switch (codeGenOutputKind) {
    case CodeGenOutputKind::ObjectFile:
      return llvm::CodeGenFileType::ObjectFile;
    case CodeGenOutputKind::NativeAssembly:
      return llvm::CodeGenFileType::AssemblyFile;
    default:
      llvm_unreachable("Unknow code generation file type!");
    }
  }
  // TODO: llvm::CodeGenOpt::Level?
  llvm::OptimizationLevel GetOptimizationLevel() const {
    switch (optimizationMode) {
    case OptimizationMode::Less:
      return llvm::OptimizationLevel::O1;
    case OptimizationMode::Default:
      return llvm::OptimizationLevel::O2;
    case OptimizationMode::Aggressive:
      return llvm::OptimizationLevel::O3;
    default:
      return llvm::OptimizationLevel::O0;
    }
  }
};

} // namespace stone

#endif
