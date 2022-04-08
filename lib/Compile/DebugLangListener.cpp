#include "stone/Compile/DebugLangListener.h"

using namespace stone;

DebugLangListener::DebugLangListener() {}

void DebugLangListener::OnCompileConfigured(LangInstance &lang) {}

void DebugLangListener::OnCompileStarted(LangInstance &lang) {}

void DebugLangListener::OnSyntaxAnalysisCompleted(LangInstance &lang) {}
void DebugLangListener::OnSemanticAnalysisCompleted(LangInstance &lang) {}

void DebugLangListener::OnCompileCompleted(LangInstance &lang) {}