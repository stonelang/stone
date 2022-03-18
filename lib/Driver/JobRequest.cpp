#include "stone/Driver/Driver.h"

using namespace stone;

void JobRequest::Print(ColorOutputStream &stream, llvm::StringRef terminator) const {

  //   /// TODO: IntentFormatter
  //   OS() << std::to_string(GetQueueID()) << ":";
  //   OS().UseGreen();
  //   OS() << GetName();
  //   OS().Reset();
  //   OS() << "(";
  //   int inputSize = inputs.size();
  //   for (auto &input : inputs) {
  //     --inputSize;
  //     OS() << input.GetName();
  //     if (inputSize != 0) {
  //       OS() << ",";
  //     }
  //   }
  //   OS() << ")"
  //        << " -> "
  //        << "object" << '\n';
  //   OS() << '\n';
  // }
}

void TopLevelJobRequest::Print(ColorOutputStream &stream,
                               llvm::StringRef terminator) const{
  for (auto jr : *this) {
    jr->Print(stream, terminator);
  }
}

void Driver::BuildJobRequests(Compilation &compilation, HotCache &hc,
                              const file::Files &inputs) {}
