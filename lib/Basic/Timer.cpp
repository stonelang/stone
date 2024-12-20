#include "stone/Basic/Timer.h"

using namespace stone;

Timer::Timer(llvm::StringRef name, llvm::StringRef desc)
    : name(name), desc(desc), timer(new llvm::Timer(name, desc)) {}

void Timer::Start() { GetTimer().startTimer(); }
void Timer::Stop() { GetTimer().stopTimer(); }
void Timer::Reset() {}

void Timer::PrintMicroSeconds() {
  // auto duration =
  //     std::chrono::duration_cast<std::chrono::microseconds>(endTime -
  //     startTime)
  //         .count();
  // llvm::outs() << "duration (microseconds): " << duration << '\n';
}

void Timer::PrintMilliSeconds() {
  // auto duration =
  //     std::chrono::duration_cast<std::chrono::milliseconds>(endTime -
  //     startTime)
  //         .count();
  // llvm::outs() << "duration (milliseconds): " << duration << '\n';
}

void Timer::PrintSeconds() {
  // auto duration =
  //     std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime)
  //         .count();
  // llvm::outs() << "duration (seconds): " << duration << '\n';
}

void Timer::PrintMinutes() {
  // auto duration =
  //     std::chrono::duration_cast<std::chrono::minutes>(endTime - startTime)
  //         .count();
  // llvm::outs() << "duration (minutes): " << duration << '\n';
}

void Timer::Print() {
  switch (timeKind) {
  case TimeKind::Microseconds:
    PrintMicroSeconds();
    break;
  case TimeKind::Milliseconds:
    PrintMilliSeconds();
    break;
  case TimeKind::Seconds:
    PrintSeconds();
    break;
  case TimeKind::Minutes:
    PrintMinutes();
    break;
  default:
    break;
  }
}