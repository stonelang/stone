#include "stone/Core/TextDiagnosticEmitter.h"
#include "stone/Core/Diagnostic.h"

using stone::TextDiagnosticEmitter;

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

void TextDiagnosticEmitter::EmitDiagnostic(const DiagnosticEvent &diagnostic) {

  auto ed = const_cast<DiagnosticEvent &>(diagnostic);
  printf("%s", ed.GetFormatMessage().data());

  // formatter.FormatText()

  // Display the diagnostic.
  // ColorefStream coloredErrs{Stream};
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