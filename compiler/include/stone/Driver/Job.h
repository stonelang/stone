#ifndef STONE_DRIVER_DRIVER_JOB_H
#define STONE_DRIVER_DRIVER_JOB_H


#include "stone/Basic/OptionSet.h"
#include "stone/Driver/CompilationEntity.h"
#include "stone/Driver/JobConstruction.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/SmallSet.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Option/Option.h"
#include "llvm/Support/Chrono.h"
#include "llvm/Support/Program.h"
#include "llvm/Support/raw_ostream.h"

namespace stone {
class Job;
class Driver;
class Compilation;

struct CommandInputPair {
  /// A filename provided from the user, either on the command line or in an
  /// input file map. Feeds into a Job graph, from InputActions, and is
  /// _associated_ with a PrimaryInput for a given Job, but may be upstream of
  /// the Job (and its PrimaryInput) and thus not necessarily passed as a
  /// filename to the job. Used as a key into the user-provided OutputFileMap
  /// (of BaseInputs and BaseOutputs), and used to derive downstream names --
  /// both temporaries and auxiliaries -- but _not_ used as a key into the
  /// DerivedOutputFileMap.
  StringRef Base;

  /// A filename that _will be passed_ to the command as a designated primary
  /// input. Typically either equal to BaseInput or a temporary with a name
  /// derived from the BaseInput it is related to. Also used as a key into
  /// the DerivedOutputFileMap.
  StringRef Primary;

  /// Construct a JobInputPair from a Base Input and, optionally, a Primary;
  /// if the Primary is empty, use the Base value for it.
  explicit CommandInputPair(llvm::StringRef BaseInput, llvm::StringRef PrimaryInput)
      : Base(BaseInput),
        Primary(PrimaryInput.empty() ? BaseInput : PrimaryInput) {}
};

class CommandOutput final {

public:
  /// A CommandOutput designates one type of output as primary, though there
  /// may be multiple outputs of that type.
  file::FileType primaryOutputFileType;

  /// A CommandOutput also restricts its attention regarding additional-outputs
  /// to a subset of the PrimaryOutputs associated with its PrimaryInputs;
  /// sometimes multiple commands operate on the same PrimaryInput, in different
  /// phases (eg. autolink-extract and link both operate on the same .o file),
  /// so Jobs cannot _just_ rely on the presence of a primary output in the
  /// DerivedOutputFileMap.
  llvm::SmallSet<file::FileType, 4> additionalOutputFileTypes;
};


} // namespace stone
#endif