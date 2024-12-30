#ifndef STONE_PARSE_PARSING_TYPE_H
#define STONE_PARSE_PARSING_TYPE_H

#include "stone/AST/QualType.h"
#include "stone/Parse/Parser.h"
#include "stone/Parse/ParserAllocation.h"

#include "llvm/Support/Timer.h"

#include <memory>

namespace stone {
class Parser;

// enum class TypeOperatorKind : uint8_t {
//   None = 0,
//   New,
//   Delete,
// };
// class TypeOperator {
//   SrcLoc loc;
//   TypeOperatorKind kind;

// public:
//   TypeOperator(TypeOperatorKind kind, SrcLoc inputLoc)
//       : kind(kind), loc(inputLoc) {}

// public:
//   TypeOperatorKind GetKind() const { return kind; }
//   SrcLoc GetLoc() { return loc; }
// };

// class ParsingTypeOperator final {
// public:

//   /// If this Declarator declares a template, its template parameter lists.
//   // llvm::ArrayRef<TemplateParameterList *> templateParameterLists;
// public:
//   TypeOperatorCollector();

// private:
//   void NoteOperator(const TypeOperator typeOperator) {
//     typeOperators.push_back(typeOperator);
//   }

// public:
//   void NoteNew(SrcLoc loc);
//   void NoteDelete(SrcLoc loc);
// };

// enum class TypeNullabilityKind : uint8 {
//   /// Values of this type can never be null.
//   NotNullable = 0,
//   /// Values of this type can be null.
//   Nullable,
//   /// Whether values of this type can be null is (explicitly)
//   /// unspecified. This captures a (fairly rare) case where we
//   /// can't conclude anything about the nullability of the type even
//   /// though it has been considered.
//   Unspecified
// };

enum class ParsingTypeChunkKind : uint8_t {
  Value = 0,
  Pointer,
  Reference,
  Array,
  Funciton,
  MemberPointer,
  Paren,
  Pipe,
};
class ParsingTypeChunk {
  SrcLoc loc;
  ParsingTypeChunkKind kind;

public:
  ParsingTypeChunk(ParsingTypeChunkKind kind, SrcLoc loc)
      : kind(kind), loc(loc) {}

public:
  ParsingTypeChunkKind GetKind() const { return kind; }
  SrcLoc GetLoc() { return loc; }
};

// class ParsingValueTypeChunk final : public ParsingTypeChunk {
// public:
//   ParsingValueTypeChunk()
//       : ParsingTypeChunk(ParsingTypeChunkKind::Value, SrcLoc()) {}
// };

// class ParsingPointerTypeChunk final : public ParsingTypeChunk {
// public:
//   PointerTypeChunk(SrcLoc loc)
//       : ParsingTypeChunk(ParsingTypeChunkKind::Pointer, loc) {}
// };

// class ParsingMemberPointerTypeChunk final : public ParsingTypeChunk {
// public:
//   ParsingMemberPointerTypeChunk(SrcLoc loc)
//       : ParsingTypeChunk(TypeChunkKind::MemberPointer, loc) {}
// };

// class ParsingReferenceTypeChunk final : public ParsingTypeChunk {
// public:
//   ParsingReferenceTypeChunk(SrcLoc loc)
//       : ParsingTypeChunk(ParsingTypeChunkKind::Reference, loc) {}
// };

// class ArrayTypeChunk final : public ParsingTypeChunk {
// public:
//   ArrayTypeChunk(SrcLoc loc)
//       : ParsingTypeChunk(ParsingTypeChunkKind::Array, loc) {}
// };

// class ParsingParenTypeChunk final : public ParsingTypeChunk {
// public:
//   ParenTypeChunk(SrcLoc loc)
//       : ParsingTypeChunk(ParsingTypeChunkKind::Paren, loc) {}
// };

// class ParsingFunctionTypeChunk final : public ParsingTypeChunk {
// public:
//   ParsingFunctionTypeChunk()
//       : ParsingTypeChunk(ParsingTypeChunkKind::Funciton, SrcLoc()) {}
// };

enum class ParsingTypeSpecKind : uint8_t {
  None = 0,
  Builtin,
  Function,
  Struct,
  Enum,
  Class,
  Identifier,
  Last = Identifier,
};

class ParsingTypeSpec : public ParserAllocation<ParsingTypeSpec> {
  friend Parser;

  QualType ty;
  SrcLoc loc;
  ParsingTypeSpecKind kind;
  llvm::SmallVector<ParsingTypeChunk, 8> parsingTypeChunks;

protected:
  ParsingTypeSpec(ParsingTypeSpecKind kind, SrcLoc loc = SrcLoc())
      : kind(kind), loc(loc) {}

public:
  /// Determine if the primary thype is null
  bool HasType() const;

  /// Set the primary type
  void SetType(QualType ty);

  /// Return the primary type
  QualType GetType() { return ty; }

  /// Return the primary type location
  SrcLoc GetLoc() const { return loc; }

  ParsingTypeSpecKind GetKind() const { return kind; }

  //\return true if the type is FunType
  bool IsBuiltin() const { return GetKind() == ParsingTypeSpecKind::Builtin; }

  //\return true if the type is FunType
  bool IsFunction() const { return GetKind() == ParsingTypeSpecKind::Function; }

  //\return true if the type is enum type
  bool IsEnum() const { return GetKind() == ParsingTypeSpecKind::Enum; }

  //\return true if the type is struct type
  bool IsStruct() const { return GetKind() == ParsingTypeSpecKind::Struct; }

  //\return true if the type is class type
  bool IsClass() const { return GetKind() == ParsingTypeSpecKind::Class; }

  //\return true if the type is an identifier type
  bool IsIdentifier() const {
    return GetKind() == ParsingTypeSpecKind::Identifier;
  }

public:
  const ParsingTypeChunk *GetInnermostParsingTypeChunk(
      ParsingTypeChunkKind exclude = ParsingTypeChunkKind::Paren) const {
    for (unsigned i = parsingTypeChunks.size(), i_end = 0; i != i_end; --i) {
      if (parsingTypeChunks[i - 1].GetKind() != exclude) {
        return &parsingTypeChunks[i - 1];
      }
    }
    return nullptr;
  }

  /// int** -- the '*' farthest from int
  const ParsingTypeChunk *GetOutermostParsingTypeChunk(
      ParsingTypeChunkKind exclude = ParsingTypeChunkKind::Paren) const {
    for (unsigned i = 0, i_end = parsingTypeChunks.size(); i < i_end; ++i) {
      if (parsingTypeChunks[i].GetKind() != exclude) {
        return &parsingTypeChunks[i];
      }
    }
    return nullptr;
  }

  void AddParsingTypeChunk(ParsingTypeChunk chunk);

public:
  static bool classof(const ParsingTypeSpec *spec) {
    return spec->GetKind() >= ParsingTypeSpecKind::Builtin &&
           spec->GetKind() <= ParsingTypeSpecKind::Last;
  }
};

class ParsingBuiltinTypeSpec : public ParsingTypeSpec {
  TypeKind kind;

  bool IsTypeKind(TypeKind k) const { return (kind == k); }

public:
  ParsingBuiltinTypeSpec(TypeKind kind, SrcLoc loc);
  TypeKind GetTypeKind() { return kind; }

public:
  ///\return true if the type is int8
  bool IsInt8Type() const;

  ///\return true if the type is int16
  bool IsInt16Type() const;

  ///\return true if the type is int32
  bool IsInt32Type() const;

  ///\return true if the type is int64
  bool IsInt64Type() const;
  ///\return true if the type is int128
  bool IsInt128Type() const;

  ///\return true if the type is uint8
  bool IsUIntType() const;
  ///\return true if the type is uint16
  bool Isuint8Type() const;
  ///\return true if the type is uint16
  bool IsUInt16Type() const;
  ///\return true if the type is uint32
  bool IsUInt32Type() const;
  ///\return true if the type is uint64
  bool IsUInt64Type() const;
  ///\return true if the type is uint128
  bool IsUInt128Type() const;

  ///\return true if the type is bool
  bool IsBoolType() const;

  ///\return true if the type is char
  bool IsCharType() const;
  ///\return true if the type is char8
  bool IsChar8Type() const;
  ///\return true if the type is char16
  bool IsChar16Type() const;
  ///\return true if the type is char32
  bool IsChar32Type() const;

  ///\return true if the type is real
  bool IsRealType() const;

  ///\return true if the type is float
  bool IsFloatType() const;
  ///\return true if the type is float16
  bool IsFloat16Type() const;
  ///\return true if the type is float32
  bool IsFloat32Type() const;
  ///\return true if the type is float64
  bool IsFloat64Type() const;
  ///\return true if the type is float128
  bool IsFloat128Type() const;

  ///\return true if the type is complex32
  bool IsComplex32Type() const;
  ///\return true if the type is complex64
  bool IsComplex64Type() const;

  ///\return true if the type is imaginary32
  bool IsImaginary32Type() const;
  ///\return true if the type is imaginary32
  bool IsImaginary64Type() const;
  ///\return true if the type is null
  bool IsNullType() const;
  ///\return true if the type is auto
  bool IsAutoType() const;
  ///\return true if the type is void
  bool IsVoidType() const;

public:
  static bool IsBuiltinType(TypeKind k);

public:
  static bool classof(const ParsingTypeSpec *spec) {
    return spec->GetKind() == ParsingTypeSpecKind::Builtin;
  }
};

class ParsingFunTypeSpec : public ParsingTypeSpec {

  SrcLoc inlineLoc;
  SrcLoc forcedInlineLoc;
  SrcLoc arrowLoc;
  SrcLoc doubleColonLoc;
  SrcLoc lParen;
  SrcLoc rParen;

  ParsingTypeSpec *resultType = nullptr;
  BraceStmt *bodyStmt = nullptr;

public:
  ParsingFunTypeSpec(SrcLoc loc)
      : ParsingTypeSpec(ParsingTypeSpecKind::Function, loc) {}

public:
  void SetResultType(ParsingTypeSpec *resultTy) { resultType = resultType; }
  ParsingTypeSpec *GetResultType() { return resultType; }

  void SetBody(BraceStmt *BS) { bodyStmt = BS; }
  BraceStmt *GetBody() { return bodyStmt; }

  void SetArrow(SrcLoc loc) { arrowLoc = loc; }
  SrcLoc GetArrow() { return arrowLoc; }
  bool HasArrow() { return GetArrow().isValid(); }

  void SetLParen(SrcLoc loc) { lParen = loc; }
  SrcLoc GetLParen() { return lParen; }
  bool HasLParen() { return GetLParen().isValid(); }

  void SetRParen(SrcLoc loc) { rParen = loc; }
  SrcLoc GetRParen() { return rParen; }
  bool HasRParen() { return GetRParen().isValid(); }

public:
  static bool classof(const ParsingTypeSpec *spec) {
    return spec->GetKind() == ParsingTypeSpecKind::Function;
  }
};

class ParsingStructTypeSpec : public ParsingTypeSpec {
public:
  ParsingStructTypeSpec(SrcLoc loc)
      : ParsingTypeSpec(ParsingTypeSpecKind::Struct, loc) {}
};

class ParsingEnumTypeSpec : public ParsingTypeSpec {
public:
  ParsingEnumTypeSpec(SrcLoc loc)
      : ParsingTypeSpec(ParsingTypeSpecKind::Enum, loc) {}
};

class ParsingIdentifierTypeSpec : public ParsingTypeSpec {
public:
  ParsingIdentifierTypeSpec(SrcLoc loc)
      : ParsingTypeSpec(ParsingTypeSpecKind::Identifier, loc) {}
};
} // namespace stone

#endif