#ifndef STONE_UTILS_TABLEGEN_TABLEGENBACKEND_H
#define STONE_UTILS_TABLEGEN_TABLEGENBACKEND_H

#include <string>

namespace llvm {
class raw_ostream;
class RecordKeeper;
} // namespace llvm

namespace stone {

void EmitDiagnosticDefs(llvm::RecordKeeper &Records, llvm::raw_ostream &OS,
                        const std::string &Component);
void EmitDiagnosticGroups(llvm::RecordKeeper &Records, llvm::raw_ostream &OS);

void EmitDiagnosticIndexName(llvm::RecordKeeper &Records,
                             llvm::raw_ostream &OS);
void EmitDiagnosticDocs(llvm::RecordKeeper &Records, llvm::raw_ostream &OS);

} // namespace stone

#endif