#ifndef STONE_BASIC_TIMER_H
#define STONE_BASIC_TIMER_H

#include <iostream>

#include "llvm/Support/Chrono.h"
#include "llvm/Support/Timer.h"

namespace stone {

class Timer final {
public:
  enum class TimeKind {
    Nanoseconds,
    Microseconds,
    Milliseconds,
    Seconds,
    Minutes,
    Hours,
  };
  TimeKind timeKind = TimeKind::Milliseconds;

private:
  llvm::StringRef name;
  llvm::StringRef desc;
  std::unique_ptr<llvm::Timer> timer;

private:
  llvm::Timer &GetTimer() { return *timer; }

public:
  Timer(llvm::StringRef name, llvm::StringRef desc);

public:
  void PrintSeconds();
  void PrintMinutes();
  void PrintMilliSeconds();
  void PrintMicroSeconds();

public:
  void Start();
  void Stop();
  void Print();
  void Reset();
  llvm::sys::TimePoint<> GetDuration();
};

} // namespace stone
#endif