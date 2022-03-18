#include "stone/Driver/Driver.h"

using namespace stone;

void JobRequest::Print(ColorOutputStream &stream, llvm::StringRef terminator) {}

void TopLevelJobRequest::Print(ColorOutputStream &stream,
                               llvm::StringRef terminator) {}



void Driver::BuildJobRequests(Compilation &compilation, HotCache &hc,
                           const file::Files &inputs) {


}

