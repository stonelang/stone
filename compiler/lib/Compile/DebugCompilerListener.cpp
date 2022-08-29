#include "stone/Compile/DebugCompilerListener.h"

using namespace stone;

DebugCompilerListener::DebugCompilerListener() {}

void DebugCompilerListener::OnCompileConfigured(
    CompilerInvocation &invocation) {}

void DebugCompilerListener::OnCompileStarted(CompilerInstance &invocation) {}

void DebugCompilerListener::OnSyntaxAnalysisCompleted(
    CompilerInstance &invocation) {}

void DebugCompilerListener::OnSemanticAnalysisCompleted(
    CompilerInstance &invocation) {}

void DebugCompilerListener::OnCompileCompleted(CompilerInstance &invocation) {}