#ifndef STONE_BASIC_DIAGNOSABLE_H
#define STONE_BASIC_DIAGNOSABLE_H

#include <assert.h>

namespace stone {

class DiagnosticEngine;
enum { DiagnosableAlingment = 8 };

class alignas(DiagnosableAlingment) Diagnosable {
public:
  ~Diagnosable() {}

public:
  // TODO: Think about passing DiagID
  virtual void Diagnose(DiagnosticEngine &de) {
    assert(true && "Caller must override.");
  }
};

// enum class DiagnosticArgumentType {
//   /// std::string
//   STDStr,

//   /// const char *
//   CStr,

//   /// llvm::StringRef
//   LLVMStr,

//   /// int
//   SInt,

//   /// unsigned
//   UInt,

//   TokenType,

//   /// custom argument
//   Complex,

// };

// template <typename T> class DiagnosticArgument {
//   T val;
//   DiagnosticArgumentType ty;

// public:
//   DiagnosticArgument(DiagnosticArgumentType ty, T val) : ty(ty), val(val) {}

// public:
//   T GetValue() { return val; }
//   DiagnosticArgumentType GetType() { return ty; }
// };

// template<typename T>
// class SimpleDiagnosticArgument : public DiagnosticArgument {
//   T val;
// public:
//   SimpleDiagnosticArgument(T val) : val(val) {}

// public:

// };

// class DiagnosticArgument {
//   DiagnosticArgumentType ty;

// public:
//   DiagnosticArgument(DiagnosticArgumentType ty) : ty(ty) {}

// public:
//   DiagnosticArgumentType GetType() { return ty; }
// };
// class SignedIntDiagnosticArgument final : public DiagnosticArgument {
//   const int val;

// public:
//   SignedIntDiagnosticArgument(const int val)
//       : DiagnosticArgument(DiagnosticArgumentType::SInt), val(val) {}

// public:
//   int GetVal() const { return val; }
// };

// class UnsignedIntDiagnosticArgument final : public DiagnosticArgument {
//   unsigned val;

// public:
//   UnsignedIntDiagnosticArgument(const unsigned val)
//       : DiagnosticArgument(DiagnosticArgumentType::UInt), val(val) {}

// public:
//   unsigned GetVal() const { return val; }
// };

// class ComplexDiagnosticArgument : public DiagnosticArgument {
//   const void *val;

// public:
//   ComplexDiagnosticArgument(const void *val)
//       : DiagnosticArgument(DiagnosticArgumentType::Complex) {}

//   void *GetVal() const { return val; }

// public:
//   virtual void Diagnose(DiagnosticEngine &de) {}
// };

// class DeclDiagnosticArgument : public CustomDiagnosticArgument {
// public:
//   DeclDiagnosticArgument(const Decl *val) : val(val) {}

// public:
//   void Diagnose(DiagnosticEngine &de) override {}
// };

} // namespace stone

#endif