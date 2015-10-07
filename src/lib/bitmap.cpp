#include "bitmap.hpp"

#include "bitops.hpp"

using namespace ray;

Bitmap::Bitmap(unsigned h, unsigned w, Color background) :
  _height(h), _width(w) {

  _image = std::unique_ptr<Color []>(new Color[pixel_count()]);

  std::fill(&_image[0], &_image[pixel_count()], background);
}

void Bitmap::write(std::ostream &out) {
  auto write_4byte = [&](uint32_t val) {
    uint8_t encoded[4];
    Bitops::encode_le(val, encoded);
    out.write((char *)(&encoded[0]), 4);
  };

  // file header

  // bitmap signature
  out.write("BM", 2);

  uint32_t file_size = 54 + pixel_count() * 3;
  write_4byte(file_size);

  uint32_t reserved_0 = 0;
  write_4byte(reserved_0);

  uint32_t offset_of_pixel_data = 54;
  write_4byte(offset_of_pixel_data);

  // bitmap header

  uint32_t header_size = 40;
  write_4byte(header_size);

  write_4byte(width());
  write_4byte(height());

  uint8_t reserved_1[2] = { 1, 0 };
  out.write((char *)reserved_1, 2);

  uint8_t bits_per_pixel[2] = { 24, 0 };
  out.write((char *)bits_per_pixel, 2);

  uint32_t compression_method = 0;
  write_4byte(compression_method);

  uint32_t size_of_pixel_data = pixel_count() * 3;
  write_4byte(size_of_pixel_data);

  uint32_t horizontal_res = 2835;
  write_4byte(horizontal_res);

  uint32_t vertical_res = 2835;
  write_4byte(vertical_res);

  uint32_t num_color_pallette = 0;
  write_4byte(num_color_pallette);

  uint32_t most_important_color = 0;
  write_4byte(most_important_color);

  // pixel data
  for (unsigned i = pixel_count(), e = 0; i != e; --i) {
    Color &c = _image[i - 1];
    uint8_t pixel_data[3] = {c.blue(), c.green(), c.red()};
    out.write((char *)pixel_data, 3);
  }
}
