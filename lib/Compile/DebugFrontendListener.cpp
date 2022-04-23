#include "stone/Compile/DebugFrontendListener.h"

using namespace stone;

DebugFrontendListener::DebugFrontendListener() {}

void DebugFrontendListener::OnCompileConfigured(
    FrontendInstance &frontendInstance) {}

void DebugFrontendListener::OnCompileStarted(
    FrontendInstance &frontendInstance) {}

void DebugFrontendListener::OnSyntaxAnalysisCompleted(
    FrontendInstance &frontendInstance) {}

void DebugFrontendListener::OnSemanticAnalysisCompleted(
    FrontendInstance &frontendInstance) {}

void DebugFrontendListener::OnCompileCompleted(
    FrontendInstance &frontendInstance) {}