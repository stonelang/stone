#ifndef STONE_PUBLIC_H
#define STONE_PUBLIC_H

#include "stone/Basic/Error.h"
#include "stone/Basic/FileMgr.h"
#include "stone/Basic/FileSystemOptions.h"
#include "stone/Basic/LangOptions.h"
#include "stone/Basic/Result.h"
#include "stone/Basic/SrcMgr.h"
#include "stone/Basic/StatisticEngine.h"
#include "stone/Diag/DiagUnit.h"

namespace llvm {
class Module;
class TargetOptions;
class TargetMachine;
} // namespace llvm

namespace stone {
class CodeGenOptions;
class CodeGenContext;
class TargetOptions;
class LangOptions;
class DiagnosticEngine;

namespace syn {
class SyntaxContext;
}

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

  DiagUnit &GetDiagUnit() { return du; }

  FileMgr &GetFileMgr() { return fm; }
  SrcMgr &GetSrcMgr() { return sm; }
};

std::unique_ptr<llvm::TargetMachine> CreateTargetMachine(CodeGenContext &cgc);

} // namespace stone
#endif
