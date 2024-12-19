#include "TableGenBackend.h" // Declares all backends.

#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/PrettyStackTrace.h"
#include "llvm/Support/Signals.h"
#include "llvm/TableGen/Error.h"
#include "llvm/TableGen/Main.h"
#include "llvm/TableGen/Record.h"

using namespace llvm;
using namespace stone;

enum ActionType {
  GenDiagnosticDefs,
  GenDiagnosticGroups,
  GenDiagnosticIndexName,
  GenDiagnosticDocs,
};

namespace {
cl::opt<ActionType> Action(
    cl::desc("Action to perform:"),
    cl::values(clEnumValN(GenDiagnosticDefs, "gen-stone-diags-defs",
                          "Generate Stone diagnostics definitions"),
               clEnumValN(GenDiagnosticGroups, "gen-stone-diag-groups",
                          "Generate Stone diagnostic groups"),
               clEnumValN(GenDiagnosticIndexName, "gen-stone-diags-index-name",
                          "Generate Stone diagnostic name index"),
               clEnumValN(GenDiagnosticDocs, "gen-diag-docs",
                   "Generate diagnostic documentation")));


cl::opt<std::string>
StoneComponent("stone-component",
               cl::desc("Only use warnings from specified component"),
               cl::value_desc("component"), cl::Hidden);



bool StoneTableGenMain(raw_ostream &OS, RecordKeeper &Records) {
  switch (Action) {
  case GenDiagnosticDefs:
    EmitDiagnosticDefs(Records, OS, StoneComponent);
    break;
  case GenDiagnosticGroups:
    EmitDiagnosticGroups(Records, OS);
    break;
  case GenDiagnosticIndexName:
    EmitDiagnosticIndexName(Records, OS);
    break;
   case GenDiagnosticDocs:
    EmitDiagnosticDocs(Records, OS);
    break;
  }
  return false;
}
} // end namespace

int main(int argc, char **argv) {
  sys::PrintStackTraceOnErrorSignal(argv[0]);
  PrettyStackTraceProgram X(argc, argv);
  cl::ParseCommandLineOptions(argc, argv);

  llvm_shutdown_obj Y;

  return TableGenMain(argv[0], &StoneTableGenMain);
}
