#ifndef STONE_BASIC_DIAGNOSTICTRANSACTION_H
#define STONE_BASIC_DIAGNOSTICTRANSACTION_H

namespace stone {

class DiagnosticTransaction {
  // protected:
  //   DiagnosticEngine &de;

  //   /// How many tentative diagnostics there were when the transaction
  //   /// was opened.
  //   unsigned numOfPrevPendingDiagnostics;

  //   /// How many other transactions were open when this transaction was
  //   /// opened.
  //   unsigned depth;

  //   /// Whether this transaction is currently open.
  //   bool isOpen = true;

  // public:
  //   DiagnosticTransaction(const DiagnosticTransaction &) = delete;
  //   DiagnosticTransaction &operator=(const DiagnosticTransaction &) = delete;

  //   explicit DiagnosticTransaction(DiagnosticEngine &de)
  //       : de(de), numOfPrevPendingDiagnostics(de.pendingDiagnostics.size()),
  //         depth(de.numOfTransactions), isOpen(true) {
  //     de.numOfTransactions++;
  //   }

  //   ~DiagnosticTransaction() {
  //     if (isOpen) {
  //       Commit();
  //     }

  //     if (depth == 0) {
  //       // TODO;
  //       // de.TransactionStrings.clear();
  //       // de.TransactionAllocator.Reset();
  //     }
  //   }

  //   bool HasErrors() const {
  //     llvm::ArrayRef<Diagnostic> diagnostics(de.pendingDiagnostics.begin() +
  //                                                numOfPrevPendingDiagnostics,
  //                                            de.pendingDiagnostics.end());

  //     for (auto &diagnostic : diagnostics) {
  //       switch (de.state.ComputeLevel(diagnostic)) {
  //       case diag::Level::Fatal:
  //       case diag::Level::Error:
  //         return true;
  //       default:
  //         return false;
  //       }
  //       return false;
  //     }

  //     /// Abort and close this transaction and erase all diagnostics
  //     /// record while it was open.
  //     void Abort() {
  //       Close();
  //       de.pendingDiagnostics.erase(de.pendingDiagnostics.begin() +
  //                                       numOfPrevPendingDiagnostics,
  //                                   de.pendingDiagnostics.end());
  //     }

  //     /// Commit and close this transaction. If this is the top-level
  //     /// transaction, emit any diagnostics that were recorded while it was
  //     open. void Commit() {
  //       Close();
  //       if (depth == 0) {
  //         assert(numOfPrevPendingDiagnostics == 0);
  //         de.EmitPendingDiagnostics();
  //       }
  //     }

  //   private:
  //     void Close() {
  //       assert(isOpen && "only open transactions may be closed");
  //       isOpen = false;
  //       de.numOfTransactions--;
  //       assert(depth == de.numOfTransactions &&
  //              "Transactions must be closed LIFO");
  //     }
};

} // namespace stone
#endif