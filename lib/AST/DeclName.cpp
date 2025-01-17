#include "stone/AST/DeclName.h"
#include "stone/AST/ASTContext.h"
using namespace stone;

// int DeclName::Compare(DeclName other) { return 0; }

// void DeclName::Print(ColorStream &os, const PrintingPolicy *policy) const {}

// void DeclName::Dump() const {}

DeclName DeclName::CreateConstructor(ASTContext &AC, SrcLoc loc) {
  return DeclName(DeclNameKind::Constructor,
                  DeclNameBase(AC.GetIdentifier("constructor"), loc));
}

DeclName DeclName::CreateDestructor(ASTContext &AC, SrcLoc loc) {
  return DeclName(DeclNameKind::Destructor,
                  DeclNameBase(AC.GetIdentifier("destructor"), loc));
}

DeclName DeclName::CreateOperator(ASTContext &AC, SrcLoc loc) {
  return DeclName(DeclNameKind::Operator,
                  DeclNameBase(AC.GetIdentifier("operator"), loc));
}

// static DeclName CreateCompound(ASTContext &AC, DeclNameBase base,
//                                llvm::ArrayRef<DeclNameBase> args,
//                                llvm::FoldingSetNodeID &id) {
//   // auto *compound = new CompoundDeclName(std::move(base), args.size());
//   // for (size_t i = 0; i < args.size(); ++i) {
//   //   compound->GetArgumentNames()[i] = args[i];
//   // }
//   // compound->Profile(id);
//   // return DeclName(compound);
// }