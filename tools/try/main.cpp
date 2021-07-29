#include <assert.h>

class Compilable {

protected:
public:
  explicit Compilable() {}
  virtual ~Compilable() {}

public:
  int CompileFile(int i);

protected:
  virtual int CompileFile() = 0;
};

int Compilable::CompileFile(int i) { return CompileFile(); }

class SyntaxParsing final : public Compilable {

public:
  SyntaxParsing() {}

protected:
  int CompileFile() override;
};

int SyntaxParsing::CompileFile() { return 0; }

// class EmittingIR final : public Compilable {
//   SyntaxParsing syntaxParsing;

// public:
//   EmittingIR() {}

// protected:
//    int DoCompileFile();
// };

// int EmittingIR::DoCompileFile() {
//   syntaxParsing.CompileFile(1);
//   return 0;
// }

int main() {

  // Decl *d = new Decl();
  // DeclDiagnosticArgument arg(d);

  // delete d;

  // F0(&arg);

  SyntaxParsing syntaxParsing;

  syntaxParsing.CompileFile(1);

  return 0;
}
