#include "stone/Compile/InFlightMode.h"
#include "stone/Basic/Ret.h"

using namespace stone;

InFlightMode::InFlightMode(Compiler &compiler) : compiler(compiler) {}

SyntaxInFlightMode::SyntaxInFlightMode(Compiler &compiler)
    : InFlightMode(compiler) {}

ParseInFlightMode::ParseInFlightMode(Compiler &compiler)
    : SyntaxInFlightMode(compiler) {}

int ParseInFlightMode::Execute() { return ret::ok; }

TypeCheckInFlightMode::TypeCheckInFlightMode(Compiler &compiler)
    : ParseInFlightMode(compiler) {}

int TypeCheckInFlightMode::Execute() {
  ParseInFlightMode::Execute();
  return ret::ok;
}

EmitIRInFlightMode::EmitIRInFlightMode(Compiler &compiler)
    : TypeCheckInFlightMode(compiler) {}

int EmitIRInFlightMode::Execute() {
  TypeCheckInFlightMode::Execute();
  return ret::ok;
}

EmitModuleInFlightMode::EmitModuleInFlightMode(Compiler &compiler)
    : EmitIRInFlightMode(compiler) {}

int EmitModuleInFlightMode::Execute() {
  TypeCheckInFlightMode::Execute();
  return ret::ok;
}

EmitBitCodeInFlightMode::EmitBitCodeInFlightMode(Compiler &compiler)
    : EmitIRInFlightMode(compiler) {}

int EmitBitCodeInFlightMode::Execute() {
  EmitIRInFlightMode::Execute();
  return ret::ok;
}

EmitObjectInFlightMode::EmitObjectInFlightMode(Compiler &compiler)
    : EmitIRInFlightMode(compiler) {}

int EmitObjectInFlightMode::Execute() {
  EmitIRInFlightMode::Execute();
  return ret::ok;
}

// EmitAssemblyInFlightMode::EmitAssemblyInFlightMode() {}
// int EmitAssemblyInFlightMode::Execute() { return ret::ok; }

// EmitLibraryInFlightMode::EmitLibraryInFlightMode() {}
// int EmitLibraryInFlightMode::Execute() { return ret::ok; }
