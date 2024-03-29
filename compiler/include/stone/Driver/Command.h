#ifndef STONE_DRIVER_COMMAND_H
#define STONE_DRIVER_COMMAND_H

#include "stone/Basic/File.h"
#include "stone/Basic/LLVM.h"
#include "stone/Basic/OutputFileMap.h"
#include "llvm/ADT/SmallSet.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/StringSaver.h"

namespace stone {

struct CommandInputPair final {
  /// A filename provided from the user, either on the command line or in an
  /// input file map. Feeds into a Job graph, from InputJobActions, and is
  /// _associated_ with a PrimaryInput for a given Job, but may be upstream of
  /// the Job (and its PrimaryInput) and thus not necessarily passed as a
  /// filename to the job. Used as a key into the user-provided OutputFileMap
  /// (of BaseInputs and BaseOutputs), and used to derive downstream names --
  /// both temporaries and auxiliaries -- but _not_ used as a key into the
  /// DerivedOutputFileMap.
  llvm::StringRef baseInput;

  /// A filename that _will be passed_ to the command as a designated primary
  /// input. Typically either equal to BaseInput or a temporary with a name
  /// derived from the BaseInput it is related to. Also used as a key into
  /// the DerivedOutputFileMap.
  llvm::StringRef primaryInput;

  /// Construct a CmdInputPair from a Base Input and, optionally, a Primary;
  /// if the Primary is empty, use the Base value for it.
  explicit CommandInputPair(llvm::StringRef baseInput,
                            llvm::StringRef primaryInput)
      : baseInput(baseInput),
        primaryInput(primaryInput.empty() ? baseInput : primaryInput) {}
};

class CommandOutput final {
  /// A CmdOutput designates one type of output as primary, though there
  /// may be multiple outputs of that type.
  file::Type primaryOutputType;

  /// A CmdOutput also restricts its attention regarding additional-outputs
  /// to a subset of the PrimaryOutputs associated with its PrimaryInputs;
  /// sometimes multiple commands operate on the same PrimaryInput, in different
  /// phases (eg. autolink-extract and link both operate on the same .o file),
  /// so Jobs cannot _just_ rely on the presence of a primary output in the
  /// DerivedOutputFileMap.
  llvm::SmallSet<file::Type, 4> additionalOutputTypes;

  /// The list of inputs for this \c CmdOutput. Each input in the list has
  /// two names (often but not always the same), of which the second (\c
  /// CmdInputPair::Primary) acts as a key into \c DerivedOutputMap.  Each
  /// input thus designates an associated _set_ of outputs, one of which (the
  /// one of type \c PrimaryOutputType) is considered the "primary output" for
  /// the input.
  llvm::SmallVector<CommandInputPair, 1> inputs;

  /// All CmdOutputs in a Compilation share the same \c
  /// DerivedOutputMap. This is computed both from any user-provided input file
  /// map, and any inference steps.
  OutputFileMap &derivedOutputMap;

  // If there is an entry in the DerivedOutputMap for a given (\p
  // PrimaryInputFile, \p Type) pair, return a nonempty StringRef, otherwise
  // return an empty StringRef.
  llvm::StringRef GetOutputForInputAndType(llvm::StringRef primaryInputFile,
                                           file::Type fileType) const;

  /// Add an entry to the \c DerivedOutputMap if it doesn't exist. If an entry
  /// already exists for \p PrimaryInputFile of type \p type, then either
  /// overwrite the entry (if \p overwrite is \c true) or assert that it has
  /// the same value as \p OutputFile.
  void EnsureEntry(llvm::StringRef primaryInputFile, file::Type fileType,
                   llvm::StringRef outputFile, bool overwrite);

public:
  CommandOutput(file::Type primaryOutputType, OutputFileMap &derived);

  /// For testing dependency graphs that use Jobs
  CommandOutput(llvm::StringRef fakeBaseName, OutputFileMap &);

  /// Return the primary output type for this CommandOutput.
  file::Type GetPrimaryOutputType() const;

  /// Associate a new \p PrimaryOutputFile (of type \c getPrimaryOutputType())
  /// with the provided \p Input pair of Base and Primary inputs.
  void AddPrimaryOutput(CommandInputPair input,
                        llvm::StringRef primaryOutputFile);

  /// Return true iff the set of additional output types in \c this is
  /// identical to the set of additional output types in \p other.
  bool HasSameAdditionalOutputTypes(CommandOutput const &other) const;

  /// Copy all the input pairs from \p other to \c this. Assumes (and asserts)
  /// that \p other shares output file map and PrimaryOutputType with \c this
  /// already, as well as AdditionalOutputTypes if \c this has any.
  void AddOutputs(CommandOutput const &other);

  /// Assuming (and asserting) that there is only one input pair, return the
  /// primary output file associated with it. Note that the returned StringRef
  /// may be invalidated by subsequent mutations to the \c CmdOutput.
  llvm::StringRef GetPrimaryOutputFilename() const;

  /// Return a all of the outputs of type \c getPrimaryOutputType() associated
  /// with a primary input. The return value will contain one \c StringRef per
  /// primary input, _even if_ the primary output type is TY_Nothing, and the
  /// primary output filenames are therefore all empty strings.
  ///
  /// FIXME: This is not really ideal behaviour -- it would be better to return
  /// only nonempty strings in all cases, and have the callers differentiate
  /// contexts with absent primary outputs another way -- but this is currently
  /// assumed at several call sites.
  llvm::SmallVector<llvm::StringRef, 16> GetPrimaryOutputFilenames() const;

  /// Assuming (and asserting) that there are one or more input pairs, associate
  /// an additional output named \p OutputFilename of type \p type with the
  /// first primary input. If the provided \p type is the primary output type,
  /// overwrite the existing entry assocaited with the first primary input.
  void SetAdditionalOutputForType(file::Type fileType,
                                  llvm::StringRef outputFilename);

  /// Assuming (and asserting) that there are one or more input pairs, return
  /// the _additional_ (not primary) output of type \p type associated with the
  /// first primary input.
  llvm::StringRef GetAdditionalOutputForType(file::Type fileType) const;

  /// Assuming (and asserting) that there are one or more input pairs, return
  /// true if there exists an _additional_ (not primary) output of type \p type
  /// associated with the first primary input.
  bool HasAdditionalOutputForType(file::Type fileType) const;
  /// Return a vector of additional (not primary) outputs of type \p type
  /// associated with the primary inputs.
  ///
  /// In contrast to \c getPrimaryOutputFilenames, this method does _not_
  /// return any empty strings or ensure the return vector is matched in
  /// size with the set of primary inputs; however it _does_ assert that the
  /// return vector's length is _either_ zero, one, or equal to the size of
  /// the set of inputs, as these are the only valid arity relationships
  /// between primary and additional outputs.
  llvm::SmallVector<llvm::StringRef, 16>
  GetAdditionalOutputsForType(file::Type fileType) const;

  /// Assuming (and asserting) that there is only one input pair, return any
  /// output -- primary or additional -- of type \p type associated with that
  /// the sole primary input.
  llvm::StringRef GetAnyOutputForType(file::Type fileType) const;

  /// Return the whole derived output map.
  const OutputFileMap &GetDerivedOutputMap() const;

  /// Return the BaseInput numbered by \p Index.
  llvm::StringRef GetBaseInput(size_t Index) const;

  /// Write a file map naming the outputs for each primary input.
  void WriteOutputFileMap(llvm::raw_ostream &out) const;

  void Print(raw_ostream &stream) const;
  /// For use in assertions: check the CmdOutput's state is consistent with
  /// its invariants.
  void CheckInvariants() const;
};

class Tool;
class Context;

class Command {
private:
  /// The tool that this command will use
  const Tool &tool;
  /// The outputs this command is expected to produce
  std::unique_ptr<CommandOutput> cmdOutput;

public:
  const char *execPath;
  llvm::SmallVector<llvm::StringRef, 16> args;
  llvm::Optional<llvm::ArrayRef<llvm::StringRef>> env = llvm::None;
  llvm::ArrayRef<llvm::Optional<llvm::StringRef>> redirects;

  unsigned waitSecs = 0;
  unsigned memLimit = 0;
  std::string *errMsg;
  bool *failed;

public:
  Command(const Tool &tool) : tool(tool) {}

public:
  const Tool &GetTool() const { return tool; }

  CommandOutput &GetOutput() { return *cmdOutput.get(); }
  void TakeCommandOutput(std::unique_ptr<CommandOutput> commandOutput) {
    cmdOutput = std::move(commandOutput);
  }

public:
  static int ExecuteSync(const Command &c, Context *ctx = nullptr);
  static int ExecuteAsync(const Command &c, Context *ctx = nullptr);
};

class CommandInvocation final {
private:
  /// The tool that this command will use
  const Tool &tool;
  /// The outputs this command is expected to produce
  std::unique_ptr<CommandOutput> cmdOutput;

public:
  llvm::SmallVector<llvm::StringRef, 16> args;
  llvm::Optional<llvm::ArrayRef<llvm::StringRef>> env = llvm::None;
  llvm::ArrayRef<llvm::Optional<llvm::StringRef>> redirects;

  unsigned waitSecs = 0;
  unsigned memLimit = 0;
  std::string *errMsg;
  bool *failed;

public:
  CommandInvocation() = delete;
  CommandInvocation(const Tool &tool) : CommandInvocation(tool, nullptr) {}
  CommandInvocation(const Tool &tool, std::unique_ptr<CommandOutput> cmdOutput)
      : tool(tool), cmdOutput(std::move(cmdOutput)) {}

public:
  const Tool &GetTool() const { return tool; }
  CommandOutput &GetOutput() const { return *cmdOutput.get(); }
};

namespace job {
int RunSync(const Command &command, Context *ctx = nullptr);
int RunAsync(const Command &command, Context *ctx = nullptr);
} // namespace job

} // namespace stone

#endif
