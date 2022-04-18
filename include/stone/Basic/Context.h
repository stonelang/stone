#ifndef STONE_BASIC_CONTEXT_H
#define STONE_BASIC_CONTEXT_H

#include "stone/Basic/DiagnosticEngine.h"
#include "stone/Basic/DiagnosticOptions.h"
#include "stone/Basic/FileMgr.h"
#include "stone/Basic/FileSystemOptions.h"
#include "stone/Basic/SrcMgr.h"
#include "stone/Basic/StatisticEngine.h"
#include "stone/Basic/SystemOptions.h"

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

enum class ErrorStatus : uint8_t {
  None = 0,
  Error,
};

template <typename T> class SafeResult final {
  ErrorStatus status;
  std::unique_ptr<T> ty;

public:
  SafeResult() : status(ErrorStatus::Error) {}
  SafeResult(std::unique_ptr<T> ty)
      : ty(std::move(ty)), status(ErrorStatus::None) {}

public:
  ErrorStatus GetStatus() { return status; }
  T &GetRef() const { return *ty; }
  template <typename A> A *GetAs() { return static_cast<A *>(ty.get()); }
};

struct alignas(uint8_t) Error final {
private:
  bool err;
  Error &operator=(const Error &other) = delete;

public:
  Error() : err(false) {}
  Error(bool err) : err(err) {}
  bool Has() { return err; }
};

class Context final {
  FileMgr fm;
  SrcMgr sm;
  SystemOptions systemOpts;
  DiagnosticOptions diagOpts;
  DiagnosticEngine de;
  StatisticEngine se;
  ColorfulStream cos;
  FileSystemOptions fsOpts;

public:
  Context();
  ~Context();

  /// The set of cleanups to be called when the Context is destroyed.
  // std::vector<std::function<void(void)>> cleanups;

public:
  ColorfulStream &Out() { return cos; }

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
void Panic();
void Panic(const char *msg);
} // namespace stone
#endif
