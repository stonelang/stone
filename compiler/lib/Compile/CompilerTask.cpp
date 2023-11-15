#include "stone/Compile/CompilerTask.h"
#include "stone/Basic/Mem.h"
#include "stone/Compile/Compiler.h"

using namespace stone;

// bool CompilerTask::IsCompilable() const {
//   switch (kind) {
//   case PreParse:
//   case Parse:
//   case ResolveImports:
//   case DumpSyntax:
//   case TypeCheck:
//   case DumpTypeInfo:
//   case PrintSyntax:
//   case PrintIR:
//   case PreEmit:
//   case EmitIRBefore:
//   case EmitIRAfter:
//   case EmitLibrary:
//   case InitModule:
//   case EmitModule:
//   case EmitBC:
//   case EmitAssembly:
//   case EmitObject:
//   case MergeModules:
//     return true;
//   default:
//     return false;
//   }
// }

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

// PreEmit
PreEmitTask *PreEmitTask::Create(const Compiler &compiler) {
  return new (compiler) PreEmitTask();
}
void PreEmitTask::Print(ColorStream &stream) {}

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
