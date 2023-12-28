#ifndef STONE_DRIVER_DRIVER_H
#define STONE_DRIVER_DRIVER_H

#include "stone/Basic/FileMgr.h"
#include "stone/Basic/FileSystemOptions.h"
#include "stone/Basic/LangOptions.h"
#include "stone/Basic/Mem.h"
#include "stone/Basic/Status.h"
#include "stone/Diag/DiagnosticEngine.h"
#include "stone/Driver/DriverOptions.h"
#include "stone/Stats/Stats.h"

namespace stone {

class Driver final {

  DriverOptions driverOpts;
  LangOptions langOpts;
  FileSystemOptions fileSystemOpts;

  FileMgr fileMgr;
  SrcMgr srcMgr;
  DiagnosticEngine diags{srcMgr};

  std::unique_ptr<llvm::opt::OptTable> optTable;
  std::unique_ptr<MemoryContext> memContext;

public:
  Driver(const Driver &) = delete;
  void operator=(const Driver &) = delete;
  Driver(Driver &&) = delete;
  void operator=(Driver &&) = delete;

public:
  Driver();

public:
  std::unique_ptr<InputArgList>
  ParseCommandLine(llvm::ArrayRef<const char *> args);

  std::unique_ptr<llvm::opt::DerivedArgList>
  TranslateInputArgList(const InputArgList &ial);

public:
  DriverOptions &GetDriverOptions() { return driverOpts; }
  const DriverOptions &GetDriverOptions() const { return driverOpts; }
  Status ParseDriverOptions(const ArgList &args);

  llvm::opt::OptTable &GetOptTable() { return *optTable; }

  FileSystemOptions &GetFileSystemOptions() { return fileSystemOpts; }
  const FileSystemOptions &GetFileSystemOptions() const {
    return fileSystemOpts;
  }

  LangOptions &GetLangOptions() { return langOpts; }
  const LangOptions &GetLangOptions() const { return langOpts; }

public:
  MemoryContext &GetMemoryContext() { return *memContext; }
  const MemoryContext &GetMemoryContext() const { return *memContext; }
  bool HasMemoryContext() const { return memContext != nullptr; }

public:
  void AddDiagnosticConsumer(DiagnosticConsumer &consumer) {
    diags.AddConsumer(consumer);
  }
  void RemoveDiagnosticConsumer(DiagnosticConsumer &consumer) {
    diags.RemoveConsumer(consumer);
  }
  DiagnosticEngine &GetDiags() { return diags; }

public:
};

} // namespace stone
#endif