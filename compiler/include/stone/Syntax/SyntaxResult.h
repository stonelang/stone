#ifndef STONE_SYNTAX_SYNTAXRESULT_H
#define STONE_SYNTAX_SYNTAXRESULT_H

#include "llvm/ADT/PointerIntPair.h"
#include <type_traits>

namespace stone {
namespace syn {
class SyntaxStatus;
/// A wrapper for a parser AST node result (Decl, Stmt, Expr,
/// etc.)
///
/// Contains the pointer to the AST node itself (or null) and additional bits
/// that indicate:
/// \li if there was a parse error;
/// \li if there was a code completion token.
///
/// If you want to return an AST node pointer in the Syntax, consider using
/// SyntaxResult instead.
template <typename T> class SyntaxResult {
  llvm::PointerIntPair<T *, 2> ptrAndBits;
  enum {
    isError = 0x1,
    IsCodeCompletion = 0x2,
  };
  template <typename U> friend class SyntaxResult;

  template <typename U>
  friend inline SyntaxResult<U> MakeSyntaxResult(SyntaxStatus Status,
                                                 U *Result);

public:
  /// Construct a null result with error bit set.
  SyntaxResult(std::nullptr_t = nullptr) { SetIsError(); }

  /// Construct a null result with specified error bits set.
  SyntaxResult(SyntaxStatus Status);

  /// Construct a successful parser result.
  explicit SyntaxResult(T *Result) : ptrAndBits(Result) {
    assert(Result && "a successful parser result cannot be null");
  }
  /// Convert from a different but compatible parser result.
  template <typename U, typename Enabler = typename std::enable_if<
                            std::is_base_of<T, U>::value>::type>
  SyntaxResult(SyntaxResult<U> Other)
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
static inline SyntaxResult<T> MakeSyntaxResult(T *Result) {
  return SyntaxResult<T>(Result);
}

/// Create a result (null or non-null) with error bit set.
template <typename T>
static inline SyntaxResult<T> MakeSyntaxErrorResult(T *Result = nullptr) {
  SyntaxResult<T> PR;
  if (Result) {
    PR = SyntaxResult<T>(Result);
  }
  PR.SetIsError();
  return PR;
}

/// Create a result (null or non-null) with error and code completion bits set.
template <typename T>
static inline SyntaxResult<T>
MakeSyntaxCodeCompletionResult(T *Result = nullptr) {
  SyntaxResult<T> PR;
  if (Result) {
    PR = SyntaxResult<T>(Result);
  }
  PR.SetHasCodeCompletionAndIsError();
  return PR;
}

/// Same as \c SyntaxResult, but just the status bits without the AST
/// node.
///
/// Useful when the AST node is returned by some other means (for example, in
/// a vector out parameter).
///
/// If you want to use 'bool' as a result type in the Syntax, consider using
/// SyntaxStatus instead.
class SyntaxStatus {
  unsigned isError : 1;
  unsigned IsCodeCompletion : 1;

public:
  /// Construct a successful parser status.
  SyntaxStatus() : isError(0), IsCodeCompletion(0) {}

  /// Construct a parser status with specified bits.
  template <typename T>
  SyntaxStatus(SyntaxResult<T> Result) : isError(0), IsCodeCompletion(0) {
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

  SyntaxStatus &operator|=(SyntaxStatus RHS) {
    isError |= RHS.isError;
    IsCodeCompletion |= RHS.IsCodeCompletion;
    return *this;
  }

  friend SyntaxStatus operator|(SyntaxStatus LHS, SyntaxStatus RHS) {
    SyntaxStatus Result = LHS;
    Result |= RHS;
    return Result;
  }
};

/// Create a successful parser status.
static inline SyntaxStatus MakeSyntaxSuccess() { return SyntaxStatus(); }

/// Create a status with error bit set.
static inline SyntaxStatus MakeSyntaxError() {
  SyntaxStatus Status;
  Status.SetIsError();
  return Status;
}

/// Create a status with error and code completion bits set.
static inline SyntaxStatus MakeSyntaxCodeCompletionStatus() {
  SyntaxStatus Status;
  Status.SetHasCodeCompletionAndIsError();
  return Status;
}

/// Create a parser result with specified bits.
template <typename T>
static inline SyntaxResult<T> MakeSyntaxResult(SyntaxStatus Status, T *Result) {
  SyntaxResult<T> PR = Status.IsError() ? MakeSyntaxErrorResult(Result)
                                        : MakeSyntaxResult(Result);

  if (Status.HasCodeCompletion()) {
    PR.SetHasCodeCompletion();
  }
  return PR;
}

template <typename T> SyntaxResult<T>::SyntaxResult(SyntaxStatus Status) {
  assert(Status.IsError());
  SetIsError();
  if (Status.HasCodeCompletion()) {
    SetHasCodeCompletion();
  }
}
} // namespace syn
} // namespace stone

#endif
