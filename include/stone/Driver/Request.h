#ifndef STONE_DRIVER_JOBREQUEST_H
#define STONE_DRIVER_JOBREQUEST_H

#include "stone/Core/Color.h"
#include "stone/Core/List.h"
#include "stone/Core/StatisticEngine.h"
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

enum class RequestKind : uint8_t {
  Input = 0,
  Compile,
  Backend,
  MergeModule,
  Link,
  Assemble,
  FirstJob = Compile,
  LastJob = Link,
};

class Request {
  RequestKind kind;
  friend class Driver;
  /// Requests are only created via Driver::CreateRequest(...)
  void *operator new(size_t size) { return ::operator new(size); };

  static const char *GetNameByKind(RequestKind kind);

public:
  Request(RequestKind kind) : kind(kind) {}
  virtual ~Request() = default;

public:
  /// Perform a complete dump of this job.
  virtual void Print(ColorOutputStream &stream,
                     llvm::StringRef terminator = "\n") const;

public:
  RequestKind GetKind() const { return kind; }
  const char *GetName() const { return Request::GetNameByKind(kind); }
};

class InputRequest : public Request {
  const file::File &input;

public:
  InputRequest(const file::File &input)
      : Request(RequestKind::Input), input(input) {}

  const file::File &GetInput() const { return input; }

public:
  static bool classof(const Request *req) {
    return req->GetKind() == RequestKind::Input;
  }
};

using RequestList = llvm::ArrayRef<const Request *>;
class JobRequest : public Request {
  file::Type outputFileType = file::Type::None;
  llvm::TinyPtrVector<const Request *> inputs;

public:
  using size_type = llvm::ArrayRef<const Request *>::size_type;
  using iterator = llvm::ArrayRef<const Request *>::iterator;
  using const_iterator = llvm::ArrayRef<const Request *>::const_iterator;

public:
  JobRequest(RequestKind kind, RequestList inputs, file::Type outputFileType)
      : Request(kind), inputs(inputs), outputFileType(outputFileType) {}

public:
  llvm::ArrayRef<const Request *> GetInputs() const { return inputs; }
  void AddInput(const Request *input) { inputs.push_back(input); }
  file::Type GetOutputFileType() const { return outputFileType; }

  /// Perform a complete dump of this job.
  void Print(ColorOutputStream &stream,
             llvm::StringRef terminator = "\n") const override;

public:
  size_type size() const { return inputs.size(); }
  iterator begin() { return inputs.begin(); }
  iterator end() { return inputs.end(); }
  const_iterator begin() const { return inputs.begin(); }
  const_iterator end() const { return inputs.end(); }

public:
  // Required for llvm::dyn_cast
  static bool classof(const Request *req) {
    return (req->GetKind() >= RequestKind::FirstJob &&
            req->GetKind() <= RequestKind::LastJob);
  }
};

class CompileJobRequest final : public JobRequest {
public:
  CompileJobRequest(file::Type outputFileType)
      : JobRequest(RequestKind::Compile, llvm::None, outputFileType) {}

  CompileJobRequest(const Request *input, file::Type outputFileType)
      : JobRequest(RequestKind::Compile, input, outputFileType) {}

public:
  static bool classof(const Request *req) {
    return req->GetKind() == RequestKind::Compile;
  }
};

class LinkJobRequest final : public JobRequest {
  LinkMode linkMode;
  bool canPerformLTO;

private:
  static bool IsLinkMode(LinkMode linkMode) {
    switch (linkMode) {
    case LinkMode::EmitStaticLibrary:
    case LinkMode::EmitDynamicLibrary:
    case LinkMode::EmitExecutable:
      return true;
    default:
      return false;
    }
  }

public:
  LinkJobRequest(RequestList inputs, LinkMode linkMode, bool canPerformLTO)
      : JobRequest(RequestKind::Link, inputs, file::Type::Image),
        linkMode(linkMode), canPerformLTO(canPerformLTO) {
    assert(LinkJobRequest::IsLinkMode(linkMode));
  }
  bool CanPerformLTO() {
    return (canPerformLTO && (linkMode == LinkMode::EmitDynamicLibrary));
  }

public:
  static bool classof(const Request *req) {
    return (req->GetKind() == RequestKind::Link);
  }
};

} // namespace stone
#endif