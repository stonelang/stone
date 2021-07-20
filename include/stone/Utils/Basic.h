#ifndef STONE_UTILS_UTILS_H
#define STONE_UTILS_UTILS_H

#include "stone/Utils/DiagnosticEngine.h"
#include "stone/Utils/DiagnosticOptions.h"
#include "stone/Utils/FileMgr.h"
#include "stone/Utils/FileSystemOptions.h"
#include "stone/Utils/Host.h"
#include "stone/Utils/LangOptions.h"
#include "stone/Utils/Stats.h"

namespace stone {
class Basic {
protected:
  LangOptions langOpts;
  DiagnosticOptions diagOpts;
  DiagnosticEngine de;
  StatEngine se;
  ColorOutputStream cos;
  FileMgr fm;
  Host host;
  FileSystemOptions fsOpts;
  /// Default target triple.
  std::string targetTriple;

public:
  Basic();
  ~Basic();

public:
  ColorOutputStream &Out() { return cos; }

  DiagnosticEngine &GetDiagEngine() { return de; }
  const DiagnosticEngine &GetDiagEngine() const { return de; }

  StatEngine &GetStatEngine() { return se; }
  const StatEngine &GeStatEngine() const { return se; }

  LangOptions &GetLangOptions() { return langOpts; }
  const LangOptions &GetLangOptions() const { return langOpts; }

  DiagnosticOptions &GetDiagOptions() { return diagOpts; }
  const DiagnosticOptions &GetDiagOptions() const { return diagOpts; }

  Host &GetHost() { return host; }
  const Host &GetHost() const { return host; }

  FileMgr &GetFileMgr() { return fm; }
  std::string GetTarget() { return targetTriple; }

  bool HasError() { return de.HasError(); }

  void Panic();

public:
  InFlightDiagnostic Diagnose(SrcLoc loc, const Diagnostic &diagnostic) {
    return de.Diagnose(loc, diagnostic);
  }

  InFlightDiagnostic Diagnose(SrcLoc loc, DiagID diagID,
                              llvm::ArrayRef<diag::Argument> args) {
    return de.Diagnose(loc, diagID, args);
  }

  InFlightDiagnostic Diagnose(SrcLoc loc, DiagID diagID) {
    return de.Diagnose(loc, diagID);
  }
  template <typename... ArgTypes>
  InFlightDiagnostic
  Diagnose(SrcLoc loc, Diag<ArgTypes...> id,
           typename detail::PassArgument<ArgTypes>::type... args) {

    return de.Diagnose(loc, id, std::forward<ArgTypes>(args)...);
  }
};

} // namespace stone
#endif
