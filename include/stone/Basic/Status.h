#ifndef STONE_BASIC_STATUS_H
#define STONE_BASIC_STATUS_H

#include <memory>

namespace stone {

constexpr size_t StatusAlignment = 8;
class alignas(1 << StatusAlignment) Status final {
  unsigned isError : 1;
  unsigned isCompletion : 1;

public:
  /// Construct a successful parser status.
  Status() : isError(0), isCompletion(0) {}

  /// Construct a parser status with specified bits.
  Status(bool isError, bool isCompletion = false)
      : isError(0), isCompletion(0) {
    if (isError) {
      SetIsError();
    }
    if (isCompletion) {
      SetHasCompletion();
    }
  }
  bool IsError() const { return isError; }
  /// Return true if we found a code completion token while parsing this.
  bool HasCompletion() const { return isCompletion; }

  /// Return true if either 1) no errors were encountered while parsing this,
  /// or 2) there were errors but the the parser already recovered from them.
  bool IsSuccess() const { return !IsError(); }
  bool IsErrorOrHasCompletion() const { return IsError() || HasCompletion(); }

  /// Return true if we encountered any errors while parsing this that the
  /// parser hasn't yet recovered from.

  void SetIsError() { isError = true; }
  void ClearIsError() { isError = false; }
  int GetFlag() { return IsError() ? 1 : 0; }

  void SetHasCompletion() { isCompletion = true; }
  void SetHasCompletionAndIsError() {
    SetIsError();
    SetHasCompletion();
  }
  Status &operator|=(Status RHS) {
    isError |= RHS.isError;
    isCompletion |= RHS.isCompletion;
    return *this;
  }

  friend Status operator|(Status LHS, Status RHS) {
    Status result = LHS;
    result |= RHS;
    return result;
  }

public:
  static Status Success() { return Status(); }
  static Status Error() {
    Status status;
    status.SetIsError();
    return status;
  }
  static Status MakeHasCompletion() {
    Status status;
    status.SetHasCompletion();
    return status;
  }
  static Status MakeHasCompletionAndIsError() {
    Status status;
    status.SetHasCompletionAndIsError();
    return status;
  }
};

enum Result : uint8_t { Success = 0, Error };

} // namespace stone
#endif
