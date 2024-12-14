#ifndef STONE_BASIC_COLOR_H
#define STONE_BASIC_COLOR_H

#include "llvm/Support/raw_ostream.h"

namespace stone {

/// RAII class for setting a color for a raw_ostream and resetting when it goes
/// out-of-scope.
class ColorStream final {
  bool hasColors;
  llvm::raw_ostream &os;

public:
  ColorStream() : ColorStream(llvm::outs()) {}

  ColorStream(llvm::raw_ostream &os) : os(os) {
    hasColors = os.has_colors();
    if (hasColors) {
      os.changeColor(llvm::raw_ostream::Colors::WHITE);
    }
  }
  ~ColorStream() {
    if (hasColors)
      os.resetColor();
  }

public:
  llvm::raw_ostream &GetOS() { return os; }

  void Reset() { os.resetColor(); }
  void UseBlack() {
    if (hasColors) {
      os.changeColor(llvm::raw_ostream::Colors::BLACK);
    }
  }
  void UseGreen() {
    if (hasColors) {
      os.changeColor(llvm::raw_ostream::Colors::GREEN);
    }
  }

  void UseYellow() {
    if (hasColors) {
      os.changeColor(llvm::raw_ostream::Colors::YELLOW);
    }
  }
  void UseBlue() {
    if (hasColors) {
      os.changeColor(llvm::raw_ostream::Colors::BLUE);
    }
  }
  void UseMagenta() {
    if (hasColors) {
      os.changeColor(llvm::raw_ostream::Colors::MAGENTA);
    }
  }
  void UseCyan() {
    if (hasColors) {
      os.changeColor(llvm::raw_ostream::Colors::CYAN);
    }
  }
  void UseWhite() {
    if (hasColors) {
      os.changeColor(llvm::raw_ostream::Colors::WHITE);
    }
  }

  ColorStream &operator<<(char *str) {
    os << str;
    return *this;
  }
  ColorStream &operator<<(char ch) {
    os << ch;
    return *this;
  }
  ColorStream &operator<<(llvm::StringRef str) {
    os << str;
    return *this;
  }
};

} // namespace stone

#endif
