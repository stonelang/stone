#include "stone/Session/Session.h"
#include "stone/Diag/CoreDiagnostic.h"
#include "stone/Public.h"
#include "stone/Session/Options.h"
#include "llvm/Option/Option.h"

using namespace stone;
using namespace stone::opts;
using namespace llvm::opt;

Session::Session(llvm::StringRef programName, llvm::StringRef programPath)
    : programName(programName), programPath(programPath),
      optst(stone::opts::CreateOptTable()) {
  CreateTimer();
}

Session::~Session() {}

void Session::CreateTimer() {
  // timerGroup =
  //     std::make_unique<llvm::TimerGroup>(GetSessionName(), GetSessionDesc());
  // timer = std::make_unique<llvm::Timer>(GetSessionName(), GetSessionDesc(),
  //                                       *timerGroup);
  // timer->startTimer();
}

std::unique_ptr<llvm::opt::InputArgList>
Session::ParseArgs(llvm::ArrayRef<const char *> args) {

  auto ial = std::make_unique<llvm::opt::InputArgList>(
      GetOpts().ParseArgs(args, missingArgIndex, missingArgCount,
                          includedFlagsBitmask, excludedFlagsBitmask));
  assert(ial && "No input argument list.");
  // Check for missing argument error.
  if (missingArgCount) {
    GetLangContext().GetDiags().PrintD(
        SrcLoc(), diag::err_missing_arg_value,
        diag::LLVMStr(ial->getArgString(missingArgIndex)),
        diag::UInt(missingArgCount));
    return nullptr;
  }
  // Check for unknown arguments.
  for (const llvm::opt::Arg *arg : ial->filtered(opts::UNKNOWN)) {
    GetLangContext().GetDiags().PrintD(SrcLoc(), diag::err_unknown_arg,
                                       diag::LLVMStr(arg->getAsString(*ial)));
    return nullptr;
  }
  return ial;
}

llvm::StringRef Session::ComputeWorkDir(const llvm::opt::InputArgList &ial) {
  if (auto *arg = ial.getLastArg(opts::WorkDir)) {
    llvm::SmallString<128> smallStr;
    smallStr = arg->getValue();
    llvm::sys::fs::make_absolute(smallStr);
    return smallStr.str();
  }
  return llvm::StringRef();
}

stone::Result<std::string>
Session::GetOptEqualValue(opts::OptID optID,
                          const llvm::opt::InputArgList &ial) {
  if (ial.hasArg(optID)) {
    auto arg = ial.getLastArg(optID);
    if (arg) {
      return stone::Result<std::string>(arg->getValue());
    }
  }
  return stone::Result<std::string>();
}

void Session::PrintHelp(raw_ostream &stream, const llvm::opt::OptTable &opts) {}

void Session::PrintArg(raw_ostream &OS, const char *Arg, StringRef TempDir) {
  const bool Escape = std::strpbrk(Arg, "\"\\$ ");

  if (!TempDir.empty()) {
    llvm::SmallString<256> ArgPath{Arg};
    llvm::sys::fs::make_absolute(ArgPath);
    llvm::sys::path::native(ArgPath);

    llvm::SmallString<256> TempPath{TempDir};
    llvm::sys::fs::make_absolute(TempPath);
    llvm::sys::path::native(TempPath);

    if (StringRef(ArgPath).startswith(TempPath)) {
      // Don't write temporary file names in the debug info. This would prevent
      // incremental llvm compilation because we would generate different IR on
      // every compiler invocation.
      Arg = "<temporary-file>";
    }
  }

  if (!Escape) {
    OS << Arg;
    return;
  }
  // Quote and escape. This isn't really complete, but good enough.
  OS << '"';
  while (const char c = *Arg++) {
    if (c == '"' || c == '\\' || c == '$')
      OS << '\\';
    OS << c;
  }
  OS << '"';
}

void Session::PrintVersion() {}
void Session::PrintTimer() {}
void Session::PrintDiagnostics() {}
void Session::PrintStatistics() {}
