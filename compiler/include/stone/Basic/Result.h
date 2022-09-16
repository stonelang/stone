#ifndef STONE_BASIC_RESULT_H
#define STONE_BASIC_RESULT_H

#include "stone/Basic/Error.h"

#include <memory>

namespace stone {

template <typename T> class Result final {
  T ty;
  Error err;

public:
  Result() : err(true) {}
  Result(T ty) : ty(ty), err(false) {}

public:
  Error &GetError() { return err; }
  T &GetRef() const { return *ty; }
  template <typename A> A *GetAs() { return static_cast<A *>(ty.get()); }
};

template <typename T> class SafeResult final {
  Error err;
  std::unique_ptr<T> ty;

public:
  SafeResult() : err(true) {}
  SafeResult(std::unique_ptr<T> ty) : ty(std::move(ty)), err(false) {}

public:
  Error &GetError() { return err; }
  T &GetRef() const { return *ty; }
  template <typename A> A *GetAs() { return static_cast<A *>(ty.get()); }
};

class ResultStatus final {
  unsigned isError : 1;
  unsigned IsCompletion : 1;

public:
  /// Construct a successful parser status.
  ResultStatus() : isError(0), IsCompletion(0) {}

  /// Construct a parser status with specified bits.
  ResultStatus(bool isError, bool isCompletion = false)
      : isError(0), IsCompletion(0) {
    if (isError) {
      SetIsError();
    }
    if (isCompletion) {
      IsCompletion = true;
    }
  }
  /// Return true if either 1) no errors were encountered while parsing this,
  /// or 2) there were errors but the the parser already recovered from them.
  bool IsSuccess() const { return !IsError(); }
  bool IsErrorOrHasCompletion() const { return isError || IsCompletion; }

  /// Return true if we found a code completion token while parsing this.
  bool HasCompletion() const { return IsCompletion; }

  /// Return true if we encountered any errors while parsing this that the
  /// parser hasn't yet recovered from.
  bool IsError() const { return isError; }

  void SetIsError() { isError = true; }

  void SetHasCompletion() { IsCompletion = true; }

  void ClearIsError() { isError = false; }

  void SetHasCompletionAndIsError() {
    isError = true;
    IsCompletion = true;
  }
  ResultStatus &operator|=(ResultStatus RHS) {
    isError |= RHS.isError;
    IsCompletion |= RHS.IsCompletion;
    return *this;
  }

  friend ResultStatus operator|(ResultStatus LHS, ResultStatus RHS) {
    ResultStatus Result = LHS;
    Result |= RHS;
    return Result;
  }

public:
  static ResultStatus MakeSuccess() { return ResultStatus(); }
  static ResultStatus MakeError() {
    ResultStatus Status;
    Status.SetIsError();
    return Status;
  }
};

} // namespace stone
#endif
