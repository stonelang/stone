#ifndef STONE_BASIC_SRCMGR_H
#define STONE_BASIC_SRCMGR_H

#include "stone/Basic/FileSystem.h"
#include "stone/Basic/SrcLoc.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseMapInfo.h"
#include "llvm/Support/SourceMgr.h"

#include <functional>
#include <map>
#include <optional>

namespace stone {

/// This class manages and owns source buffers.
class SrcMgr {
  llvm::SourceMgr llvmSrcMgr;
  unsigned CodeCompletionBufferID = 0U;
  unsigned CodeCompletionOffset;
  llvm::IntrusiveRefCntPtr<llvm::vfs::FileSystem> fileSystem;
  /// Associates buffer identifiers to buffer IDs.
  llvm::DenseMap<StringRef, unsigned> BufIdentIDMap;

  /// A cache mapping buffer identifiers to vfs Status entries.
  ///
  /// This is as much a hack to prolong the lifetime of status objects as it is
  /// to speed up stats.
  mutable llvm::DenseMap<StringRef, llvm::vfs::Status> StatusCache;

  struct ReplacedRangeType {
    SrcRange Original;
    SrcRange New;
    ReplacedRangeType() {}
    ReplacedRangeType(std::nullopt_t) {}
    ReplacedRangeType(SrcRange Original, SrcRange New)
        : Original(Original), New(New) {
      assert(Original.isValid() && New.isValid());
    }

    explicit operator bool() const { return Original.isValid(); }
  };
  ReplacedRangeType ReplacedRange;

  // \c #sourceLocation directive handling.
  struct VirtualFile {
    CharSrcRange Range;
    std::string Name;
    int LineOffset;
  };
  std::map<const char *, VirtualFile> VirtualFiles;
  mutable std::pair<const char *, const VirtualFile *> CachedVFile = {nullptr,
                                                                      nullptr};

  std::optional<unsigned> findBufferContainingLocInternal(SrcLoc Loc) const;

public:
  SrcMgr(llvm::IntrusiveRefCntPtr<llvm::vfs::FileSystem> FS =
             llvm::vfs::getRealFileSystem())
      : fileSystem(FS) {}

  llvm::SourceMgr &GetLLVMSrcMgr() { return llvmSrcMgr; }
  const llvm::SourceMgr &GetLLVMSrcMgr() const { return llvmSrcMgr; }

  void setFileSystem(llvm::IntrusiveRefCntPtr<llvm::vfs::FileSystem> FS) {
    fileSystem = FS;
  }

  llvm::IntrusiveRefCntPtr<llvm::vfs::FileSystem> getFileSystem() {
    return fileSystem;
  }

  void setCodeCompletionPoint(unsigned BufferID, unsigned Offset) {
    assert(BufferID != 0U && "Buffer should be valid");

    CodeCompletionBufferID = BufferID;
    CodeCompletionOffset = Offset;
  }

  bool HasCodeCompletionBuffer() const { return CodeCompletionBufferID != 0U; }

  unsigned getCodeCompletionBufferID() const { return CodeCompletionBufferID; }

  unsigned getCodeCompletionOffset() const { return CodeCompletionOffset; }

  SrcLoc getCodeCompletionLoc() const;

  const ReplacedRangeType &getReplacedRange() const { return ReplacedRange; }
  void setReplacedRange(const ReplacedRangeType &val) { ReplacedRange = val; }

  /// Returns true if \c LHS is before \c RHS in the source buffer.
  bool isBeforeInBuffer(SrcLoc LHS, SrcLoc RHS) const {
    return LHS.Value.getPointer() < RHS.Value.getPointer();
  }

  /// Returns true if range \c R contains the location \c Loc.  The location
  /// \c Loc should point at the beginning of the token.
  bool rangeContainsTokenLoc(SrcRange R, SrcLoc Loc) const {
    return Loc == R.Start || Loc == R.End ||
           (isBeforeInBuffer(R.Start, Loc) && isBeforeInBuffer(Loc, R.End));
  }

  /// Returns true if range \c Enclosing contains the range \c Inner.
  bool rangeContains(SrcRange Enclosing, SrcRange Inner) const {
    return rangeContainsTokenLoc(Enclosing, Inner.Start) &&
           rangeContainsTokenLoc(Enclosing, Inner.End);
  }

  /// Returns true if range \p R contains the code-completion location, if any.
  bool rangeContainsCodeCompletionLoc(SrcRange R) const {
    return CodeCompletionBufferID
               ? rangeContainsTokenLoc(R, getCodeCompletionLoc())
               : false;
  }

  /// Returns the buffer ID for the specified *valid* location.
  ///
  /// Because a valid source location always corresponds to a source buffer,
  /// this routine always returns a valid buffer ID.
  unsigned findBufferContainingLoc(SrcLoc Loc) const;

  /// Whether the source location is pointing to any buffer owned by the SrcMgr.
  bool isOwning(SrcLoc Loc) const;

  /// Adds a memory buffer to the SrcMgr, taking ownership of it.
  unsigned addNewSourceBuffer(std::unique_ptr<llvm::MemoryBuffer> Buffer);

  llvm::MemoryBuffer *GetFileBuffer(llvm::StringRef inputFile);

  /// Add a \c #sourceLocation-defined virtual file region.
  ///
  /// By default, this region continues to the end of the buffer.
  ///
  /// \returns True if the new file was added, false if the file already exists.
  /// The name and line offset must match exactly in that case.
  ///
  /// \sa closeVirtualFile
  bool openVirtualFile(SrcLoc loc, StringRef name, int lineOffset);

  /// Close a \c #sourceLocation-defined virtual file region.
  void closeVirtualFile(SrcLoc end);

  /// Creates a copy of a \c MemoryBuffer and adds it to the \c SrcMgr,
  /// taking ownership of the copy.
  unsigned addMemBufferCopy(llvm::MemoryBuffer *Buffer);

  /// Creates and adds a memory buffer to the \c SrcMgr, taking
  /// ownership of the newly created copy.
  ///
  /// \p InputData and \p BufIdentifier are copied, so that this memory can go
  /// away as soon as this function returns.
  unsigned addMemBufferCopy(StringRef InputData, StringRef BufIdentifier = "");

  /// Returns a buffer ID for a previously added buffer with the given
  /// buffer identifier, or None if there is no such buffer.
  std::optional<unsigned>
  getIDForBufferIdentifier(StringRef BufIdentifier) const;

  /// Returns the identifier for the buffer with the given ID.
  ///
  /// \p BufferID must be a valid buffer ID.
  ///
  /// This should not be used for displaying information about the \e contents
  /// of a buffer, since lines within the buffer may be marked as coming from
  /// other files using \c #sourceLocation. Use #getDisplayNameForLoc instead
  /// in that case.
  StringRef getIdentifierForBuffer(unsigned BufferID) const;

  /// Returns a SrcRange covering the entire specified buffer.
  ///
  /// Note that the start location might not point at the first token: it
  /// might point at whitespace or a comment.
  CharSrcRange getRangeForBuffer(unsigned BufferID) const;

  /// Returns the SrcLoc for the beginning of the specified buffer
  /// (at offset zero).
  ///
  /// Note that the resulting location might not point at the first token: it
  /// might point at whitespace or a comment.
  SrcLoc getLocForBufferStart(unsigned BufferID) const {
    return getRangeForBuffer(BufferID).getStart();
  }

  /// Returns the offset in bytes for the given valid source location.
  unsigned getLocOffsetInBuffer(SrcLoc Loc, unsigned BufferID) const;

  /// Returns the distance in bytes between the given valid source
  /// locations.
  unsigned getByteDistance(SrcLoc Start, SrcLoc End) const;

  /// Returns the SrcLoc for the byte offset in the specified buffer.
  SrcLoc getLocForOffset(unsigned BufferID, unsigned Offset) const {
    return getLocForBufferStart(BufferID).getAdvancedLoc(Offset);
  }

  /// Returns a buffer identifier suitable for display to the user containing
  /// the given source location.
  ///
  /// This respects \c #sourceLocation directives and the 'use-external-names'
  /// directive in VFS overlay files. If you need an on-disk file name, use
  /// #getIdentifierForBuffer instead.
  StringRef getDisplayNameForLoc(SrcLoc Loc) const;

  /// Returns the line and column represented by the given source location.
  ///
  /// If \p BufferID is provided, \p Loc must come from that source buffer.
  ///
  /// This respects \c #sourceLocation directives.
  std::pair<unsigned, unsigned>
  getPresumedLineAndColumnForLoc(SrcLoc Loc, unsigned BufferID = 0) const {
    assert(Loc.isValid());
    int LineOffset = getLineOffset(Loc);
    int l, c;
    std::tie(l, c) = llvmSrcMgr.getLineAndColumn(Loc.Value, BufferID);
    assert(LineOffset + l > 0 && "bogus line offset");
    return {LineOffset + l, c};
  }

  /// Returns the real line and column for a source location.
  ///
  /// If \p BufferID is provided, \p Loc must come from that source buffer.
  ///
  /// This does not respect \c #sourceLocation directives.
  std::pair<unsigned, unsigned>
  getLineAndColumnInBuffer(SrcLoc Loc, unsigned BufferID = 0) const {
    assert(Loc.isValid());
    return llvmSrcMgr.getLineAndColumn(Loc.Value, BufferID);
  }

  StringRef getEntireTextForBuffer(unsigned BufferID) const;

  StringRef extractText(CharSrcRange Range,
                        std::optional<unsigned> BufferID = std::nullopt) const;

  llvm::SMDiagnostic GetMessage(stone::SrcLoc loc,
                                llvm::SourceMgr::DiagKind Kind,
                                const Twine &msg,
                                ArrayRef<llvm::SMRange> ranges,
                                ArrayRef<llvm::SMFixIt> fixIts,
                                bool EmitMacroExpansionFiles = false) const;

  /// Verifies that all buffers are still valid.
  void verifyAllBuffers() const;

  /// Translate line and column pair to the offset.
  /// If the column number is the maximum unsinged int, return the offset of the
  /// end of the line.
  std::optional<unsigned> resolveFromLineCol(unsigned BufferId, unsigned Line,
                                             unsigned Col) const;

  /// Translate the end position of the given line to the offset.
  std::optional<unsigned> resolveOffsetForEndOfLine(unsigned BufferId,
                                                    unsigned Line) const;

  /// Get the length of the line
  std::optional<unsigned> getLineLength(unsigned BufferId, unsigned Line) const;

  SrcLoc getLocForLineCol(unsigned BufferId, unsigned Line,
                          unsigned Col) const {
    auto Offset = resolveFromLineCol(BufferId, Line, Col);
    return Offset.value() ? getLocForOffset(BufferId, Offset.value())
                          : SrcLoc();
  }

  std::string getLineString(unsigned BufferID, unsigned LineNumber);

  SrcLoc getLocFromExternalSource(StringRef Path, unsigned Line, unsigned Col);

private:
  const VirtualFile *getVirtualFile(SrcLoc Loc) const;
  unsigned getExternalSourceBufferId(StringRef Path);

  int getLineOffset(SrcLoc Loc) const {
    if (auto VFile = getVirtualFile(Loc))
      return VFile->LineOffset;
    else
      return 0;
  }

public:
  bool isLocInVirtualFile(SrcLoc Loc) const {
    return getVirtualFile(Loc) != nullptr;
  }
};
} // namespace stone

#endif