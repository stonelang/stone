#include "stone/Basic/About.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/LLVMInit.h"
#include "stone/Diag/DiagnosticEngine.h"
#include "stone/Diag/DiagnosticID.h"
#include "stone/Diag/DiagnosticParseKind.h"
#include "stone/Diag/TextDiagnosticPrinter.h"
#include "stone/Support/Statistics.h"

using namespace stone;

int main() {
 
  
  std::unique_ptr<TextDiagnosticPrinterImpl> DC(new TextDiagnosticPrinterImpl());

  SrcMgr SM;
  DiagnosticOptions DiagOpts;
  DiagnosticEngine DE(SM, DiagOpts);

  DE.AddClient(DC.get());
  DE.Diagnose(SrcLoc(),diag::error_duplicate_input_file, "test.stone");
  DE.FinishProcessing();

  return 0;

}
