#ifndef STONE_DRIVER_COMPILATIONJOB_H
#define STONE_DRIVER_COMPILATIONJOB_H

#include "stone/Basic/Color.h"
#include "stone/Basic/List.h"
#include "stone/Basic/StatisticEngine.h"
#include "stone/Driver/Command.h"
#include "stone/Driver/CrashState.h"
#include "stone/Driver/DriverOptions.h"
#include "stone/Driver/JobKind.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/TinyPtrVector.h"
#include "llvm/Option/Arg.h"
#include "llvm/Option/ArgList.h"
#include "llvm/Support/StringSaver.h"
#include "llvm/Support/Timer.h"

namespace stone {
// class Tool;
// class Driver;
// class CompilationJob;

// // The process ID
// using CompilationJobID = int64_t;

// enum class CompilationJobKind : uint8_t {
//   None = 0,
//   Compile,
//   Link,
//   StaticLink,
//   DynamicLink,
//   ExecLink,
//   First = Compile,
//   Last = ExecLink,
// };

// enum class CompilationJobStage : uint8_t { None = 0, Running, Finished, Error
// };

// /// See OutputOptions
// enum class ThreadingKind : uint8_t { None = 0, Single, Multi };

// using InputList = llvm::ArrayRef<const file::File *>;
// class CompilationJob : public Command {

//   friend class ToolChain;
//   /// Jobs are made via ToolChain::MakeJob(...)
//   void *operator new(size_t size) { return ::operator new(size); };

//   CompilationJobID jobID;
//   CompilationJobKind kind;

//   file::Type outputFileType = file::Type::None;
//   llvm::TinyPtrVector<const file::File *> inputs;

// protected:
//   int queueID = -1;

// public:
//   CompilationJobStage stage = CompilationJobStage::None;

// public:
//   CompilationJob(CompilationJobKind kind, const Tool &tool, InputList inputs,
//                  file::Type outputFileType)
//       : Command(tool), kind(kind), inputs(inputs),
//         outputFileType(outputFileType) {}

//   /// Print a nice summary of this job
//   virtual void Print(ColorStream &stream,
//                      CrashState *crashState = nullptr);

//   /// Perform a complete dump of this job.
//   virtual void Dump(ColorStream &stream,
//                     llvm::StringRef terminator = "\n",
//                     CrashState *crashState = nullptr);

//   int GetQueueID() const { return queueID; }
//   // const char *GetName() const { return
//   // CompilationJob::GetNameByKind(jobKind); }
// public:
//   llvm::ArrayRef<const file::File *> GetInputs() { return inputs; }
//   CompilationJobKind GetKind() const { return kind; }
//   void AddInput(const file::File *input) { inputs.push_back(input); }

// public:
//   // Required for llvm::dyn_cast
//   static bool classof(const CompilationJob *job) {
//     return (job->GetKind() >= CompilationJobKind::First &&
//             job->GetKind() <= CompilationJobKind::Last);
//   }
// };

// class CompileJob final : public CompilationJob {
//   // ToolChain::ConstructCompileJob(...)
// public:
//   CompileJob(const Tool &tool, file::Type outputFileType)
//       : CompilationJob(CompilationJobKind::Compile, tool, {}, outputFileType)
//       {}

//   CompileJob(const Tool &tool, InputList inputs, file::Type outputFileType)
//       : CompilationJob(CompilationJobKind::Compile, tool, inputs,
//                        outputFileType) {}

// public:
//   static bool classof(const CompilationJob *job) {
//     return job->GetKind() == CompilationJobKind::Compile;
//   }
// };

// using DepList = llvm::ArrayRef<const CompilationJob *>;
// // Can accept inputs and deps.
// class UniversalJob : public CompilationJob {
//   llvm::TinyPtrVector<const CompilationJob *> deps;

// public:
//   enum class Purpose { Solo, Top };

// private:
//   Purpose purpose;

// public:
//   using size_type = llvm::ArrayRef<const CompilationJob *>::size_type;
//   using iterator = llvm::ArrayRef<const CompilationJob *>::iterator;
//   using const_iterator = llvm::ArrayRef<const CompilationJob
//   *>::const_iterator;

// public:
//   UniversalJob(CompilationJobKind kind, const Tool &tool, InputList inputs,
//                file::Type outputFileType)
//       : CompilationJob(kind, tool, inputs, outputFileType),
//         purpose(Purpose::Solo) {}

//   UniversalJob(CompilationJobKind kind, const Tool &tool, DepList deps,
//                file::Type outputFileType)
//       : CompilationJob(kind, tool, {}, outputFileType), deps(deps),
//         purpose(Purpose::Top) {}

// public:
//   void AddDep(const CompilationJob *dep) { deps.push_back(dep); }

//   /// Print a nice summary of this job
//   void Print(ColorStream &stream,
//              CrashState *crashState = nullptr) override;

//   /// Perform a complete dump of this job.
//   void Dump(ColorStream &stream, llvm::StringRef terminator = "\n",
//             CrashState *crashState = nullptr) override;

//   Purpose GetPurpose() { return purpose; }

// public:
//   size_type size() const { return deps.size(); }
//   iterator begin() { return deps.begin(); }
//   iterator end() { return deps.end(); }
//   const_iterator begin() const { return deps.begin(); }
//   const_iterator end() const { return deps.end(); }

// public:
//   // Required for llvm::dyn_cast
//   static bool classof(const CompilationJob *job) {
//     return (job->GetKind() >= CompilationJobKind::First &&
//             job->GetKind() <= CompilationJobKind::Last);
//   }
// };

// class DynamicLinkJob final : public UniversalJob {
//   bool requiresLTO;

// public:
//   DynamicLinkJob(const Tool &tool, InputList inputs, file::Type
//   outputFileType,
//                  bool requiresLTO = false)
//       : UniversalJob(CompilationJobKind::DynamicLink, tool, inputs,
//                      outputFileType),
//         requiresLTO(requiresLTO) {}

//   DynamicLinkJob(const Tool &tool, DepList deps, file::Type outputFileType,
//                  bool requiresLTO = false)
//       : UniversalJob(CompilationJobKind::DynamicLink, tool, deps,
//                      outputFileType),
//         requiresLTO(requiresLTO) {}

// public:
//   static bool classof(const CompilationJob *job) {
//     return job->GetKind() == CompilationJobKind::DynamicLink;
//   }
// };

// class StaticLinkJob final : public UniversalJob {

// public:
//   StaticLinkJob(const Tool &tool, InputList inputs, file::Type
//   outputFileType)
//       : UniversalJob(CompilationJobKind::StaticLink, tool, inputs,
//                      outputFileType) {}

//   StaticLinkJob(const Tool &tool, DepList deps, file::Type outputFileType)
//       : UniversalJob(CompilationJobKind::StaticLink, tool, deps,
//                      outputFileType) {}

// public:
//   static bool classof(const CompilationJob *job) {
//     return job->GetKind() == CompilationJobKind::StaticLink;
//   }
// };

// class ExecLinkJob final : public UniversalJob {

// public:
//   ExecLinkJob(const Tool &tool, InputList inputs, file::Type outputFileType)
//       : UniversalJob(CompilationJobKind::ExecLink, tool, inputs,
//                      outputFileType) {}

//   ExecLinkJob(const Tool &tool, DepList deps, file::Type outputFileType)
//       : UniversalJob(CompilationJobKind::ExecLink, tool, deps,
//       outputFileType) {
//   }

// public:
//   static bool classof(const CompilationJob *job) {
//     return job->GetKind() == CompilationJobKind::ExecLink;
//   }
// };

} // namespace stone
#endif
