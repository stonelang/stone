#include "stone/Driver/Main.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/LLVMInit.h"
#include "stone/Basic/MainExecutablePath.h"
#include "stone/Diag/DriverDiagnostic.h"
#include "stone/Diag/TextDiagnosticConsumer.h"
#include "stone/Diag/TextDiagnosticFormatter.h"
#include "stone/Driver/Compilation.h"
#include "stone/Driver/Driver.h"

using namespace stone;

int stone::Main(llvm::ArrayRef<const char *> args, const char *arg0,
                void *mainAddr) {

  llvm::PrettyStackTraceString crashInfo("Driver construction...");

  FINISH_LLVM_INIT();

  return 0;
}
