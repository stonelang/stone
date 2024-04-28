#include "stone/Basic/SrcMgr.h"
#include "stone/Basic/Token.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/PrettyStackTrace.h"
#include "llvm/Support/raw_ostream.h"

using namespace stone;
using stone::Token;

void SrcMgr::verifyAllBuffers() const {
  llvm::PrettyStackTraceString backtrace{
      "Checking that all source buffers are still valid"};

  // FIXME: This depends on the buffer IDs chosen by llvm::SourceMgr.
  LLVM_ATTRIBUTE_USED static char arbitraryTotal = 0;
  for (unsigned i = 1, e = llvmSrcMgr.getNumBuffers(); i <= e; ++i) {
    auto *buffer = llvmSrcMgr.getMemoryBuffer(i);
    if (buffer->getBufferSize() == 0)
      continue;
    arbitraryTotal += buffer->getBufferStart()[0];
    arbitraryTotal += buffer->getBufferEnd()[-1];
  }
}

SrcLoc SrcMgr::getCodeCompletionLoc() const {
  if (CodeCompletionBufferID == 0U)
    return SrcLoc();

  return getLocForBufferStart(CodeCompletionBufferID)
      .getAdvancedLoc(CodeCompletionOffset);
}

StringRef SrcMgr::getDisplayNameForLoc(SrcLoc Loc) const {
  // Respect #line first
  if (auto VFile = getVirtualFile(Loc))
    return VFile->Name;

  // Next, try the stat cache
  auto Ident = getIdentifierForBuffer(findBufferContainingLoc(Loc));
  auto found = StatusCache.find(Ident);
  if (found != StatusCache.end()) {
    return found->second.getName();
  }

  // Populate the cache with a (virtual) stat.
  if (auto Status = fileSystem->status(Ident)) {
    return (StatusCache[Ident] = Status.get()).getName();
  }

  // Finally, fall back to the buffer identifier.
  return Ident;
}

unsigned
SrcMgr::addNewSourceBuffer(std::unique_ptr<llvm::MemoryBuffer> Buffer) {
  assert(Buffer);
  StringRef BufIdentifier = Buffer->getBufferIdentifier();
  auto ID = llvmSrcMgr.AddNewSourceBuffer(std::move(Buffer), llvm::SMLoc());
  BufIdentIDMap[BufIdentifier] = ID;
  return ID;
}

unsigned SrcMgr::addMemBufferCopy(llvm::MemoryBuffer *Buffer) {
  return addMemBufferCopy(Buffer->getBuffer(), Buffer->getBufferIdentifier());
}

unsigned SrcMgr::addMemBufferCopy(StringRef InputData,
                                  StringRef BufIdentifier) {
  auto Buffer = std::unique_ptr<llvm::MemoryBuffer>(
      llvm::MemoryBuffer::getMemBufferCopy(InputData, BufIdentifier));
  return addNewSourceBuffer(std::move(Buffer));
}

llvm::MemoryBuffer *SrcMgr::GetFileBuffer(llvm::StringRef inputFile) {
  llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> fileBufOrErr =
      llvm::MemoryBuffer::getFileOrSTDIN(inputFile);
  if (!fileBufOrErr) {
    return nullptr;
  }
  return fileBufOrErr.get().get();
}

bool SrcMgr::openVirtualFile(SrcLoc loc, StringRef name, int lineOffset) {
  CharSrcRange fullRange = getRangeForBuffer(findBufferContainingLoc(loc));
  SrcLoc end;

  auto nextRangeIter = VirtualFiles.upper_bound(loc.Value.getPointer());
  if (nextRangeIter != VirtualFiles.end() &&
      fullRange.contains(nextRangeIter->second.Range.getStart())) {
    const VirtualFile &existingFile = nextRangeIter->second;
    if (existingFile.Range.getStart() == loc) {
      assert(existingFile.Name == name);
      assert(existingFile.LineOffset == lineOffset);
      return false;
    }
    assert(!existingFile.Range.contains(loc) &&
           "must close current open file first");
    end = nextRangeIter->second.Range.getStart();
  } else {
    end = fullRange.getEnd();
  }

  CharSrcRange range = CharSrcRange(*this, loc, end);
  VirtualFiles[end.Value.getPointer()] = {range, name.str(), lineOffset};
  CachedVFile = {nullptr, nullptr};
  return true;
}

void SrcMgr::closeVirtualFile(SrcLoc end) {
  auto *virtualFile = const_cast<VirtualFile *>(getVirtualFile(end));
  if (!virtualFile) {
#ifndef NDEBUG
    unsigned bufferID = findBufferContainingLoc(end);
    CharSrcRange fullRange = getRangeForBuffer(bufferID);
    assert((fullRange.getByteLength() == 0 ||
            getVirtualFile(end.getAdvancedLoc(-1))) &&
           "no open virtual file for this location");
    assert(fullRange.getEnd() == end);
#endif
    return;
  }
  CachedVFile = {nullptr, nullptr};

  CharSrcRange oldRange = virtualFile->Range;
  virtualFile->Range = CharSrcRange(*this, virtualFile->Range.getStart(), end);
  VirtualFiles[end.Value.getPointer()] = std::move(*virtualFile);

  bool existed = VirtualFiles.erase(oldRange.getEnd().Value.getPointer());
  assert(existed);
  (void)existed;
}

const SrcMgr::VirtualFile *SrcMgr::getVirtualFile(SrcLoc Loc) const {
  const char *p = Loc.Value.getPointer();

  if (CachedVFile.first == p)
    return CachedVFile.second;

  // Returns the first element that is >p.
  auto VFileIt = VirtualFiles.upper_bound(p);
  if (VFileIt != VirtualFiles.end() && VFileIt->second.Range.contains(Loc)) {
    CachedVFile = {p, &VFileIt->second};
    return CachedVFile.second;
  }

  return nullptr;
}

std::optional<unsigned>
SrcMgr::getIDForBufferIdentifier(StringRef BufIdentifier) const {
  auto It = BufIdentIDMap.find(BufIdentifier);
  if (It == BufIdentIDMap.end())
    return std::nullopt;
  return It->second;
}

StringRef SrcMgr::getIdentifierForBuffer(unsigned bufferID) const {
  auto *buffer = llvmSrcMgr.getMemoryBuffer(bufferID);
  assert(buffer && "invalid buffer ID");
  return buffer->getBufferIdentifier();
}

CharSrcRange SrcMgr::getRangeForBuffer(unsigned bufferID) const {
  auto *buffer = llvmSrcMgr.getMemoryBuffer(bufferID);
  SrcLoc start{llvm::SMLoc::getFromPointer(buffer->getBufferStart())};
  return CharSrcRange(start, buffer->getBufferSize());
}

unsigned SrcMgr::getLocOffsetInBuffer(SrcLoc Loc, unsigned BufferID) const {
  assert(Loc.isValid() && "location should be valid");
  auto *Buffer = llvmSrcMgr.getMemoryBuffer(BufferID);
  assert(Loc.Value.getPointer() >= Buffer->getBuffer().begin() &&
         Loc.Value.getPointer() <= Buffer->getBuffer().end() &&
         "Location is not from the specified buffer");
  return Loc.Value.getPointer() - Buffer->getBuffer().begin();
}

unsigned SrcMgr::getByteDistance(SrcLoc Start, SrcLoc End) const {
  assert(Start.isValid() && "start location should be valid");
  assert(End.isValid() && "end location should be valid");
#ifndef NDEBUG
  unsigned BufferID = findBufferContainingLoc(Start);
  auto *Buffer = llvmSrcMgr.getMemoryBuffer(BufferID);
  assert(End.Value.getPointer() >= Buffer->getBuffer().begin() &&
         End.Value.getPointer() <= Buffer->getBuffer().end() &&
         "End location is not from the same buffer");
#endif
  // When we have a rope buffer, could be implemented in terms of
  // getLocOffsetInBuffer().
  return End.Value.getPointer() - Start.Value.getPointer();
}

StringRef SrcMgr::getEntireTextForBuffer(unsigned BufferID) const {
  return llvmSrcMgr.getMemoryBuffer(BufferID)->getBuffer();
}

StringRef SrcMgr::extractText(CharSrcRange Range,
                              std::optional<unsigned> BufferID) const {
  assert(Range.isValid() && "range should be valid");

  if (!BufferID)
    BufferID = findBufferContainingLoc(Range.getStart());
  StringRef Buffer = llvmSrcMgr.getMemoryBuffer(*BufferID)->getBuffer();
  return Buffer.substr(getLocOffsetInBuffer(Range.getStart(), *BufferID),
                       Range.getByteLength());
}

std::optional<unsigned>
SrcMgr::findBufferContainingLocInternal(SrcLoc Loc) const {
  assert(Loc.isValid());
  // Search the buffers back-to front, so later alias buffers are
  // visited first.
  auto less_equal = std::less_equal<const char *>();
  for (unsigned i = llvmSrcMgr.getNumBuffers(), e = 1; i >= e; --i) {
    auto Buf = llvmSrcMgr.getMemoryBuffer(i);
    if (less_equal(Buf->getBufferStart(), Loc.Value.getPointer()) &&
        // Use <= here so that a pointer to the null at the end of the buffer
        // is included as part of the buffer.
        less_equal(Loc.Value.getPointer(), Buf->getBufferEnd()))
      return i;
  }
  return std::nullopt;
}

unsigned SrcMgr::findBufferContainingLoc(SrcLoc Loc) const {
  auto Id = findBufferContainingLocInternal(Loc);
  if (Id.has_value())
    return *Id;
  llvm_unreachable("no buffer containing location found");
}

bool SrcMgr::isOwning(SrcLoc Loc) const {
  return findBufferContainingLocInternal(Loc).has_value();
}

llvm::SMDiagnostic SrcMgr::GetMessage(SrcLoc Loc,
                                      llvm::SourceMgr::DiagKind Kind,
                                      const Twine &Msg,
                                      ArrayRef<llvm::SMRange> Ranges,
                                      ArrayRef<llvm::SMFixIt> FixIts,
                                      bool EmitMacroExpansionFiles) const {

  // First thing to do: find the current buffer containing the specified
  // location to pull out the source line.
  SmallVector<std::pair<unsigned, unsigned>, 4> ColRanges;
  std::pair<unsigned, unsigned> LineAndCol;
  StringRef BufferID = "stone:";
  std::string LineStr;

  if (Loc.isValid()) {
    BufferID = getDisplayNameForLoc(Loc /* TODO: , EmitMacroExpansionFiles*/);
    auto CurMB = GetLLVMSrcMgr().getMemoryBuffer(findBufferContainingLoc(Loc));

    // Scan backward to find the start of the line.
    const char *LineStart = Loc.Value.getPointer();
    const char *BufStart = CurMB->getBufferStart();
    while (LineStart != BufStart && LineStart[-1] != '\n' &&
           LineStart[-1] != '\r')
      --LineStart;

    // Get the end of the line.
    const char *LineEnd = Loc.Value.getPointer();
    const char *BufEnd = CurMB->getBufferEnd();
    while (LineEnd != BufEnd && LineEnd[0] != '\n' && LineEnd[0] != '\r')
      ++LineEnd;
    LineStr = std::string(LineStart, LineEnd);

    // Convert any ranges to column ranges that only intersect the line of the
    // location.
    for (unsigned i = 0, e = Ranges.size(); i != e; ++i) {
      llvm::SMRange R = Ranges[i];
      if (!R.isValid())
        continue;

      // If the line doesn't contain any part of the range, then ignore it.
      if (R.Start.getPointer() > LineEnd || R.End.getPointer() < LineStart)
        continue;

      // Ignore pieces of the range that go onto other lines.
      if (R.Start.getPointer() < LineStart)
        R.Start = llvm::SMLoc::getFromPointer(LineStart);
      if (R.End.getPointer() > LineEnd)
        R.End = llvm::SMLoc::getFromPointer(LineEnd);

      // Translate from SMLoc ranges to column ranges.
      // FIXME: Handle multibyte characters.
      ColRanges.push_back(std::make_pair(R.Start.getPointer() - LineStart,
                                         R.End.getPointer() - LineStart));
    }

    LineAndCol = getPresumedLineAndColumnForLoc(Loc);
  }

  return llvm::SMDiagnostic(GetLLVMSrcMgr(), Loc.Value, BufferID,
                            LineAndCol.first, LineAndCol.second - 1, Kind,
                            Msg.str(), LineStr, ColRanges, FixIts);
}

void SrcRange::widen(SrcRange Other) {
  if (Other.Start.Value.getPointer() < Start.Value.getPointer())
    Start = Other.Start;
  if (Other.End.Value.getPointer() > End.Value.getPointer())
    End = Other.End;
}

void SrcLoc::printLineAndColumn(raw_ostream &OS, const SrcMgr &SM,
                                unsigned BufferID) const {
  if (isInvalid()) {
    OS << "<invalid loc>";
    return;
  }

  auto LineAndCol = SM.getPresumedLineAndColumnForLoc(*this, BufferID);
  OS << "line:" << LineAndCol.first << ':' << LineAndCol.second;
}

void SrcLoc::print(raw_ostream &OS, const SrcMgr &SM,
                   unsigned &LastBufferID) const {
  if (isInvalid()) {
    OS << "<invalid loc>";
    return;
  }

  unsigned BufferID = SM.findBufferContainingLoc(*this);
  if (BufferID != LastBufferID) {
    OS << SM.getIdentifierForBuffer(BufferID);
    LastBufferID = BufferID;
  } else {
    OS << "line";
  }

  auto LineAndCol = SM.getPresumedLineAndColumnForLoc(*this, BufferID);
  OS << ':' << LineAndCol.first << ':' << LineAndCol.second;
}

void SrcLoc::dump(const SrcMgr &SM) const { print(llvm::errs(), SM); }

void SrcRange::print(raw_ostream &OS, const SrcMgr &SM, unsigned &LastBufferID,
                     bool PrintText) const {
  // FIXME: CharSrcRange is a half-open character-based range, while
  // SrcRange is a closed token-based range, so this conversion omits the
  // last token in the range. Unfortunately, we can't actually get to the end
  // of the token without using the Lex library, which would be a layering
  // violation. This is still better than nothing.
  CharSrcRange(SM, Start, End).print(OS, SM, LastBufferID, PrintText);
}

void SrcRange::dump(const SrcMgr &SM) const { print(llvm::errs(), SM); }

CharSrcRange::CharSrcRange(const SrcMgr &SM, SrcLoc Start, SrcLoc End)
    : Start(Start) {
  assert(Start.isValid() == End.isValid() &&
         "Start and end should either both be valid or both be invalid!");
  if (Start.isValid())
    ByteLength = SM.getByteDistance(Start, End);
}

void CharSrcRange::print(raw_ostream &OS, const SrcMgr &SM,
                         unsigned &LastBufferID, bool PrintText) const {
  OS << '[';
  Start.print(OS, SM, LastBufferID);
  OS << " - ";
  getEnd().print(OS, SM, LastBufferID);
  OS << ']';

  if (Start.isInvalid() || getEnd().isInvalid())
    return;

  if (PrintText) {
    OS << " RangeText=\"" << SM.extractText(*this) << '"';
  }
}

CharSrcRange CharSrcRange::ToCharSrcRange(SrcMgr &sm, SrcRange range,
                                          Tokenable &tokenable) {
  return CharSrcRange(sm, range.Start,
                      tokenable.GetLocForEndOfToken(sm, range.End));
}
CharSrcRange CharSrcRange::ToCharSrcRange(SrcMgr &sm, SrcLoc start,
                                          SrcLoc end) {
  return CharSrcRange(sm, start, end);
}

void CharSrcRange::dump(const SrcMgr &SM) const { print(llvm::errs(), SM); }

std::optional<unsigned> SrcMgr::resolveOffsetForEndOfLine(unsigned BufferId,
                                                          unsigned Line) const {
  return resolveFromLineCol(BufferId, Line, ~0u);
}

std::optional<unsigned> SrcMgr::getLineLength(unsigned BufferId,
                                              unsigned Line) const {
  auto BegOffset = resolveFromLineCol(BufferId, Line, 0);
  auto EndOffset = resolveFromLineCol(BufferId, Line, ~0u);
  if (BegOffset && EndOffset) {
    return EndOffset.value() - BegOffset.value();
  }
  return std::nullopt;
}

std::optional<unsigned> SrcMgr::resolveFromLineCol(unsigned BufferId,
                                                   unsigned Line,
                                                   unsigned Col) const {
  if (Line == 0) {
    return std::nullopt;
  }
  const bool LineEnd = Col == ~0u;
  auto InputBuf = GetLLVMSrcMgr().getMemoryBuffer(BufferId);
  const char *Ptr = InputBuf->getBufferStart();
  const char *End = InputBuf->getBufferEnd();
  const char *LineStart = Ptr;
  --Line;
  for (; Line && (Ptr < End); ++Ptr) {
    if (*Ptr == '\n') {
      --Line;
      LineStart = Ptr + 1;
    }
  }
  if (Line != 0) {
    return std::nullopt;
  }
  Ptr = LineStart;
  if (Col == 0) {
    return Ptr - InputBuf->getBufferStart();
  }
  // The <= here is to allow for non-inclusive range end positions at EOF
  for (;; ++Ptr) {
    --Col;
    if (Col == 0)
      return Ptr - InputBuf->getBufferStart();
    if (*Ptr == '\n' || Ptr == End) {
      if (LineEnd) {
        return Ptr - InputBuf->getBufferStart();
      } else {
        break;
      }
    }
  }
  return std::nullopt;
}

unsigned SrcMgr::getExternalSourceBufferId(StringRef Path) {
  auto It = BufIdentIDMap.find(Path);
  if (It != BufIdentIDMap.end()) {
    return It->getSecond();
  }
  unsigned Id = 0u;
  auto InputFileOrErr = stone::vfs::getFileOrSTDIN(*getFileSystem(), Path);
  if (InputFileOrErr) {
    // This assertion ensures we can look up from the map in the future when
    // using the same Path.
    assert(InputFileOrErr.get()->getBufferIdentifier() == Path);
    Id = addNewSourceBuffer(std::move(InputFileOrErr.get()));
  }
  return Id;
}

SrcLoc SrcMgr::getLocFromExternalSource(StringRef Path, unsigned Line,
                                        unsigned Col) {
  auto BufferId = getExternalSourceBufferId(Path);
  if (BufferId == 0u)
    return SrcLoc();
  auto Offset = resolveFromLineCol(BufferId, Line, Col);
  if (!Offset.has_value())
    return SrcLoc();
  return getLocForOffset(BufferId, *Offset);
}