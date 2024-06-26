#include "stone/Support/TextDiagnosticEmitter.h"
#include "stone/Support/Diagnostic.h"

using stone::TextDiagnosticEmitter;

TextDiagnosticEmitter::TextDiagnosticEmitter(TextDiagnosticFormatter &formatter)
    : DiagnosticEmitter(formatter) {}

TextDiagnosticEmitter::~TextDiagnosticEmitter() {}

void TextDiagnosticEmitter::EmitLevel() {}

void TextDiagnosticEmitter::EmitDiagnostic(const DiagnosticMessage &de) {

  // auto ed = const_cast<DiagnosticMessage &>(de);
  // printf("%s\n", ed.GetFormatMessage().data());

  ColorStream cs;
  GetFormatter().Format(cs, de.GetDiagnostic());

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