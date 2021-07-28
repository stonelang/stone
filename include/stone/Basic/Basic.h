#ifndef STONE_BASIC_BASIC_H
#define STONE_BASIC_BASIC_H

#include "stone/Basic/DiagnosticEngine.h"
#include "stone/Basic/DiagnosticOptions.h"
#include "stone/Basic/FileMgr.h"
#include "stone/Basic/FileSystemOptions.h"
#include "stone/Basic/Host.h"
#include "stone/Basic/LangOptions.h"
#include "stone/Basic/SrcMgr.h"
#include "stone/Basic/StatisticEngine.h"

namespace stone {
class Basic {

protected:
  LangOptions langOpts;
  DiagnosticOptions diagOpts;
  DiagnosticEngine de;
  StatisticEngine se;
  ColorOutputStream cos;
  FileMgr fm;
  SrcMgr sm;
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

  StatisticEngine &GetStatEngine() { return se; }
  const StatisticEngine &GeStatEngine() const { return se; }

  LangOptions &GetLangOptions() { return langOpts; }
  const LangOptions &GetLangOptions() const { return langOpts; }

  DiagnosticOptions &GetDiagOptions() { return diagOpts; }
  const DiagnosticOptions &GetDiagOptions() const { return diagOpts; }

  Host &GetHost() { return host; }
  const Host &GetHost() const { return host; }

  FileMgr &GetFileMgr() { return fm; }
  FileMgr &GetSrcMgr() { return fm; }

  std::string GetTarget() { return targetTriple; }

  bool HasError() { return de.HasError(); }
  void Panic();

public:
  unsigned MakeSrcID(llvm::StringRef filePath);

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
