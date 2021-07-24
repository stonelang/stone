#ifndef STONE_BASIC_SOURCE_H
#define STONE_BASIC_SOURCE_H

#include "stone/Basic/LLVM.h"
#include "stone/Basic/FileSystem.h"


#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseMapInfo.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/SMLoc.h"
#include "llvm/Support/SourceMgr.h"

#include <functional>
#include <map>

namespace stone {
namespace src {

class SrcMgr;
/// SrcLoc in stone::src is just an SMLoc.  We define it as a different type
/// (instead of as a typedef) just to remove the "getFromPointer" methods and
/// enforce purity in the Swift codebase.
class SrcLoc {
  friend class SrcMgr;
  friend class SrcRange;
  friend class CharSrcRange;
  friend class DiagnosticListener;

  llvm::SMLoc Value;

public:
  SrcLoc() {}
  explicit SrcLoc(llvm::SMLoc Value) : Value(Value) {}

  bool isValid() const { return Value.isValid(); }
  bool isInvalid() const { return !isValid(); }

  bool operator==(const SrcLoc &RHS) const { return RHS.Value == Value; }
  bool operator!=(const SrcLoc &RHS) const { return !operator==(RHS); }

  /// Return a source location advanced a specified number of bytes.
  SrcLoc getAdvancedLoc(int ByteOffset) const {
    assert(isValid() && "Can't advance an invalid location");
    return SrcLoc(llvm::SMLoc::getFromPointer(Value.getPointer() + ByteOffset));
  }

  SrcLoc getAdvancedLocOrInvalid(int ByteOffset) const {
    if (isValid())
      return getAdvancedLoc(ByteOffset);
    return SrcLoc();
  }

  const void *getOpaquePointerValue() const { return Value.getPointer(); }

  /// Print out the SrcLoc.  If this location is in the same buffer
  /// as specified by \c LastBufferID, then we don't print the filename.  If
  /// not, we do print the filename, and then update \c LastBufferID with the
  /// BufferID printed.
  void print(raw_ostream &OS, const SrcMgr &SM, unsigned &LastBufferID) const;

  void printLineAndColumn(raw_ostream &OS, const SrcMgr &SM,
                          unsigned BufferID = 0) const;

  void print(raw_ostream &OS, const SrcMgr &SM) const {
    unsigned Tmp = ~0U;
    print(OS, SM, Tmp);
  }

  // SWIFT_DEBUG_DUMPER(dump(const SrcMgr &SM));

  friend size_t hash_value(SrcLoc loc) {
    return reinterpret_cast<uintptr_t>(loc.getOpaquePointerValue());
  }

  friend void simple_display(raw_ostream &OS, const SrcLoc &loc) {
    // Nothing meaningful to print.
  }
};

/// SrcRange in stone::src is a pair of locations.  However, note that the end
/// location is the start of the last token in the range, not the last character
/// in the range.  This is unlike SMRange, so we use a distinct type to make
/// sure that proper conversions happen where important.
class SrcRange {
public:
  SrcLoc Start, End;

  SrcRange() {}
  SrcRange(SrcLoc Loc) : Start(Loc), End(Loc) {}
  SrcRange(SrcLoc Start, SrcLoc End) : Start(Start), End(End) {
    assert(Start.isValid() == End.isValid() &&
           "Start and end should either both be valid or both be invalid!");
  }

  bool isValid() const { return Start.isValid(); }
  bool isInvalid() const { return !isValid(); }

  /// Extend this SrcRange to the smallest continuous SrcRange that
  /// includes both this range and the other one.
  void widen(SrcRange Other);

  bool operator==(const SrcRange &other) const {
    return Start == other.Start && End == other.End;
  }
  bool operator!=(const SrcRange &other) const { return !operator==(other); }

  /// Print out the SrcRange.  If the locations are in the same buffer
  /// as specified by LastBufferID, then we don't print the filename.  If not,
  /// we do print the filename, and then update LastBufferID with the BufferID
  /// printed.
  void print(raw_ostream &OS, const SrcMgr &SM, unsigned &LastBufferID,
             bool PrintText = true) const;

  void print(raw_ostream &OS, const SrcMgr &SM, bool PrintText = true) const {
    unsigned Tmp = ~0U;
    print(OS, SM, Tmp, PrintText);
  }

  //SWIFT_DEBUG_DUMPER(dump(const SrcMgr &SM));
};

/// A half-open character-based source range.
class CharSrcRange {
  SrcLoc Start;
  unsigned ByteLength;

public:
  /// Constructs an invalid range.
  CharSrcRange() = default;

  CharSrcRange(SrcLoc Start, unsigned ByteLength)
      : Start(Start), ByteLength(ByteLength) {}

  /// Constructs a character range which starts and ends at the
  /// specified character locations.
  CharSrcRange(const SrcMgr &SM, SrcLoc Start, SrcLoc End);

  /// Use Lexer::getCharSrcRangeFromSrcRange() instead.
  CharSrcRange(const SrcMgr &SM, SrcRange Range) = delete;

  bool isValid() const { return Start.isValid(); }
  bool isInvalid() const { return !isValid(); }

  bool operator==(const CharSrcRange &other) const {
    return Start == other.Start && ByteLength == other.ByteLength;
  }
  bool operator!=(const CharSrcRange &other) const {
    return !operator==(other);
  }

  SrcLoc getStart() const { return Start; }
  SrcLoc getEnd() const { return Start.getAdvancedLocOrInvalid(ByteLength); }

  /// Returns true if the given source location is contained in the range.
  bool contains(SrcLoc loc) const {
    auto less = std::less<const char *>();
    auto less_equal = std::less_equal<const char *>();
    return less_equal(getStart().Value.getPointer(), loc.Value.getPointer()) &&
           less(loc.Value.getPointer(), getEnd().Value.getPointer());
  }

  bool contains(CharSrcRange Other) const {
    auto less_equal = std::less_equal<const char *>();
    return contains(Other.getStart()) &&
           less_equal(Other.getEnd().Value.getPointer(),
                      getEnd().Value.getPointer());
  }

  /// expands *this to cover Other
  void widen(CharSrcRange Other) {
    auto Diff = Other.getEnd().Value.getPointer() - getEnd().Value.getPointer();
    if (Diff > 0) {
      ByteLength += Diff;
    }
    const auto MyStartPtr = getStart().Value.getPointer();
    Diff = MyStartPtr - Other.getStart().Value.getPointer();
    if (Diff > 0) {
      ByteLength += Diff;
      Start = SrcLoc(llvm::SMLoc::getFromPointer(MyStartPtr - Diff));
    }
  }

  bool overlaps(CharSrcRange Other) const {
    if (getByteLength() == 0 || Other.getByteLength() == 0)
      return false;
    return contains(Other.getStart()) || Other.contains(getStart());
  }

  StringRef str() const {
    return StringRef(Start.Value.getPointer(), ByteLength);
  }

  /// Return the length of this valid range in bytes.  Can be zero.
  unsigned getByteLength() const {
    assert(isValid() && "length does not make sense for an invalid range");
    return ByteLength;
  }

  /// Print out the CharSrcRange.  If the locations are in the same buffer
  /// as specified by LastBufferID, then we don't print the filename.  If not,
  /// we do print the filename, and then update LastBufferID with the BufferID
  /// printed.
  void print(raw_ostream &OS, const SrcMgr &SM, unsigned &LastBufferID,
             bool PrintText = true) const;

  void print(raw_ostream &OS, const SrcMgr &SM, bool PrintText = true) const {
    unsigned Tmp = ~0U;
    print(OS, SM, Tmp, PrintText);
  }

  // SWIFT_DEBUG_DUMPER(dump(const SrcMgr &SM));
};

/// This class manages and owns source buffers.
class SrcMgr {
  llvm::SourceMgr llvmSrcMgr;
  llvm::IntrusiveRefCntPtr<llvm::vfs::FileSystem> fileSystem;
  unsigned CodeCompletionBufferID = 0U;
  unsigned CodeCompletionOffset;

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
    ReplacedRangeType(llvm::NoneType) {}
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
  mutable std::pair<const char *, const VirtualFile*> CachedVFile = {nullptr, nullptr};

  Optional<unsigned> findBufferContainingLocInternal(SrcLoc Loc) const;
public:
  SrcMgr(llvm::IntrusiveRefCntPtr<llvm::vfs::FileSystem> FS =
                    llvm::vfs::getRealFileSystem())
    : fileSystem(FS) {}

  llvm::SourceMgr &getllvmSrcMgr() {
    return llvmSrcMgr;
  }
  const llvm::SourceMgr &getllvmSrcMgr() const {
    return llvmSrcMgr;
  }

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

  bool hasCodeCompletionBuffer() const {
    return CodeCompletionBufferID != 0U;
  }

  unsigned getCodeCompletionBufferID() const {
    return CodeCompletionBufferID;
  }

  unsigned getCodeCompletionOffset() const {
    return CodeCompletionOffset;
  }

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
  Optional<unsigned> getIDForBufferIdentifier(StringRef BufIdentifier) const;

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
    assert(LineOffset+l > 0 && "bogus line offset");
    return { LineOffset + l, c };
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
                        Optional<unsigned> BufferID = None) const;

  llvm::SMDiagnostic GetMessage(SrcLoc Loc, llvm::SourceMgr::DiagKind Kind,
                                const Twine &Msg,
                                ArrayRef<llvm::SMRange> Ranges,
                                ArrayRef<llvm::SMFixIt> FixIts) const;

  /// Verifies that all buffers are still valid.
  void verifyAllBuffers() const;

  /// Translate line and column pair to the offset.
  /// If the column number is the maximum unsinged int, return the offset of the end of the line.
  llvm::Optional<unsigned> resolveFromLineCol(unsigned BufferId, unsigned Line,
                                              unsigned Col) const;

  /// Translate the end position of the given line to the offset.
  llvm::Optional<unsigned> resolveOffsetForEndOfLine(unsigned BufferId,
                                                     unsigned Line) const;

  /// Get the length of the line
  llvm::Optional<unsigned> getLineLength(unsigned BufferId, unsigned Line) const;

  SrcLoc getLocForLineCol(unsigned BufferId, unsigned Line, unsigned Col) const {
    auto Offset = resolveFromLineCol(BufferId, Line, Col);
    return Offset.hasValue() ? getLocForOffset(BufferId, Offset.getValue()) :
                               SrcLoc();
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

} // namespace src

} // namespace stone

namespace llvm {
template <typename T> struct DenseMapInfo;

template <> struct DenseMapInfo<stone::src::SrcLoc> {
  static stone::src::SrcLoc getEmptyKey() {
    return stone::src::SrcLoc(
        SMLoc::getFromPointer(DenseMapInfo<const char *>::getEmptyKey()));
  }

  static stone::src::SrcLoc getTombstoneKey() {
    // Make this different from empty key. See for context:
    // http://lists.llvm.org/pipermail/llvm-dev/2015-July/088744.html
    return stone::src::SrcLoc(
        SMLoc::getFromPointer(DenseMapInfo<const char *>::getTombstoneKey()));
  }

  static unsigned getHashValue(const stone::src::SrcLoc &Val) {
    return DenseMapInfo<const void *>::getHashValue(
        Val.getOpaquePointerValue());
  }

  static bool isEqual(const stone::src::SrcLoc &LHS,
                      const stone::src::SrcLoc &RHS) {
    return LHS == RHS;
  }
};

template <> struct DenseMapInfo<stone::src::SrcRange> {
  static stone::src::SrcRange getEmptyKey() {
    return stone::src::SrcRange(stone::src::SrcLoc(
        SMLoc::getFromPointer(DenseMapInfo<const char *>::getEmptyKey())));
  }

  static stone::src::SrcRange getTombstoneKey() {
    // Make this different from empty key. See for context:
    // http://lists.llvm.org/pipermail/llvm-dev/2015-July/088744.html
    return stone::src::SrcRange(stone::src::SrcLoc(
        SMLoc::getFromPointer(DenseMapInfo<const char *>::getTombstoneKey())));
  }

  static unsigned getHashValue(const stone::src::SrcRange &Val) {
    return hash_combine(Val.Start.getOpaquePointerValue(),
                        Val.End.getOpaquePointerValue());
  }

  static bool isEqual(const stone::src::SrcRange &LHS,
                      const stone::src::SrcRange &RHS) {
    return LHS == RHS;
  }
};
} // namespace llvm
#endif