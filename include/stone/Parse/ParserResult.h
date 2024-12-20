#ifndef STONE_AST_ASTRESULT_H
#define STONE_AST_ASTRESULT_H

#include "llvm/ADT/PointerIntPair.h"
#include <type_traits>

namespace stone {

class ParserStatus;
/// A wrapper for a parser AST node result (Decl, Stmt, Expr,
/// etc.)
///
/// Contains the pointer to the AST node itself (or null) and additional bits
/// that indicate:
/// \li if there was a parse error;
/// \li if there was a code completion token.
///
/// If you want to return an AST node pointer in the Syntax, consider using
/// ParserResult instead.
template <typename T> class ParserResult {
  llvm::PointerIntPair<T *, 2> ptrAndBits;
  enum {
    isError = 0x1,
    IsCodeCompletion = 0x2,
  };
  template <typename U> friend class ParserResult;

  template <typename U>
  friend inline ParserResult<U> MakeParserResult(ParserStatus Status,
                                                 U *Result);

public:
  /// Construct a null result with error bit set.
  ParserResult(std::nullptr_t = nullptr) { SetIsError(); }

  /// Construct a null result with specified error bits set.
  ParserResult(ParserStatus Status);

  /// Construct a successful parser result.
  explicit ParserResult(T *Result) : ptrAndBits(Result) {
    assert(Result && "a successful parser result cannot be null");
  }
  /// Convert from a different but compatible parser result.
  template <typename U, typename Enabler = typename std::enable_if<
                            std::is_base_of<T, U>::value>::type>
  ParserResult(ParserResult<U> Other)
      : ptrAndBits(Other.ptrAndBits.getPointer(), Other.ptrAndBits.getInt()) {}

  /// Return true if this result does not have an AST node.
  ///
  /// If returns true, then error bit is set.
  bool IsNull() const { return GetPtrOrNull() == nullptr; }

  /// Return true if this result has an AST node.
  ///
  /// Note that this does not tell us if there was a parse error or not.
  bool IsNonNull() const { return GetPtrOrNull() != nullptr; }

  /// Return the AST node if non-null.
  T *Get() const {
    assert(GetPtrOrNull() && "not checked for nullptr");
    return GetPtrOrNull();
  }

  /// Return the AST node or a null pointer.
  T *GetPtrOrNull() const { return ptrAndBits.getPointer(); }

  /// Return true if there was a parse error that the parser has not yet
  /// recovered from.
  ///
  /// Note that we can still have an AST node which was constructed during
  /// recovery.
  bool IsError() const { return ptrAndBits.getInt() & isError; }

  /// Return true if there was a parse error that the parser has not yet
  /// recovered from, or if we found a code completion token while parsing.
  ///
  /// Note that we can still have an AST node which was constructed during
  /// recovery.
  bool IsErrorOrHasCompletion() const {
    return ptrAndBits.getInt() & (isError | IsCodeCompletion);
  }

  /// Return true if we found a code completion token while parsing this.
  bool HasCodeCompletion() const {
    return ptrAndBits.getInt() & IsCodeCompletion;
  }

  void SetIsError() { ptrAndBits.setInt(ptrAndBits.getInt() | isError); }

  void SetHasCodeCompletionAndIsError() {
    ptrAndBits.setInt(ptrAndBits.getInt() | isError | IsCodeCompletion);
  }

private:
  void SetHasCodeCompletion() {
    ptrAndBits.setInt(ptrAndBits.getInt() | IsCodeCompletion);
  }
};

/// Create a successful parser result.
template <typename T>
static inline ParserResult<T> MakeParserResult(T *Result) {
  return ParserResult<T>(Result);
}

/// Create a result (null or non-null) with error bit set.
template <typename T>
static inline ParserResult<T> MakeParserErrorResult(T *Result = nullptr) {
  ParserResult<T> PR;
  if (Result) {
    PR = ParserResult<T>(Result);
  }
  PR.SetIsError();
  return PR;
}

template <typename T> static inline ParserResult<T> MakeNullParserResult() {
  return ParserResult<T>(nullptr);
}

/// Create a result (null or non-null) with error and code completion bits set.
template <typename T>
static inline ParserResult<T>
MakeParserCodeCompletionResult(T *Result = nullptr) {
  ParserResult<T> PR;
  if (Result) {
    PR = ParserResult<T>(Result);
  }
  PR.SetHasCodeCompletionAndIsError();
  return PR;
}

/// Same as \c ParserResult, but just the status bits without the AST
/// node.
///
/// Useful when the AST node is returned by some other means (for example, in
/// a vector out parameter).
///
/// If you want to use 'bool' as a result type in the Syntax, consider using
/// ParserStatus instead.
class ParserStatus {

  // 1 = false
  unsigned isError : 1;
  unsigned IsCodeCompletion : 1;

public:
  /// Construct a successful parser status by setting values to true = 0
  ParserStatus() : isError(0), IsCodeCompletion(0) {}

  /// Construct a parser status with specified bits.
  template <typename T>
  ParserStatus(ParserResult<T> Result) : isError(0), IsCodeCompletion(0) {
    if (Result.IsError()) {
      SetIsError();
    }
    if (Result.HasCodeCompletion())
      IsCodeCompletion = true;
  }

  /// Return true if either 1) no errors were encountered while parsing this,
  /// or 2) there were errors but the the parser already recovered from them.
  bool IsSuccess() const { return !IsError(); }
  bool IsErrorOrHasCompletion() const { return isError || IsCodeCompletion; }

  /// Return true if we found a code completion token while parsing this.
  bool HasCodeCompletion() const { return IsCodeCompletion; }

  /// Return true if we encountered any errors while parsing this that the
  /// parser hasn't yet recovered from.
  bool IsError() const { return isError; }

  void SetIsError() { isError = true; }

  void SetHasCodeCompletion() { IsCodeCompletion = true; }

  void ClearIsError() { isError = false; }

  void SetHasCodeCompletionAndIsError() {
    isError = true;
    IsCodeCompletion = true;
  }

  ParserStatus &operator|=(ParserStatus RHS) {
    isError |= RHS.isError;
    IsCodeCompletion |= RHS.IsCodeCompletion;
    return *this;
  }

  friend ParserStatus operator|(ParserStatus LHS, ParserStatus RHS) {
    ParserStatus Result = LHS;
    Result |= RHS;
    return Result;
  }
};

/// Create a successful parser status.
static inline ParserStatus MakeParserSuccess() { return ParserStatus(); }

/// Create a status with error bit set.
static inline ParserStatus MakeParserError() {
  ParserStatus Status;
  Status.SetIsError();
  return Status;
}

/// Create a status with error and code completion bits set.
static inline ParserStatus MakeParserCodeCompletionStatus() {
  ParserStatus Status;
  Status.SetHasCodeCompletionAndIsError();
  return Status;
}
/// Create a parser result with specified bits.
template <typename T>
static inline ParserResult<T> MakeParserResult(ParserStatus Status, T *Result) {
  ParserResult<T> PR = Status.IsError() ? MakeParserErrorResult(Result)
                                        : MakeParserResult(Result);

  if (Status.HasCodeCompletion()) {
    PR.SetHasCodeCompletion();
  }
  return PR;
}

template <typename T> ParserResult<T>::ParserResult(ParserStatus Status) {
  assert(Status.IsError());
  SetIsError();
  if (Status.HasCodeCompletion()) {
    SetHasCodeCompletion();
  }
}

} // namespace stone

#endif
