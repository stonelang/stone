#include "stone/Compile/CompilerTask.h"
#include "stone/Basic/Mem.h"
#include "stone/Compile/Compiler.h"

using namespace stone;

CompilerTask *Compiler::GetTask(CompilerTaskKind kind) {
  switch (kind) {
  case CompilerTaskKind::PrintHelp:
  case CompilerTaskKind::PrintHelpHidden:
    return PrintHelpTask::Create(*this);
  case CompilerTaskKind::PrintVersion:
    return PrintVersionTask::Create(*this);
  case CompilerTaskKind::PreParse:
    return PreParseTask::Create(*this);
  case CompilerTaskKind::Parse:
    return ParseTask::Create(*this);
  case CompilerTaskKind::ResolveImports:
    return ResolveImportsTask::Create(*this);
  case CompilerTaskKind::DumpSyntax:
    return DumpSyntaxTask::Create(*this);
  case CompilerTaskKind::TypeCheck:
    return TypeCheckTask::Create(*this);
  case CompilerTaskKind::PrintSyntax:
    return PrintSyntaxTask::Create(*this);
  case CompilerTaskKind::MergeModules:
    return MergeModulesTask::Create(*this);
  case CompilerTaskKind::GenIR:
    return GenIRTask::Create(*this);
  case CompilerTaskKind::EmitIRBefore:
    return EmitIRBeforeTask::Create(*this);
  case CompilerTaskKind::EmitIRAfter:
    return EmitIRAfterTask::Create(*this);
  case CompilerTaskKind::EmitBC:
    return EmitBCTask::Create(*this);
  case CompilerTaskKind::EmitObject:
    return EmitObjectTask::Create(*this));
  case CompilerTaskKind::DumpTypeInfo:
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

// PreParse
PreParseTask *PreParseTask::Create(const Compiler &compiler) {
  return new (compiler) PreParseTask();
}
void PreParseTask::Print(ColorStream &stream) {}

// Parse
ParseTask *ParseTask::Create(const Compiler &compiler) {
  return new (compiler) ParseTask();
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

// Need to gen-ir before emitting or outputing native code
GenIRTask *GenIRTask::Create(const Compiler &compiler) {
  return new (compiler) GenIRTask();
}
void GenIRTask::Print(ColorStream &stream) {}

// EmitIRBefore
EmitIRBeforeTask *EmitIRBeforeTask::Create(const Compiler &compiler) {
  return new (compiler) EmitIRBeforeTask();
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

// Final Task
FinalTask *FinalTask::Create(const Compiler &compiler) {
  return new (compiler) FinalTask();
}
void FinalTask::Print(ColorStream &stream) { stream << "Final  Task" << '\n'; }
