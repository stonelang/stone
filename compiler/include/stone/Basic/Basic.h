#ifndef STONE_BASIC_BASIC_H
#define STONE_BASIC_BASIC_H

#include "stone/Basic/FileMgr.h"
#include "stone/Basic/FileSystemOptions.h"
#include "stone/Basic/LangOptions.h"
#include "stone/Basic/SrcMgr.h"
#include "stone/Basic/Status.h"

#include "stone/Basic/DiagnosticEngine.h"
#include "stone/Basic/StatisticEngine.h"

namespace stone {

class Basic final {
  FileMgr fm;
  SrcMgr sm;
  diag::DiagnosticOptions diagOpts;
  DiagnosticEngine de;
  LangOptions langOpts;
  StatisticEngine se;
  ColorStream cos;
  FileSystemOptions fsOpts;

public:
  Basic(const Basic &) = delete;
  void operator=(const Basic &) = delete;
  Basic(Basic &&) = delete;
  void operator=(Basic &&) = delete;

public:
  Basic() : fm(fsOpts), de(diagOpts, sm), cos(llvm::outs()) {}
  ~Basic() {}

public:
  ColorStream &Out() { return cos; }
  StatisticEngine &GetStats() { return se; }
  const StatisticEngine &GeStats() const { return se; }

  LangOptions &GetLangOptions() { return langOpts; }
  const LangOptions &GetLangOptions() const { return langOpts; }

  FileSystemOptions &GetFileSystemOptions() { return fsOpts; }
  const FileSystemOptions &GetFileSystemOptions() const { return fsOpts; }

  DiagnosticEngine &GetDiags() { return de; }

  FileMgr &GetFileMgr() { return fm; }
  SrcMgr &GetSrcMgr() { return sm; }
};

} // namespace stone

#endif