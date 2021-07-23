#ifndef STONE_BASIC_DIAGNOSTICENGINEINTERFACE_H
#define STONE_BASIC_DIAGNOSTICENGINEINTERFACE_H

#include "llvm/ADT/StringRef.h"

namespace stone {
/// Enumeration describing all of possible diagnostics.
///
/// Each of the diagnostics described in DiagnosticEngine.def has an entry in
/// this enumeration type that uniquely identifies it.
// enum class DiagID : uint32_t;
// enum class FixID : uint32_t;

// class DiagnosticArgumentBase {
// public:
//   DiagnosticArgumentBase() {}
//   virtual ~DiagnosticArgumentBase() {}

// public:
// };

// class DiagnosticListenerBase {
// public:
//   DiagnosticListenerBase() {}
//   virtual ~DiagnosticListenerBase() {}

// public:
// };
class DiagnosticEngineBase {
// public:
//   DiagnosticEngineBase() {}
//   virtual ~DiagnosticEngineBase() {}

public:
  //virtual void DelayDiagnostic(DiagID diagID) = 0;

  // virtual bool HasError() = 0;

  // /// Specify a limit for the number of errors we should
  // /// emit before giving up.
  // ///
  // /// Zero disables the limit.
  // virtual void SetErrorLimit(unsigned limit) = 0;

  // /// When set to true, any unmapped warnings are ignored.
  // ///
  // /// If this and WarningsAsErrors are both set, then this one wins.
  // virtual void SetIgnoreAllWarnings(bool status) = 0;
  // virtual bool GetIgnoreAllWarnings() const = 0;

  // /// Get the actual string in the ".def" for the diagnostic
  // virtual llvm::StringRef GetDiagIDString(const DiagID diagID,
  //                                         bool printDiagnosticName) = 0;

  // /// Add an additional DiagnosticListener to receive diagnostics.
  // virtual void AddListener(DiagnosticListenerBase &listener) = 0;

  // /// Remove a specific DiagnosticListener.
  // virtual void RemoveListener(DiagnosticListenerBase &listener) = 0;

public:
  // virtual void Print(ColorOutputStream &os,
  //                    const PrintingPolicy *policy) const override;
};

} // namespace stone

#endif