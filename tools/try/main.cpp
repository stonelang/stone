#include <assert.h>

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

// class DiagnosticEngine {};
// class Decl {};

// class DiagnosticArgument {
//   DiagnosticArgumentType ty;

// public:
//   DiagnosticArgument(DiagnosticArgumentType ty) : ty(ty) {}

// public:
//   DiagnosticArgumentType GetType() { return ty; }
// };

// template <typename T>
// class ComplexDiagnosticArgument : public DiagnosticArgument {
//   const T *val;

// public:
//   ComplexDiagnosticArgument(const T *val)
//       : DiagnosticArgument(DiagnosticArgumentType::Complex), val(val) {}

//   T *GetVal() const { return val; }

// public:
//   virtual void Diagnose(DiagnosticEngine &de) {}
// };

// class DeclDiagnosticArgument : public ComplexDiagnosticArgument<Decl> {
// public:
//   DeclDiagnosticArgument(const Decl *val) : ComplexDiagnosticArgument(val) {}

// public:
//   void Diagnose(DiagnosticEngine &de) override {}
// };

// void F0(DiagnosticArgument *argument) {

//   if (argument->GetType() == DiagnosticArgumentType::Complex) {

//     ComplexDiagnosticArgument *complex = (ComplexDiagnosticArgument
//     *)argument;
//   }
// }

struct DiagArg {
public:
};
enum class DiagID : uint32_t;

// template <typename... argTypes> struct Diag {
//   /// The diagnostic ID corresponding to this diagnostic.
//   DiagID diagID;
// };

// template <typename T> struct DiagWithArguments;

// template <typename... argTypes> struct DiagWithArguments<void(argTypes...)> {
//   typedef Diag<argTypes...> type;
// };

template <typename... argTypes> struct Diag {
  /// The diagnostic ID corresponding to this diagnostic.
  DiagID diagID;
};

template <typename T> struct DiagWithArguments;

struct DiagWithArguments {
  typedef Diag<DiagArg...> type;
};

struct Argument {};
template <typename T> struct InFlightArgument : public Argument {
  explicit InFlightArgument(const T val) : val(val) {}
} struct DeclArgument : public InFlightArgument {
}

ComplexArgumentType {
  Syntax
}
struct ComplexArgument : public Argument {};

int main() {

  // Decl *d = new Decl();
  // DeclDiagnosticArgument arg(d);

  // delete d;

  // F0(&arg);

  return 0;
}
