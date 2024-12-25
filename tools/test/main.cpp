#include "stone/Basic/About.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/LLVMInit.h"
#include "stone/Diag/DiagnosticEngine.h"
#include "stone/Diag/DiagnosticID.h"
#include "stone/Diag/DiagnosticParseKind.h"
#include "stone/Diag/TextDiagnosticPrinter.h"
#include "stone/Support/Statistics.h"

using namespace stone;



// struct TextSlice {

// }
// struct TextState {
//   unsigned CurArgIndex = 0;
//   unsigned TotalArgIndex = 0;
//   llvm::SmallVector<TextSlice> Slices;
// };

// struct TextParser{
//   const char *CurPtr;
//   const char *TailPtr;

//   void Consume() {
//       CurPtr++;
//   }
//   void Parse(const char *CurPtr, const char *TailPtr){

//       wile(CurPtr != TailPtr){


//       }
//   }
//   void Parse(StringRef Text) {
//     Parse(Text.begin(), Text.end());
//   }


// };

// struct TextFormatter {

//   void Format(llvm::StringRef Text){

//   }
//   void Format(TextState ){

//       wile(CurPtr != TailPtr){


//       }
//   }
// };


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
