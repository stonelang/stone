#include "stone/Compile/Compile.h"

#include "llvm/Support/CommandLine.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/Process.h"
#include "llvm/Support/Program.h"
#include "llvm/Support/StringSaver.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/TargetParser/Host.h"
#include "llvm/TargetParser/Triple.h"

using namespace stone;

class DiagnosticEngine;

// class DiagnosticInvocation final {
// public:
//   DiagnosticInvocation() {}
// };

// class DiagnosticFormatOptions {

// public:
//   bool NotifyWhenIngnoringDiagnostic = false;

// public:
//   DiagnosticFormatOptions() {}
// };

// class DiagnosticInstance final {

// public:
//   /// We exclude ignore because if you are here, you require processing.
//   enum class Kind {
//     Fatal,
//     Error,
//     Warning,
//     Remark,
//     Note,
//   };
//   DiagnosticInstance() {}

// public:
//   void PrintDiagnostic(DiagnosticClient &printer, DiagnosticEngine
//   &callback);
// };

// class DiagnosticFormatter {

// public:
//   enum class Style {
//     Stone,
//     LLVM,
//   };

// public:
//   DiagnosticFormatter() {}
// };

// class TextDiagnosticFormatter : public DiagnosticFormatter {
// public:
//   TextDiagnosticFormatter() {}
// };

// class DiagnosticClient {

// public:
//   virtual void HandleDiagnostic(const DiagnosticInstance &instance,
//                                 DiagnosticEngine &callback) = 0;

//   /// \returns true if an error occurred while finishing-up.
//   virtual bool FinishProcessing() { return false; }

//   /// Flush any in-flight diagnostics.
//   virtual void FlushDiagnostic() {}

//   /// A custom diagnostic formatter to use
//   virtual DiagnosticFormatter *GetDiagnosticFormatter() { return nullptr; }

//   bool HasDiagnosticFormatter() { return GetDiagnosticFormatter() != nullptr;
//   }
// };

// class TextDiagnosticClient : public DiagnosticClient {
//   std::unique_ptr<TextDiagnosticFormatter> formatter;

// public:
//   TextDiagnosticClient() : formatter(new TextDiagnosticFormatter()) {}

// public:
//   void HandleDiagnostic(const DiagnosticInstance &instance,
//                         DiagnosticEngine &callback) override {}

//   DiagnosticFormatter *GetDiagnosticFormatter() override {
//     return formatter.get();
//   }
// };

// class DiagnosticState {
// public:
//   unsigned TotalErrorCount = 0;
//   unsigned TotalWarningCount = 0;

// public:
//   DiagnosticState() {}
//   ~DiagnosticState() { Reset(); }
//   void Reset() {}
// };

// class DiagnosticEngine {
// public:
//   enum class Level {
//     Fatal,
//     Error,
//     Warning,
//     Remark,
//     Note,
//     Ignore,
//   };

// public:
//   void Diagnose() {}

// private:
//   // DiagnosticInstance *
//   // ConstructDiagnosticInstance(DiagnosticInvocation *invocation) {}
//   void DelayEmitDiagnostic() {}

// public:
// };

int main(int argc, const char **args) { return 0; }