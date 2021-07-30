#include <assert.h>
#include <stdio.h>

#include <memory>

// class Compilable {
// public:
//   virtual void CompileFile() = 0;
// };

// class SyntaxConsumer {
// public:
//   virtual void Listen(int i) = 0;
// };

// class SyntaxParsing : public Compilable {
//   SyntaxConsumer *consumer = nullptr;

// public:
//   SyntaxParsing(SyntaxConsumer *consumer = nullptr);

// public:
//   void CompileFile() override;
// };

// SyntaxParsing::SyntaxParsing(SyntaxConsumer *consumer) : consumer(consumer)
// {}

// void SyntaxParsing::CompileFile() {
//   consumer->Listen(1);
// }

// class TypeChecking : public SyntaxParsing, public SyntaxConsumer {
// public:
//   TypeChecking() : SyntaxParsing(this) {}

// public:
//   void CompileFile() override;
//   void Listen(int i) override;
// };

// void TypeChecking::CompileFile() {

//   SyntaxParsing::CompileFile();
// }

// void TypeChecking::Listen(int i) {
//   printf("%d\n", i);
// }

// class CompilableLisenter {
// public:
// };

// class SyntaxListener : public CompilableLisenter {
// public:
// };

// class Compilable {

// protected:
//   Compilable *dep = nullptr;
//   CompilableLisenter *listener = nullptr;

// public:
//   Compilable() {}

// public:
//   virtual void CompileFile() = 0;

// public:
//   void SetDep(Compilable *dep);
//   void SetListener(CompilableLisenter *listener);
// };

// void Compilable::SetDep(Compilable *dep) { this->dep = dep; }
// void Compilable::SetListener(CompilableLisenter *listener) {
//   this->listener = listener;
// }

// class SyntaxParsing : public Compilable {
// public:
//   SyntaxParsing();

//   void CompileFile() override;

// public:
//   virtual void Listen(SyntaxFile* syntaxFile) {}
// };

// SyntaxParsing::SyntaxParsing() {}

// void SyntaxParsing::CompileFile() {}

// class TypeChecking : public Compilable, public SyntaxParsing {
//   std::unique_ptr<SyntaxParsing> syntaxParsing;

// public:
//   TypeChecking();
// };

// TypeChecking::TypeChecking() {
//   syntaxParsing = std::make_unique<SyntaxParsing>();
//   syntaxParsing->SetListener(this);
//   SetDep(syntaxParsing.get());
// }

// class SyntaxListener {
// public:
//   virtual void Listen(int i) {}
// };

// class SyntaxParsing : public Compilable {
//   SyntaxConsumer *consumer = nullptr;

// public:
//   SyntaxParsing(SyntaxConsumer *consumer = nullptr);

// public:
//   void CompileFile() override;

//   virtual void Listen(int i) {}
// };

// SyntaxParsing::SyntaxParsing(SyntaxConsumer *consumer) : consumer(consumer)
// {}

// void SyntaxParsing::CompileFile() { consumer->Listen(1); }

// class TypeChecking : public Compilable, public SyntaxListener {
//   Compilable *dep = nullptr;
// public:
//   TypeChecking(Compilable *dep) : dep(dep) { dep->SetListener(this); }

// public:
//   void CompileFile() override;
//   void Listen(int i) override;
// };

// void TypeChecking::CompileFile() { dep->CompileFile(); }

// void TypeChecking::Listen(int i) { printf("%d\n", i); }

//

/*
class SyntaxFile {};

class Compilable {
public:
  virtual void CompileFile() = 0;
};

class SyntaxParsing : public Compilable {
  std::unique_ptr<SyntaxParsing> listener;
public:
  SyntaxParsing() {}
public:
  void CompileFile() override {
    listener->SyntaxFileCreated(nullptr);
  }
  void SetListener(std::unique_ptr<SyntaxParsing> sp) {
    listener = std::move(sp);
  }

public:
  virtual void SyntaxFileCreated(SyntaxFile *sf){}
};

class TypeChecking : public SyntaxParsing {

public:
  void CompileFile() override {

  }

public:
  // SyntaxParsingListener
  void SyntaxFileCreated(SyntaxFile *sf) override {
  }

public:
  // TypeCheckingListener
  virtual void SyntaxFileTypeChecked(SyntaxFile *sf) {}
};

*/

// SafeList<CompilableItem> cis;
// void AddCompilable(std::unique_ptr<CompilableItem> ci) {
//  cis.Add(std::move(ci));
//}
int main() { return 0; }
