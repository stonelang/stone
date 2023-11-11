#include "stone/Options/Action.h"

using namespace stone;
ActionKind action::GetActionKindByOptionID(const unsigned actionOptionID) {
  switch (actionOptionID) {
  case opts::Parse:
    return ActionKind::Parse;
  case opts::DumpAST:
    return ActionKind::DumpAST;
  case opts::TypeCheck:
    return ActionKind::TypeCheck;
  case opts::PrintAST:
    return ActionKind::PrintAST;
  case opts::PrintIR:
    return ActionKind::PrintIR;
  case opts::EmitIR:
    return ActionKind::EmitIR;
  case opts::EmitBC:
    return ActionKind::EmitBC;
  case opts::EmitObject:
    ActionKind::EmitObject;
  case opts::EmitAssembly:
    return ActionKind::EmitAssembly;
  case opts::EmitLibrary:
    return ActionKind::EmitLibrary;
  case opts::EmitModule:
    return ActionKind::EmitModule;
  case opts::PrintVersion:
    return ActionKind::PrintVersion;
  case opts::PrintHelp:
    return ActionKind::PrintVersion;
  default:
    return ActionKind::Alien;
  }
}