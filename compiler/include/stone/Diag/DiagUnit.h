#ifndef STONE_DIAG_DIAGUNIT_H
#define STONE_DIAG_DIAGUNIT_H

#include "stone/Diag/DiagnosticEngine.h"
#include "stone/Diag/DiagnosticOptions.h"

#include "llvm/Support/Debug.h"

namespace stone {

class DiagUnit final {
  SrcMgr &sm;
  DiagnosticOptions diagOpts;
  DiagnosticEngine de;

public:
  DiagUnit(SrcMgr &sm);
  ~DiagUnit();

public:
  DiagnosticEngine &GetDiagEngine() { return de; }
  const DiagnosticEngine &GetDiagEngine() const { return de; }

  DiagnosticOptions &GetDiagOptions() { return diagOpts; }
  const DiagnosticOptions &GetDiagOptions() const { return diagOpts; }

  SrcMgr &GetSrcMgr() { return sm; }
  bool HasError() { return de.HasError(); }

public:
  InFlightDiagnostic PrintD(const Diagnostic &diagnostic) {
    return PrintD(SrcLoc(), diagnostic);
  }
  InFlightDiagnostic PrintD(SrcLoc loc, const Diagnostic &diagnostic) {
    return de.PrintD(loc, diagnostic);
  }

  InFlightDiagnostic PrintD(DiagID diagID,
                            llvm::ArrayRef<diag::Argument> args) {
    return PrintD(SrcLoc(), diagID, args);
  }
  InFlightDiagnostic PrintD(SrcLoc loc, DiagID diagID,
                            llvm::ArrayRef<diag::Argument> args) {
    return de.PrintD(loc, diagID, args);
  }

  InFlightDiagnostic PrintD(DiagID diagID) { return PrintD(SrcLoc(), diagID); }

  InFlightDiagnostic PrintD(SrcLoc loc, DiagID diagID) {
    return de.PrintD(loc, diagID);
  }
  template <typename... ArgTypes>
  InFlightDiagnostic
  PrintD(SrcLoc loc, Diag<ArgTypes...> id,
         typename detail::PassArgument<ArgTypes>::type... args) {
    return de.PrintD(loc, id, std::forward<ArgTypes>(args)...);
  }
};
} // namespace stone
#endif
