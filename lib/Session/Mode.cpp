#include "stone/Session/Mode.h"
#include "stone/Basic/Context.h"
#include "stone/Session/Options.h"
#include "llvm/Option/Option.h"

using namespace stone;
using namespace stone::opts;
using namespace llvm::opt;

// static ModeKind ComputeModeKind(const llvm::opt::ArgList &args) {}

static ModeKind GetModeKind(const unsigned modeID) {
  // TODO: may have to claim
  switch (modeID) {
  case opts::Parse:
    return ModeKind::Parse;
  case opts::DumpSyntax:
    return ModeKind::DumpSyntax;
  case opts::TypeCheck:
    return ModeKind::TypeCheck;
  case opts::PrintSyntax:
    return ModeKind::PrintSyntax;
  case opts::EmitIR:
    return ModeKind::EmitIR;
  case opts::EmitBC:
    return ModeKind::EmitBC;
  case opts::EmitObject:
    ModeKind::EmitObject;
  case opts::EmitAssembly:
    return ModeKind::EmitAssembly;
  case opts::EmitLibrary:
    return ModeKind::EmitLibrary;
  case opts::EmitModule:
    return ModeKind::EmitModule;
  case opts::PrintVersion:
    return ModeKind::PrintVersion;
  case opts::PrintHelp:
    return ModeKind::PrintVersion;
  default:
    return ModeKind::Alien;
  }
}
std::unique_ptr<Mode> Mode::Create(const llvm::opt::InputArgList &ial) {
  auto modeArg = ial.getLastArg(opts::ModeGroup);
  if (modeArg) {
    auto modeKind = GetModeKind(modeArg->getOption().getID());
    if (modeKind == ModeKind::Alien) {
      return std::make_unique<Mode>(ModeKind::Alien);
    }
    return std::make_unique<Mode>(modeKind, modeArg->getOption().getName());
  } else {
    return std::make_unique<Mode>(ModeKind::None);
  }
}

// TODO
file::Type Mode::GetOutputFileType() { return file::Type::None; }
