#ifndef STONE_CORE_CONTEXT_H
#define STONE_CORE_CONTEXT_H

#include "stone/Core/DiagnosticEngine.h"
#include "stone/Core/DiagnosticOptions.h"
#include "stone/Core/FileMgr.h"
#include "stone/Core/FileSystemOptions.h"
#include "stone/Core/SrcMgr.h"
#include "stone/Core/StatisticEngine.h"
#include "stone/Core/SystemOptions.h"

#include "llvm/Support/Debug.h"

namespace stone {

enum Kind : bool { Ok = false, Err = true };

template <typename T> class Result final {
  T v;
  bool err;

public:
  Result() : v(Get()), err(true) {}
  Result(T v) : v(v), err(false) {}

public:
  bool IsErr() { return err; }
  T Get() const { return v; }
  template <typename A> A *GetAs() { return static_cast<A *>(Get()); }
};

class Context final {

  FileMgr fm;
  SrcMgr sm;
  SystemOptions systemOpts;
  DiagnosticOptions diagOpts;
  DiagnosticEngine de;
  StatisticEngine se;
  ColorOutputStream cos;
  FileSystemOptions fsOpts;

public:
  Context();
  ~Context();

public:
  ColorOutputStream &Out() { return cos; }

  DiagnosticEngine &GetDiagEngine() { return de; }
  const DiagnosticEngine &GetDiagEngine() const { return de; }

  StatisticEngine &GetStatEngine() { return se; }
  const StatisticEngine &GeStatEngine() const { return se; }

  SystemOptions &GetSystemOptions() { return systemOpts; }
  const SystemOptions &GetSystemOptions() const { return systemOpts; }

  DiagnosticOptions &GetDiagOptions() { return diagOpts; }
  const DiagnosticOptions &GetDiagOptions() const { return diagOpts; }

  FileSystemOptions &GetFileSystemOptions() { return fsOpts; }
  const FileSystemOptions &GetFileSystemOptions() const { return fsOpts; }

  FileMgr &GetFileMgr() { return fm; }
  SrcMgr &GetSrcMgr() { return sm; }

  bool HasError() { return de.HasError(); }

  /// TODO: Something to think about
  void AddDiagnosticListener(DiagnosticListener *listener);

public:
  InFlightDiagnostic Printd(SrcLoc loc, const Diagnostic &diagnostic) {
    return de.Printd(loc, diagnostic);
  }

  InFlightDiagnostic Printd(SrcLoc loc, DiagID diagID,
                            llvm::ArrayRef<diag::Argument> args) {
    return de.Printd(loc, diagID, args);
  }

  InFlightDiagnostic Printd(SrcLoc loc, DiagID diagID) {
    return de.Printd(loc, diagID);
  }
  template <typename... ArgTypes>
  InFlightDiagnostic
  Printd(SrcLoc loc, Diag<ArgTypes...> id,
         typename detail::PassArgument<ArgTypes>::type... args) {

    return de.Printd(loc, id, std::forward<ArgTypes>(args)...);
  }
};
void Panic();
void Panic(const char *msg);
} // namespace stone
#endif
