#ifndef STONE_DRIVER_DRIVER_COMPILATION_ENTITY_H
#define STONE_DRIVER_DRIVER_COMPILATION_ENTITY_H

#include "stone/Basic/FileType.h"
#include "stone/Basic/OptionSet.h"
#include "stone/Driver/DriverAllocation.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/TinyPtrVector.h"
#include "llvm/Support/TrailingObjects.h"

namespace stone {
class Job;
class Driver;
class JobConstruction;
class Compilation;
class CompilationEntity;

enum class CompilationEntityKind : uint8_t {
  Input = 0,
  CompileJobConstruction,
  BackendJobConstruction,
  GeneratePCHJobConstruction,
  MergeModuleJobConstruction,
  ModuleWrapJobConstruction,
  DynamicLinkJobConstruction,
  StaticLinkJobConstruction,
  InterpretJobConstruction,
  AutolinkExtractJobConstruction,
  Job,
  BatchJob,
  First = Input,
  Last = BatchJob
};

/// A list of all job construction inputs
using CompilationEntityList = llvm::SmallVector<const CompilationEntity *, 3>;

constexpr size_t CompilationEntityAlignInBits = 8;

class alignas(1 << CompilationEntityAlignInBits) CompilationEntity
    : public DriverAllocation<CompilationEntity> {

  CompilationEntityKind kind;
  FileType fileType;
  CompilationEntityList allInputs;

public:
  using size_type = CompilationEntityList::size_type;
  using input_iterator = CompilationEntityList::iterator;
  using input_const_iterator = CompilationEntityList::const_iterator;
  using input_range = llvm::iterator_range<input_iterator>;
  using input_const_range = llvm::iterator_range<input_const_iterator>;

  public:
  CompilationEntity(CompilationEntityKind kind, FileType fileType)
      : CompilationEntity(kind, CompilationEntityList(), fileType) {}

  CompilationEntity(CompilationEntityKind kind, CompilationEntity *input, FileType fileType)
      : CompilationEntity(kind, CompilationEntityList({input}), fileType) {}

  CompilationEntity(CompilationEntityKind kind, CompilationEntity *input)
      : CompilationEntity(kind, CompilationEntityList({input}), input->GetFileType()) {}

  CompilationEntity(CompilationEntityKind kind, const CompilationEntityList &inputs, FileType fileType)
      : kind(kind), fileType(fileType), allInputs(inputs) {}


public:
  CompilationEntityKind GetKind() const { return kind; }
  FileType GetFileType() const { return fileType; }
  bool HasFileType() { return fileType != FileType::None; }

  CompilationEntityList &GetInputs() { return allInputs; }
  const CompilationEntityList &GetInputs() const { return allInputs; }
  void AddInput(CompilationEntity *input) { allInputs.push_back(input); }

public:
  size_type size() const { return allInputs.size(); }
  input_iterator input_begin() { return allInputs.begin(); }
  input_iterator input_end() { return allInputs.end(); }
  input_range inputs() { return input_range(input_begin(), input_end()); }
  input_const_iterator input_begin() const { return allInputs.begin(); }
  input_const_iterator input_end() const { return allInputs.end(); }
  input_const_range inputs() const {
    return input_const_range(input_begin(), input_end());
  }

};

} // namespace stone

#endif