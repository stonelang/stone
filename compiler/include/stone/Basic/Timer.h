#ifndef STONE_BASIC_TIMER_H
#define STONE_BASIC_TIMER_H

#include <iostream>

#include "llvm/Support/Chrono.h"
#include "llvm/Support/Timer.h"
namespace stone {

// TODO: Replace with llvm/Support/Timer.h"
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
  // TODO:
  llvm::Timer time;
  llvm::sys::TimePoint<> startTime;
  llvm::sys::TimePoint<> endTime = llvm::sys::TimePoint<>::min();

  // timerGroup =
  //     std::make_unique<llvm::TimerGroup>(GetSessionName(), GetSessionDesc());
  // timer = std::make_unique<llvm::Timer>(GetSessionName(), GetSessionDesc(),
  //                                       *timerGroup);

public:
  void PrintSeconds();
  void PrintMinutes();
  void PrintMilliSeconds();
  void PrintMicroSeconds();

public:
  Timer(llvm::StringRef name, llvm::StringRef desc) : name(name), desc(desc) {}
  void Start();
  void Stop();
  void Print();
  void Reset();
  llvm::sys::TimePoint<> GetDuration();
};

} // namespace stone
#endif
