#include "stone/Compile/DebugFrontendListener.h"

using namespace stone;

DebugFrontendListener::DebugFrontendListener() {}

void DebugFrontendListener::OnCompileConfigured(Frontend &frontend) {}

void DebugFrontendListener::OnCompileStarted(Frontend &frontend) {}

void DebugFrontendListener::OnSyntaxAnalysisCompleted(Frontend &frontend) {}

void DebugFrontendListener::OnSemanticAnalysisCompleted(Frontend &frontend) {}

void DebugFrontendListener::OnCompileCompleted(Frontend &frontend) {}