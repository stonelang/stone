#ifndef STONE_COMPILE_COMPILEINPUTFILE_H
#define STONE_COMPILE_COMPILEINPUTFILE_H

#include "stone/Basic/FileType.h"
#include "stone/Basic/PrimaryFileSpecificPaths.h"

#include "llvm/ADT/PointerIntPair.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/Path.h"
#include <string>

namespace stone {
/// An \c CompilerInputFile encapsulates information about an input passed to
/// the invocation.
///
/// CompilerInstance inputs are usually passed on the command line without a
/// leading flag. However, there are clients that use the \c
/// CompilerConfiguration as a library like LLDB and SourceKit that generate
/// their own \c CompilerInputFile instances programmatically. Note that an \c
/// CompilerInputFile need not actually be backed by a physical file, nor does
/// its file name actually reflect its contents. \c CompilerInputFile has a
/// constructor that will try to figure out the file type from the file name if
/// none is provided, but many clients that construct \c CompilerInputFile
/// instances themselves may provide bogus file names with pre-computed kinds.
/// It is imperative that \c CompilerInputFile::GetType be used as a source of
/// truth for this information.
///
/// \warning \c CompilerInputFile takes an unfortunately lax view of the
/// ownership of its primary data. It currently only owns the file name and a
/// copy of any assigned \c PrimaryFileSpecificPaths outright. It is the
/// responsibility of the caller to ensure that an associated memory buffer
/// outlives the \c CompilerInputFile.
class CompilerInputFile final {
  std::string filename;
  stone::FileType fileTy;
  llvm::PointerIntPair<llvm::MemoryBuffer *, 1, bool> bufferAndIsPrimary;
  PrimaryFileSpecificPaths primaryFileSpecificPaths;

public:
  /// Constructs an input file from the provided data.
  ///
  /// \warning This entrypoint infers the type of the file from its extension
  /// and is therefore not suitable for most clients that use files synthesized
  /// from memory buffers. Use the overload of this constructor accepting a
  /// memory buffer and an explicit \c file_types::ID instead.
  CompilerInputFile(llvm::StringRef name, bool isPrimary,
                    llvm::MemoryBuffer *buffer = nullptr)
      : CompilerInputFile(
            name, isPrimary, buffer,
            stone::GetFileTypeByExt(llvm::sys::path::extension(name))) {}

  /// Constructs an input file from the provided data.
  CompilerInputFile(llvm::StringRef name, bool isPrimary,
                    llvm::MemoryBuffer *buffer, stone::FileType fileTy)
      : filename(
            ConvertBufferNameFromLLVMGetFileOrSTDINToStoneConventions(name)),
        fileTy(fileTy), bufferAndIsPrimary(buffer, isPrimary),
        primaryFileSpecificPaths(PrimaryFileSpecificPaths()) {
    assert(!filename.empty());
  }

public:
  /// Retrieves the type of this input file.
  stone::FileType GetType() const { return fileTy; };

  /// Retrieves whether this input file was passed as a primary to the
  /// invocation.
  bool IsPrimary() const { return bufferAndIsPrimary.getInt(); }

  /// Retrieves the backing buffer for this input file, if any.
  llvm::MemoryBuffer *GetBuffer() const {
    return bufferAndIsPrimary.getPointer();
  }

  /// The name of this \c CompilerInputFile, or `-` if this input corresponds to
  /// the standard input stream.
  ///
  /// The returned file name is guaranteed not to be the empty string.
  const std::string &GetFileName() const {
    assert(!filename.empty());
    return filename;
  }

  /// Return stone-standard file name from a buffer name set by
  /// llvm::MemoryBuffer::getFileOrSTDIN, which uses "<stdin>" instead of "-".
  static llvm::StringRef
  ConvertBufferNameFromLLVMGetFileOrSTDINToStoneConventions(
      llvm::StringRef filename) {
    if (filename.equals("<stdin>")) {
      return "-";
    }
    return filename;
  }

  /// Retrieves the name of the output file corresponding to this input.
  ///
  /// If there is no such corresponding file, the result is the empty string.
  /// If there the resulting output should be directed to the standard output
  /// stream, the result is "-".
  std::string OutputFilename() const {
    return primaryFileSpecificPaths.outputFilename;
  }

  std::string IndexUnitOutputFilename() const {
    if (!primaryFileSpecificPaths.indexUnitOutputFilename.empty()) {
      return primaryFileSpecificPaths.indexUnitOutputFilename;
    }
    return OutputFilename();
  }

  /// If there are explicit primary inputs (i.e. designated with -primary-input
  /// or -primary-filelist), the paths specific to those inputs (other than the
  /// input file path itself) are kept here. If there are no explicit primary
  /// inputs (for instance for whole module optimization), the corresponding
  /// paths are kept in the first input file.
  const PrimaryFileSpecificPaths &GetPrimaryFileSpecificPaths() const {
    return primaryFileSpecificPaths;
  }

  void SetPrimaryFileSpecificPaths(
      PrimaryFileSpecificPaths &&primaryFileSpecificPaths) {
    this->primaryFileSpecificPaths = std::move(primaryFileSpecificPaths);
  }

  // The next set of functions provides access to those primary-specific paths
  // accessed directly from an CompilerInputFile, as opposed to via
  // CompilerInputsAndOutputs. They merely make the call sites
  // a bit shorter. Add more forwarding methods as needed.

  llvm::StringRef GetDependenciesFilePath() const {
    return GetPrimaryFileSpecificPaths()
        .supplementaryOutputPaths.dependenciesFilePath;
  }
  llvm::StringRef GetLoadedModuleTracePath() const {
    return GetPrimaryFileSpecificPaths()
        .supplementaryOutputPaths.loadedModuleTracePath;
  }
  llvm::StringRef GetSerializedDiagnosticsPath() const {
    return GetPrimaryFileSpecificPaths()
        .supplementaryOutputPaths.serializedDiagnosticsPath;
  }
  llvm::StringRef GetFixItsOutputPath() const {
    return GetPrimaryFileSpecificPaths()
        .supplementaryOutputPaths.fixItsOutputPath;
  }
};
} // namespace stone

#endif // stone_FRONTEND_CompilerInputFile_H
