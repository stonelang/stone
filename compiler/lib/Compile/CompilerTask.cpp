#include "stone/Compile/CompilerTask.h"
#include "stone/Basic/Mem.h"
#include "stone/Compile/Compiler.h"

using namespace stone;

CompilerTask *Compiler::GetTask(ActionKind source) {
  switch (kind) {
  case ActionKind::PrintHelp:
  case ActionKind::PrintHelpHidden:
    return PrintHelpTask::Create(*this);
  case ActionKind::PrintVersion:
    return PrintVersionTask::Create(*this);
  case ActionKind::Parse:
    return ParseTask::Create(*this);
  case ActionKind::ResolveImports:
    return ResolveImportsTask::Create(*this);
  case ActionKind::DumpSyntax:
    return DumpSyntaxTask::Create(*this);
  case ActionKind::TypeCheck:
    return TypeCheckTask::Create(*this);
  case ActionKind::PrintSyntax:
    return PrintSyntaxTask::Create(*this);
  case ActionKind::MergeModules:
    return MergeModulesTask::Create(*this);
  case ActionKind::EmitIRBefore:
    return EmitIRBeforeTask::Create(*this);
  case ActionKind::EmitIRAfter:
    return EmitIRAfterTask::Create(*this);
  case ActionKind::EmitBC:
    return EmitBitCodeTask::Create(*this);
  case ActionKind::EmitObject:
    return EmitObjectTask::Create(*this));
  case ActionKind::DumpTypeInfo:
    return DumpTypeInfoTask::Create(*this);
  default:
    llvm_unreachable("Unknown action -- cannot compile!");
  }
}

PrintHelpTask *PrintHelpTask::Create(const Compiler &compiler) {
  return new (compiler) PrintHelpTask();
}

PrintVersionTask *PrintVersionTask::Create(const Compiler &compiler) {
  return new (compiler) PrintVersionTask();
}

// Parse
ParseTask *ParseTask::Create(const Compiler &compiler) {
  return new (compiler) ParseTask();
}
void ParseTask::Setup(Compiler &compiler) {
  // Maybe here you can perform pre-parse work
}

void ParseTask::Print(ColorStream &stream) { stream << "Parser Task" << '\n'; }

// ResolveImports
ResolveImportsTask *ResolveImportsTask::Create(const Compiler &compiler) {
  return new (compiler) ResolveImportsTask();
}
void ResolveImportsTask::Print(ColorStream &stream) {
  stream << "ResolveImport  Task" << '\n';
}

// TypeCheck
TypeCheckTask *TypeCheckTask::Create(const Compiler &compiler) {
  return new (compiler) TypeCheckTask();
}
void TypeCheckTask::Print(ColorStream &stream) {}

// EmitIRBefore
EmitIRBeforeTask *EmitIRBeforeTask::Create(const Compiler &compiler) {
  return new (compiler) EmitIRBeforeTask();
}
void EmitIRBeforeTask::Setup(Compiler &compiler) {

  // Call AbstractEmit.Setup(Compiler& compiler)
}
void EmitIRBeforeTask::Print(ColorStream &stream) {}

// EmitIRAfterTask
EmitIRAfterTask *EmitIRAfterTask::Create(const Compiler &compiler) {
  return new (compiler) EmitIRAfterTask();
}
void EmitIRAfterTask::Print(ColorStream &stream) {}

// EmitIRAfterTask
EmitObjectTask *EmitObjectTask::Create(const Compiler &compiler) {
  return new (compiler) EmitObjectTask();
}
void EmitObjectTask::Print(ColorStream &stream) {}
