#include "stone/Compile/Compiler.h"
#include "stone/Compile/CompilerInputFile.h"
#include "stone/Option/ActionKind.h"

using namespace stone;

Compiler::Compiler() : invocation(*this), execution(*this) {}

void Compiler::Setup() {
  SetupAction(invocation.GetCompilerOptions().MainAction.GetKind());

  // We do not setup the rest of the compiler if it is not compilable
  // For example, PrintHelp
  // if (!invocation.IsCompilable()) {
  //   return;
  // }

  execution.Setup();
}

void Compiler::SetupAction(ActionKind action) {
  switch (action) {
  case ActionKind::PrintHelp: {
    QueueAction(ActionKind::PrintHelp);
    break;
  case ActionKind::PrintHelpHidden:
    QueueAction(ActionKind::PrintHelpHidden);
    break;
  }
  case ActionKind::PrintVersion: {
    QueueAction(ActionKind::PrintVersion);
    break;
  }
  case ActionKind::Parse: {
    QueueAction(ActionKind::Parse);
    break;
  }
  case ActionKind::ResolveImports: {
    SetupAction(ActionKind::Parse);
    QueueAction(ActionKind::ResolveImports);
    break;
  }
  case ActionKind::DumpSyntax: {
    SetupAction(ActionKind::ResolveImports);
    QueueAction(ActionKind::DumpSyntax);
    break;
  }
  case ActionKind::TypeCheck: {
    SetupAction(ActionKind::ResolveImports);
    QueueAction(ActionKind::TypeCheck);
    break;
  }
  case ActionKind::PrintSyntax: {
    SetupAction(ActionKind::TypeCheck);
    QueueAction(ActionKind::PrintSyntax);
    break;
  }
  case ActionKind::MergeModules: {
    SetupAction(ActionKind::TypeCheck);
    QueueAction(ActionKind::MergeModules);
    break;
  }
  case ActionKind::EmitIRBefore: {
    SetupAction(ActionKind::TypeCheck);
    QueueAction(ActionKind::EmitIRBefore);
    break;
  }
  case ActionKind::EmitIRAfter: {
    SetupAction(ActionKind::TypeCheck);
    QueueAction(ActionKind::EmitIRAfter);
    break;
  }
  case ActionKind::EmitBC: {
    SetupAction(ActionKind::TypeCheck);
    QueueAction(ActionKind::EmitBC);
    break;
  }
  case ActionKind::EmitObject: {
    SetupAction(ActionKind::TypeCheck);
    QueueAction(ActionKind::EmitObject);
    break;
  }
  case ActionKind::DumpTypeInfo: {
    SetupAction(ActionKind::TypeCheck);
    QueueAction(ActionKind::DumpTypeInfo);
    break;
  }
  default: {
    llvm_unreachable("Unknown action -- cannot compile!");
  }
  }
}

// void Compiler::ForEachAction(std::function<Status(ActionKind kind)> notify) {
//   for (auto action : actions) {
//     notify(action);
//   }
// }
// void Compiler::QueueAction(ActionKind actionKind) {
//   actions.push_back(actionKind);
// }