#include "stone/Basic/Basic.h"
#include "llvm/Support/Host.h"

using namespace stone;

Basic::Basic()
    : fm(fsOpts), de(diagOpts), cos(llvm::outs()),
      targetTriple(llvm::sys::getDefaultTargetTriple()) {}

Basic::~Basic() {}

// InflightDiagnostic Basic::Diagnose(SrcLoc loc, const Diagnostic &diagnostic)
// {
//   return GetDiagEngine().Diagnose(loc, diagnostic);
// }

// InflightDiagnostic Basic::Diagnose(SrcLoc loc, DiagID diagID,
//                                    llvm::ArrayRef<DiagnosticArgument> args) {
//   return GetDiagEngine().Diagnose(loc, diagID, args);
// }

// template <typename... ArgTypes>
// InflightDiagnostic Basic::Diagnose(
//     SrcLoc loc, Diag<ArgTypes...> id,
//     typename detail::PassArgument<ArgTypes>::type... args) {

//   return GetDiagEngine().Diagnose(loc, id, std::forward<ArgTypes>(args)...);
// }

// template <typename... ArgTypes>
// InflightDiagnostic
// Basic::Diagnose(Diag<ArgTypes...> id,
//                 typename detail::PassArgument<ArgTypes>::type... args) {

//   return GetDiagEngine().Diagnose(SrcLoc(), id,
//                                   std::forward<ArgTypes>(args)...);
// }

void Basic::Panic() { assert(false && "Compiler cannot continue"); }

void Basic::Error(unsigned diagID) { Error(SrcLoc(), diagID); }
void Basic::Error(SrcLoc loc, unsigned diagID) {}

void Basic::Warn(unsigned diagID) { Warn(SrcLoc(), diagID); }
void Basic::Warn(SrcLoc loc, unsigned diagID) {}

void Basic::Note(unsigned diagID) { Note(SrcLoc(), diagID); }
void Basic::Note(SrcLoc loc, unsigned diagID) {}

void Basic::Remark(unsigned diagID) { Remark(SrcLoc(), diagID); }
void Basic::Remark(SrcLoc loc, unsigned diagID) {}
