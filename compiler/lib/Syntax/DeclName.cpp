#include "stone/Syntax/DeclName.h"

using namespace stone;
using namespace stone::syn;

constexpr const Identifier::Aligner DeclNameBase::BasicIdentifierAligner{};
constexpr const Identifier::Aligner
    DeclNameBase::ConstructorIdentifierAligner{};
constexpr const Identifier::Aligner DeclNameBase::DestructorIdentifierAligner{};
constexpr const Identifier::Aligner DeclNameBase::OperatorIdentifierAligner{};

int DeclName::Compare(DeclName other) { return 0; }

void DeclName::Print(ColorfulStream &os, const PrintingPolicy *policy) const {}

void DeclName::Dump() const {}
