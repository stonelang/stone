#ifndef STONE_BASIC_STREAMING_H
#define STONE_BASIC_STREAMING_H

#include "llvm/Support/raw_ostream.h"

namespace stone {

/// RAII class for setting a color for a raw_ostream and resetting when it goes
/// out-of-scope.
class OSColor {
  llvm::raw_ostream &OS;
  bool HasColors;

public:
  OSColor(llvm::raw_ostream &OS, llvm::raw_ostream::Colors Color) : OS(OS) {
    HasColors = OS.has_colors();
    if (HasColors)
      OS.changeColor(Color);
  }
  ~OSColor() {
    if (HasColors)
      OS.resetColor();
  }

  OSColor &operator<<(char C) {
    OS << C;
    return *this;
  }
  OSColor &operator<<(llvm::StringRef Str) {
    OS << Str;
    return *this;
  }
};

/// A stream which forces color output.
class ColorfulStream final : public llvm::raw_ostream {
  raw_ostream &underlying;

public:
  explicit ColorfulStream(raw_ostream &underlying) : underlying(underlying) {}
  ~ColorfulStream() override { flush(); }

  raw_ostream &changeColor(Colors color, bool bold = false,
                           bool bg = false) override {
    underlying.changeColor(color, bold, bg);
    return *this;
  }
  raw_ostream &resetColor() override {
    underlying.resetColor();
    return *this;
  }
  raw_ostream &reverseColor() override {
    underlying.reverseColor();
    return *this;
  }
  bool has_colors() const override { return true; }

  void write_impl(const char *ptr, size_t size) override {
    underlying.write(ptr, size);
  }
  uint64_t current_pos() const override {
    return underlying.tell() - GetNumBytesInBuffer();
  }

  size_t preferred_buffer_size() const override { return 0; }

public:
  void UseBlack() { underlying.changeColor(llvm::raw_ostream::Colors::BLACK); }
  void UseGreen() { underlying.changeColor(llvm::raw_ostream::Colors::GREEN); }
  void UseYellow() {
    underlying.changeColor(llvm::raw_ostream::Colors::YELLOW);
  }
  void UseBlue() { underlying.changeColor(llvm::raw_ostream::Colors::BLUE); }
  void UseMagenta() {
    underlying.changeColor(llvm::raw_ostream::Colors::MAGENTA);
  }
  void UseCyan() { underlying.changeColor(llvm::raw_ostream::Colors::CYAN); }
  void UseWhite() { underlying.changeColor(llvm::raw_ostream::Colors::WHITE); }
};

/// A stream which drops all color settings.
class ColorlessStream final : public llvm::raw_ostream {
  raw_ostream &underlying;

public:
  explicit ColorlessStream(raw_ostream &underlying) : underlying(underlying) {}
  ~ColorlessStream() override { flush(); }

  bool has_colors() const override { return false; }

  void write_impl(const char *ptr, size_t size) override {
    underlying.write(ptr, size);
  }
  uint64_t current_pos() const override {
    return underlying.tell() - GetNumBytesInBuffer();
  }

  size_t preferred_buffer_size() const override { return 0; }
};

} // namespace stone

#endif
