#ifndef STONE_BASIC_OUTPUTFILEMAP_H
#define STONE_BASIC_OUTPUTFILEMAP_H

#include <memory>
#include <string>

#include "stone/Basic/FileType.h"
#include "stone/Basic/LLVM.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/YAMLParser.h"

namespace stone {

using TypeToPathMap = llvm::DenseMap<filetype::FileType, std::string>;

/// A two-tiered map used to specify paths for multiple output files associated
/// with each input file in a compilation job.
///
/// The structure is a map from input paths to sub-maps, each of which maps
/// file types to output paths.
class OutputFileMap {
private:
  llvm::StringMap<TypeToPathMap> InputToOutputsMap;

public:
  OutputFileMap() {}

  ~OutputFileMap() = default;

  /// Loads an OutputFileMap from the given \p Path into the receiver, if
  /// possible.
  static llvm::Expected<OutputFileMap> LoadFromPath(llvm::StringRef path,
                                                    llvm::StringRef workDir);

  static llvm::Expected<OutputFileMap> LoadFromBuffer(llvm::StringRef data,
                                                      llvm::StringRef workDir);

  /// Loads an OutputFileMap from the given \p Buffer, taking ownership
  /// of the buffer in the process.
  ///
  /// When non-empty, \p workingDirectory is used to resolve relative paths in
  /// the output file map.
  static llvm::Expected<OutputFileMap>
  LoadFromBuffer(std::unique_ptr<llvm::MemoryBuffer> buffer,
                 llvm::StringRef workDir);

  /// Get the map of outputs for the given \p Input, if present in the
  /// OutputFileMap. (If not present, returns nullptr.)
  const TypeToPathMap *GetOutputMapForInput(llvm::StringRef input) const;

  /// Get a map of outputs for the given \p Input, creating it in
  /// the OutputFileMap if not already present.
  TypeToPathMap &GetOrCreateOutputMapForInput(llvm::StringRef input);

  /// Get the map of outputs for a single compile product.
  const TypeToPathMap *GetOutputMapForSingleOutput() const;

  /// Get or create the map of outputs for a single compile product.
  TypeToPathMap &GetOrCreateOutputMapForSingleOutput();

  /// Dump the OutputFileMap to the given \p os.
  void Dump(llvm::raw_ostream &os, bool sort = false) const;

  /// Write the OutputFileMap for the \p inputs so it can be parsed.
  ///
  /// It is not an error if the map does not contain an entry for a particular
  /// input. Instead, an empty sub-map will be written into the output.
  void Write(llvm::raw_ostream &os,
             llvm::ArrayRef<llvm::StringRef> inputs) const;

private:
  /// Parses the given \p Buffer and returns either an OutputFileMap or
  /// error, taking ownership of \p Buffer in the process.
  static llvm::Expected<OutputFileMap>
  Parse(std::unique_ptr<llvm::MemoryBuffer> buffer, llvm::StringRef workDir);
};

} // namespace stone

#endif