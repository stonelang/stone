#ifndef STONE_CONTEXT_H
#define STONE_CONTEXT_H

#include "stone/Basic/Error.h"
#include "stone/Basic/Result.h"
#include "stone/Basic/FileMgr.h"
#include "stone/Basic/FileSystemOptions.h"
#include "stone/Basic/LangOptions.h"
#include "stone/Basic/SrcMgr.h"
#include "stone/Basic/StatisticEngine.h"
#include "stone/Diag/DiagUnit.h"

namespace stone {

class LangContext final {
  FileMgr fm;
  SrcMgr sm;
  DiagUnit du; 
  LangOptions langOpts;
  StatisticEngine se;
  ColorfulStream cos;
  FileSystemOptions fsOpts;

public:
  
  LangContext() : fm(fsOpts), du(sm), cos(llvm::outs()) {}
  ~LangContext() {}
  
public:
  ColorfulStream &Out() { return cos; }

  StatisticEngine &GetStatEngine() { return se; }
  const StatisticEngine &GeStatEngine() const { return se; }

  LangOptions &GetLangOptions() { return langOpts; }
  const LangOptions &GetLangOptions() const { return langOpts; }

  FileSystemOptions &GetFileSystemOptions() { return fsOpts; }
  const FileSystemOptions &GetFileSystemOptions() const { return fsOpts; }

  DiagUnit& GetDiagUnit() { return du; }

  FileMgr &GetFileMgr() { return fm; }
  SrcMgr &GetSrcMgr() { return sm; }
};
} // namespace stone
#endif
