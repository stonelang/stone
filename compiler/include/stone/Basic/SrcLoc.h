#ifndef STONE_BASIC_SRCLOC_H
#define STONE_BASIC_SRCLOC_H

#include "stone/Basic/Debug.h"
#include "stone/Basic/LLVM.h"
#include "stone/Basic/Tokenable.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/SMLoc.h"

#include <optional>

namespace stone {

class SrcMgr;
/// SrcLoc just an SMLoc.  We define it as a different type
/// (instead of as a typedef) just to remove the "getFromPointer" methods and
/// enforce purity in the Swift codebase.
class SrcLoc {
  friend class SrcMgr;
  friend class SrcRange;
  friend class CharSrcRange;
  friend class DiagnosticConsumer;

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
    if (isValid()) {
      return getAdvancedLoc(ByteOffset);
    }
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

  STONE_DEBUG_DUMPER(dump(const SrcMgr &SM));

  friend size_t hash_value(SrcLoc loc) {
    return reinterpret_cast<uintptr_t>(loc.getOpaquePointerValue());
  }

  friend void simple_display(raw_ostream &OS, const SrcLoc &loc) {
    // Nothing meaningful to print.
  }

public:
  static SrcLoc GetFromPtr(const char *ptr) {
    return SrcLoc(llvm::SMLoc::getFromPointer(ptr));
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

  STONE_DEBUG_DUMPER(dump(const SrcMgr &SM));
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

  STONE_DEBUG_DUMPER(dump(const SrcMgr &SM));

public:
  static CharSrcRange ToCharSrcRange(SrcMgr &sm, SrcLoc start, SrcLoc end);
  static CharSrcRange ToCharSrcRange(SrcMgr &sm, SrcRange range,
                                     Tokenable &tokenable);
};

} // namespace stone
#endif