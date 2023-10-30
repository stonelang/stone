#ifndef STONE_BASIC_ADDITIONALOUTPUTPATHS_H
#define STONE_BASIC_ADDITIONALOUTPUTPATHS_H

#include "stone/Basic/LLVM.h"
#include "llvm/ADT/StringRef.h"

namespace stone {

struct SupplementaryOutputPaths final {
  /// The path to which we should emit a serialized module.
  /// It is valid whenever there are any inputs.
  ///
  /// This binary format is used to describe the interface of a module when
  /// imported by client source code. The stonemodule format is described in
  /// docs/Serialization.md.
  ///
  /// \sa stone::serialize
  std::string moduleOutputPath;

  /// The path to which we should emit a module source information file.
  /// It is valid whenever there are any inputs.
  ///
  /// This binary format stores source locations and other information about the
  /// declarations in a module.
  ///
  /// \sa stone::serialize
  std::string moduleSourceInfoOutputPath;

  /// The path to which we should emit a module documentation file.
  /// It is valid whenever there are any inputs.
  ///
  /// This binary format stores doc comments and other information about the
  /// declarations in a module.
  ///
  /// \sa stone::serialize
  std::string moduleDocOutputPath;

  /// The path to which we should output a Make-style dependencies file.
  /// It is valid whenever there are any inputs.
  ///
  /// Swift's compilation model means that Make-style dependencies aren't
  /// well-suited to model fine-grained dependencies. See docs/Driver.md for
  /// more information.
  ///
  /// \sa ReferenceDependenciesFilePath
  std::string dependenciesFilePath;

  /// The path to which we should output a Swift "reference dependencies" file.
  /// It is valid whenever there are any inputs.
  ///
  /// "Reference dependencies" track dependencies on a more fine-grained level
  /// than just "this file depends on that file". With Swift's "implicit
  /// visibility" within a module, that becomes very important for any sort of
  /// incremental build. These files are consumed by the Swift driver to decide
  /// whether a source file needs to be recompiled during a build. See
  /// docs/DependencyAnalysis.md for more information.
  ///
  /// \sa stone::emitReferenceDependencies
  /// \sa DependencyGraph
  std::string referenceDependenciesFilePath;

  /// Path to a file which should contain serialized diagnostics for this
  /// invocation invocation.
  ///
  /// This uses the same serialized diagnostics format as Clang, for tools that
  /// want machine-parseable diagnostics. There's a bit more information on
  /// how clients might use this in docs/Driver.md.
  ///
  /// \sa stone::serialized_diagnostics::createConsumer
  std::string serializedDiagnosticsPath;

  /// The path to which we should output fix-its as source edits.
  ///
  /// This is a JSON-based format that is used by the migrator, but is not
  /// really vetted for anything else.
  ///
  /// \sa stone::writeEditsInJson
  std::string fixItsOutputPath;

  /// The path to which we should output a loaded module trace file.
  /// It is valid whenever there are any inputs.
  ///
  /// The file is appended to, and consists of line-delimited JSON objects,
  /// where each line is of the form `{ "name": NAME, "target": TARGET,
  /// "stonemodules": [PATH, PATH, ...] }`, representing the (real-path) PATHs
  /// to each .stonemodule that was loaded while building module NAME for target
  /// TARGET. This format is subject to arbitrary change, however.
  std::string loadedModuleTracePath;

  /// The path to which we should output a TBD file.
  ///
  /// "TBD" stands for "text-based dylib". It's a YAML-based format that
  /// describes the public ABI of a library, which clients can link against
  /// without having an actual dynamic library binary.
  ///
  /// Only makes sense when the compiler has whole-module knowledge.
  ///
  /// \sa stone::writeTBDFile
  std::string tbdPath;

  /// The path to which we should emit a module interface, which can
  /// be used by a client source file to import this module.
  ///
  /// This format is similar to the binary format used for #moduleOutputPath,
  /// but is intended to be stable across compiler versions.
  ///
  /// Currently only makes sense when the compiler has whole-module knowledge.
  ///
  /// \sa stone::emitSwiftInterface
  std::string moduleInterfaceOutputPath;

  /// The path to which we should emit a private module interface.
  ///
  /// The private module interface contains all SPI decls and attributes.
  ///
  /// \sa ModuleInterfaceOutputPath
  std::string privateModuleInterfaceOutputPath;

  /// The path to which we should emit module summary file.
  std::string moduleSummaryOutputPath;

  /// The output path to generate ABI baseline.
  std::string abiDescriptorOutputPath;

  /// The output path of Swift semantic info for this module.
  std::string moduleSemanticInfoOutputPath;

  /// The output path for YAML optimization record file.
  std::string yamlOptRecordPath;

  /// The output path for bitstream optimization record file.
  std::string bitstreamOptRecordPath;

  SupplementaryOutputPaths() = default;
  SupplementaryOutputPaths(const SupplementaryOutputPaths &) = default;

  /// Apply a given function for each existing (non-empty string) supplementary
  /// output
  void
  ForEachSetOutput(llvm::function_ref<void(const std::string &)> fn) const {
    if (!moduleOutputPath.empty())
      fn(moduleOutputPath);
    if (!moduleSourceInfoOutputPath.empty())
      fn(moduleSourceInfoOutputPath);
    if (!moduleDocOutputPath.empty())
      fn(moduleDocOutputPath);
    if (!dependenciesFilePath.empty())
      fn(dependenciesFilePath);
    if (!referenceDependenciesFilePath.empty())
      fn(referenceDependenciesFilePath);
    if (!serializedDiagnosticsPath.empty())
      fn(serializedDiagnosticsPath);
    if (!fixItsOutputPath.empty())
      fn(fixItsOutputPath);
    if (!loadedModuleTracePath.empty())
      fn(loadedModuleTracePath);
    if (!tbdPath.empty())
      fn(tbdPath);
    if (!moduleInterfaceOutputPath.empty())
      fn(moduleInterfaceOutputPath);
    if (!privateModuleInterfaceOutputPath.empty())
      fn(privateModuleInterfaceOutputPath);
    if (!moduleSummaryOutputPath.empty())
      fn(moduleSummaryOutputPath);
    if (!abiDescriptorOutputPath.empty())
      fn(abiDescriptorOutputPath);
    if (!yamlOptRecordPath.empty())
      fn(yamlOptRecordPath);
    if (!bitstreamOptRecordPath.empty())
      fn(bitstreamOptRecordPath);
    if (!moduleSemanticInfoOutputPath.empty())
      fn(moduleSemanticInfoOutputPath);
  }

  bool Empty() const {
    return moduleOutputPath.empty() && moduleDocOutputPath.empty() &&
           dependenciesFilePath.empty() &&
           referenceDependenciesFilePath.empty() &&
           serializedDiagnosticsPath.empty() && loadedModuleTracePath.empty() &&
           tbdPath.empty() && moduleInterfaceOutputPath.empty() &&
           moduleSourceInfoOutputPath.empty() &&
           abiDescriptorOutputPath.empty() &&
           moduleSemanticInfoOutputPath.empty() && yamlOptRecordPath.empty() &&
           bitstreamOptRecordPath.empty();
  }
};
} // namespace stone

#endif
