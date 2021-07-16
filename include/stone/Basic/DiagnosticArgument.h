#ifndef STONE_BASIC_DIAGNOSTICARGUMENT_H
#define STONE_BASIC_DIAGNOSTICARGUMENT_H

#include "stone/Basic/TokenType.h"
#include "llvm/ADT/StringRef.h"

#include <assert.h>

namespace stone {

class DiagnosticEngine;

enum class DiagnosticArgumentType {
  /// std::string
  STDStr,

  /// const char *
  CStr,

  /// llvm::StringRef
  LLVMStr,

  /// int
  SInt,

  /// unsigned
  UInt,

  TokenType,

  /// custom argument
  Complex,

};

struct DiagnosticArgument {
  DiagnosticArgumentType ty;

public:
  DiagnosticArgument(DiagnosticArgumentType ty) : ty(ty) {}

public:
  DiagnosticArgumentType GetType() { return ty; }
};

struct STDStrDiagnosticArgument final : public DiagnosticArgument {
  const std::string val;

public:
  STDStrDiagnosticArgument(const std::string val)
      : DiagnosticArgument(DiagnosticArgumentType::STDStr), val(val) {}

public:
  std::string GetVal() const { return val; }
};

struct LLVMStrDiagnosticArgument final : public DiagnosticArgument {
  const std::string val;

public:
  LLVMStrDiagnosticArgument(const llvm::StringRef val)
      : DiagnosticArgument(DiagnosticArgumentType::LLVMStr), val(val) {}

public:
  std::string GetVal() const { return val; }
};

struct CStrDiagnosticArgument final : public DiagnosticArgument {
  const char *val;

public:
  CStrDiagnosticArgument(const char *val)
      : DiagnosticArgument(DiagnosticArgumentType::CStr), val(val) {}

public:
  const char *GetVal() const { return val; }
};

struct SIntDiagnosticArgument final : public DiagnosticArgument {
  const int val;

public:
  SIntDiagnosticArgument(const int val)
      : DiagnosticArgument(DiagnosticArgumentType::SInt), val(val) {}

public:
  int GetVal() const { return val; }
};

struct UIntDiagnosticArgument final : public DiagnosticArgument {
  unsigned val;

public:
  UIntDiagnosticArgument(const unsigned val)
      : DiagnosticArgument(DiagnosticArgumentType::UInt), val(val) {}

public:
  unsigned GetVal() const { return val; }
};

struct TokenTypeDiagnosticArgument final : public DiagnosticArgument {
  tk::Type val;

public:
  TokenTypeDiagnosticArgument(const tk::Type val)
      : DiagnosticArgument(DiagnosticArgumentType::TokenType), val(val) {}

public:
  tk::Type GetVal() const { return val; }
};

struct ComplexDiagnosticArgument : public DiagnosticArgument {
  const void *val;

public:
  ComplexDiagnosticArgument(const void *val)
      : DiagnosticArgument(DiagnosticArgumentType::Complex) {}

  const void *GetVal() const { return val; }

public:
  virtual void Diagnose(DiagnosticEngine &de) const {}
};

} // namespace stone

#endif