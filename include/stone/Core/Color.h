#ifndef STONE_CORE_COLOR_H
#define STONE_CORE_COLOR_H

#include "llvm/Support/raw_ostream.h"

namespace stone {

/// RAII class for setting a color for a raw_ostream and resetting when it goes
/// out-of-scope.
class ColorfulStream {
  llvm::raw_ostream &os;
  bool useColor;

public:
  ColorfulStream(llvm::raw_ostream &os) : os(os) {
    useColor = os.has_colors();
    if (useColor)
      os.changeColor(llvm::raw_ostream::Colors::WHITE);
  }
  ~ColorfulStream() {
    if (useColor)
      os.resetColor();
  }
  llvm::raw_ostream &GetOS() { return os; }
  void Reset() { os.resetColor(); }
  void UseBlack() { os.changeColor(llvm::raw_ostream::Colors::BLACK); }
  void UseGreen() { os.changeColor(llvm::raw_ostream::Colors::GREEN); }
  void UseYellow() { os.changeColor(llvm::raw_ostream::Colors::YELLOW); }
  void UseBlue() { os.changeColor(llvm::raw_ostream::Colors::BLUE); }
  void UseMagenta() { os.changeColor(llvm::raw_ostream::Colors::MAGENTA); }
  void UseCyan() { os.changeColor(llvm::raw_ostream::Colors::CYAN); }
  void UseWhite() { os.changeColor(llvm::raw_ostream::Colors::WHITE); }

  ColorfulStream &operator<<(char *str) {
    os << str;
    return *this;
  }
  ColorfulStream &operator<<(char ch) {
    os << ch;
    return *this;
  }
  ColorfulStream &operator<<(llvm::StringRef str) {
    os << str;
    return *this;
  }
};

} // namespace stone

#endif
