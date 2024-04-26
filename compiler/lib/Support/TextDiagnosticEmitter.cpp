#include "stone/Support/TextDiagnosticEmitter.h"
#include "stone/Support/Diagnostic.h"

using namespace stone;

llvm::SMDiagnostic
stone::SrcMgr::GetMessage(stone::SrcLoc loc, llvm::SourceMgr::DiagKind kind,
                          const Twine &msg,
                          llvm::ArrayRef<llvm::SMRange> ranges,
                          llvm::ArrayRef<llvm::SMFixIt> fixIts) const {
  return llvm::SMDiagnostic();
}

TextDiagnosticEmitter::TextDiagnosticEmitter() {}

TextDiagnosticEmitter::~TextDiagnosticEmitter() {}

void TextDiagnosticEmitter::EmitLevel() {}

void TextDiagnosticEmitter::EmitDiagnostic(const DiagnosticMessage &de) {

  // auto ed = const_cast<DiagnosticMessage &>(de);
  // printf("%s\n", ed.GetFormatMessage().data());

  ColorStream cs;
  Format(cs, de.GetDiagnostic());

  // formatter.FormatText()

  // Display the diagnostic.
  // ColorStream coloredErrs{Stream};
  // raw_ostream &out = ForceColors ? coloredErrs : Stream;
  // const llvm::SourceMgr &llvmSM = ed.GetSrcMgr().GetLLVMSrcMgr();

  // // Actually substitute the diagnostic arguments into the diagnostic text.
  // llvm::SmallString<256> Text;
  // {
  //   llvm::raw_svector_ostream Out(Text);
  //   DiagnosticEngine::formatDiagnosticText(Out, Info.FormatString,
  //                                          Info.FormatArgs);
  // }

  // auto Msg = ed.GetSrcMgr().GetMessage(Info.Loc, SMKind, Text, Ranges,
  // FixIts); rawSM.PrintMessage(out, Msg, ForceColors);
}

void TextDiagnosticEmitter::EmitLoc() {}

void TextDiagnosticEmitter::Format(ColorStream &out,
                                   const Diagnostic &diagnostic,
                                   DiagnosticFormatOptions fmtOpts) {}

void TextDiagnosticEmitter::Format(ColorStream &out, llvm::StringRef text,
                                   llvm::ArrayRef<DiagnosticArgument> args,
                                   DiagnosticFormatOptions fmtOpts) {}

void TextDiagnosticEmitter::FormatArgument(
    ColorStream &out, llvm::StringRef modifier,
    llvm::StringRef modifierArguments, llvm::ArrayRef<DiagnosticArgument> args,
    unsigned argIndex, DiagnosticFormatOptions fmtOpts) {}
