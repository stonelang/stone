
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

class DiagnosticEngine {};
class Decl {};

class DiagnosticArgument {
  DiagnosticArgumentType ty;

public:
  DiagnosticArgument(DiagnosticArgumentType ty) : ty(ty) {}

public:
  DiagnosticArgumentType GetType() { return ty; }
};

template <typename T>
class ComplexDiagnosticArgument : public DiagnosticArgument {
  const T *val;

public:
  ComplexDiagnosticArgument(const T *val)
      : DiagnosticArgument(DiagnosticArgumentType::Complex), val(val) {}

  T *GetVal() const { return val; }

public:
  virtual void Diagnose(DiagnosticEngine &de) {}
};

class DeclDiagnosticArgument : public ComplexDiagnosticArgument<Decl> {
public:
  DeclDiagnosticArgument(const Decl *val) : ComplexDiagnosticArgument(val) {}

public:
  void Diagnose(DiagnosticEngine &de) override {}
};

void F0(DiagnosticArgument *argument) {

  if (argument->GetType() == DiagnosticArgumentType::Complex) {

    ComplexDiagnosticArgument *complex = (ComplexDiagnosticArgument *)argument;
  }
}
int main() {

  Decl *d = new Decl();
  DeclDiagnosticArgument arg(d);

  delete d;

  F0(&arg);

  return 0;
}
