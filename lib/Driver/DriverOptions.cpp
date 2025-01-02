#include "stone/Driver/Driver.h"
#include "stone/Driver/Step.h"

using namespace stone;

llvm::ArrayRef<StepKind> DriverOptions::GetStepKindList(DriverActionKind kind) {

  switch (kind) {
  case DriverActionKind::Parse:
  case DriverActionKind::TypeCheck: {
    return {StepKind::Compile};
  }
  }
}
