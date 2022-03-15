#ifndef STONE_COMPILE_SOURCEPROFILE_H
#define STONE_COMPILE_SOURCEPROFILE_H

#include "stone/Core/File.h"
#include "stone/Core/List.h"
#include "stone/Core/OutputFile.h"

#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/Path.h"

namespace stone {

class LangContext;
class alignas(8) SourceProfile final {

  bool isPrimary;
  bool hasOutput;
  const unsigned srcID;
  const file::File &input;

  // file::File *output = nullptr;
  /// TODO:
  // OutputFile outputFile;

public:
  SourceProfile(const unsigned srcID, const file::File &input)
      : srcID(srcID), input(input) {}
  ~SourceProfile();

public:
  bool IsPrimary() { return isPrimary; }
  bool HasOutput() { return hasOutput; }

  unsigned GetSrcID() const { return srcID; }
  const file::File &GetInput() const { return input; }

public:
  // Make vanilla new/delete illegal for Decls.
  void *operator new(size_t bytes) = delete;
  void operator delete(void *data) = delete;
  // Only allow allocation of Decls using the allocator in ASTContext
  // or by doing a placement new.
  void *operator new(std::size_t bytes,
                     unsigned alignment = alignof(SourceProfile));

public:
  static SourceProfile *Allocate(const unsigned srcID, const file::File &input,
                                 LangContext &lc);

  // file::File *GetOutput() { return output; }
  // void SetOutput(file::File *o) { output = o; };
};

} // namespace stone

// Defined in Compiler.h
void *operator new(size_t bytes, const stone::LangContext &lc,
                   size_t alignment = 8);

void *operator new[](size_t bytes, const stone::LangContext &lc,
                     size_t alignment = 8);

// It is good practice to pair new/delete operators.  Also, MSVC gives many
// warnings if a matching delete overload is not declared, even though the
// throw() spec guarantees it will not be implicitly called.
void operator delete(void *currPtr, const stone::LangContext &lc, size_t);

void operator delete[](void *currPtr, const stone::LangContext &lc, size_t);

#endif