#ifndef RAY_BITMAP_HPP
#define RAY_BITMAP_HPP

#include <cassert>
#include <cstdint>
#include <memory>
#include <ostream>

namespace ray {

class Color {
  uint8_t _red = 0;
  uint8_t _green = 0;
  uint8_t _blue = 0;

public:
  Color() {}

  explicit Color(uint8_t red, uint8_t green, uint8_t blue)
      : _red(red), _green(green), _blue(blue) {}

  uint8_t red() const { return _red; }
  uint8_t green() const { return _green; }
  uint8_t blue() const { return _blue; }

  Color operator*(float f) {
    return Color(uint8_t(red() * f), uint8_t(green() * f), uint8_t(blue() * f));
  }

  static Color create_white() { return Color(255, 255, 255); }
  static Color create_black() { return Color(0, 0, 0); }

  static Color create_red() { return Color(255, 0, 0); }
  static Color create_green() { return Color(0, 255, 0); }
  static Color create_blue() { return Color(0, 0, 255); }
};

inline Color operator+(const Color &c0, const Color &c1) {
  auto clamp = [](uint16_t x) {
    if (x > 255)
      return uint8_t(255);
    return uint8_t(x);
  };

  return Color(clamp(uint16_t(c0.red()) + uint16_t(c1.red())),
               clamp(uint16_t(c0.green()) + uint16_t(c1.green())),
               clamp(uint16_t(c0.blue()) + uint16_t(c1.blue())));
}

class Bitmap {
  unsigned _height;
  unsigned _width;
  std::unique_ptr<Color[]> _image;

public:
  Bitmap(unsigned height, unsigned width, Color background);

  Color &at(unsigned x, unsigned y) {
    assert(x < width() && y < height() && "Out of bounds!");
    return _image[y * width() + x];
  }

  unsigned height() const { return _height; }
  unsigned width() const { return _width; }
  unsigned pixel_count() const { return height() * width(); }

  void write(std::ostream &out);
};
}

#endif
