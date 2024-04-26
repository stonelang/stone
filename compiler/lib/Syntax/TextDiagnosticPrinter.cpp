#include "stone/Basic/SrcMgr.h"
#include "stone/Basic/Streaming.h"
#include "stone/Syntax/Diagnostics.h"

using namespace stone;

// These must come after the declaration of AnnotatedSourceSnippet due to the
// `currentSnippet` member.
TextDiagnosticPrinter::TextDiagnosticPrinter(llvm::raw_ostream &stream)
    : Stream(stream) {}

TextDiagnosticPrinter::~TextDiagnosticPrinter() {}

// MARK: LLVM style diagnostic printing
void TextDiagnosticPrinter::printDiagnostic(SrcMgr &SM,
                                            const DiagnosticInfo &Info) {
  // Determine what kind of diagnostic we're emitting.
  llvm::SourceMgr::DiagKind SMKind;
  switch (Info.Kind) {
  case DiagnosticKind::Error:
    SMKind = llvm::SourceMgr::DK_Error;
    break;
  case DiagnosticKind::Warning:
    SMKind = llvm::SourceMgr::DK_Warning;
    break;

  case DiagnosticKind::Note:
    SMKind = llvm::SourceMgr::DK_Note;
    break;

  case DiagnosticKind::Remark:
    SMKind = llvm::SourceMgr::DK_Remark;
    break;
  }

  // Translate ranges.
  SmallVector<llvm::SMRange, 2> Ranges;
  for (auto R : Info.Ranges) {
    Ranges.push_back(getRawRange(SM, R));
  }

  // Translate fix-its.
  SmallVector<llvm::SMFixIt, 2> FixIts;
  for (DiagnosticInfo::FixIt F : Info.FixIts) {
    FixIts.push_back(getRawFixIt(SM, F));
  }

  // Display the diagnostic.
  ColorfulStream colorfulErrs{Stream};
  raw_ostream &out = ForceColors ? colorfulErrs : Stream;
  const llvm::SourceMgr &rawSM = SM.GetLLVMSrcMgr();

  // Actually substitute the diagnostic arguments into the diagnostic text.
  llvm::SmallString<256> Text;
  {
    llvm::raw_svector_ostream Out(Text);
    DiagnosticEngine::formatDiagnosticText(Out, Info.FormatString,
                                           Info.FormatArgs);
  }
  auto Msg = SM.GetMessage(Info.Loc, SMKind, Text, Ranges, FixIts, EmitMacroExpansionFiles);
  rawSM.PrintMessage(out, Msg, ForceColors);
}

void TextDiagnosticPrinter::handleDiagnostic(SrcMgr &SM,
                                             const DiagnosticInfo &Info) {}

void TextDiagnosticPrinter::flush(bool includeTrailingBreak) {}

bool TextDiagnosticPrinter::finishProcessing() {
  // If there's an in-flight snippet, flush it.
  flush(false);
  return false;
}

llvm::SMDiagnostic
SrcMgr::GetMessage(SrcLoc Loc, llvm::SourceMgr::DiagKind Kind,
                          const Twine &Msg, ArrayRef<llvm::SMRange> Ranges,
                          ArrayRef<llvm::SMFixIt> FixIts,
                          bool EmitMacroExpansionFiles) const {

  // First thing to do: find the current buffer containing the specified
  // location to pull out the source line.
  SmallVector<std::pair<unsigned, unsigned>, 4> ColRanges;
  std::pair<unsigned, unsigned> LineAndCol;
  StringRef BufferID = "stone:";
  std::string LineStr;

  if (Loc.isValid()) {
    BufferID = getDisplayNameForLoc(Loc/* TODO: , EmitMacroExpansionFiles*/);
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
