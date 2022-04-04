#ifndef STONE_COMPILE_SOURCEUNIT_H
#define STONE_COMPILE_SOURCEUNIT_H

#include "stone/Basic/File.h"
#include "stone/Basic/List.h"
#include "stone/Basic/OutputFile.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/Path.h"

namespace stone {
class LangInvocation;
class alignas(8) SourceUnit final {
  friend class LangInstance;

  bool isPrimary;
  bool hasOutput;
  const unsigned srcID;
  const file::File &input;

  std::unique_ptr<OutputFile> outputFile;

public:
  SourceUnit(const unsigned srcID, const file::File &input)
      : srcID(srcID), input(input) {}
  ~SourceUnit();

public:
  bool IsPrimary() { return isPrimary; }
  bool HasOutput() { return hasOutput; }

  unsigned GetSrcID() const { return srcID; }
  const file::File &GetInput() const { return input; }
  OutputFile &GetOutputFile() { return *outputFile.get(); }

private:
  void SetOutputFile(std::unique_ptr<OutputFile> output) {
    outputFile = std::move(output);
  }

public:
  // Make vanilla new/delete illegal for Decls.
  void *operator new(size_t bytes) = delete;
  void operator delete(void *data) = delete;
  // Only allow allocation of Decls using the allocator in ASTContext
  // or by doing a placement new.
  void *operator new(std::size_t bytes,
                     unsigned alignment = alignof(SourceUnit));

public:
  static SourceUnit *Allocate(const unsigned srcID, const file::File &input,
                              LangInvocation &langInvocation);

  // file::File *GetOutput() { return output; }
  // void SetOutput(file::File *o) { output = o; };
};

} // namespace stone

void *operator new(size_t bytes, const stone::LangInvocation &langInvocation,
                   size_t alignment = 8);

void *operator new[](size_t bytes, const stone::LangInvocation &langInvocation,
                     size_t alignment = 8);

// It is good practice to pair new/delete operators.  Also, MSVC gives many
// warnings if a matching delete overload is not declared, even though the
// throw() spec guarantees it will not be implicitly called.
void operator delete(void *currPtr, const stone::LangInvocation &langInvocation,
                     size_t);

void operator delete[](void *currPtr,
                       const stone::LangInvocation &langInvocation, size_t);

#endif