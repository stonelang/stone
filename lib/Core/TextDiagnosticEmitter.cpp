#include "stone/Core/TextDiagnosticEmitter.h"
#include "stone/Core/Diagnostic.h"

using stone::TextDiagnosticEmitter;

TextDiagnosticEmitter::TextDiagnosticEmitter() {}

TextDiagnosticEmitter::~TextDiagnosticEmitter() {}

void TextDiagnosticEmitter::EmitLevel() {}

void TextDiagnosticEmitter::EmitDiagnostic(
    const EmissionDiagnostic &diagnostic) {

  auto ed = const_cast<EmissionDiagnostic &>(diagnostic);
  printf("%s", ed.GetFormatMessage().data());

  // Display the diagnostic.
  // ColorefStream coloredErrs{Stream};
  // raw_ostream &out = ForceColors ? coloredErrs : Stream;
  // const llvm::SourceMgr &rawSM = SM.getLLVMSourceMgr();

  // // Actually substitute the diagnostic arguments into the diagnostic text.
  // llvm::SmallString<256> Text;
  // {
  //   llvm::raw_svector_ostream Out(Text);
  //   DiagnosticEngine::formatDiagnosticText(Out, Info.FormatString,
  //                                          Info.FormatArgs);
  // }

  // auto Msg = SM.GetMessage(Info.Loc, SMKind, Text, Ranges, FixIts);
  // rawSM.PrintMessage(out, Msg, ForceColors);
}

void TextDiagnosticEmitter::EmitLoc() {}